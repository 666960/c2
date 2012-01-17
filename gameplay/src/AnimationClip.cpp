#include "Base.h"
#include "AnimationClip.h"
#include "Animation.h"
#include "AnimationTarget.h"
#include "Game.h"
#include "Quaternion.h"

namespace gameplay
{

AnimationClip::AnimationClip(const char* id, Animation* animation, unsigned long startTime, unsigned long endTime)
    : _id(id), _animation(animation), _startTime(startTime), _endTime(endTime), _duration(_endTime - _startTime), _repeatCount(1.0f), 
      _activeDuration(_duration * _repeatCount), _speed(1.0f), _isPlaying(false), _timeStarted(0), _elapsedTime(0), _runningTime(0), 
      _crossFadeToClip(NULL), _crossFadeStart(0), _crossFadeOutElapsed(0), _crossFadeOutDuration(0), _blendWeight(1.0f), 
      _isFadingOutStarted(false), _isFadingOut(false), _isFadingIn(false), _beginListeners(NULL), _endListeners(NULL)
{
    assert(0 <= startTime && startTime <= animation->_duration && 0 <= endTime && endTime <= animation->_duration);
    
    unsigned int channelCount = _animation->_channels.size();    
    for (unsigned int i = 0; i < channelCount; i++)
    {
        _values.push_back(new AnimationValue(_animation->_channels[i]->_curve->getComponentCount()));
    }
}

AnimationClip::~AnimationClip()
{
    std::vector<AnimationValue*>::iterator valueIter = _values.begin();
    while (valueIter != _values.end())
    {
        SAFE_DELETE(*valueIter);
        valueIter++;
    }
    _values.clear();

    SAFE_RELEASE(_crossFadeToClip);
    SAFE_DELETE(_beginListeners);
    SAFE_DELETE(_endListeners);
}

const char* AnimationClip::getID() const
{
    return _id.c_str();
}

Animation* AnimationClip::getAnimation() const
{
    return _animation;
}

unsigned long AnimationClip::getStartTime() const
{
    return _startTime;
}

unsigned long AnimationClip::getEndTime() const
{
    return _endTime;
}

unsigned long AnimationClip::getElaspedTime() const
{
    return _elapsedTime;
}

void AnimationClip::setRepeatCount(float repeatCount)
{
    assert(repeatCount == REPEAT_INDEFINITE || repeatCount > 0.0f);

    _repeatCount = repeatCount;

    if (repeatCount == REPEAT_INDEFINITE)
    {
        _activeDuration = _duration;
    }
    else
    {
        _activeDuration = _repeatCount * _duration;
    }
}

float AnimationClip::getRepeatCount() const
{
    return _repeatCount;
}

void AnimationClip::setActiveDuration(unsigned long duration)
{
    if (duration == REPEAT_INDEFINITE)
    {
        _repeatCount = REPEAT_INDEFINITE;
        _activeDuration = _duration;
    }
    else
    {
        _activeDuration = _duration;
        _repeatCount = (float) _activeDuration / (float) _duration;
    }
}

unsigned long AnimationClip::getActiveDuration() const
{
    if (_repeatCount == REPEAT_INDEFINITE)
        return REPEAT_INDEFINITE;

    return _activeDuration;
}

void AnimationClip::setSpeed(float speed)
{
    _speed = speed;
}

float AnimationClip::getSpeed() const
{
    return _speed;
}

void AnimationClip::setBlendWeight(float blendWeight)
{
    _blendWeight = blendWeight;
}

float AnimationClip::getBlendWeight() const
{
    return _blendWeight;
}

bool AnimationClip::isPlaying() const
{
    return _isPlaying;
}

void AnimationClip::play()
{
    _animation->_controller->schedule(this);
    _timeStarted = Game::getGameTime();
}

void AnimationClip::stop()
{
    _animation->_controller->unschedule(this);
    if (_isPlaying)
    {
        _isPlaying = false;
        onEnd();
    }
}

void AnimationClip::crossFade(AnimationClip* clip, unsigned long duration)
{
    assert(clip);

    if (clip->_isPlaying && clip->_isFadingOut)
    {
        clip->_isFadingOut = false;
        clip->_crossFadeToClip->_isFadingIn = false;
        SAFE_RELEASE(clip->_crossFadeToClip);
    }

    // If I already have a clip I'm fading too.. release it.
    if (_crossFadeToClip)
        SAFE_RELEASE(_crossFadeToClip);

    // Assign the clip we're fading to, and increase its ref count.
    _crossFadeToClip = clip;
    _crossFadeToClip->addRef();
        
    // Set the fade in clip to fading in, and set the duration of the fade in.
    _crossFadeToClip->_isFadingIn = true;
    
    // Set this clip to fade out, and reset the elapsed time for the fade out.
    _isFadingOut = true;
    _crossFadeOutElapsed = 0;
    _crossFadeOutDuration = duration;
    _crossFadeStart = (Game::getGameTime() - _timeStarted);
    _isFadingOutStarted = true;
    
    if (!_isPlaying)
        play();

    _crossFadeToClip->play(); 
}

void AnimationClip::addBeginListener(AnimationClip::Listener* listener)
{
    if (!_beginListeners)
        _beginListeners = new std::vector<Listener*>;

    _beginListeners->push_back(listener);
}

void AnimationClip::addEndListener(AnimationClip::Listener* listener)
{
    if (!_endListeners)
        _endListeners = new std::vector<Listener*>;

    _endListeners->push_back(listener);
}

bool AnimationClip::update(unsigned long elapsedTime, std::list<AnimationTarget*>* activeTargets)
{
    float speed = _speed;
    if (!_isPlaying)
    {
        onBegin();
        _elapsedTime = Game::getGameTime() - _timeStarted;
        _runningTime = _elapsedTime * speed;
    }
    else
    {
        // Update elapsed time.
        _elapsedTime += elapsedTime;
        _runningTime += elapsedTime * speed;
    }

    float percentComplete = 0.0f;

    // Check to see if clip is complete.
    if (_repeatCount != REPEAT_INDEFINITE && ((_speed >= 0 && _runningTime >= (long) _activeDuration) || (_speed < 0 && _runningTime <= 0)))
    {
        _isPlaying = false;
        if (_speed >= 0)
        {
            percentComplete = _activeDuration % _duration; // Get's the fractional part of the final repeat.
            if (percentComplete == 0.0f)
                percentComplete = _duration;
        }
        else
        {
            percentComplete = 0.0f; // If we are negative speed, the end value should be 0.
        }
    }
    else
    {
        // Gets portion/fraction of the repeat.
        percentComplete = (float) (_runningTime % _duration);
    }

    // Add back in start time, and divide by the total animation's duration to get the actual percentage complete
    percentComplete = (float)(_startTime + percentComplete) / (float) _animation->_duration;
    
    if (_isFadingOut)
    {
        if (_isFadingOutStarted) // Calculate elapsed time since the fade out begin.
        {
            _crossFadeOutElapsed = (_elapsedTime - _crossFadeStart) * speed;
            _isFadingOutStarted = false;
        }
        else
        {
            // continue tracking elapsed time.
            _crossFadeOutElapsed += elapsedTime * speed;
        }

        if (_crossFadeOutElapsed < _crossFadeOutDuration)
        {
            float tempBlendWeight = (float) (_crossFadeOutDuration - _crossFadeOutElapsed) / (float) _crossFadeOutDuration;
            _crossFadeToClip->_blendWeight = (1.0f - tempBlendWeight);
            
            // adjust the clip your blending to's weight to be a percentage of your current blend weight
            if (_isFadingIn)
            {
                _crossFadeToClip->_blendWeight *= _blendWeight;
                _blendWeight -= _crossFadeToClip->_blendWeight;
            }
            else
            {
                _blendWeight = tempBlendWeight;
            }
        }
        else
        {   // Fade done.
            _crossFadeToClip->_blendWeight = 1.0f;
                
            if (_isFadingIn)
                _crossFadeToClip->_blendWeight *= _blendWeight;

            _crossFadeToClip->_isFadingIn = false;
            SAFE_RELEASE(_crossFadeToClip);
            _blendWeight = 0.0f; 
            _isFadingOut = false;
            _isPlaying = false;
        }
    }
    
    // Evaluate this clip.
    Animation::Channel* channel = NULL;
    AnimationValue* value = NULL;
    AnimationTarget* target = NULL;
    unsigned int channelCount = _animation->_channels.size();
    for (unsigned int i = 0; i < channelCount; i++)
    {
        channel = _animation->_channels[i];
        target = channel->_target;
        value = _values[i];

        // If the target's _animationPropertyBitFlag is clear, we can assume that this is the first
        // animation channel to act on the target and we can add the target to the list of
        // active targets stored by the AnimationController.
        if (target->_animationPropertyBitFlag == 0x00)
            activeTargets->push_front(target);

        // Evaluate the point on Curve
        channel->_curve->evaluate(percentComplete, value->_value);
        // Set the animation value on the target property.
        target->setAnimationPropertyValue(channel->_propertyId, value, _blendWeight);
    }

    // When ended. Probably should move to it's own method so we can call it when the clip is ended early.
    if (!_isPlaying)
    {
        onEnd();
        // Notify end listeners if any.
        if (_endListeners)
        {
            std::vector<Listener*>::iterator listener = _endListeners->begin();
            while (listener != _endListeners->end())
            {
                (*listener)->animationEvent(this, Listener::END);
                listener++;
            }
        }
    }

    return !_isPlaying;
}

void AnimationClip::onBegin()
{
    // Initialize animation to play.
    _isPlaying = true;
    _elapsedTime = 0;

    if (_speed > 0)
    {
        _runningTime = 0;
    }
    else
    {
        _runningTime = _activeDuration;
    }

    // Notify begin listeners.. if any.
    if (_beginListeners)
    {
        std::vector<Listener*>::iterator listener = _beginListeners->begin();
        while (listener != _beginListeners->end())
        {
            (*listener)->animationEvent(this, Listener::BEGIN);
            listener++;
        }
    }
}

void AnimationClip::onEnd()
{
    _blendWeight = 1.0f;
    _timeStarted = 0;
}

}
