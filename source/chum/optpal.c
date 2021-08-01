
/*

    optpal.c 
    
    Main file for optpal app

    OPtimal color palette for 24-bpp pixmap

*/
 
#include "inc/stdafx.h"
#include "inc/mapx.h"
#include "inc/bob.h"


static char m_infile[MAX_PATH+1];


// Show help
static void usage(void)
{
    printf("\nUSAGE:\n");
    printf("optpal infile\n");
    exit(0);
}

// Handle arguments
static void sort_out_args(int argc, char **argp)
{
    int num_files = 0;
    m_infile[0] = 0;
    if (argc < 2)
    {
        usage();
    }
    for (int x=1; x<argc; x++)
    {
        if (num_files > 1)
        {
            usage();
        }
        if (++num_files == 1)
        {
            strcpy(m_infile, argp[x]);
        }
    }
    if (num_files < 1)
    {
        usage();
    }
    strcat(m_infile, ".img");
}

// Does most of the real work
static int map_colors(void)
{
    RGBPalette pal; 
    ClearBuffer(&pal, sizeof(RGBPalette));
    fprintf(stderr, "Loading : %s\n", m_infile);
    LPIMAGE img = LoadImageFileBob(m_infile);
    InitPaletteOptimal(pal.color, img, NULL);
    PrintMapFile(&pal);
    return 0;
}

// Application entry point
int main(int argc, char **argp, char **envp)
{
    sort_out_args(argc, argp);
    return map_colors();
}

