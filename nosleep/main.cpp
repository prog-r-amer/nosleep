#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <string>
#include <commctrl.h>
#include <strsafe.h>
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ICON (WM_APP + 100)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
	LPCSTR name = "name";
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = name;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		name,                     // Window class
		"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	NOTIFYICONDATA nid = {};
	nid.hWnd = hwnd;
	nid.cbSize = sizeof(nid);
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID | NIF_MESSAGE;
	nid.uVersion = NOTIFYICON_VERSION_4; //necessary to get x, y data from icon click
	nid.uCallbackMessage = ICON;
	nid.uID = 0;

	// This text will be shown as the icon's tooltip.
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), "Test application");
	// Load the icon for high DPI.
	LoadIconMetric(NULL, (PCWSTR)MAKEINTRESOURCE(IDI_ERROR), LIM_SMALL, &(nid.hIcon));
	Shell_NotifyIcon(NIM_ADD, &nid);
	Shell_NotifyIcon(NIM_SETVERSION, &nid);
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case ICON:
		{
			WORD message_type = LOWORD(lParam);
			if (message_type == WM_RBUTTONDOWN ) {
				int x = GET_X_LPARAM(wParam);
				int y = GET_Y_LPARAM(wParam);
				SetWindowPos(hwnd, HWND_TOP, x + 100, y + 100, 100, 100, SWP_SHOWWINDOW);
			}
			return 0;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	}
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}