
/*

    Polygon Support

*/

#include <malloc.h>
#include "inc/stdafx.h"
#include "inc/polygon.h"


#define SWAP(a, b) \
    {              \
        temp = a;  \
        a = b;     \
        b = temp;  \
    }

struct EdgeState
{
    struct EdgeState *NextEdge;
    int X;
    int StartY;
    int WholePixelXMove;
    int XDirection;
    int ErrorTerm;
    int ErrorTermAdjUp;
    int ErrorTermAdjDown;
    int Count;
};


static void build_get(struct PointListHeader *, struct EdgeState *,
                     int, int);
static void move_xsorted_to_aet(int);
static void scan_out_aet(PDC, int);
static void advance_aet(void);
static void xsort_aet(void);

static void draw_hline_seg(PDC pdc, int Y, int LeftX, int RightX);
static void draw_hline_list(PDC pdc, struct HLineList *HLineListPtr);

static void scan_edge(int X1, int Y1, int X2, int Y2, int SetXStart,
    int SkipFirst, struct HLine **EdgePointPtr);
static void scan_edge2(int X1, int Y1, int X2, int Y2, int SetXStart,
    int SkipFirst, struct HLine **EdgePointPtr);


// Pointers to global edge table (GET) and active edge table (AET)
static struct EdgeState *GETPtr, *AETPtr;


/* 

   Color-fills an arbitrarily-shaped polygon described by VertexList.
   If the first and last points in VertexList are not the same, the path
   around the polygon is automatically closed. All vertices are offset
   by (XOffset, YOffset). Returns 1 for success, 0 if memory allocation
   failed.

   If the polygon shape is known in advance, speedier processing may be
   enabled by specifying the shape as follows: "convex" - a rubber band
   stretched around the polygon would touch every vertex in order;
   "nonconvex" - the polygon is not self-intersecting, but need not be
   convex; "complex" - the polygon may be self-intersecting, or, indeed,
   any sort of polygon at all. Complex will work for all polygons; convex
   is fastest. Undefined results will occur if convex is specified for a
   nonconvex or complex polygon.

   Define CONVEX_CODE_LINKED if the fast convex polygon filling code from
   Chapter 21 is linked in. Otherwise, convex polygons are
   handled by the complex polygon filling code.

   Nonconvex is handled as complex in this implementation. See text for a
   discussion of faster nonconvex handling

*/

BOOL FillPolygon(PDC pdc, struct PointListHeader *VertexList, 
                int PolygonShape, int XOffset, int YOffset)
{
    struct EdgeState *EdgeTableBuffer;
    int CurrentY;

    // Pass convex polygons through to fast convex polygon filler
    if (PolygonShape == CONVEX)
        return (FillConvexPolygon(pdc, VertexList, XOffset, YOffset));

    // It takes a minimum of 3 vertices to cause any pixels to be
    // drawn; reject polygons that are guaranteed to be invisible
    if (VertexList->Length < 3)
        return (1);
    // Get enough memory to store the entire edge table
    if ((EdgeTableBuffer =
             (struct EdgeState *)(malloc(sizeof(struct EdgeState) *
                                         VertexList->Length))) == NULL)
        return FALSE;   // couldn't get memory for the edge table
    // Build the global edge table
    build_get(VertexList, EdgeTableBuffer, XOffset, YOffset);
    // Scan down through the polygon edges, one scan line at a time,
    // so long as at least one edge remains in either the GET or AET
    AETPtr = NULL;              // initialize the active edge table to empty
    CurrentY = GETPtr->StartY;  // start at the top polygon vertex
    while ((GETPtr != NULL) || (AETPtr != NULL))
    {
        move_xsorted_to_aet(CurrentY);  // update AET for this scan line
        scan_out_aet(pdc, CurrentY);    // draw this scan line from AET
        advance_aet();                  // advance AET edges 1 scan line
        xsort_aet();                    // resort on X
        CurrentY++;                     // advance to the next scan line
    }
    // Release the memory we've allocated and we're done
    free(EdgeTableBuffer);
    return TRUE;
}


/*

   Creates a GET in the buffer pointed to by NextFreeEdgeStruc from
   the vertex list. Edge endpoints are flipped, if necessary, to
   guarantee all edges go top to bottom. The GET is sorted primarily
   by ascending Y start coordinate, and secondarily by ascending X
   start coordinate within edges with common Y coordinates 

*/

static void build_get(struct PointListHeader *VertexList,
                     struct EdgeState *NextFreeEdgeStruc, int XOffset, int YOffset)
{
    int i, StartX, StartY, EndX, EndY, DeltaY, DeltaX, Width, temp;
    struct EdgeState *NewEdgePtr;
    struct EdgeState *FollowingEdge, **FollowingEdgeLink;
    struct Point *VertexPtr;

