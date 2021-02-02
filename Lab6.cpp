// Lab6.cpp : Определяет точку входа для приложения.
//

#include <Windows.h>
#include <commctrl.h>
#include "Lab6.h"
#include "framework.h"
#include <assert.h>
#include <vector>
#include <tuple>



#pragma comment(lib, "Comctl32.lib")


//основные функции
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateSimpleToolbar(HWND hWndParent);

//сохранение файла
void CreateBMPFile(LPCWSTR pszFile, HBITMAP hBMP);
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
void Save(HWND hWnd);
//открытие файла
void Open(HWND hWnd);
//рисование объектов
void dRect(HWND hWnd);
void dLine(HWND hWnd,LPARAM lParam);
void Draw(HDC hDC, std::vector<std::tuple<INT, RECT>> obj);

static HWND ToolBar;


const int ImageListID = 0;
const DWORD buttonStyles = BTNS_AUTOSIZE;
const INT bSize = 45;
static BOOL DrawRect = FALSE;
static BOOL DrawLine = FALSE;
static BOOL DrawCircle = FALSE;
static INT count = 0;
static int xPosOld;
static int yPosOld;
static int yPosNew;
static int xPosNew;
static HBITMAP hBitmap;
HWND hWnd;



constexpr INT IDM_NEW = 100;
constexpr INT IDM_OPEN = 101;
constexpr INT IDM_SAVE = 102;
constexpr INT IDM_RECT = 97;
constexpr INT IDM_LINE = 98;
constexpr INT IDM_CIRCLE = 99;
constexpr INT RESTORE = 96;
TBBUTTON tbButtons[] =
{
    { MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("New"))},
    { MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Open"))},
    { MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Save"))},
    { MAKELONG(STD_DELETE,   ImageListID), IDM_EXIT, TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Exit"))},
    { MAKELONG(NULL,ImageListID),IDM_RECT,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Rectangle"))},
    { MAKELONG(NULL,ImageListID),IDM_LINE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Line"))},
    { MAKELONG(NULL,ImageListID),IDM_CIRCLE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Circle"))},
};
HIMAGELIST g_hImageList = NULL;
HINSTANCE hInst;




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
    static INT xPosNewNew, yPosNewNew;
    static std::vector<std::tuple<INT, RECT>> obj;
    static RECT coord;
    HDC hDC;
    HDC bmpDC;
    static BITMAP bm;
    static PAINTSTRUCT ps;
    static HPEN Pen;
    
    RECT rect;
    switch (uMsg)
    {
    case WM_CREATE:
        InvalidateRect(hWnd, NULL, TRUE);
        Pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
        ToolBar = CreateSimpleToolbar(hWnd);
        break;
    case WM_MOUSEMOVE:
        if (count>=2)
        {
            xPosNew = LOWORD(lParam);
            yPosNew = HIWORD(lParam);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    case WM_LBUTTONDOWN:
        xPosNew = xPosOld = LOWORD(lParam);
        yPosNew = yPosOld = HIWORD(lParam);
        count++;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_LBUTTONUP:
        InvalidateRect(hWnd, NULL, TRUE);
        xPosNewNew = LOWORD(lParam);
        yPosNewNew = HIWORD(lParam);
        count--;
        break;
   case WM_COMMAND:
   {
       switch (LOWORD(wParam))
       {
       case IDM_ABOUT:
           count = 0;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
           break;
       case IDM_NEW:
           InvalidateRect(hWnd, NULL, TRUE);
           obj.resize(0);
           count = 0;
           RedrawWindow(hWnd, NULL, NULL, RDW_ERASE);
           break;
       case IDM_OPEN:
           count = 0;
           InvalidateRect(hWnd, NULL, TRUE);
           Open(hWnd);
           break;
       case IDM_SAVE:
           count = 0;
           ShowWindow(ToolBar, SW_HIDE);
           Save(hWnd);
           ShowWindow(ToolBar, TRUE);
           break;
       case IDM_CIRCLE:
           if (DrawLine || DrawRect)
           {
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
        Draw(hDC, obj);
        if (DrawLine&&count>=2)
        {
            if (xPosOld != xPosNew || yPosOld != yPosNew)
            {
                HPEN OldPen = static_cast<HPEN>(SelectObject(hDC, Pen));
                MoveToEx(hDC, xPosOld, yPosOld, NULL);
                LineTo(hDC, xPosNew, yPosNew);
                SelectObject(hDC, OldPen);
                coord.left = xPosOld;
                coord.top = yPosOld;
                coord.right = xPosNew;
                coord.bottom = yPosNew;
                obj.push_back({ IDM_LINE,coord });
            }
        }
        if (DrawCircle && count >= 2)
        {
            if (xPosOld != xPosNew || yPosOld != yPosNew)
            {
                HPEN OldPen = static_cast<HPEN>(SelectObject(hDC, Pen));
                Ellipse(hDC, xPosOld, yPosOld, xPosNew, yPosNew);
                SelectObject(hDC, OldPen);
                coord.left = xPosOld;
                coord.top = yPosOld;
                coord.right=xPosNew;
                coord.bottom = yPosNew;
                obj.push_back({ IDM_CIRCLE,coord });
              
            }
        }
        if (DrawRect && count >= 2)
        {
            if (xPosOld != xPosNew || yPosOld != yPosNew)
            {
                HPEN OldPen = static_cast<HPEN>(SelectObject(hDC, Pen));
                Rectangle(hDC, xPosOld, yPosOld, xPosNew, yPosNew);
                
                SelectObject(hDC, OldPen);
                coord.left = xPosOld;
                coord.top = yPosOld;
                coord.right = xPosNew;
                coord.bottom = yPosNew;
                obj.push_back({ IDM_RECT,coord });
            }
        }
        
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SIZE:
        SendMessage(ToolBar, TB_AUTOSIZE, 0, 0);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}


HWND CreateSimpleToolbar(HWND hWndParent)
{
    const int numButtons = 7;
    const int bitmapSize = 16;
    HWND hWndToolbar = CreateWindowExW(WS_EX_WINDOWEDGE, TOOLBARCLASSNAME, NULL,
        WS_CHILD | TBSTYLE_WRAPABLE, bSize, bSize, bSize, bSize,
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
    HBITMAP hBitmap;
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
    DeleteObject(hBitmap);
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

    pbih = (PBITMAPINFOHEADER)pbi;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    assert(lpBits);

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    assert(GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
        DIB_RGB_COLORS));

    // Create the .BMP file.  
    hf = CreateFile(pszFile,
        GENERIC_READ | GENERIC_WRITE,
        (DWORD)0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);
    assert(hf != INVALID_HANDLE_VALUE);

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    assert(WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD)&dwTmp, NULL));

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    assert(WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
        + pbih->biClrUsed * sizeof(RGBQUAD),
        (LPDWORD)&dwTmp, (NULL)));

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    assert(WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL));

    // Close the .BMP file.  
    assert(CloseHandle(hf));

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);

}



void Draw(HDC hDC, std::vector<std::tuple<INT, RECT>> obj)
{
    for (int i = 0; i < obj.size(); i++)
    {
       
        if (std::get<0>(obj[i]) == 98)
        {
            RECT rect = std::get<1>(obj[i]);
            MoveToEx(hDC, rect.left, rect.top, NULL);
            LineTo(hDC, rect.right, rect.bottom);

        }
        else if (std::get<0>(obj[i]) == 97)
        {
            RECT rect = std::get<1>(obj[i]);
            Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
        }
        else if (std::get<0>(obj[i]) == 99)
        {
            RECT rect = std::get<1>(obj[i]);
            Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom);
        }
    }
}