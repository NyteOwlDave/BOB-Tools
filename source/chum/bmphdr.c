
/*

    bmphdr.c

    Main file for bmphdr app.

*/

#include "inc/stdafx.h"
#include "inc/bmp.h"
#include "inc/raw.h"
#include "inc/bob.h"
#include "inc/pic.h"
#include "inc/vga.h"

static char szPathName[MAX_PATH+1];

static void usage(void);

// Where all the work is done
static void show(int argc, char **argp)
{

    BOOL fh = FALSE;
    BOOL fi = FALSE;

    szPathName[0] = 0;

    for (int i = 1; i < argc; i++)
    {
        if (argp[i][0] == '-')
        {
            switch (argp[i][1])
            {
            case 'h':
                fh = TRUE;
                break;
            case 'i':
                fi = TRUE;
                break;
            }
        }
        else
        {
            strcpy(szPathName, argp[i]);
        }
    }

    if (!szPathName[0])
    {
        usage();
    }

    if (!(fh || fi))
    {
        usage();
    }

    if (fh)
    {
        ShowBitmapFileHeader(szPathName);
    }

    if (fi)
    {
        ShowBitmapInfoHeader(szPathName);
    }
}

// Application entry point
int main(int argc, char **argp, char **envp)
{
    show(argc, argp); // No return
}

static void usage(void)
{
    printf("\n");
    printf("Usage:\n");
    printf("\n");
    printf("bmphdr [flags] <filename>\n");
    printf("\n");
    printf("Flags:\n");
    printf("\n");
    printf("-h\tshow bitmap file header\n");
    printf("-i\tshow bitmap info header\n");
    printf("\n");
    printf("Filename requires extension.\n");
    printf("\n");
    exit(0);
}
