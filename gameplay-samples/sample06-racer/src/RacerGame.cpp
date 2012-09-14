#include "RacerGame.h"

// Render queue indexes (in order of drawing).
enum RenderQueue
{
    QUEUE_OPAQUE = 0,
    QUEUE_TRANSPARENT,
    QUEUE_COUNT
};

bool __viewFrusumCulling = true;
bool __flythruCamera = false;
bool __drawDebug = false;

// Declare our game instance
RacerGame game;

// Input bit-flags (powers of 2)
#define ACCELERATOR (1)
#define BRAKE (2)
#define REVERSE (4)
#define UPRIGHT (8)

#define BUTTON_A (_gamepad->isVirtual() ? 0 : 10)
#define BUTTON_B (_gamepad->isVirtual() ? 1 : 11)
#define BUTTON_X (12)
#define BUTTON_Y (13)

RacerGame::RacerGame()
    : _scene(NULL), _keyFlags(0), _gamepad(NULL), _carVehicle(NULL)
{
}

void RacerGame::initialize()
{
    setVsync(false);

    setMultiTouch(true);

    _font = Font::create("res/common/arial40.gpb");

    // Display the gameplay splash screen during loading, for at least 1 second.
    displayScreen(this, &RacerGame::drawSplash, NULL, 1000L);

    // Load the scene
    _scene = Scene::load("res/common/game.scene");

    // Set the aspect ratio for the scene's camera to match the current resolution
    _scene->getActiveCamera()->setAspectRatio((float)getWidth() / (float)getHeight());

    // Initialize scene
    _scene->visit(this, &RacerGame::initializeScene);

    // Initialize the gamepad
    _gamepad = getGamepad(0);

    // Load and initialize game script
    getScriptController()->loadScript("res/common/game.lua");
    getScriptController()->executeFunction<void>("setScene", "<Scene>", _scene);

    /*Animation* animation = _scene->findNode("camera1")->getAnimation();
    if (animation)
    {
        animation->getClip()->setSpeed(10);
        animation->play();
    }*/
    _virtualGamepad = getGamepad(0);
    Form* gamepadForm = _virtualGamepad->getForm();

    float from = 0.0f;
    float to = getHeight();
    Animation* virtualGamepadAnimation = gamepadForm->createAnimationFromTo("gamepad_transition", Form::ANIMATE_POSITION_Y, &from, &to, Curve::LINEAR, 600L);
    _virtualGamepadClip = virtualGamepadAnimation->getClip();

    Node* carNode = _scene->findNode("carbody");
    if (carNode && carNode->getCollisionObject()->getType() == PhysicsCollisionObject::VEHICLE)
    {
        _carVehicle = static_cast<PhysicsVehicle*>(carNode->getCollisionObject());
        resetVehicle();
    }

    // Create audio tracks
    _carSound = AudioSource::create("res/common/engine_loop.ogg");
    if (_carSound)
    {
        _carSound->setLooped(true);
        _carSound->play();
        _carSound->setGain(0.5f);
    }
//    _backgroundSound = AudioSource::create("res/common/background_track.ogg");
//    if (_backgroundSound)
//    {
//        _backgroundSound->setLooped(true);
//        _backgroundSound->play();
//        _backgroundSound->setGain(0.05f);
//    }
}

bool RacerGame::initializeScene(Node* node)
{
    static Node* lightNode = _scene->findNode("directionalLight1");

    Model* model = node->getModel();
    if (model)
    {
        Material* material = model->getMaterial();
        if (material && material->getTechnique()->getPassByIndex(0)->getEffect()->getUniform("u_lightDirection"))
        {
            material->getParameter("u_ambientColor")->setValue(_scene->getAmbientColor());
            material->getParameter("u_lightColor")->setValue(lightNode->getLight()->getColor());
            material->getParameter("u_lightDirection")->setValue(lightNode->getForwardVectorView());
        }
    }

    return true;
}

void RacerGame::finalize()
{
//    SAFE_RELEASE(_backgroundSound);
    SAFE_RELEASE(_carSound);
    SAFE_RELEASE(_scene);
    SAFE_RELEASE(_font);
}

