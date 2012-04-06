#include "CharacterGame.h"

// Declare our game instance
CharacterGame game; 

unsigned int keyFlags = 0;
float _rotateY = 0.0f;
#define WALK_SPEED  7.5f
#define ANIM_SPEED 1.0f
#define BLEND_DURATION 150.0f

float cameraFocusDistance = 16.0f;

int drawDebug = 0;
bool moveBall = false;

CharacterGame::CharacterGame()
    : _font(NULL), _scene(NULL), _character(NULL), _animation(NULL), _animationState(0), _rotateX(0)
{
}

CharacterGame::~CharacterGame()
{
}

void CharacterGame::initialize()
{
    // Display the gameplay splash screen for at least 1 second.
    displayScreen(this, &CharacterGame::drawSplash, NULL, 1000L);

    // Load the font.
    _font = Font::create("res/arial40.gpb");

    // Load scene.
    _scene = Scene::load("res/scene.scene");

    // Store character node.
    Node* node = _scene->findNode("BoyCharacter");
    PhysicsRigidBody::Parameters p;
    p.mass = 20.0f;
    node->setTranslationY(5.0f);
    node->setCollisionObject(PhysicsCollisionObject::CHARACTER, PhysicsCollisionShape::capsule(1.2f, 5.0f, Vector3(0, 2.5, 0), true), &p);
    _character = static_cast<PhysicsCharacter*>(node->getCollisionObject());
    _character->setMaxStepHeight(0.0f);
    _character->addCollisionListener(this);

    // Store character mesh node.
    _characterMeshNode = node->findNode("BoyMesh");

    // Set a ghost object on our camera node to assist in camera occlusion adjustments
    _scene->findNode("Camera")->setCollisionObject(PhysicsCollisionObject::GHOST_OBJECT, PhysicsCollisionShape::sphere(0.5f));

    // Initialize scene.
    _scene->visit(this, &CharacterGame::initScene);

    // Load animations clips.
    loadAnimationClips(node);

    // Initialize the gamepad.
	_gamepad = new Gamepad("res/gamepad.png", 1, 1);

	Gamepad::Rect leftRegionInner = {130.0f, this->getHeight() - 130.0f, 47.0f, 47.0f};
    Gamepad::Rect leftTexRegionInner = {10.0f, 188.0f, 47.0f, 47.0f};
    Gamepad::Rect leftRegionOuter = {120.0f, this->getHeight() - 130.0f, 170.0f, 170.0f};
    Gamepad::Rect leftTexRegionOuter = {0.0f, 0.0f, 170.0f, 170.0f};
    _gamepad->setJoystick(JOYSTICK, &leftRegionInner, &leftTexRegionInner, &leftRegionOuter, &leftTexRegionOuter, 45.0f);

	Gamepad::Rect regionOnScreen = {this->getWidth() - 120.0f, this->getHeight() - 130.0f, 47.0f, 47.0f};
	Gamepad::Rect defaultRegion = {10.0f, 188.0f, 50.0f, 47.0f};
	Gamepad::Rect focusRegion = {69.0f, 188.0f, 50.0f, 47.0f};
	_gamepad->setButton(BUTTON_1, &regionOnScreen, &defaultRegion, &focusRegion);
}

void CharacterGame::initMaterial(Scene* scene, Node* node, Material* material)
{
    // Bind light shader parameters to dynamic objects only
    std::string id = node->getId();
    if (material)// &&
        //(id == "Basketball" || id.find("GreenChair") != id.npos || id.find("BlueChair") != id.npos || 
        //id == "Easel" || id == "BoyMesh" || id == "BoyShadow" || id == "Rainbow"))
    {
        Node* lightNode = scene->findNode("SunLight");

        material->getParameter("u_lightDirection")->bindValue(lightNode, &Node::getForwardVectorView);
        material->getParameter("u_lightColor")->bindValue(lightNode->getLight(), &Light::getColor);
        material->getParameter("u_ambientColor")->bindValue(scene, &Scene::getAmbientColor);
    }
}

