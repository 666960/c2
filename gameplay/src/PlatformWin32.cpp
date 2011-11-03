/*
 * PlatformWin32.cpp
 */
#ifdef WIN32

#include "Base.h"
#include "Platform.h"
#include "FileSystem.h"
#include "Game.h"

using namespace std;

static long __timeTicksPerMillis;
static long __timeStart;
static long __timeAbsolute;
static bool __vsync = WINDOW_VSYNC;
static float __roll;
static float __pitch;
static HWND __hwnd = 0;
static HINSTANCE __hinstance = 0;
static HDC __hdc = 0;
static HGLRC __hrc = 0;

// Gets the gameplay::Input::Key enumeration constant that corresponds
// to the given key and shift modifier combination.
static gameplay::Input::Key getGameplayInputKey(WPARAM win32KeyCode, bool shiftDown)
{
    // TODO: Handle the following keys
    //gameplay::Input::KEY_SYSREQ
    //gameplay::Input::KEY_BREAK
    //gameplay::Input::KEY_MENU
    //gameplay::Input::KEY_KP_ENTER

    switch (win32KeyCode)
    {
    case VK_PAUSE:
        return gameplay::Input::KEY_PAUSE;
    case VK_SCROLL:
        return gameplay::Input::KEY_SCROLL_LOCK;
    case VK_PRINT:
        return gameplay::Input::KEY_PRINT;
    case VK_ESCAPE:
        return gameplay::Input::KEY_ESCAPE;
    case VK_BACK:
        return gameplay::Input::KEY_BACKSPACE;
    case VK_TAB:
        return shiftDown ? gameplay::Input::KEY_BACK_TAB : gameplay::Input::KEY_TAB;
    case VK_RETURN:
        return gameplay::Input::KEY_RETURN;
    case VK_CAPITAL:
        return gameplay::Input::KEY_CAPS_LOCK;
    case VK_LSHIFT:
        return gameplay::Input::KEY_LEFT_SHIFT;
    case VK_RSHIFT:
        return gameplay::Input::KEY_RIGHT_SHIFT;
    case VK_LCONTROL:
        return gameplay::Input::KEY_LEFT_CTRL;
    case VK_RCONTROL:
        return gameplay::Input::KEY_RIGHT_CTRL;
    case VK_LMENU:
        return gameplay::Input::KEY_LEFT_ALT;
    case VK_RMENU:
        return gameplay::Input::KEY_RIGHT_ALT;
    case VK_LWIN:
        return gameplay::Input::KEY_LEFT_HYPER;
    case VK_RWIN:
        return gameplay::Input::KEY_RIGHT_HYPER;
    case VK_INSERT:
        return gameplay::Input::KEY_INSERT;
    case VK_HOME:
        return gameplay::Input::KEY_HOME;
    case VK_PRIOR:
        return gameplay::Input::KEY_PG_UP;
    case VK_DELETE:
        return gameplay::Input::KEY_DELETE;
    case VK_END:
        return gameplay::Input::KEY_END;
    case VK_NEXT:
        return gameplay::Input::KEY_PG_DOWN;
    case VK_LEFT:
        return gameplay::Input::KEY_LEFT;
    case VK_RIGHT:
        return gameplay::Input::KEY_RIGHT;
    case VK_UP:
        return gameplay::Input::KEY_UP;
    case VK_DOWN:
        return gameplay::Input::KEY_DOWN;
    case VK_NUMLOCK:
        return gameplay::Input::KEY_NUM_LOCK;
    case VK_ADD:
        return gameplay::Input::KEY_KP_PLUS;
    case VK_SUBTRACT:
        return gameplay::Input::KEY_KP_MINUS;
    case VK_MULTIPLY:
        return gameplay::Input::KEY_KP_MULTIPLY;
    case VK_DIVIDE:
        return gameplay::Input::KEY_KP_DIVIDE;
    case VK_NUMPAD7:
        return gameplay::Input::KEY_KP_HOME;
    case VK_NUMPAD8:
        return gameplay::Input::KEY_KP_UP;
    case VK_NUMPAD9:
        return gameplay::Input::KEY_KP_PG_UP;
    case VK_NUMPAD4:
        return gameplay::Input::KEY_KP_LEFT;
    case VK_NUMPAD5:
        return gameplay::Input::KEY_KP_FIVE;
    case VK_NUMPAD6:
        return gameplay::Input::KEY_KP_RIGHT;
    case VK_NUMPAD1:
        return gameplay::Input::KEY_KP_END;
    case VK_NUMPAD2:
        return gameplay::Input::KEY_KP_DOWN;
    case VK_NUMPAD3:
        return gameplay::Input::KEY_KP_PG_DOWN;
    case VK_NUMPAD0:
        return gameplay::Input::KEY_KP_INSERT;
    case VK_DECIMAL:
        return gameplay::Input::KEY_KP_DELETE;
    case VK_F1:
        return gameplay::Input::KEY_F1;
    case VK_F2:
        return gameplay::Input::KEY_F2;
    case VK_F3:
        return gameplay::Input::KEY_F3;
    case VK_F4:
        return gameplay::Input::KEY_F4;
    case VK_F5:
        return gameplay::Input::KEY_F5;
    case VK_F6:
        return gameplay::Input::KEY_F6;
    case VK_F7:
        return gameplay::Input::KEY_F7;
    case VK_F8:
        return gameplay::Input::KEY_F8;
    case VK_F9:
        return gameplay::Input::KEY_F9;
    case VK_F10:
        return gameplay::Input::KEY_F10;
    case VK_F11:
        return gameplay::Input::KEY_F11;
    case VK_F12:
        return gameplay::Input::KEY_F12;
    case VK_SPACE:
        return gameplay::Input::KEY_SPACE;
    case 0x30:
        return shiftDown ? gameplay::Input::KEY_RIGHT_PARENTHESIS : gameplay::Input::KEY_ZERO;
    case 0x31:
        return shiftDown ? gameplay::Input::KEY_EXCLAM : gameplay::Input::KEY_ONE;
    case 0x32:
        return shiftDown ? gameplay::Input::KEY_AT : gameplay::Input::KEY_TWO;
    case 0x33:
        return shiftDown ? gameplay::Input::KEY_NUMBER : gameplay::Input::KEY_THREE;
    case 0x34:
        return shiftDown ? gameplay::Input::KEY_DOLLAR : gameplay::Input::KEY_FOUR;
    case 0x35:
        return shiftDown ? gameplay::Input::KEY_PERCENT : gameplay::Input::KEY_FIVE;
    case 0x36:
        return shiftDown ? gameplay::Input::KEY_CIRCUMFLEX : gameplay::Input::KEY_SIX;
    case 0x37:
        return shiftDown ? gameplay::Input::KEY_AMPERSAND : gameplay::Input::KEY_SEVEN;
    case 0x38:
        return shiftDown ? gameplay::Input::KEY_ASTERISK : gameplay::Input::KEY_EIGHT;
    case 0x39:
        return shiftDown ? gameplay::Input::KEY_LEFT_PARENTHESIS : gameplay::Input::KEY_NINE;
    case VK_OEM_PLUS:
        return shiftDown ? gameplay::Input::KEY_EQUAL : gameplay::Input::KEY_PLUS;
    case VK_OEM_COMMA:
        return shiftDown ? gameplay::Input::KEY_LESS_THAN : gameplay::Input::KEY_COMMA;
    case VK_OEM_MINUS:
        return shiftDown ? gameplay::Input::KEY_UNDERSCORE : gameplay::Input::KEY_MINUS;
    case VK_OEM_PERIOD:
        return shiftDown ? gameplay::Input::KEY_GREATER_THAN : gameplay::Input::KEY_PERIOD;
    case VK_OEM_1:
        return shiftDown ? gameplay::Input::KEY_COLON : gameplay::Input::KEY_SEMICOLON;
    case VK_OEM_2:
        return shiftDown ? gameplay::Input::KEY_QUESTION : gameplay::Input::KEY_SLASH;
    case VK_OEM_3:
        return shiftDown ? gameplay::Input::KEY_GRAVE : gameplay::Input::KEY_TILDE;
    case VK_OEM_4:
        return shiftDown ? gameplay::Input::KEY_LEFT_BRACE : gameplay::Input::KEY_LEFT_BRACKET;
    case VK_OEM_5:
        return shiftDown ? gameplay::Input::KEY_BAR : gameplay::Input::KEY_BACK_SLASH;
    case VK_OEM_6:
        return shiftDown ? gameplay::Input::KEY_RIGHT_BRACE : gameplay::Input::KEY_RIGHT_BRACKET;
    case VK_OEM_7:
        return shiftDown ? gameplay::Input::KEY_QUOTE : gameplay::Input::KEY_APOSTROPHE;
    case 0x41:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_A : gameplay::Input::KEY_A;
    case 0x42:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_B : gameplay::Input::KEY_B;
    case 0x43:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_C : gameplay::Input::KEY_C;
    case 0x44:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_D : gameplay::Input::KEY_D;
    case 0x45:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_E : gameplay::Input::KEY_E;
    case 0x46:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_F : gameplay::Input::KEY_F;
    case 0x47:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_G : gameplay::Input::KEY_G;
    case 0x48:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_H : gameplay::Input::KEY_H;
    case 0x49:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_I : gameplay::Input::KEY_I;
    case 0x4A:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_J : gameplay::Input::KEY_J;
    case 0x4B:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_K : gameplay::Input::KEY_K;
    case 0x4C:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_L : gameplay::Input::KEY_L;
    case 0x4D:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_M : gameplay::Input::KEY_M;
    case 0x4E:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_N : gameplay::Input::KEY_N;
    case 0x4F:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_O : gameplay::Input::KEY_O;
    case 0x50:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_P : gameplay::Input::KEY_P;
    case 0x51:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_Q : gameplay::Input::KEY_Q;
    case 0x52:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_R : gameplay::Input::KEY_R;
    case 0x53:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_S : gameplay::Input::KEY_S;
    case 0x54:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_T : gameplay::Input::KEY_T;
    case 0x55:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_U : gameplay::Input::KEY_U;
    case 0x56:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_V : gameplay::Input::KEY_V;
    case 0x57:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_W : gameplay::Input::KEY_W;
    case 0x58:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_X : gameplay::Input::KEY_X;
    case 0x59:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_Y : gameplay::Input::KEY_Y;
    case 0x5A:
        return shiftDown ? gameplay::Input::KEY_CAPITAL_Z : gameplay::Input::KEY_Z;
    default:
        return gameplay::Input::KEY_NONE;
    }
}

