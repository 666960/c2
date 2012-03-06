#include "TextBox.h"
#include "Game.h"

namespace gameplay
{

static std::vector<TextBox*> __textBoxes;

TextBox::TextBox()
{
}

TextBox::TextBox(const TextBox& copy)
{
}

TextBox::~TextBox()
{
}

TextBox* TextBox::create(Theme::Style* style, Properties* properties)
{
    TextBox* textBox = new TextBox();
    textBox->init(style, properties);
    __textBoxes.push_back(textBox);

    return textBox;
}

TextBox* TextBox::getTextBox(const char* id)
{
    std::vector<TextBox*>::const_iterator it;
    for (it = __textBoxes.begin(); it < __textBoxes.end(); it++)
    {
        TextBox* t = *it;
        if (strcmp(id, t->getID()) == 0)
        {
            return t;
        }
    }

    return NULL;
}

void TextBox::setCursorLocation(int x, int y)
{
    Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
    Theme::ContainerRegion* containerRegion = overlay->getContainerRegion();
    Theme::Border border;
    if (containerRegion)
    {
        border = containerRegion->getBorder();
    }
    Theme::Padding padding = _style->getPadding();

    _cursorLocation.set(x - border.left - padding.left + _clip.x,
                       y - border.top - padding.top + _clip.y);
}

bool TextBox::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{   
    if (!isEnabled())
    {
        return false;
    }

    switch (evt)
    {
    case Touch::TOUCH_PRESS: 
        if (_state == NORMAL)
        {
            _state = ACTIVE;
            Game::getInstance()->displayKeyboard(true);
            _dirty = true;
            return _consumeTouchEvents;
        }
        else if (!(x > 0 && x <= _bounds.width &&
                    y > 0 && y <= _bounds.height))
        {
            _state = NORMAL;
            Game::getInstance()->displayKeyboard(false);
            _dirty = true;
            return _consumeTouchEvents;
        }
        break;
    case Touch::TOUCH_MOVE:
        if (_state == FOCUS &&
            x > 0 && x <= _bounds.width &&
            y > 0 && y <= _bounds.height)
        {
            setCursorLocation(x, y);
            _dirty = true;
            return _consumeTouchEvents;
        }
        break;
    case Touch::TOUCH_RELEASE:
        if (x > 0 && x <= _bounds.width &&
            y > 0 && y <= _bounds.height)
        {
            setCursorLocation(x, y);
            _state = FOCUS;
            _dirty = true;
            return _consumeTouchEvents;
        }
        break;
    }

    return _consumeTouchEvents;
}

void TextBox::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (_state == FOCUS)
    {
        switch (evt)
        {
            case Keyboard::KEY_PRESS:
            {
                switch (key)
                {
                    case Keyboard::KEY_HOME:
                    {
                        // TODO: Move cursor to beginning of line.
                        // This only works for left alignment...
                        
                        //_cursorLocation.x = _clip.x;
                        //_dirty = true;
                        break;
                    }
                    case Keyboard::KEY_END:
                    {
                        // TODO: Move cursor to end of line.
                        break;
                    }
                    case Keyboard::KEY_DELETE:
                    {
                        Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
                        Font* font = overlay->getFont();
                        unsigned int fontSize = overlay->getFontSize();
                        unsigned int textIndex = font->getIndexAtLocation(_text.c_str(), _clip, fontSize, _cursorLocation, &_cursorLocation,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());

                        _text.erase(textIndex, 1);
                        font->getLocationAtIndex(_text.c_str(), _clip, fontSize, &_cursorLocation, textIndex,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());
                        _dirty = true;
                        break;
                    }
                    case Keyboard::KEY_LEFT_ARROW:
                    {
                        Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
                        Font* font = overlay->getFont();
                        unsigned int fontSize = overlay->getFontSize();
                        unsigned int textIndex = font->getIndexAtLocation(_text.c_str(), _clip, fontSize, _cursorLocation, &_cursorLocation,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());

                        font->getLocationAtIndex(_text.c_str(), _clip, fontSize, &_cursorLocation, textIndex - 1,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());
                        _dirty = true;
                        break;
                    }
                    case Keyboard::KEY_RIGHT_ARROW:
                    {
                        Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
                        Font* font = overlay->getFont();
                        unsigned int fontSize = overlay->getFontSize();
                        unsigned int textIndex = font->getIndexAtLocation(_text.c_str(), _clip, fontSize, _cursorLocation, &_cursorLocation,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());

                        font->getLocationAtIndex(_text.c_str(), _clip, fontSize, &_cursorLocation, textIndex + 1,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());
                        _dirty = true;
                        break;
                    }
                    case Keyboard::KEY_UP_ARROW:
                    {
                        Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
                        Font* font = overlay->getFont();
                        unsigned int fontSize = overlay->getFontSize();

                        _cursorLocation.y -= fontSize;
                        font->getIndexAtLocation(_text.c_str(), _clip, fontSize, _cursorLocation, &_cursorLocation,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());
                        _dirty = true;
                        break;
                    }
                    case Keyboard::KEY_DOWN_ARROW:
                    {
                        Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
                        Font* font = overlay->getFont();
                        unsigned int fontSize = overlay->getFontSize();

                        _cursorLocation.y += fontSize;
                        font->getIndexAtLocation(_text.c_str(), _clip, fontSize, _cursorLocation, &_cursorLocation,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());
                        _dirty = true;
                        break;
                    }
                }
                break;
            }

            case Keyboard::KEY_CHAR:
            {
                Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
                Font* font = overlay->getFont();
                unsigned int fontSize = overlay->getFontSize();
                unsigned int textIndex = font->getIndexAtLocation(_text.c_str(), _clip, fontSize, _cursorLocation, &_cursorLocation,
                    overlay->getTextAlignment(), true, overlay->getTextRightToLeft());

                switch (key)
                {
                    case Keyboard::KEY_BACKSPACE:
                    {
                        if (textIndex > 0)
                        {
                            --textIndex;
                            _text.erase(textIndex, 1);
                            font->getLocationAtIndex(_text.c_str(), _clip, fontSize, &_cursorLocation, textIndex,
                                overlay->getTextAlignment(), true, overlay->getTextRightToLeft());

                            _dirty = true;
                        }
                        break;
                    }
                    case Keyboard::KEY_RETURN:
                        // TODO: Handle line-break insertion correctly.
                        break;
                    default:
                    {
                        // Insert character into string.
                        _text.insert(textIndex, 1, (char)key);

                        // Get new location of cursor.
                        font->getLocationAtIndex(_text.c_str(), _clip, fontSize, &_cursorLocation, textIndex + 1,
                            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());
                
                        _dirty = true;
                        break;
                    }
            
                    break;
                }
            }
        }
    }
}