    // Scan through the vertex list and put all non-0-height edges into
    // the GET, sorted by increasing Y start coordinate
    VertexPtr = VertexList->PointPtr; // point to the vertex list
    GETPtr = NULL;                    // initialize the global edge table to empty
    for (i = 0; i < VertexList->Length; i++)
    {
        // Calculate the edge height and width
        StartX = VertexPtr[i].X + XOffset;
        StartY = VertexPtr[i].Y + YOffset;
        // The edge runs from the current point to the previous one
        if (i == 0)
        {
            // Wrap back around to the end of the list
            EndX = VertexPtr[VertexList->Length - 1].X + XOffset;
            EndY = VertexPtr[VertexList->Length - 1].Y + YOffset;
        }
        else
        {
            EndX = VertexPtr[i - 1].X + XOffset;
            EndY = VertexPtr[i - 1].Y + YOffset;
        }
        // Make sure the edge runs top to bottom
        if (StartY > EndY)
        {
            SWAP(StartX, EndX);
            SWAP(StartY, EndY);
        }
        // Skip if this can't ever be an active edge (has 0 height)
        if ((DeltaY = EndY - StartY) != 0)
        {
            // Allocate space for this edge's info, and fill in the
            // structure
            NewEdgePtr = NextFreeEdgeStruc++;
            NewEdgePtr->XDirection =    // direction in which X moves
                ((DeltaX = EndX - StartX) > 0) ? 1 : -1;
            Width = abs(DeltaX);
            NewEdgePtr->X = StartX;
            NewEdgePtr->StartY = StartY;
            NewEdgePtr->Count = DeltaY;
            NewEdgePtr->ErrorTermAdjDown = DeltaY;
            if (DeltaX >= 0)    // initial error term going L->R
                NewEdgePtr->ErrorTerm = 0;
            else                // initial error term going R->L
                NewEdgePtr->ErrorTerm = -DeltaY + 1;
            if (DeltaY >= Width)
            { // Y-major edge
                NewEdgePtr->WholePixelXMove = 0;
                NewEdgePtr->ErrorTermAdjUp = Width;
            }
            else
            { // X-major edge
                NewEdgePtr->WholePixelXMove =
                    (Width / DeltaY) * NewEdgePtr->XDirection;
                NewEdgePtr->ErrorTermAdjUp = Width % DeltaY;
            }
            // Link the new edge into the GET so that the edge list is
            // still sorted by Y coordinate, and by X coordinate for all
            // edges with the same Y coordinate
            FollowingEdgeLink = &GETPtr;
            for (;;)
            {
                FollowingEdge = *FollowingEdgeLink;
                if ((FollowingEdge == NULL) ||
                    (FollowingEdge->StartY > StartY) ||
                    ((FollowingEdge->StartY == StartY) &&
                     (FollowingEdge->X >= StartX)))
                {
                    NewEdgePtr->NextEdge = FollowingEdge;
                    *FollowingEdgeLink = NewEdgePtr;
                    break;
                }
                FollowingEdgeLink = &FollowingEdge->NextEdge;
            }
        }
    }
}


/*

   Sorts all edges currently in the active edge table into ascending
   order of current X coordinates 

*/

static void xsort_aet()
{
    struct EdgeState *CurrentEdge, **CurrentEdgePtr, *TempEdge;
    int SwapOccurred;

    // Scan through the AET and swap any adjacent edges for which the
    // second edge is at a lower current X coord than the first edge.
    // Repeat until no further swapping is needed
    if (AETPtr != NULL)
    {
        do
        {
            SwapOccurred = 0;
            CurrentEdgePtr = &AETPtr;
            while ((CurrentEdge = *CurrentEdgePtr)->NextEdge != NULL)
            {
                if (CurrentEdge->X > CurrentEdge->NextEdge->X)
                {
                    // The second edge has a lower X than the first;
                    // swap them in the AET
                    TempEdge = CurrentEdge->NextEdge->NextEdge;
                    *CurrentEdgePtr = CurrentEdge->NextEdge;
                    CurrentEdge->NextEdge->NextEdge = CurrentEdge;
                    CurrentEdge->NextEdge = TempEdge;
                    SwapOccurred = 1;
                }
                CurrentEdgePtr = &(*CurrentEdgePtr)->NextEdge;
            }
        } while (SwapOccurred != 0);
    }
}


/*
   Advances each edge in the AET by one scan line.
   Removes edges that have been fully scanned.
*/

static void advance_aet()
{
    struct EdgeState *CurrentEdge, **CurrentEdgePtr;

    // Count down and remove or advance each edge in the AET
    CurrentEdgePtr = &AETPtr;
    while ((CurrentEdge = *CurrentEdgePtr) != NULL)
    {
        // Count off one scan line for this edge
        if ((--(CurrentEdge->Count)) == 0)
        {
            // This edge is finished, so remove it from the AET
            *CurrentEdgePtr = CurrentEdge->NextEdge;
        }
        else
        {
            // Advance the edge's X coordinate by minimum move
            CurrentEdge->X += CurrentEdge->WholePixelXMove;
            // Determine whether it's time for X to advance one extra
            if ((CurrentEdge->ErrorTerm +=
                 CurrentEdge->ErrorTermAdjUp) > 0)
            {
                CurrentEdge->X += CurrentEdge->XDirection;
                CurrentEdge->ErrorTerm -= CurrentEdge->ErrorTermAdjDown;
            }
            CurrentEdgePtr = &CurrentEdge->NextEdge;
        }
    }
}


/*

   Moves all edges that start at the specified Y coordinate from the
   GET to the AET, maintaining the X sorting of the AET. 

*/

static void move_xsorted_to_aet(int YToMove)
{
    struct EdgeState *AETEdge, **AETEdgePtr, *TempEdge;
    int CurrentX;

    // The GET is Y sorted. Any edges that start at the desired Y
    // coordinate will be first in the GET, so we'll move edges from
    // the GET to AET until the first edge left in the GET is no longer
    // at the desired Y coordinate. Also, the GET is X sorted within
    // each Y coordinate, so each successive edge we add to the AET is
    // guaranteed to belong later in the AET than the one just added
    AETEdgePtr = &AETPtr;
    while ((GETPtr != NULL) && (GETPtr->StartY == YToMove))
    {
        CurrentX = GETPtr->X;
        // Link the new edge into the AET so that the AET is still
        // sorted by X coordinate
        for (;;)
        {
            AETEdge = *AETEdgePtr;
            if ((AETEdge == NULL) || (AETEdge->X >= CurrentX))
            {
                TempEdge = GETPtr->NextEdge;
                *AETEdgePtr = GETPtr;   // link the edge into the AET
                GETPtr->NextEdge = AETEdge;
                AETEdgePtr = &GETPtr->NextEdge;
                GETPtr = TempEdge;      // unlink the edge from the GET
                break;
            }
            else
            {
                AETEdgePtr = &AETEdge->NextEdge;
            }
        }
    }
}


/*

   Fills the scan line described by the current AET at the specified Y
   coordinate in the specified color, using the odd/even fill rule 

*/

