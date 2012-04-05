#ifndef FORM_H_
#define FORM_H_

#include "Ref.h"
#include "Container.h"
#include "Mesh.h"
#include "Node.h"
#include "FrameBuffer.h"
#include "Touch.h"
#include "Keyboard.h"

namespace gameplay
{

class Theme;

/**
 * Top-level container of UI controls.
 */
class Form : public Container
{
    friend class Platform;

public:

    /**
     * Create from properties file.
     * The top-most namespace in the file must be named 'form'.  The following properties are available for forms:
     *
     * form <Form ID>
     * {
     *      // Form properties.
     *      theme    = <Path to Theme File> // See Theme.h.
     *      layout   = <Layout Type>        // A value from the Layout::Type enum.  E.g.: LAYOUT_VERTICAL
     *      style    = <Style ID>           // A style from the referenced theme.
     *      position = <x, y>               // Position of the form on-screen, measured in pixels.
     *      size     = <width, height>      // Size of the form, measured in pixels.
     *   
     *      // All the controls within this form.
     *      container { }
     *      label { }
     *      textBox { }
     *      button { }
     *      checkBox { }
     *      radioButton { }
     *      slider { }
     * }
     *
     * @param path Path to the properties file to create a new form from.
     */
    static Form* create(const char* path);

    /**
     * Get a form from its ID.
     *
     * @param id The ID of the form to search for.
     *
     * @return A form with the given ID, or null if one was not found.
     */
    static Form* getForm(const char* id);

    /**
     * Create a 3D quad to texture with this Form.
     *
     * The specified points should describe a triangle strip with the first 3 points
     * forming a triangle wound in counter-clockwise direction, with the second triangle
     * formed from the last three points in clockwise direction.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     * @param p3 The third point.
     * @param p4 The fourth point.
     */
    void setQuad(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4);

    /**
     * Create a 2D quad to texture with this Form.
     *
     * @param x The x coordinate.
     * @param y The y coordinate.
     * @param width The width of the quad.
     * @param height The height of the quad.
     */
    void setQuad(float x, float y, float width, float height);

    /**
     * Attach this form to a node.
     *
     * A form can be drawn as part of the 3-dimensional world if it is attached to a node.
     * The form's contents will be rendered into a framebuffer which will be used to texture a quad.
     * This quad will be given the same dimensions as the form and must be transformed appropriately.
     * Alternatively, a quad can be set explicitly on a form with the setQuad() methods.
     *
     * @param node The node to attach this form to.
     */
    void setNode(Node* node);

    /**
     * Updates each control within this form, and positions them according to its layout.
     */
    void update();

    /**
     * Draws this form.
     */
    void draw();

private:
    
    /**
     * Constructor.
     */
    Form();

    /**
     * Constructor.
     */
    Form(const Form& copy);

    /**
     * Destructor.
     */
    virtual ~Form();

    /**
     * Initialize a quad for this form in order to draw it in 3D.
     *
     * @param mesh The mesh to create a model from.
     */
    void initializeQuad(Mesh* mesh);

    /**
     * Draw this form into the current framebuffer.
     *
     * @param spriteBatch The sprite batch containing this form's theme texture.
     * @param clip The form's clipping rectangle.
     */
    void draw(SpriteBatch* spriteBatch, const Rectangle& clip);

    /**
     * Propagate touch events to enabled forms.
     *
     * @return Whether the touch event was consumed by a form.
     */
    static bool touchEventInternal(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    /**
     * Propagate key events to enabled forms.
     */
    static void keyEventInternal(Keyboard::KeyEvent evt, int key);

    Theme* _theme;              // The Theme applied to this Form.
    Model* _quad;               // Quad for rendering this Form in world-space.
    Node* _node;                // Node for transforming this Form in world-space.
    FrameBuffer* _frameBuffer;  // FBO the Form is rendered into for texturing the quad.
    Matrix _projectionMatrix;   // Orthographic projection matrix to be set on SpriteBatch objects when rendering into the FBO.
};

}

#endif