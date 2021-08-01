
/*

    shape.c

    Shape functions

*/

#include "inc/forge.h"


/* ---------------------- Point Functions ---------------------- */

// Create a new point object
PPOINT PointCreate(void)
{
    size_t cb = sizeof (point_t);
    return (PPOINT) vmalloc( cb );
}

// Convert point to vector
void PointToVec(PPOINT ppoint, PVEC pv)
{
    ptr_check(ppoint, "PointToVec (ppoint)");
    ptr_check(pv, "PointToVec (pv)");
    *pv[0] = ppoint->x;
    *pv[1] = ppoint->y;
    *pv[2] = ppoint->z;
}

// Convert vector to point
void PointFromVec(PVEC pv, PPOINT ppoint)
{
    ptr_check(ppoint, "PointFromVec (ppoint)");
    ptr_check(pv, "PointFromVec (pv)");
    ppoint->x = *pv[0];
    ppoint->y = *pv[1];
    ppoint->z = *pv[2];
}

// Normalize point
double PointNormalize(PPOINT ppoint)
{
    Vec v;
    PointToVec(ppoint, &v);
    double len = VecNormalize( &v );
    PointFromVec( &v, ppoint );
    return len;
}

/* ---------------------- Patch Functions ---------------------- */

// Create new patch object
PPATCH PatchCreate(void)
{
    size_t cb = sizeof (patch_t);
    return (PPATCH) vmalloc( cb );
}

// Calculate patch surface normal
void PatchCalcNormal(PPATCH ppatch) {
    ptr_check(ppatch, "PatchCalcNormal");
    PointNormalize(&(ppatch->tri_n[0]));
    PointNormalize(&(ppatch->tri_n[1]));
    PointNormalize(&(ppatch->tri_n[2]));
    Vec va, vb, vc, vd, ve, vn;
	PointToVec(&(ppatch->tri_p[0]), &va);
	PointToVec(&(ppatch->tri_p[1]), &vb);
	PointToVec(&(ppatch->tri_p[2]), &vc);
    VecSub(&vb, &va, &vd);
    VecSub(&vc, &va, &ve);
	VecCross(&vd, &ve, &vn);
	VecNormalize(&vn);
    PointFromVec(&vn, &(ppatch->normal));
}

// Compose patch object
void PatchCompose(PPATCH ppatch) 
{
    ptr_check(ppatch, "PatchCompose");
    printf("%s\n", "tri {");
    for (int n=0; n<3; n++) 
    {
        printf("vertex %f %f %f ",  ppatch->tri_p[n].x, ppatch->tri_p[n].y, ppatch->tri_p[n].z);
        printf("normal %f %f %f\n", ppatch->tri_n[n].x, ppatch->tri_n[n].y, ppatch->tri_n[n].z);
    }
    printf("%s\n", "}");
}

/* ---------------------- Polygon Functions ---------------------- */

// Create new polygon object
PPOLYGON PolyCreate(int num_verts)
{
    if (num_verts < 3) 
    {
        last_call("Not enough vertices for polygon", 4);
    }
    size_t cb = sizeof (polygon_t) + num_verts * sizeof (point_t);
    PPOLYGON ppoly = (PPOLYGON) vmalloc( cb );
    ppoly->points = num_verts;
    return ppoly;
}

// Calculate polygon surface normal
void PolyCalcNormal(PPOLYGON ppoly) 
{
    ptr_check(ppoly, "PolyCalcNormal");
    int n = 2, verts = ppoly->points;
    Vec va, vb, vc, vd, ve, vn;
    PointToVec(&(ppoly->vtx[0]), &va);
    PointToVec(&(ppoly->vtx[1]), &vb);
    VecSub(&vb, &va, &vd);
    while (n < verts) {
        PointToVec(&(ppoly->vtx[n]), &vc);
        VecSub(&vc, &vb, &ve);
        VecCross(&vd, &ve, &vn);
        if (VecLen( &vn ) > TINY) {
            VecNormalize( &vn );
            PointFromVec( &vn, &(ppoly->normal));
            return;
        }
        n++;
    }
    last_call("Degenerate polygon", 5);
}

