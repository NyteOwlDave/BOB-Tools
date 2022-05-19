
/*
	mtx.c

	Vector and matrix function

*/

#include "inc/stdafx.h"
#include "inc/mtx.h"


#define SIZE (4)
#define HUGE (1.0e+8)
#define TINY (1.0e-6)


static void lubksb(Matrix a, int *indx, Flt *b);
static void ludcmp(Matrix a, int *indx, Flt *d);


Flt VecNormalize(Vec vec)
{

    Flt len;

    len = VecDot(vec, vec);
    if (ABS(len) > TINY)
    {
        len = sqrt(len);
        vec[0] /= len;
        vec[1] /= len;
        vec[2] /= len;
    }
    else
    {
        // printf("%.4f %.4f %.4f == %.4f bummer\n", vec[0], vec[1], vec[2], len);
        len = 1.0;
        vec[0] = 1.0;
        vec[1] = 0.0;
        vec[2] = 0.0;
    }
    return (len);
}

void InitIdentityMat(Matrix mat)
{

    int i, j;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (i == j)
                mat[i][j] = 1.0;
            else
                mat[i][j] = 0.0;
}

void CatMat(Matrix m1, Matrix m2, Matrix dest)
{

    Matrix m3;
    int i, j, k;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            m3[i][j] = 0;
            for (k = 0; k < 4; k++)
            {
                m3[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    // copy results to dest
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            dest[i][j] = m3[i][j];
        }
    }
}

void ApplyMat(Matrix mat, Vec in, Vec out)
{

    Flt in4[4], out4[4];
    int i, j;

    in4[0] = in[0];
    in4[1] = in[1];
    in4[2] = in[2];
    in4[3] = 1.0;

    for (i = 0; i < 4; i++)
    {
        out4[i] = 0.0;
        for (j = 0; j < 4; j++)
        {
            out4[i] += mat[j][i] * in4[j];
        }
    }
    out[0] = out4[0] / out4[3];
    out[1] = out4[1] / out4[3];
    out[2] = out4[2] / out4[3];
}

void ApplyNormalMat(Matrix mat, Vec in, Vec out)
{

    Vec t1, t2; // tangent vectors
    Vec orig;   // imaginary center
    Flt dot;

    VecNormalize(in);

    MakeVector(0.0, 0.0, 0.0, orig);

    // create a vector not aligned with in
    MakeVector(1.0, 0.0, 0.0, t1);
    dot = VecDot(t1, in);
    if (ABS(dot) > 0.8)
    {
        MakeVector(0.0, 1.0, 0.0, t1);
    }

    VecCross(in, t1, t2); // create t2
    VecCross(t2, in, t1); // create proper t1

    // transform tangents
    ApplyMat(mat, t1, t1);
    ApplyMat(mat, t2, t2);
    ApplyMat(mat, orig, orig);

    VecSub(t1, orig, t1);
    VecSub(t2, orig, t2);

    VecCross(t1, t2, out); // recreate normal
}

void InverseMat(Matrix m, Matrix n)
{

    Matrix y;
    int i, j, indx[4];
    double d, col[4];

    CopyMat(m, n);   // save original matrix
    ludcmp(n, indx, &d); // matrix lu decomposition

    for (j = 0; j < SIZE; j++)
    { // matrix inversion
        for (i = 0; i < SIZE; i++)
        {
            col[i] = 0.0;
        }
        col[j] = 1.0;
        lubksb(n, indx, col);
        for (i = 0; i < SIZE; i++)
        {
            y[i][j] = col[i];
        }
    }
    CopyMat(y, n);
}

//
// LU backward substitution
// a			input matrix
// indx			row permutation record
// b			right hand vector
//
static void lubksb(Matrix a, int *indx, Flt *b)
{

    int i, j, ii = -1, ip;
    Flt sum;

    for (i = 0; i < SIZE; i++)
    {
        ip = indx[i];
        sum = b[ip];
        b[ip] = b[i];
        if (ii >= 0)
        {
            for (j = ii; j <= i - 1; j++)
            {
                sum -= a[i][j] * b[j];
            }
        }
        else if (sum != 0.0)
        {
            ii = i;
        }
        b[i] = sum;
    }
    for (i = SIZE - 1; i >= 0; i--)
    {
        sum = b[i];
        for (j = i + 1; j < SIZE; j++)
        {
            sum -= a[i][j] * b[j];
        }
        b[i] = sum / a[i][i];
    }
}

