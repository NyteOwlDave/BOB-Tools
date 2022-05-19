
/*

    ortc.c

    ORTC file format

*/

#include <ctype.h>
#include "inc/stdafx.h"
#include "inc/ortc.h"


static char m_label[MAX_TOKEN+1];


// Read in palette
static void read_colors(FILE* fp, PRGBPALETTE pal)
{
    int n=0;
    while (!feof(fp) && !ferror(fp))
    {
        float r, g, b;
        fscanf(fp, "%s %f %f %f", m_label, &r, &g, &b);
        Log(m_label);
        pal->color[n].r = r * 255;
        pal->color[n].g = g * 255;
        pal->color[n].b = b * 255;
        pal->color[n].a = 0;
        if (++n > 255) break;
    }
}

// Write out palette
static void write_colors(FILE* fp, PRGBPALETTE pal)
{
    int n=0;
    while (!ferror(fp))
    {
        float r = pal->color[n].r / 255.0;
        float g = pal->color[n].g / 255.0;
        float b = pal->color[n].b / 255.0;
        sprintf(m_label, "Color%i", n+1);
        fprintf(fp, "%s %f %f %f", m_label, r, g, b);
        if (++n > 255) break;
    }
}

// Read ORTC file
void LoadOrtcFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileRead(szPathName, "Opening i/p ORTC file");
    read_colors(fp, pal);
    Log("Read ORTC colors");
    CloseFile(fp);
}

// Write ORTC file
void SaveOrtcFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p ORTC file");
    write_colors(fp, pal);
    Log("Wrote ORTC colors");
    CloseFile(fp);
}

