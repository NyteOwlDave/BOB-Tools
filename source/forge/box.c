
/*

    box.c

    Draw a simple box

*/

#include "inc/forge.h"


static void usage(void);

static int m_sides[6][4] = {
    { 1, 2, 3, 4 },     // Top
    { 5, 8, 7, 6 },     // Bottom
    { 1, 4, 8, 5 },     // Left
    { 2, 6, 7, 3 },     // Right
    { 1, 5, 6, 2 },     // Front
    { 3, 7, 8, 4 }      // Back
};

static point_t m_verts[8];

// Entry point
int main(int argc, char** argv, char** envp) {
    if (argc != 4) {
        usage();
    }
    double w = atof(argv[1]) / 2;
    double h = atof(argv[2]) / 2;
    double d = atof(argv[3]) / 2;
	double cube = w*h*d;

    if (cube == 0) {
        usage();
    }

    if (isnan( cube )) {
        usage();
    }
    
    // 1
    m_verts[0].x = -w;
    m_verts[0].y =  h;
    m_verts[0].z = -d;

    // 2
    m_verts[1].x =  w;
    m_verts[1].y =  h;
    m_verts[1].z = -d;

    // 3
    m_verts[2].x =  w;
    m_verts[2].y =  h;
    m_verts[2].z =  d;

    // 4
    m_verts[3].x = -w;
    m_verts[3].y =  h;
    m_verts[3].z =  d;

    // 5
    m_verts[4].x = -w;
    m_verts[4].y = -h;
    m_verts[4].z = -d;

    // 6
    m_verts[5].x =  w;
    m_verts[5].y = -h;
    m_verts[5].z = -d;

    // 7
    m_verts[6].x =  w;
    m_verts[6].y = -h;
    m_verts[6].z =  d;

    // 8
    m_verts[7].x = -w;
    m_verts[7].y = -h;
    m_verts[7].z =  d;

    PPOLYGON ppoly = PolyCreate(4);
    for (int i=0; i<6; i++)
    {
        for (int j=0; j<4; j++) {
            ppoly->vtx[j] = m_verts[m_sides[i][j]-1];
        }
        PolyCompose(ppoly);
    }

    return 0;
}

// Show command line usage
static void usage(void)
{
    printf("BOX width height depth\n");
    exit(1);
}