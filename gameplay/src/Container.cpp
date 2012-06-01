#include "Base.h"
#include "Container.h"
#include "Layout.h"
#include "AbsoluteLayout.h"
#include "FlowLayout.h"
#include "VerticalLayout.h"
#include "Label.h"
#include "Button.h"
#include "CheckBox.h"
#include "RadioButton.h"
#include "Slider.h"
#include "TextBox.h"
#include "Joystick.h"
#include "Game.h"

namespace gameplay
{

// If the user stops scrolling for this amount of time (in millis) before touch/click release, don't apply inertia.
static const long SCROLL_INERTIA_DELAY = 100L;
// Factor to multiply friction by before applying to velocity.
static const float SCROLL_FRICTION_FACTOR = 5.0f;

Container::Container()
    : _layout(NULL), _scrollBarTopCap(NULL), _scrollBarVertical(NULL), _scrollBarBottomCap(NULL),
      _scrollBarLeftCap(NULL), _scrollBarHorizontal(NULL), _scrollBarRightCap(NULL),
      _scroll(SCROLL_NONE), _scrollBarBounds(Rectangle::empty()), _scrollPosition(Vector2::zero()),
      _scrollBarsAutoHide(false), _scrollBarOpacity(1.0f), _scrolling(false),
       _scrollingFirstX(0), _scrollingFirstY(0),
      _scrollingLastX(0), _scrollingLastY(0),
      _scrollingStartTimeX(0), _scrollingStartTimeY(0), _scrollingLastTime(0),
      _scrollingVelocity(Vector2::zero()), _scrollingFriction(1.0f),
      _scrollingRight(false), _scrollingDown(false), _scrollBarOpacityClip(NULL), _zIndexDefault(0)
{
}

Container::Container(const Container& copy)
{
}

Container::~Container()
{
    std::vector<Control*>::iterator it;
    for (it = _controls.begin(); it < _controls.end(); it++)
    {
        SAFE_RELEASE((*it));
    }
    SAFE_RELEASE(_layout);
}

Container* Container::create(Layout::Type type)
{
    Layout* layout = NULL;
    switch (type)
    {
    case Layout::LAYOUT_ABSOLUTE:
        layout = AbsoluteLayout::create();
        break;
    case Layout::LAYOUT_FLOW:
        layout = FlowLayout::create();
        break;
    case Layout::LAYOUT_VERTICAL:
        layout = VerticalLayout::create();
        break;
    }

    Container* container = new Container();
    container->_layout = layout;

    return container;
}

Container* Container::create(Theme::Style* style, Properties* properties, Theme* theme)
{
    GP_ASSERT(properties);

    const char* layoutString = properties->getString("layout");
    Container* container = Container::create(getLayoutType(layoutString));
    container->initialize(style, properties);
    container->_scroll = getScroll(properties->getString("scroll"));
    container->_scrollBarsAutoHide = properties->getBool("scrollBarsAutoHide");
    if (container->_scrollBarsAutoHide)
    {
        container->_scrollBarOpacity = 0.0f;
    }
    container->addControls(theme, properties);
    container->_layout->update(container, container->_scrollPosition);

    return container;
}

void Container::addControls(Theme* theme, Properties* properties)
{
    GP_ASSERT(theme);
    GP_ASSERT(properties);

    // Add all the controls to this container.
    Properties* controlSpace = properties->getNextNamespace();
    while (controlSpace != NULL)
    {
        Control* control = NULL;

        const char* controlStyleName = controlSpace->getString("style");
        Theme::Style* controlStyle = NULL;
        if (controlStyleName)
        {
            controlStyle = theme->getStyle(controlStyleName);
        }
        else
        {
            Theme::Style::Overlay* overlay = Theme::Style::Overlay::create();
            controlStyle = new Theme::Style(theme, "", 1.0f / theme->_texture->getWidth(), 1.0f / theme->_texture->getHeight(),
                Theme::Margin::empty(), Theme::Border::empty(), overlay, overlay, overlay, overlay);
        }

        std::string controlName(controlSpace->getNamespace());
        std::transform(controlName.begin(), controlName.end(), controlName.begin(), (int(*)(int))toupper);
        if (controlName == "LABEL")
        {
            control = Label::create(controlStyle, controlSpace);
        }
        else if (controlName == "BUTTON")
        {
            control = Button::create(controlStyle, controlSpace);
        }
        else if (controlName == "CHECKBOX")
        {
            control = CheckBox::create(controlStyle, controlSpace);
        }
        else if (controlName == "RADIOBUTTON")
        {
            control = RadioButton::create(controlStyle, controlSpace);
        }
        else if (controlName == "CONTAINER")
        {
            control = Container::create(controlStyle, controlSpace, theme);
        }
        else if (controlName == "SLIDER")
        {
            control = Slider::create(controlStyle, controlSpace);
        }
        else if (controlName == "TEXTBOX")
        {
            control = TextBox::create(controlStyle, controlSpace);
        }
        else if (controlName == "JOYSTICK")
        {
            control = Joystick::create(controlStyle, controlSpace);
        }
        else
        {
            GP_ERROR("Failed to create control; unrecognized control name '%s'.", controlName.c_str());
        }

        // Add the new control to the form.
        if (control)
        {
            addControl(control);

            if (control->getZIndex() == -1)
            {
                control->setZIndex(_zIndexDefault++);
            }
        }

        // Get the next control.
        controlSpace = properties->getNextNamespace();
    }

    // Sort controls by Z-Order.
    std::sort(_controls.begin(), _controls.end(), &sortControlsByZOrder);
}

Layout* Container::getLayout()
{
    return _layout;
}

unsigned int Container::addControl(Control* control)
{
    GP_ASSERT(control);
    _controls.push_back(control);

    return _controls.size() - 1;
}

void Container::insertControl(Control* control, unsigned int index)
{
    GP_ASSERT(control);
    std::vector<Control*>::iterator it = _controls.begin() + index;
    _controls.insert(it, control);
}

void Container::removeControl(unsigned int index)
{
    std::vector<Control*>::iterator it = _controls.begin() + index;
    _controls.erase(it);
}

void Container::removeControl(const char* id)
{
    std::vector<Control*>::iterator it;
    for (it = _controls.begin(); it < _controls.end(); it++)
    {
        Control* c = *it;
        if (strcmp(id, c->getID()) == 0)
        {
            _controls.erase(it);
            return;
        }
    }
}

void Container::removeControl(Control* control)
{
    GP_ASSERT(control);
    std::vector<Control*>::iterator it;
    for (it = _controls.begin(); it < _controls.end(); it++)
    {
        if (*it == control)
        {
            _controls.erase(it);
            return;
        }
    }
}

Control* Container::getControl(unsigned int index) const
{
    std::vector<Control*>::const_iterator it = _controls.begin() + index;
    return *it;
}

Control* Container::getControl(const char* id) const
{
    GP_ASSERT(id);
    std::vector<Control*>::const_iterator it;
    for (it = _controls.begin(); it < _controls.end(); it++)
    {
        Control* c = *it;
        GP_ASSERT(c);
        if (strcmp(id, c->getID()) == 0)
        {
            return c;
        }
        else if (c->isContainer())
        {
            Control* cc = ((Container*)c)->getControl(id);
            if (cc)
            {
                return cc;
            }
        }
    }

    return NULL;
}

const std::vector<Control*>& Container::getControls() const
{
    return _controls;
}

void Container::setScroll(Scroll scroll)
{
    if (scroll != _scroll)
    {
        _scroll = scroll;
        _dirty = true;
    }
}

Container::Scroll Container::getScroll() const
{
    return _scroll;
}

void Container::setScrollBarsAutoHide(bool autoHide)
{
    if (autoHide != _scrollBarsAutoHide)
    {
        _scrollBarsAutoHide = autoHide;
        _dirty = true;
    }
}

bool Container::isScrollBarsAutoHide() const
{
    return _scrollBarsAutoHide;
}

Animation* Container::getAnimation(const char* id) const
{
    std::vector<Control*>::const_iterator itr = _controls.begin();
    std::vector<Control*>::const_iterator end = _controls.end();
        
    Control* control = NULL;
    for (; itr != end; itr++)
    {
        control = *itr;
        GP_ASSERT(control);
        Animation* animation = control->getAnimation(id);
        if (animation)
            return animation;

        if (control->isContainer())
        {
            animation = ((Container*)control)->getAnimation(id);
            if (animation)
                return animation;
        }
    }

    return NULL;
}

void Container::update(const Control* container, const Vector2& offset)
{
    // Update this container's viewport.
    Control::update(container, offset);

    // Get scrollbar images and diminish clipping bounds to make room for scrollbars.
    if ((_scroll & SCROLL_HORIZONTAL) == SCROLL_HORIZONTAL)
    {
        _scrollBarLeftCap = getImage("scrollBarLeftCap", _state);
        _scrollBarHorizontal = getImage("horizontalScrollBar", _state);
        _scrollBarRightCap = getImage("scrollBarRightCap", _state);

        GP_ASSERT(_scrollBarLeftCap && _scrollBarHorizontal && _scrollBarRightCap);

        _viewportClipBounds.height -= _scrollBarHorizontal->getRegion().height;
    }

    if ((_scroll & SCROLL_VERTICAL) == SCROLL_VERTICAL)
    {
        _scrollBarTopCap = getImage("scrollBarTopCap", _state);
        _scrollBarVertical = getImage("verticalScrollBar", _state);
        _scrollBarBottomCap = getImage("scrollBarBottomCap", _state);

        GP_ASSERT(_scrollBarTopCap && _scrollBarVertical && _scrollBarBottomCap);
        
        _viewportClipBounds.width -= _scrollBarVertical->getRegion().width;
    }

    // Sort controls by Z-Order.
    std::sort(_controls.begin(), _controls.end(), &sortControlsByZOrder);

    GP_ASSERT(_layout);
    if (_scroll != SCROLL_NONE)
        updateScroll();
    else
        _layout->update(this, Vector2::zero());
}

void Container::draw(SpriteBatch* spriteBatch, const Rectangle& clip, bool needsClear, bool cleared, float targetHeight)
{
    if (needsClear)
    {
        GL_ASSERT( glEnable(GL_SCISSOR_TEST) );
        GL_ASSERT( glClearColor(0, 0, 0, 0) );
        float clearY = targetHeight - _clearBounds.y - _clearBounds.height;
        GL_ASSERT( glScissor(_clearBounds.x, clearY,
            _clearBounds.width, _clearBounds.height) );
        GL_ASSERT( glClear(GL_COLOR_BUFFER_BIT) );
        GL_ASSERT( glDisable(GL_SCISSOR_TEST) );

        needsClear = false;
        cleared = true;
    }
    else if (!cleared)
    {
        needsClear = true;
    }

    spriteBatch->begin();
    Control::drawBorder(spriteBatch, clip);
    spriteBatch->end();

    std::vector<Control*>::const_iterator it;
    Rectangle boundsUnion = Rectangle::empty();
    for (it = _controls.begin(); it < _controls.end(); it++)
    {
        Control* control = *it;
        GP_ASSERT(control);
        if (!needsClear || control->isDirty() || control->_clearBounds.intersects(boundsUnion))
        {
            control->draw(spriteBatch, _viewportClipBounds, needsClear, cleared, targetHeight);
            Rectangle::combine(control->_clearBounds, boundsUnion, &boundsUnion);
        }
    }

    if (_scroll != SCROLL_NONE && (_scrollBarOpacity > 0.0f))
    {
        // Draw scroll bars.
        Rectangle clipRegion(_viewportClipBounds);

        spriteBatch->begin();

        if (_scrollBarBounds.height > 0)
        {
            const Rectangle& topRegion = _scrollBarTopCap->getRegion();
            const Theme::UVs& topUVs = _scrollBarTopCap->getUVs();
            Vector4 topColor = _scrollBarTopCap->getColor();
            topColor.w *= _scrollBarOpacity * _opacity;

            const Rectangle& verticalRegion = _scrollBarVertical->getRegion();
            const Theme::UVs& verticalUVs = _scrollBarVertical->getUVs();
            Vector4 verticalColor = _scrollBarVertical->getColor();
            verticalColor.w *= _scrollBarOpacity * _opacity;

            const Rectangle& bottomRegion = _scrollBarBottomCap->getRegion();
            const Theme::UVs& bottomUVs = _scrollBarBottomCap->getUVs();
            Vector4 bottomColor = _scrollBarBottomCap->getColor();
            bottomColor.w *= _scrollBarOpacity * _opacity;

            clipRegion.width += verticalRegion.width;

            Rectangle bounds(_viewportBounds.x + _viewportBounds.width - verticalRegion.width,
                             _viewportBounds.y + _scrollBarBounds.y,
                             topRegion.width, topRegion.height);
            spriteBatch->draw(bounds.x, bounds.y, bounds.width, bounds.height, topUVs.u1, topUVs.v1, topUVs.u2, topUVs.v2, topColor, clipRegion);

            bounds.y += topRegion.height;
            bounds.height = _scrollBarBounds.height - topRegion.height - bottomRegion.height;
            spriteBatch->draw(bounds.x, bounds.y, bounds.width, bounds.height, verticalUVs.u1, verticalUVs.v1, verticalUVs.u2, verticalUVs.v2, verticalColor, clipRegion);

            bounds.y += bounds.height;
            bounds.height = bottomRegion.height;
            spriteBatch->draw(bounds.x, bounds.y, bounds.width, bounds.height, bottomUVs.u1, bottomUVs.v1, bottomUVs.u2, bottomUVs.v2, bottomColor, clipRegion);
        }

        if (_scrollBarBounds.width > 0)
        {
            const Rectangle& leftRegion = _scrollBarLeftCap->getRegion();
            const Theme::UVs& leftUVs = _scrollBarLeftCap->getUVs();
            Vector4 leftColor = _scrollBarLeftCap->getColor();
            leftColor.w *= _scrollBarOpacity * _opacity;

            const Rectangle& horizontalRegion = _scrollBarHorizontal->getRegion();
            const Theme::UVs& horizontalUVs = _scrollBarHorizontal->getUVs();
            Vector4 horizontalColor = _scrollBarHorizontal->getColor();
            horizontalColor.w *= _scrollBarOpacity * _opacity;

            const Rectangle& rightRegion = _scrollBarRightCap->getRegion();
            const Theme::UVs& rightUVs = _scrollBarRightCap->getUVs();
            Vector4 rightColor = _scrollBarRightCap->getColor();
            rightColor.w *= _scrollBarOpacity * _opacity;

            clipRegion.height += horizontalRegion.height;
        
            Rectangle bounds(_viewportBounds.x + _scrollBarBounds.x,
                             _viewportBounds.y + _viewportBounds.height - horizontalRegion.height,
                             leftRegion.width, leftRegion.height);
            spriteBatch->draw(bounds.x, bounds.y, bounds.width, bounds.height, leftUVs.u1, leftUVs.v1, leftUVs.u2, leftUVs.v2, leftColor, clipRegion);

            bounds.x += leftRegion.width;
            bounds.width = _scrollBarBounds.width - leftRegion.width - rightRegion.width;
            spriteBatch->draw(bounds.x, bounds.y, bounds.width, bounds.height, horizontalUVs.u1, horizontalUVs.v1, horizontalUVs.u2, horizontalUVs.v2, horizontalColor, clipRegion);

            bounds.x += bounds.width;
            bounds.width = rightRegion.width;
            spriteBatch->draw(bounds.x, bounds.y, bounds.width, bounds.height, rightUVs.u1, rightUVs.v1, rightUVs.u2, rightUVs.v2, rightColor, clipRegion);
        }

        spriteBatch->end();

        if (_scrollingVelocity.isZero())
        {
            _dirty = false;
        }
    }
    else
    {
        _dirty = false;
    }
}

bool Container::isDirty()
{
    if (_dirty)
    {
        return true;
    }
    else
    {
        std::vector<Control*>::const_iterator it;
        for (it = _controls.begin(); it < _controls.end(); it++)
        {
            GP_ASSERT(*it);
            if ((*it)->isDirty())
            {
                return true;
            }
        }
    }

    return false;
}

bool Container::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    if (!isEnabled())
    {
        return false;
    }

