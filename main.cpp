#include <windows.h>
#include <iostream>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            CreateWindow(
                "BUTTON", "Test Button",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                50, 50, 150, 50, hWnd, (HMENU)1, NULL, NULL
            );
            RegisterTouchWindow(hWnd, TWF_FINETOUCH | TWF_WANTPALM);
            std::cout << "[WM_CREATE] Window Created" << std::endl;
        } break;
        case WM_SETFOCUS: {
            std::cout << "[WM_SETFOCUS] Focus Gained" << std::endl;
        } break;
        case WM_KILLFOCUS: {
            std::cout << "[WM_KILLFOCUS] Focus Lost" << std::endl;
        } break;
        case WM_MOUSEMOVE: {
            static bool is_inside = false;
            if (!is_inside) {
                std::cout << "[WM_MOUSEMOVE] Mouse Entered Window" << std::endl;
                is_inside = true;
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
            }
        } break;
        case WM_MOUSELEAVE: {
            std::cout << "[WM_MOUSELEAVE] Mouse Left Window" << std::endl;
        } break;
        case WM_TOUCH: {
            UINT cInputs = LOWORD(wParam);
            PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
            if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
                for (UINT i = 0; i < cInputs; i++) {
                    if (pInputs[i].dwFlags & TOUCHEVENTF_DOWN) 
                        std::cout << "[WM_TOUCH] Touch DOWN at " << pInputs[i].x / 100 << ", " << pInputs[i].y / 100 << std::endl;
                }
                CloseTouchInputHandle((HTOUCHINPUT)lParam);
            }
            delete[] pInputs;
        } break;
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1 && HIWORD(wParam) == BN_CLICKED)
                std::cout << "[WM_COMMAND] Button Clicked" << std::endl;
        } break;
        case WM_DESTROY: {
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