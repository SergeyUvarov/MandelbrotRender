#include "WindowWrapper.h"

void smooth(Photo* out, Photo* canvas) {
    for (int32_t y = 0; y < canvas->height; y+=2)
    {
        for (int32_t x = 0; x < canvas->width; x+=2)
        {
            int64_t c[] = {
                canvas->getPixel(x, y),
                canvas->getPixel(x+1, y),
                canvas->getPixel(x-1, y+1),
                canvas->getPixel(x, y+1),
            };

            int64_t r = getR(c[0]) + getR(c[1]) + getR(c[2]) + getR(c[3]);
            int64_t g = getG(c[0]) + getG(c[1]) + getG(c[2]) + getG(c[3]);
            int64_t b = getB(c[0]) + getB(c[1]) + getB(c[2]) + getB(c[3]);

            out->setPixel(x / 2, y / 2, sRGB(r/4, g/4, b/4));
        }
    }
}

void displayFrames(HWND hWnd, Photo* canvas, void (*renderFrameFunc)(HWND, Photo*, bool*), bool* workStat)
{
    HDC hdc, tempDc;
    while (*workStat)
    {
        hdc = GetDC(hWnd);
        tempDc = CreateCompatibleDC(hdc);

        Photo pht(canvas->width, canvas->height);
        smooth(&pht, canvas);

        HBITMAP hBmp = pht.draw(hdc);
        SelectObject(tempDc, hBmp);
        StretchBlt(hdc, 0, 0, 1024, 1024, tempDc, 0, 0, 1024, 1024, SRCCOPY);

        DeleteObject(hBmp);
        DeleteDC(tempDc);
        ReleaseDC(hWnd, hdc);
    }
}

void (*mlbuttonfunc)(int32_t, int32_t);
void (*mwheelfunc)(int32_t);
void (*chldCommand)(int32_t);
void def(int32_t, int32_t) {};
void def1(int32_t) {};


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_MOUSEWHEEL:
        mwheelfunc(GET_WHEEL_DELTA_WPARAM(wParam));
        break;

    case WM_COMMAND:
        chldCommand(wParam);
        break;

    case WM_LBUTTONUP:
        mlbuttonfunc(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


WindowWrapper::WindowWrapper(const WCHAR* szTitle, void (*renderFrameFunc)(HWND, Photo*, bool*), void (*mouseClickFunction)(int32_t, int32_t), void (*mouseWheelFunction)(int32_t), void (*childCommand)(int32_t)) : _hWnd(0), _hInstance(nullptr), _wndClassName(L""), _canvas(2048, 2048), _workStat(true)
{
    mlbuttonfunc = mouseClickFunction;
    mwheelfunc = mouseWheelFunction;
    chldCommand = childCommand;

    memcpy(_wndClassName, szTitle, MAX_LOADSTRING);
    _hInstance = GetModuleHandle(NULL);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = _hInstance;

    wcex.hIcon = LoadIcon(_hInstance, IDI_WINLOGO);
    wcex.hIconSm = LoadIcon(_hInstance, IDI_WINLOGO);

    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = _wndClassName;

    if (!RegisterClassExW(&wcex))
    {
        return;
    }

    _hWnd = CreateWindowW(_wndClassName, 
        szTitle, 
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
        nullptr, nullptr, _hInstance, nullptr);

    if (!_hWnd)
    {
        return;
    }

    ShowWindow(_hWnd, SW_MAXIMIZE);
    UpdateWindow(_hWnd);

    _displayThread = std::thread(displayFrames, _hWnd, &_canvas, renderFrameFunc, &_workStat);
    _renderThread = std::thread(renderFrameFunc, _hWnd, &_canvas, &_workStat);
}

void WindowWrapper::run()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HWND WindowWrapper::getHwnd()
{
    return _hWnd;
}

WindowWrapper::~WindowWrapper()
{
    _workStat = false;
    _renderThread.join();
    _displayThread.join();
}