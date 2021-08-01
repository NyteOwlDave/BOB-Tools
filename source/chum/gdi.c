
/*

    gdi.c

    Graphics device interface (drawing)

*/

#include "inc/stdafx.h"
#include "inc/gdi.h"
#include "inc/polygon.h"


// Dotted line
static void line_dotted(PDC pdc, POINT a, POINT b);


//=============================================================
// MISC.
//=============================================================

static void TODO(LPCSTR msg)
{
    char sz[MAX_TOKEN+1];
    sprintf(sz, "TODO: %s\n", msg);
    Warning(sz);
}

void TexelCoords(PDC pdc, float x, float y, float *u, float *v)
{
    POINT pt;
    pt.x = x;
    pt.y = y;
    pt = ApplyMat3x2(&(pdc->tex_mtx), pt);
    *u = pt.x;
    *v = pt.y;
}

BOOL DottedColor(PDC pdc, PARGB pcolor)
{
    int i = pdc->dots.current++;
    i %= pdc->dots.length;
    if (i < 0) i += pdc->dots.length;
    int ch = pdc->dots.szPattern[i];
    switch (ch)
    {
    case 'F':
    case 'f':
    case '+':
        //fprintf(stderr, "F");
        *pcolor = PixelFromColor(pdc->fgc);
        return TRUE;
    case 'B':
    case 'b':
    case '-':
        //fprintf(stderr, "B");
        *pcolor = PixelFromColor(pdc->bgc);
        return TRUE;
    default:
        //fprintf(stderr, "_");
        return FALSE;
    }
}

ARGB PixelFromColor(RGBColor color)
{
    ARGB pixel;
    pixel.r = color.r;
    pixel.g = color.g;
    pixel.b = color.b;
    pixel.a = 0;
    return pixel;
}

float PolyNormalZ(POINT pa, POINT pb, POINT pc)
{
    float ax = pb.x - pa.x;
    float ay = pb.y - pa.y;
    float bx = pc.x - pb.x;
    float by = pc.y - pb.y;
    return ax * by - ay * bx;
}

void SetLinePattern(PDC pdc, LPCSTR sz)
{
    pdc->dots.length = 1;
    pdc->dots.current = 0;
    if (pdc->dots.szPattern)
    {
        free(pdc->dots.szPattern);
        pdc->dots.szPattern = NULL;
    }
    if (sz)
    {
        pdc->dots.szPattern = strdup(sz);
        pdc->dots.length = strlen(sz);
    }
}


//=============================================================
// Span filler
//=============================================================

// Forward/down only
void DrawSpanSolid(PSPAN span)
{
    //fprintf(stderr, "=> %i %i\n", (int)span->x1, (int)span->x2);
    if (span->dx == 1)
    {
        int x = span->x1;
        while (x <= span->x2)
        {
            span->img->line[span->y1][x] = span->color;
            x++;
        }
    }
    else if (span->dy == 1);
    {
        int y = span->y1;
        while (y <= span->y2)
        {
            span->img->line[y][span->x1] = span->color;
            y++;
        }
    }
}

// Forward/down only
void DrawSpanBlend(PSPAN span)
{
    float b1 = span->blend;
    b1 = MID(b1, 0, 1);
    float r = (float)span->color.r * b1;
    float g = (float)span->color.g * b1;
    float b = (float)span->color.b * b1;
    //fprintf(stderr, "\n%f\n", b1);
    //fprintf(stderr, "%i %i\n", span->x1, span->y1);
    //fprintf(stderr, "%i %i\n", span->x2, span->y2);
    //fprintf(stderr, "%i %i\n", span->dx, span->dy);
    if (span->dx == 1)
    {
        int x = span->x1;
        while (x <= span->x2)
        {
            ARGB pixel = span->img->line[span->y1][x];
            float fr = (pixel.r + r); 
            float fg = (pixel.g + g); 
            float fb = (pixel.b + b); 
            pixel.r = MID(fr, 0, 255);
            pixel.g = MID(fg, 0, 255);
            pixel.b = MID(fb, 0, 255);
            span->img->line[span->y1][x] = pixel;
            x++;
        }
    }
    else if (span->dy == 1);
    {
        int y = span->y1;
        while (y <= span->y2)
        {
            ARGB pixel = span->img->line[y][span->x1];
            float fr = (pixel.r + r); 
            float fg = (pixel.g + g); 
            float fb = (pixel.b + b); 
            pixel.r = MID(fr, 0, 255);
            pixel.g = MID(fg, 0, 255);
            pixel.b = MID(fb, 0, 255);
            span->img->line[y][span->x1] = pixel;
            y++;
        }
    }
}

// Forward/down only
void DrawSpanTex(PSPAN span)
{
    if (span->dx == 1)
    {
        PARGB line = span->img->line[span->y1];
        int x = span->x1;
        while (x <= span->x2)
        {
            int u = (int)span->u % span->tex->w;
            int v = (int)span->v % span->tex->h;
            if (u < 0) u += span->tex->w;
            if (v < 0) v += span->tex->h;
            ARGB color = span->tex->line[v][u];
            line[x] = color;
            span->u += span->du;
            span->v += span->dv;
            x++;
        }
        return;
    }
    else if (span->dy == 1)
    {
        int y = span->y1;
        while (y <= span->y2)
        {
            int u = (int)span->u % span->tex->w;
            int v = (int)span->v % span->tex->h;
            if (u < 0) u += span->tex->w;
            if (v < 0) v += span->tex->h;
            ARGB color = span->tex->line[v][u];
            span->img->line[y][span->x1] = color;
            span->u += span->du;
            span->v += span->dv;
            y++;
        }
        return;
    }
}

// Forward/down only
void DrawSpanTexBlend(PSPAN span)
{
    float b1 = span->blend;
    b1 = MID(b1, 0, 1);
    // fprintf(stderr, "%i %i\n", span->dx, span->dy);
    if (span->dx == 1)
    {
        int x = span->x1;
        while (x <= span->x2)
        {
            int u = (int)span->u % span->tex->w;
            int v = (int)span->v % span->tex->h;
            if (u < 0) u += span->tex->w;
            if (v < 0) v += span->tex->h;
            ARGB color1 = span->tex->line[v][u];
            ARGB color2 = span->img->line[span->y1][x];
            float fr = color1.r + color2.r * b1;
            float fg = color1.g + color2.g * b1;
            float fb = color1.b + color2.b * b1;
            color1.r = MID(fr, 0, 255);
            color1.g = MID(fg, 0, 255);
            color1.b = MID(fb, 0, 255);
            span->img->line[span->y1][x] = color1;
            span->u += span->du;
            span->v += span->dv;
            x += span->dx;
        }
        return;
    }
    else if (span->dy == 1)
    {
        int y = span->y1;
        while (y <= span->y2)
        {
            int u = (int)span->u % span->tex->w;
            int v = (int)span->v % span->tex->h;
            if (u < 0) u += span->tex->w;
            if (v < 0) v += span->tex->h;
            ARGB color1 = span->tex->line[v][u];
            ARGB color2 = span->img->line[y][span->x1];
            float fr = color1.r + color2.r * b1;
            float fg = color1.g + color2.g * b1;
            float fb = color1.b + color2.b * b1;
            color1.r = MID(fr, 0, 255);
            color1.g = MID(fg, 0, 255);
            color1.b = MID(fb, 0, 255);
            span->img->line[y][span->x1] = color1;
            span->u += span->du;
            span->v += span->dv;
            y += span->dy;
        }
        return;
    }
}


//=============================================================
// DC
//=============================================================

PDC CreateDC(int w, int h)
{
    //Log("Create DC");
    PDC pdc = (PDC)CreateBuffer(sizeof(DC), TRUE);
    pdc->fgc = MakeColor(255, 255, 255);
    //Log("Create Image");
    pdc->frame = CreateImage(w, h);
    InitIdentityMat3x2(&(pdc->tex_mtx));
    RECT rc;
    GetImageRect(pdc->frame, &rc);
    //Log("Clip DC");
    ClipDC(pdc, &rc);
    return pdc;
}

void DestroyDC(PDC pdc)
{
    if (pdc)
    {
        if (pdc->frame)
        {
            DestroyImage(pdc->frame);
        }
        if (pdc->tex)
        {
            DestroyImage(pdc->tex);
        }
        if (pdc->dots.szPattern)
        {
            free(pdc->dots.szPattern);
        }
        DestroyBuffer(pdc);
    }
}

void ClearDC(PDC pdc)
{
    ARGB color = PixelFromColor(pdc->bgc);
    // fprintf(stderr, "%i %i %i %i\n", pdc->cl, pdc->ct, pdc->cr, pdc->cb);
    // fprintf(stderr, "%i %i \n", (int)pdc->frame->w, (int)pdc->frame->h);
    for (int y=pdc->ct; y<=pdc->cb; y++)
    {
        // fprintf(stderr, "%i\n", y);
        PARGB line = pdc->frame->line[y];
        for (int x=pdc->cl; x<=pdc->cr; x++)
        {
            line[x] = color;
        }
    }
}