static void scan_out_aet(PDC pdc, int YToScan)
{
    int LeftX;
    struct EdgeState *CurrentEdge;

    // Scan through the AET, drawing line segments as each pair of edge
    //  crossings is encountered. The nearest pixel on or to the right
    //  of left edges is drawn, and the nearest pixel to the left of but
    //  not on right edges is drawn
    CurrentEdge = AETPtr;
    while (CurrentEdge != NULL)
    {
        LeftX = CurrentEdge->X;
        CurrentEdge = CurrentEdge->NextEdge;
        draw_hline_seg(pdc, YToScan, LeftX, CurrentEdge->X - 1);
        CurrentEdge = CurrentEdge->NextEdge;
    }
}


/* 
   
   Color-fills a convex polygon. All vertices are offset by (XOffset,
   YOffset). "Convex" means that every horizontal line drawn through
   the polygon at any point would cross exactly two active edges
   (neither horizontal lines nor zero-length edges count as active
   edges; both are acceptable anywhere in the polygon), and that the
   right & left edges never cross. (It's OK for them to touch, though,
   so long as the right edge never crosses over to the left of the
   left edge.) Nonconvex polygons won't be drawn properly. Returns 
   TRUE for success, FALSE if memory allocation failed.

 */

// Advances the index by one vertex forward through the vertex list,
// wrapping at the end of the list
#define INDEX_FORWARD(Index) \
    Index = (Index + 1) % VertexList->Length;

// Advances the index by one vertex backward through the vertex list,
// wrapping at the start of the list
#define INDEX_BACKWARD(Index) \
    Index = (Index - 1 + VertexList->Length) % VertexList->Length;

// Advances the index by one vertex either forward or backward through
// the vertex list, wrapping at either end of the list
#define INDEX_MOVE(Index, Direction)              \
    if (Direction > 0)                            \
        Index = (Index + 1) % VertexList->Length; \
    else                                          \
        Index = (Index - 1 + VertexList->Length) % VertexList->Length;


BOOL FillConvexPolygon(PDC pdc, struct PointListHeader *VertexList,
                      int XOffset, int YOffset)
{

    int i, MinIndexL, MaxIndex, MinIndexR, SkipFirst, Temp;
    int MinPoint_Y, MaxPoint_Y, TopIsFlat, LeftEdgeDir;
    int NextIndex, CurrentIndex, PreviousIndex;
    int DeltaXN, DeltaYN, DeltaXP, DeltaYP;
    struct HLineList WorkingHLineList;
    struct HLine *EdgePointPtr;
    struct Point *VertexPtr;

    // Point to the vertex list
    VertexPtr = VertexList->PointPtr;

    // Scan the list to find the top and bottom of the polygon
    if (VertexList->Length == 0)
        return TRUE; // reject null polygons
    MaxPoint_Y = MinPoint_Y = VertexPtr[MinIndexL = MaxIndex = 0].Y;
    for (i = 1; i < VertexList->Length; i++)
    {
        if (VertexPtr[i].Y < MinPoint_Y)
            MinPoint_Y = VertexPtr[MinIndexL = i].Y; // new top
        else if (VertexPtr[i].Y > MaxPoint_Y)
            MaxPoint_Y = VertexPtr[MaxIndex = i].Y; // new bottom
    }
    if (MinPoint_Y == MaxPoint_Y)
        return TRUE;    // polygon is 0-height; avoid infinite loop below

    // Scan in ascending order to find the last top-edge point
    MinIndexR = MinIndexL;
    while (VertexPtr[MinIndexR].Y == MinPoint_Y)
        INDEX_FORWARD(MinIndexR);
    INDEX_BACKWARD(MinIndexR); // back up to last top-edge point

    // Now scan in descending order to find the first top-edge point
    while (VertexPtr[MinIndexL].Y == MinPoint_Y)
        INDEX_BACKWARD(MinIndexL);
    INDEX_FORWARD(MinIndexL); // back up to first top-edge point

    // Figure out which direction through the vertex list from the top
    // vertex is the left edge and which is the right
    LeftEdgeDir = -1; // assume left edge runs down thru vertex list
    if ((TopIsFlat = (VertexPtr[MinIndexL].X !=
                      VertexPtr[MinIndexR].X)
                         ? 1
                         : 0) == 1)
    {
        // If the top is flat, just see which of the ends is leftmost
        if (VertexPtr[MinIndexL].X > VertexPtr[MinIndexR].X)
        {
            LeftEdgeDir = 1;       // left edge runs up through vertex list
            Temp = MinIndexL;      // swap the indices so MinIndexL
            MinIndexL = MinIndexR; // points to the start of the left
            MinIndexR = Temp;      // edge, similarly for MinIndexR
        }
    }
    else
    {
        // Point to the downward end of the first line of each of the
        //  two edges down from the top
        NextIndex = MinIndexR;
        INDEX_FORWARD(NextIndex);
        PreviousIndex = MinIndexL;
        INDEX_BACKWARD(PreviousIndex);
        // Calculate X and Y lengths from the top vertex to the end of
        // the first line down each edge; use those to compare slopes
        // and see which line is leftmost
        DeltaXN = VertexPtr[NextIndex].X - VertexPtr[MinIndexL].X;
        DeltaYN = VertexPtr[NextIndex].Y - VertexPtr[MinIndexL].Y;
        DeltaXP = VertexPtr[PreviousIndex].X - VertexPtr[MinIndexL].X;
        DeltaYP = VertexPtr[PreviousIndex].Y - VertexPtr[MinIndexL].Y;
        if (((long)DeltaXN * DeltaYP - (long)DeltaYN * DeltaXP) < 0L)
        {
            LeftEdgeDir = 1;       // left edge runs up through vertex list
            Temp = MinIndexL;      // swap the indices so MinIndexL
            MinIndexL = MinIndexR; // points to the start of the left
            MinIndexR = Temp;      // edge, similarly for MinIndexR
        }
    }

    // Set the # of scan lines in the polygon, skipping the bottom edge
    // and also skipping the top vertex if the top isn't flat because
    // in that case the top vertex has a right edge component, and set
    // the top scan line to draw, which is likewise the second line of
    // the polygon unless the top is flat
    if ((WorkingHLineList.Length =
             MaxPoint_Y - MinPoint_Y - 1 + TopIsFlat) <= 0)
        return TRUE;    // there's nothing to draw, so we're done
    WorkingHLineList.YStart = YOffset + MinPoint_Y + 1 - TopIsFlat;

    // Get memory in which to store the line list we generate
    if ((WorkingHLineList.HLinePtr =
             (struct HLine *)(malloc(sizeof(struct HLine) *
                                     WorkingHLineList.Length))) == NULL)
        return FALSE;   // couldn't get memory for the line list
    // Scan the left edge and store the boundary points in the list
    // Initial pointer for storing scan converted left-edge coords
    EdgePointPtr = WorkingHLineList.HLinePtr;
    // Start from the top of the left edge
    PreviousIndex = CurrentIndex = MinIndexL;
    // Skip the first point of the first line unless the top is flat;
    // if the top isn't flat, the top vertex is exactly on a right
    // edge and isn't drawn
    SkipFirst = TopIsFlat ? 0 : 1;
    // Scan convert each line in the left edge from top to bottom
    do
    {
        INDEX_MOVE(CurrentIndex, LeftEdgeDir);
        scan_edge(VertexPtr[PreviousIndex].X + XOffset,
                 VertexPtr[PreviousIndex].Y,
                 VertexPtr[CurrentIndex].X + XOffset,
                 VertexPtr[CurrentIndex].Y, 1, SkipFirst, &EdgePointPtr);
        PreviousIndex = CurrentIndex;
        SkipFirst = 0; // scan convert the first point from now on
    } while (CurrentIndex != MaxIndex);

    // Scan the right edge and store the boundary points in the list
    EdgePointPtr = WorkingHLineList.HLinePtr;
    PreviousIndex = CurrentIndex = MinIndexR;
    SkipFirst = TopIsFlat ? 0 : 1;
    // Scan convert the right edge, top to bottom. X coordinates are
    // adjusted 1 to the left, effectively causing scan conversion of
    // the nearest points to the left of but not exactly on the edge
    do
    {
        INDEX_MOVE(CurrentIndex, -LeftEdgeDir);
        scan_edge(VertexPtr[PreviousIndex].X + XOffset - 1,
                 VertexPtr[PreviousIndex].Y,
                 VertexPtr[CurrentIndex].X + XOffset - 1,
                 VertexPtr[CurrentIndex].Y, 0, SkipFirst, &EdgePointPtr);
        PreviousIndex = CurrentIndex;
        SkipFirst = 0; // scan convert the first point from now on
    } while (CurrentIndex != MaxIndex);

    // Draw the line list representing the scan converted polygon
    draw_hline_list(pdc, &WorkingHLineList);

    // Release the line list's memory and we're successfully done
    free(WorkingHLineList.HLinePtr);
    return TRUE;
}


