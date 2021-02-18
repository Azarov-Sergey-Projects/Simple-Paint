#pragma once
#include <Windows.h>
#include "resource1.h"

static INT pSize = 3;
static INT pRed=0;
static INT pGreen=0;
static INT pBlue = 0;
static BOOL pNullBrush = FALSE;

INT_PTR CALLBACK DlgProcPen(HWND DialogBox, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        return static_cast<INT_PTR>(TRUE);
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

            pSize = size = 0;
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
                MessageBoxW(DialogBox, TEXT("–азмер карандаша стал равным 0, размер установлен в 1"), TEXT("Error"), NULL);
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
            return static_cast<INT_PTR>(TRUE);
        case WM_CLOSE:
            EndDialog(DialogBox, LOWORD(wParam));
            return static_cast<INT_PTR>(TRUE);
        case IDCANCEL:
            EndDialog(DialogBox, 0);
            return  static_cast<INT_PTR>(TRUE);
        }

    default:return static_cast<INT_PTR>(FALSE);
    }
}