void RacerGame::update(float elapsedTime)
{
    // Check if we have any physical gamepad connections.
    getGamepadsConnected();
    
    _gamepad->update(elapsedTime);

    Node* cameraNode;
    if (_scene->getActiveCamera() && (cameraNode = _scene->getActiveCamera()->getNode()))
    {
        float dt = elapsedTime / 1000.0f;
        float steering = 0;
        float braking = 0;
        float driving = 0;

        if (!__flythruCamera && _carVehicle)
        {
            //
            // Vehicle Control (Normal Mode)
            //
            Vector2 direction;
            if (_gamepad->isJoystickActive(0))
            {
                _gamepad->getJoystickAxisValues(0, &direction);
                steering = -direction.x;
            }

            if (_keyFlags & ACCELERATOR || _gamepad->getButtonState(BUTTON_A) == Gamepad::BUTTON_PRESSED)
            {
                driving = 1;
                _carSound->setGain(1.0f);
            }
            else
            {
                _carSound->setGain(0.5f);
            }
            float s = (int)_carVehicle->getSpeedKph() / 100.0f;
            _carSound->setPitch(max(0.2f, min(s, 2.0f)));

            // Reverse only below a reasonable speed
            bool isReverseCommanded = _keyFlags & REVERSE
                    || !_gamepad->isVirtual() && _gamepad->getButtonState(BUTTON_X) == Gamepad::BUTTON_PRESSED
                    || direction.y < -0.9;
            if (isReverseCommanded && _carVehicle->getSpeedKph() < 30.0f)
            {
                driving = -0.6f;
            }

            if (_keyFlags & BRAKE || _gamepad->getButtonState(BUTTON_B) == Gamepad::BUTTON_PRESSED)
            {
                braking = 1;
            }

            //
            // Make the camera follow the car
            //
            Node* carNode = _carVehicle->getNode();
            Vector3 carPosition(carNode->getTranslation());
            Vector3 fixedArm(Vector3::unitY()*4.0f - carNode->getBackVector()*10.0f);
            Vector3 swingArm(carPosition, _carPositionPrevious);
            swingArm.y = max(0.0f, swingArm.y);
            swingArm += fixedArm*0.0001f;
            swingArm.normalize();
            Vector3 commandedPosition(carPosition + fixedArm + swingArm*5.0f);
            cameraNode->translate((commandedPosition - cameraNode->getTranslation()) * (dt / (dt + 0.2)));
            Matrix m;
            Matrix::createLookAt(cameraNode->getTranslation(), carPosition, Vector3::unitY(), &m);
            m.transpose();
            Quaternion q;
            m.getRotation(&q);
            cameraNode->setRotation(q);
            _carPositionPrevious.set(carPosition);
        }
        else
        {
            //
            // Flythru Control (Dev Mode)
            //
            float speed = 0;
            if (_keyFlags & ACCELERATOR || _gamepad->getButtonState(BUTTON_A) == Gamepad::BUTTON_PRESSED)
            {
                speed = 60.0f;
            }
            else if (_keyFlags & BRAKE || _gamepad->getButtonState(BUTTON_B) == Gamepad::BUTTON_PRESSED)
            {
                speed = -60.0f;
            }
            cameraNode->translate(cameraNode->getForwardVector() * speed * dt);

            if (_gamepad->isJoystickActive(0))
            {
                Vector2 direction;
                _gamepad->getJoystickAxisValues(0, &direction);

                speed = 0.8f;

                // Yaw in world frame, pitch in body frame
                Matrix m;
                cameraNode->getWorldMatrix().transpose(&m);
                Vector3 yaw;
                m.getUpVector(&yaw);
                cameraNode->rotate(yaw, -direction.x * speed * dt);
                cameraNode->rotateX(direction.y * speed * dt);
            }

            _carPositionPrevious.set(0, 0, 0);
        }

        if (_carVehicle)
        {
            float blowdown = max(0.1f, 1 - 0.009f*fabs(_carVehicle->getSpeedKph()));
            _carVehicle->update(blowdown*steering, braking, driving);

            if (_keyFlags & UPRIGHT
                    || !_gamepad->isVirtual() && _gamepad->getButtonState(BUTTON_Y) == Gamepad::BUTTON_PRESSED)
            {
                resetVehicle();
            }
        }
    }
}

void RacerGame::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

    // Visit all the nodes in the scene to build our render queues
    for (unsigned int i = 0; i < QUEUE_COUNT; ++i)
        _renderQueues[i].clear();
    _scene->visit(this, &RacerGame::buildRenderQueues);

    // Draw the scene from our render queues
    drawScene();

    if (__drawDebug)
    {
        Game::getInstance()->getPhysicsController()->drawDebug(_scene->getActiveCamera()->getViewProjectionMatrix());
    }

    // Draw the gamepad
    _virtualGamepad->draw();
        
    // Draw FPS and speed
    int carSpeed = _carVehicle ? (int)_carVehicle->getSpeedKph() : 0;
    _font->start();
    char fps[32];
    sprintf(fps, "%d", getFrameRate());
    _font->drawText(fps, 5, 5, Vector4(0,0.5f,1,1), 20);
    char kph[32];
    sprintf(kph, "%d [km/h]", carSpeed);
    _font->drawText(kph, 300, 350, Vector4(1,1,1,1), 40);
    _font->finish();
}