LRESULT CALLBACK __WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Scale factors for the mouse movement used to simulate the accelerometer.
    static const float ACCELEROMETER_X_FACTOR = 90.0f / WINDOW_WIDTH;
    static const float ACCELEROMETER_Y_FACTOR = 90.0f / WINDOW_HEIGHT;

    static bool hasMouse = false;
    static bool lMouseDown = false;
    static bool rMouseDown = false;
    static int lx, ly;

    static bool shiftDown = false;

    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(__hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_LBUTTONDOWN:
        gameplay::Game::getInstance()->touch(LOWORD(lParam), HIWORD(lParam), gameplay::Input::TOUCHEVENT_PRESS);
        lMouseDown = true;
        return 0;

    case WM_LBUTTONUP:
        lMouseDown = false;
        gameplay::Game::getInstance()->touch(LOWORD(lParam), HIWORD(lParam), gameplay::Input::TOUCHEVENT_RELEASE);
        return 0;

    case WM_RBUTTONDOWN:
        rMouseDown = true;
        lx = LOWORD(lParam);
        ly = HIWORD(lParam);
        break;

    case WM_RBUTTONUP:
        rMouseDown = false;
        break;

    case WM_MOUSEMOVE:
        if (!hasMouse)
        {
            hasMouse = true;

            // Call TrackMouseEvent to detect the next WM_MOUSE_LEAVE.
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = __hwnd;
            TrackMouseEvent(&tme);
        }

        if (lMouseDown)
        {
            gameplay::Game::getInstance()->touch(LOWORD(lParam), HIWORD(lParam), gameplay::Input::TOUCHEVENT_MOVE);
            return 0;
        }
        else if (rMouseDown)
        {
            // Update the pitch and roll by adding the scaled deltas.
            __roll += -(float)(LOWORD(lParam) - lx) * ACCELEROMETER_X_FACTOR;
            __pitch += (float)(HIWORD(lParam) - ly) * ACCELEROMETER_Y_FACTOR;

            // Clamp the values to the valid range.
            __roll = max(min(__roll, 90.0), -90.0);
            __pitch = max(min(__pitch, 90.0), -90.0);

            // Update the last X/Y values.
            lx = LOWORD(lParam);
            ly = HIWORD(lParam);
        }
        break;

    case WM_MOUSELEAVE:
        hasMouse = false;
        lMouseDown = false;
        rMouseDown = false;
        break;

    case WM_KEYDOWN:
        if (wParam == VK_LSHIFT || wParam == VK_RSHIFT)
            shiftDown = true;

        gameplay::Game::getInstance()->keyPress(getGameplayInputKey(wParam, shiftDown), gameplay::Input::KEYEVENT_DOWN);
        break;

    case WM_KEYUP:
        if (wParam == VK_LSHIFT || wParam == VK_RSHIFT)
            shiftDown = false;

        gameplay::Game::getInstance()->keyPress(getGameplayInputKey(wParam, shiftDown), gameplay::Input::KEYEVENT_UP);
        break;

    case WM_SETFOCUS:
        break;

    case WM_KILLFOCUS:
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam); 
}