    bool eventConsumed = false;
    const Theme::Border& border = getBorder(_state);
    const Theme::Padding& padding = getPadding();
    float xPos = border.left + padding.left;
    float yPos = border.top + padding.top;

    Vector2* offset = NULL;
    if (_scroll != SCROLL_NONE)
    {
        offset = &_scrollPosition;
    }

    std::vector<Control*>::const_iterator it;
    for (it = _controls.begin(); it < _controls.end(); it++)
    {
        Control* control = *it;
        GP_ASSERT(control);
        if (!control->isEnabled())
        {
            continue;
        }

        const Rectangle& bounds = control->getBounds();
        float boundsX = bounds.x;
        float boundsY = bounds.y;
        if (offset)
        {
            boundsX += offset->x;
            boundsY += offset->y;
        }

        if (control->getState() != Control::NORMAL ||
            (evt == Touch::TOUCH_PRESS &&
                x >= xPos + boundsX &&
                x <= xPos + boundsX + bounds.width &&
                y >= yPos + boundsY &&
                y <= yPos + boundsY + bounds.height))
        {
            // Pass on the event's clip relative to the control.
            eventConsumed |= control->touchEvent(evt, x - xPos - boundsX, y - yPos - boundsY, contactIndex);
        }
    }

    if (!isEnabled())
    {
        return (_consumeTouchEvents | eventConsumed);
    }

    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        setState(Control::FOCUS);
        break;
    case Touch::TOUCH_RELEASE:
        setState(Control::NORMAL);
        break;
    }

    if (!eventConsumed && _scroll != SCROLL_NONE)
    {
        if (touchEventScroll(evt, x - xPos, y - yPos, contactIndex))
        {
            _dirty = true;
        }
    }

    return (_consumeTouchEvents | eventConsumed);
}

