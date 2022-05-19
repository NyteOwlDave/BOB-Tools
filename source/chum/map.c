
/*

    map.c

    BOB style map file functions

    RGB Palette

*/

#include "inc/stdafx.h"
#include "inc/stdafx.h"
#include "inc/map.h"
#include "inc/image.h"
#include "inc/bob.h"
#include "inc/gdi.h"


// Helper to print/save map file
static void write_map(FILE* fp, PRGBPALETTE pal);

// Helper to load map file
static void read_map(FILE* fp, PRGBPALETTE pal);


// Read map file
void LoadMapFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "LoadMapFile");
    FILE* fp = OpenFileRead(szPathName, "Opening i/p MAP file");
    read_map(fp, pal);
    Log("Read MAP colors");
    CloseFile(fp);
}

// Write map file
void SaveMapFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "SaveMapFile");
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p MAP file");
    write_map(fp, pal);
    Log("Wrote MAP colors");
    CloseFile(fp);
}

// Print map file to stdout
void PrintPalette(PRGBPALETTE pal)
{
    CheckPtr(pal, "PrintMapFile");
    write_map(stdout, pal);
}

// Print map file to stdout (as JSON)
void PrintPaletteJSON(PRGBPALETTE pal)
{
    for (int n=0; n<256; n++)
    {
        int r = pal->color[n].r;
        int g = pal->color[n].g;
        int b = pal->color[n].b;
        fprintf(stdout, "[%i, %i, %i, 0],\n", r, g, b);
    }
}

// Print map file to stdout (as C/C++)
void PrintPaletteC(PRGBPALETTE pal)
{
    for (int n=0; n<256; n++)
    {
        int r = pal->color[n].r;
        int g = pal->color[n].g;
        int b = pal->color[n].b;
        fprintf(stdout, "{%i, %i, %i, 0},\n", r, g, b);
    }
}

// Palette linear interpolation (full)
void LerpPalette(RGBColor start, RGBColor stop, PRGBPALETTE pal)
{
    CheckPtr(pal, "LerpPalette");
    LerpPaletteRange(start, 0, stop, 255, pal);
}

// Palette linear interpolation (range)
void LerpPaletteRange(RGBColor start, int nStart, RGBColor stop, int nStop, PRGBPALETTE pal)
{
    CheckPtr(pal, "LerpPaletteRange");
    if (nStart < nStop)
    {
        int tmp = nStart;
        nStart = nStop;
        nStop = nStart;
        RGBColor c = start;
        start = stop;
        stop = c;
    }
    float sr = start.r;
    float sg = start.g;
    float sb = start.b;
    float dr = 1 / (stop.r - start.r);
    float dg = 1 / (stop.g - start.g);
    float db = 1 / (stop.b - start.b);
    if (isnan(dr)) dr = 0;
    if (isnan(dg)) dg = 0;
    if (isnan(db)) db = 0;
    for (int n = nStart; n <= nStop; n++)
    {
        if ((n >= 0) && (n < 256))
        {
            pal->color[n].r = sr;
            pal->color[n].g = sg;
            pal->color[n].b = sb;
        }
        sr += dr;
        sg += dg;
        sb += db;
    }
}

// Helper to print/save map file
static void write_map(FILE* fp, PRGBPALETTE pal)
{
    for (int n=0; n<256; n++)
    {
        int r = pal->color[n].r;
        int g = pal->color[n].g;
        int b = pal->color[n].b;
        fprintf(fp, "%i %i %i\n", r, g, b);
        if (ferror(fp))
        {
            LastCall("Writing MAP file colors");
        }
    }
}

// Helper to load map file
static void read_map(FILE* fp, PRGBPALETTE pal)
{
    for (int n=0; n<256; n++)
    {
        int r, g, b;
        fscanf(fp, "%i %i %i\n", &r, &g, &b);
        pal->color[n].r = r;
        pal->color[n].g = g;
        pal->color[n].b = b;
        if (ferror(fp))
        {
            LastCall("Reading MAP file colors");
        }
    }
}

// Scale color to 6-bit from 8-bit
RGBColor ScaleColorTo6( RGBColor color )
{
    RGBColor result;
    result.r = color.r >> 2;
    result.g = color.g >> 2;
    result.b = color.b >> 2;
    result.a = 0;
    return result;
}

// Scale color to 6-bit from 8-bit
RGBColor ScaleColorTo8( RGBColor color )
{
    RGBColor result;
    int r = color.r & 0x1F;
    int g = color.g & 0x1F;
    int b = color.b & 0x1F;
    result.r = floor(r * 4.0 + 0.5);
    result.g = floor(r * 4.0 + 0.5);
    result.b = floor(r * 4.0 + 0.5);
    result.a = 0;
    return result;
}

// Scale colors to 6-bit from 8-bit
void ScalePaletteTo6(PRGBPALETTE src, PRGBPALETTE dst)
{
    CheckPtr(src, "ScalePaletteTo6");
    CheckPtr(dst, "ScalePaletteTo6");
    for (int n=0; n < 256; n++ )
    {
        dst->color[n] = ScaleColorTo6(src->color[n]);
    }
}

// Scale colors to 6-bit from 8-bit
void ScalePaletteTo8(PRGBPALETTE src, PRGBPALETTE dst)
{
    CheckPtr(src, "ScalePaletteTo8");
    CheckPtr(dst, "ScalePaletteTo8");
    for (int n=0; n < 256; n++ )
    {
        dst->color[n] = ScaleColorTo8(src->color[n]);
    }
}

// Preview palette
void ViewPalette(PRGBPALETTE pal)
{
    int cbox = 10;
    int cgap = 3;
    int cspan = cbox + cgap;
    int w = cgap*17 + cbox*16;
    int h = w;
    PDC pdc = CreateDC(w, h);
    ClearDC(pdc);
    RECT rc;
    RGBColor white = {255, 255, 255, 0};
    rc.w = rc.h = cbox;
    for (int y=0, i=0; y<16; y++)
    {
        rc.y = cgap + y * cspan;
        for (int x=0; x<16; x++)
        {
            rc.x = cgap + x * cspan;
            pdc->fgc = pal->color[i++];
            FillRect(pdc, &rc);
            pdc->fgc = white;
            DrawRect(pdc, &rc);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
    DestroyDC(pdc);
}

// Initialize a color
RGBColor MakeColor(int r, int g, int b)
{
    RGBColor color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 0;
    return color;
}
