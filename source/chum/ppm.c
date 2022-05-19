
/*

    ppm.c

    PPM file format
    
    https://en.wikipedia.org/wiki/Netpbm

*/

#include <ctype.h>
#include "inc/stdafx.h"
#include "inc/ppm.h"


#define MAX_LINE        (1024)

#define TOKEN_ERROR     (-1)
#define TOKEN_EOF       (0)
#define TOKEN_COMMENT   (1)
#define TOKEN_PUNCT     (2)
#define TOKEN_NUMBER    (3)
#define TOKEN_KEYWORD   (4)

#define MAGIC_P3    "P3"
#define MAGIC_P6    "P6"


typedef int token_t;

typedef struct _t_ppm_hdr {
    char magic[8];
    int width;
    int height;
    int colors;
} t_ppm_hdr, *PPPMHDR;


static char m_line[MAX_LINE+1];
static char m_token[MAX_LINE+1];

/*
static void save_p3(LPCSTR szPathName, LPIMAGE pImage);
static void save_p6(LPCSTR szPathName, LPIMAGE pImage);
static LPIMAGE read_p3_pixels(FILE* fp, int w, int h);
static LPIMAGE read_p6_pixels(FILE* fp, int w, int h);
*/


// Tokenizer
// Read next line
static BOOL line_read(FILE* fp)
{
    Log("Line Read");
    BOOL ok = (fgets(m_line, MAX_LINE, fp) ? TRUE : FALSE);
    if (ok)
    {
        // Convert whitespace to all BLANK spaces
        LPSTR pos = m_line;
        while (*pos)
        {
            if (isspace(*pos))
            {
                if (*pos == '\n')
                {
                    *pos = 0;
                }
                else
                {
                    *pos = 32;
                }
            }
            pos++;
        }
        Log(m_line);
    }
    else
    {
        Log("Failed");
    }
    return ok;
}

// Tokenizer
// Read token
static token_t token_read(FILE* fp, LPCSTR* ppos)
{
    static LPCSTR szMaxToken = "Exceeded maximum token length";
    int out;
    LPCSTR pos = *ppos;
    
    Log("Token Read");

    // Start with empty token
    m_token[0] = 0;

    // Skip whitespace
    if (isspace(*pos))
    {
        Log("Skip whitespace");
        while (isspace(*pos)) 
        {
            pos++;
        }
    }
    
    // Refresh line if buffer empty
    if (*pos == 0)
    {
        Log("Detected EOL");
        if (!line_read(fp))
        {
            if (feof(fp))
            {
                Log("EOF");
                return TOKEN_EOF;
            }
            Log("Error");
            return TOKEN_ERROR;
        }
        *ppos = m_line;
        return token_read(fp, ppos);
    }

    // Capture comment
    if (*pos == '#')
    {
        Log("Capture comment");
        out = 0;
        while (*pos)
        {
            if (out > MAX_LINE)
            {
                Log(szMaxToken);
                out = MAX_LINE;
                break;
            }
            m_token[out++] = *pos;
            pos++;
        }
        m_token[out] = 0;
        Log("Comment");
        Log(m_token);
        *ppos = pos;
        return TOKEN_COMMENT;
    }

    // Capture punctuation
    if (ispunct(*pos))
    {
        Log("Capture punctuation");
        out = 0;
        while (ispunct(*pos))
        {
            if (out > MAX_LINE)
            {
                Log(szMaxToken);
                out = MAX_LINE;
                break;
            }
            m_token[out++] = *pos;
            pos++;
        }
        m_token[out] = 0;
        Log("Punctuation");
        Log(m_token);
        *ppos = pos;
        return TOKEN_PUNCT;
    }

    // Capture number
    if (isdigit(*pos))
    {
        Log("Capture number");
        out = 0;
        while (isdigit(*pos))
        {
            if (out > MAX_LINE)
            {
                Log(szMaxToken);
                out = MAX_LINE;
                break;
            }
            m_token[out++] = *pos;
            pos++;
        }
        m_token[out] = 0;
        Log("Number");
        Log(m_token);
        *ppos = pos;
        return TOKEN_NUMBER;
    }

    // Capture keyword
    Log("Capture keyword");
    out = 0;
    while ((*pos) && !isspace(*pos))
    {
        if (out > MAX_LINE)
        {
            Log(szMaxToken);
            out = MAX_LINE;
            break;
        }
        m_token[out++] = *pos;
        pos++;
    }
    m_token[out] = 0;
    Log("Keyword");
    Log(m_token);
    *ppos = pos;
    return TOKEN_KEYWORD;
}

