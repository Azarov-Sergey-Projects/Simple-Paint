// Lab6.cpp : Определяет точку входа для приложения.
//

#include <Windows.h>
#include <commctrl.h>
#include <vector>
#include <map>
#include <tuple>
#include <assert.h>
#include "Lab6.h"
#include "framework.h"
#include "Resource.h"
#include "resource1.h"
#include "resource2.h"

#pragma comment(lib, "Comctl32.lib")

//основные функции
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateSimpleToolbar(HWND hWndParent);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc2(HWND DialogBox, UINT uMsg, WPARAM wParam, LPARAM lParam);
//сохранение файла
void CreateBMPFile(LPCWSTR pszFile, HBITMAP hBMP);
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
void Save(HWND hWnd);
//открытие файла
void Open(HWND hWnd);
//рисование объектов которые уже были
void Draw(HDC hDC, std::vector<std::tuple<INT, RECT,HPEN,HBRUSH,BOOL,BOOL>>obj);

HINSTANCE hInst;



//характеристики кнопок
const int ImageListID = 0;
const DWORD buttonStyles = BTNS_AUTOSIZE;
const INT bSize = 45;
//нажатие на вид 
static BOOL DrawRect = FALSE;
static BOOL DrawLine = FALSE;
static BOOL DrawCircle = FALSE;
static BOOL DrawContinuousLine = FALSE;
static BOOL isDown = FALSE;
//счетчик нажатий
static INT count = 0;
//позиции мішки
static int xPosOld;
static int yPosOld;
static int yPosNew;
static int xPosNew;
//характеристики карандаша
static HPEN Pen;
static INT pSize = 3;
static INT pRed;
static INT pGreen;
static INT pBlue;
static BOOL pNullBrush = FALSE;
//характеристики заливки
static HBRUSH MyBrush;
static BOOL hBrushButton = TRUE;
static BOOL hBrush = FALSE;
static INT pGreenBrush=255;
static INT pRedBrush=255;
static INT pBlueBrush=255;
static POINT BrushPoint;
//для загрузки файлов 
static HBITMAP hBitmap;
HWND hWnd;

HWND Brush = NULL;
static HWND ToolBar;