/*

   Scan converts an edge from (X1,Y1) to (X2,Y2), not including the
   point at (X2,Y2). If SkipFirst == 1, the point at (X1,Y1) isn't
   drawn; if SkipFirst == 0, it is. For each scan line, the pixel
   closest to the scanned edge without being to the left of the
   scanned edge is chosen. 
   
   Uses an all-integer approach for speed and precision.

*/

void scan_edge(int X1, int Y1, int X2, int Y2, int SetXStart,
              int SkipFirst, struct HLine **EdgePointPtr)
{
    int Y, DeltaX, Height, Width, AdvanceAmt, ErrorTerm, i;
    int ErrorTermAdvance, XMajorAdvanceAmt;
    struct HLine *WorkingEdgePointPtr;

    WorkingEdgePointPtr = *EdgePointPtr; // avoid double dereference
    AdvanceAmt = ((DeltaX = X2 - X1) > 0) ? 1 : -1;
    // direction in which X moves (Y2 is
    // always > Y1, so Y always counts up)

    if ((Height = Y2 - Y1) <= 0) // Y length of the edge
        return;                  // guard against 0-length and horizontal edges

    // Figure out whether the edge is vertical, diagonal, X-major
    // (mostly horizontal), or Y-major (mostly vertical) and handle
    // appropriately
    if ((Width = abs(DeltaX)) == 0)
    {
        // The edge is vertical; special-case by just storing the same
        // X coordinate for every scan line
        // Scan the edge for each scan line in turn
        for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++)
        {
            // Store the X coordinate in the appropriate edge list
            if (SetXStart == 1)
                WorkingEdgePointPtr->XStart = X1;
            else
                WorkingEdgePointPtr->XEnd = X1;
        }
    }
    else if (Width == Height)
    {
        // The edge is diagonal; special-case by advancing the X
        // coordinate 1 pixel for each scan line
        if (SkipFirst)          // skip the first point if so indicated
            X1 += AdvanceAmt;   // move 1 pixel to the left or right
        // Scan the edge for each scan line in turn
        for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++)
        {
            // Store the X coordinate in the appropriate edge list
            if (SetXStart == 1)
                WorkingEdgePointPtr->XStart = X1;
            else
                WorkingEdgePointPtr->XEnd = X1;
            X1 += AdvanceAmt;   // move 1 pixel to the left or right
        }
    }
    else if (Height > Width)
    {
        // Edge is closer to vertical than horizontal (Y-major)
        if (DeltaX >= 0)
            ErrorTerm = 0;  // initial error term going left->right
        else
            ErrorTerm = -Height + 1;    // going right->left
        if (SkipFirst)
        { // skip the first point if so indicated
            // Determine whether it's time for the X coord to advance
            if ((ErrorTerm += Width) > 0)
            {
                X1 += AdvanceAmt;       // move 1 pixel to the left or right
                ErrorTerm -= Height;    // advance ErrorTerm to next point
            }
        }
        // Scan the edge for each scan line in turn
        for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++)
        {
            // Store the X coordinate in the appropriate edge list
            if (SetXStart == 1)
                WorkingEdgePointPtr->XStart = X1;
            else
                WorkingEdgePointPtr->XEnd = X1;
            // Determine whether it's time for the X coord to advance
            if ((ErrorTerm += Width) > 0)
            {
                X1 += AdvanceAmt;    // move 1 pixel to the left or right
                ErrorTerm -= Height; // advance ErrorTerm to correspond
            }
        }
    }
    else
    {
        // Edge is closer to horizontal than vertical (X-major)
        // Minimum distance to advance X each time
        XMajorAdvanceAmt = (Width / Height) * AdvanceAmt;
        // Error term advance for deciding when to advance X 1 extra
        ErrorTermAdvance = Width % Height;
        if (DeltaX >= 0)
            ErrorTerm = 0; // initial error term going left->right
        else
            ErrorTerm = -Height + 1;    // going right->left
        if (SkipFirst)
        {                               // skip the first point if so indicated
            X1 += XMajorAdvanceAmt;     // move X minimum distance
            // Determine whether it's time for X to advance one extra
            if ((ErrorTerm += ErrorTermAdvance) > 0)
            {
                X1 += AdvanceAmt;       // move X one more
                ErrorTerm -= Height;    // advance ErrorTerm to correspond
            }
        }
        // Scan the edge for each scan line in turn
        for (i = Height - SkipFirst; i-- > 0; WorkingEdgePointPtr++)
        {
            // Store the X coordinate in the appropriate edge list
            if (SetXStart == 1)
                WorkingEdgePointPtr->XStart = X1;
            else
                WorkingEdgePointPtr->XEnd = X1;
            X1 += XMajorAdvanceAmt; // move X minimum distance
            // Determine whether it's time for X to advance one extra
            if ((ErrorTerm += ErrorTermAdvance) > 0)
            {
                X1 += AdvanceAmt;    // move X one more
                ErrorTerm -= Height; // advance ErrorTerm to correspond
            }
        }
    }

    *EdgePointPtr = WorkingEdgePointPtr; // advance caller's ptr
}


