
// #define _DEBUG_MODE_

#include "wintypes.h"

static void Log(LPCSTR msg)
{
    #ifdef _DEBUG_MODE_
        Info(msg);
    #else
    if (getenv("CHUM_DEBUG"))
    {
        Info(msg);
    }
    #endif
}

