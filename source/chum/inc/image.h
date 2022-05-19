#ifndef IMAGE_H_DEFINED
#define IMAGE_H_DEFINED

#include "wintypes.h"


// ARGB Structure
typedef struct _t_argb {
    BYTE b;
    BYTE g;
    BYTE r;
    BYTE a;
} ARGB, *PARGB;
typedef PARGB LPARGB;
typedef ARGB t_argb;

// IMAGE Structure
typedef struct _t_image {
    WORD w;
    WORD h;
    DWORD id;
    LPARGB buffer;
    LPARGB line[];
} IMAGE, *PIMAGE;
typedef PIMAGE LPIMAGE;
typedef IMAGE t_image;


// Check image pointer
extern void CheckImage(LPIMAGE pImage);

// Create a new image object
extern LPIMAGE CreateImage(WORD w, WORD h);

// Destroy an existing image object
extern void DestroyImage(LPIMAGE pImage);

// Read pixel (slow and safe)
extern ARGB GetPixel(LPIMAGE pImage, int x, int y);

// Write pixel (slow and safe)
extern void SetPixel(LPIMAGE pImage, int x, int y, ARGB color);

// Read pixel (unsafe and fast)
inline ARGB a_GetPixel(LPIMAGE pImage, int x, int y)
{
    return (pImage->line[y][x]);
}

// Write pixel (unsafe and fast)
inline void a_SetPixel(LPIMAGE pImage, int x, int y, ARGB color) 
{
    pImage->line[y][x] = color;
}

// Compose a pixel
extern ARGB MakePixel(int r, int g, int b );

// Determine if point is inside image
extern BOOL ImageContains(LPIMAGE pImage, int x, int y);

// Fill image with a color
extern BOOL FillImage(LPIMAGE pImage, ARGB color);


#endif // !IMAGE_H_DEFINED

