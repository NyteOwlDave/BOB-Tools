
#ifndef BC_H_DEFINED
#define BC_H_DEFINED

#include "map.h"


// Read BC file
extern void LoadBobColorFile(LPCSTR szPathName, PRGBPALETTE pal);

// Write BC file
extern void SaveBobColorFile(LPCSTR szPathName, PRGBPALETTE pal);


#endif // !BC_H_DEFINED

