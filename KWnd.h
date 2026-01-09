#include <windows.h>
#include <tchar.h>
#include "resource.h"
class KWnd
{
protected:
	HWND hWnd;
	WNDCLASSEX wc;
public:
	KWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
		LRESULT(WINAPI* pWndProc)(HWND, UINT, WPARAM, LPARAM),
		LPCTSTR menuName = NULL,
		int x = CW_USEDEFAULT, int y = 0,
		int width = CW_USEDEFAULT, int height = 0,
		UINT classStyle = CS_HREDRAW | CS_VREDRAW,
		DWORD windowStyle = WS_SYSMENU | WS_MINIMIZEBOX,
		HWND hParent = NULL);
	HWND GetHWnd() { return hWnd; }
	void ShiftWindow(HWND hWnd, int dX = 0, int dY = 0,
		int dW = 0, int dH = 0);

	void ShiftWindow(HWND hChild, HWND hParent, int dX = 0,
		int dY = 0, int dW = 0, int dH = 0);

	void ShiftWindow(int ctrlID, HWND hParent, int dX = 0,
		int dY = 0, int dW = 0, int dH = 0);
};

KWnd::KWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
	LRESULT(WINAPI* pWndProc)(HWND, UINT, WPARAM, LPARAM),
	LPCTSTR menuName, int x, int y, int width, int height,
	UINT classStyle, DWORD windowStyle, HWND hParent)
{
	TCHAR szClassName[] = _T("KWndClass");
	wc.cbSize = sizeof(wc);
	wc.style = classStyle;
	wc.lpfnWndProc = pWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(68, 68, 68));
	wc.lpszMenuName = menuName;
	wc.lpszClassName = szClassName;

	if (!RegisterClassEx(&wc))
	{
		TCHAR msg[100] = _T("Cannot register class: ");
		_tcscat_s(msg, szClassName);
		MessageBox(NULL, msg, L"Error", MB_OK);
		return;
	}
	hWnd = CreateWindowEx(WS_EX_CONTROLPARENT, szClassName, windowName, windowStyle,
		x, y, width, height, hParent, (HMENU)NULL, hInst, NULL);

	if (!hWnd)
	{
		TCHAR text[100] = _T("Cannot register class: ");
		_tcscat_s(text, szClassName);
		MessageBox(NULL, text, L"Error", MB_OK);
		return;
	}

	ShowWindow(hWnd, cmdShow);
}

void ShiftWindow(HWND hWnd, int dX, int dY, int dW, int dH)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int x0 = rect.left + dX;
	int y0 = rect.top + dY;
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;
	MoveWindow(hWnd, x0, y0, width, height, TRUE);
}

void ShiftWindow(HWND hChild, HWND hParent, int dX, int dY, int dW, int dH)
{
	RECT rect;
	POINT p0;
	GetWindowRect(hChild, &rect);
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;
	p0.x = rect.left + dX;
	p0.y = rect.top + dY;
	ScreenToClient(hParent, &p0);
	MoveWindow(hChild, p0.x, p0.y, width, height, TRUE);
}

void ShiftWindow(int ctrlID, HWND hParent, int dX, int dY, int dW, int dH)
{
	RECT rect;
	HWND hCtrl = GetDlgItem(hParent, ctrlID);
	POINT p0;
	GetWindowRect(hCtrl, &rect);
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;
	p0.x = rect.left + dX;
	p0.y = rect.top + dY;
	ScreenToClient(hParent, &p0);
	ShowWindow(hCtrl, SW_HIDE);
	MoveWindow(hCtrl, p0.x, p0.y, width, height, TRUE);
	ShowWindow(hCtrl, SW_SHOW);
}