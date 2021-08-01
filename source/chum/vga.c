
/*

    vga.c

    NCS style vga pixmap

*/

#include "inc/stdafx.h"
#include "inc/vga.h"


// Image file header (VGA)
typedef struct _t_vga_hdr
{
    WORD width;
    WORD height;
} t_vga_hdr, *PVGAHDR;


// Read in VGA header
static void load_file_header(FILE *fp, PVGAHDR hdr)
{
    fread(hdr, sizeof(t_vga_hdr), 1, fp);
    if (ferror(fp))
    {
        LastCall("Reading VGA header");
    }
}

// Write out VGA header
static void save_file_header(FILE *fp, PVGAHDR hdr)
{
    fwrite(hdr, sizeof(t_vga_hdr), 1, fp);
    if (ferror(fp))
    {
        LastCall("Writing VGA header");
    }
}

// List VGA header
static void list_file_header(PVGAHDR hdr)
{
    Log("List VGA header");
    printf("Width: %i\n", (int)hdr->width);
    printf("Height: %i\n", (int)hdr->height);
}

// Read in 32-bpp pixmap
static void load_pixels_32(FILE* fp, LPIMAGE img)
{
    for (int y = 0; y < img->h; y++)
    {
        ARGB* p = img->line[y];
        fread(p, sizeof(ARGB), img->w, fp);
        if (ferror(fp))
        {
            LastCall("Reading i/p VGA pixel map");
        }
    }
}

// Write out 32-bpp pixmap
static void save_pixels_32(FILE* fp, LPIMAGE img)
{
    for (int y = 0; y < img->h; y++)
    {
        ARGB* p = img->line[y];
        fwrite(p, sizeof(ARGB), img->w, fp);
        if (ferror(fp))
        {
            LastCall("Writing o/p VGA pixel map");
        }
    }    
}


// Load image from file (Vga)
LPIMAGE LoadImageFileVga(LPCSTR szPathName)
{
    FILE *fp = OpenFileRead(szPathName, "Opening i/p VGA file");
    t_vga_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read VGA header");
    LPIMAGE pImage = CreateImage(hdr.width, hdr.height);
    load_pixels_32(fp, pImage);
    Log("Read VGA 32-bpp pixmap");
    CloseFile(fp);
    return pImage;
}

// Save image to file (VGA)
BOOL SaveImageFileVga(LPCSTR szPathName, LPIMAGE pImage)
{
    CheckImage(pImage);
    FILE *fp = OpenFileWrite(szPathName, "Creating o/p VGA file");
    t_vga_hdr hdr; 
    ClearBuffer(&hdr, sizeof(t_vga_hdr));
    hdr.width = pImage->w;
    hdr.height = pImage->h;
    save_file_header(fp, &hdr);
    Log("Wrote VGA header");
    save_pixels_32(fp, pImage);
    Log("Wrote VGA 32-bpp pixmap");
    CloseFile(fp);
    return TRUE;
}

// List VGA file info
void ListImageFileVga(LPCSTR szPathName)
{
    FILE *fp = OpenFileRead(szPathName, "Opening i/p VGA file");
    t_vga_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read VGA header");
    list_file_header(&hdr);
    CloseFile(fp);
}

