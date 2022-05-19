
/*

    sys.c

    System support

*/

#include <memory.h>
#include <sys/stat.h>
#include "inc/forge.h"


// Allocate buffer
PVOID vmalloc(size_t size) 
{
    const PVOID p = malloc(size);
    memset(p, 0, size);
    return p;
}

// Last call (fails)
void last_call(PCSTR msg, int retcode) 
{
    fprintf(stderr, "Fatal error. ");
    if (msg) {
        fprintf(stderr, "%s. Return code: %i\n", msg, retcode);
    } else {
        fprintf(stderr, "Return code: %i\n", retcode);
    }
}

// Pointer check
void ptr_check(PCVOID ptr, PCSTR msg) 
{
    if (ptr) return;
    fprintf(stderr, "Null pointer. ");
    last_call(msg, 1);
}

// Ensures a file exists
BOOL FileExists(PCSTR szPath)
{
    struct stat st;
    int n = stat(szPath, &st);
    if (n)
    {
        return FALSE;
    }
    return (st.st_mode & __S_IFREG);
}

// Ensures a folder exists
BOOL FolderExists(PCSTR szPath)
{
    struct stat st;
    int n = stat(szPath, &st);
    if (n)
    {
        return FALSE;
    }
    return (st.st_mode & __S_IFDIR);
}

// Invokes the command shell
void Shell(PCSTR command)
{
    Print("");
    if ((!command) || (!command[0]))
    {
        system("start");
    }
    else
    {
        system(command);
    }
    Print("");
}

// Displays a file directory
// NOTE: updated from Win to Linux
void Dir(PCSTR path)
{
    if (path)
    {
        char temp[MAX_TOKEN + MAX_PATH];
        sprintf(temp, "ls %s", path);
        Shell(temp);
    }
    else
    {
        Shell("ls");
    }
}

// Clear the screen
// NOTE: updated from Win to Linux
void Cls()
{
    system("clear");
}

// Pauses for a keypress
// NOTE: updated from Win to Linux
void Pause()
{
    // NOTE: Need to rewrite
    Shell("read -n1 -r -p \"Press any key to continue...\" key");
    // Shell("pause");
}

// Lists a text file
void ListFile(PCSTR path)
{
    if (path)
    {
        char temp[MAX_TOKEN + MAX_PATH];
        sprintf(temp, "type %s", path);
        Shell(temp);
    }
}

// Displays an ordinary string
void Print(PCSTR msg)
{
    fprintf(stderr, "%s\n", msg);
}

// Displays a boolean flag
void PrintFlag(PCSTR msg, BOOL flag)
{
    fprintf(stderr, "%s = %s\n", msg, flag ? "TRUE" : "FALSE");
}

// Displays a text message
void PrintText(PCSTR msg, PCSTR txt)
{
    fprintf(stderr, "%s = %s\n", msg, txt);
}

// Displays a text array
// (final array element MUST be NULL!!!)
void PrintTextArray(PCSTR msg, PCSTR *pa)
{
    fprintf(stderr, "%s\n", msg);
    while (*pa)
    {
        Print(*pa);
        ++pa;
    }
}

// Displays an integer
void PrintInteger(PCSTR msg, int num)
{
    fprintf(stderr, "%s = %i\n", msg, num);
}

// Displays a floating point number
void PrintFloat(PCSTR msg, double num)
{
    fprintf(stderr, "%s = %g\n", msg, num);
}

// Displays a hexadecimal number
void PrintHex(PCSTR msg, DWORD num)
{
    fprintf(stderr, "%s = 0x%08Xi\n", msg, num);
}

// Displays a memory address
void PrintAddress(PCSTR msg, const void *p)
{
    if (p)
    {
        QWORD hi = (QWORD)p >> 16;
        QWORD lo = (QWORD)p & 0xFFFFFFFF;
        fprintf(stderr, "%s = %08X:%08X\n", msg, (DWORD)hi, (DWORD)lo);
    }
    else
    {
        fprintf(stderr, "%s = NULL\n", msg);
    }
}

// Displays
void PrintVector(PCSTR msg, Vec v)
{
    fprintf(stderr, "%s = %g %g %g\n", msg, v[0], v[1], v[2]);
}

// Displays a 4x4 matrix
void PrintMatrix(PCSTR msg, Mtx m)
{
    fprintf(stderr, "%s = \n", msg);
    fprintf(stderr, "%g %g %g %g\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    fprintf(stderr, "%g %g %g %g\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    fprintf(stderr, "%g %g %g %g\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    fprintf(stderr, "%g %g %g %g\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

