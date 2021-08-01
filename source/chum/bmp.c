
/*

    bmp.c

    Bitmap functions
    
    https://en.wikipedia.org/wiki/BMP_file_format

*/

#include "inc/stdafx.h"
#include "inc/bmp.h"
#include "inc/clrspace.h"

#define MAGIC (((WORD)'M'<<8)+(WORD)'B') // "BM" in intel order


#pragma pack(push)

#pragma pack(1)

typedef struct __attribute__((__packed__)) tagRGBTRIPLE
{
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} RGBTRIPLE, *PRGBTRIPLE, *NPRGBTRIPLE, *LPRGBTRIPLE;

typedef struct __attribute__((__packed__)) tagRGBQUAD
{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD, *PRGBQUAD, *NPRGBQUAD, *LPRGBQUAD;

/*
typedef struct __attribute__((__packed__)) tagBITMAP
{
    LONG bmType;
    LONG bmWidth;
    LONG bmHeight;
    LONG bmWidthBytes;
    WORD bmPlanes;
    WORD bmBitsPixel;
    LPVOID bmBits;
} BITMAP, *PBITMAP, *NPBITMAP, *LPBITMAP;
*/

typedef struct __attribute__((__packed__)) tagBITMAPCOREHEADER
{
    DWORD bcSize;
    WORD bcWidth;
    WORD bcHeight;
    WORD bcPlanes;
    WORD bcBitCount;
} BITMAPCOREHEADER, *LPBITMAPCOREHEADER, *PBITMAPCOREHEADER;

/*
typedef struct __attribute__((__packed__)) tagBITMAPCOREINFO
{
    BITMAPCOREHEADER bmciHeader;
    RGBTRIPLE bmciColors[1];
} BITMAPCOREINFO, *LPBITMAPCOREINFO, *PBITMAPCOREINFO;
*/

typedef struct __attribute__((__packed__)) tagBITMAPINFOHEADER
{
    DWORD biSize;
    INT biWidth;
    INT biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    INT biXPelsPerMeter;
    INT biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

/*
typedef struct __attribute__((__packed__)) tagBITMAPV4HEADER
{
    DWORD bV4Size;
    LONG bV4Width;
    LONG bV4Height;
    WORD bV4Planes;
    WORD bV4BitCount;
    DWORD bV4V4Compression;
    DWORD bV4SizeImage;
    LONG bV4XPelsPerMeter;
    LONG bV4YPelsPerMeter;
    DWORD bV4ClrUsed;
    DWORD bV4ClrImportant;
    DWORD bV4RedMask;
    DWORD bV4GreenMask;
    DWORD bV4BlueMask;
    DWORD bV4AlphaMask;
    DWORD bV4CSType;
    CIEXYZTRIPLE bV4Endpoints;
    DWORD bV4GammaRed;
    DWORD bV4GammaGreen;
    DWORD bV4GammaBlue;
} BITMAPV4HEADER, *LPBITMAPV4HEADER, *PBITMAPV4HEADER;
*/

/*
typedef struct __attribute__((__packed__)) tagBITMAPV5HEADER
{
    DWORD bV5Size;
    LONG bV5Width;
    LONG bV5Height;
    WORD bV5Planes;
    WORD bV5BitCount;
    DWORD bV5Compression;
    DWORD bV5SizeImage;
    LONG bV5XPelsPerMeter;
    LONG bV5YPelsPerMeter;
    DWORD bV5ClrUsed;
    DWORD bV5ClrImportant;
    DWORD bV5RedMask;
    DWORD bV5GreenMask;
    DWORD bV5BlueMask;
    DWORD bV5AlphaMask;
    DWORD bV5CSType;
    CIEXYZTRIPLE bV5Endpoints;
    DWORD bV5GammaRed;
    DWORD bV5GammaGreen;
    DWORD bV5GammaBlue;
    DWORD bV5Intent;
    DWORD bV5ProfileData;
    DWORD bV5ProfileSize;
    DWORD bV5Reserved;
} BITMAPV5HEADER, *LPBITMAPV5HEADER, *PBITMAPV5HEADER;
*/

/*
typedef struct __attribute__((__packed__)) tagBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;
*/

typedef struct __attribute__((__packed__)) tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

#pragma pack(pop)


// Load the file header
static void load_file_header(FILE* fp, PBITMAPFILEHEADER hdr)
{
    fread(hdr, sizeof(BITMAPFILEHEADER), 1, fp);
    if (ferror(fp))
    {
        LastCall("Reading BITMAPFILEHEADER");
    }
}

// Save the file header
static void save_file_header(FILE* fp, PBITMAPFILEHEADER hdr)
{
    fwrite(hdr, sizeof(BITMAPFILEHEADER), 1, fp);
    if (ferror(fp))
    {
        LastCall("Writing BITMAPINFOHEADER");
    }    
}

// List info header
static void list_file_header(PBITMAPFILEHEADER hdr)
{
    Log("List BMP file header");
    printf("Magic Initials: %04X\n", hdr->bfType);
    printf("Pixel Map Offset: %04X\n", hdr->bfOffBits);
    printf("File Size:  %08X\n", hdr->bfSize);
    printf("Reserved 1:  %04X\n", hdr->bfReserved1);
    printf("Reserved 2:  %04X\n", hdr->bfReserved2);
}


// Load the info header
static void load_info_header(FILE* fp, PBITMAPINFOHEADER hdr)
{
    fread(hdr, sizeof(BITMAPINFOHEADER), 1, fp);
    if (ferror(fp))
    {
        LastCall("Reading BITMAPINFOHEADER");
    }
}

// Save the info header
static void save_info_header(FILE* fp, PBITMAPINFOHEADER hdr)
{
    fwrite(hdr, sizeof(BITMAPINFOHEADER), 1, fp);
    if (ferror(fp))
    {
        LastCall("Writing BITMAPINFOHEADER");
    }
}

// List info header
static void list_info_header(PBITMAPINFOHEADER hdr)
{
    Log("List BMP info header");
    printf("Bit Count: %i\n", (int)hdr->biBitCount);
    printf("Compression: %u\n", (DWORD)hdr->biCompression);
    printf("Width: %i\n", (int)hdr->biWidth);
    printf("Height: %i\n", (int)hdr->biHeight);
    printf("Planes: %i\n", (int)hdr->biPlanes);
    printf("Used Colors: %u\n", (DWORD)hdr->biClrUsed);
    printf("Important Colors: %u\n", (DWORD)hdr->biClrImportant);
    printf("Info Block Size: %u\n", (DWORD)hdr->biSize);
    printf("Image Size: %u\n", (DWORD)hdr->biSizeImage);
    printf("X Pixels/Meter: %i\n", (DWORD)hdr->biXPelsPerMeter);
    printf("Y Pixels/Meter: %i\n", (DWORD)hdr->biYPelsPerMeter);
}


// Load the 32-bpp pixel map
static LPIMAGE load_pixels_32(FILE* fp, int w, int h)
{
    DWORD cx = (DWORD)w;
    DWORD cy = (DWORD)h;
    LPIMAGE img = CreateImage(w, h);
    for (int y = 0; y < h; y++)
    {
        int yy = h - (y + 1);
        LPARGB pLine = img->line[yy];
        for (int x = 0; x < w; x++)
        {
            ARGB color;
            color.b = fgetc(fp);
            color.g = fgetc(fp);
            color.r = fgetc(fp);
            color.a = fgetc(fp);
            pLine[x] = color;
        }
    }
    return img;
}

// Load the 24-bpp pixel map
static LPIMAGE load_pixels_24(FILE* fp, int w, int h)
{
    DWORD cx = (DWORD)w;
    DWORD cy = (DWORD)h;
    DWORD cbPixel = sizeof(RGBQUAD);
    DWORD cbScanLine = cx * cbPixel;
    DWORD cbDanglers = cbScanLine & 3;
    DWORD cbExtra = cbDanglers ? 4 - cbDanglers : 0;
    LPIMAGE img = CreateImage(w, h);
    for (int y = 0; y < h; y++)
    {
        int yy = h - (y + 1);
        LPARGB pLine = img->line[yy];
        for (int x = 0; x < w; x++)
        {
            ARGB color;
            color.b = fgetc(fp);
            color.g = fgetc(fp);
            color.r = fgetc(fp);
            color.a = 0;
            pLine[x] = color;
        }
        for (DWORD i = 0; i < cbExtra; i++)
            fgetc(fp);
    }
    return img;
}

// Save the 32-bpp pixel map
static void save_pixels_32(FILE* fp, LPIMAGE img)
{
    DWORD cx = (DWORD)(img->w);
    DWORD cy = (DWORD)(img->h);
    for (DWORD y = 0; y < cy; y++)
    {
        DWORD yy = cy - (y + 1); // Bottom up mode
        LPARGB pLine = img->line[yy];
        for (int x = 0; x < img->w; x++)
        {
            ARGB color = pLine[x];
            fputc(color.b, fp);
            fputc(color.g, fp);
            fputc(color.r, fp);
            fputc(0, fp);
        }
    }

}


// Save image to file (BMP)
BOOL SaveImageFileBmp(LPCSTR szPathName, LPIMAGE pImage)
{
    CheckImage(pImage);
    // Open file for o/p
    FILE *fp = OpenFileWrite(szPathName, "Creating o/p bitmap file");
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    // Calculate metrics
    DWORD cbFileHeader = sizeof(bmfh);
    DWORD cbInfoHeader = sizeof(bmih);
    DWORD cbPixel = sizeof(RGBQUAD);
    DWORD cbScanLine = (DWORD)(pImage->w) * cbPixel;
    DWORD cbPixelMap = (DWORD)(pImage->h) * cbScanLine;
    DWORD cbTotal = cbPixelMap + cbInfoHeader + cbFileHeader;
    DWORD cbDanglers = cbScanLine & 3;
    DWORD cbExtra = cbDanglers ? 4 - cbDanglers : 0;
    // Initialize file header
    bmfh.bfType = MAGIC;
    bmfh.bfOffBits = cbFileHeader + cbInfoHeader;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfSize = cbPixelMap;
    save_file_header(fp, &bmfh);
    Log("Wrote file header");
    // Initialize info header
    bmih.biBitCount = 32;
    bmih.biClrImportant = 0;
    bmih.biClrUsed = 0;
    bmih.biCompression = 0L; // BI_RGB
    bmih.biHeight = pImage->h;
    bmih.biPlanes = 1;
    bmih.biSize = cbInfoHeader;
    bmih.biSizeImage = cbPixelMap;
    bmih.biWidth = pImage->w;
    bmih.biXPelsPerMeter = 2835; // 72 dpi
    bmih.biYPelsPerMeter = 2835; // 72 dpi
    save_info_header(fp, &bmih);
    Log("Wrote info header");
    save_pixels_32(fp, pImage);
    Log("Wrote 24-bpp pixmap");
    CloseFile(fp);
    return TRUE;
}

// Load image from file (BMP)
LPIMAGE LoadImageFileBmp(LPCSTR szPathName)
{
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER fi;
    LPIMAGE pImage = NULL;
    FILE *fp = OpenFileRead(szPathName, "Opening i/p bitmap file");
    load_file_header(fp, &fh);
    Log("Read file header");
    if (fh.bfType != MAGIC)
    {
        errno = EBADMAGIC;
        LastCall("Reading signature");
    }
    load_info_header(fp, &fi);
    Log("Read info header");
    if (fi.biCompression != 0)
    {
        errno = EBADFORMAT;
        LastCall("Checking file header");
    }
    int w = fi.biWidth;
    int h = fi.biHeight;
    LPIMAGE img = NULL;
    if (fi.biBitCount == 24)
    {
        img = load_pixels_24(fp, w, h);
        Log("Read 24-bpp pixmap");
    }
    else if (fi.biBitCount == 32)
    {
        img = load_pixels_32(fp, w, h);
        Log("Read 32-bpp pixmap");
    }
    else
    {
        errno = EBADFORMAT;
        LastCall("Checking file header");
    }
    CloseFile(fp);
    return img;
}

// Show bitmap file header
void ShowBitmapFileHeader(LPCSTR szPathName)
{
    BITMAPFILEHEADER fh;
    FILE *fp = OpenFileRead(szPathName, "Opening i/p bitmap file");
    load_file_header(fp, &fh);
    Log("Read BMP file header");
    list_file_header(&fh);
    CloseFile(fp);
}

// Show bitmap info header
void ShowBitmapInfoHeader(LPCSTR szPathName)
{
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER fi;
    FILE *fp = OpenFileRead(szPathName, "Opening i/p bitmap file");
    load_file_header(fp, &fh);
    Log("Read BMP file header");
    load_info_header(fp, &fi);
    Log("Read BMP info header");
    list_info_header(&fi);
    CloseFile(fp);
}

// List BMP file info
void ListImageFileBmp(LPCSTR szPathName)
{
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER fi;
    FILE *fp = OpenFileRead(szPathName, "Opening i/p bitmap file");
    load_file_header(fp, &fh);
    Log("Read BMP file header");
    list_file_header(&fh);
    load_info_header(fp, &fi);
    Log("Read BMP info header");
    list_info_header(&fi);
    CloseFile(fp);
}