/*

    Slower and less precise floating point version.

*/

void scan_edge2(int X1, int Y1, int X2, int Y2, int SetXStart,
               int SkipFirst, struct HLine **EdgePointPtr)
{
    int Y, DeltaX, DeltaY;
    double InverseSlope;
    struct HLine *WorkingEdgePointPtr;

    // Calculate X and Y lengths of the line and the inverse slope
    DeltaX = X2 - X1;
    if ((DeltaY = Y2 - Y1) <= 0)
        return; // guard against 0-length and horizontal edges
    InverseSlope = (double)DeltaX / (double)DeltaY;

    // Store the X coordinate of the pixel closest to but not to the
    // left of the line for each Y coordinate between Y1 and Y2, not
    // including Y2 and also not including Y1 if SkipFirst != 0
    WorkingEdgePointPtr = *EdgePointPtr;    // avoid double dereference
    for (Y = Y1 + SkipFirst; Y < Y2; Y++, WorkingEdgePointPtr++)
    {
        // Store the X coordinate in the appropriate edge list
        if (SetXStart == 1)
            WorkingEdgePointPtr->XStart =
                X1 + (int)(ceil((Y - Y1) * InverseSlope));
        else
            WorkingEdgePointPtr->XEnd =
                X1 + (int)(ceil((Y - Y1) * InverseSlope));
    }
    *EdgePointPtr = WorkingEdgePointPtr; // advance caller's ptr
}


/* 

   Draws all pixels in the list of horizontal lines passed in.
   Uses a slow pixel-by-pixel approach, which does have the virtue
   of being easily ported to any environment. 

*/

void draw_hline_list(PDC pdc, struct HLineList *HLineListPtr)
{
    struct HLine *HLinePtr;
    int Y, X;

    // Point to the XStart/XEnd descriptor for the first (top)
    // horizontal line
    HLinePtr = HLineListPtr->HLinePtr;
    // Draw each horizontal line in turn, starting with the top one and
    // advancing one line each time
    for (Y = HLineListPtr->YStart; Y < (HLineListPtr->YStart +
                                        HLineListPtr->Length);
         Y++, HLinePtr++)
    {
        // Draw each pixel in the current horizontal line in turn,
        // starting with the leftmost one
        draw_hline_seg(pdc, Y, HLinePtr->XStart, HLinePtr->XEnd);
    }
}


/*

   Draws all pixels in the horizontal line segment passed in, from
   (LeftX,Y) to (RightX,Y), using the appropriate span filler.
   Both LeftX and RightX are drawn. 
   No drawing will take place if LeftX > RightX.

*/

void draw_hline_seg(PDC pdc, int y, int leftX, int rightX)
{
    t_span span;
    span.img = pdc->frame;
    span.x1 = leftX;
    span.x2 = rightX;
    span.y1 = span.y2 = y;
    span.dx = 1;
    span.dy = 0;
    span.blend = pdc->blend;
    PFN_SPAN filler;
    if (pdc->tex)
    {
        span.tex = pdc->tex;
        if (span.blend) filler = DrawSpanTexBlend;
        else filler = DrawSpanTex;
        float u2, v2;
        TexelCoords(pdc, span.x1, span.y1, &span.u, &span.v);
        TexelCoords(pdc, span.x2, span.y2, &u2, &v2);
        int cx = span.x2 - span.x1;
        float one_over_cx = cx ? 1.0f / cx : 0;
        span.du = (u2 - span.u) * one_over_cx;
        span.dv = (v2 - span.v) * one_over_cx;
    }
    else
    {
        span.color = PixelFromColor(pdc->fgc);
        if (span.blend) filler = DrawSpanBlend;
        else filler = DrawSpanSolid;
    }
    filler(&span);
}


