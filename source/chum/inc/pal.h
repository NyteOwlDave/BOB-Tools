
#ifndef PAL_H_DEFINED
#define PAL_H_DEFINED

#include "map.h"


// Read pal file
void LoadPalFile(LPCSTR szPathName, PRGBPALETTE pal);

// Write pal file
void SavePalFile(LPCSTR szPathName, PRGBPALETTE pal);


#endif // !PAL_H_DEFINED

