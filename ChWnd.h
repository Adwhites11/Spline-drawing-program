#include <Windows.h>
#include <tchar.h>

class ChWnd
{
protected:
	WNDCLASSEX wcex;
	HWND hWnd;
public:
	ChWnd() {}
	BOOL SetWnd(LPCTSTR windowName, HINSTANCE hInst,
		LRESULT(WINAPI* pWndProc)(HWND, UINT, WPARAM, LPARAM),
		int x = 0, int y = 0, int width = 0, int height = 0,
		UINT exStyle = NULL,
		UINT classStyle = CS_HREDRAW | CS_VREDRAW,
		DWORD windowStyle = WS_CHILD | WS_VISIBLE,
		HWND hParent = NULL, HBRUSH hBr = CreateSolidBrush(RGB(230, 230, 230)));
	HWND GethWnd() { return hWnd; }
};

BOOL ChWnd::SetWnd(LPCTSTR windowName, HINSTANCE hInst,
	LRESULT(WINAPI* pWndProc)(HWND, UINT, WPARAM, LPARAM),
	int x, int y, int width, int height, UINT exStyle, UINT classStyle,
	DWORD windowStyle, HWND hParent, HBRUSH hBr)
{
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = classStyle;
	wcex.lpfnWndProc = pWndProc;
	wcex.hInstance = hInst;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = hBr;
	wcex.lpszClassName = windowName;
	
	if (!RegisterClassEx(&wcex))
	{
		TCHAR msg[100] = _T("Cannot register class: ");
		_tcscat_s(msg, windowName);
		MessageBox(NULL, msg, L"Error", MB_OK);
		return FALSE;
	}

	hWnd = CreateWindowEx(exStyle, windowName, NULL, windowStyle, x, y, 
		width, height, hParent, NULL, hInst, NULL);
	return TRUE;
}