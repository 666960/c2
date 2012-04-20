#include "Base.h"
#include "Node.h"
#include "AudioListener.h"
#include "Game.h"

namespace gameplay
{

AudioListener::AudioListener()
    : _gain(1.0f), _camera(NULL)
{
}

AudioListener::~AudioListener()
{
    SAFE_RELEASE(_camera);
}

AudioListener* AudioListener::getInstance()
{
    return Game::getInstance()->getAudioListener();
}

float AudioListener::getGain() const 
{ 
    return _gain; 
}

void AudioListener::setGain(float gain)
{
    _gain = gain;
}

const Vector3& AudioListener::getPosition() const 
{ 
    return _position; 
}

void AudioListener::setPosition(const Vector3& position)
{
    _position = position;
}

const Vector3& AudioListener::getVelocity() const 
{ 
    return _velocity; 
}

void AudioListener::setVelocity(const Vector3& velocity)
{
    _velocity = velocity;
}

const float* AudioListener::getOrientation() const
{
    return (const float*)&_orientation[0];
}

const Vector3& AudioListener::getOrientationForward() const 
{ 
    return _orientation[0]; 
}

const Vector3& AudioListener::getOrientationUp() const 
{ 
    return _orientation[1]; 
}

void AudioListener::setOrientation(const Vector3& forward, const Vector3& up)
{
    _orientation[0].x = forward.x;
    _orientation[0].y = forward.y;
    _orientation[0].z = forward.z;

    _orientation[1].x = up.x;
    _orientation[1].y = up.y;
    _orientation[1].z = up.z;
}

Camera* AudioListener::getCamera() const
{
    return _camera;
}

void AudioListener::setCamera(Camera* c)
{
    if (_camera != c)
    {
        // Disconnect our current camera.
        if (_camera)
        {
            _camera->getNode()->removeListener(this);
            SAFE_RELEASE(_camera);
        }

        // Connect the new camera.
        _camera = c;

        if (_camera)
        {
            _camera->addRef();
            _camera->getNode()->addListener(this);
        }
    }
}

void AudioListener::transformChanged(Transform* transform, long cookie)
{
    if (transform)
    {
        Node* node = static_cast<Node*>(transform);
        setPosition(node->getTranslationWorld());
        
        Vector3 up;
        node->getWorldMatrix().getUpVector(&up);
        setOrientation(node->getForwardVectorWorld(), up);
    }
}

}