void ClipDC(PDC pdc, PRECT prc)
{
    if (prc)
    {
        RECT temp;
        //Log("Copy Rect");
        CopyRect(prc, &temp);
        //Log("Normalize Rect");
        NormalizeRect(&temp);
        RECT screen;
        screen.x = screen.y = 0;
        screen.w = pdc->frame->w;
        screen.h = pdc->frame->h;
        RECT clipped;
        //Log("Intersect Rect");
        IntersectRect(&temp, &screen, &clipped);
        //Log("Is Normal Rect");
        if (IsNormalRect(&clipped))
        {
            pdc->cl = clipped.x;
            pdc->ct = clipped.y;
            pdc->cr = clipped.x + clipped.w - 1;
            pdc->cb = clipped.y + clipped.h - 1;
            return;
        }
    }
    pdc->cl = pdc->ct = 0;
    pdc->cr = pdc->frame->w - 1;
    pdc->cb = pdc->frame->h - 1;
}

//=========================================,,====================
// MATRIX
//=============================================================

void InitIdentityMat3x2(PMAT3X2 mout)
{
    mout->m[0][0] = mout->m[1][1] = 1;
    mout->m[0][1] = mout->m[1][0] =
    mout->t[0] = mout->t[1] = 0;
}

void InitTranslateMat3x2(float tx, float ty, PMAT3X2 mout)
{
    mout->m[0][0] = mout->m[1][1] = 1;
    mout->m[0][1] = mout->m[1][0] = 0;
    mout->t[0] = tx; 
    mout->t[1] = ty;
}

void InitScaleMat3x2(float sx, float sy, PMAT3X2 mout)
{
    mout->m[0][0] = sx;
    mout->m[1][1] = sy;
    mout->m[0][1] = mout->m[1][0] =
    mout->t[0] = mout->t[1] = 0;
}

void InitRotateMat3x2(float r, PMAT3X2 mout)
{
    float c = cos(r);
    float s = sin(r);
    mout->m[0][0] = c;
    mout->m[0][1] = -s;
    mout->m[1][0] = s;
    mout->m[1][1] = c;
    mout->t[0] = mout->t[1] = 0;
}

void TranslateMat3x2(float tx, float ty, PMAT3X2 min, PMAT3X2 mout)
{
    POINT pa = { tx, ty };
    pa = ApplyMat3x2(min, pa);
    *mout = *min;
    mout->t[0] += pa.x;
    mout->t[1] += pa.y;
}

void ScaleMat3x2(float sx, float sy, PMAT3X2 min, PMAT3X2 mout)
{
    MAT3X2 tmp;
    InitScaleMat3x2(sx, sy, &tmp);
    CatMat3x2(&tmp, min, mout);
}

void RotateMat3x2(float r, PMAT3X2 min, PMAT3X2 mout)
{
    MAT3X2 tmp;
    InitRotateMat3x2(r, &tmp);
    CatMat3x2(&tmp, min, mout);
}

// Post multiply
void CatMat3x2(PMAT3X2 ma, PMAT3X2 mb, PMAT3X2 mout)
{
    MAT3X2 tmp;
    tmp.m[0][0] = ma->m[0][0] * mb->m[0][0]
                + ma->m[0][1] * mb->m[1][0];
    tmp.m[0][1] = ma->m[1][0] * mb->m[0][0]
                + ma->m[1][1] * mb->m[1][0];
    tmp.m[1][0] = ma->m[0][0] * mb->m[0][1]
                + ma->m[0][1] * mb->m[1][1];
    tmp.m[1][1] = ma->m[1][0] * mb->m[0][1]
                + ma->m[1][1] * mb->m[1][1];
    tmp.t[0] = ma->t[0] * mb->m[0][0] + ma->t[1] * mb->m[1][0] + mb->t[0];
    tmp.t[1] = ma->t[0] * mb->m[0][1] + ma->t[1] * mb->m[1][1] + mb->t[1];
    *mout = tmp;
}

POINT ApplyMat3x2(PMAT3X2 mtx, POINT pt)
{
    POINT out;
    out.x = pt.x * mtx->m[0][0] + pt.y * mtx->m[0][1] + mtx->t[0];
    out.y = pt.x * mtx->m[1][0] + pt.y * mtx->m[1][1] + mtx->t[1];
    return out;
}

void ApplyListMat3x2(PMAT3X2 mtx, PPOINT pin, PPOINT pout, int count)
{
    while (count > 0)
    {
        *pout = ApplyMat3x2(mtx, *pin);
        pout++;
        pin++;
        count--;
    }
}

void InitPatchMat3x2(POINT a, POINT b, POINT c, PMAT3X2 mout)
{
    TODO("InitPatchMat3x2");
    InitIdentityMat3x2(mout);
}


//=============================================================
// COORDS
//=============================================================

void InitRect(PRECT prc, float x, float y, float w, float h)
{
    prc->x = x;
    prc->y = y;
    prc->w = w;
    prc->h = h;
}

BOOL CopyRect(PRECT prc, PRECT pout)
{
    *pout = *prc;
}

BOOL IsNullRect(PRECT prc)
{
    return ((prc->w==0) || (prc->h==0));
}

BOOL IsNormalRect(PRECT prc)
{
    return ((prc->w>0) && (prc->h>0));
}

void NormalizeRect(PRECT prc)
{
    if (prc->w < 0)
    {
        prc->x += prc->w;
        prc->w = -prc->w;
    }
    if (prc->h < 0)
    {
        prc->y += prc->h;
        prc->h = -prc->h;
    }
}

void UnionRect(PRECT pa, PRECT pb, PRECT pout)
{
    float la = pa->x;
    float ra = la + pa->w;
    float ta = pa->y;
    float ba = ta + pa->h;
    float lb = pb->x;
    float rb = lb + pb->w;
    float tb = pb->y;
    float bb = tb + pb->h;
    pout->x = MIN(la, lb);
    pout->y = MIN(ta, tb);
    pout->w = MAX(ra, rb) - pout->x;
    pout->h = MAX(ba, bb) - pout->y;
}

void IntersectRect(PRECT pa, PRECT pb, PRECT pout)
{
    float la = pa->x;
    float ra = la + pa->w;
    float ta = pa->y;
    float ba = ta + pa->h;
    float lb = pb->x;
    float rb = lb + pb->w;
    float tb = pb->y;
    float bb = tb + pb->h;
    pout->x = MAX(la, lb);
    pout->y = MAX(ta, tb);
    pout->w = MIN(ra, rb) - pout->x;
    pout->h = MIN(ba, bb) - pout->y;
}

POINT GetRectStart(PRECT prc)
{
    POINT p;
    p.x = prc->x;
    p.y = prc->y;
    return p;
}

POINT GetRectEnd(PRECT prc)
{
    POINT p;
    p.x = prc->x + prc->w;
    p.y = prc->y + prc->h;
    return p;
}

SIZE GetRectSize(PRECT prc)
{
    SIZE s;
    s.w = prc->w;
    s.h = prc->h;
    return s;
}

void FlipRectHorz(PRECT prc)
{
    prc->x += prc->w;
    prc->w = -prc->w;
}

void FlipRectVert(PRECT prc)
{
    prc->y += prc->h;
    prc->h = -prc->h;
}

void FlipRect(PRECT prc)
{
    FlipRectHorz(prc);
    FlipRectVert(prc);
}

void InflateRect(PRECT prc, SIZE sz, PRECT pout)
{
    prc->x -= sz.w;
    prc->w += 2*sz.w;
    prc->y -= sz.h;
    prc->h += 2*sz.h;
}

void OffsetRect(PRECT prc, SIZE sz, PRECT pout)
{
    pout->w = prc->w;
    pout->h = prc->h;
    pout->x = prc->x + sz.w;
    pout->y = prc->y + sz.h;
}

void PreviewRect(SIZE parent, SIZE child, PRECT pout)
{
    float half_parent_w = parent.w / 2;
    float half_parent_h = parent.h / 2;
    float half_child_w = child.w / 2;
    float half_child_h = child.h / 2;
    float scale_x = parent.w / child.w;
    float scale_y = parent.h / child.h;
    if (scale_x >= scale_y)
    {
        half_child_w *= scale_y;
        half_child_h *= scale_y;
    }
    else
    {
        half_child_w *= scale_x;
        half_child_h *= scale_x;
    }
    pout->x = half_parent_w - half_child_w;
    pout->y = half_parent_h - half_child_h;
    pout->w = 2 * half_child_w;
    pout->h = 2 * half_child_h;
}

void GetImageRect(LPIMAGE img, PRECT pout)
{
    pout->x = pout->y = 0;
    pout->w = img->w;
    pout->h = img->h;
}

SIZE GetImageSize(LPIMAGE img)
{
    SIZE sz;
    sz.w = img->w;
    sz.h = img->h;
    return sz;
}


//=============================================================
// POINT
//=============================================================

