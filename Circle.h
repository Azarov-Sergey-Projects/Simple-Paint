#pragma once
#include "Shape.h"


class Circle :virtual public Object
{
public:
	Circle() = default;
	Circle(POINT _ptBeg, POINT _ptEnd, HPEN _Pen, HBRUSH _Brush)
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
		Ellipse(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		ReleaseDC(hWnd, hDC);
	}
	~Circle()
	{
		DeleteObject(Pen);
		DeleteObject(Brush);
	}
	void Clear()
	{
		ptBeg.x = ptEnd.x = ptEnd.y = ptBeg.y = 0;
	}
};