void TextBox::update(const Rectangle& clip)
{
    /*
    Vector2 pos(clip.x + _position.x, clip.y + _position.y);
    Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
    Theme::Border border;
    Theme::ContainerRegion* containerRegion = overlay->getContainerRegion();
    if (containerRegion)
    {
        border = containerRegion->getBorder();
    }
    Theme::Padding padding = _style->getPadding();

    // Set up the text viewport.
    Font* font = overlay->getFont();
    _clip.set(pos.x + border.left + padding.left,
                  pos.y + border.top + padding.top,
                  _size.x - border.left - padding.left - border.right - padding.right,
                  _size.y - border.top - padding.top - border.bottom - padding.bottom - overlay->getFontSize());

                  */

    Control::update(clip);

    // Get index into string and cursor location from the last recorded touch location.
    if (_state == FOCUS)
    {
        Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
        Font* font = overlay->getFont();
        font->getIndexAtLocation(_text.c_str(), _clip, overlay->getFontSize(), _cursorLocation, &_cursorLocation,
            overlay->getTextAlignment(), true, overlay->getTextRightToLeft());
    }
}

void TextBox::drawSprites(SpriteBatch* spriteBatch, const Rectangle& clip)
{
    if (_state == FOCUS)
    {
        // Draw the cursor at its current location.
        Theme::Style::Overlay* overlay = _style->getOverlay(getOverlayType());
        Theme::Cursor* cursor = overlay->getTextCursor();
        if (cursor)
        {
            Theme::Border border;
            Theme::ContainerRegion* containerRegion = overlay->getContainerRegion();
            if (containerRegion)
            {
                border = containerRegion->getBorder();
            }
            const Theme::Padding padding = _style->getPadding();
            const Vector2 size = cursor->getSize();
            const Vector4 color = cursor->getColor();
            const Theme::UVs uvs = cursor->getUVs();
            unsigned int fontSize = overlay->getFontSize();

            spriteBatch->draw(_cursorLocation.x - (size.x / 2.0f), _cursorLocation.y, size.x, fontSize, uvs.u1, uvs.v1, uvs.u2, uvs.v2, color);
        }
    }

    _dirty = false;
}

}