// Draw single point (solid only)
void DrawPoint(PDC pdc, POINT pt)
{
    int x = pt.x;
    int y = pt.y;
    if (x < pdc->cl) return;
    if (y < pdc->ct) return;
    if (x > pdc->cr) return;
    if (y > pdc->cb) return;
    PARGB p = pdc->frame->line[y] + x;
    p->r = pdc->fgc.r;
    p->g = pdc->fgc.g;
    p->b = pdc->fgc.b;
    p->a = 0;
}

// Draw list of points (solid only)
void DrawPoints(PDC pdc, PPOINT ppt, int num_points)
{
    while (num_points > 0)
    {
        DrawPoint(pdc, ppt[--num_points]);
    }
}


//=============================================================
// LINE PRIMITIVE
//=============================================================

void DrawHLine(PDC pdc, POINT a, int count)
{
    if (!count) return;                 // Trivial rejection
    LPIMAGE img = pdc->frame;
    int dx = SGN(count);
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x = a.x;
    int y = a.y;
    int x2 = x + count - SGN(count);
    if (count < 0)
    {
        int tmp = x;
        x = x2;
        x2 = tmp;
    }
    if (y  < ct) return;            // Trivial clipping in y
    if (y  > cb) return;
    if (x2 < cl) return;            // Trivial clipping in x
    if (x  > cr) return;
    if (x < pdc->cl) x = pdc->cl;   // Clip left
    if (x2 > pdc->cr) x2 = pdc->cr; // Clip right
    // Final sanity check
    if (x2 < x) return;
    if (pdc->dots.szPattern)
    {
        a.x = x;
        a.y = y;
        POINT b;
        b.x = x2;
        b.y = a.y;
        //Log("Line dotted");
        line_dotted(pdc, a, b);
        return;
    }
    else if (pdc->blend)
    {
        t_span span;
        span.img = img;
        span.x1 = x;
        span.x2 = x2;
        span.y1 = span.y2 = y;
        span.dx = 1;
        span.dy = 0;
        span.color = PixelFromColor(pdc->fgc);
        span.blend = pdc->blend;
        //Log("Span blend");
        DrawSpanBlend(&span);
        return;
    }
    else
    {
        t_span span;
        span.img = img;
        span.x1 = x;
        span.x2 = x2;
        span.y1 = span.y2 = y;
        span.dx = 1;
        span.dy = 0;
        span.color = PixelFromColor(pdc->fgc);
        //Log("Span solid");
        DrawSpanSolid(&span);
        return;
    }
}

void DrawVLine(PDC pdc, POINT a, int count)
{
    if (!count) return;                     // Trivial rejection
    LPIMAGE img = pdc->frame;
    int dy = SGN(count);
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x = a.x;
    int y = a.y;
    int y2 = y + count - SGN(count);
    if (count < 0)
    {
        int tmp = y;
        y = y2;
        y2 = tmp;
    }
    if (x  < cl) return;            // Trivial clipping in x
    if (x  > cr) return;
    if (y2 < ct) return;            // Trivial clipping in y
    if (y  > cb) return;
    if (y < pdc->ct) y = pdc->ct;       // Clip top
    if (y2 > pdc->cb) y2 = pdc->cb;     // Clip bottom
    // Final sanity check
    if (y2 < y) return;
    if (pdc->dots.szPattern)
    {
        a.x = x;
        a.y = y;
        POINT b;
        b.x = a.x;
        b.y = y2;
        line_dotted(pdc, a, b);
        return;
    }
    else if (pdc->blend)
    {
        t_span span;
        span.img = img;
        span.x1 = span.x2 = x;
        span.y1 = y;
        span.y2 = y2;
        span.dx = 0;
        span.dy = 1;
        span.color = PixelFromColor(pdc->fgc);
        span.blend = pdc->blend;
        DrawSpanBlend(&span);
    }
    else
    {
        t_span span;
        span.img = img;
        span.x1 = span.x2 = x;
        span.y1 = y;
        span.y2 = y2;
        span.dx = 0;
        span.dy = 1;
        span.color = PixelFromColor(pdc->fgc);
        DrawSpanSolid(&span);
    }
}

// No clipping. Doesn't do horz/vert lines.
static void line_solid(PDC pdc, POINT a, POINT b)
{
    LPIMAGE img = pdc->frame;
    float cx = b.x - a.x;
    float cy = b.y - a.y;
    ARGB pixel = PixelFromColor(pdc->fgc);
    // Diagonal line
    if (ABS(cx) == ABS(cy))
    {
        //Log("Diag");
        int x1 = a.x;
        int y1 = a.y;
        int x2 = b.x;
        int y2 = b.y;
        int count = ABS(cx);
        int dx = SGN(cx);
        int dy = SGN(cy);
        while (count-- > 0)
        {
            img->line[y1][x1] = pixel;
            x1 += dx;
            y1 += dy;
        }
        return;
    }
    // Y-major line
    else if (ABS(cx) < ABS(cy))
    {
        //Log("Y-Major");
        float x1 = a.x;
        int y1 = a.y;
        float x2 = b.x;
        int y2 = b.y;
        int count = ABS(cy);
        float dx = SGN(cx) * ABS(cx / cy);
        int dy = SGN(cy);
        while (count-- > 0)
        {
            img->line[y1][(int)x1] = pixel;
            x1 += dx;
            y1 += dy;
        }
        return;
    }
    // X-major line
    else if (ABS(cx) > ABS(cy))
    {
        //Log("X-Major");
        int x1 = a.x;
        float y1 = a.y;
        int x2 = b.x;
        float y2 = b.y;
        int count = ABS(cx);
        int dx = SGN(cx);
        float dy = SGN(cy) * ABS(cy / cx);
        while (count-- > 0)
        {
            img->line[(int)y1][x1] = pixel;
            x1 += dx;
            y1 += dy;
        }
        return;
    }
    else
    {
        Warning("Internal logic error");
    }
}

// No clipping. Doesn't do horz/vert lines.
static void line_blend(PDC pdc, POINT pa, POINT pb)
{
    float b1 = pdc->blend;
    b1 = MID(b1, 0, 1);
    float r = (float)pdc->fgc.r * b1;
    float g = (float)pdc->fgc.g * b1;
    float b = (float)pdc->fgc.b * b1;
    LPIMAGE img = pdc->frame;
    float cx = pb.x - pa.x;
    float cy = pb.y - pa.y;
    // Diagonal line
    if (ABS(cx) == ABS(cy))
    {
        int x1 = pa.x;
        int y1 = pa.y;
        int x2 = pb.x;
        int y2 = pb.y;
        int count = ABS(cx);
        int dx = SGN(cx);
        int dy = SGN(cy);
        while (count-- > 0)
        {
            ARGB pixel = img->line[y1][x1];
            float fr = (pixel.r + r); 
            float fg = (pixel.g + g); 
            float fb = (pixel.b + b); 
            pixel.r = MID(fr, 0, 255);
            pixel.g = MID(fg, 0, 255);
            pixel.b = MID(fb, 0, 255);
            img->line[y1][x1] = pixel;
            x1 += dx;
            y1 += dy;
        }
        return;
    }
    // Y-major line
    else if (ABS(cx) < ABS(cy))
    {
        float x1 = pa.x;
        int y1 = pa.y;
        float x2 = pb.x;
        int y2 = pb.y;
        int count = ABS(cy);
        float dx = SGN(cx) * ABS(cx / cy);
        int dy = SGN(cy);
        while (count-- > 0)
        {
            int x = x1;
            ARGB pixel = img->line[y1][x];
            float fr = (pixel.r + r); 
            float fg = (pixel.g + g); 
            float fb = (pixel.b + b); 
            pixel.r = MID(fr, 0, 255);
            pixel.g = MID(fg, 0, 255);
            pixel.b = MID(fb, 0, 255);
            img->line[y1][x] = pixel;
            x1 += dx;
            y1 += dy;
        }
        return;
    }
    // X-major line
    else
    {
        int x1 = pa.x;
        float y1 = pa.y;
        int x2 = pb.x;
        float y2 = pb.y;
        int count = ABS(x2 - x1);
        int dx = SGN(cx);
        float dy = SGN(cy) * ABS(cy / cx);
        while (count-- > 0)
        {
            int y = y1;
            ARGB pixel = img->line[y][x1];
            float fr = (pixel.r + r); 
            float fg = (pixel.g + g); 
            float fb = (pixel.b + b); 
            pixel.r = MID(fr, 0, 255);
            pixel.g = MID(fg, 0, 255);
            pixel.b = MID(fb, 0, 255);
            img->line[y][x1] = pixel;
            x1 += dx;
            y1 += dy;
        }
        return;
    }
}

