#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>


void LogMessage(const char * msg_str, const char * info = "") {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);
    printf(
        "[%02d:%02d:%02d.%03lld, %s] %s\n", 
        bt.tm_hour, bt.tm_min, bt.tm_sec, ms.count(),
        msg_str, info
    );
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static bool is_mouse_entered = false;
    static char buffer[256];
    switch (message) {
        case WM_CREATE: {
            LogMessage("WM_CREATE");
            CreateWindow(
                "BUTTON", "Test Button",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                50, 50, 150, 50, hWnd, (HMENU)1, NULL, NULL
            );
            RegisterTouchWindow(hWnd, TWF_FINETOUCH | TWF_WANTPALM);
        } break;
        case WM_PAINT: {
            LogMessage("WM_PAINT");
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1)); 
            EndPaint(hWnd, &ps);
        } break;
        case WM_SIZE: {
            LogMessage("WM_SIZE");
            InvalidateRect(hWnd, NULL, TRUE);
        } break;
        case WM_SETFOCUS: {
            LogMessage("WM_SETFOCUS", "Focus Gained");
        } break;
        case WM_KILLFOCUS: {
            LogMessage("WM_KILLFOCUS", "Focus Lost");
        } break;
        case WM_MOUSEACTIVATE: {
            LogMessage("WM_MOUSEACTIVATE");
        } break;
        case WM_ACTIVATEAPP: {
            LogMessage("WM_ACTIVATEAPP");
        } break;
        case WM_ACTIVATE: {
            LogMessage("WM_ACTIVATE");
        } break;
        case WM_MOUSEMOVE: {
            if (!is_mouse_entered) {
                LogMessage("WM_MOUSEMOVE", "Mouse Entered Window");
                is_mouse_entered = true;
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
            }
            if (wParam & MK_LBUTTON) {
                snprintf(buffer, sizeof(buffer), "Mouse DRAGGING at %d, %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                LogMessage("WM_MOUSEMOVE", buffer);
            }
        } break;
        case WM_MOUSELEAVE: {
            is_mouse_entered = false;
            LogMessage("WM_MOUSELEAVE", "Mouse Left Window");
        } break;
        case WM_TOUCH: {
            UINT cInputs = LOWORD(wParam);
            PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
            if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
                for (UINT i = 0; i < cInputs; i++) {
                    if (pInputs[i].dwFlags & TOUCHEVENTF_DOWN) {
                        snprintf(buffer, sizeof(buffer), "Touch DOWN at %d, %d", pInputs[i].x / 100, pInputs[i].y / 100);
                        LogMessage("WM_TOUCH", buffer);
                    }
                    else if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE) {
                        snprintf(buffer, sizeof(buffer), "Touch MOVE at %d, %d", pInputs[i].x / 100, pInputs[i].y / 100);
                        LogMessage("WM_TOUCH", buffer);
                    }
                    else if (pInputs[i].dwFlags & TOUCHEVENTF_UP) {
                        snprintf(buffer, sizeof(buffer), "Touch UP at %d, %d", pInputs[i].x / 100, pInputs[i].y / 100);
                        LogMessage("WM_TOUCH", buffer);
                    }
                }
                CloseTouchInputHandle((HTOUCHINPUT)lParam);
            }
            delete[] pInputs;
        } break;
        case WM_LBUTTONDOWN: {
            snprintf(buffer, sizeof(buffer), "Mouse LEFT DOWN at %d, %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            LogMessage("WM_LBUTTONDOWN", buffer);
        } break;

        case WM_LBUTTONUP: {
            snprintf(buffer, sizeof(buffer), "Mouse LEFT UP at %d, %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            LogMessage("WM_LBUTTONUP", buffer);
        } break;
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED) {
                LogMessage("WM_COMMAND", "Button Clicked");
            }
        } break;
        case WM_DESTROY: {
            LogMessage("WM_DESTROY");
            PostQuitMessage(0);
        } break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "win32_test_class";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        "win32_test_class", "win32 test window", WS_OVERLAPPEDWINDOW, 
        100, 100, 400, 300, NULL, NULL, hInstance, NULL
    );
    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}