// Compose polygon object
void PolyCompose(PPOLYGON ppoly) 
{
    ptr_check(ppoly, "PolyCompose");
    int num_verts = ppoly->points;
    printf("%s\n", "poly {");
    printf("points %i\n", num_verts);
    for (int n=0; n<num_verts; n++) 
    {
        printf("vertex %f %f %f\n",  ppoly->vtx[n].x, ppoly->vtx[n].y, ppoly->vtx[n].z);
    }
    printf("%s\n", "}");
}

/* ---------------------- Ring Functions ---------------------- */

// Create a new ring object
PRING RingCreate(void)
{
    size_t cb = sizeof (ring_t);
    return (PRING) vmalloc( cb );
}

// Calculate ring surface normal
void RingCalcNormal(PRING pring) 
{
    ptr_check(pring, "RingCalcNormal");
    PointNormalize(&(pring->normal));
}

// Compose ring object
void RingCompose(PRING pring) 
{
    ptr_check(pring, "RingCompose");
    printf("%s\n", "ring {");
    printf("center %f, %f, %f\n", pring->center.x, pring->center.y, pring->center.z);
    printf("normal %f, %f, %f\n", pring->normal.x, pring->normal.y, pring->normal.z);
    double a = pring->min_radius;
    double b = pring->max_radius;
    if (a < TINY) 
    {
        printf("radius %f\n", b);
    } 
    else
    {
        printf("min_radius %f\n", a);
        printf("max_radius %f\n", b);
    }
    printf("%s\n", "}");
}

/* ---------------------- Cone Functions ---------------------- */

// Create a new cone object
PCONE ConeCreate(void)
{
    size_t cb = sizeof (cone_t);
    return (PCONE) vmalloc( cb );
}

// Calculate cone surface normal
void ConeCalcNormal(PCONE pcone) 
{
    ptr_check(pcone, "ConeCalcNormal");
    Vec va, vb, vc;
    PointToVec(&(pcone->base), &va);
    PointToVec(&(pcone->apex), &vb);
    VecSub(&vb, &va, &vc);
    VecNormalize( &vc );
    PointFromVec( &vc, &(pcone->normal) );
}

// Compose cone object
void ConeCompose(PCONE pcone) 
{
    ptr_check(pcone, "ConeCompose");
    printf("%s\n", "cone {");
    printf("apex %f, %f, %f\n", pcone->apex.x, pcone->apex.y, pcone->apex.z);
    printf("base %f, %f, %f\n", pcone->base.x, pcone->base.y, pcone->base.z);
    double a = pcone->apex_radius;
    double b = pcone->base_radius;
    if (a < TINY) 
    {
        printf("radius %f\n", b);
    } 
    else
    {
        printf("apex_radius %f\n", a);
        printf("base_radius %f\n", b);
    }
    printf("%s\n", "}");
}

/* ---------------------- Sphere Functions ---------------------- */

// Create a new sphere object
PSPHERE SphereCreate(void)
{
    size_t cb = sizeof (sphere_t);
    return (PSPHERE) vmalloc( cb );
}

// Calculate sphere surface normal
void SphereCalcNormal(PSPHERE psphere) 
{
    ptr_check(psphere, "SphereCalcNormal");
    PointNormalize(&(psphere->normal));
}

// Compose sphere object
void SphereCompose(PSPHERE psphere) 
{
    ptr_check(psphere, "SphereCompose");
    printf("%s\n", "sphere {");
    printf("center %f, %f, %f\n", psphere->center.x, psphere->center.y, psphere->center.z);
    printf("radius %f\n", psphere->radius);
    printf("fuzz %f\n", psphere->fuzz);
    printf("%s\n", "}");
}
