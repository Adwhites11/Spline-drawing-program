#include "curves.h"
#include <cmath>
DWORD Ermit(HDC hdc, POINT pt[], int q, int s, int X, int Y)
{
	int x1, y1;
	int x, y;
	double t1, t2, t3, t4;
	double sp;
	s = s;
	for (int i = 0; i < (q - 1) / 3; i++)
	{
		if (i == 0)
		{
			x1 = pt[i * 3].x * s;
			y1 = pt[i * 3].y * s;
			MoveToEx(hdc, X + x1, Y - y1, NULL);
		}
		sp = sqrt(pow(pt[i + 3].x - pt[i].x, 2) + pow(pt[i + 3].y - pt[i].y, 2));
		sp /= GetSystemMetrics(SM_CXSCREEN) * 2;
		for (double t = 0.0; t < 1.0; t += sp)
		{
			t1 = (1.0 - 3.0 * pow(t, 2.0) + 2.0 * pow(t, 3.0));
			t2 = (t - 2.0 * pow(t, 2.0) + pow(t, 3.0));
			t3 = (-1.0 * pow(t, 2.0) + pow(t, 3.0));
			t4 = (3.0 * pow(t, 2.0) - 2.0 * pow(t, 3));
			x = t1 * pt[i * 3].x * s + t2 * (3.0 * (pt[i * 3 + 1].x * s - pt[i * 3].x * s)) + t3 * (3.0 * (pt[i * 3 + 3].x * s - pt[i * 3 + 2].x * s)) + t4 * (pt[i * 3 + 3].x * s);
			y = t1 * pt[i * 3].y * s + t2 * (3.0 * (pt[i * 3 + 1].y * s - pt[i * 3].y * s)) + t3 * (3.0 * (pt[i * 3 + 3].y * s - pt[i * 3 + 2].y * s)) + t4 * (pt[i * 3 + 3].y * s);
			LineTo(hdc, X + x, Y - y);
			MoveToEx(hdc, X + x, Y-  y, NULL);
		}
	}
	return 0;
}


DWORD B_spline(HDC hdc, POINT pt[], int q, int s, int X, int Y)
{
	int x1, y1;
	int x, y;
	double a0, a1, a2, a3, b0, b1, b2, b3;
	double sp;

	for (int i = 1; i < q - 2; i++)
	{
		sp = sqrt(pow(pt[i + 1].x - pt[i].x, 2) + pow(pt[i + 1].y - pt[i].y, 2));
		sp /= GetSystemMetrics(SM_CXSCREEN) * 2;
		a0 = ((pt[i - 1].x + 4.0 * pt[i].x + pt[i + 1].x) / 6.0) * s;
		b0 = ((pt[i - 1].y + 4.0 * pt[i].y + pt[i + 1].y) / 6.0) * s;

		a1 = ((-1.0 * pt[i - 1].x + pt[i + 1].x) * 0.5) * s;
		b1 = ((-1.0 * pt[i - 1].y + pt[i + 1].y) * 0.5) * s;

		a2 = ((pt[i - 1].x - 2.0 * pt[i].x + pt[i + 1].x) * 0.5) * s;
		b2 = ((pt[i - 1].y - 2.0 * pt[i].y + pt[i + 1].y) * 0.5) * s;

		a3 = ((-1.0 * pt[i - 1].x + 3.0 * pt[i].x - 3.0 * pt[i + 1].x + pt[i + 2].x) / 6.0) * s;
		b3 = ((-1.0 * pt[i - 1].y + 3.0 * pt[i].y - 3.0 * pt[i + 1].y + pt[i + 2].y) / 6.0) * s;

		for (double t = 0.0; t < 1.0; t += sp)
		{
			if (t == 0.0000)
			{
				x1 = ((a3 * t + a2) * t + a1) * t + a0;
				y1 = ((b3 * t + b2) * t + b1) * t + b0;
				MoveToEx(hdc, X + x1, Y - y1, NULL);
				continue;
			}
			x = ((a3 * t + a2) * t + a1) * t + a0;
			y = ((b3 * t + b2) * t + b1) * t + b0;
			LineTo(hdc, X + x, Y - y);
			MoveToEx(hdc, X + x, Y - y, NULL);
		}
	}
	return 0;
}

DWORD Bezier(HDC hdc, POINT pt[], int q, int s, int X, int Y)
{
	int x1, y1;
	int x, y;
	double t1, t2, t3, t4;
	int p_size = 2;
	double sp;
	for (int i = 0; i < (q - 1) / 3; i++)
	{
		sp = sqrt(pow(pt[i + 3].x - pt[i].x, 2) + pow(pt[i + 3].y - pt[i].y, 2));
		sp /= GetSystemMetrics(SM_CXSCREEN) * 2;
		if (i == 0)
		{
			x1 = pt[i * 3].x * s;
			y1 = pt[i * 3].y * s;
			MoveToEx(hdc, X + x1, Y - y1, NULL);
		}
		for (double t = 0.0; t < 1.0; t += sp)
		{
			t1 = (pow((1 - t), 3.0));
			t2 = (3.0 * t * pow((t - 1.0), 2.0));
			t3 = (3 * pow(t, 2) * (1 - t));
			t4 = pow(t, 3);
			x = t1 * (pt[i * 3].x * s) + t2 * (pt[i * 3 + 1].x * s) + t3 * (pt[i * 3 + 2].x * s) + t4 * (pt[i * 3 + 3].x * s);
			y = t1 * (pt[i * 3].y * s) + t2 * (pt[i * 3 + 1].y * s) + t3 * (pt[i * 3 + 2].y * s) + t4 * (pt[i * 3 + 3].y * s);
			LineTo(hdc, X + x, Y - y);
			MoveToEx(hdc, X + x, Y - y, NULL);
		}
	}
	return 0;
}


DWORD Cat_Rom(HDC hdc, POINT pt[], int q, int s, int X, int Y)
{
	int x1, y1;
	double x, y;
	double t1, t2, t3, t4;
	int p_size = 2;
	double sp;
	for (int i = 1; i < q - 2; i++)
	{
		sp = sqrt(pow(pt[i + 1].x - pt[i].x, 2) + pow(pt[i + 1].y - pt[i].y, 2));
		sp /= (double)GetSystemMetrics(SM_CXSCREEN) * 2;
		x1 = pt[i].x * s;
		y1 = pt[i].y * s;
		MoveToEx(hdc, X + x1, Y - y1, NULL);
		for (double t = 0.0; t < 1.0; t += sp)
		{
			t1 = -1.0 * t * pow((1.0 - t), 2.0);
			t2 = 2.0 - 5.0 * pow(t, 2.0) + 3.0 * pow(t, 3.0);
			t3 = t * (1.0 + 4.0 * t - 3.0 * pow(t, 2));
			t4 = pow(t, 2) * (1.0 - t);
			x = 0.5 * ((t1 * (pt[i - 1].x * s) + t2 * (pt[i].x * s) + t3 * (pt[i + 1].x * s) - t4 * (pt[i + 2].x * s)));
			y = 0.5 * ((t1 * (pt[i - 1].y * s) + t2 * (pt[i].y * s) + t3 * (pt[i + 1].y * s) - t4 * (pt[i + 2].y * s)));
			LineTo(hdc, X + x, Y - y);
			MoveToEx(hdc, X + x, Y - y, NULL);
		}
	}
	return 0;
}