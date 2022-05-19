
#ifndef MAPX_H_DEFINED
#define MAPX_H_DEFINED


#include "map.h"
#include "image.h"


extern void CopyPalette(PRGBCOLOR pDst, PRGBCOLOR pSrc, int iColors);
extern void FillPalette(PRGBCOLOR pDst, RGBColor clr, int iColors);
extern void InitPalette332(PRGBCOLOR pal);
extern void InitPaletteJedi(PRGBCOLOR pal);
extern void InitPaletteFire(PRGBCOLOR pal);
extern void InitPaletteVGA(PRGBCOLOR pal);
extern void InitPaletteMono(PRGBCOLOR pal, RGBColor color, float gamma);
extern int InitPaletteOptimal(PRGBCOLOR pPal, LPIMAGE img, char *rsvd);


#endif // !MAPX_H_DEFINED

