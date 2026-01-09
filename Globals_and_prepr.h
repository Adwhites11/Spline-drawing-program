#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
using namespace std;
#include "ChWnd.h"

struct ThreadManager
{
	HWND hWnd;
	HDC hdc;
	int x, y;
	int step;
};

enum UserMsg { UM_THREAD_DONE = WM_USER + 1, UM_OUT_OF_FOCUS = WM_USER + 2 };

static HANDLE hThread;
static HINSTANCE hInst;
POINT* pt;
static OPENFILENAME file;
static int mode;

HWND hColorDlg;
static HWND hEditBox;
static HWND hPaint;
static HWND hButtons[5];

static ChWnd hEdit;
static ChWnd hButton[5];
static ChWnd hPain;

const wchar_t* er = L"Недействительное количество точек";

static RECT bc[5];

static int x, y;
static int hIncr, wIncr;
static int W, H;
static int xB, yB, wB, hB;
static int a, b;
static int xBut, yBut, wBut, hBut;
static int xE, yE, wE, hE;
static int sms;
static bool FocusBut;
static bool on_focus[5];

const wchar_t* sm[50] = { L"1", L"2", L"3", L"4", L"5", L"6", L"7",
	L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15", L"16", L"17",
	L"18", L"19", L"20", L"21", L"22", L"23", L"24", L"25", L"26",
	L"27", L"28", L"29", L"30", L"31", L"32", L"33", L"34", L"35",
	L"36", L"37", L"38", L"39", L"40", L"41", L"42", L"43", L"44",
	L"45", L"46", L"47", L"48", L"49", L"50"};

const wchar_t* sm_min[50] = { L"-1", L"-2", L"-3", L"-4", L"-5",
	L"-6", L"-7", L"-8", L"-9", L"-10", L"-11", L"-12", L"-13",
	L"-14", L"-15", L"-16", L"-17", L"-18", L"-19", L"-20", L"-21",
	L"-22", L"-23", L"-24", L"-25", L"-26",	L"-27", L"-28", L"-29",
	L"-30", L"-31", L"-32", L"-33", L"-34", L"-35",	L"-36", L"-37",
	L"-38", L"-39", L"-40", L"-41", L"-42", L"-43", L"-44",	L"-45",
	L"-46", L"-47", L"-48", L"-49", L"-50" };

const wchar_t* but[5] = { L"Эрмитова кривая", L"B - сплайн",
	L"Кривая Безье", L"Кривая Catmull-Rom", L"Сброс" };