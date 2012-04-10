#ifndef BUTTON_H_
#define BUTTON_H_

#include "Label.h"
#include "Touch.h"
#include "Theme.h"
#include "Properties.h"

namespace gameplay
{

/**
 * Defines a button UI control. This is essentially a label that can have a callback method set on it.
 *
 * The following properties are available for buttons:
 *
 * button <buttonID>
 * {
 *      style       = <styleID>
 *      alignment   = <Control::Alignment constant> // Note: 'position' will be ignored.
 *      position    = <x, y>
 *      autoWidth   = <bool>
 *      autoHeight  = <bool>
 *      size        = <width, height>
 *      width       = <width>   // Can be used in place of 'size', e.g. with 'autoHeight = true'
 *      height      = <height>  // Can be used in place of 'size', e.g. with 'autoWidth = true'
 *      text        = <string>
 * }
 */
class Button : public Label
{
    friend class Container;

protected:

    /**
     * Constructor.
     */
    Button();

    /**
     * Destructor.
     */
    virtual ~Button();

    /**
     * Create a button with a given style and properties.
     *
     * @param style The style to apply to this button.
     * @param properties The properties to set on this button.
     *
     * @return The new button.
     */
    static Button* create(Theme::Style* style, Properties* properties);

    /**
     * Touch callback on touch events.  Controls return true if they consume the touch event.
     *
     * @param evt The touch event that occurred.
     * @param x The x position of the touch in pixels. Left edge is zero.
     * @param y The y position of the touch in pixels. Top edge is zero.
     * @param contactIndex The order of occurrence for multiple touch contacts starting at zero.
     *
     * @return Whether the touch event was consumed by the control.
     *
     * @see Touch::TouchEvent
     */
    bool touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

private:

    /*
     * Constructor.
     */
    Button(const Button& copy);
};

}

#endif