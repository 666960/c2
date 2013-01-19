#ifndef TERRAINTEST_H_
#define TERRAINTEST_H_

#include "gameplay.h"
#include "Test.h"

using namespace gameplay;

class TerrainTest : public Test, public Control::Listener
{
public:

	TerrainTest();

	~TerrainTest();

    void keyEvent(Keyboard::KeyEvent evt, int key);

    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);

    void controlEvent(Control* control, EventType evt);

protected:

    void initialize();

    void finalize();

    void update(float elapsedTime);

    void render(float elapsedTime);

	bool drawScene(Node* node);

    void setMessage(const char* message);

private:

    enum Mode
    {
        MODE_LOOK,
        MODE_DROP_SPHERE,
        MODE_DROP_BOX
    };

	Font* _font;
	Scene* _scene;
	Terrain* _terrain;
    Form* _form;
    bool _formVisible;
    Node* _sky;
    Vector2 _formSize;
	bool _wireframe;
    bool _debugPhysics;
	bool _snapToGround;
    bool _vsync;
    Mode _mode;
    Node* _sphere;
    Node* _box;
    std::list<Node*> _shapes;

};

#endif