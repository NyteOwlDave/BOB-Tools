
#ifndef VCP_H_DEFINED
#define VCP_H_DEFINED

#include "map.h"


// Read VCP file
void LoadVcpFile(LPCSTR szPathName, PRGBPALETTE pal);

// Write VCP file
void SaveVcpFile(LPCSTR szPathName, PRGBPALETTE pal);


#endif // !VCP_H_DEFINED