// No clipping. Any kind of line is okay.
static void line_dotted(PDC pdc, POINT a, POINT b)
{
    ARGB color;
    LPIMAGE img = pdc->frame;
    int x1 = a.x;
    int y1 = a.y;
    int x2 = b.x;
    int y2 = b.y;
    float cx = x2 - x1;
    float cy = y2 - y1;
//    x2 -= SGNZ(cx);
//    y2 -= SGNZ(cy);
    if (x1 == x2)
    {
        if (y1 == y2)
        {
            if (DottedColor(pdc, &color))
                img->line[y1][x1] = color;
            return;
        }
        if (y1 > y2)
        {
            while (y1 >= y2)
            {
                if (DottedColor(pdc, &color))
                    img->line[y1][x1] = color;
                y1--;
            }
            return;
        }
        else
        {
            while (y1 <= y2)
            {
                if (DottedColor(pdc, &color))
                    img->line[y1][x1] = color;
                y1++;
            }
            return;
        }
    }
    else if (y1 == y2)
    {
        if (x1 < x2)
        {
            while (x1 <= x2)
            {
                if (DottedColor(pdc, &color))
                    img->line[y1][x1] = color;
                x1++;
            }
            return;
        }
        else
        {
            while (x1 >= x2)
            {
                if (DottedColor(pdc, &color))
                    img->line[y1][x1] = color;
                x1--;
            }
            return;
        }
    }
    else
    {
        // X-Major
        if (ABS(cx) > ABS(cy))
        {
            int x1 = a.x;
            float y1 = a.y;
            int x2 = b.x;
            float y2 = b.y;
            int count = ABS(cx);
            int dx = SGN(cx);
            float dy = SGN(cy) * ABS(cy / cx);
            while (count-- > 0)
            {
                if (DottedColor(pdc, &color))
                    img->line[(int)y1][x1] = color;
                x1 += dx;
                y1 += dy;
            }
            return;
        }
        // Y-Major
        else if (ABS(cx) < ABS(cy))
        {
            float x1 = a.x;
            int y1 = a.y;
            float x2 = b.x;
            int y2 = b.y;
            int count = ABS(cy);
            float dx = SGN(cx) * ABS(cx / cy);
            int dy = SGN(cy);
            while (count-- > 0)
            {
                if (DottedColor(pdc, &color))
                    img->line[y1][(int)x1] = color;
                x1 += dx;
                y1 += dy;
            }
            return;
        }
        // Diagonal
        else
        {
            int x1 = a.x;
            int y1 = a.y;
            int x2 = b.x;
            int y2 = b.y;
            int count = ABS(cx);
            int dx = SGN(cx);
            int dy = SGN(cy);
            while (count-- > 0)
            {
                if (DottedColor(pdc, &color))
                    img->line[y1][x1] = color;
                x1 += dx;
                y1 += dy;
            }
            return;
        }
    }
}

// Draw any kind of line
void DrawLine(PDC pdc, POINT a, POINT b)
{
    LPIMAGE img = pdc->frame;
    int x1 = a.x;
    int y1 = a.y;
    int x2 = b.x;
    int y2 = b.y;
    int cx = x2 - x1;
    int cy = y2 - y1;
    // x2 -= SGNZ(cx);
    // y2 -= SGNZ(cy);
    if (x1 == x2)
    {
        // Log("VLine");
        DrawVLine(pdc, a, cy);
        return;
    }
    if (y1 == y2)
    {
        // Log("HLine");
        DrawHLine(pdc, a, cx);
        return;
    }
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    if (x1 < x2)
    {
        if (x2 < cl) return;
        if (x1 > cr) return;
    }
    else
    {
        if (x1 < cl) return;
        if (x2 > cr) return;
    }
    if (y1 < y2)
    {
        if (y2 < ct) return;
        if (y1 > cb) return;
    }
    else
    {
        if (y1 < ct) return;
        if (y2 > cb) return;
    }
    // Forward X clipping
    if (x1 < x2)
    {
        if (x1 < cl)
        {
            // fprintf(stderr, "\ncl1> %i %i %i %i\n", x1, y1, x2, y2);
            y1 = y1 + (float)((y2 - y1) * (cl - x1)) / (x2 - x1);
            if ((y1 < ct) && (y2 < ct)) return;
            if ((y1 > cb) && (y2 > cb)) return;
            x1 = cl;
            // fprintf(stderr, "cl1> %i %i %i %i\n", x1, y1, x2, y2);
        }
        if (x2 > cr)
        {
            //fprintf(stderr, "\ncr2> %i %i %i %i\n", x1, y1, x2, y2);
            y2 = y1 + (float)((y2 - y1) * (cr - x1)) / (x2 - x1);
            if ((y1 < ct) && (y2 < ct)) return;
            if ((y1 > cb) && (y2 > cb)) return;
            x2 = cr;
            //fprintf(stderr, "cr2> %i %i %i %i\n", x1, y1, x2, y2);
        } 
    }
    // Reverse X clipping
    else
    {
        if (x2 < cl)
        {
            //fprintf(stderr, "\ncl2> %i %i %i %i\n", x1, y1, x2, y2);
            y2 = y2 + (float)((y1 - y2) * (cl - x2)) / (x1 - x2);
            if ((y1 < ct) && (y2 < ct)) return;
            if ((y1 > cb) && (y2 > cb)) return;
            x2 = cl;
            //fprintf(stderr, "cl2> %i %i %i %i\n", x1, y1, x2, y2);
        }
        if (x1 > cr)
        {
            //fprintf(stderr, "\ncr1> %i %i %i %i\n", x1, y1, x2, y2);
            y1 = y2 + (float)((y1 - y2) * (cr - x2)) / (x1 - x2);
            if ((y1 < ct) && (y2 < ct)) return;
            if ((y1 > cb) && (y2 > cb)) return;
            x1 = cr;
            //fprintf(stderr, "cr1> %i %i %i %i\n", x1, y1, x2, y2);
        }
    }
    // Forward Y clipping
    if (y1 < y2)
    {
        if (y1 < ct)
        {
            //fprintf(stderr, "\nct1> %i %i %i %i\n", x1, y1, x2, y2);
            x1 = x1 + (float)((x2 - x1) * (ct - y1)) / (y2 - y1);
            if ((x1 < cl) && (x2 < cl)) return;
            if ((x1 > cr) && (x2 > cr)) return;
            y1 = ct;
            //fprintf(stderr, "ct1> %i %i %i %i\n", x1, y1, x2, y2);
        }
        if (y2 > cb)
        {
            //fprintf(stderr, "\ncb2> %i %i %i %i\n", x1, y1, x2, y2);
            x2 = x1 + (float)((x2 - x1) * (cb - y1)) / (y2 - y1);
            if ((x1 < cl) && (x2 < cl)) return;
            if ((x1 > cr) && (x2 > cr)) return;
            y2 = cb;
            //fprintf(stderr, "cb2> %i %i %i %i\n", x1, y1, x2, y2);
        }
    }
    // Reverse Y clipping
    else
    {
        if (y2 < ct)
        {
            //fprintf(stderr, "\nct2> %i %i %i %i\n", x1, y1, x2, y2);
            x2 = x2 + (float)((x1 - x2) * (ct - y2)) / (y1 - y2);
            if ((x1 < cl) && (x2 < cl)) return;
            if ((x1 > cr) && (x2 > cr)) return;
            y2 = ct;
            //fprintf(stderr, "ct2> %i %i %i %i\n", x1, y1, x2, y2);
        }
        if (y1 > cb)
        {
            //fprintf(stderr, "\ncb1> %i %i %i %i\n", x1, y1, x2, y2);
            x1 = x2 + (float)((x1 - x2) * (cb - y2)) / (y1 - y2);
            if ((x1 < cl) && (x2 < cl)) return;
            if ((x1 > cr) && (x2 > cr)) return;
            y1 = cb;
            //fprintf(stderr, "cb1> %i %i %i %i\n", x1, y1, x2, y2);
        }
    }
    a.x = x1;
    a.y = y1;
    b.x = x2;
    b.y = y2;
    if (pdc->dots.szPattern)
    {
        line_dotted(pdc, a, b);
    }
    else if (pdc->blend)
    {
        line_blend(pdc, a, b);
    }
    else
    {
        // fprintf(stderr, "=> %f %f %f %f\n", a.x, a.y, b.x, b.y);
        line_solid(pdc, a, b);
    }
}

// Draw poly lines (any kind)
// Does not close final edge (see: DrawPoly for that)
void DrawPolyLine(PDC pdc, PPOINT pts, int num_lines)
{
    if (num_lines < 1) return;
    int i = 0;
    while (num_lines--)
    {
        int j = i + 1;
        DrawLine(pdc, pts[i], pts[j]);
        i++;
    }
}

// Draw line list (any kind)
void DrawLineList(PDC pdc, PPOINT pts, int num_lines)
{
    if (num_lines < 1) return;
    int i = 0;
    while (num_lines--)
    {
        int j = i + 1;
        DrawLine(pdc, pts[i], pts[j]);
        i += 2;
    }
}


//=============================================================
// CIRCLE PRIMITIVE
//=============================================================

