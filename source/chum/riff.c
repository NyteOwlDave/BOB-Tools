
#include "inc/stdafx.h"
#include "inc/riff.h"


#define MAGIC       "RIFF"
#define MAGIC_PAL   "PAL data"
#define ENTRY_SIZE  (3)


#pragma pack(2)

typedef struct _t_riff
{
    char        szRiff[4];          // Tag = "RIFF" (no null terminator)
    DWORD       dwChunkSize;          // Total file length - 4 bytes
    char        szPalTag[8];        // Tag = "PAL data" (no null terminator)
    DWORD       dwSubChunkSize;       // Total file length - 16 bytes
    WORD        wPaletteBufferSize;  // # of bytes to allocate for palette memory buffer
    WORD        wPaletteEntryCount;  // # of entries in palette color table
//  RGBColor    Entry[];            // Palette colors
} t_riff, *PRIFF;

#pragma pack()


// Read in header
static void read_header(FILE* fp, PRIFF riff)
{
    size_t cb = sizeof(t_riff);
    fread(riff, cb, 1, fp);
    if (ferror(fp))
    {
        LastCall("Reading RIFF header");
    }
}

// Write out header
static void write_header(FILE* fp, PRIFF riff)
{
    size_t cb = sizeof(t_riff);
    fread(riff, cb, 1, fp);
    if (ferror(fp))
    {
        LastCall("Reading RIFF header");
    }
}

// Initialize RIFF header
static void init_header(PRIFF riff)
{
    DWORD cbPalette = 256 * ENTRY_SIZE;
    DWORD cbChunk = sizeof(t_riff) + cbPalette - sizeof(riff->szRiff);
    DWORD cbSubChunk = cbChunk = cbChunk - sizeof(riff->dwChunkSize) - sizeof(riff->szPalTag);
    memcpy(riff->szRiff, MAGIC, sizeof(riff->szRiff));
    riff->dwChunkSize = cbChunk;
    memcpy(riff->szPalTag, MAGIC_PAL, sizeof(riff->szPalTag));
    riff->dwSubChunkSize = cbSubChunk;
    riff->wPaletteEntryCount = 256;
    riff->wPaletteBufferSize = cbPalette;
}

// Check the RIFF header
static void check_header(PRIFF riff)
{
    if (memcmp(riff->szRiff, MAGIC, sizeof(riff->szRiff)))
    {
        errno = EBADFORMAT;
        LastCall("Checking RIFF header");
    }
    if (memcmp(riff->szPalTag, MAGIC_PAL, sizeof(riff->szPalTag)))
    {
        errno = EBADFORMAT;
        LastCall("Checking RIFF header (pal)");
    }
    if ((riff->wPaletteEntryCount * ENTRY_SIZE) != riff->wPaletteBufferSize)
    {
        errno = EBADFORMAT;
        LastCall("Checking RIFF header (size)");
    }
    if (riff->wPaletteEntryCount > 256)
    {
        Warning("Ignore extra colors");
        riff->wPaletteEntryCount = 256;
    }
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
//        fputc(0, fp);
    }
}

// Helper to load pal (8-bit) file
static void read_pa8(FILE* fp, PRGBPALETTE pal, int count)
{
    for (int n=0; n<count; n++)
    {
        int r = fgetc(fp);
        int g = fgetc(fp);
        int b = fgetc(fp);
//        fgetc(fp);   // Reserved for alpha (unused)
        pal->color[n].r = r;
        pal->color[n].g = g;
        pal->color[n].b = b;
        pal->color[n].a = 0;
    }
}

// Helper to list RIFF header
static void list_header(PRIFF riff)
{
    char sz[MAX_TOKEN+1];
    Log("List RIFF header");
    memcpy(sz, riff->szRiff, sizeof(riff->szRiff));
    sz[sizeof(riff->szRiff)] = 0;
    printf("File Magic: %s\n", sz);
    printf("File Chunk Size: %u\n", riff->dwChunkSize);
    memcpy(sz, riff->szPalTag, sizeof(riff->szPalTag));
    sz[sizeof(riff->szPalTag)] = 0;
    printf("Palette Magic: %s\n", sz);
    printf("Palette Chunk Size: %u\n", riff->dwSubChunkSize);
    printf("Palette Buffer Size: %u\n", (DWORD)(riff->wPaletteBufferSize));
    printf("Palette Entry Count: %u\n", (DWORD)(riff->wPaletteEntryCount));
}


// Read RIFF file
void LoadRiffFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "LoadPalFile");
    FILE* fp = OpenFileRead(szPathName, "Opening i/p RIFF file");
    t_riff hdr;
    read_header(fp, &hdr);
    check_header(&hdr);
    ClearBuffer(pal, sizeof(RGBPalette));
    read_pa8(fp, pal, hdr.wPaletteEntryCount);
    CloseFile(fp);
}

// Write RIFF file
void SaveRiffFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "LoadPalFile");
    FILE* fp = OpenFileRead(szPathName, "Creating o/p RIFF file");
    t_riff hdr;
    init_header(&hdr);
    write_header(fp, &hdr);
    write_pa8(fp, pal);
    CloseFile(fp);
}

// List RIFF header
void ListRiffHeader(LPCSTR szPathName)
{
    FILE* fp = OpenFileRead(szPathName, "Opening i/p RIFF file");
    t_riff hdr;
    read_header(fp, &hdr);
    list_header(&hdr);
    CloseFile(fp);
}
