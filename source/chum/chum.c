
/*

    chum.c

    Main file for chum app.

    Pixel Map conversions

*/

#include "inc/stdafx.h"
#include "inc/bmp.h"
#include "inc/raw.h"
#include "inc/bob.h"
#include "inc/pic.h"
#include "inc/vga.h"
#include "inc/ppm.h"

// DONE... BMP, RAW, BOB, PIC, VGA, PPM
// TODO... GIF, PCX, TGA

#define MAX_TOKEN   (80)
#define MAX_PATH    (260)

typedef struct _t_renamer
{
    char szInputName[MAX_PATH];
    char szOutputName[MAX_PATH];
    LPCSTR szInputExt;
    LPCSTR szOutputExt;
    BOOL bListMode;
} t_renamer, *LPRENAMER;


typedef LPIMAGE (*PFN_LOAD)(LPCSTR szPathName);
typedef BOOL (*PFN_SAVE)(LPCSTR szPathName, LPIMAGE pImage);
typedef void (*PFN_LIST)(LPCSTR szPathName);


static BOOL save_image_file_ppm6(LPCSTR szPathName, LPIMAGE pImage);


typedef struct _t_load_entry {
    LPCSTR ext;
    PFN_LOAD func;
} t_load_entry, *PLOADENTRY;

typedef struct _t_save_entry {
    LPCSTR ext;
    PFN_SAVE func;
} t_save_entry, *PSAVEENTRY;

typedef struct _t_list_entry {
    LPCSTR ext;
    PFN_LIST func;
} t_list_entry, *PLISTENTRY;


static t_load_entry m_loader[] = {
    { "bmp",  LoadImageFileBmp },
    { "raw",  LoadImageFileRaw },
    { "img",  LoadImageFileBob },
    { "pic",  LoadImageFilePic },
    { "vga",  LoadImageFileVga },
    { "ppm",  LoadImageFilePpm },
    { NULL, NULL }
};

static t_save_entry m_saver[] = {
    { "bmp",  SaveImageFileBmp },
    { "raw",  SaveImageFileRaw },
    { "img",  SaveImageFileBob },
    { "pic",  SaveImageFilePic },
    { "vga",  SaveImageFileVga },
    { "ppm",  save_image_file_ppm6 },
    { NULL, NULL }
};

static t_list_entry m_lister[] = {
    { "bmp",  ListImageFileBmp },
    { "raw",  ListImageFileRaw },
    { "img",  ListImageFileBob },
    { "pic",  ListImageFilePic },
    { "vga",  ListImageFileVga },
    { "ppm",  ListImageFilePpm },
    { NULL, NULL }
};


// Get loader function
static PFN_LOAD get_load_func(LPCSTR szExt)
{
    static char token[MAX_TOKEN];
    PLOADENTRY e;
    for (int n=0; m_loader[n].ext; n++)
    {
        if (strcmp(m_loader[n].ext, szExt) == 0)
        {
            return m_loader[n].func;
        }
    }
    sprintf(token, "Unsupported i/p format: %s", szExt);
    errno = EINVAL;
    LastCall(token);
}

// Get loader function
static PFN_SAVE get_save_func(LPCSTR szExt)
{
    static char token[MAX_TOKEN];
    PSAVEENTRY e;
    for (int n=0; m_saver[n].ext; n++)
    {
        if (strcmp(m_saver[n].ext, szExt) == 0)
        {
            return m_saver[n].func;
        }
    }
    sprintf(token, "Unsupported o/p format: %s", szExt);
    errno = EINVAL;
    LastCall(token);
}

// Get lister function
static PFN_LIST get_list_func(LPCSTR szExt)
{
    static char token[MAX_TOKEN];
    PLISTENTRY e;
    for (int n=0; m_lister[n].ext; n++)
    {
        if (strcmp(m_lister[n].ext, szExt) == 0)
        {
            return m_lister[n].func;
        }
    }
    sprintf(token, "Unsupported lister format: %s", szExt);
    errno = EINVAL;
    LastCall(token);
}


// Show help
static void usage(void)
{
    printf("\nUSAGE:\n");
    printf("convert <infile> <outfile>\n\n");
    exit(0);
}

// Handle arguments
static void sort_out_args(int argc, char **argp, LPRENAMER info)
{
    int numfiles = 0;
    ClearBuffer(info, sizeof(t_renamer));
    for (int x=1; x<argc; x++)
    {
        if (numfiles > 1)
        {
            usage();
        }
        if (++numfiles == 1)
        {
            strcpy(info->szInputName, argp[x]);
        }
        else
        {
            strcpy(info->szOutputName, argp[x]);
        }
    }
    if (numfiles < 1)
    {
        usage();
    }
    info->szInputExt = GetExtPtr(info->szInputName);    
    if (numfiles < 2)
    {
        info->bListMode = TRUE;
    }
    else
    {
        info->szOutputExt = GetExtPtr(info->szOutputName);
        if (strcmp(info->szInputName, info->szOutputName) == 0)
        {
            info->bListMode = TRUE;
        }
    }
}

// Helper to convert image file from one format to another
static int cvt(LPRENAMER info)
{
    if (info->bListMode)
    {
        fprintf(stderr, "Listing : %s\n", info->szInputName);
        PFN_LIST list = get_list_func(info->szInputExt);
        list(info->szInputName);
    }
    else
    {
        PFN_LOAD load = get_load_func(info->szInputExt);
        fprintf(stderr, "Loading : %s\n", info->szInputName);
        LPIMAGE img = load(info->szInputName);
        fprintf(stderr, "Saving : %s\n", info->szOutputName);
        PFN_SAVE save = get_save_func(info->szOutputExt);
        save(info->szOutputName, img);
    }
    return 0;
}

// Application entry point
int main(int argc, char **argp, char **envp)
{
    t_renamer info;
    sort_out_args(argc, argp, &info);
    return cvt(&info);
}

// Thunk for conformity with the function call standard
// for save functions
static BOOL save_image_file_ppm6(LPCSTR szPathName, LPIMAGE pImage)
{
    return SaveImageFilePpm(szPathName, pImage, P6);
}