//идентификаторы ToolBar
constexpr INT IDM_NEW = 100;
constexpr INT IDM_OPEN = 101;
constexpr INT IDM_SAVE = 102;
constexpr INT IDM_RECT = 97;
constexpr INT IDM_LINE = 98;
constexpr INT IDM_CIRCLE = 99;
constexpr INT IDM_PEN = 96;
constexpr INT IDM_BRUSH = 95;
constexpr INT IDM_СONTINUOUS_LINE = 94;
TBBUTTON tbButtons[] =
{
    { MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("New"))},
    { MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Open"))},
    { MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Save"))},
    { MAKELONG(STD_DELETE,   ImageListID), IDM_EXIT, TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Exit"))},
    { MAKELONG(NULL,ImageListID),IDM_RECT,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Rectangle"))},
    { MAKELONG(NULL,ImageListID),IDM_LINE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Line"))},
     { MAKELONG(NULL,ImageListID),IDM_СONTINUOUS_LINE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Continuous Line"))},
    { MAKELONG(NULL,ImageListID),IDM_CIRCLE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Circle"))},
    { MAKELONG(NULL,ImageListID),IDM_PEN,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Pen"))},
    { MAKELONG(NULL,ImageListID),IDM_BRUSH,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Brush"))},
};
HIMAGELIST g_hImageList = NULL;

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
    HDC hdc;
    hdc = GetDC(hwnd);
    SetROP2(hdc, R2_NOT);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
    ReleaseDC(hwnd, hdc);
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
           DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, static_cast<DLGPROC>(DlgProc));
           break;
       case IDM_BRUSH:
           DrawContinuousLine = FALSE;
           fValidBox = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           DrawCircle = FALSE;
           hBrush = TRUE;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, static_cast<DLGPROC>(DlgProc2));
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
           Open(hWnd);
           break;
       case IDM_SAVE:
           DrawContinuousLine = FALSE;
           hBrush = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           count = 0;
           ShowWindow(ToolBar, SW_HIDE);
           Save(hWnd);
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

HWND CreateSimpleToolbar(HWND hWndParent)
{
    const int numButtons = 10;
    const int bitmapSize = 16;
    HWND hWndToolbar = CreateWindowExW(WS_EX_WINDOWEDGE, TOOLBARCLASSNAME, NULL,
        WS_CHILD | TBSTYLE_WRAPABLE, bSize, bSize, bSize,bSize,
        hWndParent, NULL, hInst, NULL);
    if (hWndToolbar == NULL)
        return NULL;
    g_hImageList = ImageList_Create(bitmapSize, bitmapSize,  
        ILC_COLOR16 | ILC_MASK,   
        numButtons, 0);
    SendMessage(hWndToolbar, TB_SETIMAGELIST,
        static_cast<WPARAM>(ImageListID),
        reinterpret_cast<LPARAM>(g_hImageList));
    SendMessage(hWndToolbar, TB_LOADIMAGES,
        static_cast<WPARAM>(IDB_STD_SMALL_COLOR),
        reinterpret_cast<LPARAM>(HINST_COMMCTRL));
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, static_cast<WPARAM>(sizeof(TBBUTTON)), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS, static_cast<WPARAM>(numButtons), reinterpret_cast<LPARAM>(&tbButtons));
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    ShowWindow(hWndToolbar, TRUE);
    return hWndToolbar;
}

void Save(HWND hWnd)
{
    HDC hdc[2];
    HBITMAP hbitmap;
    RECT rect;
    GetClientRect(hWnd, &rect);
    hdc[0] = GetDC(hWnd);
    hbitmap = CreateCompatibleBitmap(hdc[0], rect.right, rect.bottom-bSize);
    hdc[1] = CreateCompatibleDC(hdc[0]);
    SelectObject(hdc[1], hbitmap);
    BitBlt(
        hdc[1],
        rect.left,
        rect.top-bSize,
        rect.right,
        rect.bottom,
        hdc[0],
        0,
        0,
        SRCCOPY
    );
    LPCWSTR fileName = TEXT("Bitmap.bmp");
    CreateBMPFile(fileName, hbitmap);
}

void Open(HWND hWnd)
{
    INT xWinPos;
    INT yWinPos;
    RECT rect;
    GetClientRect(hWnd, &rect);
    xWinPos = rect.left;
    yWinPos = rect.top+ bSize;
    TCHAR szFile[260];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("Bitmap files(*.bmp)\0*.bmp\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);
    if (ofn.lpstrFile == _T("\0")) { return; }
    PAINTSTRUCT ps;
    BITMAP bm;
    HDC hDC;
    HDC hMemDC;
    hDC = GetDC(hWnd);
    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = static_cast<HBITMAP>(LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hDC, xWinPos, yWinPos, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDC);
   // DeleteObject(hBitmap);
    ReleaseDC(hWnd, hDC);
    UpdateWindow(hWnd);
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
    static BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap color format, width, and height.  
    assert(GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp));

    // Convert the color format to a count of bits.  
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure  
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
    // data structures.)  

    if (cClrBits < 24)
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
            sizeof(BITMAPINFOHEADER) +
            sizeof(RGBQUAD) * (1 << cClrBits));

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

    else
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
            sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.  

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.  
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE 
    // compressed. 
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
        * pbmi->bmiHeader.biHeight;
    // Set biClrImportant to 0, indicating that all of the  
    // device colors are important.  
    pbmi->bmiHeader.biClrImportant = 0;
    return pbmi;
}

void CreateBMPFile(LPCWSTR pszFile, HBITMAP hBMP)
{
    HANDLE hf;                 // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE* hp;                   // byte pointer  
    DWORD dwTmp;
    PBITMAPINFO pbi;
    HDC hDC;

    hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
    SelectObject(hDC, hBMP);
    pbi = CreateBitmapInfoStruct(hBMP);
    pbih = reinterpret_cast<PBITMAPINFOHEADER>(pbi);
    lpBits = static_cast<LPBYTE>(GlobalAlloc(GMEM_FIXED, pbih->biSizeImage));

    assert(lpBits);

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    assert(GetDIBits(hDC, hBMP, 0, static_cast<DWORD>(pbih->biHeight), lpBits, pbi,
        DIB_RGB_COLORS));

    // Create the .BMP file.  
    hf = CreateFile(pszFile,
        GENERIC_READ | GENERIC_WRITE,
        static_cast<DWORD>(0),
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        static_cast<HANDLE>(NULL));
    assert(hf != INVALID_HANDLE_VALUE);

    hdr.bfType = 0x4d42;     
    hdr.bfSize = static_cast<DWORD>((sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD) + pbih->biSizeImage));
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = static_cast<DWORD>(sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD));

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    assert(WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
        reinterpret_cast<LPDWORD>(&dwTmp), NULL));

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    assert(WriteFile(hf, static_cast<LPVOID>(pbih), sizeof(BITMAPINFOHEADER)
        + pbih->biClrUsed * sizeof(RGBQUAD),
        reinterpret_cast<LPDWORD>(&dwTmp), (NULL)));

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    assert(WriteFile(hf, reinterpret_cast<LPSTR>(hp), static_cast<int>(cb), reinterpret_cast<LPDWORD>(&dwTmp), NULL));

    // Close the .BMP file.  
    assert(CloseHandle(hf));

    // Free memory.  
    GlobalFree(static_cast<HGLOBAL>(lpBits));

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

BOOL CALLBACK DlgProc(HWND DialogBox, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static INT size = 3;
    static INT Green = 0;
    static INT Red = 0;
    static INT Blue = 0;
    switch (uMsg)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(DialogBox, IDC_STATIC_1, pSize, FALSE);
        SetDlgItemInt(DialogBox, IDC_STATIC_2, pGreen, FALSE);
        SetDlgItemInt(DialogBox, IDC_STATIC_3, pRed, FALSE);
        SetDlgItemInt(DialogBox, IDC_STATIC_4, pBlue, FALSE);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_NULL:
            if (pNullBrush)
            {
                pNullBrush = FALSE;
            }
            else
            {
                pNullBrush = TRUE;
            }
           
            pSize =size= 0;
            Green = 0;
            Red = 0;
            Blue = 0;
            SetDlgItemInt(DialogBox, IDC_STATIC_1, size, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_2, Green, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_3, Red, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_4, Blue, FALSE);
            break;
        case IDC_SIZE_LINE:
            pNullBrush = FALSE;
            size = GetDlgItemInt(DialogBox, IDC_SIZE_LINE, NULL, FALSE);
            break;
        case IDC_UP_TEXT:
            pNullBrush = FALSE;
            size++;
            SetDlgItemInt(DialogBox, IDC_STATIC_1, size, FALSE);
            break;
        case IDC_DOWN_TEXT:
            pNullBrush = FALSE;
            size--;
            SetDlgItemInt(DialogBox, IDC_STATIC_1, size, FALSE);
            if (size == 0)
            {
                MessageBoxW(DialogBox, TEXT("Размер карандаша стал равным 0, размер установлен в 1"), TEXT("Error"), NULL);
                size = 1;
                SetDlgItemInt(DialogBox, IDC_STATIC_1, size, FALSE);
            }
            break;
        case IDC_GREEN:
            pNullBrush = FALSE;
            Green = GetDlgItemInt(DialogBox, IDC_GREEN, NULL, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_2, Green, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_3, Red, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_4, Blue, FALSE);
            break;
        case IDC_RED:
            pNullBrush = FALSE;
            Red = GetDlgItemInt(DialogBox, IDC_RED, NULL, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_2, Green, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_3, Red, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_4, Blue, FALSE);
            break;
        case IDC_BLUE:
            pNullBrush = FALSE;
            Blue = GetDlgItemInt(DialogBox, IDC_BLUE, NULL, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_2, Green, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_3, Red, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_4, Blue, FALSE);
            break;
          
        case IDC_BUTTON_GREEN:
            pNullBrush = FALSE;
            Green = 255;
            Red = 0;
            Blue = 0;
            SetDlgItemInt(DialogBox, IDC_STATIC_2, Green, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_3, Red, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_4, Blue, FALSE);
            break;
        case IDC_BUTTON_BLUE:
            pNullBrush = FALSE;
            Green = 0;
            Red = 0;
            Blue = 255;
            SetDlgItemInt(DialogBox, IDC_STATIC_2, Green, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_3, Red, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_4, Blue, FALSE);
            break;
        case IDC_BUTTON_RED:
            pNullBrush = FALSE;
            Green = 0;
            Red = 255;
            Blue = 0;
            SetDlgItemInt(DialogBox, IDC_STATIC_2, Green, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_3, Red, FALSE);
            SetDlgItemInt(DialogBox, IDC_STATIC_4, Blue, FALSE);
            break;
        case IDOK:
            pSize = size;
            pGreen = Green;
            pRed = Red;
            pBlue = Blue;
            EndDialog(DialogBox, 0);
            return TRUE;
        case WM_CLOSE:
            EndDialog(DialogBox, LOWORD(wParam));
            return TRUE;
            break;
        case IDCANCEL:
            EndDialog(DialogBox, 0);
            return  TRUE;
            break;
        }

    default:return FALSE;
    }
}

