#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <tchar.h>
#include <cmath>
using namespace std;

struct state
{
	bool changed;
	bool repaint;
	int j;

};

struct Point
{
	int x;
	int y;
	Point(int xa, int ya) : x(xa), y(ya) {}
	Point() {}
};

class Coordinates
{
private:
	vector<Point> coordFile;
	vector<wstring> coordinates;
	int w, h;
	int max;
public:
	int yStep; 
	SCROLLINFO vsi;
	int vertRange;
	POINT* pt;
	void SetSize(int wE, int hE, int yS) { w = wE; h = hE; yStep = yS; }
	state Open(const wchar_t* name);
	void ScrollSettings(HWND hwnd, int height);
	void UpdateVscroll(HWND hwnd, int yInc);
	void PutText(HWND hwnd, HDC hdc);
	void erasec(){ coordinates.erase(coordinates.begin(), coordinates.end()); }
	int rmax() { return max; }
};

state Coordinates::Open(const wchar_t* name)
{
	wfstream f;
	state s;
	int x, y;
	static wchar_t buffer[500];
	wstring c;
	coordinates.erase(coordinates.begin(), coordinates.end());
	max = 0;
	f.open(name, ios::in);
	if (!f.is_open())
	{
		s.changed = false; 
		s.repaint = true;
		s.j = 0;
		max = 16;
		return s;
	}
		

	if (coordFile.size() != 0)
		coordFile.erase(coordFile.begin(), coordFile.end());
	s.changed = true;
	s.j = 0;
	while (!f.eof())
	{
		f >> x;
		if (f.eof()) break;
		max = max > abs(x) ? max : abs(x);
		f >> y;
		max = max > abs(y) ? max : abs(y);
		coordFile.push_back(Point(x, y));
		s.j++;
	}
	max += 1;
	f.close();

	if (pt) delete[] pt;
	pt = new POINT[s.j];
	for (int i = 0; i < s.j; i++)
	{
		pt[i].x = coordFile[i].x;
		pt[i].y = coordFile[i].y;
	}

	s.repaint = true;
	for (int i = 0; i < coordFile.size(); i++)
	{
		_stprintf_s(buffer, L"%d, %d;   ",
			coordFile[i].x, coordFile[i].y);
		c.append(buffer);
		if (c.size() > 170)
		{
			coordinates.push_back(wstring(c));
			c.erase(c.begin(), c.end());
		}
		else if (i == coordFile.size() - 1)
		{
			coordinates.push_back(wstring(c));
			c.erase(c.begin(), c.end());
		}

	}
	return s;
}
void Coordinates::ScrollSettings(HWND hwnd, int height)
{
	vsi.cbSize = sizeof(vsi);
	vsi.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	vsi.nPage = height / yStep;
	vsi.nMax = (int) coordinates.size() - 1;
	if ((int)vsi.nPage > vsi.nMax)
		vsi.nPos = vsi.nMin;
	vertRange = vsi.nMax - vsi.nMin + 1;
	SetScrollInfo(hwnd, SB_VERT, &vsi, TRUE);
}
void Coordinates::UpdateVscroll(HWND hwnd, int yInc)
{
	yInc = min(yInc, vertRange - (int)vsi.nPage - vsi.nPos);
	yInc = max(yInc, vsi.nMin - vsi.nPos);
	if (yInc) {
		ScrollWindow(hwnd, 0, -yStep * yInc, NULL, NULL);
		vsi.nPos += yInc;
		SetScrollInfo(hwnd, SB_VERT, &vsi, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
	}
}
void Coordinates::PutText(HWND hWnd, HDC hdc)
{
	RECT rt;
	static HFONT Font, OldFont;

	SetBkMode(hdc, TRANSPARENT);
	
	int y = 0;
	int amountLines = (int) coordinates.size();
	int iBeg = vsi.nPos;
	int iEnd = (vsi.nPos + (int) vsi.nPage < amountLines) ? vsi.nPos + vsi.nPage :
		amountLines;
	GetClientRect(hWnd, &rt);
	Font = CreateFontW(15, 0, 0, 0, 400, FALSE, FALSE, FALSE,
		204, 7, 0, 2, FF_SWISS | 0x2, L"New\0");
	OldFont = (HFONT)SelectObject(hdc, Font);

	SetTextColor(hdc, RGB(255, 255, 255));

	
	for (int i = iBeg; i < iEnd; ++i)
	{
		SetRect(&rt, 0, y, w, (int) coordinates[i].size());
		DrawText(hdc, coordinates[i].c_str(), -1, &rt, DT_LEFT);
		y += yStep;
	}

	SelectObject(hdc, OldFont);
	DeleteObject(Font);
}