// Draw any kind of circle outline
void DrawCircle(PDC pdc, POINT center, float radius)
{
    if (radius < 1) return;             // Trivial rejection
    LPIMAGE img = pdc->frame;
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x1 = center.x - radius;
    int y1 = center.y - radius;
    int x2 = center.x + radius;;
    int y2 = center.y + radius;;
    if (x2 < cl) return;   // Trivial rejection
    if (x1 > cr) return;
    if (y2 < ct) return;   // Trivial rejection
    if (y1 > cb) return;
    // Dotted
    if (pdc->dots.szPattern)
    {
        if (x1 < cl)  x1 = cl;
        if (x2 > cr)  x2 = cr;
        if (y1 < ct)  y1 = ct;
        if (y2 > cb)  y2 = cb;
        ARGB color;
        int rlt = radius * radius + radius;
        int rgt = radius * radius - radius;
        int xc = center.x;
        int yc = center.y;
        for (int y=y1; y<=y2; y++)
        {
            int yy = y - yc;
            yy *= yy;
            for (int x=x1; x<=x2; x++)
            {
                int xx = x - xc;
                xx *= xx;
                if (((yy + xx) < rlt) && ((yy + xx) > rgt))
                {
                    float theta = atan2(y - yc, x - xc);
                    if (theta < 1) theta = PI - theta;
                    int arclen = radius * theta;
                    pdc->dots.current = arclen;
                    if (DottedColor(pdc, &color));
                        img->line[y][x] = color;
                }
            }
        }
        return;
    }
    // Blended
    else if (pdc->blend)
    {
        if (x1 < cl)  x1 = cl;
        if (x2 > cr)  x2 = cr;
        if (y1 < ct)  y1 = ct;
        if (y2 > cb)  y2 = cb;
        ARGB color = PixelFromColor(pdc->fgc);
        float blend = pdc->blend;
        blend = MID(blend, 0, 1);
        float fr = color.r * blend;
        float fg = color.g * blend;
        float fb = color.b * blend;
        int rlt = radius * radius + radius;
        int rgt = radius * radius - radius;
        int xc = center.x;
        int yc = center.y;
        for (int y=y1; y<=y2; y++)
        {
            int yy = y - yc;
            yy *= yy;
            for (int x=x1; x<=x2; x++)
            {
                int xx = x - xc;
                xx *= xx;
                if (((yy + xx) < rlt) && ((yy + xx) > rgt))
                {
                    ARGB pixel = img->line[y][x];
                    float r = pixel.r + fr;
                    float g = pixel.g + fg;
                    float b = pixel.b + fb;
                    pixel.r = MID(r, 0, 255);
                    pixel.g = MID(r, 0, 255);
                    pixel.b = MID(r, 0, 255);
                    img->line[y][x] = pixel;
                }
            }
        }
        return;
    }
    // Solid
    else
    {
        if (x1 < cl)  x1 = cl;
        if (x2 > cr)  x2 = cr;
        if (y1 < ct)  y1 = ct;
        if (y2 > cb)  y2 = cb;
        ARGB color = PixelFromColor(pdc->fgc);
        int rlt = radius * radius + radius;
        int rgt = radius * radius - radius;
        int xc = center.x;
        int yc = center.y;
        for (int y=y1; y<=y2; y++)
        {
            int yy = y - yc;
            yy *= yy;
            for (int x=x1; x<=x2; x++)
            {
                int xx = x - xc;
                xx *= xx;
                if (((yy + xx) < rlt) && ((yy + xx) > rgt))
                {
                    img->line[y][x] = color;
                }
            }
        }
        return;
    }
}

// Solid
static void fill_circle_solid(PDC pdc, POINT center, float radius)
{
    LPIMAGE img = pdc->frame;
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x1 = center.x - radius;
    int y1 = center.y - radius;
    int x2 = center.x + radius;
    int y2 = center.y + radius;
    if (x2 < cl) return;   // Trivial rejection
    if (x1 > cr) return;
    if (y2 < ct) return;   // Trivial rejection
    if (y1 > cb) return;
    if (x1 < cl)  x1 = cl;
    if (x2 > cr)  x2 = cr;
    if (y1 < ct)  y1 = ct;
    if (y2 > cb)  y2 = cb;
    ARGB color = PixelFromColor(pdc->fgc);
    int rlt = radius * radius + radius;
    int rgt = radius * radius - radius;
    int xc = center.x;
    int yc = center.y;
    for (int y=y1; y<=y2; y++)
    {
        int yy = y - yc;
        yy *= yy;
        for (int x=x1; x<=x2; x++)
        {
            int xx = x - xc;
            xx *= xx;
            if ((yy + xx) < rlt)
            {
                img->line[y][x] = color;
            }
        }
    }
}

// Solid blend
static void fill_circle_blend(PDC pdc, POINT center, float radius)
{
    LPIMAGE img = pdc->frame;
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x1 = center.x - radius;
    int y1 = center.y - radius;
    int x2 = center.x + radius;
    int y2 = center.y + radius;
    if (x2 < cl) return;   // Trivial rejection
    if (x1 > cr) return;
    if (y2 < ct) return;   // Trivial rejection
    if (y1 > cb) return;
    if (x1 < cl)  x1 = cl;
    if (x2 > cr)  x2 = cr;
    if (y1 < ct)  y1 = ct;
    if (y2 > cb)  y2 = cb;
    int rlt = radius * radius + radius;
    int rgt = radius * radius - radius;
    int xc = center.x;
    int yc = center.y;
    t_span span;
    span.img = img;
    span.blend = pdc->blend;
    span.color = PixelFromColor(pdc->fgc);
    span.dx = 1;
    span.dy = 0;
    for (int y=y1; y<=y2; y++)
    {
        span.y1 = span.y2 = y;
        int yy = y - yc;
        yy *= yy;
        for (int x=x1; x<=x2; x++)
        {
            int xx = x - xc;
            xx *= xx;
            if ((yy + xx) < rlt)
            {
                int i = x;
                for (int j=x+1; j <= x2; j++)
                {
                    int jj = j - xc;
                    jj *= jj;
                    if ((yy + jj) < rlt) i = j;
                    else break;
                }
                span.x1 = x;
                span.x2 = i;
                DrawSpanBlend(&span);
                x = x2;
            }
        }
    }
}

// Tex or tex blend
static void fill_circle_tex(PDC pdc, POINT center, float radius)
{
    LPIMAGE img = pdc->frame;
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x1 = center.x - radius;
    int y1 = center.y - radius;
    int x2 = center.x + radius;
    int y2 = center.y + radius;
    if (x2 < cl) return;   // Trivial rejection
    if (x1 > cr) return;
    if (y2 < ct) return;   // Trivial rejection
    if (y1 > cb) return;
    if (x1 < cl)  x1 = cl;
    if (x2 > cr)  x2 = cr;
    if (y1 < ct)  y1 = ct;
    if (y2 > cb)  y2 = cb;
    int rlt = radius * radius + radius;
    int rgt = radius * radius - radius;
    int xc = center.x;
    int yc = center.y;
    t_span span;
    span.img = img;
    span.tex = pdc->tex;
    span.blend = pdc->blend;
    PFN_SPAN span_filler;
    if (span.blend)
        span_filler = DrawSpanTexBlend;
    else
        span_filler = DrawSpanTex;
    for (int y=y1; y<=y2; y++)
    {
        span.y1 = span.y2 = y;
        int yy = y - yc;
        yy *= yy;
        for (int x=x1; x<=x2; x++)
        {
            int xx = x - xc;
            xx *= xx;
            if ((yy + xx) < rlt)
            {
                int i = x;
                for (int j=x+1; j <= x2; j++)
                {
                    int jj = j - xc;
                    jj *= jj;
                    if ((yy + jj) < rlt) i = j;
                    else break;
                }
                span.x1 = x;
                span.x2 = i;
                TexelCoords(pdc, span.x1, span.y1, &span.u, &span.v);
                float u2, v2;
                TexelCoords(pdc, span.x2, span.y2, &u2, &v2);
                int cx = span.x2 - span.x1;
                float one_over_cx = cx ? 1.0f / cx : 0;
                span.du = (u2 - span.u) * one_over_cx;
                span.dv = (v2 - span.v) * one_over_cx;
                span.dx = 1;
                span.dy = 0;
                span_filler(&span);
                x = x2;
            }
        }
    }
}

// Fill any kind of circle
void FillCircle(PDC pdc, POINT center, float radius)
{
    if (pdc->tex)
    {
        fill_circle_tex(pdc, center, radius);
    }
    else
    {
        if (pdc->blend) fill_circle_blend(pdc, center, radius);
        else fill_circle_solid(pdc, center, radius);
    }
}


//=============================================================
// ELLIPSE PRIMITIVE
//=============================================================

// Single solid pixel
static void draw_pixel(PDC pdc, int x, int y)
{
    POINT pt;
    pt.x = x;
    pt.y = y;
    DrawPoint(pdc, pt);
}

