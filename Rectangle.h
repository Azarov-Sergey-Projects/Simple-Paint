#pragma once
#include "Shape.h"
class Rect :public Object
{
public:
	Rect() = default;
	Rect(POINT _ptBeg, POINT _ptEnd, HPEN _Pen, HBRUSH _Brush)
	{
		ptBeg = _ptBeg;
		ptEnd = _ptEnd;
		Pen = _Pen;
		Brush = _Brush;
	}
	void DrawOutLine(HWND hWnd)
	{
		RECT rect;
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		SetROP2(hDC, R2_MERGEPENNOT);
		SelectObject(hDC, Brush);
		Rectangle(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		GetClientRect(hWnd, &rect);

		BitBlt(hMemDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.right, hDC, rect.left, rect.top, SRCCOPY);

		ReleaseDC(hWnd, hDC);
	}
	~Rect()
	{
		DeleteObject(Pen);
		DeleteObject(Brush);
	}
	void Clear()
	{
		ptBeg.x = ptEnd.x = ptEnd.y = ptBeg.y = 0;
	}

};
