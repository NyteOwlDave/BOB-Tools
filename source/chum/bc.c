
/*

    bc.c

    BOB style color definition file functions

    RGB Palette

*/

#include "inc/stdafx.h"
#include "inc/stdafx.h"
#include "inc/map.h"


#define MAGIC "vcp1"


static char m_token[MAX_TOKEN+1];
static char m_label[MAX_TOKEN+1];


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
        fprintf(fp, "#define %s %f %f %f", m_label, r, g, b);
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
        fscanf(fp, "%s %s %f %f %f", m_token, m_label, &r, &g, &b);
        if (strcmp(m_token, "#define") == 0)
        {
            Log(m_label);
            pal->color[n].r = r * 255;
            pal->color[n].g = g * 255;
            pal->color[n].b = b * 255;
            pal->color[n].a = 0;
            if (++n > 255) break;
        }
    }
}


// Read BC file
void LoadBobColorFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileRead(szPathName, "Opening i/p BC file");
    read_colors(fp, pal);
    Log("Read BC colors");
    CloseFile(fp);
}

// Write BC file
void SaveBobColorFile(LPCSTR szPathName, PRGBPALETTE pal)
{
    CheckPtr(pal, "Palette pointer");
    FILE* fp = OpenFileWrite(szPathName, "Creating o/p BC file");
    write_colors(fp, pal);
    Log("Wrote BC colors");
    CloseFile(fp);
}