// Solid only
static void draw_ellipse_solid(PDC pdc, POINT center, float rx, float ry)
{

    int x = center.x;
    int y = center.y;
    int workingX, workingY;
    double aSquared = (double)rx * rx;
    double bSquared = (double)ry * ry;
    double temp;

    // Draw the four symmetric arcs for which X advances faster (that is,
    //  for which X is the major axis)
    // Draw the initial top & bottom points
    draw_pixel(pdc, x, y + ry);
    draw_pixel(pdc, x, y - ry);

    // Draw the four arcs
    for (workingX = 0;;)
    {
        // Advance one pixel along the X axis
        workingX++;

        // Calculate the corresponding point along the Y axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = bSquared - (bSquared *
                           (double)workingX * (double)workingX / aSquared);
        if (temp >= 0)
        {
            workingY = sqrt(temp) + 0.5;
        }
        else
        {
            workingY = 0;
        }

        // Stop if X is no longer the major axis (the arc has passed the
        // 45-degree point)
        if (((double)workingY / bSquared) <= ((double)workingX / aSquared))
            break;

        // Draw the 4 symmetries of the current point
        draw_pixel(pdc, x + workingX, y - workingY);
        draw_pixel(pdc, x - workingX, y - workingY);
        draw_pixel(pdc, x + workingX, y + workingY);
        draw_pixel(pdc, x - workingX, y + workingY);
    }

    // Draw the four symmetric arcs for which Y advances faster (that is,
    //  for which Y is the major axis)
    // Draw the initial left & right points
    draw_pixel(pdc, x + rx, y);
    draw_pixel(pdc, x - rx, y);

    // Draw the four arcs
    for (workingY = 0;;)
    {
        // Advance one pixel along the Y axis
        workingY++;

        // Calculate the corresponding point along the X axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = aSquared - (aSquared *
                           (double)workingY * (double)workingY / bSquared);
        if (temp >= 0)
        {
            workingX = sqrt(temp) + 0.5;
        }
        else
        {
            workingX = 0;   // floating-point roundoff
        }

        // Stop if Y is no longer the major axis (the arc has passed the
        // 45-degree point)
        if (((double)workingX / aSquared) < ((double)workingY / bSquared))
            break;

        // Draw the 4 symmetries of the current point
        draw_pixel(pdc, x + workingX, y - workingY);
        draw_pixel(pdc, x - workingX, y - workingY);
        draw_pixel(pdc, x + workingX, y + workingY);
        draw_pixel(pdc, x - workingX, y + workingY);
    }

}

// Single blended pixel
static void blend_pixel(PDC pdc, int x, int y)
{
    PIMAGE img = pdc->frame;
    if (x < pdc->cl) return;
    if (x > pdc->cr) return;
    if (y < pdc->ct) return;
    if (y > pdc->cb) return;
    ARGB color1 = PixelFromColor(pdc->fgc);
    ARGB color2 = img->line[y][x];
    float fr = color2.r + color1.r * pdc->blend;
    float fg = color2.g + color1.g * pdc->blend;
    float fb = color2.b + color1.b * pdc->blend;
    color2.r = MID(fr, 0, 255);
    color2.g = MID(fg, 0, 255);
    color2.b = MID(fb, 0, 255);
    img->line[y][x] = color2;
}

// Solid blend only
static void draw_ellipse_blend(PDC pdc, POINT center, float rx, float ry)
{
    PIMAGE img = pdc->frame;
    pdc->blend = MID(pdc->blend, 0, 1);

    int x = center.x;
    int y = center.y;
    int workingX, workingY;
    double aSquared = (double)rx * rx;
    double bSquared = (double)ry * ry;
    double temp;

    // Draw the four symmetric arcs for which X advances faster (that is,
    //  for which X is the major axis)
    // Draw the initial top & bottom points
    blend_pixel(pdc, x, y + ry);
    blend_pixel(pdc, x, y - ry);

    // Draw the four arcs
    for (workingX = 0;;)
    {
        // Advance one pixel along the X axis
        workingX++;

        // Calculate the corresponding point along the Y axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = bSquared - (bSquared *
                           (double)workingX * (double)workingX / aSquared);
        if (temp >= 0)
        {
            workingY = sqrt(temp) + 0.5;
        }
        else
        {
            workingY = 0;
        }

        // Stop if X is no longer the major axis (the arc has passed the
        // 45-degree point)
        if (((double)workingY / bSquared) <= ((double)workingX / aSquared))
            break;

        // Draw the 4 symmetries of the current point
        blend_pixel(pdc, x + workingX, y - workingY);
        blend_pixel(pdc, x - workingX, y - workingY);
        blend_pixel(pdc, x + workingX, y + workingY);
        blend_pixel(pdc, x - workingX, y + workingY);
    }

    // Draw the four symmetric arcs for which Y advances faster (that is,
    //  for which Y is the major axis)
    // Draw the initial left & right points
    blend_pixel(pdc, x + rx, y);
    blend_pixel(pdc, x - rx, y);

    // Draw the four arcs
    for (workingY = 0;;)
    {
        // Advance one pixel along the Y axis
        workingY++;

        // Calculate the corresponding point along the X axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = aSquared - (aSquared *
                           (double)workingY * (double)workingY / bSquared);
        if (temp >= 0)
        {
            workingX = sqrt(temp) + 0.5;
        }
        else
        {
            workingX = 0;   // floating-point roundoff
        }

        // Stop if Y is no longer the major axis (the arc has passed the
        // 45-degree point)
        if (((double)workingX / aSquared) < ((double)workingY / bSquared))
            break;

        // Draw the 4 symmetries of the current point
        blend_pixel(pdc, x + workingX, y - workingY);
        blend_pixel(pdc, x - workingX, y - workingY);
        blend_pixel(pdc, x + workingX, y + workingY);
        blend_pixel(pdc, x - workingX, y + workingY);
    }
    
}

// Single dotted pixel
static void draw_dotted(PDC pdc, int x, int y, int count)
{
    pdc->dots.current = count;
    ARGB color;
    if (x < pdc->cl) return;
    if (x > pdc->cr) return;
    if (y < pdc->ct) return;
    if (y > pdc->cb) return;
    if (DottedColor(pdc, &color))
    {
        pdc->frame->line[y][x] = color;
    }
}

// Dotted only
static void draw_ellipse_dots(PDC pdc, POINT center, float rx, float ry)
{
    int x = center.x;
    int y = center.y;
    int workingX, workingY;
    double aSquared = (double)rx * rx;
    double bSquared = (double)ry * ry;
    double temp;

    // Draw the four symmetric arcs for which X advances faster (that is,
    //  for which X is the major axis)
    // Draw the initial top & bottom points
    draw_dotted(pdc, x, y + ry, 0);
    draw_dotted(pdc, x, y - ry, 0);

    // Draw the four arcs
    for (workingX = 0;;)
    {
        // Advance one pixel along the X axis
        workingX++;

        // Calculate the corresponding point along the Y axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = bSquared - (bSquared *
                           (double)workingX * (double)workingX / aSquared);
        if (temp >= 0)
        {
            workingY = sqrt(temp) + 0.5;
        }
        else
        {
            workingY = 0;
        }

        // Stop if X is no longer the major axis (the arc has passed the
        // 45-degree point)
        if (((double)workingY / bSquared) <= ((double)workingX / aSquared))
            break;

        // Draw the 4 symmetries of the current point
        draw_dotted(pdc, x + workingX, y - workingY, workingX);
        draw_dotted(pdc, x - workingX, y - workingY, workingX);
        draw_dotted(pdc, x + workingX, y + workingY, workingX);
        draw_dotted(pdc, x - workingX, y + workingY, workingX);
    }

    // Draw the four symmetric arcs for which Y advances faster (that is,
    //  for which Y is the major axis)
    // Draw the initial left & right points
    draw_dotted(pdc, x + rx, y, 0);
    draw_dotted(pdc, x - rx, y, 0);

    // Draw the four arcs
    for (workingY = 0;;)
    {
        // Advance one pixel along the Y axis
        workingY++;

        // Calculate the corresponding point along the X axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = aSquared - (aSquared *
                           (double)workingY * (double)workingY / bSquared);
        if (temp >= 0)
        {
            workingX = sqrt(temp) + 0.5;
        }
        else
        {
            workingX = 0;   // floating-point roundoff
        }

        // Stop if Y is no longer the major axis (the arc has passed the
        // 45-degree point)
        if (((double)workingX / aSquared) < ((double)workingY / bSquared))
            break;

        // Draw the 4 symmetries of the current point
        draw_dotted(pdc, x + workingX, y - workingY, workingY);
        draw_dotted(pdc, x - workingX, y - workingY, workingY);
        draw_dotted(pdc, x + workingX, y + workingY, workingY);
        draw_dotted(pdc, x - workingX, y + workingY, workingY);
    }    
}

// Draw any kind of ellipse outline
void DrawEllipse(PDC pdc, POINT center, float rx, float ry)
{
    if ((rx < 1) || (ry < 1)) return;
    if (pdc->dots.szPattern)
    {
        draw_ellipse_dots(pdc, center, rx, ry);
        return;
    }
    else if (pdc->blend)
    {
        draw_ellipse_blend(pdc, center, rx, ry);
        return;
    }
    else
    {
        draw_ellipse_solid(pdc, center, rx, ry);
        return;
    }
}

