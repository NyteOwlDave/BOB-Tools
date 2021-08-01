#ifndef BITMAP_H_DEFINED
#define BITMAP_H_DEFINED

#include "image.h"

// Load image from file (BMP)
LPIMAGE LoadImageFileBmp(LPCSTR szPathName);

// Save image to file (BMP)
BOOL SaveImageFileBmp(LPCSTR szPathName, LPIMAGE pImage);

// List BMP file info
void ListImageFileBmp(LPCSTR szPathName);

// Show bitmap file header
void ShowBitmapFileHeader(LPCSTR szPathName);

// Show bitmap info header
void ShowBitmapInfoHeader(LPCSTR szPathName);

#endif // !BITMAP_H_DEFINED
