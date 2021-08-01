
/*

    drawtest.c

    Tests for the gdi 2d drawing system

*/

#include "inc/stdafx.h"
#include "inc/drawtest.h"


// Show help
static void usage(void)
{
    printf("\nUSAGE:\n");
    printf("drawtest [flags] <what>\n");
    printf("\nFLAGS:\n");
    printf("-b\t\tBlended\n");
    printf("-c\t\tClipped (poly test)\n");
    printf("-d\t\tDotted\n");
    printf("-f\t\tFilled\n");
    printf("-t\t\tTextured\n");
    printf("\nWHAT:\n");
    printf("point\n");
    printf("hline\n");
    printf("vline\n");
    printf("line\n");
    printf("polyline\n");
    printf("linelist\n");
    printf("rect\n");
    printf("cirlce\n");
    printf("ellipse\n");
    printf("patch\n");
    printf("poly\n");
    printf("blit\n");
    printf("stretch\n");
    exit(0);
}

// Handle arguments
static void sort_out_args(int argc, char **argp, PFLAGS pflags)
{
    if (argc < 2) usage();
    memset(pflags, 0, sizeof(t_flags));
    pflags->what = W_ERROR;
    for (int i=1; i<argc; i++)
    {
        // Info(argp[i]);
        int ch = argp[i][0];
        if (ch == '-')
        {
            switch(argp[i][1])
            {
            case 'b':
                pflags->blended = TRUE;
                Log("Blended");
                break;
            case 'c':
                pflags->clipped = TRUE;
                Log("Clipped");
                break;
            case 'd':
                pflags->dotted = TRUE;
                Log("Dotted");
                break;
            case 'f':
                pflags->filled = TRUE;
                Log("Filled");
                break;
            case 't':
                pflags->textured = TRUE;
                Log("Textured");
                break;
            default:
                usage();
            }
        }
        else
        {
            if (pflags->what >= 0)
                usage();
            pflags->what = GetTokenID(argp[i]);
            if (pflags-> what < 0)
                usage();
        }
    }
}

// Dispatcher
static void dispatch(PFLAGS pflags)
{
    PFN_TEST func = GetTestFromID(pflags->what);
    if (!func) usage();
    func(pflags);
}

// Application entry point
int main(int argc, char **argp, char **envp)
{
    t_flags flags;
    sort_out_args(argc, argp, &flags);
    dispatch(&flags);
    return 0;
}
