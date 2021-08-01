#ifndef PPM_H_DEFINED
#define PPM_H_DEFINED

#include "image.h"

typedef enum _ppm_mode_t {
    P3 = 0,
    P6 = 1
} ppm_mode_t;

// Load image from file (PPM)
LPIMAGE LoadImageFilePpm(LPCSTR szPathName);

// Save image to file (PPM)
BOOL SaveImageFilePpm(LPCSTR szPathName, LPIMAGE pImage, ppm_mode_t type);

// List PPM file info
void ListImageFilePpm(LPCSTR szPathName );

#endif // !PPM_H_DEFINED
