#include "OpenSaving.h"

void Save(HWND hWnd,INT bSize)
{
    HDC hdc[2];
    HBITMAP hbitmap;
    RECT rect;
    GetClientRect(hWnd, &rect);
    hdc[0] = GetDC(hWnd);
    hbitmap = CreateCompatibleBitmap(hdc[0], rect.right, rect.bottom - bSize);
    hdc[1] = CreateCompatibleDC(hdc[0]);
    SelectObject(hdc[1], hbitmap);
    BitBlt(
        hdc[1],
        rect.left,
        rect.top - bSize,
        rect.right,
        rect.bottom,
        hdc[0],
        0,
        0,
        SRCCOPY
    );
    TCHAR szFile[MAX_PATH]=TEXT("");
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("Bitmap Files(*.bmp)\0*.bmp\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    GetSaveFileName(&ofn);
    CreateBMPFile(ofn.lpstrFile, hbitmap);
}

HBITMAP Open(HWND hWnd, INT bSize)
{
    static HBITMAP hBitmap;
    INT xWinPos;
    INT yWinPos;
    RECT rect;
    GetClientRect(hWnd, &rect);
    xWinPos = rect.left;
    yWinPos = rect.top + bSize;
    TCHAR szFile[260];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("Bitmap files(*.bmp)\0*.bmp\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);
    if (ofn.lpstrFile == TEXT("\0")) { return hBitmap; }
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
    return hBitmap;
}