//
// LU decomposition
//	a           input matrix. gets thrashed
//	indx        row permutation record
//	d           +/- 1.0 (even or odd # of row interchanges
//
static void ludcmp(Matrix a, int *indx, Flt *d)
{

    Flt vv[SIZE]; // implicit scale for each row
    Flt big, dum, sum, tmp;
    int i, imax, j, k;

    *d = 1.0;
    for (i = 0; i < SIZE; i++)
    {
        big = 0.0;
        for (j = 0; j < SIZE; j++)
        {
            if ((tmp = fabs(a[i][j])) > big)
            {
                big = tmp;
            }
        }
        if (big == 0.0)
        {
            errno = EDOM;
            LastCall("Singular matrix");
        }
        vv[i] = 1.0 / big;
    }
    for (j = 0; j < SIZE; j++)
    {
        for (i = 0; i < j; i++)
        {
            sum = a[i][j];
            for (k = 0; k < i; k++)
            {
                sum -= a[i][k] * a[k][j];
            }
            a[i][j] = sum;
        }
        big = 0.0;
        for (i = j; i < SIZE; i++)
        {
            sum = a[i][j];
            for (k = 0; k < j; k++)
            {
                sum -= a[i][k] * a[k][j];
            }
            a[i][j] = sum;
            if ((dum = vv[i] * fabs(sum)) >= big)
            {
                big = dum;
                imax = i;
            }
        }
        if (j != imax)
        {
            for (k = 0; k < SIZE; k++)
            {
                dum = a[imax][k];
                a[imax][k] = a[j][k];
                a[j][k] = dum;
            }
            *d = -(*d);
            vv[imax] = vv[j];
        }
        indx[j] = imax;
        if (a[j][j] == 0.0)
        {
            a[j][j] = 1.0e-20; // can be 0.0 also...
        }
        if (j != SIZE - 1)
        {
            dum = 1.0 / a[j][j];
            for (i = j + 1; i < SIZE; i++)
            {
                a[i][j] *= dum;
            }
        }
    }
}

void CopyMat(Matrix m1, Matrix m2)
{
    memcpy(m2, m1, sizeof(Matrix));
}

void InitTranslateMat(Vec vt, Matrix mout)
{
    InitIdentityMat(mout);
    mout[0][3] = vt[0];
    mout[1][3] = vt[1];
    mout[2][3] = vt[2];
}

void InitScaleMat(Vec vs, Matrix mout)
{

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (i != j)
                mout[i][j] = 0.0;
    mout[0][0] = vs[0];
    mout[1][1] = vs[1];
    mout[2][2] = vs[2];
    mout[3][3] = 1;
}

void InitRotateMat(Vec vr, Matrix mout)
{
    Flt ra = vr[0];
    Flt rb = vr[1];
    Flt rc = vr[2];
    if (ra == 0) 
    {
        if (rb == 0)
        {
            if (rc == 0)
            {
                // No rotation specified
                InitIdentityMat(mout);
                return;
            }
            // Rotation around z-axis
            else
            {
                Flt c = cos(rc);
                Flt s = sin(rc);
                mout[0][0] = c;
                mout[0][1] = -s;
                mout[0][2] = 0;
                mout[0][3] = 0;
                mout[1][0] = s;
                mout[1][1] = c;
                mout[1][2] = 0;
                mout[1][3] = 0;
                mout[2][0] = 0;
                mout[2][1] = 0;
                mout[2][2] = 1;
                mout[2][3] = 0;
                mout[3][0] = 0;
                mout[3][1] = 0;
                mout[3][2] = 0;
                mout[3][3] = 1;
                return;
            }
        }
        else if (rc == 0)
        {
            // Rotation around y-axis
            Flt c = cos(rb);
            Flt s = sin(rb);
            mout[0][0] = c;
            mout[0][1] = 0;
            mout[0][2] = s;
            mout[0][3] = 0;
            mout[1][0] = 0;
            mout[1][1] = 1;
            mout[1][2] = 0;
            mout[1][3] = 0;
            mout[2][0] = -s;
            mout[2][1] = 0;
            mout[2][2] = c;
            mout[2][3] = 0;
            mout[3][0] = 0;
            mout[3][1] = 0;
            mout[3][2] = 0;
            mout[3][3] = 1;
            return;
        }

    }
    else if (rb == 0)
    {
        if (rc == 0)
        {
            // Rotation around x axis
            Flt c = cos(ra);
            Flt s = sin(ra);
            mout[0][0] = 1;
            mout[0][1] = 0;
            mout[0][2] = 0;
            mout[0][3] = 0;
            mout[1][0] = 0;
            mout[1][1] = c;
            mout[1][2] = -s;
            mout[1][3] = 0;
            mout[2][0] = 0;
            mout[2][1] = s;
            mout[2][2] = c;
            mout[2][3] = 0;
            mout[3][0] = 0;
            mout[3][1] = 0;
            mout[3][2] = 0;
            mout[3][3] = 1;
            return;
        }
    }
    Flt sx = sin(ra);
    Flt cx = cos(ra);
    Flt sy = sin(rb);
    Flt cy = cos(rb);
    Flt sz = sin(rc);
    Flt cz = cos(rc);
    mout[0][0] = cy * cz;
    mout[0][1] = sx * sy * cz - cx * sz;
    mout[0][2] = cx * sy * cz + sx * sz;
    mout[0][3] = 0;
    mout[1][0] = cy * sz;
    mout[1][1] = sx * sy * sz + cx * cz;
    mout[1][2] = cx * sy * sz - sx * cz;
    mout[1][3] = 0;
    mout[2][0] = -sy;
    mout[2][1] = sx * cy;
    mout[2][2] = cx * cy;
    mout[2][3] = 0;
    mout[3][0] = 0;
    mout[3][1] = 0;
    mout[3][2] = 0;
    mout[3][3] = 1;
}

