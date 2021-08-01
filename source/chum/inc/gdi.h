
#ifndef GDI_H_DEFINED
#define GDI_H_DEFINED

#include "image.h"
#include "map.h"
#include "mtx.h"


// 3x2 matrix
typedef struct _t_mat3x2 {
    float m[2][2];
    float t[2];
} MAT3X2, *PMAT3X2;

// Dotted lines
typedef struct _t_dots {
    int current;
    int length;
    char* szPattern;
} t_dots, *PDOTS;

// Device Context
typedef struct _t_dc {
    RGBColor    fgc;
    RGBColor    bgc;
    RGBColor    xpc;
    float       blend;
    t_dots      dots;
    PIMAGE      frame;
    PIMAGE      tex;
    MAT3X2      tex_mtx;
    int cl, cr, ct, cb;
} DC, *PDC;

// Point
typedef struct _t_point {
    float x;
    float y;
} POINT, *PPOINT;

// Size
typedef struct _t_size {
    float w;
    float h;
} SIZE, *PSIZE;

// Rectangle
typedef struct _t_rect {
    float x;
    float y;
    float w;
    float h;
} RECT, *PRECT;

// Span filler info
typedef struct t_span
{
    LPIMAGE img;
    LPIMAGE tex;
    ARGB color;
    int x1;
    int x2;
    int y1;
    int y2;
    int dx;
    int dy;
    float u;
    float v;
    float du;
    float dv;
    float blend;
} t_span, *PSPAN;

typedef void (*PFN_SPAN)(PSPAN);

extern void DrawSpanSolid(PSPAN span);
extern void DrawSpanBlend(PSPAN span);
extern void DrawSpanTex(PSPAN span);
extern void DrawSpanTexBlend(PSPAN span);

extern PDC CreateDC(int w, int h);
extern void DestroyDC(PDC pdc);
extern void ClearDC(PDC pdc);
extern void ClipDC(PDC pdc, PRECT prc);

extern void SetLinePattern(PDC pdc, LPCSTR sz);
extern ARGB PixelFromColor(RGBColor color);
extern float PolyNormalZ(POINT pa, POINT pb, POINT pc);
extern void TexelCoords(PDC pdc, float x, float y, float *u, float *v);
extern BOOL DottedColor(PDC pdc, PARGB pcolor);

extern void InitIdentityMat3x2(PMAT3X2 mout);
extern void InitTranslateMat3x2(float tx, float ty, PMAT3X2 mout);
extern void InitScaleMat3x2(float sx, float sy, PMAT3X2 mout);
extern void InitRotateMat3x2(float r, PMAT3X2 mout);
extern void InitPatchMat3x2(POINT a, POINT b, POINT c, PMAT3X2 mout);
extern void TranslateMat3x2(float tx, float ty, PMAT3X2 min, PMAT3X2 mout);
extern void ScaleMat3x2(float sx, float sy, PMAT3X2 min, PMAT3X2 mout);
extern void RotateMat3x2(float r, PMAT3X2 min, PMAT3X2 mout);
extern void CatMat3x2(PMAT3X2 ma, PMAT3X2 mb, PMAT3X2 mout);
extern POINT ApplyMat3x2(PMAT3X2 mtx, POINT pt);
extern void ApplyListMat3x2(PMAT3X2 mtx, PPOINT pin, PPOINT pout, int count);

extern void InitRect(PRECT prc, float x, float y, float w, float h);
extern BOOL CopyRect(PRECT prc, PRECT pout);
extern BOOL IsNullRect(PRECT prc);
extern BOOL IsNormalRect(PRECT prc);
extern void UnionRect(PRECT pa, PRECT pb, PRECT pout);
extern void IntersectRect(PRECT pa, PRECT pb, PRECT pout);
extern void NormalizeRect(PRECT prc);
extern void InflateRect(PRECT prc, SIZE sz, PRECT pout);
extern void OffsetRect(PRECT prc, SIZE sz, PRECT pout);
extern POINT GetRectStart(PRECT prc);
extern POINT GetRectEnd(PRECT prc);
extern SIZE GetRectSize(PRECT prc);
extern void FlipRectHorz(PRECT prc);
extern void FlipRectVert(PRECT prc);
extern void FlipRect(PRECT prc);
extern void PreviewRect(SIZE parent, SIZE child, PRECT pout);
extern void GetImageRect(LPIMAGE img, PRECT pout);
extern SIZE GetImageSize(LPIMAGE img);

extern void DrawPoint(PDC pdc, POINT pt);
extern void DrawPoints(PDC pdc, PPOINT ppt, int num_points);

extern void DrawHLine(PDC pdc, POINT a, int count);
extern void DrawVLine(PDC pdc, POINT a, int count);
extern void DrawLine(PDC pdc, POINT a, POINT b);
extern void DrawPolyLine(PDC pdc, PPOINT pts, int num_lines);
extern void DrawLineList(PDC pdc, PPOINT pts, int num_lines);

extern void DrawCircle(PDC pdc, POINT center, float radius);
extern void FillCircle(PDC pdc, POINT center, float radius);

extern void DrawEllipse(PDC pdc, POINT center, float rx, float ry);
extern void FillEllipse(PDC pdc, POINT center, float rx, float ry);

extern void DrawPatch(PDC pdc, PPOINT pts);
extern void FillPatch(PDC pdc, PPOINT pts);

extern void DrawPoly(PDC pdc, PPOINT pts, int count);
extern void FillPoly(PDC pdc, PPOINT pts, int count);

extern void DrawRect(PDC pdc, PRECT prc);
extern void FillRect(PDC pdc, PRECT prc);

extern void BlitImage(PDC pdc, LPIMAGE img, POINT pt);
extern void StretchImage(PDC pdc, LPIMAGE img, POINT pt, SIZE sz);


#endif // !GDI_H_DEFINED