// Tokenizer
// Read a number
static BOOL number_read(FILE* fp, LPCSTR* pos, int* result)
{
    token_t tok = token_read(fp, pos);
    while (tok == TOKEN_COMMENT)
    {
        tok = token_read(fp, pos);
    }
    if (tok != TOKEN_NUMBER)
    {
        *result = 0;
        return FALSE;
    }
    *result = atoi(m_token);
    return TRUE;
}

// Load the file header
static void load_file_header(FILE *fp, PPPMHDR hdr)
{
    // Clear the line buffer
    m_line[0] = 0;
    // Skip all comments
    LPCSTR pos = m_line;
    token_t tok = token_read(fp, &pos);
    while (tok == TOKEN_COMMENT)
    {
        tok = token_read(fp, &pos);
    }
    if (tok != TOKEN_KEYWORD)
    {
        errno = EBADFORMAT;
        LastCall("Finding PPM magic initials");
    }
    strncpy(hdr->magic, m_token, sizeof(hdr->magic) - 1);
    tok = token_read(fp, &pos);
    while (tok == TOKEN_COMMENT)
    {
        tok = token_read(fp, &pos);
    }
    if (tok != TOKEN_NUMBER)
    {
        errno = EBADFORMAT;
        LastCall("Reading PPM magic initials");
    }
    hdr->width = atoi(m_token);
    tok = token_read(fp, &pos);
    while (tok == TOKEN_COMMENT)
    {
        tok = token_read(fp, &pos);
    }
    if (tok != TOKEN_NUMBER)
    {
        errno = EBADFORMAT;
        LastCall("Reading PPM width field");
    }
    hdr->height = atoi(m_token);
    tok = token_read(fp, &pos);
    while (tok == TOKEN_COMMENT)
    {
        tok = token_read(fp, &pos);
    }
    if (tok != TOKEN_NUMBER)
    {
        errno = EBADFORMAT;
        LastCall("Reading PPM height field");
    }
    hdr->colors = atoi(m_token);
    if (hdr->colors != 255)
    {
        errno = EBADFORMAT;
        LastCall("Checking PPM bits field");
    }
}

// Save the file header
static void save_file_header(FILE *fp, PPPMHDR hdr)
{
    fprintf(fp, "%s\n", hdr->magic);
    fprintf(fp, "%i %i\n", hdr->width, hdr->height);
    fprintf(fp, "%i\n", hdr->colors);
}

// List the file header
static void list_file_header(PPPMHDR hdr)
{
    Log("List PPM header");
    printf("Magic: %s\n", hdr->magic);
    printf("Width: %i\n", hdr->width);
    printf("Height: %i\n", hdr->height);
    printf("Bits: %i\n", hdr->colors);
}


// Read all P3 mode pixels
static LPIMAGE read_p3_pixels(FILE* fp, int w, int h)
{
    LPIMAGE pImage = CreateImage(w, h);
    // Reset line input buffer
    m_line[0] = 0;
    LPCSTR pos = m_line;
    // Clear coords
    int x=0;
    int y=0;
    while (TRUE)
    {
        if (x >= w) 
        {
            x = 0;
            y++;
            if (y >= h) break;
        }
        int r, g, b;
        if (!number_read(fp, &pos, &r)) 
        {
            Warning("Too few pixels in PPM pixel map");
            break;
        }
        if (!number_read(fp, &pos, &g))
        {
            Warning("Too few pixels in PPM pixel map");
            break;
        }
        if (!number_read(fp, &pos, &b))
        {
            Warning("Too few pixels in PPM pixel map");
            break;
        }
        ARGB color;
        color.r = r;
        color.g = g;
        color.b = b;
        pImage->line[y][x] = color;
        x++;
    }
    return pImage;
}

