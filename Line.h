#pragma once
#include "Shape.h"


class Line :virtual public Object
{
public:
	Line() = default;
	Line(POINT _ptBeg, POINT _ptEnd, HPEN _Pen, HBRUSH _Brush)
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
		MoveToEx(hDC, ptBeg.x, ptBeg.y, NULL);
		LineTo(hDC, ptEnd.x, ptEnd.y);
		ReleaseDC(hWnd, hDC);
	}
	void DrawOutLineFin(HWND hWnd, HDC hdcMem, int width, int height)
	{
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		SelectObject(hDC, Brush);
		MoveToEx(hDC, ptBeg.x, ptBeg.y, NULL);
		LineTo(hDC, ptEnd.x, ptEnd.y);
		BitBlt(hdcMem, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hDC);
	}
	~Line()
	{
		DeleteObject(Pen);
		DeleteObject(Brush);
	}
	void Clear()
	{
		ptBeg.x = ptEnd.x = ptEnd.y = ptBeg.y = 0;
	}
};