/* 

 Color-fills a convex polygon. All vertices are offset by (XOffset, YOffset).
 "Convex" means "monotone with respect to a vertical line"; that is, every 
 horizontal line drawn through the polygon at any point would cross exactly two 
 active edges (neither horizontal lines nor zero-length edges count as active 
 edges; both are acceptable anywhere in the polygon). Right & left edges may 
 cross (polygons may be nonsimple). Polygons that are not convex according to 
 this definition won't be drawn properly. (Yes, "convex" is a lousy name for 
 this type of polygon, but it's convenient; use "monotone-vertical" if it makes 
 you happier!)
 *******************************************************************
 NOTE: the low-level drawing routine, draw_hline_list, must be able to 
 reverse the edges, if necessary to make the correct edge left edge. It must 
 also expect right edge to be specified in +1 format (the X coordinate is 1 past
 highest coordinate to draw). In both respects, this differs from low-level 
 drawing routines presented in earlier columns; changes are necessary to make it
 possible to draw nonsimple monotone-vertical polygons; that in turn makes it 
 possible to use Jim Kent's test for monotone-vertical polygons.
 *******************************************************************
 Returns TRUE for success, FALSE if memory allocation failed.

*/

// Advances the index by one vertex forward through the vertex list,
// wrapping at the end of the list
#undef INDEX_FORWARD
#define INDEX_FORWARD(Index) \
    Index = (Index + 1) % VertexList->Length;

// Advances the index by one vertex backward through the vertex list,
// wrapping at the start of the list
#undef INDEX_BACKWARD
#define INDEX_BACKWARD(Index) \
    Index = (Index - 1 + VertexList->Length) % VertexList->Length;

// Advances the index by one vertex either forward or backward through
// the vertex list, wrapping at either end of the list
#undef INDEX_MOVE
#define INDEX_MOVE(Index, Direction)              \
    if (Direction > 0)                            \
        Index = (Index + 1) % VertexList->Length; \
    else                                          \
        Index = (Index - 1 + VertexList->Length) % VertexList->Length;

BOOL FillMonotoneVerticalPolygon(PDC pdc, struct PointListHeader *VertexList,
                                 int XOffset, int YOffset)
{
    int i, MinIndex, MaxIndex, MinPoint_Y, MaxPoint_Y;
    int NextIndex, CurrentIndex, PreviousIndex;
    struct HLineList WorkingHLineList;
    struct HLine *EdgePointPtr;
    struct Point *VertexPtr;

    // Point to the vertex list
    VertexPtr = VertexList->PointPtr;

    // Scan the list to find the top and bottom of the polygon
    if (VertexList->Length == 0)
        return TRUE;     // reject null polygons
    MaxPoint_Y = MinPoint_Y = VertexPtr[MinIndex = MaxIndex = 0].Y;
    for (i = 1; i < VertexList->Length; i++)
    {
        if (VertexPtr[i].Y < MinPoint_Y)
            MinPoint_Y = VertexPtr[MinIndex = i].Y;     // new top
        else if (VertexPtr[i].Y > MaxPoint_Y)
            MaxPoint_Y = VertexPtr[MaxIndex = i].Y;     // new bottom
    }

    // Set the # of scan lines in the polygon, skipping the bottom edge
    if ((WorkingHLineList.Length = MaxPoint_Y - MinPoint_Y) <= 0)
        return (1);     // there's nothing to draw
    WorkingHLineList.YStart = YOffset + MinPoint_Y;

    // Get memory in which to store the line list we generate
    if ((WorkingHLineList.HLinePtr =
             (struct HLine *)(malloc(sizeof(struct HLine) *
                                     WorkingHLineList.Length))) == NULL)
        return (0); // couldn't get memory for the line list

    // Scan the first edge and store the boundary points in the list
    // Initial pointer for storing scan converted first-edge coords
    EdgePointPtr = WorkingHLineList.HLinePtr;
    // Start from the top of the first edge
    PreviousIndex = CurrentIndex = MinIndex;
    // Scan convert each line in the first edge from top to bottom
    do
    {
        INDEX_BACKWARD(CurrentIndex);
        scan_edge(VertexPtr[PreviousIndex].X + XOffset,
                 VertexPtr[PreviousIndex].Y,
                 VertexPtr[CurrentIndex].X + XOffset,
                 VertexPtr[CurrentIndex].Y, 1, 0, &EdgePointPtr);
        PreviousIndex = CurrentIndex;
    } while (CurrentIndex != MaxIndex);

    // Scan the second edge and store the boundary points in the list
    EdgePointPtr = WorkingHLineList.HLinePtr;
    PreviousIndex = CurrentIndex = MinIndex;
    // Scan convert the second edge, top to bottom
    do
    {
        INDEX_FORWARD(CurrentIndex);
        scan_edge(VertexPtr[PreviousIndex].X + XOffset,
                 VertexPtr[PreviousIndex].Y,
                 VertexPtr[CurrentIndex].X + XOffset,
                 VertexPtr[CurrentIndex].Y, 0, 0, &EdgePointPtr);
        PreviousIndex = CurrentIndex;
    } while (CurrentIndex != MaxIndex);

    // Draw the line list representing the scan converted polygon
    draw_hline_list(pdc, &WorkingHLineList);

    // Release the line list's memory and we're successfully done
    free(WorkingHLineList.HLinePtr);
    
    // Success
    return (1);
}

/* 

 Returns 1 if polygon described by passed-in vertex list is monotone with
 respect to a vertical line, 0 otherwise. Doesn't matter if polygon is simple 
 (non-self-intersecting) or not.

*/

#define SIGNUM(a) ((a > 0) ? 1 : ((a < 0) ? -1 : 0))

