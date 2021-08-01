#ifndef VGA_H_DEFINED
#define VGA_H_DEFINED

#include "image.h"

// Load image from file (VGA)
LPIMAGE LoadImageFileVga(LPCSTR szPathName);

// Save image to file (VGA)
BOOL SaveImageFileVga(LPCSTR szPathName, LPIMAGE pImage);

// List VGA file info
void ListImageFileVga(LPCSTR szPathName );

#endif // !VGA_H_DEFINED

