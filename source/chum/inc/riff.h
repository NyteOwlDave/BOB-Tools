
#ifndef RIFF_H_DEFINED
#define RIFF_H_DEFINED

#include "map.h"


// Read BC file
extern void LoadRiffFile(LPCSTR szPathName, PRGBPALETTE pal);

// Write BC file
extern void SaveRiffFile(LPCSTR szPathName, PRGBPALETTE pal);

// List RIFF header
void ListRiffHeader(LPCSTR szPathName);


#endif // !RIFF_H_DEFINED

