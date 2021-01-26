// Lab6.cpp : Определяет точку входа для приложения.
//

#include <Windows.h>
#include <commctrl.h>
#include "Lab6.h"
#include "framework.h"


#pragma comment(lib, "Comctl32.lib")
constexpr INT IDM_NEW = 100;
#define IDM_OPEN 101
#define IDM_SAVE 102



LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


void Save(HWND hWnd);
void Open(HWND hWnd);


const int ImageListID = 0;
const DWORD buttonStyles = BTNS_AUTOSIZE;

TBBUTTON tbButtons[] =
{
    { MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("New"))},
    { MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Open"))},
    { MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Save"))},
    { MAKELONG(STD_DELETE,   ImageListID), IDM_EXIT, TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(_T("Exit"))}
};
HIMAGELIST g_hImageList = NULL;
HINSTANCE hInst;


HWND CreateSimpleToolbar(HWND hWndParent);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    TCHAR szClassName[] = TEXT("Мой класс");
    HWND hWnd;
    MSG msg;
    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.lpszMenuName = NULL;
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
    HWND ToolBar;
    switch (uMsg)
    {
    case WM_CREATE:
        ToolBar = CreateSimpleToolbar(hWnd);
        
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDM_NEW:
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case IDM_OPEN:
            Open(hWnd);
            break;
        case IDM_SAVE:
            Save(hWnd);
            break;
        case IDM_EXIT:
            PostQuitMessage(0);
            break;
       }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
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
    const int numButtons = 4;
    const int bitmapSize = 16;
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0,
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
    RECT windowRect;
    GetClientRect(hWnd, &windowRect);

    int bitmap_dx = windowRect.right - windowRect.left;
    int bitmap_dy = windowRect.bottom - windowRect.top;

    BITMAPINFOHEADER bmpInfoHeader;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAP* pBitmap;

    bmpFileHeader.bfType = 0x4d42;
    bmpFileHeader.bfSize = 0;
    bmpFileHeader.bfReserved1 = 0;
    bmpFileHeader.bfReserved2 = 0;
    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    bmpInfoHeader.biSize = sizeof(bmpInfoHeader);
    bmpInfoHeader.biWidth = bitmap_dx;
    bmpInfoHeader.biHeight = bitmap_dy;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = 24;
    bmpInfoHeader.biCompression = BI_RGB;
    bmpInfoHeader.biSizeImage = bitmap_dx * bitmap_dy * (24 / 8);
    bmpInfoHeader.biXPelsPerMeter = 0;
    bmpInfoHeader.biYPelsPerMeter = 0;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImportant = 0;

    BITMAPINFO info;
    info.bmiHeader = bmpInfoHeader;

    BYTE* memory;
    HDC winDC = GetDC(hWnd);
    HDC bmpDC = CreateCompatibleDC(winDC);
   
    HBITMAP bitmap = CreateDIBSection(winDC, &info, DIB_RGB_COLORS, (void**)&memory, NULL, 0);
    SelectObject(bmpDC, bitmap);
    BitBlt(bmpDC, 0, 0, bitmap_dx, bitmap_dy, winDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, winDC);

    OPENFILENAME ofn;
    wchar_t strFilter[] = TEXT("*.bmp");
    wchar_t strFileName[MAX_PATH] = TEXT("");
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = strFilter;
    ofn.lpstrFile = strFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
    wcsstr(strFileName, static_cast<LPWSTR>(ofn.lpstrFile));
    GetSaveFileName(&ofn); 

    HANDLE hFile = CreateFile(
        ofn.lpstrFile, GENERIC_WRITE,
        0, NULL, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return;

    DWORD dwWritten = 0;
    WriteFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(hFile, &bmpInfoHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
    WriteFile(hFile, memory, bmpInfoHeader.biSizeImage, &dwWritten, NULL);
    CloseHandle(hFile);
}


void Open(HWND hWnd)
{
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
    if (ofn.lpstrFile == TEXT("\0")) { return; }
    PAINTSTRUCT ps;
    HBITMAP hBitmap;
    BITMAP bm;
    HDC hDC;
    HDC hMemDC;
    hDC = BeginPaint(hWnd, &ps);
    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
    ReleaseDC(hWnd, hDC);
    DeleteObject(hBitmap);
    EndPaint(hWnd, &ps);
    UpdateWindow(hWnd);
}