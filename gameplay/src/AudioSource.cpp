/*
 * AudioSource.cpp
 */

#include "Base.h"
#include "Node.h"
#include "AudioBuffer.h"
#include "AudioSource.h"

namespace gameplay
{

AudioSource::AudioSource(AudioBuffer* buffer, ALuint source) 
    : _alSource(source), _buffer(buffer), _looped(true), _gain(1.0f), _pitch(1.0f), _node(NULL)
{
    alSourcei(_alSource, AL_BUFFER, buffer->_alBuffer);
    alSourcei(_alSource, AL_LOOPING, _looped);
    alSourcef(_alSource, AL_PITCH, _pitch);
    alSourcef(_alSource, AL_GAIN, _gain);
    alSourcefv(_alSource, AL_VELOCITY, (const ALfloat*)&_velocity);
}

AudioSource::~AudioSource()
{
    if (_alSource)
    {
        alDeleteSources(1, &_alSource);
        _alSource = 0;
    }

    SAFE_RELEASE(_buffer);
}

AudioSource* AudioSource::create(const char* path)
{
    assert(path);

    // Create an audio buffer from this path.
    AudioBuffer* buffer = AudioBuffer::create(path);
    if (buffer == NULL)
        return NULL;

    // Load the audio source.
    ALuint alSource;
    alGenSources(1, &alSource);
    if (alGetError() != AL_NO_ERROR)
    {
        SAFE_RELEASE(buffer);
        LOG_ERROR("AudioSource::createAudioSource Error generating audio source.");
        return NULL;
    }

    return new AudioSource(buffer, alSource);
}

AudioSource::State AudioSource::getState() const
{
    ALint state;
    alGetSourcei(_alSource, AL_SOURCE_STATE, &state);

    switch (state)
    {
        case AL_PLAYING: 
            return PLAYING;
        case AL_PAUSED:  
            return PAUSED;
        case AL_STOPPED: 
            return STOPPED;
        default:         
            return INITIAL;
    }

    return INITIAL;
}

void AudioSource::play()
{
    alSourcePlay(_alSource);
}

void AudioSource::pause()
{
    alSourcePause(_alSource);
}

void AudioSource::resume()
{
    if (getState() == PAUSED)
    {
        play();
    }
}

void AudioSource::stop()
{
    alSourceStop(_alSource);
}

void AudioSource::rewind()
{
    alSourceRewind(_alSource);
}

bool AudioSource::isLooped() const
{
    return _looped;
}

void AudioSource::setLooped(bool looped)
{
     // Clear error state.
    alGetError();
    alSourcei(_alSource, AL_LOOPING, (looped) ? AL_TRUE : AL_FALSE);

    ALCenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        LOG_ERROR_VARG("AudioSource::setLooped Error: %d", error);
    }

    _looped = looped;
}

float AudioSource::getGain() const
{
    return _gain;
}

void AudioSource::setGain(float gain)
{
    alSourcef(_alSource, AL_GAIN, gain);
    _gain = gain;
}

float AudioSource::getPitch() const
{
    return _pitch;
}

void AudioSource::setPitch(float pitch)
{
    alSourcef(_alSource, AL_PITCH, pitch);
    _pitch = pitch;
}

const Vector3& AudioSource::getVelocity() const
{
    return _velocity;
}

void AudioSource::setVelocity(const Vector3& velocity)
{
    alSourcefv(_alSource, AL_VELOCITY, (ALfloat*)&velocity);
    _velocity = velocity;
}

Node* AudioSource::getNode() const
{
    return _node;
}

void AudioSource::setNode(Node* node)
{
    if (_node != node)
    {
        // Disconnect our current transform.
        if (_node)
        {
            _node->removeListener(this);
        }

        // Connect the new node.
        _node = node;

        if (_node)
        {
            _node->addListener(this);
        }
    }
}

void AudioSource::transformChanged(Transform* transform)
{
    alSourcefv(_alSource, AL_POSITION, (const ALfloat*)&transform->getTranslation());
}

}
