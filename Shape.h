#pragma once
#include <Windows.h>
#include <vector>
#include "DialogPen.h"
class Object
{
public:
	BOOL isDown;
	BOOL Draw;
	POINT ptBeg;
	POINT ptEnd;
	HPEN Pen;
	HBRUSH Brush;
	virtual void Clear() = 0;
	virtual void DrawOutLine(HWND hWnd) = 0;
};

