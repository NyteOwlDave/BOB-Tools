
/*

    bob.c

    Bob IMG functions

*/

#include "inc/stdafx.h"
#include "inc/bob.h"
#include "inc/motorola.h"

// Picture file
typedef struct _t_pic
{
    FILE *fp; // stream pointer
    int w;    // width
    int h;    // height
} t_pic, *PPIC;

#pragma pack(1)

typedef struct _t_bob_hdr
{
    WORD width;
    WORD height;
    WORD start;
    WORD stop;
    WORD bpp;
} t_bob_hdr, *PBOBHDR;

#pragma pack()


// Inline functions
extern inline void a_SetPixel(LPIMAGE, int, int, ARGB)
    __attribute__((always_inline));
extern inline ARGB a_GetPixel(LPIMAGE, int, int)
    __attribute__((always_inline));


// Load the file header
static void load_file_header(FILE *fp, PBOBHDR hdr)
{
    Log("Read IMG Header");
    hdr->width = ReadMotoralaWord(fp);
    hdr->height = ReadMotoralaWord(fp);
    hdr->start = ReadMotoralaWord(fp);
    hdr->stop = ReadMotoralaWord(fp);
    hdr->bpp = ReadMotoralaWord(fp);
    if (ferror(fp))
    {
        LastCall("Reading IMG header");
    }
}

// Save the file header
static void save_file_header(FILE *fp, PBOBHDR hdr)
{
    Log("Write IMG Header");
    WriteMotoralaWord(fp, hdr->width);
    WriteMotoralaWord(fp, hdr->height);
    WriteMotoralaWord(fp, hdr->start);
    WriteMotoralaWord(fp, hdr->stop);
    WriteMotoralaWord(fp, hdr->bpp);
    if (ferror(fp))
    {
        LastCall("Writing IMG header");
    }
}

// List the file header
static void list_file_header(PBOBHDR hdr)
{
    Log("List IMG Header");
    printf("Width: %i\n", (int)(hdr->width));
    printf("Height: %i\n", (int)(hdr->height));
    printf("Start: %i\n", (int)(hdr->start));
    printf("Stop: %i\n", (int)(hdr->stop));
    printf("BPP: %i\n", (int)(hdr->bpp));
}

// Local functions
static void write_scan_line(PPIC pic, LPARGB buf)
{
    int i;          // which pixel?
    int total;      // how many left in scan?
    int count;      // current run total
    int or, og, ob; // current run color
    int r, g, b;    // next pixel color
    double seconds; // another helping?

    i = 0;
    total = pic->w;
    or = buf[i].r;
    og = buf[i].g;
    ob = buf[i].b;
    i++;
    do
    {
        count = 1;
        total--;
        while (TRUE)
        {
            r = buf[i].r;
            g = buf[i].g;
            b = buf[i].b;
            i++;
            if (r != or || g != og || b != ob || count >= 254 || total <= 0)
            {
                break;
            }
            total--;
            count++;
        }
        fputc(count, pic->fp);
        fputc(ob, pic->fp);
        fputc(og, pic->fp);
        fputc(or, pic->fp);
        if (ferror(pic->fp))
        {
            LastCall("Writing IMG scan line");
        }
        or = r;
        og = g;
        ob = b;
        // Special case for final pixel
        if (total == 1)
        {
            fputc(1, pic->fp);
            fputc(buf[pic->w - 1].b, pic->fp);
            fputc(buf[pic->w - 1].g, pic->fp);
            fputc(buf[pic->w - 1].r, pic->fp);
            total--;
        }
    } while (total > 0);
    if (fflush(pic->fp) == EOF)
    {
        LastCall("Flushing IMG scan line");
    }
}

// Read in a scanline
static void read_scan_line(PPIC pic, LPARGB buf)
{
    // Read in the image
    ARGB color;
    color.a = 0;
    int i = 0;
    while (i < pic->w)
    {
        int count = fgetc(pic->fp) & 0xff;
        color.b = (BYTE)(fgetc(pic->fp) & 0xff);
        color.g = (BYTE)(fgetc(pic->fp) & 0xff);
        color.r = (BYTE)(fgetc(pic->fp) & 0xff);
        while (count)
        {
            *buf++ = color;
            if (++i == pic->w)
                break;
            count--;
        }
    }
}

// Load the 24-bpp pixel map
static LPIMAGE load_pixels_24(FILE* fp, LPIMAGE img)
{
    t_pic pic;
    pic.fp = fp;
    pic.w = img->w;
    pic.h = img->h;
    // Read in scanlines
    for (int y = 0; y < img->h; y++)
    {
        //char buf[MAX_TOKEN];
        //sprintf(buf, "Read line %i", y);
        //Log(buf);
        read_scan_line(&pic, img->line[y]);
    }
}

static void save_pixels_24(FILE* fp, LPIMAGE img)
{
    t_pic pic;
    pic.fp = fp;
    pic.w = img->w;
    pic.h = img->h;
    // Write out scanlines
    for (int y = 0; y < img->h; y++)
    {
        //char buf[MAX_TOKEN];
        //sprintf(buf, "Write line %i", y);
        //Log(buf);
        write_scan_line(&pic, img->line[y]);
    }
}

// Save image to file (BOB)
BOOL SaveImageFileBob(LPCSTR szPathName, LPIMAGE pImage)
{
    CheckImage(pImage);
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p IMG file");
    t_bob_hdr hdr;
    ClearBuffer(&hdr, sizeof(t_bob_hdr));
    hdr.width = pImage->w;
    hdr.height = pImage->h;
    hdr.bpp = 24;
    save_file_header(fp, &hdr);
    Log("Wrote IMG header");
    save_pixels_24(fp, pImage);
    Log("Wrote 24-bpp pixmap");
    CloseFile(fp);
    return TRUE;
}

// Load image from file (BOB)
LPIMAGE LoadImageFileBob(LPCSTR szPathName)
{
    LPIMAGE pImage;
    t_bob_hdr hdr;
    FILE* fp = OpenFileRead(szPathName, "Opening i/p IMG file");
    load_file_header(fp, &hdr);
    Log("Check IMG Header");
    if (hdr.bpp != 24)
    {
        errno = EBADFORMAT;
        LastCall("Checking signature");
    }
    pImage = CreateImage(hdr.width, hdr.height);
    load_pixels_24(fp, pImage);
    Log("Read 24-bpp pixmap");
    CloseFile(fp);
    return pImage;
}

// List BOB file info
void ListImageFileBob(LPCSTR szPathName)
{
    FILE* fp = OpenFileRead(szPathName, "Opening i/p IMG file");
    t_bob_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read IMG header");
    list_file_header(&hdr);
    CloseFile(fp);
}

