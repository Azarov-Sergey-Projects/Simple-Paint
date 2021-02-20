#pragma once
#include <Windows.h>
#include <vector>
#include "DialogPen.h"
class Object
{
public:
	POINT ptBeg;
	POINT ptEnd;
	HPEN Pen;
	HBRUSH Brush;
	void Clear()
	{
		ptBeg.x = ptEnd.x = ptEnd.y = ptBeg.y = 0;
	}
};


class Rect :public Object
{
public:
	Rect()
	{

	}
	Rect(POINT _ptBeg,POINT _ptEnd,HPEN _Pen,HBRUSH _Brush)
	{
		ptBeg = _ptBeg;
		ptEnd = _ptEnd;
		Pen = _Pen;
		Brush = _Brush;
	}
	void DrawOutLine(HWND hWnd)
	{
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		SetROP2(hDC, R2_MERGEPENNOT);
		SelectObject(hDC, Brush);
		Rectangle(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		ReleaseDC(hWnd, hDC);
	}
	~Rect()
	{
		DeleteObject(Pen);
		DeleteObject(Brush);
	}
};

class Circle :public Object
{
public:
	Circle()
	{

	}
	Circle(POINT _ptBeg, POINT _ptEnd, HPEN _Pen, HBRUSH _Brush)
	{
		ptBeg = _ptBeg;
		ptEnd = _ptEnd;
		Pen = _Pen;
		Brush = _Brush;
	}
	void DrawOutline(HWND hWnd)
	{
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		SetROP2(hDC, R2_MERGEPENNOT);
		SelectObject(hDC, Brush);
		Ellipse(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		ReleaseDC(hWnd, hDC);
	}
	~Circle()
	{
		DeleteObject(Pen);
		DeleteObject(Brush);
	}
};

class Line :public Object
{
public:
	Line()
	{

	}
	Line(POINT _ptBeg, POINT _ptEnd, HPEN _Pen, HBRUSH _Brush)
	{
		ptBeg = _ptBeg;
		ptEnd = _ptEnd;
		Pen = _Pen;
		Brush = _Brush;
	}
	void DrawOutline(HWND hWnd)
	{
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		SetROP2(hDC, R2_MERGEPENNOT);
		MoveToEx(hDC, ptBeg.x, ptBeg.y, NULL);
		LineTo(hDC, ptEnd.x, ptEnd.y);
		ReleaseDC(hWnd, hDC);
	}
	~Line()
	{
		DeleteObject(Pen);
		DeleteObject(Brush);
	}
};

class ContinuousLine :public Object
{
public:
	std::vector<POINT> Line;
	POINT pt;
	ContinuousLine()
	{

	}
	ContinuousLine(std::vector<POINT> _Line, HPEN _Pen, HBRUSH _Brush)
	{
		Line = _Line;
		Pen = _Pen;
		Brush = _Brush;
	}
	void DrawOutline(HWND hWnd)
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