void Container::keyEvent(Keyboard::KeyEvent evt, int key)
{
    std::vector<Control*>::const_iterator it;
    for (it = _controls.begin(); it < _controls.end(); it++)
    {
        Control* control = *it;
        GP_ASSERT(control);
        if (!control->isEnabled())
        {
            continue;
        }

        if (control->isContainer() || control->getState() == Control::FOCUS)
        {
            control->keyEvent(evt, key);
        }
    }
}

bool Container::isContainer()
{
    return true;
}

Layout::Type Container::getLayoutType(const char* layoutString)
{
    if (!layoutString)
    {
        return Layout::LAYOUT_ABSOLUTE;
    }

    std::string layoutName(layoutString);
    std::transform(layoutName.begin(), layoutName.end(), layoutName.begin(), (int(*)(int))toupper);
    if (layoutName == "LAYOUT_ABSOLUTE")
    {
        return Layout::LAYOUT_ABSOLUTE;
    }
    else if (layoutName == "LAYOUT_VERTICAL")
    {
        return Layout::LAYOUT_VERTICAL;
    }
    else if (layoutName == "LAYOUT_FLOW")
    {
        return Layout::LAYOUT_FLOW;
    }
    else if (layoutName == "LAYOUT_SCROLL")
    {
        return Layout::LAYOUT_SCROLL;
    }
    else
    {
        // Default.
        return Layout::LAYOUT_ABSOLUTE;
    }
}