namespace gameplay
{

extern void printError(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    fprintf(stderr, "\n");
    int sz = vfprintf(stderr, format, argptr);
    if (sz > 0)
    {
        char* buf = new char[sz + 2];
        vsprintf(buf, format, argptr);
        buf[sz] = '\n';
        buf[sz+1] = 0;
        OutputDebugStringA(buf);
        SAFE_DELETE_ARRAY(buf);
    }
    va_end(argptr);
}

Platform::Platform(Game* game)
    : _game(game)
{
}

Platform::Platform(const Platform& copy)
{
    // hidden
}

Platform::~Platform()
{
    if (__hwnd)
    {
        DestroyWindow(__hwnd);
        __hwnd = 0;
    }
}

// TODO: Fix Fullscreen + More error handling.
Platform* Platform::create(Game* game)
{
    FileSystem::setResourcePath("./");

    Platform* platform = new Platform(game);

    // Get the application module handle.
    __hinstance = ::GetModuleHandle(NULL);

    LPCTSTR windowClass = L"gameplay";
    LPCTSTR windowName = L"gameplay Window";

    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    // Register our window class.
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc    = (WNDPROC)__WndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = __hinstance;
    wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm        = NULL;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = NULL;  // No brush - we are going to paint our own background
    wc.lpszMenuName   = NULL;  // No default menu
    wc.lpszClassName  = windowClass;

    if (!::RegisterClassEx(&wc))
        goto error;
    
    // Set the window style.
    DWORD style   = ( WINDOW_FULLSCREEN ? WS_POPUP : WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    DWORD styleEx = (WINDOW_FULLSCREEN ? WS_EX_APPWINDOW : WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

    // Adjust the window rectangle so the client size is the requested size.
    AdjustWindowRectEx(&rect, style, FALSE, styleEx);
    
    // Create the native Windows window.
    __hwnd = CreateWindowEx(styleEx, windowClass, windowName, style, 0, 0, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, __hinstance, NULL);

    // Get the drawing context.
    __hdc = GetDC(__hwnd);

    // Choose pixel format. 32-bit. RGBA.
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(__hdc, &pfd);

    if (pixelFormat == 0 || !SetPixelFormat (__hdc, pixelFormat, &pfd))
        goto error;

    HGLRC tempContext = wglCreateContext(__hdc);
    wglMakeCurrent(__hdc, tempContext);

    if (GLEW_OK != glewInit())
        goto error;

    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        0
    };

    if (!(__hrc = wglCreateContextAttribsARB(__hdc, 0, attribs) ) )
    {
        wglDeleteContext(tempContext);
        goto error;
    }
    wglDeleteContext(tempContext);

    if (!wglMakeCurrent(__hdc, __hrc) || !__hrc)
        goto error;

    // Vertical sync.
    wglSwapIntervalEXT(__vsync ? 1 : 0);

    ShowWindow(__hwnd, SW_SHOW);

    return platform;

error:

    // TODO: cleanup
    exit(0);
    return NULL;
}

int Platform::enterMessagePump()
{  
    int rc = 0;

    // Get the initial time.
    LARGE_INTEGER tps;
    QueryPerformanceFrequency(&tps);
    __timeTicksPerMillis = (long)(tps.QuadPart / 1000L);
    LARGE_INTEGER queryTime;
    QueryPerformanceCounter(&queryTime);
    __timeStart = queryTime.QuadPart / __timeTicksPerMillis;

    // Set the initial pitch and roll values.
    __pitch = 0.0;
    __roll = 0.0;

    _game->run(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Enter event dispatch loop.
    MSG msg;
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                _game->exit();
                break;
            }
        }

        if (_game->getState() != Game::PAUSED)
        {
            _game->frame();
            SwapBuffers(__hdc);
        }
    }

    return msg.wParam;
}

long Platform::getAbsoluteTime()
{
       LARGE_INTEGER queryTime;
    QueryPerformanceCounter(&queryTime);
    __timeAbsolute = queryTime.QuadPart / __timeTicksPerMillis;

    return __timeAbsolute;
}

void Platform::setAbsoluteTime(long time)
{
    __timeAbsolute = time;
}

bool Platform::isVsync()
{
    return __vsync;
}

void Platform::setVsync(bool enable)
{
     wglSwapIntervalEXT(enable ? 1 : 0);
    __vsync = enable;
}

int Platform::getOrientationAngle()
{
    return 0;
}

bool Platform::isAccelerometerSupported()
{
    return true;
}

void Platform::getAccelerometerPitchAndRoll(float* pitch, float* roll)
{
    *pitch = __pitch;
    *roll = __roll;
}

}

#endif
