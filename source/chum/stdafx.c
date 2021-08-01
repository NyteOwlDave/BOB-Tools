
/*

    stdafx.c

    Standard system functions

*/

#include "inc/stdafx.h"

static LPCSTR m_errmsg[] = {
    "Invalid magic number",             // EBADMAGIC 
    "Unsupported file format",          // EBADFORMAT
    "Feature not implemented",          // ENOTIMPL  
    "Missing image point",              // ENOIMG    
    "Missing image buffer pointer",     // ENOIMGBFR 
    "Logical error in code",            // ELOGIC 
    NULL
};

void Info(LPCSTR szMsg)
{
    fprintf(stderr, "%s\n", szMsg ? szMsg : "(unspecified info)");
}

void Warning(LPCSTR szMsg)
{
    fprintf(stderr, "W: %s\n", szMsg ? szMsg : "(unspecified warning)");
}

void Error(LPCSTR szMsg)
{
    fprintf(stderr, "E: %s\n", szMsg ? szMsg : "(unspecified error)");
    exit(1);
}

void LastCall(LPCSTR sz)
{   
    int e = errno;
    if ((e >= EFIRSTUSER) && (e <= ELASTUSER))
    {
        int n = e - EFIRSTUSER;
        LPCSTR sz2 = m_errmsg[n];
        if (sz)
        {
            fprintf(stderr, "%s: %s\n", sz, sz2);
        }
        else 
        {
            fprintf(stderr, "%s\n", sz2);
        }
    }
    else
    {
        perror(sz);

    }
    exit(e);
}

// Locate filename extension
LPCSTR GetExtPtr(LPCSTR path)
{
    static LPCSTR szError = "Seeking filename extension";
    int pos = strlen(path) - 1;
    if (pos < 0)
    {
        errno = EINVAL;
        last_call(szError);
    }
    while ((pos >= 0) && (path[pos] != '.'))
    {
        if (path[pos] == '/')
        {
            pos = -1;
        }
        else
        {
            pos--;
        }
    }
    if (pos >= 0)
    {
        return path + ++pos;
    }
    errno = EINVAL;
    last_call(szError);
}

// Check a pointer
void CheckPtr(LPCVOID p, LPCSTR szName)
{
    if (p) return;
    errno = EFAULT;
    LastCall(szName);
}

// Open file for reading
FILE* OpenFileRead(LPCSTR szPathName, LPCSTR szRoutine)
{
    CheckPtr(szPathName, szRoutine);
    FILE* fp = fopen(szPathName, "r");
    if (!fp)
    {
        LastCall(szRoutine);
    }
    return fp;
}

// Open file for writing
FILE* OpenFileWrite(LPCSTR szPathName, LPCSTR szRoutine)
{
    CheckPtr(szPathName, szRoutine);
    FILE* fp = fopen(szPathName, "w");
    if (!fp)
    {
        LastCall(szRoutine);
    }
    return fp;
}

// Close an open file
void CloseFile(FILE* fp)
{
    if (fp) fclose(fp);
}

// Allocate memory
LPVOID CreateBuffer(size_t cb, BOOL bZero)
{
    LPVOID p = malloc(cb);
    if (p)
    {
        if (bZero)
        {
            memset(p, 0, cb);
        }
    }
    return p;
}

// Destroy memory buffer
void DestroyBuffer(LPVOID p)
{
    if (p) free(p);
}

// Check file pointer
void CheckFilePtr(FILE* fp, LPCSTR szRoutine)
{
    if (fp) return;
    errno = EBADF;
    LastCall(szRoutine);
}

// Fill memory buffer
void FillBuffer(LPVOID p, int n, size_t cb)
{
    CheckPtr(p, "FillBuffer");
    memset(p, n, cb);
}

// Clear memory buffer
void ClearBuffer(LPVOID p, size_t cb)
{
    CheckPtr(p, "CopyBuffer");
    FillBuffer(p, 0, cb);
}

// Copy memory buffer
void CopyBuffer(LPCVOID src, LPVOID dst, size_t cb)
{
    CheckPtr(src, "CopyBuffer (src");
    CheckPtr(dst, "CopyBuffer (dst)");
    if (memcpy(dst, src, cb)) return;
    LastCall("CopyBuffer");
}
