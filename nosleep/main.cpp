#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <string>
#include <commctrl.h>
#include <strsafe.h>
#include "resource.h"
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ICON (WM_APP + 100)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static const GUID guid = { 0xe5817653, 0x5a53, 0x4068, { 0x94, 0xa7, 0x73, 0x59, 0xf2, 0x6, 0xa, 0x41 } };


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
		WS_OVERLAPPED,            // Window style

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
	nid.uVersion = NOTIFYICON_VERSION_4;
	nid.uCallbackMessage = ICON;
	nid.guidItem = guid;


	// This text will be shown as the icon's tooltip.
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), "Test application");
	// Load the icon for high DPI.
	LoadIconMetric(hInstance, (PCWSTR)MAKEINTRESOURCE(LIGHT_ICON), LIM_SMALL, &(nid.hIcon));
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
			RECT position = {};
			//get bounding rectangle of clicked icon
			NOTIFYICONIDENTIFIER icon = {};
			icon.cbSize = sizeof(NOTIFYICONIDENTIFIER);
			icon.guidItem = guid;
			HRESULT result = Shell_NotifyIconGetRect(&icon, &position);

			if (message_type == WM_RBUTTONDOWN ) {
				SetWindowPos(hwnd, HWND_TOP, position.left, position.top - 80, 100, 100, SWP_SHOWWINDOW);
			}
			return 0;
		}
	case WM_DESTROY:
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uVersion = NOTIFYICON_VERSION_4;
		nid.uFlags = NIF_GUID;
		nid.guidItem = guid;
		Shell_NotifyIcon(NIM_DELETE, &nid);
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