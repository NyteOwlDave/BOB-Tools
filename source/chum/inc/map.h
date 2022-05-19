
#ifndef MAP_H_DEFINED
#define MAP_H_DEFINED

#include "wintypes.h"


// RGBColor Definition
#pragma pack(1)
typedef struct t_rgb_color {
	BYTE	r;
	BYTE	g;
	BYTE	b;
	BYTE	a;
} RGBColor, *PRGBCOLOR;
#pragma pack()

// RGBPalette Definition
typedef struct t_rgb_palette {
	RGBColor color[256];
} RGBPalette, *PRGBPALETTE;


// Read map file
extern void LoadMapFile(LPCSTR szPathName, PRGBPALETTE pal);

// Write map file
extern void SaveMapFile(LPCSTR szPathName, PRGBPALETTE pal);

// Print palette to stdout
extern void PrintPalette(PRGBPALETTE pal);

// Print palette to stdout (as JSON)
extern void PrintPaletteJSON(PRGBPALETTE pal);

// Print palette to stdout (as C/C++)
extern void PrintPaletteC(PRGBPALETTE pal);

// Preview palette
extern void ViewPalette(PRGBPALETTE pal);

// Palette linear interpolation (full)
extern void LerpPalette(RGBColor start, RGBColor stop, PRGBPALETTE pal);

// Palette linear interpolation (range)
extern void LerpPaletteRange(RGBColor start, int nStart, RGBColor stop, int nStop, PRGBPALETTE pal);

// Scale color to 6-bit from 8-bit
extern RGBColor ScaleColorTo6(RGBColor color);

// Scale color to 6-bit from 8-bit
extern RGBColor ScaleColorTo8(RGBColor color);

// Scale colors to 6-bit from 8-bit
extern void ScalePaletteTo6(PRGBPALETTE src, PRGBPALETTE dst);

// Scale colors to 6-bit from 8-bit
extern void ScalePaletteTo8(PRGBPALETTE src, PRGBPALETTE dst);

// Initialize a color
extern RGBColor MakeColor(int r, int g, int b);


#endif // !MAP_H_DEFINED