bool RacerGame::buildRenderQueues(Node* node)
{
    Model* model = node->getModel(); 
    if (model)
    {
        // Perform view-frustum culling for this node
        if (__viewFrusumCulling && !node->getBoundingSphere().intersects(_scene->getActiveCamera()->getFrustum()))
            return true;

        // Determine which render queue to insert the node into
        std::vector<Node*>* queue;
        if (node->hasTag("transparent"))
            queue = &_renderQueues[QUEUE_TRANSPARENT];
        else
            queue = &_renderQueues[QUEUE_OPAQUE];

        queue->push_back(node);
    }
    return true;
}

void RacerGame::drawScene()
{
    // Iterate through each render queue and draw the nodes in them
    for (unsigned int i = 0; i < QUEUE_COUNT; ++i)
    {
        std::vector<Node*>& queue = _renderQueues[i];

        for (unsigned int j = 0, ncount = queue.size(); j < ncount; ++j)
        {
            queue[j]->getModel()->draw();
        }
    }
}

void RacerGame::drawSplash(void* param)
{
    clear(CLEAR_COLOR_DEPTH, Vector4(0, 0, 0, 1), 1.0f, 0);

    SpriteBatch* batch = SpriteBatch::create("res/logo_powered_white.png");
    batch->start();
    batch->draw(this->getWidth() * 0.5f, this->getHeight() * 0.5f, 0.0f, 512.0f, 512.0f, 0.0f, 1.0f, 1.0f, 0.0f, Vector4::one(), true);
    batch->finish();
    SAFE_DELETE(batch);
}

void RacerGame::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case Keyboard::KEY_ESCAPE:
            exit();
            break;
        case Keyboard::KEY_A:
        case Keyboard::KEY_CAPITAL_A:
            _keyFlags |= ACCELERATOR;
            break;
        case Keyboard::KEY_B:
        case Keyboard::KEY_CAPITAL_B:
            _keyFlags |= BRAKE;
            break;
        case Keyboard::KEY_X:
        case Keyboard::KEY_CAPITAL_X:
            _keyFlags |= REVERSE;
            break;
        case Keyboard::KEY_Y:
        case Keyboard::KEY_CAPITAL_Y:
            _keyFlags |= UPRIGHT;
            break;
        case Keyboard::KEY_V:
            __viewFrusumCulling = !__viewFrusumCulling;
            break;
        case Keyboard::KEY_F:
            __flythruCamera = !__flythruCamera;
            break;
        case Keyboard::KEY_D:
            __drawDebug = !__drawDebug;
            break;
        }
    }
    else if (evt == Keyboard::KEY_RELEASE)
    {
        switch (key)
        {
        case Keyboard::KEY_A:
        case Keyboard::KEY_CAPITAL_A:
            _keyFlags &= ~ACCELERATOR;
            break;
        case Keyboard::KEY_B:
        case Keyboard::KEY_CAPITAL_B:
            _keyFlags &= ~BRAKE;
            break;
        case Keyboard::KEY_X:
        case Keyboard::KEY_CAPITAL_X:
            _keyFlags &= ~REVERSE;
            break;
        case Keyboard::KEY_Y:
        case Keyboard::KEY_CAPITAL_Y:
            _keyFlags &= ~UPRIGHT;
            break;
        }
    }
}

void RacerGame::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}

void RacerGame::gamepadEvent(Gamepad::GamepadEvent evt, Gamepad* gamepad)
{
    switch(evt)
    {
    case Gamepad::CONNECTED_EVENT:
        if (gamepad->isVirtual())
        {
            _virtualGamepadClip->setSpeed(-1.0f);
        }
        else
        {
            _virtualGamepadClip->setSpeed(1.0f);
        }
        _virtualGamepadClip->play();
        _gamepad = gamepad;

        break;
    case Gamepad::DISCONNECTED_EVENT:
        _virtualGamepadClip->setSpeed(-1.0f);
        _virtualGamepadClip->play();
        _gamepad = getGamepad(0);
        break;
    }
}

void RacerGame::menuEvent()
{
    resetVehicle();
}

void RacerGame::resetVehicle()
{
    Node* carNode = _carVehicle->getNode();

    _carVehicle->getRigidBody()->setEnabled(false);
    carNode->setTranslation(-328.0f, 20.0f, 371.0f);
    carNode->setRotation(Vector3::unitY(), 143.15f*3.1415927f/180.0f);
    _carVehicle->getRigidBody()->setLinearVelocity(Vector3::zero());
    _carVehicle->getRigidBody()->setAngularVelocity(Vector3::zero());
    _carVehicle->getRigidBody()->setEnabled(true);
}
