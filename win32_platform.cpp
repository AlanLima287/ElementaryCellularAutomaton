#include <Windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <math.h>

#include <iostream>
#include <random>

#include "input_handler.cpp"
#include "rendering.cpp"

void setupApplication();
void simulateApplication(float);
void onResize();

namespace Window {

    static WindowState window = {};
    static Input input = {};

    static const unsigned int PIXEL_RATIO = 2;
    static bool running = true;

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    int WINAPI WinStart(LPCWSTR WindowName, bool showConsole = false) {

        HINSTANCE hInstance = {};
        WNDCLASS wndClass = {};

        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpszClassName = WindowName;
        wndClass.lpfnWndProc = WindowProc;

        if (!showConsole) FreeConsole();
        FontMap::fontAssembler(ctx::font, "assets/Consola8.ftmp");
        srand((unsigned int)time(0));
        ctx::setWindow(&window);

        RegisterClass(&wndClass);

        HWND hWnd = CreateWindow(
            WindowName, WindowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, hInstance, 0
        );

        if (hWnd == NULL) return 0;

        ShowWindow(hWnd, SW_SHOWMAXIMIZED);
        HDC hdc = GetDC(hWnd);
        setupApplication();

        float deltaTime = 0.0166666f;
        LARGE_INTEGER frameBeginTime, frameEndTime;
        QueryPerformanceCounter(&frameBeginTime);

        float performaceFrequency;
        {
            LARGE_INTEGER perf;
            QueryPerformanceFrequency(&perf);
            performaceFrequency = (float)perf.QuadPart;
        }

        MSG msg = {};

        while (running) {

            input.mouse.left.changed = false;
            input.mouse.right.changed = false;
            input.mouse.middle.changed = false;
            for (int i = 0; i < KEYBOARD_BUTTON_COUNT; i++) {
                input.keyboard[i].changed = false;
            }

            while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            simulateApplication(deltaTime);

            StretchDIBits(
                hdc, 0, 0, window.screenWidth, window.screenHeight, 0, 0, window.width, window.height,
                window.buffer, &window.bitmapInfo, DIB_RGB_COLORS, SRCCOPY
            );

            Sleep(6);

            QueryPerformanceCounter(&frameEndTime);
            deltaTime = (float)(frameEndTime.QuadPart - frameBeginTime.QuadPart) / performaceFrequency;
            frameBeginTime = frameEndTime;
        }

        return 0;
    }

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        LRESULT result = 0;
        switch (uMsg) {

            case WM_CLOSE:
            case WM_DESTROY: {
                running = false;
            } break;

            case WM_KEYUP:
            case WM_KEYDOWN: {
                unsigned long vkCode = (unsigned long)wParam;
                bool isDown = (lParam & 0x80000000i64) == 0;

                input.keyboard[vkCode].changed = isDown != input.keyboard[vkCode].isDown;
                input.keyboard[vkCode].isDown = isDown;

                input.lastPressedKey = (unsigned char)vkCode;

            } break;

            case WM_LBUTTONUP:
            case WM_LBUTTONDOWN: {
                bool isDownL = ((wParam & 0b001) > 0);

                if (isDownL) SetCapture(hWnd);
                else ReleaseCapture();

                input.mouse.right.changed = isDownL != input.mouse.right.isDown;
                input.mouse.right.isDown = isDownL;

            } break;

            case WM_RBUTTONUP:
            case WM_RBUTTONDOWN: {
                bool isDownR = ((wParam & 0b010) > 0);

                input.mouse.right.changed = isDownR != input.mouse.right.isDown;
                input.mouse.right.isDown = isDownR;

            } break;

            case WM_MBUTTONUP:
            case WM_MBUTTONDOWN: {
                bool isDownM = ((wParam & 0b100) > 0);

                input.mouse.middle.changed = isDownM != input.mouse.middle.isDown;
                input.mouse.middle.isDown = isDownM;
            } break;

            case WM_MOUSEMOVE: {
                input.mouse.offsetX = ((int)lParam & 0xffff) / PIXEL_RATIO;
                input.mouse.offsetY = window.height - 1 - ((int)(lParam >> 16) & 0xffff) / PIXEL_RATIO;
            } break;

            case WM_SIZE: {

                RECT rect = {};

                GetClientRect(hWnd, &rect);
                window.width = (int)round((float)(rect.right - rect.left) / PIXEL_RATIO);
                window.height = (int)round((float)(rect.bottom - rect.top) / PIXEL_RATIO);
                window.screenHeight = window.height * PIXEL_RATIO;
                window.screenWidth = window.width * PIXEL_RATIO;

                size_t length = (size_t)window.width * window.height;

                if (window.buffer) free(window.buffer);
                window.buffer = malloc(length * sizeof(unsigned long));

                window.bitmapInfo.bmiHeader.biSize = sizeof(window.bitmapInfo.bmiHeader);
                window.bitmapInfo.bmiHeader.biHeight = window.height;
                window.bitmapInfo.bmiHeader.biWidth = window.width;
                window.bitmapInfo.bmiHeader.biPlanes = 1;
                window.bitmapInfo.bmiHeader.biBitCount = 32;
                window.bitmapInfo.bmiHeader.biCompression = BI_RGB;

                window.midPointY = (window.height >> 1) - 1;
                window.midPointX = (window.width >> 1) - 1;
                onResize();

            } break;

            default: {
                result = DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        }

        return result;
    }
};