BOOL PolygonIsMonotoneVertical(struct PointListHeader *VertexList)
{
    int i, Length, DeltaYSign, PreviousDeltaYSign;
    int NumYReversals = 0;
    struct Point *VertexPtr = VertexList->PointPtr;

    // Three or fewer points can't make a non-vertical-monotone polygon
    if ((Length = VertexList->Length) < 4)
        return TRUE;

    // Scan to the first non-horizontal edge
    PreviousDeltaYSign = SIGNUM(VertexPtr[Length - 1].Y - VertexPtr[0].Y);
    i = 0;
    while ((PreviousDeltaYSign == 0) && (i < (Length - 1)))
    {
        PreviousDeltaYSign = SIGNUM(VertexPtr[i].Y - VertexPtr[i + 1].Y);
        i++;
    }

    if (i == (Length - 1))
        return TRUE;     // polygon is a flat line

    // Now count Y reversals. Might miss one reversal, at the last vertex, but 
    // because reversal counts must be even, being off by one isn't a problem
    do
    {
        if ((DeltaYSign = SIGNUM(VertexPtr[i].Y - VertexPtr[i + 1].Y)) != 0)
        {
            if (DeltaYSign != PreviousDeltaYSign)
            {
                // Switched Y direction; not vertical-monotone if
                // reversed Y direction as many as three times
                if (++NumYReversals > 2)
                    return FALSE;
                PreviousDeltaYSign = DeltaYSign;
            }
        }
    } while (i++ < (Length - 1));
    return TRUE;    // it's a vertical-monotone polygon
}


/*

    Create/destroy dynamic point list

*/

BOOL CreatePointList(struct PointListHeader * ptr, int length)
{
    int cb = length * sizeof(struct Point);
    ptr->PointPtr = (struct Point *) malloc(cb);
    ptr->Length = 0;
    if (ptr->PointPtr)
    {
        ptr->Allocated = length;
        return TRUE;
    }
    else
    {
        ptr->Allocated = 0;
        return FALSE;
    }
}

// Destroy dynamic pointlist

void DestroyPointList(struct PointListHeader * ptr)
{
    if (ptr->PointPtr && ptr->Allocated) 
    {
        free(ptr->PointPtr);
        ptr->PointPtr = NULL;
    }
    else
    {
        ptr->PointPtr = NULL;
    }
    ptr->Length = 0;
    ptr->Allocated = 0;
}


/*

    Convert floating point list to integer point list

*/

void ConvertToPointList(PPOINT pin, struct Point* pout, int length)
{
    while (length > 0)
    {
        pout->X = pin->x;
        pout->Y = pin->y;
        pout++;
        pin++;
        length--;
    }
}

void ConvertFromPointList(struct Point* pin, PPOINT pout, int length)
{
    while (length > 0)
    {
        pout->x = pin->X;
        pout->y = pin->Y;
        pout++;
        pin++;
        length--;
    }
}


/*

    Clips a polygon to the DC's clipping rect.

    Te original point list will be destroyed and replaced
    by the resulting point list (dynamic), so the caller
    is responsible for later destruction.

    Returns FALSE for memory allocation failure
    or degenerate polygon.

*/

