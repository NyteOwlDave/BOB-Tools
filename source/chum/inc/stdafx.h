
#ifndef STDAFX_H_DEFINED
#define STDAFX_H_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <memory.h>
#include "wintypes.h"


#define MAX_TOKEN   (80)
#define MAX_PATH    (260)


extern void Info(LPCSTR szMsg);
extern void Warning(LPCSTR szMsg);
extern void Error(LPCSTR szMsg);
extern void LastCall(LPCSTR sz);
extern LPCSTR GetExtPtr(LPCSTR path);
extern void CheckPtr(LPCVOID p, LPCSTR szName);
extern FILE* OpenFileWrite(LPCSTR szPathName, LPCSTR szRoutine);
extern FILE* OpenFileRead(LPCSTR szPathName, LPCSTR szRoutine);
extern void CloseFile(FILE* fp);
extern void CheckFilePtr(FILE* fp, LPCSTR szRoutine);
extern LPVOID CreateBuffer(size_t cb, BOOL bZero);
extern void DestroyBuffer(LPVOID p);
extern void FillBuffer(LPVOID p, int n, size_t cb);
extern void ClearBuffer(LPVOID p, size_t cb);
extern void CopyBuffer(LPCVOID src, LPVOID dst, size_t cb);


#define last_call LastCall
#define get_ext_ptr GetExtPtr


#define EFIRSTUSER      (200)
#define EBADMAGIC       (EFIRSTUSER)
#define EBADFORMAT      (EFIRSTUSER+1)
#define ENOTIMPL        (EFIRSTUSER+2)
#define ENOIMG          (EFIRSTUSER+3)
#define ENOIMGBFR       (EFIRSTUSER+4)
#define ELOGIC          (EFIRSTUSER+5)
#define ELASTUSER       ELOGIC

#include "log.h"


#endif // !STDAFX_H_DEFINED

