// Lab6.cpp : Определяет точку входа для приложения.
//

#include <Windows.h>
#include <vector>
#include <map>
#include <tuple>
#include <assert.h>


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

//рисование объектов которые уже были
void Draw(HDC hDC, std::vector<std::tuple<INT, RECT,HPEN,HBRUSH,BOOL,BOOL>>obj);

//нажатие на вид 
static BOOL DrawRect = FALSE;
static BOOL DrawLine = FALSE;
static BOOL DrawCircle = FALSE;
static BOOL DrawContinuousLine = FALSE;
static BOOL isDown = FALSE;
//счетчик нажатий
static INT count = 0;
//позиции мшки
static int xPosOld;
static int yPosOld;
static int yPosNew;
static int xPosNew;
static HPEN Pen;
//характеристики заливки
static HBRUSH MyBrush;

static POINT BrushPoint;
//для загрузки файлов 
static HBITMAP hBitmap;
HWND hWnd;

HWND Brush = NULL;
static HWND ToolBar;



static std::vector<std::vector<POINT>>LinePoints;
static std::vector<INT> vCount;
static INT NumberOfLines = 0;



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

void DrawBoxOutline(HWND hwnd, POINT ptBeg, POINT ptEnd)
{
    HDC hDC;  
    hDC = GetDC(hwnd);
    SetROP2(hDC, R2_NOT);
    SelectObject(hDC, GetStockObject(NULL_BRUSH));
    Rectangle(hDC, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
    ReleaseDC(hwnd, hDC);
}

void DrawLineOutline(HWND hwnd, POINT ptBeg, POINT ptEnd)
{
    HDC hDC;
    hDC = GetDC(hwnd);
    SetROP2(hDC, R2_NOT);
    SelectObject(hDC, GetStockObject(NULL_BRUSH));
    MoveToEx(hDC, ptBeg.x, ptBeg.y,NULL);
    LineTo(hDC, ptEnd.x, ptEnd.y);
    ReleaseDC(hwnd, hDC);
}

void DrawCircleOutline(HWND hwnd, POINT ptBeg, POINT ptEnd)
{
    HDC hdc;
    hdc = GetDC(hwnd);
    SetROP2(hdc, R2_NOT);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Ellipse(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
    ReleaseDC(hwnd, hdc);
}

void DrawLineContinuous(HWND hWnd, std::vector<POINT> Line)
{
    HDC hDC;
    hDC = GetDC(hWnd);
    MoveToEx(hDC, Line[0].x, Line[0].y, NULL);
    SelectObject(hDC, Pen);
    for (int i = 1; i < Line.size(); ++i)
    {
        LineTo(hDC, Line[i].x, Line[i].y);
    }
    ReleaseDC(hWnd, hDC);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL fBlocking, fValidBox;
    static POINT ptBeg, ptEnd, ptBoxBeg, ptBoxEnd;
    static std::vector<std::tuple<INT, RECT,HPEN,HBRUSH,BOOL,BOOL>> obj;
    static std::vector<POINT> continuousLine;
    static RECT coord;
    HDC hDC;
    HDC bmpDC;
    static PAINTSTRUCT ps;
    HDC hMemDC;
    RECT rect;
    switch (uMsg)
    {
    case WM_CREATE:
        InvalidateRect(hWnd, NULL, TRUE);
        Pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
        ToolBar = CreateSimpleToolbar(hWnd);
        continuousLine.clear();
        break;
    case WM_MOUSEMOVE:
        if (DrawRect)
        {
            if (fBlocking)
            {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                DrawBoxOutline(hWnd, ptBeg, ptEnd);
                ptEnd.x = LOWORD(lParam);
                ptEnd.y = HIWORD(lParam);
                DrawBoxOutline(hWnd, ptBeg, ptEnd);
            }
        }
        if (DrawLine)
        {
            if (fBlocking)
            {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                DrawLineOutline(hWnd, ptBeg, ptEnd);
                ptEnd.x = LOWORD(lParam);
                ptEnd.y = HIWORD(lParam);
                DrawLineOutline(hWnd, ptBeg, ptEnd);
            }
        }
        if (DrawCircle)
        {
            if (fBlocking)
            {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
                DrawCircleOutline(hWnd, ptBeg, ptEnd);
                ptEnd.x = LOWORD(lParam);
                ptEnd.y = HIWORD(lParam);
                DrawCircleOutline(hWnd, ptBeg, ptEnd);
            }
        }
        if (DrawContinuousLine)
        {
            if (isDown)
            {
                POINT pt;
                GetClientRect(hWnd, &rect);
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);
                continuousLine.push_back(pt);
                DrawLineContinuous(hWnd, continuousLine);
            }
        }
        break;
    case WM_LBUTTONDOWN:
        count++;
        if (DrawRect)
        {
            ptBeg.x = ptEnd.x = LOWORD(lParam);
            ptBeg.y = ptEnd.y = HIWORD(lParam);
            DrawBoxOutline(hWnd, ptBeg, ptEnd);
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            fBlocking = TRUE;
        }
        if (DrawLine)
        {
            ptBeg.x = ptEnd.x = LOWORD(lParam);
            ptBeg.y = ptEnd.y = HIWORD(lParam);
            DrawLineOutline(hWnd, ptBeg, ptEnd);
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            fBlocking = TRUE;
        }
        if (DrawCircle)
        {
            ptBeg.x = ptEnd.x = LOWORD(lParam);
            ptBeg.y = ptEnd.y = HIWORD(lParam);
            DrawCircleOutline(hWnd, ptBeg, ptEnd);
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            fBlocking = TRUE;
        }
        if (DrawContinuousLine)
        {
            continuousLine.clear();
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            continuousLine.push_back(pt);
            isDown = true;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    case WM_LBUTTONUP:
        if (DrawRect)
        {
            if (fBlocking)
            {
                DrawBoxOutline(hWnd, ptBeg, ptEnd);
                ptBoxBeg = ptBeg;
                ptBoxEnd.x = LOWORD(lParam);
                ptBoxEnd.y = HIWORD(lParam);
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                fBlocking = FALSE;
                fValidBox = TRUE;
                InvalidateRect(hWnd, NULL, TRUE);
                count--;
            }
        }
        if (DrawLine)
        {
            if (fBlocking)
            {
                DrawLineOutline(hWnd, ptBeg, ptEnd);
                ptBoxBeg = ptBeg;
                ptBoxEnd.x = LOWORD(lParam);
                ptBoxEnd.y = HIWORD(lParam);
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                fBlocking = FALSE;
                fValidBox = TRUE;
                InvalidateRect(hWnd, NULL, TRUE);
                count--;
            }
        }
        if (DrawCircle)
        {
            if (fBlocking)
            {
                DrawCircleOutline(hWnd, ptBeg, ptEnd);
                ptBoxBeg = ptBeg;
                ptBoxEnd.x = LOWORD(lParam);
                ptBoxEnd.y = HIWORD(lParam);
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                fBlocking = FALSE;
                fValidBox = TRUE;
                InvalidateRect(hWnd, NULL, TRUE);
                count--;
            }
        }
        if (DrawContinuousLine)
        {
            isDown = FALSE;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
   case WM_COMMAND:
    {
       switch (LOWORD(wParam))
       {
       case IDM_PEN:
           DrawContinuousLine = FALSE;
           fValidBox = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           hBrush = FALSE;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProcPen);
           break;
       case IDM_BRUSH:
           DrawContinuousLine = FALSE;
           fValidBox = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           DrawCircle = FALSE;
           hBrush = TRUE;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, DlgProcBrush);
           break;
       case IDM_ABOUT:
           DrawContinuousLine = FALSE;
           hBrush = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           count = 0;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
           break;
       case IDM_NEW:
           DrawContinuousLine = FALSE;
           hBrush = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           obj.resize(0);
           LinePoints.resize(0);
           InvalidateRect(hWnd, NULL, TRUE);
           DeleteObject(hBitmap);
           fValidBox = FALSE;
           count = 0;
           break;
       case IDM_OPEN:
           DrawContinuousLine = FALSE;
           hBrush = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           count = 0;
           InvalidateRect(hWnd, NULL, TRUE);
           obj.resize(0);
           fValidBox = FALSE;
           hBitmap=Open(hWnd,bSize);
           break;
       case IDM_SAVE:
           DrawContinuousLine = FALSE;
           hBrush = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           count = 0;
           ShowWindow(ToolBar, SW_HIDE);
           Save(hWnd,bSize);
           ShowWindow(ToolBar, TRUE);
           break;
       case IDM_CIRCLE:
           if (DrawLine || DrawRect)
           {
               ptBeg.x = ptEnd.x = 0;
               ptBeg.y = ptEnd.y = 0;
               count = 0;
           }
           DrawContinuousLine = FALSE;
           DrawCircle = TRUE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           hBrush = FALSE;
           count++;
           break;
       case IDM_LINE:
           if (DrawCircle || DrawRect)
           {
               ptBeg.x = ptEnd.x = 0;
               ptBeg.y = ptEnd.y = 0;
               count = 0;
           }
           DrawContinuousLine = FALSE;
           DrawLine = TRUE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           count++;
           break;
       case IDM_RECT:
           if (DrawLine || DrawCircle)
           {
               ptBeg.x = ptEnd.x = 0;
               ptBeg.y = ptEnd.y = 0;
               count = 0;
           }
           DrawContinuousLine = FALSE;
               hBrush = FALSE;
               DrawLine = FALSE;
               DrawCircle = FALSE;
               DrawRect = TRUE;
               count++;
           break;
       case IDM_СONTINUOUS_LINE:
           if (DrawLine || DrawCircle||DrawRect)
           {
               ptBeg.x = ptEnd.x = 0;
               ptBeg.y = ptEnd.y = 0;
               count = 0;
           }
           hBrush = FALSE;
           DrawLine = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawContinuousLine = TRUE;
           break;
      
       case IDM_EXIT:
           DestroyWindow(hWnd);
           break;
       }
    }
    case WM_PAINT:
    { 
        hDC = BeginPaint(hWnd, &ps);
        static BOOL myBool = FALSE;
        MyBrush = CreateSolidBrush(RGB(pRedBrush, pGreenBrush, pBlueBrush));
        if (!pNullBrush)
        {
            Pen = CreatePen(PS_SOLID, pSize, RGB(pRed, pGreen, pBlue));
        }
        else
        {
            Pen = CreatePen(PS_NULL, pSize, RGB(pRed, pGreen, pBlue));
        }
      
        Draw(hDC, obj); 
        if (DrawRect)
        {
            if (fValidBox)
            {
                SelectObject(hDC, Pen);
                if (hBrushButton)
                {
                    myBool = TRUE;
                    SelectObject(hDC, GetStockObject(NULL_BRUSH));
                }
                else 
                {
                    myBool = FALSE;
                    SelectObject(hDC, MyBrush);
                }
                Rectangle(hDC, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
                rect.left = ptBeg.x;
                rect.top = ptBeg.y;
                rect.right = ptEnd.x;
                rect.bottom = ptEnd.y;
                obj.push_back({ IDM_RECT,rect,Pen,MyBrush,myBool,pNullBrush });
            }
           if (fBlocking)
            {
               InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        if (DrawLine)
        {
            if (fValidBox)
            {
                SelectObject(hDC, Pen);
                MoveToEx(hDC, ptBeg.x, ptBeg.y, NULL);
                LineTo(hDC, ptEnd.x, ptEnd.y);
                rect.left = ptBeg.x;
                rect.top = ptBeg.y;
                rect.right = ptEnd.x;
                rect.bottom = ptEnd.y;
                obj.push_back({ IDM_LINE,rect,Pen,MyBrush,myBool,pNullBrush});
            }
            if (fBlocking)
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        if (DrawCircle)
        {
            if (fValidBox)
            {
                SelectObject(hDC, Pen);
                if (hBrushButton)
                {
                    myBool = TRUE;
                    SelectObject(hDC, GetStockObject(NULL_BRUSH));
                }
                else
                {
                    myBool = FALSE;
                    SelectObject(hDC, MyBrush);
                }
                Ellipse(hDC, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
                rect.left = ptBeg.x;
                rect.top = ptBeg.y;
                rect.right = ptEnd.x;
                rect.bottom = ptEnd.y;
                obj.push_back({ IDM_CIRCLE,rect,Pen,MyBrush,myBool,pNullBrush});
            }
            if (fBlocking)
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        if (DrawContinuousLine)
        {
            if (continuousLine.size() > 1)
            {
                if (pNullBrush)
                {
                    SelectObject(hDC, GetStockObject(NULL_BRUSH));
                }
                else
                {
                    SelectObject(hDC, Pen);
                }
                MoveToEx(hDC, continuousLine[0].x, continuousLine[0].y, NULL);
                for (size_t i = 1; i < continuousLine.size(); ++i) {
                    LineTo(hDC, continuousLine[i].x, continuousLine[i].y);
                  
                }
                obj.push_back({ IDM_СONTINUOUS_LINE,rect,Pen,MyBrush,myBool,pNullBrush });
                LinePoints.push_back(continuousLine);
            }
        }
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SIZE:
        SendMessage(ToolBar, TB_AUTOSIZE, 0, 0);
        break;
    case WM_DESTROY:
        DeleteObject(hBitmap);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

void Draw(HDC hDC, std::vector<std::tuple<INT, RECT, HPEN, HBRUSH, BOOL,BOOL>> obj)
{
    HBRUSH Fill;
    HPEN drawPen;
    HDC hMemDC;
    BITMAP bm;
    hMemDC = CreateCompatibleDC(hDC);
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hDC, NULL, bSize, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
    static BOOL nPen;
    for (int i = 0; i < obj.size(); i++)
    {
        static RECT rect;
        static INT num;
        if (std::get<0>(obj[i]) == 98)
        {
            drawPen = std::get<2>(obj[i]);
            SelectObject(hDC, drawPen);
            rect = std::get<1>(obj[i]);
            MoveToEx(hDC, rect.left, rect.top, NULL);
            LineTo(hDC, rect.right, rect.bottom);
        }
        else if (std::get<0>(obj[i]) == 97)
        {
            nPen = std::get<5>(obj[i]);
            rect = std::get<1>(obj[i]);
            Fill = std::get<3>(obj[i]);
            drawPen = std::get<2>(obj[i]);
            SelectObject(hDC, drawPen);
           
            if (std::get<4>(obj[i]))
            {
                SelectObject(hDC, GetStockObject(NULL_BRUSH));
            }
            else
            {
                SelectObject(hDC, Fill);
            }
            Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
        }
        else if (std::get<0>(obj[i]) == 99)
        {
            rect = std::get<1>(obj[i]);
            Fill = std::get<3>(obj[i]);
            drawPen = std::get<2>(obj[i]);
            SelectObject(hDC, drawPen);
            if (std::get<4>(obj[i]))
            {
                SelectObject(hDC, GetStockObject(NULL_BRUSH));
            }
            else
            {
                SelectObject(hDC, Fill);
            }
            Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);
        }
        else if (std::get<0>(obj[i]) == 94)
        {
            drawPen = std::get<2>(obj[i]);
            SelectObject(hDC, drawPen);
            for (int i = 0; i < LinePoints.size(); i++)
            {
                MoveToEx(hDC, LinePoints[i][0].x, LinePoints[i][0].y, NULL);
                for (int j = 0; j < LinePoints[i].size(); j++)
                {
                    LineTo(hDC, LinePoints[i][j].x, LinePoints[i][j].y);
                }
            }
        }
    }
    DeleteObject(hMemDC);
}
