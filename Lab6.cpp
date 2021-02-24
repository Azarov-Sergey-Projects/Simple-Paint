﻿// Lab6.cpp : Определяет точку входа для приложения.
//

#include <Windows.h>
#include <vector>
#include <assert.h>

#include "Rectangle.h"
#include "Circle.h"
#include "ContinuousLine.h"
#include "Line.h"
#include "Bitmap.h"
#include "OpenSaving.h"
#include "DialogPen.h"
#include "DialogBrush.h"
#include "OpenSaving.h"
#include "ButtonBar.h"
#include "AboutProc.h"
#include "Lab6.h"
#include "framework.h"


//основные функции
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//позиции мшки
static HPEN Pen;
//характеристики заливки
static HBRUSH MyBrush;
//для загрузки файлов 
HBITMAP hBitmap;
//главное окно и окно ToolBar
HWND hWnd;
static HWND ToolBar;


void SetParameters(Rect &myRect, Line &myLine, Circle &myCircle, ContinuousLine &mycLine, BOOL rect, BOOL line, BOOL circle, BOOL cLine);
void WMCreateCanvas(HWND hWnd, HDC& hdcMem)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    HDC hdc = GetDC(hWnd);
    hdcMem = CreateCompatibleDC(hdc);
    const int width = rc.right - rc.left;
    const int height = rc.bottom - rc.top;
    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, width, height, hdc, 0, 0, WHITENESS);
    ReleaseDC(hWnd, hdc);
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    TCHAR szClassName[] = TEXT("Мой класс");
    MSG msg;
    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB6);;
    wc.lpszClassName = szClassName;
    wc.cbWndExtra = NULL;
    wc.cbClsExtra = NULL;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.hInstance = hInst;
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("Не получилось зарегистрировать класс!"), TEXT("Ошибка"), MB_OK);
        return NULL;
    }
    hWnd = CreateWindow(szClassName, TEXT("Лабораторая работа 6"), WS_OVERLAPPEDWINDOW | WS_VSCROLL, CW_USEDEFAULT, NULL, CW_USEDEFAULT, NULL, static_cast<HWND>(NULL), NULL, static_cast<HINSTANCE>(hInst), NULL);
    if (!hWnd)
    {
        MessageBox(NULL, TEXT("Не получилось создать окно!"), TEXT("Ошибка"), MB_OK);
        return NULL;
    }
    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC = GetDC(hWnd);
    HDC hMemDC = CreateCompatibleDC(hDC);
    RECT rect = { 0 };
    static PAINTSTRUCT ps;
    static Rect myRect;
    static Line myLine;
    static Circle myCircle;
    static ContinuousLine myContinuousLine;
    switch (uMsg)
    {
    case WM_CREATE:
        InvalidateRect(hWnd, NULL, TRUE);
        GetClientRect(hWnd, &rect);
        ToolBar = CreateSimpleToolbar(hWnd);
        hDC = GetDC(hWnd);
        hMemDC = CreateCompatibleDC(hDC);
        WMCreateCanvas(hWnd, hMemDC);
        ReleaseDC(hWnd, hDC);
        break;
    case WM_MOUSEMOVE:
        if (myRect.Draw)
        {
            if (myRect.isDown)
            {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                myRect.DrawOutLine(hWnd);
                myRect.ptEnd.x = LOWORD(lParam);
                myRect.ptEnd.y = HIWORD(lParam);
                myRect.DrawOutLine(hWnd);
            }
        }
        if (myLine.Draw)
        {
            if (myLine.isDown)
            {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                myLine.DrawOutLine(hWnd);
                myLine.ptEnd.x = LOWORD(lParam);
                myLine.ptEnd.y = HIWORD(lParam);
                myLine.DrawOutLine(hWnd);
            }
        }
        if (myCircle.Draw)
        {
            if (myCircle.isDown)
            {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                myCircle.DrawOutLine(hWnd);
                myCircle.ptEnd.x = LOWORD(lParam);
                myCircle.ptEnd.y = HIWORD(lParam);
                myCircle.DrawOutLine(hWnd);
            }
        }
        if (myContinuousLine.Draw)
        {
            if (myContinuousLine.isDown)
            {
                myContinuousLine.pt.x = LOWORD(lParam);
                myContinuousLine.pt.y = HIWORD(lParam);
                if (myContinuousLine.pt.x <= rect.left+bSize || myContinuousLine.pt.y <= rect.top+bSize)
                {
                    SendMessageW(hWnd, WM_LBUTTONDOWN, wParam, lParam);
                    ReleaseCapture();
                }
                else
                {
                    SetCapture(hWnd);
                    myContinuousLine.Line.push_back(myContinuousLine.pt);
                    myContinuousLine.DrawOutLine(hWnd);
                }
            }
        }
        break;
    case WM_LBUTTONDOWN:
        if (myRect.Draw)
        {
            myRect.ptBeg.x = myRect.ptEnd.x = LOWORD(lParam);
            myRect.ptBeg.y = myRect.ptEnd.y = HIWORD(lParam);
            myRect.Pen = Pen;
            myRect.Brush = MyBrush;
            myRect.DrawOutLine(hWnd);
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            myRect.isDown = TRUE;
        }
        if (myLine.Draw)
        {
            myLine.ptBeg.x = myLine.ptEnd.x = LOWORD(lParam);
            myLine.ptBeg.y = myLine.ptEnd.y = HIWORD(lParam);
            myLine.Pen = Pen;
            myLine.DrawOutLine(hWnd);
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            myLine.isDown = TRUE;
        }
        if (myCircle.Draw)
        {
            myCircle.ptBeg.x = myCircle.ptEnd.x = LOWORD(lParam);
            myCircle.ptBeg.y = myCircle.ptEnd.y = HIWORD(lParam);
            myCircle.Pen = Pen;
            myCircle.Brush = MyBrush;
            myCircle.DrawOutLine(hWnd);
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            myCircle.isDown = TRUE;
        }
        if (myContinuousLine.Draw)
        {
            SetCapture(hWnd);
            myContinuousLine.isDown = TRUE;
            myContinuousLine.Line.clear();
            myContinuousLine.pt.x = LOWORD(lParam);
            myContinuousLine.pt.y = HIWORD(lParam);
            myContinuousLine.Line.push_back(myContinuousLine.pt);
            myContinuousLine.Pen = Pen;
            myContinuousLine.DrawOutLine(hWnd);
        }
        break;
    case WM_LBUTTONUP:
        if (myRect.Draw)
        {
            hDC = GetDC(hWnd);
            myRect.DrawOutLine(hWnd);
            GetClientRect(hWnd, &rect);
            BitBlt(hMemDC, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            myRect.isDown = FALSE;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        if (myLine.Draw)
        {
             myLine.DrawOutLine(hWnd);
             SetCursor(LoadCursor(NULL, IDC_ARROW));
             myLine.isDown = FALSE;
             InvalidateRect(hWnd, NULL, TRUE);
        }
        if (myCircle.Draw)
        {
             myCircle.DrawOutLine(hWnd);
             SetCursor(LoadCursor(NULL, IDC_ARROW));
             myCircle.isDown = FALSE;
             InvalidateRect(hWnd, NULL, TRUE);
        }
        if (myContinuousLine.Draw)
        {
            ReleaseCapture();
            myContinuousLine.isDown = FALSE;
            myContinuousLine.DrawOutLine(hWnd);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
   case WM_COMMAND:
    {
       switch (LOWORD(wParam))
       {
       case IDM_PEN:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, FALSE, FALSE);
           DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProcPen);
           break;
       case IDM_BRUSH:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, FALSE, FALSE);
           DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, DlgProcBrush);
           break;
       case IDM_ABOUT:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, FALSE, FALSE);
           DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
           break;
       case IDM_NEW:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, FALSE, FALSE);
           InvalidateRect(hWnd, NULL, TRUE);
           DeleteObject(hBitmap);
           myContinuousLine.Clear();
           break;
       case IDM_OPEN:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, FALSE, FALSE);
           InvalidateRect(hWnd, NULL, TRUE);
           hBitmap=Open(hWnd,bSize);
           break;
       case IDM_SAVE:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, FALSE, FALSE);
           ShowWindow(ToolBar, SW_HIDE);
           Save(hWnd,bSize);
           ShowWindow(ToolBar, TRUE);
           break;
       case IDM_CIRCLE:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, TRUE, FALSE);
           break;
       case IDM_LINE:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, TRUE, FALSE, FALSE);
           break;
       case IDM_RECT:
           SetParameters(myRect, myLine, myCircle, myContinuousLine,TRUE, FALSE, FALSE, FALSE);
           break;
       case IDM_СONTINUOUS_LINE:
           SetParameters(myRect, myLine, myCircle, myContinuousLine, FALSE, FALSE, FALSE, TRUE);
           break;
       case IDM_EXIT:
           DestroyWindow(hWnd);
           break;
       }
    }
    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rect);
        MyBrush = CreateSolidBrush(RGB(pRedBrush, pGreenBrush, pBlueBrush));
        if (pNullBrush)
        {
            Pen = CreatePen(PS_NULL, pSize, RGB(pRed, pGreen, pBlue));
        }
        else
        {
            Pen = CreatePen(PS_SOLID, pSize, RGB(pRed, pGreen, pBlue));
        }
        BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
        myRect.DrawOutLine(hWnd);
        myRect.Clear();
        myLine.DrawOutLine(hWnd);
        myLine.Clear();
        myCircle.DrawOutLine(hWnd);
        myCircle.Clear();
        myContinuousLine.DrawOutLine(hWnd);
        myContinuousLine.Clear();
        EndPaint(hWnd, &ps);
        break;
    case WM_SIZE:
        SendMessage(ToolBar, TB_AUTOSIZE, 0, 0);
        break;
    case WM_DESTROY:
        DeleteObject(hBitmap);
        DeleteObject(Pen);
        DeleteObject(MyBrush);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

void SetParameters(Rect &myRect,Line &myLine,Circle &myCircle,ContinuousLine &mycLine,BOOL rect, BOOL line, BOOL circle, BOOL cLine)
{
    myRect.Draw = rect;
    myLine.Draw = line;
    myCircle.Draw = circle;
    mycLine.Draw = cLine;
}