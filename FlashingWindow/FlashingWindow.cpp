// FlashingWindow.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FlashingWindow.h"
#include "resource.h"

#define MAX_LOADSTRING 100
#define IDT_TIMER 1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING] = L"";                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
bool drawAlternateColor = true;
const HBRUSH LightWhiteBrush = CreateSolidBrush(RGB(254, 254, 254));
const HBRUSH WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
const HBRUSH DarkGrayBrush = CreateSolidBrush(RGB(1, 1, 1));
const HBRUSH BlackBrush = CreateSolidBrush(RGB(0, 0, 0));
const DWORD WindowSize = 50;
bool isAlwaysOnTop = true;
bool darkTheme = true;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDC_FLASHINGWINDOW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLASHINGWINDOW));

    MSG msg;

	// Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLASHINGWINDOW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = _T("");
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

VOID ConfigureAlwaysOnTop(HWND hWnd, bool alwaysOnTop) {
	SetWindowPos(hWnd, alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, szWindowClass, szTitle, WS_POPUP,
      300, 300, WindowSize, WindowSize, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   ConfigureAlwaysOnTop(hWnd, isAlwaysOnTop);

   SetTimer(hWnd,             // handle to main window 
	   IDT_TIMER,            // timer identifier 
	   1000,                 // 1-second interval 
	   (TIMERPROC)NULL);     // no timer callback 

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_TIMER:
		drawAlternateColor = !drawAlternateColor;
		InvalidateRect(hWnd, nullptr, false);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_CLOSE:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			case ID_ALWAYSVISIBLE:
				isAlwaysOnTop = !isAlwaysOnTop;
				ConfigureAlwaysOnTop(hWnd, isAlwaysOnTop);
				break;
			case ID_DARKTHEME:
				darkTheme = !darkTheme;
				InvalidateRect(hWnd, nullptr, false);
				break;
			default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
	{
		auto drawingBrush = darkTheme ?
			(drawAlternateColor ? DarkGrayBrush : BlackBrush)
			: (drawAlternateColor ? LightWhiteBrush : WhiteBrush);

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect = { 0, 0, WindowSize, WindowSize };
		FillRect(hdc, &rect, drawingBrush);
		EndPaint(hWnd, &ps);
	}
        break;
	case WM_NCHITTEST:
	{
		// Call the default window procedure for default handling.
		const LRESULT result = ::DefWindowProc(hWnd, message, wParam, lParam);

		if (GetAsyncKeyState(VK_LBUTTON) & VK_LBUTTON && result == HTCLIENT)
			return HTCAPTION;
		else
			return result;
	}
	case WM_RBUTTONDOWN:
	{
		POINTS ps = MAKEPOINTS(lParam);
		POINT pt;
		pt.x = ps.x;
		pt.y = ps.y;
		ClientToScreen(hWnd, &pt);
		HMENU hPopupMenu = CreatePopupMenu();

		DWORD menuFlags = MF_BYPOSITION | MF_STRING;
		if (isAlwaysOnTop) menuFlags |= MF_CHECKED;
		InsertMenu(hPopupMenu, 0, menuFlags, ID_ALWAYSVISIBLE, L"Always On Top");

		menuFlags = MF_BYPOSITION | MF_STRING;
		if (darkTheme) menuFlags |= MF_CHECKED;
		InsertMenu(hPopupMenu, 1, menuFlags, ID_DARKTHEME, L"Dark Theme");

		InsertMenu(hPopupMenu, 2, MF_BYPOSITION | MF_STRING, ID_CLOSE, L"Close");

		SetForegroundWindow(hWnd);
		TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
		break;
	}
	case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
