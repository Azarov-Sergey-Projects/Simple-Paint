#pragma once
#include "Shape.h"
#include "DialogBrush.h"

static BOOL CanDraw = hBrushButton;
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
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		if (hBrushButton)
		{
			SelectObject(hDC, GetStockObject(NULL_BRUSH));
		}
		else
		{
			SelectObject(hDC, Brush);
		}
	
		SetROP2(hDC, R2_MERGEPENNOT);
		Rectangle(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		ReleaseDC(hWnd, hDC);
	}
	void DrawOutLineFin(HWND hWnd, HDC hdcMem, int width, int height)
	{
		HDC hDC;
		hDC = GetDC(hWnd);
		SelectObject(hDC, Pen);
		if (hBrushButton)
		{
			SelectObject(hDC, GetStockObject(NULL_BRUSH));
		}
		else
		{
			SelectObject(hDC, Brush);
		}
		Rectangle(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
		BitBlt(hdcMem, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
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
