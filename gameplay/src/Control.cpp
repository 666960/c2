#include "Base.h"
#include "Game.h"
#include "Control.h"

namespace gameplay
{
    Control::Control()
        : _id(""), _state(Control::NORMAL), _bounds(Rectangle::empty()), _clipBounds(Rectangle::empty()), _clip(Rectangle::empty()),
            _dirty(true), _consumeTouchEvents(true), _listeners(NULL), _styleOverridden(false)
    {
    }

    Control::Control(const Control& copy)
    {
    }

    Control::~Control()
    {
        if (_listeners)
        {
            for (std::map<Listener::EventType, std::list<Listener*>*>::const_iterator itr = _listeners->begin(); itr != _listeners->end(); itr++)
            {
                std::list<Listener*>* list = itr->second;
                SAFE_DELETE(list);
            }
            SAFE_DELETE(_listeners);
        }

        if (_styleOverridden)
        {
            SAFE_DELETE(_style);
        }
    }

    void Control::init(Theme::Style* style, Properties* properties)
    {
        _style = style;

        Vector2 position;
        Vector2 size;
        properties->getVector2("position", &position);
        properties->getVector2("size", &size);
        _bounds.set(position.x, position.y, size.x, size.y);

        _state = Control::getStateFromString(properties->getString("state"));

        const char* id = properties->getId();
        if (id)
            _id = id;
    }

    const char* Control::getID() const
    {
        return _id.c_str();
    }

    void Control::setPosition(float x, float y)
    {
        _bounds.x = x;
        _bounds.y = y;
        _dirty = true;
    }

    void Control::setSize(float width, float height)
    {
        _bounds.width = width;
        _bounds.height = height;
        _dirty = true;
    }

    void Control::setBounds(const Rectangle& bounds)
    {
        _bounds.set(bounds);
    }

    const Rectangle& Control::getBounds() const
    {
        return _bounds;
    }

    float Control::getX() const
    {
        return _bounds.x;
    }

    float Control::getY() const
    {
        return _bounds.y;
    }

    float Control::getWidth() const
    {
        return _bounds.width;
    }

    float Control::getHeight() const
    {
        return _bounds.height;
    }

    void Control::setOpacity(float opacity, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setOpacity(opacity);
        }
        
