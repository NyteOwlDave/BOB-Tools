
/*

    cmap.c 
    
    Main file for cmap app

    Color Map conversions

*/
 
#include "inc/stdafx.h"
#include "inc/map.h"
#include "inc/pal.h"
#include "inc/bc.h"
#include "inc/vcp.h"
#include "inc/ortc.h"
#include "inc/hexc.h"
#include "inc/dotc.h"
#include "inc/riff.h"


static int m_numfiles = 0;
static char m_infile[MAX_PATH+1];
static char m_outfile[MAX_PATH+1];
static BOOL m_listmode = FALSE;
static BOOL m_viewmode = FALSE;
static BOOL m_hdrmode = FALSE;
static BOOL m_jsonlist = FALSE;
static BOOL m_clist = FALSE;
static LPCSTR m_inext = NULL;
static LPCSTR m_outext = NULL;

typedef void (*PFN_LOAD)(LPCSTR szPathName, PRGBPALETTE pal);
typedef void (*PFN_SAVE)(LPCSTR szPathName, PRGBPALETTE pal);
typedef void (*PFN_HDR)(LPCSTR szPathName);

typedef struct _t_load_entry {
    LPCSTR ext;
    PFN_LOAD func;
} t_load_entry, *PLOADENTRY;

typedef struct _t_save_entry {
    LPCSTR ext;
    PFN_SAVE func;
} t_save_entry, *PSAVEENTRY;

typedef struct _t_hdr_entry {
    LPCSTR ext;
    PFN_HDR func;
} t_hdr_entry, *PHDRENTRY;


static t_load_entry m_loader[] = {
    { "map", LoadMapFile },
    { "pal", LoadPalFile },
    { "bc", LoadBobColorFile },
    { "vcp", LoadVcpFile },
    { "ortc", LoadOrtcFile },
    { "hexc", LoadHexcFile },
    { "dotc", LoadDotcFile },
    { "riff", LoadRiffFile },
    { NULL, NULL }
};

static t_save_entry m_saver[] = {
    { "map", SaveMapFile },
    { "pal", SavePalFile },
    { "bc", SaveBobColorFile },
    { "vcp", SaveVcpFile },
    { "ortc", SaveOrtcFile },
    { "hexc", SaveHexcFile },
    { "dotc", SaveDotcFile },
    { "riff", SaveRiffFile },
    { NULL, NULL }
};

static t_hdr_entry m_hdr[] = {
//    { "map", SaveMapFile },
//    { "pal", SavePalFile },
//    { "bc", SaveBobColorFile },
//    { "vcp", SaveVcpFile },
//    { "ortc", SaveOrtcFile },
//    { "hexc", SaveHexcFile },
//    { "dotc", SaveDotcFile },
    { "riff", ListRiffHeader },
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

// Get saver function
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

// Get header list function
static PFN_HDR get_hdr_func(LPCSTR szExt)
{
    static char token[MAX_TOKEN];
    PHDRENTRY e;
    for (int n=0; m_hdr[n].ext; n++)
    {
        if (strcmp(m_hdr[n].ext, szExt) == 0)
        {
            return m_hdr[n].func;
        }
    }
    sprintf(token, "Unsupported header list format: %s", szExt);
    errno = EINVAL;
    LastCall(token);
}


// Show help
static void usage(void)
{
    printf("\nUSAGE:\n");
    printf("cmap <flags> infile [outfile]\n");
    printf("\nFLAGS:\n");
    printf("-c\t\tPrint as C\n");
    printf("-h\t\tPrint out header (if any)\n");
    printf("-j\t\tPrint as JSON\n");
    printf("-v\t\tGenerate preview pixmap\n");
    exit(0);
}

// Handle arguments
static void sort_out_args(int argc, char **argp)
{
    m_numfiles = 0;
    m_infile[0] = 0;
    m_outfile[0] = 0;
    m_inext = NULL;
    m_outext = NULL;
    if (argc < 2)
    {
        usage();
    }
    for (int x=1; x<argc; x++)
    {
        int ch = argp[x][0];
        if (ch == '-')
        {
            ch = argp[x][1];
            switch (ch)
            {
            case 'v':
                m_viewmode = TRUE;
                break;
            case 'h':
                m_hdrmode = TRUE;
                break;
            case 'j':
                m_jsonlist = TRUE;
                break;
            case 'c':
                m_clist = TRUE;
                break;
            default:
                usage();
            }
            continue;
        }
        if (m_numfiles > 1)
        {
            usage();
        }
        if (++m_numfiles == 1)
        {
            strcpy(m_infile, argp[x]);
        }
        else
        {
            strcpy(m_outfile, argp[x]);
        }
    }
    if (m_numfiles < 1)
    {
        usage();
    }
    m_inext = GetExtPtr(m_infile);
    m_listmode = m_numfiles > 1 ? FALSE : TRUE;
    m_viewmode = m_listmode && m_viewmode;
    m_hdrmode = m_hdrmode && m_listmode;
    if (!m_listmode)
    {
        m_outext = GetExtPtr(m_outfile);
        if (strcmp(m_inext, m_outext) == 0)
        {
            errno = EINVAL;
            LastCall("Source and target must not match");
        }
    }
}

// Does most of the real work
static int map_colors(void)
{
    RGBPalette pal; 
    ClearBuffer(&pal, sizeof(RGBPalette));
    PFN_LOAD load = get_load_func(m_inext);
    if (m_listmode)
    {
        if (m_viewmode)
        {
            fprintf(stderr, "Loading : %s\n", m_infile);
            load(m_infile, &pal);
            fprintf(stderr, "Viewing : %s\n", m_infile);
            ViewPalette(&pal);
        }
        else if (m_hdrmode)
        {
            PFN_HDR hdr = get_hdr_func(m_inext);
            fprintf(stderr, "Header : %s\n", m_infile);
            hdr(m_infile);
        }
        else
        {
            fprintf(stderr, "Loading : %s\n", m_infile);
            load(m_infile, &pal);
            fprintf(stderr, "Listing : %s\n", m_infile);
            if (m_jsonlist)     PrintPaletteJSON(&pal);
            else if (m_clist)   PrintPaletteC(&pal);
            else                PrintPalette(&pal);
        }
    }
    else
    {
        fprintf(stderr, "Loading : %s\n", m_infile);
        load(m_infile, &pal);
        fprintf(stderr, "Saving : %s\n", m_outfile);
        PFN_SAVE save = get_save_func(m_outext);
        save(m_outfile, &pal);
    }
    return 0;
}

// Application entry point
int main(int argc, char **argp, char **envp)
{
    sort_out_args(argc, argp);
    return map_colors();
}
