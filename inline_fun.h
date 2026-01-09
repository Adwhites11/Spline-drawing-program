#include <Windows.h>

int step = 5;
inline void AnimBut(int& red, int& green, int& blue, int& sh1, int& sh2,
	int nr, int ng, int nb, int shadow1, int shadow2)
{
	red = nr;
	green = ng;
	blue = nb;
	sh1 = shadow1;
	sh2 = shadow2;
}
inline void SetCont(POINT p[], const RECT& rt, int s1, int s2)
{
	p[0].x = rt.left + s2;
	p[0].y = rt.top + s2;
	p[1].x = rt.right - s1;
	p[1].y = rt.top + s2;
	p[2].x = rt.right - s1;
	p[2].y = rt.bottom - s1;
	p[3].x = rt.left + s2;
	p[3].y = rt.bottom - s1;
	p[4].x = rt.left + s2;
	p[4].y = rt.top + s2;
}

inline void AnimText(RECT& rt, int s1, int s2)
{
	rt.left += s2;
	rt.top += s2;
	rt.right -= s1;
	rt.bottom -= s1;
}

inline void SetMetrics(int & x, int& y, int& W, int& H, int& xB, int& yB, int& wB,
	int& hB, int& a, int& b, int& xBut, int& yBut, int& wBut,
	int& hBut, int& xE, int& yE, int& wE, int& hE, int &wIncr, int & hIncr)
{
	x = GetSystemMetrics(SM_CXSCREEN);
	y = GetSystemMetrics(SM_CYSCREEN);
	double s;
	s = (double) x / (double)y;
	if (s <= 1.8 && s >= 1.59)
	{
		W = int((double)(GetSystemMetrics(SM_CXSCREEN) / 100) * 65.3);
		H = int((double)(GetSystemMetrics(SM_CYSCREEN) / 100) * 98);
	}
	if (s <= 1.4 && s >= 1.3)
	{
		W = int((double)(GetSystemMetrics(SM_CXSCREEN) / 100) * 76.7);
		H = int((double)(GetSystemMetrics(SM_CYSCREEN) / 100) * 90);
	}
	if (s <= 2.4 && s >= 2.3)
	{
		W = int((double)(GetSystemMetrics(SM_CXSCREEN) / 100) * 76.7);
		H = int((double)(GetSystemMetrics(SM_CYSCREEN) / 100) * 90);
	}
	

	a = (W - 16) / 10 * 7 - step * 2;
	b = (H - 59) - step * 11;
	xB = (W - 16) / 10 * 3 + step;
	yB = step * 10;
	wB = a >= b ? b : a;
	hB = a >= b ? b : a;

	xBut = (W - 16) / 30;
	yBut = (H - 59) / 20;
	wBut = int((double)GetSystemMetrics(SM_CXSCREEN) / 6.5);
	hBut = int((double)GetSystemMetrics(SM_CYSCREEN) / 25);

	xE = yE = step;
	wE = (W - 16) - step * 2;
	hE = 40;

	wIncr = int((double)GetSystemMetrics(SM_CXSCREEN) / 34);
	hIncr = int((double)GetSystemMetrics(SM_CXSCREEN) / 140);
}

