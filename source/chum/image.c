
/*

    image.c

    Allegro style memory pixmaps

*/

#include "inc/stdafx.h"
#include "inc/image.h"


inline ARGB a_GetPixel(LPIMAGE pImage, int x, int y)
    __attribute__((always_inline));

inline void a_SetPixel(LPIMAGE pImage, int x, int y, ARGB color)
    __attribute__((always_inline));


// Create new image
LPIMAGE CreateImage(WORD w, WORD h)
{
    char sz[128];
    DWORD wi = (DWORD)w;
    DWORD hi = (DWORD)h;
    DWORD si = wi * hi;
    if (si < 1)
    {
        errno = EINVAL; // Invalid argument
        LastCall("Image is too small");
    }
    if (si > (65535U * 65535U))
    {
        errno = EINVAL; // Invalid argument
        LastCall("Image is too large");
    }
    DWORD cbFull = sizeof(LPARGB) * hi + sizeof(IMAGE);
    LPIMAGE pImage = (LPIMAGE)CreateBuffer(cbFull, FALSE);
    if (!pImage)
    {
        errno = ENOMEM;
        LastCall("Creating memory image");
    }
    ClearBuffer(pImage, sizeof(IMAGE));
    pImage->w = w;
    pImage->h = h;
    DWORD cbBuffer = sizeof(ARGB) * si;
    pImage->buffer = (LPARGB)CreateBuffer(cbBuffer, FALSE);
    if (pImage->buffer)
    {
        for (DWORD n = 0; n < hi; n++)
        {
            LPARGB p = pImage->buffer + n * wi;
            pImage->line[n] = p;
        }
        return pImage;
    }
    DestroyImage(pImage);
    errno = ENOMEM;
    LastCall("Allocating memory pixmap buffer");
}

// Destroy existing image
void DestroyImage(LPIMAGE pImage)
{
    if (pImage)
    {
        if (pImage->buffer)
        {
            free(pImage->buffer);
        }
        free(pImage);
    }
}

// Determine if point is within image
BOOL ImageContains(LPIMAGE pImage, int x, int y)
{
    if (pImage && pImage->buffer)
    {
        if ((x < 0) || (y < 0))
            return FALSE;
        if (x >= (int)(pImage->w))
            return FALSE;
        if (y >= (int)(pImage->h))
            return FALSE;
        return TRUE;
    }
    return FALSE;
}

// Read pixel (safe and slow)
ARGB GetPixel(LPIMAGE pImage, int x, int y)
{
    if (ImageContains(pImage, x, y))
    {
        return a_GetPixel(pImage, x, y);
    }
    else
    {
        ARGB color = {0, 0, 0, 0};
        return color;
    }
}

// Write pixel (safe and slow)
void SetPixel(LPIMAGE pImage, int x, int y, ARGB color)
{
    if (ImageContains(pImage, x, y))
    {
        a_SetPixel(pImage, x, y, color);
    }
}

// Compose a pixel
ARGB MakePixel(int r, int g, int b)
{
    ARGB pixel;
    pixel.r = r;
    pixel.g = g;
    pixel.b = b;
    return pixel;
}

// Fill entire image with a color
BOOL FillImage(LPIMAGE pImage, ARGB color)
{
    CheckImage(pImage);
    for (int y = 0; y < pImage->h; y++)
    {
        for (int x = 0; x < pImage->w; x++)
        {
            a_SetPixel(pImage, x, y, color);
        }
    }
    return TRUE;
}

// Check image pointer
void CheckImage(LPIMAGE pImage)
{
    LPCSTR szError = "Checking memory image";
    if (!pImage)
    {
        errno = ENOIMG;
        LastCall(szError);
    }
    if (!pImage->buffer)
    {
        errno = ENOIMGBFR;
        LastCall(szError);
    }
}
