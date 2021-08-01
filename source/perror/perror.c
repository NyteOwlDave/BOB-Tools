#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

typedef int exit_t;

#define MAX_KNOWN_ERRNO 133
#define MAX_TOKEN 255

char m_buffer[MAX_TOKEN+1];

static void showerrno(int n)
{
    sprintf(m_buffer, "%i", n);
    errno = n;
    perror(m_buffer);
}

static void usage(void)
{
    printf("\nUSAGE:\n");
    printf("perror <flags> [errno [...]]\n");
    printf("\nFLAGS:\n");
    printf("-all\t\tShow all known messages\n\n");
    exit(0);
}

static void showall(void)
{
    for (int x=1; x<=MAX_KNOWN_ERRNO; x++) 
    {
        showerrno(x);
    }
    exit(0);
}

static int go(int argc, char** argv)
{
    if (argc < 2)
    {
        usage();
        // No return
    }
    for (int x=1; x<argc; x++) 
    {
        int ch = argv[x][0];
        if (isdigit(ch))
        {
            showerrno(atoi(argv[x]));
        }
        else if (ch == '-')
        {
            if (strcmp(argv[x], "-all") == 0)
            {
                showall();
                // No return
            }
        }
    }
    return 0;
}

int main(int argc, char** argv, char** envp)
{
  return go(argc, argv);
}