bool CharacterGame::initScene(Node* node, void* cookie)
{
    Model* model = node->getModel();
    if (model)
    {
        if (model->getMaterial())
        {
            initMaterial(_scene, node, model->getMaterial());
        }
        for (unsigned int i = 0; i < model->getMeshPartCount(); ++i)
        {
            if (model->hasMaterial(i))
            {
                initMaterial(_scene, node, model->getMaterial(i));
            }
        }
    }

    return true;
}

void CharacterGame::finalize()
{
    SAFE_RELEASE(_scene);
    SAFE_RELEASE(_font);
    SAFE_DELETE(_gamepad);
}

void CharacterGame::update(long elapsedTime)
{
    Gamepad::ButtonState buttonOneState = _gamepad->getButtonState(BUTTON_1);
	Vector2 joystickVec = _gamepad->getJoystickState(JOYSTICK);
    if (!joystickVec.isZero())
    {
	    keyFlags = 0;

        // Calculate forward/backward movement.
        if (joystickVec.y > 0)
	    {
		    keyFlags |= 1;
	    }
	    else if (joystickVec.y < 0)
	    {
		    keyFlags |= 2;
	    }

        // Calculate rotation
        float angle = joystickVec.x * MATH_PI * -0.02;
        _character->rotate(Vector3::unitY(), angle);
    }

    // Update character animation and movement
    if (keyFlags == 0)
    {
        _character->play("idle", PhysicsCharacter::ANIMATION_REPEAT, 1.0f, BLEND_DURATION);
    }
    else
    {
        // Forward motion
        if (keyFlags & 1)
        {
            if (moveBall)
            {
                static_cast<PhysicsRigidBody*>(_scene->findNode("Basketball")->getCollisionObject())->applyForce(Vector3(0, 0, -WALK_SPEED));
            }
            else
            {
                _character->play("walk", PhysicsCharacter::ANIMATION_REPEAT, ANIM_SPEED, BLEND_DURATION);
                _character->setForwardVelocity(1.0f);
            }
        }
        else if (keyFlags & 2)
        {
            if (moveBall)
            {
                static_cast<PhysicsRigidBody*>(_scene->findNode("Basketball")->getCollisionObject())->applyForce(Vector3(0, 0, WALK_SPEED));
            }
            else
            {
                _character->play("walk", PhysicsCharacter::ANIMATION_REPEAT, -ANIM_SPEED, BLEND_DURATION);
                _character->setForwardVelocity(-1.0f);
            }
        }
        else
        {
            // Cancel forward movement
            _character->setForwardVelocity(0.0f);
        }

        // Strafing
        if (keyFlags & 4)
        {
            if (moveBall)
            {
                static_cast<PhysicsRigidBody*>(_scene->findNode("Basketball")->getCollisionObject())->applyForce(Vector3(-WALK_SPEED, 0, 0));
            }
            else
            {
                _character->play("walk", PhysicsCharacter::ANIMATION_REPEAT, ANIM_SPEED, BLEND_DURATION);
                _character->setRightVelocity(1.0f);
            }
        }
        else if (keyFlags & 8)
        {
            if (moveBall)
            {
                static_cast<PhysicsRigidBody*>(_scene->findNode("Basketball")->getCollisionObject())->applyForce(Vector3(WALK_SPEED, 0, 0));
            }
            else
            {
                _character->play("walk", PhysicsCharacter::ANIMATION_REPEAT, -ANIM_SPEED, BLEND_DURATION);
                _character->setRightVelocity(-1.0f);
            }
        }
        else
        {
            // Cancel right movement
            _character->setRightVelocity(0.0f);
        }
    }

	if (!moveBall)
	{
		adjustCamera(elapsedTime);
	}
}

