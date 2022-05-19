
#ifndef DOTC_H_DEFINED
#define DOTC_H_DEFINED

#include "map.h"


// Read HEXC file
extern void LoadDotcFile(LPCSTR szPathName, PRGBPALETTE pal);

// Write HEXC file
extern void SaveDotcFile(LPCSTR szPathName, PRGBPALETTE pal);


#endif // !DOTC_H_DEFINED

