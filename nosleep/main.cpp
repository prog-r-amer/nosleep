#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <string>
#include <commctrl.h>
#include <strsafe.h>
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ICON (WM_APP + 100)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static const GUID guid = { 0xe5817653, 0x5a53, 0x4068, { 0x94, 0xa7, 0x73, 0x59, 0xf2, 0x6, 0xa, 0x41 } };

HWND parent_window;
HWND menu;
HWND hwnd_text;

HMENU test;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
	LPCSTR name = "windowclass";
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = name;
	LoadIconMetric(hInstance, (PCWSTR)MAKEINTRESOURCE(MAINICON), LIM_LARGE, &(wc.hIcon));

	RegisterClass(&wc);

	// Create the window.

	parent_window = CreateWindowEx(
		0,                              // Optional window styles.
		name,                     // Window class
		"about",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);



	if (parent_window == NULL)
	{
		return 0;
	}

	menu = CreateWindowEx(
		0,                              // Optional window styles.
		name,                     // Window class
		"",    // Window text
		WS_POPUP,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		parent_window,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (menu == NULL) {
		return 0;
	}

	hwnd_text = CreateWindowEx(
		0, "EDIT",   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE |
		ES_LEFT | ES_MULTILINE | ES_READONLY,
		0, 0, 0, 0,   // set size in WM_SIZE message 
		parent_window,         // parent window 
		NULL,   // edit control ID 
		(HINSTANCE)GetWindowLong(parent_window, GWL_HINSTANCE),
		NULL);        // pointer not needed 

	TCHAR text[] = "coffee icon by linearicon https://linearicons.com/ \r\n"
		"creator is https://perxis.com/";

	SendMessage(hwnd_text, WM_SETTEXT, 0, (LPARAM)text);
	NOTIFYICONDATA nid = {};
	nid.hWnd = menu;
	nid.cbSize = sizeof(nid);
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID | NIF_MESSAGE;
	nid.uVersion = NOTIFYICON_VERSION_4;
	nid.uCallbackMessage = ICON;
	nid.guidItem = guid;


	// This text will be shown as the icon's tooltip.
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), "Test application");
	// Load the icon for high DPI.
	LoadIconMetric(hInstance, (PCWSTR)MAKEINTRESOURCE(LIGHT_ICON), LIM_SMALL, &(nid.hIcon));
	Shell_NotifyIcon(NIM_DELETE, &nid);
	Shell_NotifyIcon(NIM_ADD, &nid);
	Shell_NotifyIcon(NIM_SETVERSION, &nid);

	test = CreatePopupMenu();
	MENUITEMINFO info = {};
	info.cbSize = sizeof(info);
	info.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID;
	info.fType = MFT_STRING;
	info.dwTypeData = (LPSTR)"about";
	info.wID = MENU_ABOUT;
	info.cch = lstrlen(info.dwTypeData);
	InsertMenuItem(test, 0, true, &info);
	info.wID = MENU_EXIT;
	info.dwTypeData = (LPSTR)"exit";
	info.cch = lstrlen(info.dwTypeData);
	InsertMenuItem(test, 1, true, &info);

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
		SetForegroundWindow(menu);
		if (message_type == WM_RBUTTONDOWN) {
			bool result = TrackPopupMenuEx(test, TPM_LEFTALIGN, position.left, position.top, menu, NULL);
		}
		return 0;
	}
	case WM_COMMAND:
	{
		WORD id = LOWORD(wParam);
		WORD type = HIWORD(wParam);

		if (id == MENU_ABOUT) {
			ShowWindow(parent_window, SW_SHOW);
		}

		if (id == MENU_EXIT) {
			SendMessage(hwnd, WM_DESTROY, NULL, NULL);
		}
		return 0;
	}

	case WM_DESTROY:
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uVersion = NOTIFYICON_VERSION_4;
		nid.uFlags = NIF_GUID;
		nid.guidItem = guid;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE:
		// Make the edit control the size of the window's client area. 
		MoveWindow(hwnd_text,
			0, 0,                  // starting x- and y-coordinates 
			LOWORD(lParam),        // width of client area 
			HIWORD(lParam),        // height of client area 
			TRUE);                 // repaint window 
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
		return 0;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}