#ifndef GAME_H_
#define GAME_H_

#include "Keyboard.h"
#include "Touch.h"
#include "AudioController.h"
#include "AnimationController.h"
#include "PhysicsController.h"
#include "Vector4.h"

namespace gameplay
{

/**
 * Defines the basic game initialization, logic and platform delegates.
 */
class Game
{
public:

    /**
     * The game states.
     */
    enum State
    {
        UNINITIALIZED,
        RUNNING,
        PAUSED
    };

    /**
     * Flags used when clearing the active frame buffer targets.
     */
    enum ClearFlags
    {
        CLEAR_COLOR = GL_COLOR_BUFFER_BIT,
        CLEAR_DEPTH = GL_DEPTH_BUFFER_BIT,
        CLEAR_STENCIL = GL_STENCIL_BUFFER_BIT,
        CLEAR_COLOR_DEPTH = CLEAR_COLOR | CLEAR_DEPTH,
        CLEAR_COLOR_STENCIL = CLEAR_COLOR | CLEAR_STENCIL,
        CLEAR_DEPTH_STENCIL = CLEAR_DEPTH | CLEAR_STENCIL,
        CLEAR_COLOR_DEPTH_STENCIL = CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL
    };

    /**
     * Destructor.
     */
    virtual ~Game();

    /**
     * Gets the single instance of the game.
     * 
     * @return The single instance of the game.
     */
    static Game* getInstance();

    /**
     * Gets whether vertical sync is enabled for the game display.
     * 
     * @return true if vsync is enabled; false if not.
     */
    static bool isVsync();

    /**
     * Sets whether vertical sync is enabled for the game display.
     *
     * @param enable true if vsync is enabled; false if not.
     */
    static void setVsync(bool enable);

    /**
     * Gets the total absolute running time (in milliseconds) since Game::run().
     * 
     * @return The total absolute running time (in milliseconds).
     */
    static long getAbsoluteTime();

    /**
     * Gets the total game time (in milliseconds). This is the total accumulated game time (unpaused).
     *
     * You would typically use things in your game that you want to stop when the game is paused.
     * This includes things such as game physics and animation.
     * 
     * @return The total game time (in milliseconds).
     */
    static long getGameTime();

    /**
     * Gets the game state.
     *
     * @return The current game state.
     */
    inline State getState() const;

    /**
     * Call this method to initialize the game, and begin running the game.
     *
     * @param width The width of the game window to run at.
     * @param height The height of the game window to run at.
     * 
     * @return Zero for normal termination, or non-zero if an error occurred.
     */
    int run(int width = -1, int height = -1);

    /**
     * Pauses the game after being run.
     */
    void pause();

    /**
     * Resumes the game after being paused.
     */
    void resume();

    /**
     * Exits the game.
     */
    void exit();

    /**
     * Platform frame delagate.
     *
     * This is called every frame from the platform.
     * This in turn calls back on the user implemented game methods: update() then render()
     */
    void frame();

    /**
     * Gets the current frame rate.
     * 
     * @return The current frame rate.
     */
    inline unsigned int getFrameRate() const;

    /**
     * Gets the game window width.
     * 
     * @return The game window width.
     */
    inline unsigned int getWidth() const;

    /**
     * Gets the game window height.
     * 
     * @return The game window height.
     */
    inline unsigned int getHeight() const;

    /**
     * Clears the specified resource buffers to the specified clear values. 
     *
     * @param flags The flags indicating which buffers to be cleared.
     * @param clearColor The color value to clear to when the flags includes the color buffer.
     * @param clearDepth The depth value to clear to when the flags includes the color buffer.
     * @param clearStencil The stencil value to clear to when the flags includes the color buffer.
     */
    void clear(ClearFlags flags, const Vector4& clearColor, float clearDepth, int clearStencil);

    /**
     * Gets the audio controller for managing control of audio
     * associated with the game.
     *
     * @return The audio controller for this game.
     */
    inline AudioController* getAudioController() const;

    /**
     * Gets the animation controller for managing control of animations
     * associated with the game.
     * 
     * @return The animation controller for this game.
     */
    inline AnimationController* getAnimationController() const;

    /**
     * Gets the physics controller for managing control of physics
     * associated with the game.
     * 
     * @return The physics controller for this game.
     */
    inline PhysicsController* getPhysicsController() const;

    /**
     * Menu callback on menu events.
     */
    virtual void menu();

    /**
     * Keyboard callback on keyPress events.
     *
     * @param evt The key event that occured.
     * @param key The key code that was pressed, released or repeated.
     * 
     * @see Keyboard::Key
     */
    virtual void keyEvent(Keyboard::KeyEvent evt, int key);

    /**
     * Touch callback on touch events.
     *
     * @param evt The touch event that occurred.
     *
     * @see Touch::TouchEvent
     */
    virtual void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    /**
     * Sets muli-touch is to be enabled/disabled. Default is disabled.
     *
     * @param enabled true sets multi-touch is enabled, false to be disabled.
     */
    inline void setMultiTouch(bool enabled);

    /**
     * Is multi-touch mode enabled.
     *
     * @return true is multi-touch is enabled.
     */
    inline bool isMultiTouch() const;

    /**
     * Gets the current accelerometer values.
     *
     * @param pitch The pitch angle returned (in degrees). If NULL then not returned.
     * @param roll The roll angle returned (in degrees). If NULL then not returned.
     */
    inline void getAccelerometerValues(float* pitch, float* roll);

protected:

    /**
     * Constructor.
     */
    Game();

    /**
     * Initialize callback that is called just before the first frame when the game starts.
     */
    virtual void initialize() = 0;

    /**
     * Finalize callback that is called when the game on exits.
     */
    virtual void finalize() = 0;

    /**
     * Update callback for handling update routines.
     *
     * Called just before render, once per frame when game is running.
     * Ideal for non-render code and game logic such as input and animation.
     *
     * @param elapsedTime The elapsed game time.
     */
    virtual void update(long elapsedTime) = 0;

    /**
     * Render callback for handling rendering routines.
     *
     * Called just after update, once per frame when game is running.
     * Ideal for all rendering code.
     *
     * @param elapsedTime The elapsed game time.
     */
    virtual void render(long elapsedTime) = 0;

    /**
     * Renders a single frame once and then swaps it to the display.
     *
     * This is useful for rendering splash screens.
     */
    template <class T>
    void renderOnce(T* instance, void (T::*method)(void*), void* cookie);

private:

    /**
     * Constructor.
     *
     * @param copy The game to copy.
     */
    Game(const Game& copy);

    /**
     * Starts core game.
     */
    bool startup();

    /**
     * Shuts down the game.
     */
    void shutdown();

    bool _initialized;                          // If game has initialized yet.
    State _state;                               // The game state.
    static long _pausedTimeLast;                // The last time paused.
    static long _pausedTimeTotal;               // The total time paused.
    long _frameLastFPS;                         // The last time the frame count was updated.
    unsigned int _frameCount;                   // The current frame count.
    unsigned int _frameRate;                    // The current frame rate.
    unsigned int _width;                        // The game's display width.
    unsigned int _height;                       // The game's display height.
    Vector4 _clearColor;                        // The clear color value last used for clearing the color buffer.
    float _clearDepth;                          // The clear depth value last used for clearing the depth buffer.
    int _clearStencil;                          // The clear stencil value last used for clearing the stencil buffer.
    AnimationController* _animationController;  // Controls the scheduling and running of animations.
    AudioController* _audioController;          // Controls audio sources that are playing in the game.
    PhysicsController* _physicsController;      // Controls the simulation of a physics scene and entities.
};

}

#include "Game.inl"

#endif
