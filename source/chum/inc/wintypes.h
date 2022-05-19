#ifndef WINTYPES_H_DEFINED
#define WINTYPES_H_DEFINED

#define FALSE 0
#define TRUE 1
typedef int BOOL;

typedef void VOID;

typedef char CHAR;
typedef short SHORT;
typedef int INT;
typedef long LONG;

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long QWORD;

typedef char* LPSTR;
typedef const char* LPCSTR;
typedef void* LPVOID;
typedef const void* LPCVOID;

typedef struct tagMEMO {
    LPCSTR szName;
    LPCSTR szDesc;
    ULONG id;
} MEMO, *LPMEMO;

#endif // !WINTYPES_H_DEFINED

