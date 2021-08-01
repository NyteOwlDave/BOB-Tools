
/*

    drawtest2.c

    Tests for the gdi 2d drawing system

*/

#include <time.h>
#include "inc/stdafx.h"
#include "inc/drawtest.h"
#include "inc/bob.h"
#include "inc/polygon.h"

/*
*/

t_dispatch m_disp[] = {
    { W_POINT, "point", test_DrawPoints },
    { W_HLINE, "hline", test_DrawHLine },
    { W_VLINE, "vline", test_DrawVLine },
    { W_LINE, "line", test_DrawLine },
    { W_POLYLINE, "polyline", test_DrawPolyLine },
    { W_LINELIST, "linelist", test_DrawLineList },
    { W_RECT, "rect", test_DrawRect },
    { W_CIRCLE, "circle", test_DrawCircle },
    { W_ELLIPSE, "ellipse", test_DrawEllipse },
    { W_PATCH, "tri", test_DrawPatch },
    { W_POLY, "poly", test_DrawPoly },
    { W_BLIT, "blit", test_BlitImage },
    { W_STRETCH, "stretch", test_StretchImage },
    { W_EXTRA, "extra", test_Extra },
    { W_ERROR, NULL, NULL }
};


//=============================================================
// Helpers
//=============================================================

static PDC simple_dc()
{
    PDC pdc = CreateDC(500, 500);
    RECT rc;
    //Log("Get Image Rect");
    if (pdc->frame == NULL)
    {
        errno = ENOIMG;
        LastCall("Missing frame buffer");
    }
    GetImageRect(pdc->frame, &rc);
    //Log("Make Color");
    pdc->bgc = MakeColor(128, 128, 128);
    //Log("Clear DC");
    ClearDC(pdc);
    SIZE sz;
    sz.w = -50;
    sz.h = -50;
    //Log("Inflate DC");
    InflateRect(&rc, sz, &rc);
    ClipDC(pdc, &rc);
    pdc->bgc = MakeColor(0, 0, 0);
    ClearDC(pdc);
    return pdc;
}

static PDC textured_dc()
{
    PDC pdc = CreateDC(500, 500);
    RECT rc;
    GetImageRect(pdc->frame, &rc);
    pdc->bgc = MakeColor(128, 128, 128);
    //Log("Clear DC");
    ClearDC(pdc);
    SIZE sz;
    sz.w = -50;
    sz.h = -50;
    InflateRect(&rc, sz, &rc);
    ClipDC(pdc, &rc);
    pdc->bgc = MakeColor(0, 0, 0);
    pdc->tex = LoadImageFileBob("dt-tex.img");
    RotateMat3x2(0.4f, &pdc->tex_mtx, &pdc->tex_mtx);
    // Log("Fill Rect");
    FillRect(pdc, &rc);
    DestroyImage(pdc->tex);
    pdc->tex = NULL;
    InitIdentityMat3x2(&pdc->tex_mtx);
    return pdc;
}


//=============================================================
// Points
//=============================================================

