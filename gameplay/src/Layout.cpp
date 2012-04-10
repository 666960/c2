#include "Base.h"
#include "Layout.h"
#include "Control.h"
#include "Container.h"

namespace gameplay
{
    void Layout::align(Control* control, const Container* container)
    {
        if (control->_alignment != Control::ALIGN_TOP_LEFT ||
            control->_autoWidth || control->_autoHeight)
        {
            Rectangle controlBounds = control->getBounds();
            const Rectangle& containerBounds = container->getClip();
            const Theme::Border& containerBorder = container->getBorder(container->getState());
            const Theme::Padding& containerPadding = container->getPadding();

            if (control->_autoWidth)
            {
                controlBounds.width = containerBounds.width;
            }

            if (control->_autoHeight)
            {
                controlBounds.height = containerBounds.height;
            }

            // Vertical alignment
            if ((control->_alignment & Control::ALIGN_BOTTOM) == Control::ALIGN_BOTTOM)
            {
                controlBounds.y = containerBounds.height - controlBounds.height;
            }
            else if ((control->_alignment & Control::ALIGN_VCENTER) == Control::ALIGN_VCENTER)
            {
                controlBounds.y = containerBounds.height * 0.5f - controlBounds.height * 0.5f;
            }

            // Horizontal alignment
            if ((control->_alignment & Control::ALIGN_RIGHT) == Control::ALIGN_RIGHT)
            {
                controlBounds.x = containerBounds.width - controlBounds.width;
            }
            else if ((control->_alignment & Control::ALIGN_HCENTER) == Control::ALIGN_HCENTER)
            {
                controlBounds.x = containerBounds.width * 0.5f - controlBounds.width * 0.5f;
            }

            control->setBounds(controlBounds);
        }
    }
}