// Read all P6 mode pixels
static LPIMAGE read_p6_pixels(FILE* fp, int w, int h)
{
    LPIMAGE pImage = CreateImage(w, h);
    // Clear coords
    int x=0;
    int y=0;
    while (TRUE)
    {
        if (x >= w) 
        {
            x = 0;
            y++;
            if (y >= h) break;
        }
        ARGB color;
        color.r = fgetc(fp);
        color.g = fgetc(fp);
        color.b = fgetc(fp);
        if (ferror(fp))
        {
            Warning("Too few pixels in PPM pixel map");
            break;
        }
        pImage->line[y][x] = color;
        x++;
    }
    return pImage;
}

// Write all P3 mode pixels
static void write_p3_pixels(FILE* fp, LPIMAGE img)
{
    for (int y=0; y < img->h; y++) 
    {
        for (int x=0; x < img->w; x++)
        {
            ARGB color = img->line[y][x];
            int r = color.r;
            int g = color.g;
            int b = color.b;
            fprintf(fp, "%i %i %i\n", r, g, b);
        }
        if (fflush(fp) == EOF)
        {
            LastCall("Writing P3 pixel map");
        }
    }
}

// Write all P6 mode pixels
static void write_p6_pixels(FILE* fp, LPIMAGE img)
{
    for (int y=0; y < img->h; y++) 
    {
        for (int x=0; x < img->w; x++)
        {
            ARGB color = img->line[y][x];
            int r = color.r;
            int g = color.g;
            int b = color.b;
            fputc(r, fp);
            fputc(g, fp);
            fputc(b, fp);
        }
        if (fflush(fp) == EOF)
        {
            LastCall("Writing P6 pixel map");
        }
    }
}

// Save in P3 (plain) format
static void save_p3(FILE* fp, LPIMAGE pImage)
{
    t_ppm_hdr hdr;
    strcpy(hdr.magic, MAGIC_P3);
    hdr.width = pImage->w;
    hdr.height = pImage->h;
    hdr.colors = 255;
    save_file_header(fp, &hdr);
    write_p3_pixels(fp, pImage);
}

// Save in P6 (raw) format
static void save_p6(FILE* fp, LPIMAGE pImage)
{
    t_ppm_hdr hdr;
    strcpy(hdr.magic, MAGIC_P6);
    hdr.width = pImage->w;
    hdr.height = pImage->h;
    hdr.colors = 255;
    save_file_header(fp, &hdr);
    write_p6_pixels(fp, pImage);
}

// Load image from file (PPM)
LPIMAGE LoadImageFilePpm(LPCSTR szPathName)
{
    FILE* fp = OpenFileRead(szPathName, "Opening i/p PPM file");
    t_ppm_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read PPM header");
    LPIMAGE pImage = NULL;
    if (strcmp(hdr.magic, MAGIC_P3) == 0)
    {
        Log("Detected P3 format");
        pImage = read_p3_pixels(fp, hdr.width, hdr.height);
        Log("Read P3 pixmap");
    }
    else if (strcmp(hdr.magic, MAGIC_P6) == 0)
    {
        Log("Detected P6 format");
        pImage = read_p6_pixels(fp, hdr.width, hdr.height);
        Log("Read P6 pixmap");
    }
    else
    {
        errno = EBADFORMAT;
        LastCall("Checking PPM signature");
    }
    CloseFile(fp);
    return pImage;
}

// Save image to file (PPM)
BOOL SaveImageFilePpm(LPCSTR szPathName, LPIMAGE pImage, ppm_mode_t type)
{
    CheckImage(pImage);
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p PPM file");
    switch (type)
    {
    case P3:
        Log("Detected P3 mode");
        save_p3(fp, pImage);
        Log("Wrote P3 pixmap");
        break;
    case P6:
        Log("Detected P6 mode");
        save_p6(fp, pImage);
        Log("Wrote P6 pixmap");
        break;
    }
    CloseFile(fp);
    return TRUE;
}

// List PPM file info
void ListImageFilePpm(LPCSTR szPathName)
{
    FILE* fp = OpenFileRead(szPathName, "Opening i/p PPM file");
    t_ppm_hdr hdr;
    load_file_header(fp, &hdr);
    Log("Read PPM header");
    list_file_header(&hdr);
    CloseFile(fp);
}