void Container::updateScroll()
{
    // Update Time.
    static long lastFrameTime = Game::getGameTime();
    long frameTime = Game::getGameTime();
    long elapsedTime = (frameTime - lastFrameTime);
    lastFrameTime = frameTime;

    const Theme::Border& containerBorder = getBorder(_state);
    const Theme::Padding& containerPadding = getPadding();

    // Calculate total width and height.
    float totalWidth = 0;
    float totalHeight = 0;
    std::vector<Control*> controls = getControls();
    unsigned int controlsCount = controls.size();
    for (unsigned int i = 0; i < controlsCount; i++)
    {
        Control* control = controls.at(i);

        const Rectangle& bounds = control->getBounds();
        const Theme::Margin& margin = control->getMargin();

        float newWidth = bounds.x + bounds.width;
        if (newWidth > totalWidth)
        {
            totalWidth = newWidth;
        }

        float newHeight = bounds.y + bounds.height;
        if (newHeight > totalHeight)
        {
            totalHeight = newHeight;
        }
    }

    float vWidth = getImageRegion("verticalScrollBar", _state).width;
    float hHeight = getImageRegion("horizontalScrollBar", _state).height;
    float clipWidth = _bounds.width - containerBorder.left - containerBorder.right - containerPadding.left - containerPadding.right - vWidth;
    float clipHeight = _bounds.height - containerBorder.top - containerBorder.bottom - containerPadding.top - containerPadding.bottom - hHeight;

    // Apply and dampen inertia.
    if (!_scrolling && !_scrollingVelocity.isZero())
    {
        // Calculate the time passed since last update.
        float elapsedSecs = (float)elapsedTime * 0.001f;

        _scrollPosition.x += _scrollingVelocity.x * elapsedSecs;
        _scrollPosition.y += _scrollingVelocity.y * elapsedSecs;

        float dampening = 1.0f - _scrollingFriction * SCROLL_FRICTION_FACTOR * elapsedSecs;
        _scrollingVelocity.x *= dampening;
        _scrollingVelocity.y *= dampening;

        if (fabs(_scrollingVelocity.x) < 100.0f)
            _scrollingVelocity.x = 0.0f;
        if (fabs(_scrollingVelocity.y) < 100.0f)
            _scrollingVelocity.y = 0.0f;
    }

    // Stop scrolling when the far edge is reached.
    if (-_scrollPosition.x > totalWidth - clipWidth)
    {
        _scrollPosition.x = -(totalWidth - clipWidth);
        _scrollingVelocity.x = 0;
    }
    
    if (-_scrollPosition.y > totalHeight - clipHeight)
    {
        _scrollPosition.y = -(totalHeight - clipHeight);
        _scrollingVelocity.y = 0;
    }

    if (_scrollPosition.x > 0)
    {
        _scrollPosition.x = 0;
        _scrollingVelocity.x = 0;
    }

    if (_scrollPosition.y > 0)
    {
        _scrollPosition.y = 0;
        _scrollingVelocity.y = 0;
    }

    float scrollWidth = 0;
    if (clipWidth < totalWidth)
        scrollWidth = (clipWidth / totalWidth) * clipWidth;

    float scrollHeight = 0;
    if (clipHeight < totalHeight)
        scrollHeight = (clipHeight / totalHeight) * clipHeight;

    _scrollBarBounds.set(((-_scrollPosition.x) / totalWidth) * clipWidth,
                         ((-_scrollPosition.y) / totalHeight) * clipHeight,
                         scrollWidth, scrollHeight);

    // If scroll velocity is 0 and scrollbars are not always visible, trigger fade-out animation.
    if (!_scrolling && _scrollingVelocity.isZero() && _scrollBarsAutoHide && _scrollBarOpacity == 1.0f)
    {
        float to = 0;
        _scrollBarOpacity = 0.99f;
        Animation* animation = createAnimationFromTo("scrollbar-fade-out", ANIMATE_OPACITY, &_scrollBarOpacity, &to, Curve::QUADRATIC_IN_OUT, 500L);
        _scrollBarOpacityClip = animation->getClip();
        _scrollBarOpacityClip->play();
    }

    // Position controls within scroll area.
    _layout->update(this, _scrollPosition);
}

