#pragma once
#include "Shape.h"
class ContinuousLine :virtual public Object
{
public:

	std::vector<POINT> Line;
	POINT pt = { 0 };
	ContinuousLine() = default;
	ContinuousLine(std::vector<POINT> _Line, HPEN _Pen, HBRUSH _Brush)
	{
		Line = _Line;
		Pen = _Pen;
		Brush = _Brush;
	}
	void DrawOutLine(HWND hWnd)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		if (Line.size() > 1)
		{
			HDC hDC;
			hDC = GetDC(hWnd);
			SelectObject(hDC, Pen);
			MoveToEx(hDC, Line[0].x, Line[0].y, NULL);
			for (int i = 1; i < Line.size(); ++i)
			{
				LineTo(hDC, Line[i].x, Line[i].y);
			}
			ReleaseDC(hWnd, hDC);
		}
	}
	void DrawOutLineFin(HWND hWnd, HDC hdcMem, int width, int height)
	{
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		SelectObject(hDC, Brush);
		if (Line.size() > 1)
		{
			MoveToEx(hDC, Line[0].x, Line[0].y, NULL);
			for (int i = 1; i < Line.size(); ++i)
			{
				LineTo(hDC, Line[i].x, Line[i].y);
			}
		}
		BitBlt(hdcMem, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hDC);
	}
	~ContinuousLine()
	{
		DeleteObject(Pen);
		DeleteObject(Brush);
	}
	void Clear()
	{
		Line.clear();
		pt.x = pt.y = 0;
	}
};