void CharacterGame::render(long elapsedTime)
{
    // Clear the color and depth buffers.
    clear(CLEAR_COLOR_DEPTH, Vector4(0.41f, 0.48f, 0.54f, 1.0f), 1.0f, 0);

    // Draw our scene
    _scene->visit(this, &CharacterGame::drawScene, (void*)0);
    _scene->visit(this, &CharacterGame::drawScene, (void*)1);

    switch (drawDebug)
    {
    case 1:
        Game::getInstance()->getPhysicsController()->drawDebug(_scene->getActiveCamera()->getViewProjectionMatrix());
        break;
    case 2:
        _scene->drawDebug(Scene::DEBUG_BOXES);
        break;
    case 3:
        _scene->drawDebug(Scene::DEBUG_SPHERES);
        break;
    }

    _gamepad->draw();

    _font->begin();
    char fps[32];
    sprintf(fps, "FPS: %d\nCamera Focus: %d", getFrameRate(), (int)cameraFocusDistance);
    _font->drawText(fps, 5, 5, Vector4(1,1,0,1), 20);
    _font->end();
}

bool CharacterGame::drawScene(Node* node, void* cookie)
{
    Model* model = node->getModel();
    if (model)
    {
        switch ((int)cookie)
        {
        case 0: // opaque objects
            if (!node->isTransparent())
                model->draw();
            break;

        case 1: // transparent objects
            if (node->isTransparent())
                model->draw();
            break;
        }
    }

    return true;
}

void CharacterGame::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case Keyboard::KEY_W:
            keyFlags |= 1;
            break;
        case Keyboard::KEY_S:
            keyFlags |= 2;
            break;
        case Keyboard::KEY_A:
            keyFlags |= 4;
            break;
        case Keyboard::KEY_D:
            keyFlags |= 8;
            break;
        case Keyboard::KEY_P:
            drawDebug++;
            if (drawDebug > 3)
                drawDebug = 0;
            break;
        case Keyboard::KEY_B:
            moveBall = !moveBall;
            break;
        case Keyboard::KEY_EQUAL:
        case Keyboard::KEY_PLUS:
            cameraFocusDistance++;
            break;
        case Keyboard::KEY_MINUS:
            cameraFocusDistance--;
            break;
        }
    }
    else if (evt == Keyboard::KEY_RELEASE)
    {
        switch (key)
        {
        case Keyboard::KEY_W:
            keyFlags &= ~1;
            break;
        case Keyboard::KEY_S:
            keyFlags &= ~2;
            break;
        case Keyboard::KEY_A:
            keyFlags &= ~4;
            break;
        case Keyboard::KEY_D:
            keyFlags &= ~8;
            break;
        }
    }
}

void CharacterGame::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    // Get the joystick's current state.
    bool wasActive = _gamepad->isJoystickActive(JOYSTICK);

    _gamepad->touch(x, y, evt, contactIndex);

    // See if the joystick is still active.
    bool isActive = _gamepad->isJoystickActive(JOYSTICK);
    if (!isActive)
    {
        // If it was active before, reset the joystick's influence on the keyflags.
        if (wasActive)
            keyFlags = 0;
    
        switch (evt)
        {
        case Touch::TOUCH_PRESS:
            {
                _rotateX = x;
                _rotateY = y;
            }
            break;
        case Touch::TOUCH_RELEASE:
            {
                _rotateX = 0;
                _rotateY = 0;
            }
            break;
        case Touch::TOUCH_MOVE:
            {
                int deltaX = x - _rotateX;
                int deltaY = y - _rotateY;
                _rotateX = x;
                _rotateY = y;
                _character->getNode()->rotateY(-MATH_DEG_TO_RAD(deltaX * 0.5f));
            }
            break;
        default:
            break;
        }
    }
}

void CharacterGame::loadAnimationClips(Node* node)
{
    _animation = node->getAnimation("movements");
    _animation->createClips("res/boy.animation");

    _character->addAnimation("idle", _animation->getClip("idle"), 0.0f);
    _character->addAnimation("walk", _animation->getClip("walk"), WALK_SPEED);

    _character->play("idle", PhysicsCharacter::ANIMATION_REPEAT);
}

