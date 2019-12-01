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


HWND parent_window;
HWND edit_window;
HWND edit;
HMENU menu;


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
		"",    // Window text
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

	edit_window = CreateWindowEx(
		0,                              // Optional window styles.
		name,                     // Window class
		"about",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		parent_window,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);



	if (edit_window == NULL)
	{
		return 0;
	}

	edit = CreateWindowEx(
		0, "EDIT",   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE |
		ES_LEFT | ES_MULTILINE | ES_READONLY,
		0, 0, 0, 0,   // set size in WM_SIZE message 
		edit_window,         // parent window 
		NULL,   // edit control ID 
		hInstance,
		NULL);        // pointer not needed 

	if (edit == NULL) {
		return 0;
	}

	TCHAR text[] = "coffee icon by linearicon https://linearicons.com/ \r\n"
		"creator is https://perxis.com/";

	SendMessage(edit, WM_SETTEXT, 0, (LPARAM)text);
	NOTIFYICONDATA nid = {};
	nid.hWnd = parent_window;
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uVersion = NOTIFYICON_VERSION_4;
	nid.uCallbackMessage = ICON;


	// This text will be shown as the icon's tooltip.
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), "Test application");
	// Load the icon for high DPI.
	LoadIconMetric(hInstance, (PCWSTR)MAKEINTRESOURCE(LIGHT_ICON), LIM_SMALL, &(nid.hIcon));
	Shell_NotifyIcon(NIM_DELETE, &nid);
	Shell_NotifyIcon(NIM_ADD, &nid);
	Shell_NotifyIcon(NIM_SETVERSION, &nid);

	menu = CreatePopupMenu();
	MENUITEMINFO info = {};
	info.cbSize = sizeof(info);
	info.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID;
	info.fType = MFT_STRING;
	info.dwTypeData = (LPSTR)"about";
	info.wID = MENU_ABOUT;
	info.cch = lstrlen(info.dwTypeData);
	InsertMenuItem(menu, 0, true, &info);
	info.wID = MENU_EXIT;
	info.dwTypeData = (LPSTR)"exit";
	info.cch = lstrlen(info.dwTypeData);
	InsertMenuItem(menu, 1, true, &info);

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
		icon.hWnd = parent_window;
		icon.uID = 0;
		icon.cbSize = sizeof(NOTIFYICONIDENTIFIER);
		HRESULT result = Shell_NotifyIconGetRect(&icon, &position);
		SetForegroundWindow(parent_window);
		if (message_type == WM_RBUTTONDOWN) {
			bool result = TrackPopupMenuEx(menu, TPM_LEFTALIGN, position.left, position.top, parent_window, NULL);
		}
		return 0;
	}
	case WM_COMMAND:
	{
		WORD id = LOWORD(wParam);
		WORD type = HIWORD(wParam);

		if (id == MENU_ABOUT) {
			ShowWindow(edit_window, SW_SHOW);
		}

		if (id == MENU_EXIT) {
			SendMessage(parent_window, WM_DESTROY, NULL, NULL);
		}
		return 0;
	}

	case WM_CLOSE:
	{
		if (hwnd == edit_window) {
			ShowWindow(edit_window, SW_HIDE);
		}
		return 0;
	}

	case WM_DESTROY:
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uVersion = NOTIFYICON_VERSION_4;
		nid.uID = 0;
		nid.hWnd = parent_window;
		nid.uFlags = NIF_GUID;
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE:
		// Make the edit control the size of the window's client area. 
		MoveWindow(edit,
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