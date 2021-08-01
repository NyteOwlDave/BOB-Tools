#ifndef RAW_H_DEFINED
#define RAW_H_DEFINED

#include "image.h"

// Load image from file (RAW)
LPIMAGE LoadImageFileRaw(LPCSTR szPathName);

// Save image to file (RAW)
BOOL SaveImageFileRaw(LPCSTR szPathName, LPIMAGE pImage);

// List RAW file info
void ListImageFileRaw(LPCSTR szPathName );

#endif // !RAW_H_DEFINED