void CharacterGame::collisionEvent(
    PhysicsCollisionObject::CollisionListener::EventType type,
    const PhysicsCollisionObject::CollisionPair& collisionPair,
    const Vector3& contactPointA, const Vector3& contactPointB)
{
    if (collisionPair.objectA == _character)
    {
        if (collisionPair.objectB->getType() == PhysicsCollisionObject::RIGID_BODY)
        {
            PhysicsCharacter* c = static_cast<PhysicsCharacter*>(collisionPair.objectA);
        }
    }
}

void CharacterGame::adjustCamera(long elapsedTime)
{
    static float cameraOffset = 0.0f;

    PhysicsController* physics = Game::getInstance()->getPhysicsController();
    Node* cameraNode = _scene->getActiveCamera()->getNode();

    // Reset camera
    if (cameraOffset != 0.0f)
    {
        cameraNode->translateForward(-cameraOffset);
        cameraOffset = 0.0f;
    }

    Vector3 cameraPosition = cameraNode->getTranslationWorld();
    Vector3 cameraDirection = cameraNode->getForwardVectorWorld();
    cameraDirection.normalize();

    // Get focal point of camera (use the resolved world location of the head joint as a focal point)
    Vector3 focalPoint(cameraPosition + (cameraDirection * cameraFocusDistance));

    PhysicsController::HitResult result;
    PhysicsCollisionObject* occlusion = NULL;
    do
    {
        // Perform a ray test to check for camera collisions
        if (!physics->sweepTest(cameraNode->getCollisionObject(), focalPoint, &result) || result.object == _character)
            break;

        occlusion = result.object;

        // Step the camera closer to the focal point to resolve the occlusion
        do
        {
            // Prevent the camera from getting too close to the character.
            // Without this check, it's possible for the camera to fly past the character
            // and essentially end up in an infinite loop here.
            if (cameraNode->getTranslationWorld().distanceSquared(focalPoint) <= 2.0f)
                return;

            cameraNode->translateForward(0.1f);
            cameraOffset += 0.1f;

        } while (physics->sweepTest(cameraNode->getCollisionObject(), focalPoint, &result) && result.object == occlusion);

    } while (true);

    if (occlusion)
    {
        // TODO: When we change the character over to use a single material+texture, this code will be much cleaner (no material parts and can store MaterialParameter)
        float d = _scene->getActiveCamera()->getNode()->getTranslationWorld().distance(_characterMeshNode->getTranslationWorld());
        if (d < 10)
        {
            float alpha = d / 10.0f;
            _characterMeshNode->setTransparent(true);
            for (unsigned int i = 0; i < 4; i++)
                _characterMeshNode->getModel()->getMaterial(i)->getTechnique((unsigned int)0)->getPass((unsigned int)0)->getParameter("u_alpha")->setValue(alpha);
        }
        else
        {
            _characterMeshNode->setTransparent(false);
            for (unsigned int i = 0; i < 4; i++)
                _characterMeshNode->getModel()->getMaterial(i)->getTechnique((unsigned int)0)->getPass((unsigned int)0)->getParameter("u_alpha")->setValue(1.0f);
        }
    }
}

void CharacterGame::drawSplash(void* param)
{
    clear(CLEAR_COLOR_DEPTH, Vector4(0, 0, 0, 1), 1.0f, 0);
    SpriteBatch* batch = SpriteBatch::create("res/logo_powered_white.png");
    batch->begin();
    batch->draw(this->getWidth() * 0.5f, this->getHeight() * 0.5f, 0.0f, 512.0f, 512.0f, 0.0f, 1.0f, 1.0f, 0.0f, Vector4::one(), true);
    batch->end();
    SAFE_DELETE(batch);
}