bool Container::touchEventScroll(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch(evt)
    {
    case Touch::TOUCH_PRESS:
        _scrollingLastX = _scrollingFirstX = x;
        _scrollingLastY = _scrollingFirstY = y;
        _scrollingVelocity.set(0, 0);
        _scrolling = true;
        _scrollingStartTimeX = _scrollingStartTimeY = 0;
        
        if (_scrollBarOpacityClip && _scrollBarOpacityClip->isPlaying())
        {
            _scrollBarOpacityClip->stop();
            _scrollBarOpacityClip = NULL;
        }
        _scrollBarOpacity = 1.0f;
        return true;

    case Touch::TOUCH_MOVE:
        if (_scrolling)
        {
            // Calculate the latest movement delta for the next update to use.
            int vx = x - _scrollingLastX;
            int vy = y - _scrollingLastY;
            _scrollingVelocity.set(vx, vy);
            _scrollPosition.x += vx;
            _scrollPosition.y += vy;
            _scrollingLastX = x;
            _scrollingLastY = y;

            // If the user changes direction, reset the start time and position.
            bool goingRight = (vx > 0);
            if (goingRight != _scrollingRight)
            {
                _scrollingFirstX = x;
                _scrollingRight = goingRight;
                _scrollingStartTimeX = Game::getAbsoluteTime();
            }

            bool goingDown = (vy > 0);
            if (goingDown != _scrollingDown)
            {
                _scrollingFirstY = y;
                _scrollingDown = goingDown;
                _scrollingStartTimeY = Game::getAbsoluteTime();
            }

            if (!_scrollingStartTimeX)
                _scrollingStartTimeX = Game::getAbsoluteTime();

            if (!_scrollingStartTimeY)
                _scrollingStartTimeY = Game::getAbsoluteTime();

            _scrollingLastTime = Game::getAbsoluteTime();

            return true;
        }
        break;

    case Touch::TOUCH_RELEASE:
        _scrolling = false;
        long timeSinceLastMove = Game::getAbsoluteTime() - _scrollingLastTime;
        if (timeSinceLastMove > SCROLL_INERTIA_DELAY)
        {
            _scrollingVelocity.set(0, 0);
            return true;
        }

        int dx = _scrollingLastX - _scrollingFirstX;
        int dy = _scrollingLastY - _scrollingFirstY;

        long timeTakenX = Game::getAbsoluteTime() - _scrollingStartTimeX;
        float elapsedSecsX = (float)timeTakenX * 0.001f;
        long timeTakenY = Game::getAbsoluteTime() - _scrollingStartTimeY;
        float elapsedSecsY = (float)timeTakenY * 0.001f;

        float vx = dx;
        float vy = dy;
        if (elapsedSecsX > 0)
            vx = (float)dx / elapsedSecsX;
        if (elapsedSecsY > 0)
            vy = (float)dy / elapsedSecsY;

        _scrollingVelocity.set(vx, vy);

        return true;
    }

    return false;
}