BOOL CALLBACK DlgProc2(HWND Brush, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static INT red=255;
    static INT blue=255;
    static INT green=255;
    switch (uMsg)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(Brush, IDC_STATIC1, pRedBrush, FALSE);
        SetDlgItemInt(Brush, IDC_STATIC2, pGreenBrush, FALSE);
        SetDlgItemInt(Brush, IDC_STATIC3, pBlueBrush, FALSE);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RED_BRUSH:
            hBrushButton = FALSE;
            red= GetDlgItemInt(Brush, IDC_RED_BRUSH, NULL, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC1, red, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, green, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, blue, FALSE);
            break;
        case IDC_BLUE_BRUSH:
            hBrushButton = FALSE;
            blue= GetDlgItemInt(Brush, IDC_BLUE_BRUSH, NULL, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC1, red, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, green, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, blue, FALSE);
            break;
        case IDC_GREEN_BRUSH:
            hBrushButton = FALSE;
            green = GetDlgItemInt(Brush, IDC_GREEN_BRUSH, NULL, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC1, red, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, green, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, blue, FALSE);
            break;
        case IDC_BUTTON_BLUE_BRUSH:
            hBrushButton = FALSE;
            blue = 255;
            green = 0;
            red = 0;
            SetDlgItemInt(Brush, IDC_STATIC1, red, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, green, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, blue, FALSE);
            break;
        case IDC_BUTTON_RED_BRUSH:
            hBrushButton = FALSE;
            red = 255;
            blue = 0;
            green = 0;
            SetDlgItemInt(Brush, IDC_STATIC1, red, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, green, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, blue, FALSE);
            break;
        case IDC_BUTTON_GREEN_BRUSH:
            hBrushButton = FALSE;
            green = 255;
            red = 0;
            blue = 0;
            SetDlgItemInt(Brush, IDC_STATIC1, red, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, green, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, blue, FALSE);
            break;
        case IDC_BUTTON_NULL_BRUSH:
            hBrushButton = TRUE;
            SetDlgItemInt(Brush, IDC_STATIC1, 0, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, 0, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, 0, FALSE);
            break;
        case ID_OK:
            pGreenBrush = green;
            pRedBrush = red;
            pBlueBrush = blue;
            EndDialog(Brush, 0);
            return TRUE;
        case WM_CLOSE:
            EndDialog(Brush, LOWORD(wParam));
            return TRUE;
            break;
        case ID_CANCEL:
            EndDialog(Brush, 0);
            return  TRUE;
            break;
        }
    default: return FALSE;
    }
}