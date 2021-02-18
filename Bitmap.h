#pragma once
#include <Windows.h>
#include <assert.h>



PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
void CreateBMPFile(LPCWSTR pszFile, HBITMAP hBMP);