Container::Scroll Container::getScroll(const char* scroll)
{
    if (!scroll)
        return Container::SCROLL_NONE;

    if (strcmp(scroll, "SCROLL_NONE") == 0)
    {
        return Container::SCROLL_NONE;
    }
    else if (strcmp(scroll, "SCROLL_HORIZONTAL") == 0)
    {
        return Container::SCROLL_HORIZONTAL;
    }
    else if (strcmp(scroll, "SCROLL_VERTICAL") == 0)
    {
        return Container::SCROLL_VERTICAL;
    }
    else if (strcmp(scroll, "SCROLL_BOTH") == 0)
    {
        return Container::SCROLL_BOTH;
    }
    else
    {
        GP_ERROR("Failed to get corresponding scroll state for unsupported value '%s'.", scroll);
    }

    return Container::SCROLL_NONE;
}

bool sortControlsByZOrder(Control* c1, Control* c2)
{
    if (c1->getZIndex() < c2->getZIndex())
        return true;

    return false;
}

unsigned int Container::getAnimationPropertyComponentCount(int propertyId) const
{
    switch(propertyId)
    {
    case ANIMATE_OPACITY:
        return 1;
    default:
        return Control::getAnimationPropertyComponentCount(propertyId);
    }
}

void Container::getAnimationPropertyValue(int propertyId, AnimationValue* value)
{
    GP_ASSERT(value);

    switch(propertyId)
    {
    case ANIMATE_OPACITY:
        value->setFloat(0, _scrollBarOpacity);
        break;
    default:
        Control::getAnimationPropertyValue(propertyId, value);
        break;
    }
}

void Container::setAnimationPropertyValue(int propertyId, AnimationValue* value, float blendWeight)
{
    GP_ASSERT(value);

    switch(propertyId)
    {
    case ANIMATE_OPACITY:
        _scrollBarOpacity = Curve::lerp(blendWeight, _opacity, value->getFloat(0));
        _dirty = true;
        break;
    default:
        Control::setAnimationPropertyValue(propertyId, value, blendWeight);
        break;
    }
}

}
