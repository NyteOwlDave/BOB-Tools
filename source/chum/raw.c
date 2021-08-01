
/*

    raw.c

    NCS style raw pixmap

*/

#include "inc/stdafx.h"
#include "inc/raw.h"


#define MAGIC "RAW"


// Image file header (RAW)
typedef struct _t_raw_hdr
{
    char magic[4];
    WORD height;
    WORD width;
} t_raw_hdr, *PRAWHDR;


// Read in RAW header
static void load_file_header(FILE *fp, PRAWHDR hdr)
{
    fread(hdr, sizeof(t_raw_hdr), 1, fp);
    if (ferror(fp))
    {
        LastCall("Reading RAW header");
    }
    hdr->magic[3] = 0;
}

// Write out RAW header
static void save_file_header(FILE *fp, PRAWHDR hdr)
{
    fwrite(hdr, sizeof(t_raw_hdr), 1, fp);
    if (ferror(fp))
    {
        LastCall("Writing RAW header");
    }
}

// List RAW header
static void list_file_header(PRAWHDR hdr)
{
    Log("List RAW header");
    printf("Magic: %s\n", hdr->magic);
    printf("Height: %i\n", (int)hdr->height);
    printf("Width: %i\n", (int)hdr->width);
}

// Read in 24-bpp pixmap
static void load_pixels_32(FILE* fp, LPIMAGE img)
{
    for (int y = 0; y < img->h; y++)
    {
        ARGB* p = img->line[y];
        fread(p, sizeof(ARGB), img->w, fp);
        if (ferror(fp))
        {
            LastCall("Reading RAW pixel map");
        }
    }
}

// Write 24-bpp pixmap
static void save_pixels_32(FILE* fp, LPIMAGE img)
{
    for (int y = 0; y < img->h; y++)
    {
        ARGB* p = img->line[y];
        fwrite(p, sizeof(ARGB), img->w, fp);
        if (ferror(fp))
        {
            LastCall("Writing RAW pixel map");
        }
    }
}


// Load image from file (RAW)
LPIMAGE LoadImageFileRaw(LPCSTR szPathName)
{
    FILE *fp = OpenFileRead(szPathName, "Opening i/p RAW file");
    t_raw_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read RAW header");
    if (strcmp(hdr.magic, MAGIC))
    {
        errno = EBADFORMAT;
        LastCall("Checking PPM signature");
    }
    LPIMAGE pImage = CreateImage(hdr.width, hdr.height);
    load_pixels_32(fp, pImage);
    Log("Read RAW 32-bpp pixmap");
    CloseFile(fp);
    return pImage;
}

// Save image to file (RAW)
BOOL SaveImageFileRaw(LPCSTR szPathName, LPIMAGE pImage)
{
    CheckImage(pImage);
    FILE *fp = OpenFileWrite(szPathName, "Creating o/p RAW file");
    t_raw_hdr hdr;
    strcpy(hdr.magic, MAGIC);
    hdr.width = pImage->w;
    hdr.height = pImage->h;
    save_file_header(fp, &hdr);
    Log("Wrote RAW header");
    save_pixels_32(fp, pImage);
    Log("Wrote RAW 32-bpp pixmap");
    CloseFile(fp);
    return TRUE;
}

// List RAW file info
void ListImageFileRaw(LPCSTR szPathName)
{
    FILE *fp = OpenFileRead(szPathName, "Opening i/p RAW file");
    t_raw_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read RAW header");
    list_file_header(&hdr);
    CloseFile(fp);
}

