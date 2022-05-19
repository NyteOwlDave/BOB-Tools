
/*

  vec.c 
  
  Scalar and Vector 3 Math

*/

#include "inc/forge.h"

// Gamma scalar function
double gamma_correct(double n, double nMax, double gamma)
{
    return (nMax * pow(n / nMax, 1 / gamma));
}

// Lerp scalar function
double lerp(double n, double a, double b)
{
    return ((n * a) + ((1 - n) * b));
}

// Initialize vector elements
void VecMake(PVEC v, double x, double y, double z)
{
    ptr_check(v, "VecInit");
    *v[0] = x;
    *v[1] = y;
    *v[2] = z;
}

// Allocate and initialize vector
PVEC VecCreate(double x, double y, double z)
{
    PVEC vout = (PVEC)malloc(sizeof(Vec));
    VecMake(vout, x, y, z);
}

// Duplicate vector
PVEC VecClone(PVEC vin)
{
    if (vin)
    {
        return VecCreate(*vin[0], *vin[1], *vin[2]);
    }
    else
    {
        return VecCreate(0, 0, 0);
    }
}

// Elementwise negate
void VecNegate(PVEC v)
{
    ptr_check(v, "VecNegate");
    *v[0] = -*v[0];
    *v[1] = -*v[1];
    *v[2] = -*v[2];
}

// Dot product
double VecDot(PVEC va, PVEC vb)
{
    ptr_check(va, "VecDot (va)");
    ptr_check(vb, "VecDot (vb)");
    return (*va[0]) * (*vb[0]) +
           (*va[1]) * (*vb[1]) +
           (*va[2]) * (*vb[2]);
}

// Vector length squared
double VecLenSqr(PVEC v)
{
    ptr_check(v, "VecLenSqr");
    return VecDot(v, v);
}

// Vector length
double VecLen(PVEC v)
{
    return (sqrt(VecLenSqr(v)));
}

// Copy a -> b
void VecCopy(PVEC va, PVEC vb)
{
    ptr_check(va, "VecCopy (va)");
    ptr_check(vb, "VecCopy (vb)");
    *vb[0] = *va[0];
    *vb[1] = *va[1];
    *vb[2] = *va[2];
}

// Elementwise add (c = a + b)
void VecAdd(PVEC va, PVEC vb, PVEC vc)
{
    ptr_check(va, "VecAdd (va)");
    ptr_check(vb, "VecAdd (vb)");
    ptr_check(vb, "VecAdd (vc)");
    *vc[0] = *va[0] + *vb[0];
    *vc[1] = *va[1] + *vb[1];
    *vc[2] = *va[2] + *vb[2];
}

// Elementwise subtract (c = a - b)
void VecSub(PVEC va, PVEC vb, PVEC vc)
{
    ptr_check(va, "VecSub (va)");
    ptr_check(vb, "VecSub (vb)");
    ptr_check(vb, "VecSub (vc)");
    *vc[0] = *va[0] - *vb[0];
    *vc[1] = *va[1] - *vb[1];
    *vc[2] = *va[2] - *vb[2];
}

// Elementwise multiply (c = a * b)
void VecMul(PVEC va, PVEC vb, PVEC vc)
{
    ptr_check(va, "VecMul (va)");
    ptr_check(vb, "VecMul (vb)");
    ptr_check(vb, "VecMul (vc)");
    *vc[0] = (*va[0]) * (*vb[0]);
    *vc[1] = (*va[1]) * (*vb[1]);
    *vc[2] = (*va[2]) * (*vb[2]);
}

// Combine vectors (A*a + B*b -> c)
void VecComb(double a, PVEC va, double b, PVEC vb, PVEC vc)
{
    ptr_check(va, "VecComb (va)");
    ptr_check(vb, "VecComb (vb)");
    ptr_check(vb, "VecComb (vc)");
    *vc[0] = a * (*va[0]) + b * (*vb[0]);
    *vc[1] = a * (*va[1]) + b * (*vb[1]);
    *vc[2] = a * (*va[2]) + b * (*vb[2]);
}

// Scale vector (A*a -> b)
void VecS(double a, PVEC va, PVEC vb)
{
    ptr_check(va, "VecS (va)");
    ptr_check(vb, "VecS (vb)");
    *vb[0] = a * (*va[0]);
    *vb[1] = a * (*va[1]);
    *vb[2] = a * (*va[2]);
}

// Add scaled vectors (A*a + b -> c)
void VecAddS(double a, PVEC va, PVEC vb, PVEC vc)
{
    ptr_check(va, "VecAddS (va)");
    ptr_check(vb, "VecAddS (vb)");
    ptr_check(vb, "VecAddS (vc)");
    *vc[0] = a * (*va[0]) + (*vb[0]);
    *vc[1] = a * (*va[1]) + (*vb[1]);
    *vc[2] = a * (*va[2]) + (*vb[2]);
}

// Left handed cross product
void VecCross(PVEC va, PVEC vb, PVEC vc)
{
    ptr_check(va, "VecAddS (va)");
    ptr_check(vb, "VecAddS (vb)");
    ptr_check(vb, "VecAddS (vc)");
    (*vc)[0] = (*va)[1] * (*vb)[2] - (*va)[2] * (*vb)[1];
    (*vc)[1] = (*va)[2] * (*vb)[0] - (*va)[0] * (*vb)[2];
    (*vc)[2] = (*va)[0] * (*vb)[1] - (*va)[1] * (*vb)[0];
}

// Zero vector to 0 0 0
void VecZero(PVEC v)
{
    ptr_check(v, "VecZero");
    *v[0] = 0;
    *v[1] = 0;
    *v[2] = 0;
}

// Normalize vector (b = a/|a|)
double VecNormal(PVEC va, PVEC vb)
{
    ptr_check(vb, "VecNormal (vb)");
    double len = VecLenSqr(va);
    if (len > TINY)
    {
        len = sqrt(len);
        double t = 1 / len;
        (*vb)[0] = t * (*va)[0];
        (*vb)[1] = t * (*va)[1];
        (*vb)[2] = t * (*va)[2];
    }
    else
    {
        len = 1.0;
        VecMake(vb, 1, 0, 0);
    }
    return len;
}

// Normalize vector in place (v = v/|v|)
double VecNormalize(PVEC v)
{
    return (VecNormal(v, v));
}

// 3D line segment midpoint
void VecMidpoint2(PVEC va, PVEC vb, PVEC vc)
{
    ptr_check(va, "VecMidpoint2 (va)");
    ptr_check(vb, "VecMidpoint2 (vb)");
    ptr_check(vc, "VecMidpoint2 (vc)");
    *vc[0] = (*va[0] + *vb[0]) / 2;
    *vc[1] = (*va[1] + *vb[1]) / 2;
    *vc[2] = (*va[2] + *vb[2]) / 2;
}

// 3D triangle midpoint
void VecMidpoint3(PVEC va, PVEC vb, PVEC vc, PVEC vd)
{
    ptr_check(va, "VecMidpoint3 (va)");
    ptr_check(vb, "VecMidpoint3 (vb)");
    ptr_check(vc, "VecMidpoint3 (vc)");
    ptr_check(vd, "VecMidpoint3 (vd)");
    *vd[0] = (*va[0] + *vb[0] + *vc[0]) / 3;
    *vd[1] = (*va[1] + *vb[1] + *vc[1]) / 3;
    *vd[2] = (*va[2] + *vb[2] + *vc[2]) / 3;
}

// Manhatten length
double VecManhatten(PVEC v)
{
    return (*v[0] + *v[1] + *v[2]);
}
