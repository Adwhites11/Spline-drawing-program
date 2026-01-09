#include <windows.h>
#include <process.h>
#include "KWnd.h"
#include "curves.h"
#include "Globals_and_prepr.h"
#include "inline_fun.h"
#include "vscr.h"
#include "resource.h"
#include <dwmapi.h>

#pragma comment (lib, "Dwmapi")

static state st;
static Coordinates coords;

unsigned _stdcall Paint(void* param);

LRESULT CALLBACK ChildProcEditBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildProcPaint(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildProcButton(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	SetMetrics(x, y, W, H, xB, yB, wB, hB, a, b, xBut, yBut, wBut, hBut,
		xE, yE, wE, hE, wIncr, hIncr);
	
	MSG msg;
	KWnd mainWnd(L"Spliner", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(101), 200, 50, W, H); 

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static wchar_t buf[23];
	static wchar_t name[256];
	int wmId;
	static BOOL USE_DARK_MODE = true;
	static BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS;
	switch (uMsg)
	{
	case WM_CREATE:
		SET_IMMERSIVE_DARK_MODE_SUCCESS = DwmSetWindowAttribute(
			hWnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
			&USE_DARK_MODE, sizeof(USE_DARK_MODE));
		hInst = GetModuleHandle(NULL);
		coords.SetSize(wE, hE, 16);
		coords.vsi.nMin = coords.vsi.nPos = 0;
		for (int i = 0; i < 5; i++)
		{
			_stprintf_s(buf, L"Button %d", i);
			if (!hButton[i].SetWnd(buf, hInst, ChildProcButton,
				0, 0, 0, 0, NULL, CS_HREDRAW | CS_VREDRAW,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				hWnd, NULL))
				return 1;
			hButtons[i] = hButton[i].GethWnd();
		}

		if (!hPain.SetWnd(L"Paint Window", hInst, ChildProcPaint,
			0, 0, 0, 0, WS_EX_STATICEDGE,
			CS_HREDRAW | CS_VREDRAW, WS_CHILD | WS_VISIBLE,
			hWnd, CreateSolidBrush(RGB(33, 33, 33))))
			return 1;
		hPaint = hPain.GethWnd();

		if (!hEdit.SetWnd(L"Edit Box", hInst, ChildProcEditBox,
			0, 0, 0, 0, WS_EX_STATICEDGE,
			CS_HREDRAW | CS_VREDRAW, WS_CHILD | WS_VISIBLE | WS_VSCROLL,
			hWnd, CreateSolidBrush(RGB(60, 60, 60))))
			return 1;
		hEditBox = hEdit.GethWnd();

		file.lStructSize = sizeof(OPENFILENAME);
		file.hInstance = hInst;
		file.lpstrFilter = L"Text\0*.txt";
		file.lpstrFile = name;
		file.nMaxFile = 256;
		file.lpstrInitialDir = L".\\";
		file.lpstrDefExt = L"txt";

		st.changed = false;
		st.repaint = true;

		st = coords.Open(L"coordinates.txt");
		sms = coords.rmax();

		InvalidateRect(hEditBox, NULL, 1);
		SendMessage(hEditBox, WM_SIZE, NULL, NULL);

		InvalidateRect(hPaint, NULL, 1);
		
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_OPEN:
			file.lpstrTitle = L"Открыть файл для чтения";
			file.Flags = OFN_HIDEREADONLY;
			if (!GetOpenFileName(&file)) return 1;

			st = coords.Open(name);
			sms = coords.rmax();

			InvalidateRect(hEditBox, NULL, 1);
			SendMessage(hEditBox, WM_SIZE, NULL, NULL);

			if (st.changed)
			{
				st.repaint = true;
				SendMessage(hPaint, WM_PAINT, NULL, NULL);
			}
			break;
		case ID_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC) AboutDlgProc);
			break;
		}
		break;
	case WM_SIZE:
		for (int i = 0; i < 5; i++)
		{
			MoveWindow(hButtons[i], xBut, yBut + (i + 1) * ((hB-16) / 7),
				wBut, hBut, TRUE);
			SetRect(&bc[i], xBut, yBut + (i + 1) * ((hB - 16) / 7),
				xBut + wBut, yBut + (i + 1) * ((hB - 16) / 7) + hBut);
			UpdateWindow(hButtons[i]);
		}

		MoveWindow(hPaint, xB, yB, wB, hB, TRUE);
		UpdateWindow(hPaint);

		MoveWindow(hEditBox, xE, yE, wE, hE, TRUE);
		UpdateWindow(hEditBox);
		break;
	case WM_MOUSEMOVE:
		if (FocusBut == true)
		{
			FocusBut = false;
			for (int i = 0; i < 5; i++)
			{
				on_focus[i] = false;
				SendMessage(hButtons[i], WM_LBUTTONUP, NULL, NULL);
				SendMessage(hButtons[i], UM_OUT_OF_FOCUS, i, NULL);
			}	
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ChildProcButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	HFONT Font, OldFont;
	HPEN hCant;
	static POINT contur[5];
	HBRUSH hbutBr;
	static int r, g, b;
	static int st1, st2;
	static int i;
	int j;
	static int rgb[5][3];
	switch (uMsg)
	{
	case WM_CREATE:
		AnimBut(r, g, b, st1, st2, 110, 110, 110, 1, 0);
		for (int i = 0; i < 5; i++)
		{
			on_focus[i] = false;
			for (int j = 0; j < 3; j++)
				rgb[i][j] = 60;
		}
		FocusBut = false;
		break;
	case WM_LBUTTONDOWN:
		for (i = 0; hWnd != hButtons[i]; i++);
		AnimBut(r, g, b, st1, st2, 30, 30, 30, 0, 1);
		InvalidateRect(hWnd, NULL, 1);
		if (!pt) break;
		if (i == mode) break;
		if (i != mode && i < 5)
		{
			st.repaint = true;
			mode = i;
			SendMessage(hPaint, WM_PAINT, NULL, NULL);
		}		
		break;
	case WM_LBUTTONUP:
		AnimBut(r, g, b, st1, st2, 110, 110, 110, 1, 0);
		InvalidateRect(hWnd, NULL, 1);
		break;
	case WM_MOUSEMOVE:
		for (j = 0; hWnd != hButtons[j]; j++);

		FocusBut = true;

		for (int k = 0; k < 5; k++)
			on_focus[k] = false;
		
		on_focus[j] = true;

		for (int k = 0; k < 5; k++)
		{
			if (on_focus[k] == false && rgb[k][0] == 80)
			{
				rgb[k][0] = rgb[k][1] = rgb[k][2] = 60;
				SendMessage(hButtons[k], UM_OUT_OF_FOCUS, k, NULL);
			}
		}
		
		if (rgb[j][0] == 80 && rgb[j][1] == 80 && rgb[j][2] == 80)
			break;
		else
		{
			rgb[j][0] = rgb[j][1] = rgb[j][2] = 80;
			SendMessage(hButtons[j], UM_OUT_OF_FOCUS, j, NULL);
		}
		break;
	case UM_OUT_OF_FOCUS:
		i = (int) wParam;
		hdc = GetDC(hButtons[i]);
		if (on_focus[i] == false)
			rgb[i][0] = rgb[i][1] = rgb[i][2] = 60;
		
		GetClientRect(hButtons[i], &rt);

		hbutBr = CreateSolidBrush(RGB(rgb[i][0], rgb[i][1], rgb[i][2]));
		FillRect(hdc, &rt, hbutBr);
		DeleteObject(hbutBr);

		SetBkMode(hdc, TRANSPARENT);

		hCant = CreatePen(PS_SOLID, 1, RGB(r, g, b));
		SelectObject(hdc, hCant);

		SetCont(contur, rt, st1, st2);
		Polyline(hdc, contur, 5);

		Font = CreateFontW(14, 0, 0, 0, 400, FALSE, FALSE, FALSE,
			204, 7, 0, 2, FF_SWISS | 0x2, L"New\0");
		OldFont = (HFONT)SelectObject(hdc, Font);
		SetTextColor(hdc, RGB(255, 255, 255));

		AnimText(rt, st1, st2);
		DrawText(hdc, but[i], -1, &rt,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		SelectObject(hdc, OldFont);
		DeleteObject(Font);
		DeleteObject(hCant);
		ReleaseDC(hButtons[i], hdc);
		break;
	case WM_PAINT:
		for (i = 0; hWnd != hButtons[i]; i++);
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rt);

		hbutBr = CreateSolidBrush(RGB(rgb[i][0], rgb[i][1], rgb[i][2]));
		FillRect(hdc, &rt, hbutBr);
		DeleteObject(hbutBr);

		SetBkMode(hdc, TRANSPARENT);
		
		hCant = CreatePen(PS_SOLID, 1, RGB(r, g, b));
		SelectObject(hdc, hCant);

		SetCont(contur, rt, st1, st2);
		Polyline(hdc, contur, 5);

		Font = CreateFontW(14, 0, 0, 0, 400, FALSE, FALSE, FALSE,
			204, 7, 0, 2, FF_SWISS | 0x2, L"New\0");
		OldFont = (HFONT)SelectObject(hdc, Font);
		SetTextColor(hdc, RGB(255, 255, 255));

		AnimText(rt, st1, st2);
		DrawText(hdc, but[i], -1, &rt,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		SelectObject(hdc, OldFont);
		DeleteObject(Font);
		DeleteObject(hCant);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		break;
	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ChildProcEditBox(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static int yInc;

	switch (uMsg)
	{
	case WM_SIZE:
		hdc = GetDC(hWnd);
		coords.ScrollSettings(hWnd, hE);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			yInc = -1; break;
		case SB_LINEDOWN:
			yInc = 1; break;
		case SB_PAGEUP:
			yInc = -(int)coords.vsi.nPage; break;
		case SB_PAGEDOWN:
			yInc = (int)coords.vsi.nPage; break;
		case SB_THUMBTRACK:
			yInc = HIWORD(wParam) - coords.vsi.nPos; break;
		default: yInc = 0;
		}
		coords.UpdateVscroll(hWnd, yInc);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		coords.PutText(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ChildProcPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static ThreadManager _tm;
	HDC hdc;
	PAINTSTRUCT ps;
	static HDC memBit;
	static HBITMAP hBitmap;
	RECT rt;
	static HFONT Font, OldFont;
	HPEN hCoord, hCant;
	static HBRUSH hFon;
	static HPEN hEr, hB_s, hBe, hC_R;
	static int step;
	static int x, y;
	switch (uMsg)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd);
		memBit = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, wB, hB);
		SelectObject(memBit, hBitmap);
		SetGraphicsMode(memBit, GM_ADVANCED);
		ReleaseDC(hWnd, hdc);
		_tm.hWnd = hWnd;
		_tm.hdc = memBit;

		hEr = CreatePen(PS_SOLID, 3, RGB(136, 184, 228));
		hB_s = CreatePen(PS_SOLID, 3, RGB(82, 181, 127));
		hBe = CreatePen(PS_SOLID, 3, RGB(254, 226, 184));
		hC_R = CreatePen(PS_SOLID, 3, RGB(150, 43, 235));

		mode = 4;
		break;
	case UM_THREAD_DONE:
		hdc = GetDC(hWnd);
		BitBlt(hdc, 0, 0, x * 2, y * 2, memBit, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hCoord = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		hCant = CreatePen(PS_SOLID, 1, RGB(33, 33, 33));
		if (st.repaint)
		{
			x = wB / 2;
			y = hB / 2;
			
			hFon = CreateSolidBrush(RGB(33, 33, 33));
			Font = CreateFontW(int (1.5 / (double) sms * 130.0), 0, 0, 0, 400, FALSE, FALSE, FALSE,
				204, 7, 0, 2, FF_SWISS | 0x2, L"New\0");
			OldFont = (HFONT)SelectObject(memBit, Font);
			SelectObject(memBit, Font);
			SelectObject(memBit, hFon);
			SelectObject(memBit, hCant);

			Rectangle(memBit, 0, 0, x * 2, y * 2);
			
			SelectObject(memBit, hCoord);
			MoveToEx(memBit, x, 0, NULL);
			LineTo(memBit, x, y * 2);
			MoveToEx(memBit, 0, y, NULL);
			LineTo(memBit, x * 2, y);

			step = int ((double) x / (double)sms / 1.01);
			_tm.x = x;
			_tm.y = y;
			_tm.step = step;
			SetBkMode(memBit, TRANSPARENT);
			SetTextColor(memBit, RGB(255, 255, 255));
			DeleteObject(hFon);

			for (int j = 1; j <= sms; j++)
			{
				// +y
				SetRect(&rt, int ((double) x - (double)wIncr * 1.2), int ((double) y - j * step - hIncr / 2), x, y - j * step + hIncr);
				DrawText(memBit, sm[j - 1], -1, &rt, DT_CENTER);
				MoveToEx(memBit, x - 7, x - j * step, NULL);
				LineTo(memBit, x + 7, x - j * step);

				// -y
				SetRect(&rt, x + 4, y + j * step - hIncr / 2, int ((double) x + (double)wIncr * 1.2), y + j * step + hIncr);
				DrawText(memBit, sm_min[j - 1], -1, &rt, DT_CENTER);
				MoveToEx(memBit, x - 7, x + j * step, NULL);
				LineTo(memBit, x + 7, x + j * step);

				// +x
				SetRect(&rt, x + j * step - hIncr, int ((double) y - (double)wIncr * 0.7), x + j * step + hIncr, y - 5);
				DrawText(memBit, sm[j - 1], -1, &rt, DT_CENTER);
				MoveToEx(memBit, x + j * step, y - 7, NULL);
				LineTo(memBit, x + j * step, y + 7);

				//-x
				SetRect(&rt, int ((double)x - (double) j * (double) step - (double) hIncr * 1.3), y + 10, x - j * step + hIncr, int ((double) y + (double) wIncr * 1.2));
				DrawText(memBit, sm_min[j - 1], -1, &rt, DT_CENTER);
				MoveToEx(memBit, x - j * step, y - 7, NULL);
				LineTo(memBit, x - j * step, y + 7);
			}
			if (pt) delete[] pt;
			pt = new POINT[st.j];
			for (int i = 0; i < st.j; i++)
			{
				pt[i].x = coords.pt[i].x;
				pt[i].y = coords.pt[i].y ;
			}
			switch (mode)
			{
			case 0:
				SelectObject(_tm.hdc, hEr);
				break;
			case 1:
				SelectObject(_tm.hdc, hB_s);
				break;
			case 2:
				SelectObject(_tm.hdc, hBe);
				break;
			case 3:
				SelectObject(_tm.hdc, hC_R);
				break;
			case 4: break;
			}
			hThread = (HANDLE)_beginthreadex(NULL, 0, Paint, &_tm,
				0, NULL);
			

			st.repaint = false;
		}
		BitBlt(hdc, 0, 0, x * 2, y * 2, memBit, 0, 0, SRCCOPY);
		SelectObject(memBit, OldFont);
		DeleteObject(Font);
		DeleteObject(hCant);
		DeleteObject(hCoord);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		CloseHandle(hThread);
		DeleteDC(memBit);
		DeleteObject(hEr);
		DeleteObject(hB_s);
		DeleteObject(hBe);
		DeleteObject(hC_R);
		break;
	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

unsigned _stdcall Paint(void* param)
{
	ThreadManager* pTm = (ThreadManager*)param;
	
	DWORD s = 1;
	switch (mode)
	{
	case 0:
		s = Ermit(pTm->hdc, pt, st.j, pTm->step, pTm->x, pTm->y);
		break;
	case 1:
		s = B_spline(pTm->hdc, pt, st.j, pTm->step, pTm->x, pTm->y);
		break;
	case 2:
		s = Bezier(pTm->hdc, pt, st.j, pTm->step, pTm->x, pTm->y);
		break;
	case 3:
		s = Cat_Rom(pTm->hdc, pt, st.j, pTm->step, pTm->x, pTm->y);
		break;
	case 4: break;
	default: s = 1; break;
	}
	if (mode >= 0 && mode <= 3)
	{
		int p_size = 2;
		HPEN hWhi = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
		HPEN hOldPen = (HPEN)SelectObject(pTm->hdc, hWhi);
		HBRUSH hBr = CreateSolidBrush(RGB(255, 255, 255));
		SelectObject(pTm->hdc, hBr);
		for (int i = 0; i < st.j; i++)
			Rectangle(pTm->hdc, pTm->x + pt[i].x * pTm->step - p_size, pTm->y - pt[i].y * pTm->step + p_size,
				pTm->x + pt[i].x * pTm->step + p_size, pTm->y - pt[i].y * pTm->step - p_size);
		DeleteObject(SelectObject(pTm->hdc, hOldPen));
		DeleteObject(hBr);
	}
	
	SendMessage(hPaint, UM_THREAD_DONE, NULL, NULL);
	return s;
}

BOOL CALLBACK AboutDlgProc(HWND hWnd, UINT uMsg, 
	WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}