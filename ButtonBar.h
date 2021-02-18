#pragma once
#include <Windows.h>
#include <commctrl.h>
#include "Resource.h"

#pragma comment(lib, "comctl32.lib")

constexpr INT IDM_NEW = 100;
constexpr INT IDM_OPEN = 101;
constexpr INT IDM_SAVE = 102;
constexpr INT IDM_RECT = 97;
constexpr INT IDM_LINE = 98;
constexpr INT IDM_CIRCLE = 99;
constexpr INT IDM_PEN = 96;
constexpr INT IDM_BRUSH = 95;
constexpr INT IDM_ÑONTINUOUS_LINE = 94;

static HINSTANCE hInst;

const int ImageListID = 0;
const DWORD buttonStyles = BTNS_AUTOSIZE;
const INT bSize = 45;
static TBBUTTON tbButtons[] =
{
    { MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("New"))},
    { MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Open"))},
    { MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, TBSTATE_ENABLED, buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Save"))},
    { MAKELONG(STD_DELETE,   ImageListID), IDM_EXIT, TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Exit"))},
    { MAKELONG(NULL,ImageListID),IDM_RECT,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Rectangle"))},
    { MAKELONG(NULL,ImageListID),IDM_LINE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Line"))},
    { MAKELONG(NULL,ImageListID),IDM_ÑONTINUOUS_LINE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Continuous Line"))},
    { MAKELONG(NULL,ImageListID),IDM_CIRCLE,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Circle"))},
    { MAKELONG(NULL,ImageListID),IDM_PEN,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Pen"))},
    { MAKELONG(NULL,ImageListID),IDM_BRUSH,TBSTATE_ENABLED,buttonStyles, {0}, 0, reinterpret_cast<INT_PTR>(TEXT("Brush"))},
};
static HIMAGELIST g_hImageList=NULL ;


HWND CreateSimpleToolbar(HWND hWndParent);