BOOL ClipPolygon(PDC pdc, struct PointListHeader* PointList)
{
    int count = PointList->Length;
    if (count < 3) 
    {
        DestroyPointList(PointList);
        return FALSE;
    }

    struct PointListHeader WorkingListA;
    if (!CreatePointList(&WorkingListA, PointList->Length * 2 + 2))
    {
        DestroyPointList(PointList);
        return FALSE;
    }
    WorkingListA.Length = 0;

    struct PointListHeader WorkingListB;
    if (!CreatePointList(&WorkingListB, PointList->Length * 2 + 2))
    {
        DestroyPointList(&WorkingListA);
        DestroyPointList(PointList);
        return FALSE;
    }
    WorkingListB.Length = 0;

    // Copy source into working list B
    for (int i=0; i<count; i++)
        WorkingListB.PointPtr[i] = PointList->PointPtr[i];
    // Wrap back to beginning;
    WorkingListB.PointPtr[count] = PointList->PointPtr[0];
    WorkingListB.Length = ++count;

    // Get rid of original point list
    DestroyPointList(PointList);

    // Set initial conditions for source/target lists
    struct PointListHeader* SourceList = &WorkingListB;
    struct PointListHeader* TargetList = &WorkingListA;

    int selector = 0;
    BOOL prevInside = FALSE;
    BOOL currentInside = FALSE;

    int cl = pdc->cl;
    int cr = pdc->cr;
    int ct = pdc->ct;
    int cb = pdc->cb;

    int x, y;

    // Clip left

    x = SourceList->PointPtr[0].X;
    prevInside = (x >= cl);

    if (prevInside)
    {
        y = SourceList->PointPtr[0].Y;
        TargetList->PointPtr[TargetList->Length].X = x;
        TargetList->PointPtr[TargetList->Length].Y = y;
        TargetList->Length++;
    }

    for (int i=1; i<SourceList->Length; i++)
    {
        x = SourceList->PointPtr[i].X;
        currentInside = (x >= cl);
        if (prevInside != currentInside)
        {
            int x1 = SourceList->PointPtr[i-1].X;
            int y1 = SourceList->PointPtr[i-1].Y;
            int x2 = SourceList->PointPtr[i].X;
            int y2 = SourceList->PointPtr[i].Y;
            int xo = cl;
            int yo = y1 + (float)((xo - x1) * (y2 - y1)) / (x2 - x1);
            TargetList->PointPtr[TargetList->Length].X = xo;
            TargetList->PointPtr[TargetList->Length].Y = yo;
            TargetList->Length++;
        }
        if (currentInside)
        {
            y = SourceList->PointPtr[i].Y;
            TargetList->PointPtr[TargetList->Length].X = x;
            TargetList->PointPtr[TargetList->Length].Y = y;
            TargetList->Length++;
        }
        prevInside = currentInside;
    }

    // Check for degenerate poly
    if (TargetList->Length < 3)
    {
        DestroyPointList(&WorkingListA);
        DestroyPointList(&WorkingListB);
        return FALSE;
    }

    // Swap source/target lists
    selector = 1 - selector;
    if (selector)
    {
        TargetList = &WorkingListB;
        SourceList = &WorkingListA;
    }
    else
    {
        TargetList = &WorkingListA;
        SourceList = &WorkingListB;
    }
    TargetList->Length = 0;

    // Clip right

    x = SourceList->PointPtr[0].X;
    prevInside = (x <= cr);

    if (prevInside)
    {
        TargetList->PointPtr[TargetList->Length].X = x;
        y = SourceList->PointPtr[0].Y;
        TargetList->PointPtr[TargetList->Length].Y = y;
        TargetList->Length++;
    }

    for (int i=1; i<SourceList->Length; i++)
    {
        x = SourceList->PointPtr[i].X;
        currentInside = (x <= cr);
        if (prevInside != currentInside)
        {
            int x1 = SourceList->PointPtr[i-1].X;
            int y1 = SourceList->PointPtr[i-1].Y;
            int x2 = SourceList->PointPtr[i].X;
            int y2 = SourceList->PointPtr[i].Y;
            int xo = cr;
            int yo = y1 + (float)((xo - x1) * (y2 - y1)) / (x2 - x1);
            TargetList->PointPtr[TargetList->Length].X = xo;
            TargetList->PointPtr[TargetList->Length].Y = yo;
            TargetList->Length++;
        }
        if (currentInside)
        {
            y = SourceList->PointPtr[i].Y;
            TargetList->PointPtr[TargetList->Length].X = x;
            TargetList->PointPtr[TargetList->Length].Y = y;
            TargetList->Length++;
        }
        prevInside = currentInside;
    }

    // Check for degenerate poly
    if (TargetList->Length < 3)
    {
        DestroyPointList(&WorkingListA);
        DestroyPointList(&WorkingListB);
        return FALSE;
    }

    // Swap source/target lists
    selector = 1 - selector;
    if (selector)
    {
        TargetList = &WorkingListB;
        SourceList = &WorkingListA;
    }
    else
    {
        TargetList = &WorkingListA;
        SourceList = &WorkingListB;
    }
    TargetList->Length = 0;

    // Clip top

    y = SourceList->PointPtr[0].Y;
    prevInside = (y >= ct);

    if (prevInside)
    {
        x = SourceList->PointPtr[0].X;
        TargetList->PointPtr[TargetList->Length].X = x;
        TargetList->PointPtr[TargetList->Length].Y = y;
        TargetList->Length++;
    }

    for (int i=1; i<SourceList->Length; i++)
    {
        y = SourceList->PointPtr[i].Y;
        currentInside = (y >= ct);
        if (prevInside != currentInside)
        {
            int x1 = SourceList->PointPtr[i-1].X;
            int y1 = SourceList->PointPtr[i-1].Y;
            int x2 = SourceList->PointPtr[i].X;
            int y2 = SourceList->PointPtr[i].Y;
            int yo = ct;
            int xo = x1 + (float)((yo - y1) * (x2 - x1)) / (y2 - y1);
            TargetList->PointPtr[TargetList->Length].X = xo;
            TargetList->PointPtr[TargetList->Length].Y = yo;
            TargetList->Length++;
        }
        if (currentInside)
        {
            x = SourceList->PointPtr[i].X;
            TargetList->PointPtr[TargetList->Length].X = x;
            TargetList->PointPtr[TargetList->Length].Y = y;
            TargetList->Length++;
        }
        prevInside = currentInside;
    }

    // Check for degenerate poly
    if (TargetList->Length < 3)
    {
        DestroyPointList(&WorkingListA);
        DestroyPointList(&WorkingListB);
        return FALSE;
    }

    // Swap source/target lists
    selector = 1 - selector;
    if (selector)
    {
        TargetList = &WorkingListB;
        SourceList = &WorkingListA;
    }
    else
    {
        TargetList = &WorkingListA;
        SourceList = &WorkingListB;
    }
    TargetList->Length = 0;

    // Clip bottom

    y = SourceList->PointPtr[0].Y;
    prevInside = (y <= cb);

    if (prevInside)
    {
        x = SourceList->PointPtr[0].X;
        TargetList->PointPtr[TargetList->Length].X = x;
        TargetList->PointPtr[TargetList->Length].Y = y;
        TargetList->Length++;
    }

    for (int i=1; i<SourceList->Length; i++)
    {
        y = SourceList->PointPtr[i].Y;
        currentInside = (y <= cb);
        if (prevInside != currentInside)
        {
            int x1 = SourceList->PointPtr[i-1].X;
            int y1 = SourceList->PointPtr[i-1].Y;
            int x2 = SourceList->PointPtr[i].X;
            int y2 = SourceList->PointPtr[i].Y;
            int yo = cb;
            int xo = x1 + (float)((yo - y1) * (x2 - x1)) / (y2 - y1);
            TargetList->PointPtr[TargetList->Length].X = xo;
            TargetList->PointPtr[TargetList->Length].Y = yo;
            TargetList->Length++;
        }
        if (currentInside)
        {
            x = SourceList->PointPtr[i].X;
            TargetList->PointPtr[TargetList->Length].X = x;
            TargetList->PointPtr[TargetList->Length].Y = y;
            TargetList->Length++;
        }
        prevInside = currentInside;
    }

    // Check for degenerate poly
    if (TargetList->Length < 3)
    {
        DestroyPointList(&WorkingListA);
        DestroyPointList(&WorkingListB);
        return FALSE;
    }

    // Destroy unneeded buffers
    DestroyPointList(SourceList);
    DestroyPointList(PointList);

    // Pass the final target list back through the original list
    PointList->Allocated = TargetList->Allocated;
    PointList->Length = TargetList->Length;
    PointList->PointPtr = TargetList->PointPtr;
    return TRUE;
}