// Solid or solid-blend
static void span_solid_clipped(PDC pdc, PSPAN span, PFN_SPAN filler)
{

    if (span->y1 < pdc->ct) return;
    if (span->y1 > pdc->cb) return;
    if (span->x1 > span->x2)
    {
        int temp = span->x1;
        span->x1 = span->x2;
        span->x2 = temp;
    }
    if (span->x1 < pdc->cl) span->x1 = pdc->cl;
    if (span->x2 > pdc->cr) span->x2 = pdc->cr;

    span->dx = 1;
    span->dy = 0;

    filler(span);
}

// Handles solid or solid-blend
static void fill_ellipse_solid(PDC pdc, POINT center, float rx, float ry)
{
    int x = center.x;
    int y = center.y;
    int workingX, workingY;
    double aSquared = (double)rx * rx;
    double bSquared = (double)ry * ry;
    double temp;

    t_span span;
    PFN_SPAN filler;
    if (pdc->blend) filler = DrawSpanBlend;
    else filler = DrawSpanSolid;
    span.blend = pdc->blend;
    span.color = PixelFromColor(pdc->fgc);
    span.img = pdc->frame;

    // Draw the four symmetric arcs for which Y advances faster (that is,
    //  for which Y is the major axis)
    // Draw the initial left & right points
    span.x1 = x - rx;
    span.x2 = x + rx;
    span.y1 = y;
    span.y2 = span.y1;
    span_solid_clipped(pdc, &span, filler);

    // Draw the four arcs
    for (workingY = 1; workingY <= ry; workingY++)
    {
        // Calculate the corresponding point along the X axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = aSquared - (aSquared *
                           (double)workingY * (double)workingY / bSquared);
        if (temp >= 0)
        {
            workingX = sqrt(temp) + 0.5;
        }
        else
        {
            workingX = 0;   // floating-point roundoff
        }
        span.x1 = x - workingX;
        span.x2 = x + workingX;
        span.y1 = y - workingY;
        span.y2 = span.y1;
        span_solid_clipped(pdc, &span, filler);
        span.x1 = x - workingX;
        span.x2 = x + workingX;
        span.y1 = y + workingY;
        span.y2 = span.y1;
        span_solid_clipped(pdc, &span, filler);
    }

}

// Tex or tex-blend
static void span_tex_clipped(PDC pdc, PSPAN span, PFN_SPAN filler)
{
    if (span->y1 < pdc->ct) return;
    if (span->y1 > pdc->cb) return;
    if (span->x1 > span->x2)
    {
        int temp = span->x1;
        span->x1 = span->x2;
        span->x2 = temp;
    }
    if (span->x1 < pdc->cl) span->x1 = pdc->cl;
    if (span->x2 > pdc->cr) span->x2 = pdc->cr;

    TexelCoords(pdc, span->x1, span->y1, &span->u, &span->v);
    float u2, v2;
    TexelCoords(pdc, span->x2, span->y2, &u2, &v2);
    int cx = span->x2 - span->x1;
    float one_over_cx = cx ? 1.0f / cx : 0;
    span->du = (u2 - span->u) * one_over_cx;
    span->dv = (v2 - span->v) * one_over_cx;
    span->dx = 1;
    span->dy = 0;

    filler(span);

}

// Handles tex or tex-blend
static void fill_ellipse_tex(PDC pdc, POINT center, float rx, float ry)
{

    int x = center.x;
    int y = center.y;
    int workingX, workingY;
    double aSquared = (double)rx * rx;
    double bSquared = (double)ry * ry;
    double temp;

    t_span span;
    PFN_SPAN filler;
    if (pdc->blend) filler = DrawSpanTexBlend;
    else filler = DrawSpanTex;
    span.tex = pdc->tex;
    span.blend = pdc->blend;
    span.img = pdc->frame;
    span.dx = 1;
    span.dy = 0;

    // Draw the four symmetric arcs for which Y advances faster (that is,
    //  for which Y is the major axis)
    // Draw the initial left & right points
    span.x1 = x - rx;
    span.x2 = x + rx;
    span.y1 = y;
    span.y2 = span.y1;
    span_tex_clipped(pdc, &span, filler);

    // Draw the four arcs
    for (workingY = 1; workingY <= ry; workingY++)
    {
        // Calculate the corresponding point along the X axis. Guard
        // against floating-point roundoff making the intermediate term
        // less than 0
        temp = aSquared - (aSquared *
                           (double)workingY * (double)workingY / bSquared);
        if (temp >= 0)
        {
            workingX = sqrt(temp) + 0.5;
        }
        else
        {
            workingX = 0;   // floating-point roundoff
        }
        span.x1 = x - workingX;
        span.x2 = x + workingX;
        span.y1 = y - workingY;
        span.y2 = span.y1;
        span_tex_clipped(pdc, &span, filler);
        span.x1 = x - workingX;
        span.x2 = x + workingX;
        span.y1 = y + workingY;
        span.y2 = span.y1;
        span_tex_clipped(pdc, &span, filler);
    }

}

// Fill any kind of ellipse
void FillEllipse(PDC pdc, POINT center, float rx, float ry)
{
    if ((rx < 1) || (ry < 1)) return;
    if (pdc->tex)
    {
        // Handles tex or tex-blend
        fill_ellipse_tex(pdc, center, rx, ry);
    }
    else
    {
        // Handles solid or solid-blend
        fill_ellipse_solid(pdc, center, rx, ry);
    }
}


//=============================================================
// PATCH PRIMITIVE
//=============================================================

void DrawPatch(PDC pdc, PPOINT pts)
{
    DrawPoly(pdc, pts, 3);
}

void FillPatch(PDC pdc, PPOINT pts)
{
    FillPoly(pdc, pts, 3);
}

//=============================================================
// POLY PRIMITIVE
//=============================================================

void DrawPoly(PDC pdc, PPOINT pts, int count)
{
    if (count < 3) return;  // Trivial rejection
    DrawPolyLine(pdc, pts, count-1);
    POINT a = pts[count-1];
    POINT b = pts[0];
    DrawLine(pdc, a, b);
}

void FillPoly(PDC pdc, PPOINT pts, int count)
{
    if (count < 3) return;
    int shape = (count == 3) ? CONVEX : COMPLEX;
    struct PointListHeader PointList = {0, 0, NULL};
    if (!CreatePointList(&PointList, count))
        return;
    ConvertToPointList(pts, PointList.PointPtr, count);
    PointList.Length = count;
    if (ClipPolygon(pdc, &PointList))
    {
        FillPolygon(pdc, &PointList, shape, 0, 0);
    }
    DestroyPointList(&PointList);
}


//=============================================================
// RECT PRIMITIVE
//=============================================================

void DrawRect(PDC pdc, PRECT prc)
{
    // Trivial clipping
    if ((prc->w == 0) || (prc->h == 0)) return;
    POINT a, b, c, d;
    a.x = prc->x;
    a.y = prc->y;
    b.x = prc->x + prc->w;
    //b.x -= SGN(prc->w);
    b.y = a.y;
    c.x = b.x;
    c.y = a.y + prc->h;
    //c.y -= SGN(prc->h);
    d.x = a.x;
    d.y = c.y;
    int cx = (b.x - a.x);
    int cy = (b.y - a.y);
    if (cx == 1)
    {
        DrawVLine(pdc, a, cy);
        return;
    }
    else if (cy == 1)
    {
        DrawHLine(pdc, a, cx);
        return;
    }
    DrawLine(pdc, a, b);
    DrawLine(pdc, b, c);
    DrawLine(pdc, c, d);
    DrawLine(pdc, d, a);
}

