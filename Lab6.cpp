// Lab6.cpp : Определяет точку входа для приложения.
//

#include <Windows.h>
#include <commctrl.h>
#include "Lab6.h"
#include "framework.h"
#include "resource1.h"
#include <assert.h>
#include <vector>
#include <tuple>



#pragma comment(lib, "Comctl32.lib")

//основные функции
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateSimpleToolbar(HWND hWndParent);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
//сохранение файла
void CreateBMPFile(LPCWSTR pszFile, HBITMAP hBMP);
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
void Save(HWND hWnd);
//открытие файла
void Open(HWND hWnd);
//рисование объектов которые уже были
void Draw(HDC hDC, std::vector<std::tuple<INT, RECT,HPEN>> obj);

HINSTANCE hInst;



//характеристики кнопок
const int ImageListID = 0;
const DWORD buttonStyles = BTNS_AUTOSIZE;
const INT bSize = 45;
//нажатие на вид 
static BOOL DrawRect = FALSE;
static BOOL DrawLine = FALSE;
static BOOL DrawCircle = FALSE;
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
//для загрузки файлов 
static HBITMAP hBitmap;
HWND hWnd;
HWND DialogBox=NULL;
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

TBBUTTON tbButtons[] =
{
    { MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("New"))},
    { MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Open"))},
    { MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Save"))},
    { MAKELONG(STD_DELETE,   ImageListID), IDM_EXIT, TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Exit"))},
    { MAKELONG(NULL,ImageListID),IDM_RECT,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Rectangle"))},
    { MAKELONG(NULL,ImageListID),IDM_LINE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Line"))},
    { MAKELONG(NULL,ImageListID),IDM_CIRCLE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Circle"))},
    { MAKELONG(NULL,ImageListID),IDM_PEN,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Pen"))},
    { MAKELONG(NULL,ImageListID),IDM_BRUSH,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Brush"))},
};
HIMAGELIST g_hImageList = NULL;






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

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HBRUSH Brush;
    static BOOL fBlocking, fValidBox;
    static POINT ptBeg, ptEnd, ptBoxBeg, ptBoxEnd;
    static std::vector<std::tuple<INT, RECT,HPEN>> obj;
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
        break;
   case WM_COMMAND:
    {
       switch (LOWORD(wParam))
       {
       case IDM_PEN:
           fValidBox = FALSE;
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, static_cast<DLGPROC>(DlgProc));
           break;
       case IDM_BRUSH:
           DrawRect = FALSE;
           DrawLine = FALSE;
           DrawCircle = FALSE;
           break;
       case IDM_ABOUT:
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           count = 0;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
           break;
       case IDM_NEW:
           DrawCircle = FALSE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           obj.resize(0);
           InvalidateRect(hWnd, NULL, TRUE);
           DeleteObject(hBitmap);
           fValidBox = FALSE;
           count = 0;
           break;
       case IDM_OPEN:
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
           DrawCircle = TRUE;
           DrawRect = FALSE;
           DrawLine = FALSE;
           count++;
           break;
       case IDM_LINE:
           if (DrawCircle || DrawRect)
           {
               ptBeg.x = ptEnd.x = 0;
               ptBeg.y = ptEnd.y = 0;
               count = 0;
           }
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
               DrawLine = FALSE;
               DrawCircle = FALSE;
               DrawRect = TRUE;
               count++;
           break;
      
       case IDM_EXIT:
           DestroyWindow(hWnd);
           break;
       }
   }
    case WM_PAINT:
    { 
        hDC = BeginPaint(hWnd, &ps);
        Pen = CreatePen(BS_SOLID, pSize, RGB(pRed, pGreen, pBlue));
        SelectObject(hDC, Pen);
        Draw(hDC, obj);
      
        if (DrawRect)
        {
            if (fValidBox)
            {
                SelectObject(hDC, Pen);
                Rectangle(hDC, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
                rect.left = ptBeg.x;
                rect.top = ptBeg.y;
                rect.right = ptEnd.x;
                rect.bottom = ptEnd.y;
                obj.push_back({ IDM_RECT,rect,Pen });
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
                MoveToEx(hDC, ptBeg.x, ptBeg.y, NULL);
                LineTo(hDC, ptEnd.x, ptEnd.y);
                rect.left = ptBeg.x;
                rect.top = ptBeg.y;
                rect.right = ptEnd.x;
                rect.bottom = ptEnd.y;
                obj.push_back({ IDM_LINE,rect,Pen });
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
                Ellipse(hDC, ptBoxBeg.x, ptBoxBeg.y, ptBoxEnd.x, ptBoxEnd.y);
                rect.left = ptBeg.x;
                rect.top = ptBeg.y;
                rect.right = ptEnd.x;
                rect.bottom = ptEnd.y;
                obj.push_back({ IDM_CIRCLE,rect,Pen });
            }
            if (fBlocking)
            {
                InvalidateRect(hWnd, NULL, TRUE);
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
    const int numButtons = 9;
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
    BITMAP bmp;
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

void Draw(HDC hDC, std::vector<std::tuple<INT, RECT,HPEN>> obj)
{
    HPEN drawPen;
    HDC hMemDC;
    BITMAP bm;
    hMemDC = CreateCompatibleDC(hDC);
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hDC, NULL, bSize, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
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
            drawPen = std::get<2>(obj[i]);
            SelectObject(hDC, drawPen);
            rect = std::get<1>(obj[i]);
            Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
        }
        else if (std::get<0>(obj[i]) == 99)
        {
            drawPen = std::get<2>(obj[i]);
            SelectObject(hDC, drawPen);
            rect = std::get<1>(obj[i]);
            Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);
        }
    }
    DeleteObject(hMemDC);
}

BOOL CALLBACK DlgProc(HWND DialogBox, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
       
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_SIZE_LINE:
            pSize = GetDlgItemInt(DialogBox, IDC_SIZE_LINE, NULL, FALSE);
            break;
        case IDC_UP_TEXT:
            pSize++;
            break;
        case IDC_DOWN_TEXT:
            pSize--;
            break;
        case IDC_GREEN:
            pGreen = GetDlgItemInt(DialogBox, IDC_GREEN, NULL, FALSE);
            break;
        case IDC_RED:
           pRed = GetDlgItemInt(DialogBox, IDC_RED, NULL, FALSE);
            break;
        case IDC_BLUE:
            pBlue = GetDlgItemInt(DialogBox, IDC_BLUE, NULL, FALSE);
            break;
        case IDC_BUTTON_GREEN:
           pGreen = 255;
           pRed = 0;
           pBlue = 0;
            break;
        case IDC_BUTTON_BLUE:
            pGreen = 0;
            pRed = 0;
            pBlue = 255;
            break;
        case IDC_BUTTON_RED:
            pGreen =0;
            pRed = 255;
            pBlue = 0;
            break;
        case IDOK:
            
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