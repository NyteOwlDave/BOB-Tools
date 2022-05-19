
/*

    bc.c

    BOB style color definition file functions

    RGB Palette

*/

#include <ctype.h>
#include "inc/stdafx.h"
#include "inc/stdafx.h"
#include "inc/map.h"


#define MAGIC "VCP1"


char m_label[MAX_TOKEN+1];


// Helper to print/save color file
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

// Helper to load color file
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


// Read VCP file
void LoadVcpFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileRead(szPathName, "Opening i/p VCP file");
    fgets(m_label, MAX_TOKEN, fp);
    for (LPSTR sz=m_label; *sz; sz++)
    {
        if (isspace(*sz))
        {
            *sz=0;
            break;
        }
    }
    if (strcmp(m_label, MAGIC) != 0)
    {
        errno = EBADFORMAT;
        LastCall("Checking signature");
    }
    Log("Read VCP header");
    read_colors(fp, pal);
    Log("Read VCP colors");
    CloseFile(fp);
}

// Write VCP file
void SaveVcpFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p VCP file");
    fprintf(fp, "%s\n", MAGIC);
    Log("Wrote VCP header");
    write_colors(fp, pal);
    Log("Wrote VCP colors");
    CloseFile(fp);
}