// Color solid
static void rect_solid(PDC pdc, int x1, int y1, int x2, int y2)
{
    t_span span;
    span.img = pdc->frame;
    span.tex = NULL;
    if (x1 > x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    span.x1 = x1;
    span.x2 = x2;
    span.dx = 1;
    span.dy = 0;
    span.blend = 0;
    span.color = PixelFromColor(pdc->fgc);
    span.du = 0;
    span.du = 0;
    for (int y=y1; y<=y2; y++)
    {
        span.y1 = span.y2 = y;
        DrawSpanSolid(&span);
    }
}

// Color blended
static void rect_blend(PDC pdc, int x1, int y1, int x2, int y2)
{
    t_span span;
    span.img = pdc->frame;
    span.tex = NULL;
    if (x1 > x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    span.x1 = x1;
    span.x2 = x2;
    span.dx = 1;
    span.dy = 0;
    span.blend = pdc->blend;
    span.color = PixelFromColor(pdc->fgc);
    span.du = 0;
    span.du = 0;
    for (int y=y1; y<=y2; y++)
    {
        span.y1 = span.y2 = y;
        DrawSpanBlend(&span);
    }
}

// Textured blended
static void rect_tex_blend(PDC pdc, int x1, int y1, int x2, int y2)
{
    t_span span;
    span.img = pdc->frame;
    span.tex = pdc->tex;
    if (x1 > x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    int cx = x2 - x1;
    int cy = y2 - y1;
    span.x1 = x1;
    span.x2 = x2;
    span.dx = 1;
    span.dy = 0;
    span.blend = pdc->blend;
    span.color = MakePixel(0,0,0);
    for (int y=y1; y<=y2; y++)
    {
        TexelCoords(pdc, x1, y, &span.u, &span.v);
        float u2, v2;
        TexelCoords(pdc, x2, y, &u2, &v2);
        if (cx)
        {
            span.du = (u2 - span.u) / cx;
            span.dv = (v2 - span.v) / cx;
        }
        else
        {
            span.du = 0;
            span.dv = 0;
        }
        span.y1 = span.y2 = y;
        DrawSpanTexBlend(&span);
    }
}

// Textured solid
static void rect_tex_solid(PDC pdc, int x1, int y1, int x2, int y2)
{
    t_span span;
    span.img = pdc->frame;
    span.tex = pdc->tex;
    if (x1 > x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    int cx = x2 - x1;
    int cy = y2 - y1;
    span.x1 = x1;
    span.x2 = x2;
    span.dx = 1;
    span.dy = 0;
    span.blend = 0;
    span.color = MakePixel(0,0,0);
    float one_over_cx = cx ? 1.0f / cx : 0;
    for (int y=y1; y<=y2; y++)
    {
        TexelCoords(pdc, x1, y, &span.u, &span.v);
        float u2, v2;
        TexelCoords(pdc, x2, y, &u2, &v2);
        span.du = (u2 - span.u) * one_over_cx;
        span.dv = (v2 - span.v) * one_over_cx;
        span.y1 = span.y2 = y;
        DrawSpanTex(&span);
    }
}

void FillRect(PDC pdc, PRECT prc)
{
    // Trivial clipping
    if ((prc->w == 0) || (prc->h == 0)) return;
    LPIMAGE img = pdc->frame;
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x1 = prc->x;
    int x2 = x1 + prc->w - SGNZ(prc->w);
    int y1 = prc->y;
    int y2 = y1 + prc->h - SGNZ(prc->h);
    // Clipping in X
    if (prc->w < 0)
    {
        if (x1 < cl) return;    // Fully clipped
        if (x2 > cr) return;    // Fully clipped
        if (x2 < cl) x2 = cl;
        if (x1 > cr) x1 = cr;
    }
    else
    {
        if (x2 < cl) return;    // Fully clipped
        if (x1 > cr) return;    // Fully clipped
        if (x2 > cr) x2 = cr;
        if (x1 < cl) x1 = cl;
    }
    // Clipping in Y
    if (prc->h < 0)
    {
        if (y1 < ct) return;    // Fully clipped
        if (y2 > cb) return;    // Fully clipped
        if (y2 < ct) y2 = ct;
        if (y1 > cb) y1 = cb;
    }
    else
    {
        if (y2 < ct) return;    // Fully clipped
        if (y1 > cb) return;    // Fully clipped
        if (y2 > cb) y2 = cb;
        if (y1 < ct) y1 = ct;
    }
    if (pdc->tex)
    {
        if (pdc->blend)
        {
            // Log("Rect tex blend");
            rect_tex_blend(pdc, x1, y1, x2, y2);
        }
        else
        {
            //Log("Rect tex solid");
            rect_tex_solid(pdc, x1, y1, x2, y2);
        }
    }
    else
    {
        if (pdc->blend)
        {
            //Log("Rect blend");
            rect_blend(pdc, x1, y1, x2, y2);
        }
        else
        {
            rect_solid(pdc, x1, y1, x2, y2);
        }
    }
}


//=============================================================
// BLITTING
//=============================================================

// No clipping
static void blit_image_solid(PDC pdc, LPIMAGE src, int x1, int y1, int x2, int y2, int tx, int ty)
{
    LPIMAGE dst = pdc->frame;
    for (int j=y1, tj=ty; j <= y2; j++, tj++)
    {
        for (int i=x1, ti=tx; i <= x2; i++, ti++)
        {
            dst->line[j][i] = src->line[tj][ti];
        }
    }
}

// No clipping
static void blit_image_blend(PDC pdc, LPIMAGE src, int x1, int y1, int x2, int y2, int tx, int ty)
{
    LPIMAGE dst = pdc->frame;
    float blend = MID(pdc->blend, 0, 1);
    int ti, tj;
    for (int j=y1, tj=ty; j <= y2; j++, tj++)
    {
        for (int i=x1, ti=tx; i <= x2; i++, ti++)
        {
            ARGB pixel = src->line[tj][ti];
            ARGB color = dst->line[j][i];
            float fr = (color.r + pixel.r * blend); 
            float fg = (color.g + pixel.g * blend); 
            float fb = (color.b + pixel.b * blend); 
            pixel.r = MID(fr, 0, 255);
            pixel.g = MID(fg, 0, 255);
            pixel.b = MID(fb, 0, 255);
            dst->line[j][i] = pixel;
        }
    }
}

void BlitImage(PDC pdc, LPIMAGE src, POINT pt)
{
    LPIMAGE dst = pdc->frame;
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x = pt.x;
    int y = pt.y;
    int x2 = x + src->w - 1;
    int y2 = y + src->h - 1;
    int tx = 0;
    int ty = 0;
    if ((x > cr) || (x2 < cl)) return;      // Trivial clip
    if ((y > cb) || (y2 < ct)) return;      // Trivial clip
    // Clip left
    if (x < cl)
    {
        tx += cl - x;
        x = cl;
    }
    // Clip right
    if (x2 > cr)
    {
        x2 = cr;
    }
    // Clip top
    if (y < ct)
    {
        ty += ct - y;
        y = ct;
    }
    // Clip bottom
    if (y2 > cb)
    {
        y2 = cb;
    }
    if (pdc->blend)
    {
        blit_image_blend(pdc, src, x, y, x2, y2, tx, ty);
    }
    else
    {
        blit_image_solid(pdc, src, x, y, x2, y2, tx, ty);
    }
}

// No clipping
static void stretch_image_solid(PDC pdc, LPIMAGE src, 
int x1, int y1, int x2, int y2, 
int tx, int ty, float tsx, float tsy)
{
    LPIMAGE dst = pdc->frame;
    float tj=ty;
    for (int j=y1; j <= y2; j++, tj+=tsy)
    {
        float ti=tx;
        for (int i=x1; i <= x2; i++, ti+=tsx)
        {
            dst->line[j][i] = src->line[(int)tj][(int)ti];
        }
    }
}

// No clipping
static void stretch_image_blend(PDC pdc, LPIMAGE src, 
int x1, int y1, int x2, int y2, 
int tx, int ty, float tsx, float tsy)
{
    LPIMAGE dst = pdc->frame;
    float b1 = pdc->blend;
    b1 = MID(b1, 0, 1);
    float tj = ty;
    for (int j=y1; j <= y2; j++, tj+=tsy)
    {
        float ti = tx;
        for (int i=x1; i <= x2; i++, ti+=tsx)
        {
            ARGB color1 = src->line[(int)tj][(int)ti];
            ARGB color2 = dst->line[j][i];
            float fr = color1.r + color2.r * b1;
            float fg = color1.g + color2.g * b1;
            float fb = color1.b + color2.b * b1;
            color1.r = MID(fr, 0, 255);
            color1.g = MID(fg, 0, 255);
            color1.b = MID(fb, 0, 255);
            dst->line[j][i] = color1;
        }
    }
}

void StretchImage(PDC pdc, LPIMAGE src, POINT pt, SIZE sz)
{
    LPIMAGE dst = pdc->frame;
    int ct = pdc->ct;
    int cl = pdc->cl;
    int cb = pdc->cb;
    int cr = pdc->cr;
    int x = pt.x;
    int y = pt.y;
    int x2 = x + sz.w - 1;
    int y2 = y + sz.h - 1;
    int tx = 0;
    int ty = 0;
    if ((x > cr) || (x2 < cl)) return;      // Trivial clip
    if ((y > cb) || (y2 < ct)) return;      // Trivial clip
    // Texture scaling
    float tsx = src->w / (float)sz.w;
    float tsy = src->h / (float)sz.h;
    // Clip left
    if (x < cl)
    {
        tx += (cl - x) * tsx;
        x = cl;
    }
    // Clip right
    if (x2 > cr)
    {
        x2 = cr;
    }
    // Clip top
    if (y < ct)
    {
        ty += (ct - y) * tsy;
        y = ct;
    }
    // Clip bottom
    if (y2 > cb)
    {
        y2 = cb;
    }
    if (pdc->blend)
    {
        stretch_image_blend(pdc, src, x, y, x2, y2, tx, ty, tsx, tsy);
    }
    else
    {
        stretch_image_solid(pdc, src, x, y, x2, y2, tx, ty, tsx, tsy);
    }
}