void InitPatchMat(Vec a, Vec b, Vec c, Matrix mout)
{
    Vec d, e, f;
    VecSub(b, a, d);
    VecSub(c, b, e);
    VecNormalize(d);
    VecNormalize(e);
    VecCross(d, e, f);
    mout[0][0] = d[0];
    mout[0][1] = d[1];
    mout[0][2] = d[2];
    mout[0][3] = 0;
    mout[1][0] = e[0];
    mout[1][1] = e[1];
    mout[1][2] = e[2];
    mout[1][3] = 0;
    mout[2][0] = f[0];
    mout[2][1] = f[1];
    mout[2][2] = f[2];
    mout[2][3] = 0;
    mout[0][0] = 
    mout[0][1] = 
    mout[0][2] = 0;
    mout[0][3] = 1;
}

void InitQuatMat(Vec v, Flt r, Matrix mout)
{
    Flt c = cos( r );
    Flt s = sin( r );
    Flt cc = 1 - c;
    Vec u;
    VecCopy(v, u);
    VecNormalize(u);
    Flt x = v[0];
    Flt y = v[1];
    Flt z = v[2];
    mout[0][0] = (cc*x*x)+c;
    mout[0][1] = (cc*x*y)+(z*s);
    mout[0][2] = (cc*x*z)-(y*s);
    mout[0][3] = 0;
    mout[1][0] = (cc*x*y)-(z*s);
    mout[1][1] = (cc*y*y)+c;
    mout[1][2] = (cc*z*y)+(x*s);
    mout[1][3] = 0;
    mout[2][0] = (cc*x*z)+(y*s);
    mout[2][1] = (cc*y*z)-(x*s);
    mout[2][2] = (cc*z*z)+c;
    mout[2][3] = 0;
    mout[0][0] = 
    mout[0][1] = 
    mout[0][2] = 0;
    mout[0][3] = 1;
}

void InitViewMat(Vec eye, Vec at, Vec up, Matrix mout)
{
    Vec x, y, z;
    VecSub(eye, at, z);
    if (VecLenSqr(z) < TINY) 
    {
        // eye and target are in the same position
        z[2] = 1;
    }
    VecNormalize(z);
    VecCross(up, z, x);
    if (VecLenSqr(x) < TINY) 
    {
        // eye and target are in the same vertical
        z[2] += 0.0001;
        VecCross(up, z, x);
    }
    VecNormalize(x);
    VecCross(z,x,y);
    mout[0][0] = x[0];
    mout[0][1] = x[1];
    mout[0][2] = x[2];
    mout[0][3] = 0;
    mout[1][0] = y[0];
    mout[1][1] = y[1];
    mout[1][2] = y[2];
    mout[1][3] = 0;
    mout[2][0] = z[0];
    mout[2][1] = z[1];
    mout[2][2] = z[2];
    mout[2][3] = 0;
    mout[0][0] = 
    mout[0][1] = 
    mout[0][2] = 0;
    mout[0][3] = 1;
}

void TranslateMat(Vec vt, Matrix min, Matrix mout)
{
    Matrix tmp;
    InitTranslateMat(vt, tmp);
    CatMat(tmp, min, mout);
}

void ScaleMat(Vec vs, Matrix min, Matrix mout)
{
    Matrix tmp;
    InitScaleMat(vs, tmp);
    CatMat(tmp, min, mout);
}

void RotateMat(Vec vr, Matrix min, Matrix mout)
{
    Matrix tmp;
    InitRotateMat(vr, tmp);
    CatMat(tmp, min, mout);
}

