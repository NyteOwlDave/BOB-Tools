
#ifndef POLYGON_H_DEFINED
#define POLYGON_H_DEFINED


#include "gdi.h"


#define CONVEX 0
#define NONCONVEX 1
#define COMPLEX 2

 
// Describes a single point (used for a single vertex)
struct Point
{
    int X;  // X coordinate
    int Y;  // Y coordinate
};

// Describes series of points (used to store a list of vertices that describe 
// a polygon; each vertex is assumed to connect to the two adjacent vertices, and 
// last vertex is assumed to connect to the first)
struct PointListHeader
{
    int Allocated;          // # of allocated points
    int Length;             // # of points
    struct Point *PointPtr; // pointer to list of points
};

// Describes beginning and ending X coordinates of a single horizontal line
struct HLine
{
    int XStart; // X coordinate of leftmost pixel in line
    int XEnd;   // X coordinate of rightmost pixel in line
};

// Describes a Length-long series of horizontal lines, all assumed to be on 
// contiguous scan lines starting at YStart and proceeding downward (used to 
// describe scan-converted polygon to low-level hardware-dependent drawing code)
struct HLineList
{
    int Length;             // # of horizontal lines
    int YStart;             // Y coordinate of topmost line
    struct HLine *HLinePtr; // pointer to list of horz lines
};

// Describes a color as an RGB triple, plus one byte for other info
/*
struct RGB
{
    BYTE Red;
    BYTE Green;
    BYTE Blue;
    BYTE Spare;
};
*/


// extern void ClearScreen();
// extern void Snapshot();
// extern void SetForegroundColor(int index);

// extern void DrawPixel(int x, int y, int color);

extern BOOL FillConvexPolygon(PDC pdc, struct PointListHeader *VertexList,
    int XOffset, int YOffset);
extern BOOL FillPolygon(PDC pdc, struct PointListHeader *VertexList,
    int PolygonShape, int XOffset, int YOffset);
extern BOOL FillMonotoneVerticalPolygon(PDC pdc, struct PointListHeader *VertexList,
    int XOffset, int YOffset);

extern BOOL PolygonIsMonotoneVertical(struct PointListHeader *VertexList);

extern BOOL CreatePointList(struct PointListHeader * ptr, int length);
extern void DestroyPointList(struct PointListHeader * ptr);
extern void ConvertToPointList(PPOINT pin, struct Point* pout, int length);
extern void ConvertFromPointList(struct Point* pin, PPOINT pout, int length);

extern BOOL ClipPolygon(PDC pdc, struct PointListHeader* PointList);

// Global DC
// extern PDC g_pdc;

// Global palette
// extern RGBPalette g_pal;


#endif // !POLYGON_H_DEFINED