        _dirty = true;
    }

    float Control::getOpacity(State state) const
    {
        return getOverlay(state)->getOpacity();
    }

    void Control::setBorder(float top, float bottom, float left, float right, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setBorder(top, bottom, left, right);
        }

        _dirty = true;
    }

    const Theme::Border& Control::getBorder(State state) const
    {
        return getOverlay(state)->getBorder();
    }

    void Control::setSkinRegion(const Rectangle& region, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setSkinRegion(region, _style->_tw, _style->_th);
        }

        _dirty = true;
    }

    const Rectangle& Control::getSkinRegion(State state) const
    {
        return getOverlay(state)->getSkinRegion();
    }

    const Theme::UVs& Control::getSkinUVs(Theme::Skin::SkinArea area, State state) const
    {
        return getOverlay(state)->getSkinUVs(area);
    }

    void Control::setSkinColor(const Vector4& color, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setSkinColor(color);
        }

        _dirty = true;
    }

    const Vector4& Control::getSkinColor(State state) const
    {
        return getOverlay(state)->getSkinColor();
    }

    void Control::setMargin(float top, float bottom, float left, float right)
    {
        _style->setMargin(top, bottom, left, right);
        _dirty = true;
    }

    const Theme::Margin& Control::getMargin() const
    {
        return _style->getMargin();
    }

    void Control::setPadding(float top, float bottom, float left, float right)
    {
        _style->setPadding(top, bottom, left, right);
        _dirty = true;
    }
    
    const Theme::Padding& Control::getPadding() const
    {
        return _style->getPadding();
    }

    void Control::setImageRegion(const char* id, const Rectangle& region, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setImageRegion(id, region, _style->_tw, _style->_th);
        }

        _dirty = true;
    }

    const Rectangle& Control::getImageRegion(const char* id, State state) const
    {
        return getOverlay(state)->getImageRegion(id);
    }

    void Control::setImageColor(const char* id, const Vector4& color, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setImageColor(id, color);
        }

        _dirty = true;
    }

    const Vector4& Control::getImageColor(const char* id, State state) const
    {
        return getOverlay(state)->getImageColor(id);
    }

    const Theme::UVs& Control::getImageUVs(const char* id, State state) const
    {
        return getOverlay(state)->getImageUVs(id);
    }

    void Control::setCursorRegion(const Rectangle& region, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setCursorRegion(region, _style->_tw, _style->_th);
        }

        _dirty = true;
    }

    const Rectangle& Control::getCursorRegion(State state) const
    {
        return getOverlay(state)->getCursorRegion();
    }

    void Control::setCursorColor(const Vector4& color, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setCursorColor(color);
        }

        _dirty = true;
    }

    const Vector4& Control::getCursorColor(State state)
    {
        return getOverlay(state)->getCursorColor();
    }
    
    const Theme::UVs& Control::getCursorUVs(State state)
    {
        return getOverlay(state)->getCursorUVs();
    }

    void Control::setFont(Font* font, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setFont(font);
        }

        _dirty = true;
    }

    Font* Control::getFont(State state) const
    {
        return getOverlay(state)->getFont();
    }

    void Control::setFontSize(unsigned int fontSize, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setFontSize(fontSize);
        }

        _dirty = true;
    }

    unsigned int Control::getFontSize(State state) const
    {
        return getOverlay(state)->getFontSize();
    }

    void Control::setTextColor(const Vector4& color, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setTextColor(color);
        }

        _dirty = true;
    }

    const Vector4& Control::getTextColor(State state) const
    {
        return getOverlay(state)->getTextColor();
    }

    void Control::setTextAlignment(Font::Justify alignment, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setTextAlignment(alignment);
        }

        _dirty = true;
    }

    Font::Justify Control::getTextAlignment(State state) const
    {
        return getOverlay(state)->getTextAlignment();
    }

    void Control::setTextRightToLeft(bool rightToLeft, unsigned char states)
    {
        overrideStyle();
        Theme::Style::Overlay* overlays[Theme::Style::OVERLAY_MAX] = { 0 };
        getOverlays(states, overlays);

        for (int i = 0; i < Theme::Style::OVERLAY_MAX - 1 && overlays[i]; ++i)
        {
            overlays[i]->setTextRightToLeft(rightToLeft);
        }

        _dirty = true;
    }

    bool Control::getTextRightToLeft(State state) const
    {
        return getOverlay(state)->getTextRightToLeft();
    }

    const Rectangle& Control::getClipBounds() const
    {
        return _clipBounds;
    }

    const Rectangle& Control::getClip() const
    {
        return _clip;
    }

    void Control::setStyle(Theme::Style* style)
    {
        if (style != _style)
        {
            _dirty = true;
        }

        _style = style;
    }

    Theme::Style* Control::getStyle() const
    {
        return _style;
    }

    void Control::setState(State state)
    {
        _state = state;
        _dirty = true;
    }

    Control::State Control::getState() const
    {
        return _state;
    }

    void Control::disable()
    {
        _state = DISABLED;
        _dirty = true;
    }

    void Control::enable()
    {
        _state = NORMAL;
        _dirty = true;
    }

    bool Control::isEnabled()
    {
        return _state != DISABLED;
    }

    Theme::Style::OverlayType Control::getOverlayType() const
    {
        switch (_state)
        {
        case Control::NORMAL:
            return Theme::Style::OVERLAY_NORMAL;
        case Control::FOCUS:
            return Theme::Style::OVERLAY_FOCUS;
        case Control::ACTIVE:
            return Theme::Style::OVERLAY_ACTIVE;
        case Control::DISABLED:
            return Theme::Style::OVERLAY_DISABLED;
        default:
            return Theme::Style::OVERLAY_NORMAL;
        }
    }

    void Control::setConsumeTouchEvents(bool consume)
    {
        _consumeTouchEvents = consume;
    }
    
    bool Control::getConsumeTouchEvents()
    {
        return _consumeTouchEvents;
    }

    void Control::addListener(Control::Listener* listener, int eventFlags)
    {
        if ((eventFlags & Listener::PRESS) == Listener::PRESS)
        {
            addSpecificListener(listener, Listener::PRESS);
        }

        if ((eventFlags & Listener::RELEASE) == Listener::RELEASE)
        {
            addSpecificListener(listener, Listener::RELEASE);
        }

        if ((eventFlags & Listener::CLICK) == Listener::CLICK)
        {
            addSpecificListener(listener, Listener::CLICK);
        }

        if ((eventFlags & Listener::VALUE_CHANGED) == Listener::VALUE_CHANGED)
        {
            addSpecificListener(listener, Listener::VALUE_CHANGED);
        }

        if ((eventFlags & Listener::TEXT_CHANGED) == Listener::TEXT_CHANGED)
        {
            addSpecificListener(listener, Listener::TEXT_CHANGED);
        }
    }

    void Control::addSpecificListener(Control::Listener* listener, Listener::EventType eventType)
    {
        if (!_listeners)
        {
            _listeners = new std::map<Listener::EventType, std::list<Listener*>*>();
        }

        std::map<Listener::EventType, std::list<Listener*>*>::const_iterator itr = _listeners->find(eventType);
        if (itr == _listeners->end())
        {
            _listeners->insert(std::make_pair(eventType, new std::list<Listener*>()));
            itr = _listeners->find(eventType);
        }

        std::list<Listener*>* listenerList = itr->second;
        listenerList->push_back(listener);
    }

    bool Control::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
    {
        if (!isEnabled())
        {
            return false;
        }

        switch (evt)
        {
        case Touch::TOUCH_PRESS:
            notifyListeners(Listener::PRESS);
            break;
            
        case Touch::TOUCH_RELEASE:
            // Always trigger Listener::RELEASE
            notifyListeners(Listener::RELEASE);

            // Only trigger Listener::CLICK if both PRESS and RELEASE took place within the control's bounds.
            if (x > 0 && x <= _clipBounds.width &&
                y > 0 && y <= _clipBounds.height)
            {
                notifyListeners(Listener::CLICK);
            }
            break;
        }

        return _consumeTouchEvents;
    }

    void Control::keyEvent(Keyboard::KeyEvent evt, int key)
    {
    }

    void Control::notifyListeners(Listener::EventType eventType)
    {
        if (_listeners)
        {
            std::map<Listener::EventType, std::list<Listener*>*>::const_iterator itr = _listeners->find(eventType);
            if (itr != _listeners->end())
            {
                std::list<Listener*>* listenerList = itr->second;
                for (std::list<Listener*>::iterator listenerItr = listenerList->begin(); listenerItr != listenerList->end(); listenerItr++)
                {
                    (*listenerItr)->controlEvent(this, eventType);
                }
            }
        }
    }

    void Control::update(const Rectangle& clip)
    {
        // Calculate the bounds.
        float x = clip.x + _bounds.x;
        float y = clip.y + _bounds.y;
        float width = _bounds.width;
        float height = _bounds.height;

        float clipX2 = clip.x + clip.width;
        float x2 = x + width;
        if (x2 > clipX2)
        {
            width = clipX2 - x;
        }

        float clipY2 = clip.y + clip.height;
        float y2 = y + height;
        if (y2 > clipY2)
        {
            height = clipY2 - y;
        }

        _clipBounds.set(_bounds.x, _bounds.y, width, height);

        // Calculate the clipping viewport.
        const Theme::Border& border = getBorder(_state);
        const Theme::Padding& padding = getPadding();

        x +=  border.left + padding.left;
        y +=  border.top + padding.top;
        width = _bounds.width - border.left - padding.left - border.right - padding.right;
        height = _bounds.height - border.top - padding.top - border.bottom - padding.bottom;

        _textBounds.set(x, y, width, height);

        clipX2 = clip.x + clip.width;
        x2 = x + width;
        if (x2 > clipX2)
        {
            width = clipX2 - x;
        }

        clipY2 = clip.y + clip.height;
        y2 = y + height;
        if (y2 > clipY2)
        {
            height = clipY2 - y;
        }

        if (x < clip.x)
        {
            x = clip.x;
        }

        if (y < clip.y)
        {
            y = clip.y;
        }

        _clip.set(x, y, width, height);
    }

    void Control::drawBorder(SpriteBatch* spriteBatch, const Rectangle& clip)
    {
        Vector2 pos(clip.x + _bounds.x, clip.y + _bounds.y);

        // Get the border and background images for this control's current state.
        //Theme::UVs topLeft, top, topRight, left, center, right, bottomLeft, bottom, bottomRight;
        const Theme::UVs& topLeft = getSkinUVs(Theme::Skin::TOP_LEFT, _state);
        const Theme::UVs& top = getSkinUVs(Theme::Skin::TOP, _state);
        const Theme::UVs& topRight = getSkinUVs(Theme::Skin::TOP_RIGHT, _state);
        const Theme::UVs& left = getSkinUVs(Theme::Skin::LEFT, _state);
        const Theme::UVs& center = getSkinUVs(Theme::Skin::CENTER, _state);
        const Theme::UVs& right = getSkinUVs(Theme::Skin::RIGHT, _state);
        const Theme::UVs& bottomLeft = getSkinUVs(Theme::Skin::BOTTOM_LEFT, _state);
        const Theme::UVs& bottom = getSkinUVs(Theme::Skin::BOTTOM, _state);
        const Theme::UVs& bottomRight = getSkinUVs(Theme::Skin::BOTTOM_RIGHT, _state);

        // Calculate screen-space positions.
        const Theme::Border& border = getBorder(_state);
        const Theme::Padding& padding = getPadding();
        Vector4 skinColor = getSkinColor(_state);
        skinColor.w *= getOpacity(_state);

        float midWidth = _bounds.width - border.left - border.right;
        float midHeight = _bounds.height - border.top - border.bottom;
        float midX = pos.x + border.left;
        float midY = pos.y + border.top;
        float rightX = pos.x + _bounds.width - border.right;
        float bottomY = pos.y + _bounds.height - border.bottom;

        // Draw themed border sprites.
        if (!border.left && !border.right && !border.top && !border.bottom)
        {
            // No border, just draw the image.
            spriteBatch->draw(pos.x, pos.y, _bounds.width, _bounds.height, center.u1, center.v1, center.u2, center.v2, skinColor, clip);
        }
        else
        {
            if (border.left && border.top)
                spriteBatch->draw(pos.x, pos.y, border.left, border.top, topLeft.u1, topLeft.v1, topLeft.u2, topLeft.v2, skinColor, clip);
            if (border.top)
                spriteBatch->draw(pos.x + border.left, pos.y, midWidth, border.top, top.u1, top.v1, top.u2, top.v2, skinColor, clip);
            if (border.right && border.top)
                spriteBatch->draw(rightX, pos.y, border.right, border.top, topRight.u1, topRight.v1, topRight.u2, topRight.v2, skinColor, clip);
            if (border.left)
                spriteBatch->draw(pos.x, midY, border.left, midHeight, left.u1, left.v1, left.u2, left.v2, skinColor, clip);
            if (border.left && border.right && border.top && border.bottom)
                spriteBatch->draw(pos.x + border.left, pos.y + border.top, _bounds.width - border.left - border.right, _bounds.height - border.top - border.bottom,
                    center.u1, center.v1, center.u2, center.v2, skinColor, clip);
            if (border.right)
                spriteBatch->draw(rightX, midY, border.right, midHeight, right.u1, right.v1, right.u2, right.v2, skinColor, clip);
            if (border.bottom && border.left)
                spriteBatch->draw(pos.x, bottomY, border.left, border.bottom, bottomLeft.u1, bottomLeft.v1, bottomLeft.u2, bottomLeft.v2, skinColor, clip);
            if (border.bottom)
                spriteBatch->draw(midX, bottomY, midWidth, border.bottom, bottom.u1, bottom.v1, bottom.u2, bottom.v2, skinColor, clip);
            if (border.bottom && border.right)
                spriteBatch->draw(rightX, bottomY, border.right, border.bottom, bottomRight.u1, bottomRight.v1, bottomRight.u2, bottomRight.v2, skinColor, clip);
        }
    }

    void Control::drawImages(SpriteBatch* spriteBatch, const Rectangle& position)
    {
    }

    void Control::drawText(const Rectangle& position)
    {
    }

    bool Control::isDirty()
    {
        return _dirty;
    }

    bool Control::isContainer()
    {
        return false;
    }

    Control::State Control::getStateFromString(const char* state)
    {
        if (!state)
        {
            return NORMAL;
        }

        if (strcmp(state, "NORMAL") == 0)
        {
            return NORMAL;
        }
        else if (strcmp(state, "ACTIVE") == 0)
        {
            return ACTIVE;
        }
        else if (strcmp(state, "FOCUS") == 0)
        {
            return FOCUS;
        }
        else if (strcmp(state, "DISABLED") == 0)
        {
            return DISABLED;
        }

        return NORMAL;
    }

    // Implementation of AnimationHandler
    unsigned int Control::getAnimationPropertyComponentCount(int propertyId) const
    {
        switch(propertyId)
        {
        case ANIMATE_POSITION:
        case ANIMATE_SIZE:
            return 2;

        case ANIMATE_POSITION_X:
        case ANIMATE_POSITION_Y:
        case ANIMATE_SIZE_WIDTH:
        case ANIMATE_SIZE_HEIGHT:
        case ANIMATE_OPACITY:
            return 1;

        default:
            return -1;
        }
    }

    void Control::getAnimationPropertyValue(int propertyId, AnimationValue* value)
    {
        switch(propertyId)
        {
        case ANIMATE_POSITION:
            value->setFloat(0, _bounds.x);
            value->setFloat(1, _bounds.y);
            break;
        case ANIMATE_SIZE:
            value->setFloat(0, _clipBounds.width);
            value->setFloat(1, _clipBounds.height);
            break;
        case ANIMATE_POSITION_X:
            value->setFloat(0, _bounds.x);
            break;
        case ANIMATE_POSITION_Y:
            value->setFloat(0, _bounds.y);
            break;
        case ANIMATE_SIZE_WIDTH:
            value->setFloat(0, _clipBounds.width);
            break;
        case ANIMATE_SIZE_HEIGHT:
            value->setFloat(0, _clipBounds.height);
            break;
        case ANIMATE_OPACITY:
        default:
            break;
        }
    }

    void Control::setAnimationPropertyValue(int propertyId, AnimationValue* value, float blendWeight)
    {
        switch(propertyId)
        {
        case ANIMATE_POSITION:
            applyAnimationValuePositionX(value->getFloat(0), blendWeight);
            applyAnimationValuePositionY(value->getFloat(1), blendWeight);
            break;
        case ANIMATE_POSITION_X:
            applyAnimationValuePositionX(value->getFloat(0), blendWeight);
            break;
        case ANIMATE_POSITION_Y:
            applyAnimationValuePositionY(value->getFloat(0), blendWeight);
            break;
        case ANIMATE_SIZE:
            applyAnimationValueSizeWidth(value->getFloat(0), blendWeight);
            applyAnimationValueSizeHeight(value->getFloat(1), blendWeight);
            break;
        case ANIMATE_SIZE_WIDTH:
            applyAnimationValueSizeWidth(value->getFloat(0), blendWeight);
            break;
        case ANIMATE_SIZE_HEIGHT:
            applyAnimationValueSizeHeight(value->getFloat(0), blendWeight);
            break;
        case ANIMATE_OPACITY:
            applyAnimationValueOpacity();
        default:
            break;
        }
    }

    void Control::applyAnimationValuePositionX(float x, float blendWeight)
    {
        if ((_animationPropertyBitFlag & ANIMATION_POSITION_X_BIT) != ANIMATION_POSITION_X_BIT)
        {
            _animationPropertyBitFlag |= ANIMATION_POSITION_X_BIT;
        }
        else
        {
            x = Curve::lerp(blendWeight, _bounds.x, x);
        }
        _bounds.x = x;
        _dirty = true;
    }
    
    void Control::applyAnimationValuePositionY(float y, float blendWeight)
    {
        if ((_animationPropertyBitFlag & ANIMATION_POSITION_Y_BIT) != ANIMATION_POSITION_Y_BIT)
        {
            _animationPropertyBitFlag |= ANIMATION_POSITION_Y_BIT;
        }
        else
        {
            y = Curve::lerp(blendWeight, _bounds.y, y);
        }
        _bounds.y = y;
        _dirty = true;
    }
    
    void Control::applyAnimationValueSizeWidth(float width, float blendWeight)
    {
        if ((_animationPropertyBitFlag & ANIMATION_SIZE_WIDTH_BIT) != ANIMATION_SIZE_WIDTH_BIT)
        {
            _animationPropertyBitFlag |= ANIMATION_SIZE_WIDTH_BIT;
        }
        else
        {
            width = Curve::lerp(blendWeight, _clipBounds.width, width);
        }
        _clipBounds.width = width;
        _dirty = true;
    }

    void Control::applyAnimationValueSizeHeight(float height, float blendWeight)
    {
        if ((_animationPropertyBitFlag & ANIMATION_SIZE_HEIGHT_BIT) != ANIMATION_SIZE_HEIGHT_BIT)
        {
            _animationPropertyBitFlag |= ANIMATION_SIZE_HEIGHT_BIT;
        }
        else
        {
            height = Curve::lerp(blendWeight, _clipBounds.height, height);
        }
        _clipBounds.height = height;
        _dirty = true;
    }

    void Control::applyAnimationValueOpacity()
    {
        if ((_animationPropertyBitFlag & ANIMATION_OPACITY_BIT) != ANIMATION_OPACITY_BIT)
        {
            _animationPropertyBitFlag |= ANIMATION_OPACITY_BIT;
        }
        _dirty = true;
    }
    
    Theme::Style::Overlay** Control::getOverlays(unsigned char overlayTypes, Theme::Style::Overlay** overlays)
    {
        unsigned int index = 0;
        if ((overlayTypes & NORMAL) == NORMAL)
        {
            overlays[index++] = _style->getOverlay(Theme::Style::OVERLAY_NORMAL);
        }

        if ((overlayTypes & FOCUS) == FOCUS)
        {
            overlays[index++] = _style->getOverlay(Theme::Style::OVERLAY_FOCUS);
        }

        if ((overlayTypes & ACTIVE) == ACTIVE)
        {
            overlays[index++] = _style->getOverlay(Theme::Style::OVERLAY_ACTIVE);
        }

        if ((overlayTypes & DISABLED) == DISABLED)
        {
            overlays[index++] = _style->getOverlay(Theme::Style::OVERLAY_DISABLED);
        }

        return overlays;
    }

    Theme::Style::Overlay* Control::getOverlay(State state) const
    {
        switch(state)
        {
        case Control::NORMAL:
            return _style->getOverlay(Theme::Style::OVERLAY_NORMAL);
        case Control::FOCUS:
            return _style->getOverlay(Theme::Style::OVERLAY_FOCUS);
        case Control::ACTIVE:
            return _style->getOverlay(Theme::Style::OVERLAY_ACTIVE);
        case Control::DISABLED:
            return _style->getOverlay(Theme::Style::OVERLAY_DISABLED);
        default:
            return NULL;
        }
    }

    void Control::overrideStyle()
    {
        if (_styleOverridden)
        {
            return;
        }

        // Copy the style.
        WARN_VARG("%d", sizeof(Theme::Style::Overlay));
        _style = new Theme::Style(*_style);
        _styleOverridden = true;
    }
}
