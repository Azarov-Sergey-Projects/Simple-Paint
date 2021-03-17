#pragma once
#include <Windows.h>
#include "resource2.h"

static BOOL hBrushButton = TRUE;
static INT pGreenBrush = 255;
static INT pRedBrush = 255;
static INT pBlueBrush = 255;
INT_PTR CALLBACK DlgProcBrush(HWND Brush, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static INT red = 255;
    static INT blue = 255;
    static INT green = 255;
    switch (uMsg)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(Brush, IDC_STATIC1, pRedBrush, FALSE);
        SetDlgItemInt(Brush, IDC_STATIC2, pGreenBrush, FALSE);
        SetDlgItemInt(Brush, IDC_STATIC3, pBlueBrush, FALSE);
        return static_cast<INT_PTR>(TRUE);
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RED_BRUSH:
            hBrushButton = FALSE;
            red = GetDlgItemInt(Brush, IDC_RED_BRUSH, NULL, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC1, red, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC2, green, FALSE);
            SetDlgItemInt(Brush, IDC_STATIC3, blue, FALSE);
            break;
        case IDC_BLUE_BRUSH:
            hBrushButton = FALSE;
            blue = GetDlgItemInt(Brush, IDC_BLUE_BRUSH, NULL, FALSE);
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
            return static_cast<INT_PTR>(TRUE);
        case WM_CLOSE:
            EndDialog(Brush, LOWORD(wParam));
            return static_cast<INT_PTR>(TRUE);
            break;
        case ID_CANCEL:
            EndDialog(Brush, 0);
            return  static_cast<INT_PTR>(TRUE);
            break;
        }
    default: return static_cast<INT_PTR>(FALSE);
    }
}