static void points_1(void)
{
    PDC pdc = simple_dc();
    float radius = 50;
    int count = radius * radius * 0.5f;
    int cb = count * sizeof(POINT);
    PPOINT pa = (PPOINT)CreateBuffer(cb, FALSE);
    PPOINT pb = (PPOINT)CreateBuffer(cb, FALSE);
    for (int n=0; n<count; n++)
    {
        float h = RND() * radius;
        float t = RND() * PI * 0.5f;
        float dx = (RND() < 0.5f) ? -1 : 1;
        float dy = (RND() < 0.5f) ? -1 : 1;
        pa[n].x = h * dx * cos(t);
        pa[n].y = h * dy * sin(t);
    }
    float ox = pdc->frame->w / 2;
    float oy = pdc->frame->h / 2;
    for (int y=-1; y<=1; y++)
    {
        for (int x=-1; x<=1; x++)
        {
            int ax = ox + x * ox * 0.8;
            int ay = oy + y * oy * 0.8;
            for (int n=0; n<count; n++)
            {
                pb[n].x = pa[n].x + ax;
                pb[n].y = pa[n].y + ay;
            }
            DrawPoints(pdc, pb, count);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
    DestroyBuffer(pa);
    DestroyBuffer(pb);
}

static void points_2(void)
{
    PDC pdc = simple_dc();
    for (int i=0; i<10000; i++)
    {
        POINT pt;
        pt.x = RND() * pdc->frame->w;
        pt.y = RND() * pdc->frame->h;
        DrawPoint(pdc, pt);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawPoints( PFLAGS pflags )
{
    if (pflags->filled)
        points_2();
    else
        points_1();
    exit(0);
}


//=============================================================
// Horizontal Lines
//=============================================================

static void hline_solid(void)
{
    PDC pdc = textured_dc();
    int band = 10;
    int skip = 7;
    int ymax = pdc->frame->h - band;
    POINT pt;
    pt.x = band;
    int count = pdc->frame->w - 2 * band;
    for (int y=band; y<ymax; y+=skip)
    {
        pt.y = y;
        DrawHLine(pdc, pt, count);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void hline_blend(void)
{
    PDC pdc = textured_dc();
    pdc->blend = .99f;
    int band = 10;
    int skip = 7;
    int ymax = pdc->frame->h - band;
    POINT pt;
    pt.x = band;
    int count = pdc->frame->w - 2 * band;
    for (int y=band; y<ymax; y+=skip)
    {
        pt.y = y;
        DrawHLine(pdc, pt, count);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void hline_dot(void)
{
    PDC pdc = textured_dc();
    SetLinePattern(pdc, "FB___FFBB_B");
    int band = 10;
    int skip = 7;
    int ymax = pdc->frame->h - band;
    POINT pt;
    pt.x = band;
    int count = pdc->frame->w - 2 * band;
    for (int y=band; y<ymax; y+=skip)
    {
        pt.y = y;
        DrawHLine(pdc, pt, count);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawHLine( PFLAGS pflags )
{
    if (pflags->blended) hline_blend();
    else if (pflags->dotted) hline_dot();
    else hline_solid();
    exit(0);
}


//=============================================================
// Vertical Lines
//=============================================================

static void vline_solid(void)
{
    PDC pdc = textured_dc();
    int band = 10;
    int skip = 7;
    int xmax = pdc->frame->w - band;
    POINT pt;
    pt.y = band;
    int count = pdc->frame->w - 2 * band;
    for (int x=band; x<xmax; x+=skip)
    {
        pt.x = x;
        DrawVLine(pdc, pt, count);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void vline_blend(void)
{
    PDC pdc = textured_dc();
    pdc->blend = .99f;
    int band = 10;
    int skip = 7;
    int xmax = pdc->frame->w - band;
    POINT pt;
    pt.y = band;
    int count = pdc->frame->h - 2 * band;
    for (int x=band; x<xmax; x+=skip)
    {
        pt.x = x;
        DrawVLine(pdc, pt, count);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void vline_dot(void)
{
    PDC pdc = textured_dc();
    SetLinePattern(pdc, "FB___FFBB_B");
    int band = 10;
    int skip = 7;
    int xmax = pdc->frame->w - band;
    POINT pt;
    pt.y = band;
    int count = pdc->frame->h - 2 * band;
    for (int x=band; x<xmax; x+=skip)
    {
        pt.x = x;
        DrawVLine(pdc, pt, count);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawVLine( PFLAGS pflags )
{
    if (pflags->blended) vline_blend();
    else if (pflags->dotted) vline_dot();
    else vline_solid();
    exit(0);
}


//=============================================================
// Arbitrary Lines
//=============================================================

static void line_blend(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    pdc->blend = 0.1f;
    POINT pa, pb;
    srand(time(NULL));
    for (int i=0; i<10000; i += 7)
    {
        pa.x = RND() * img->w;
        pa.y = RND() * img->h;
        pb.x = RND() * img->w;
        pb.y = RND() * img->h;
        DrawLine(pdc, pa, pb);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void line_solid1(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    POINT pa, pb;
    srand(time(NULL));
    int sw = 0;
    for (int y=0; y<img->h; y += 7)
    {
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.y = y;
        pb.x = 0;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.y = y;
        pb.x = img->w;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        sw++;
    }
    sw = 0;
    for (int x=0; x<img->w; x += 7)
    {
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.x = x;
        pb.y = 0;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.x = x;
        pb.y = img->h;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        sw++;
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void line_solid2(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    POINT pa, pb;
    srand(time(NULL));
    for (int i=0; i<img->h; i += 7)
    {
        pa.x = i;
        pa.y = 0;
        pb.x = img->h - i;
        pb.y = img->h;
        DrawLine(pdc, pa, pb);
        pa.y = i;
        pa.x = 0;
        pb.y = img->h - i;
        pb.x = img->h;
        DrawLine(pdc, pa, pb);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void line_solid3(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    POINT pa, pb;
    srand(time(NULL));
    for (int i=0; i<20000; i += 7)
    {
        pa.x = RND() * img->w;
        pa.y = RND() * img->h;
        pb.x = RND() * img->w;
        pb.y = RND() * img->h;
        DrawLine(pdc, pa, pb);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void line_dot1(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    POINT pa, pb;
    srand(time(NULL));
    for (int i=0; i<100; i += 7)
    {
        pa.x = RND() * img->w;
        pa.y = RND() * img->h;
        pb.x = RND() * img->w;
        pb.y = RND() * img->h;
        pdc->fgc = MakeColor(200,200,10);
        SetLinePattern(pdc, NULL);
        DrawLine(pdc, pa, pb);
        pdc->fgc = MakeColor(255,255,255);
        SetLinePattern(pdc, "FFBB___");
        DrawLine(pdc, pa, pb);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void line_dot2(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    SetLinePattern(pdc, "FFBB___");
    int band = 50;
    POINT pa, pb;
    srand(time(NULL));
    int sw = 0;
    for (int y=0; y<img->h; y += 7)
    {
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.y = y;
        pb.x = 0;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.y = y;
        pb.x = img->w;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        sw++;
    }
    sw = 0;
    for (int x=0; x<img->w; x += 7)
    {
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.x = x;
        pb.y = 0;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        pa.x = img->w / 2;
        pa.y = img->h / 2;
        pb.x = x;
        pb.y = img->h;
        if (sw & 1) DrawLine(pdc, pb, pa);
        else DrawLine(pdc, pa, pb);
        sw++;
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawLine( PFLAGS pflags )
{
    if (pflags->blended) line_blend();
    else if (pflags->dotted) line_dot2();
    else line_solid1();
    exit(0);
}


//=============================================================
// Poly Lines
//=============================================================

static void polyline_blend(void)
{
    PDC pdc = textured_dc();
    pdc->blend = 0.1f;
    PIMAGE img = pdc->frame;
    srand(time(NULL));
    int count = 500;
    int cb = sizeof(POINT) * count;
    PPOINT pts = (PPOINT) CreateBuffer(cb, FALSE);
    for (int i=0; i<count; i++)
    {
        pts[i].x = RND() * img->w;
        pts[i].y = RND() * img->h;
    }
    DrawPolyLine(pdc, pts, count-1);
    SaveImageFileBob("_.img", pdc->frame);
}

static void polyline_solid(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    srand(time(NULL));
    int count = 500;
    int cb = sizeof(POINT) * count;
    PPOINT pts = (PPOINT) CreateBuffer(cb, FALSE);
    for (int i=0; i<count; i++)
    {
        pts[i].x = RND() * img->w;
        pts[i].y = RND() * img->h;
    }
    DrawPolyLine(pdc, pts, count-1);
    SaveImageFileBob("_.img", pdc->frame);
}

static void polyline_dots(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    SetLinePattern(pdc, "FFBB___");
    srand(time(NULL));
    int count = 500;
    int cb = sizeof(POINT) * count;
    PPOINT pts = (PPOINT) CreateBuffer(cb, FALSE);
    for (int i=0; i<count; i++)
    {
        pts[i].x = RND() * img->w;
        pts[i].y = RND() * img->h;
    }
    DrawPolyLine(pdc, pts, count-1);
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawPolyLine( PFLAGS pflags )
{
    if (pflags->blended) polyline_blend();
    else if (pflags->dotted) polyline_dots();
    else polyline_solid();
    exit(0);
}


//=============================================================
// Line Lists
//=============================================================

static void linelist_blend(void)
{
    PDC pdc = textured_dc();
    pdc->blend = 0.1f;
    PIMAGE img = pdc->frame;
    srand(time(NULL));
    int count = 1000;
    int cb = sizeof(POINT) * count;
    PPOINT pts = (PPOINT) CreateBuffer(cb, FALSE);
    for (int i=0; i<count; i++)
    {
        pts[i].x = RND() * img->w;
        pts[i].y = RND() * img->h;
    }
    DrawLineList(pdc, pts, count/2);
    SaveImageFileBob("_.img", pdc->frame);
}

static void linelist_solid(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    srand(time(NULL));
    int count = 1000;
    int cb = sizeof(POINT) * count;
    PPOINT pts = (PPOINT) CreateBuffer(cb, FALSE);
    for (int i=0; i<count; i++)
    {
        pts[i].x = RND() * img->w;
        pts[i].y = RND() * img->h;
    }
    DrawLineList(pdc, pts, count/2);
    SaveImageFileBob("_.img", pdc->frame);
}

static void linelist_dots(void)
{
    PDC pdc = textured_dc();
    PIMAGE img = pdc->frame;
    SetLinePattern(pdc, "FFBB___");
    srand(time(NULL));
    int count = 1000;
    int cb = sizeof(POINT) * count;
    PPOINT pts = (PPOINT) CreateBuffer(cb, FALSE);
    for (int i=0; i<count; i++)
    {
        pts[i].x = RND() * img->w;
        pts[i].y = RND() * img->h;
    }
    DrawLineList(pdc, pts, count/2);
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawLineList( PFLAGS pflags )
{
    if (pflags->blended) linelist_blend();
    else if (pflags->dotted) linelist_dots();
    else linelist_solid();
    exit(0);
}


//=============================================================
// Circles
//=============================================================

static void draw_circle(PDC pdc) 
{
    float radius = 50;
    float ox = pdc->frame->w / 2;
    float oy = pdc->frame->h / 2;
    POINT pt;
    for (int y=-1; y<=1; y++)
    {
        for (int x=-1; x<=1; x++)
        {
            pt.y = oy + y * oy * 0.8;
            pt.x = ox + x * ox * 0.8;
            DrawCircle(pdc, pt, radius);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void fill_circle(PDC pdc) 
{
    float radius = 50;
    float ox = pdc->frame->w / 2;
    float oy = pdc->frame->h / 2;
    POINT pt;
    for (int y=-1; y<=1; y++)
    {
        for (int x=-1; x<=1; x++)
        {
            pt.y = oy + y * oy * 0.8;
            pt.x = ox + x * ox * 0.8;
            FillCircle(pdc, pt, radius);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void draw_circle_solid(void) 
{
    PDC pdc = textured_dc();
    draw_circle(pdc);
}

static void draw_circle_dots(void) 
{
    PDC pdc = textured_dc();
    SetLinePattern(pdc, "FFBB___");
    draw_circle(pdc);
}

static void draw_circle_blend(void) 
{
    PDC pdc = textured_dc();
    pdc->blend = 0.2;
    draw_circle(pdc);
}

static void fill_circle_solid(void) 
{
    PDC pdc = textured_dc();
    fill_circle(pdc);
}

static void fill_circle_blend(void) 
{
    PDC pdc = textured_dc();
    pdc->blend = 0.5;
    fill_circle(pdc);
}

static void fill_circle_tex(void) 
{
    PDC pdc = textured_dc();
    pdc->tex = LoadImageFileBob("dt-tex2.img");
    fill_circle(pdc);
}

static void fill_circle_tex_blend(void) 
{
    PDC pdc = textured_dc();
    pdc->blend = 0.5;
    pdc->tex = LoadImageFileBob("dt-tex2.img");
    fill_circle(pdc);
}

void test_DrawCircle( PFLAGS pflags )
{
    if (pflags->filled)
    {
        test_FillCircle(pflags);
    }
    else
    {
        if (pflags->blended) draw_circle_blend();
        else if (pflags->dotted) draw_circle_dots();
        else draw_circle_solid();
    }
    exit(0);
}

void test_FillCircle( PFLAGS pflags )
{
    if (pflags->textured)
    {
        if (pflags->blended)
        {
            fill_circle_tex_blend();
        }
        else
        {
            fill_circle_tex();
        }
    }
    else
    {
        if (pflags->blended)
        {
            fill_circle_blend();
        }
        else
        {
            fill_circle_solid();
        }
    }
    exit(0);
}


//=============================================================
// Rectangles
//=============================================================

static void draw_rect(PDC pdc) 
{
    float radius = 30;
    float ox = pdc->frame->w / 2;
    float oy = pdc->frame->h / 2;
    RECT rc;
    for (int y=-1; y<=1; y++)
    {
        for (int x=-1; x<=1; x++)
        {
            int xc = oy + y * oy * 0.8;
            int yc = ox + x * ox * 0.8;
            rc.x = xc - radius;
            rc.y = yc - radius;
            rc.w = radius * 2;
            rc.h = radius * 2;
            DrawRect(pdc, &rc);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
}

static void fill_rect(PDC pdc) 
{
    float radius = 30;
    float ox = pdc->frame->w / 2;
    float oy = pdc->frame->h / 2;
    RECT rc;
    for (int y=-1; y<=1; y++)
    {
        for (int x=-1; x<=1; x++)
        {
            int xc = oy + y * oy * 0.8;
            int yc = ox + x * ox * 0.8;
            rc.x = xc - radius;
            rc.y = yc - radius;
            rc.w = radius * 2;
            rc.h = radius * 2;
            FillRect(pdc, &rc);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawRect( PFLAGS pflags )
{
    if (pflags->filled) 
    {
        test_FillRect(pflags);
        exit(0);
    }
    PDC pdc = textured_dc();
    if (pflags->dotted)
    {
        SetLinePattern(pdc, "FFBB___");
        draw_rect(pdc);        
        exit(0);
    }
    else if (pflags->blended)
    {
        pdc->blend = 0.5f;
        draw_rect(pdc);
        exit(0);
    }
    else
    {
        draw_rect(pdc);
        exit(0);
    }
}

void test_FillRect( PFLAGS pflags )
{
    PDC pdc = textured_dc();
    if (pflags->textured)
    {
        if (pflags->blended)
        {
            pdc->blend = 0.5;
            pdc->tex = LoadImageFileBob("dt-tex2.img");
            fill_rect(pdc);
            exit(0);
        }
        else
        {
            pdc->tex = LoadImageFileBob("dt-tex2.img");
            fill_rect(pdc);
            exit(0);
        }
    }
    else
    {
        if (pflags->blended)
        {
            pdc->blend = 0.5;
            fill_rect(pdc);
            exit(0);
        }
        else
        {
            fill_rect(pdc);
            exit(0);
        }
    }
}


//=============================================================
// Ellipses
//=============================================================

static void draw_ellipse(PDC pdc)
{
    float rx = 50;
    float ry = 35;
    float ox = pdc->frame->w / 2;
    float oy = pdc->frame->h / 2;
    RECT rc;
    for (int y=-1; y<=1; y++)
    {
        for (int x=-1; x<=1; x++)
        {
            POINT center;
            center.x = oy + y * oy * 0.8;
            center.y = ox + x * ox * 0.8;
            DrawEllipse(pdc, center, rx, ry);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawEllipse( PFLAGS pflags )
{
    if (pflags->filled) 
    {
        test_FillEllipse(pflags);
        exit(0);
    }
    PDC pdc = textured_dc();
    if (pflags->dotted)
    {
        pdc->bgc = MakeColor(0,0,0);
        SetLinePattern(pdc, "FFFFF__");
        draw_ellipse(pdc);
    }
    else if (pflags->blended)
    {
        pdc->blend = 0.5f;
        draw_ellipse(pdc);
    }
    else
    {
        draw_ellipse(pdc);
    }

    exit(0);
}

static void fill_ellipse(PDC pdc)
{
    float rx = 50;
    float ry = 35;
    float ox = pdc->frame->w / 2;
    float oy = pdc->frame->h / 2;
    RECT rc;
    for (int y=-1; y<=1; y++)
    {
        for (int x=-1; x<=1; x++)
        {
            POINT center;
            center.x = oy + y * oy * 0.8;
            center.y = ox + x * ox * 0.8;
            FillEllipse(pdc, center, rx, ry);
        }
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_FillEllipse( PFLAGS pflags )
{
    PDC pdc = textured_dc();
    if (pflags->textured)
    {
        if (pflags->blended)
        {
            pdc->blend = 0.5;
            pdc->tex = LoadImageFileBob("dt-tex2.img");
            fill_ellipse(pdc);
            exit(0);
        }
        else
        {
            pdc->tex = LoadImageFileBob("dt-tex2.img");
            fill_ellipse(pdc);
            exit(0);
        }
    }
    else
    {
        if (pflags->blended)
        {
            pdc->blend = 0.5;
            fill_ellipse(pdc);
            exit(0);
        }
        else
        {
            fill_ellipse(pdc);
            exit(0);
        }
    }
}


//=============================================================
// Patches (Triangles)
//=============================================================

static void draw_patch(PDC pdc)
{
    static POINT triangle[] = {
        {-3, 3},
        {3, 3},
        {0, -5}
    };
    POINT working[3];
    MAT3X2 mtxA, mtxB, mtxC;
    float centerX = pdc->frame->w / 2;
    float centerY = pdc->frame->h / 2;
    InitTranslateMat3x2(centerX, centerY, &mtxB);
    for (int i=1; i<15; i++)
    {
        float scale = i * 5;
        InitScaleMat3x2(scale, scale, &mtxA);
        CatMat3x2(&mtxA, &mtxB, &mtxC);
        ApplyListMat3x2(&mtxC, triangle, working, 3);
        DrawPatch(pdc, working);
    }
    SaveImageFileBob("_.img", pdc->frame);
}

void test_DrawPatch( PFLAGS pflags )
{
    if (pflags->filled)
    {
        test_FillPatch(pflags);
        exit(0);
    }

    PDC pdc = textured_dc();
    if (pflags->dotted)
    {
        pdc->bgc = MakeColor(0,0,0);
        SetLinePattern(pdc, "FFFFF__");
        draw_patch(pdc);
    }
    else if (pflags->blended)
    {
        pdc->blend = 0.5f;
        draw_patch(pdc);
    }
    else
    {
        draw_patch(pdc);
    }

    exit(0);
}

static void draw_clipped_polygons(PDC pdc)
{
    static POINT octagon[] =
    {
        {  5.000000,  0.000000 },
        {  3.535534,  3.535534 },
        {  0.000000,  5.000000 },
        { -3.535534,  3.535534 },
        { -5.000000, -0.000000 },
        { -3.535533, -3.535535 },
        {  0.000000, -5.000000 },
        {  3.535533, -3.535534 }
    };
    int count = sizeof(octagon) / sizeof(POINT);
    MAT3X2 mtxA, mtxB, mtxC, mtxD;
    float cx = pdc->frame->w / 2;
    float cy = pdc->frame->h / 2;
    InitTranslateMat3x2(cx, cy, &mtxA);
    float r = PI / 8;
    InitRotateMat3x2(r, &mtxB);
    float k = 45;
    InitScaleMat3x2(k, k, &mtxC);
    CatMat3x2(&mtxB, &mtxC, &mtxD);
    CatMat3x2(&mtxD, &mtxA, &mtxD);
    int cb = sizeof(POINT) * count;
    PPOINT octaReal = CreateBuffer(cb, FALSE);
    if (!octaReal)
    {
        LastCall("Test clipped polygon");
    }
    for (int i=0; i<count; i++)
    {
        octaReal[i] = ApplyMat3x2(&mtxD, octagon[i]);
    }
    // DrawPoly(pdc, octaReal, count);
    struct PointListHeader workingList;
    if (!CreatePointList(&workingList, count))
    {
        LastCall("Test clipped polygon");
    }
    workingList.Length = count;
    ConvertToPointList(octaReal, workingList.PointPtr, count);
    if (!ClipPolygon(pdc, &workingList))
    {
        LastCall("Test clipped polygon");
    }
    DestroyBuffer(octaReal);
    cb = count * sizeof(POINT);
    octaReal = CreateBuffer(cb, FALSE);
    if (!octaReal)
    {
        LastCall("Test clipped polygon");
    }
    ConvertFromPointList(workingList.PointPtr, octaReal, count);
    DrawPoly(pdc, octaReal, count);
    SaveImageFileBob("_.img", pdc->frame);
    DestroyBuffer(octaReal);
    DestroyPointList(&workingList);
}

static void fill_polygons(PDC pdc)
{
    static POINT triangle[] = {
        {-3, 3},
        {3, 3},
        {0, -5}
    };
    POINT working[3];
    MAT3X2 mtxA, mtxB, mtxC, mtxD;
    float centerX = pdc->frame->w / 2;
    float centerY = pdc->frame->h / 2;

    for (int i = -5; i <= 5; i++)
    {
        float r = i * PI / 10;
        float x = centerX + i * 50;
        InitRotateMat3x2(r, &mtxA);
        InitScaleMat3x2(5, 5, &mtxB);
        CatMat3x2(&mtxA, &mtxB, &mtxA);
        // Top row
        float y = 60;
        InitTranslateMat3x2(x, y, &mtxC);
        CatMat3x2(&mtxA, &mtxC, &mtxD);
        ApplyListMat3x2(&mtxD, triangle, working, 3);
        FillPoly(pdc, working, 3);
        // Bottom row
        y = pdc->frame->h - 60;
        InitTranslateMat3x2(x, y, &mtxC);
        CatMat3x2(&mtxA, &mtxC, &mtxD);
        ApplyListMat3x2(&mtxD, triangle, working, 3);
        FillPoly(pdc, working, 3);
    }

    SaveImageFileBob("_.img", pdc->frame);
}

void test_FillPatch( PFLAGS pflags )
{
    PDC pdc = textured_dc();
    if (pflags->clipped)
    {
        draw_clipped_polygons(pdc);
        exit(0);
    }
    if (pflags->blended)
    {
        pdc->blend = 0.5f;
    }
    if (pflags->textured)
    {
        pdc->tex = LoadImageFileBob("dt-tex2.img");
    }
    fill_polygons(pdc);
    exit(0);
}



//=============================================================
// Polygons
//=============================================================

void test_DrawPoly( PFLAGS pflags )
{
    if (pflags->filled)
    {
        test_FillPoly(pflags);
        exit(0);
    }
    exit(0);
}

void test_FillPoly( PFLAGS pflags )
{
    exit(0);
}


//=============================================================
// Blitting
//=============================================================

void test_BlitImage( PFLAGS pflags )
{
    PDC pdc = textured_dc();
    LPIMAGE img = LoadImageFileBob("dt-tex2.img");
    if (pflags->blended) pdc->blend = 0.5f;
    POINT pt;
    pt.x = 5;
    pt.y = 5;
    BlitImage(pdc, img, pt);
    SaveImageFileBob("_.img", pdc->frame);
    exit(0);
}

void test_StretchImage( PFLAGS pflags )
{
    PDC pdc = textured_dc();
    LPIMAGE img = LoadImageFileBob("dt-tex2.img");
    if (pflags->blended) pdc->blend = 0.5f;
    POINT pt;
    pt.x = 5;
    pt.y = 5;
    SIZE sz;
    sz.w = 490;
    sz.h = 490;
    StretchImage(pdc, img, pt, sz);
    SaveImageFileBob("_.img", pdc->frame);
    exit(0);
}


//=============================================================
// Lookup Tables
//=============================================================

int GetTokenID(LPCSTR token)
{
    LPCSTR sz;
    for (int i=0; sz=m_disp[i].token; i++)
    {
        if (strcmp(token, sz) == 0)
            return m_disp[i].what;
    }
    return W_ERROR;
}

PFN_TEST GetTestFromID(int what)
{
    if (what < 0) return NULL;
    for (int i=0; m_disp[i].token; i++)
    {
        if (m_disp[i].what == what)
            return m_disp[i].func;
    }
    return NULL;
}


//=============================================================
// Extra Test
//=============================================================

typedef struct _t_poly_header
{
    int length;
    POINT points[100];
} t_poly_header, *PPOLYHEADER;

static BOOL read_poly_file(PPOLYHEADER hdr)
{
    FILE* file = fopen("dt-poly.txt", "r");
    if (!file) return FALSE;
    hdr->length = 0;
    while (!feof(file) && (hdr->length<100))
    {
        float x, y;
        fscanf(file, "%f %f", &x, &y);
        hdr->points[hdr->length].x = x;
        hdr->points[hdr->length].y = y;
        hdr->length++;
    }
    fclose(file);
    return TRUE;
}

void get_bounding_box(PPOLYHEADER hdr, PRECT rc)
{
    float cl=1e+20;
    float cr=-1e+20;
    float ct=1e+20;
    float cb=-1e+20;
    float x, y;
    for (int i=0; i<hdr->length; i++)
    {
        x = hdr->points[i].x;
        y = hdr->points[i].y;
        if (x < cl) cl = x;
        if (x > cr) cr = x;
        if (y < ct) ct = y;
        if (y > cb) cb = y;
    }
    rc->x = cl;
    rc->y = ct;
    rc->w = cr - cl + 1;
    rc->h = cb - ct + 1;
}

void test_Extra( PFLAGS pflags )
{
    PDC pdc = textured_dc();
    t_poly_header hdr;
    float centerX = pdc->frame->w / 2;
    float centerY = pdc->frame->h / 2;
    if (pflags->blended)
    {
        pdc->blend = 0.5f;
    }
    if (pflags->textured)
    {
        pflags->filled = TRUE;
        pdc->tex = LoadImageFileBob("dt-tex2.img");
    }
    else if (pflags->dotted)
    {
        pflags->filled = FALSE;
        SetLinePattern(pdc, "FFBB___");
    }
    if (read_poly_file(&hdr))
    {
        MAT3X2 mtxA, mtxB;
        RECT rc;
        PFN_SPAN filler;
        get_bounding_box(&hdr, &rc);
        float scale = MAX(rc.w, rc.h);
        scale = 350 / scale;
        InitScaleMat3x2(scale, scale, &mtxB);
        InitTranslateMat3x2(centerX, centerY, &mtxA);
        CatMat3x2(&mtxB, &mtxA, &mtxA);
        ApplyListMat3x2(&mtxA, hdr.points, hdr.points, hdr.length);
        if (pflags->filled) FillPoly(pdc, hdr.points, hdr.length);
        else DrawPoly(pdc, hdr.points, hdr.length);
        SaveImageFileBob("_.img", pdc->frame);
        exit(0);
    }
    else
    {
        LastCall("Failed to read polygon file");
    }
}

