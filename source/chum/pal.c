
/*

    pal.c

    NCS style pal file functions

    RGB Palettes

    Uses BITS_PER_CHANNEL environment variable.

    Set to 6 or 8 (default).

*/

#include "inc/stdafx.h"
#include "inc/pal.h"

// Helper to save pal (8-bit) file
static void write_pa8(FILE* fp, PRGBPALETTE pal);

// Helper to load pal (8-bit) file
static void read_pa8(FILE* fp, PRGBPALETTE pal);

// Helper to save pal (6-bit) file
static void write_pa6(FILE* fp, PRGBPALETTE pal);

// Helper to load pal (6-bit) file
static void read_pa6(FILE* fp, PRGBPALETTE pal);


#define MAGIC_PA6 "PAL"
#define MAGIC_PA8 "PA8"

// Read pal file
void LoadPalFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "LoadPalFile");
    char magic[5];
    FILE* fp = OpenFileRead(szPathName, "Opening i/p PAL file");
    fread(magic, 4, 1, fp);
    magic[4]=0;
    Log("Read PAL header");
    if (strcmp(magic, MAGIC_PA6) == 0)
    {
        read_pa6(fp, pal);
        Log("Read 6-bit PAL colors");
    }
    else if (strcmp(magic, MAGIC_PA8) == 0)
    {
        read_pa8(fp, pal);
        Log("Read 8-bit PAL colors");
    }
    else
    {
        errno = EBADFORMAT;  // Operation not permitted
        LastCall("Checking PAL signature");
    }
    if (ferror(fp))
    {
        LastCall("Reading i/p PAL file");
    }
    CloseFile(fp);
}

// Write pal file
void SavePalFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "SavePalFile");
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p PAL file");
    char* bpc = getenv("BITS_PER_CHANNEL");
    if (bpc)
    {
        int i = atoi(bpc);
        switch(i)
        {
        case 6:
            write_pa6(fp, pal);
            Log("Write 6-bit PAL colors");
            break;
        default:
            write_pa8(fp, pal);
            Log("Write 8-bit PAL colors");
            break;
        }
    }
    else
    {
        write_pa8(fp, pal);
        Log("Write 8-bit PAL colors");
    }
    if (ferror(fp))
    {
        LastCall("Writing o/p file");
    }
    CloseFile(fp);
}


// Helper to save pal (8-bit) file
static void write_pa8(FILE* fp, PRGBPALETTE pal)
{
    for (int n=0; n<256; n++)
    {
        int r = pal->color[n].r;
        int g = pal->color[n].g;
        int b = pal->color[n].b;
        fputc(r, fp);
        fputc(g, fp);
        fputc(b, fp);
        fputc(0, fp);
    }
}

// Helper to load pal (8-bit) file
static void read_pa8(FILE* fp, PRGBPALETTE pal)
{
    for (int n=0; n<256; n++)
    {
        int r = fgetc(fp);
        int g = fgetc(fp);
        int b = fgetc(fp);
        fgetc(fp);   // Reserved for alpha (unused)
        // fprintf(stderr, "%i %i %i %i\n", n, r, g, b);
        pal->color[n].r = r;
        pal->color[n].g = g;
        pal->color[n].b = b;
    }
}

// Helper to save pal (6-bit) file
static void write_pa6(FILE* fp, PRGBPALETTE pal)
{
    RGBPalette copy;
    ScalePaletteTo8(pal, &copy);
    write_pa8(fp, &copy);
}

// Helper to load pal (6-bit) file
static void read_pa6(FILE* fp, PRGBPALETTE pal)
{
    read_pa8(fp, pal);
    ScalePaletteTo6(pal, pal);
}

