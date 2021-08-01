
#ifndef HEXC_H_DEFINED
#define HEXC_H_DEFINED

#include "map.h"


// Read HEXC file
extern void LoadHexcFile(LPCSTR szPathName, PRGBPALETTE pal);

// Write HEXC file
extern void SaveHexcFile(LPCSTR szPathName, PRGBPALETTE pal);


#endif // !HEXC_H_DEFINED

