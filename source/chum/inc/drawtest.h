
#ifndef DRAWTEST_H_DEFINED
#define DRAWTEST_H_DEFINED

#include "wintypes.h"
#include "gdi.h"


#define W_ERROR     (-1)
#define W_POINT     (0)
#define W_HLINE     (1)
#define W_VLINE     (2)
#define W_LINE      (3)
#define W_POLYLINE  (4)
#define W_LINELIST  (5)
#define W_RECT      (6)
#define W_CIRCLE    (7)
#define W_ELLIPSE   (8)
#define W_PATCH     (9)
#define W_POLY      (10)
#define W_BLIT      (11)
#define W_STRETCH   (12)
#define W_EXTRA     (13)


typedef struct _t_flags {
    BOOL blended;
    BOOL dotted;
    BOOL textured;
    BOOL filled;
    BOOL clipped;
    int what;
} t_flags, *PFLAGS;

typedef void (*PFN_TEST)(PFLAGS pflags);

typedef struct _t_dispatch {
    int what;
    LPCSTR token;
    PFN_TEST func;
} t_dispatch, *PDISPATCH;


extern void test_DrawPoints( PFLAGS pflags );
extern void test_DrawHLine( PFLAGS pflags );   
extern void test_DrawVLine( PFLAGS pflags );   
extern void test_DrawLine( PFLAGS pflags );    
extern void test_DrawPolyLine( PFLAGS pflags );
extern void test_DrawLineList( PFLAGS pflags );
extern void test_DrawCircle( PFLAGS pflags );
extern void test_DrawEllipse( PFLAGS pflags ); 
extern void test_DrawPatch( PFLAGS pflags );   
extern void test_DrawPoly( PFLAGS pflags );    
extern void test_DrawRect( PFLAGS pflags );    
extern void test_FillCircle( PFLAGS pflags );  
extern void test_FillEllipse( PFLAGS pflags ); 
extern void test_FillPatch( PFLAGS pflags );   
extern void test_FillPoly( PFLAGS pflags );    
extern void test_FillRect( PFLAGS pflags );    
extern void test_BlitImage( PFLAGS pflags );
extern void test_StretchImage( PFLAGS pflags );
extern void test_Extra( PFLAGS pflags );

extern int GetTokenID(LPCSTR token);
extern PFN_TEST GetTestFromID(int what);


#endif // !DRAWTEST_H_DEFINED
