
/*

    pic.c

    PIC file format

*/

#include "inc/stdafx.h"
#include "inc/pic.h"
#include "inc/motorola.h"


// Image file header (PIC)
typedef struct _t_pic_hdr
{
    int width;
    int height;
} t_pic_hdr, *PPICHDR;


// Load the file header
static void load_file_header(FILE *fp, PPICHDR hdr)
{
    fread(hdr, sizeof(t_pic_hdr), 1, fp);
    if (ferror(fp))
    {
        LastCall("Loading PIC file header");
    }
}

// Load the file header
static void save_file_header(FILE *fp, PPICHDR hdr)
{
    fwrite(hdr, sizeof(t_pic_hdr), 1, fp);
    if (ferror(fp))
    {
        LastCall("Saving PIC file header");
    }
}

// Load the file header
static void list_file_header(PPICHDR hdr)
{
    Log("List PIC header");
    printf("Width: %i\n", hdr->width);
    printf("Height: %i\n", hdr->height);
}

// Read in 24-bpp piixel map
static void read_pixmap_24(FILE *fp, LPIMAGE img)
{
    for (int y = 0; y < img->h; y++)
    {
        // Read row index (WORD little-endian)
        int index = fgetc(fp);
        index += (fgetc(fp) << 8);
        // Verify row index matches counter
        if (index != y)
        {
            errno = EILSEQ;
            LastCall("Reading row index");
        }
        // Read in red channel for scanline
        for (int x = 0; x < img->w; x++)
        {
            img->line[index][x].r = (BYTE)(fgetc(fp));
        }
        // Read in green channel for scanline
        for (int x = 0; x < img->w; x++)
        {
            img->line[index][x].g = (BYTE)(fgetc(fp));
        }
        // Read in blue channel for scanline
        for (int x = 0; x < img->w; x++)
        {
            img->line[index][x].b = (BYTE)(fgetc(fp));
        }
        // Detect error
        if (ferror(fp))
        {
            LastCall("Reading PIC scanline");
        }
    }
}

// Write out 24-bpp piixel map
static void write_pixmap_24(FILE *fp, LPIMAGE img)
{
    // Write all scanlines
    for (int y = 0; y < img->h; y++)
    {
        // Write row index
        WriteMotoralaWord(fp, (WORD)y);
        // Write out red channel for scanline
        for (int x = 0; x < img->w; x++)
        {
            fputc(img->line[y][x].r, fp);
        }
        // Write out green channel for scanline
        for (int x = 0; x < img->w; x++)
        {
            fputc(img->line[y][x].g, fp);
        }
        // Write out blue channel for scanline
        for (int x = 0; x < img->w; x++)
        {
            fputc(img->line[y][x].b, fp);
        }
        // Flush
        if (fflush(fp) == EOF)
        {
            LastCall("Flushing PIC scanline");
        }
    }
}

// Load image from file (PIC)
LPIMAGE LoadImageFilePic(LPCSTR szPathName)
{
    FILE *fp = OpenFileRead(szPathName, "Opening i/p PIC file");
    t_pic_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read PIC header");
    LPIMAGE pImage = CreateImage((WORD)hdr.width, (WORD)hdr.height);
    read_pixmap_24(fp, pImage);
    Log("Read 24-bpp PIC pixmap");
    CloseFile(fp);
    return pImage;
}

// Save image to file (PIC)
BOOL SaveImageFilePic(LPCSTR szPathName, LPIMAGE pImage)
{
    CheckImage(pImage);
    FILE *fp = OpenFileWrite(szPathName, "Creating o/p PIC file");
    t_pic_hdr hdr;
    ClearBuffer(&hdr, sizeof(t_pic_hdr));
    hdr.width = pImage->w;
    hdr.height = pImage->h;
    save_file_header(fp, &hdr);
    Log("Wrote PIC header");
    write_pixmap_24(fp, pImage);
    Log("Wrote 24-bpp PIC pixmap");
    CloseFile(fp);
    return TRUE;
}

// List PIC file info
void ListImageFilePic(LPCSTR szPathName)
{
    FILE *fp = OpenFileRead(szPathName, "Opening i/p PIC file");
    t_pic_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read PIC header");
    list_file_header(&hdr);
    CloseFile(fp);
}
