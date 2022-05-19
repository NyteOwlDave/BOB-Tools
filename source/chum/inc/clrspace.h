#ifndef CLRSPACE_H_DEFINED
#define CLRSPACE_H_DEFINED

#include "wintypes.h"

// Fixed point 2.30
typedef DWORD FXPT2DOT30;

// TODO: These are guesses!
typedef DWORD LCSCSTYPE;
typedef DWORD LCSGAMUTMATCH;

typedef struct tagCIEXYZ {
  FXPT2DOT30 ciexyzX;
  FXPT2DOT30 ciexyzY;
  FXPT2DOT30 ciexyzZ;
} CIEXYZ;

typedef struct tagICEXYZTRIPLE {
  CIEXYZ ciexyzRed;
  CIEXYZ ciexyzGreen;
  CIEXYZ ciexyzBlue;
} CIEXYZTRIPLE;

typedef struct tagLOGCOLORSPACEA {
  DWORD         lcsSignature;
  DWORD         lcsVersion;
  DWORD         lcsSize;
  LCSCSTYPE     lcsCSType;
  LCSGAMUTMATCH lcsIntent;
  CIEXYZTRIPLE  lcsEndpoints;
  DWORD         lcsGammaRed;
  DWORD         lcsGammaGreen;
  DWORD         lcsGammaBlue;
  CHAR          lcsFilename[MAX_PATH];
} LOGCOLORSPACEA, *LPLOGCOLORSPACEA;

#endif // !CLRSPACE_H_DEFINED
