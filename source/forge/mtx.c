
/*

  mtx.c
  
  Matrix 4x4 Math
  
*/

#include <memory.h>
#include "inc/forge.h"


static void ludcmp(Mtx a, int* indx, double* d);
static void lubksb(Mtx a, int* indx, double* b);


// Create a matrix
PMTX MtxCreate(void) {
    return (PMTX) malloc(sizeof (Mtx));
}

// Create a copy of a matrix
PMTX MtxClone(PMTX pm) {
    PMTX pnew = MtxCreate();
    MtxCopy(pm, pnew);
    return pnew;
}

// Copy a matrix (a -> b)
void MtxCopy(PMTX ma, PMTX mb) {
    ptr_check(ma, "MtxCopy (ma)");
    ptr_check(mb, "MtxCopy (mb)");
    memcpy(mb, ma, sizeof (Mtx));
}

// Return empty matrix
void MtxZero(PMTX pm) {
    ptr_check(pm, "MtxZero");
    *pm[0][0] = *pm[0][1] = *pm[0][2] = *pm[0][3] =
    *pm[1][0] = *pm[1][1] = *pm[1][2] = *pm[1][3] =
    *pm[2][0] = *pm[2][1] = *pm[2][2] = *pm[2][3] =
    *pm[3][0] = *pm[3][1] = *pm[3][2] = *pm[3][3] = 0;
}

// Return identity matrix
void MtxIdentity(PMTX pm) {
    ptr_check(pm, "MtxIdentity");
    *pm[0][1] = *pm[0][2] = *pm[0][3] =
    *pm[1][0] = *pm[1][2] = *pm[1][3] =
    *pm[2][0] = *pm[2][1] = *pm[2][3] =
    *pm[3][0] = *pm[3][1] = *pm[3][2] = 0;
    *pm[0][0] = *pm[1][1] = *pm[2][2] = *pm[3][3] = 1;
}

// Return x,y,z scaling matrix
void MtxScale(PMTX pm, double sx, double sy, double sz) {
    ptr_check(pm, "MtxIdentity");
    *pm[0][1] = *pm[0][2] = *pm[0][3] =
    *pm[1][0] = *pm[1][2] = *pm[1][3] =
    *pm[2][0] = *pm[2][1] = *pm[2][3] =
    *pm[3][0] = *pm[3][1] = *pm[3][2] = 0;
    *pm[0][0] = sx;
    *pm[1][1] = sy;
    *pm[2][2] = sz;
    *pm[3][3] = 1;
}

// Return x,y,z translate matrix
void MtxTranslate(PMTX pm, double tx, double ty, double tz) {
    MtxIdentity(pm);
    *pm[3][0] = tx;
    *pm[3][1] = ty;
    *pm[3][2] = tz;
}

// Return x rotate matrix
void MtxRotateX(PMTX pm, double angle) {
    ptr_check(pm, "MtxRotateX");
    double c = cos(angle);
    double s = sin(angle);
    *pm[0][0] = 1; *pm[0][1] = 0; *pm[0][2] =  0; *pm[0][3] = 0;
    *pm[1][0] = 0; *pm[1][1] = c; *pm[1][2] = -s; *pm[1][3] = 0;
    *pm[2][0] = 0; *pm[2][1] = s; *pm[2][2] =  c; *pm[2][3] = 0;
    *pm[3][0] = 0; *pm[3][1] = 0; *pm[3][2] =  0; *pm[3][3] = 1;
}

// Return y rotate matrix
void MtxRotateY(PMTX pm, double angle) {
    ptr_check(pm, "MtxRotateY");
    double c = cos(angle);
    double s = sin(angle);
    *pm[0][0] =  c; *pm[0][1] = 0; *pm[0][2] = s; *pm[0][3] = 0;
    *pm[1][0] =  0; *pm[1][1] = 1; *pm[1][2] = 0; *pm[1][3] = 0;
    *pm[2][0] = -s; *pm[2][1] = 0; *pm[2][2] = c; *pm[2][3] = 0;
    *pm[3][0] =  0; *pm[3][1] = 0; *pm[3][2] = 0; *pm[3][3] = 1;
}

// Return z rotate matrix
void MtxRotateZ(PMTX pm, double angle) {
    ptr_check(pm, "MtxRotateZ");
    double c = cos(angle);
    double s = sin(angle);
    *pm[0][0] = c; *pm[0][1] = -s; *pm[0][2] = 0; *pm[0][3] = 0;
    *pm[1][0] = s; *pm[1][1] =  c; *pm[1][2] = 0; *pm[1][3] = 0;
    *pm[2][0] = 0; *pm[2][1] =  0; *pm[2][2] = 1; *pm[2][3] = 0;
    *pm[3][0] = 0; *pm[3][1] =  0; *pm[3][2] = 0; *pm[3][3] = 1;
}

// Return x,y,z rotate matrix
void MtxRotate(PMTX pm, double ax, double ay, double az) {
    ptr_check(pm, "MtxRotate");
    double sx = sin(ax);
    double cx = cos(ax);
    double sy = sin(ay);
    double cy = cos(ay);
    double sz = sin(az);
    double cz = cos(az);
    *pm[0][0] = cy * cz;
    *pm[0][1] = sx * sy * cz - cx * sz;
    *pm[0][2] = cx * sy * cz + sx * sz;
    *pm[0][3] = 0;
    *pm[1][0] = cy * sz;
    *pm[1][1] = sx * sy * sz + cx * cz;
    *pm[1][2] = cx * sy * sz - sx * cz;
    *pm[1][3] = 0;
    *pm[2][0] = -sy;
    *pm[2][1] = sx * cy;
    *pm[2][2] = cx * cy;
    *pm[2][3] = 0;
    *pm[3][0] = 0;
    *pm[3][1] = 0;
    *pm[3][2] = 0;
    *pm[3][3] = 1;
}

// Return x,y,z rotate matrix (debug version)
void MtxRotateDebug(PMTX pm, double ax, double ay, double az) {
    ptr_check(pm, "MtxRotateDebug");
    Mtx ma; MtxRotateX(&ma, ax);
    Mtx mb; MtxRotateY(&mb, ay);
    Mtx mc; MtxRotateZ(&mc, az);
    MtxCat(&mb ,&ma, pm);
    MtxCat(&mc, pm, pm);
}

// Return quaternion matrix
void MtxQuat(PMTX pm, double x, double y, double z, double angle) {
    ptr_check(pm, "MtxQuat");
    double c = cos( angle );
    double s = sin( angle );
    double cc = 1 - c;
    Vec v;
    VecMake( &v, x, y, z );
    VecNormalize( &v );
    x = v[0];
    y = v[1];
    z = v[2];
    *pm[0][0] = (cc*x*x)+c;
    *pm[0][1] = (cc*x*y)+(z*s);
    *pm[0][2] = (cc*x*z)-(y*s);
    *pm[0][3] = 0;
    *pm[1][0] = (cc*x*y)-(z*s);
    *pm[1][1] = (cc*y*y)+c;
    *pm[1][2] = (cc*z*y)+(x*s);
    *pm[1][3] = 0;
    *pm[2][0] = (cc*x*z)+(y*s);
    *pm[2][1] = (cc*y*z)-(x*s);
    *pm[2][2] = (cc*z*z)+c;
    *pm[2][3] = 0;
    *pm[3][0] = 0;
    *pm[3][1] = 0;
    *pm[3][2] = 0;
    *pm[3][3] = 1;
}

// Return viewpoint matrix
void MtxLookat(PMTX pm, PVEC eye, PVEC target, PVEC up) {
    ptr_check(pm, "MtxLookat (pm)");
    ptr_check(eye, "MtxLookat (eye)");
    ptr_check(target, "MtxLookat (target)");
    ptr_check(up, "MtxLookat (up)");
    Vec x, y, z;
    VecSub(eye, target, &z);
    if (VecLenSqr( &z ) < TINY) {
      // eye and target are in the same position
      z[2] = 1;
    }
    VecNormalize( &z );
    VecCross( up, &z, &x);
    if (VecLenSqr( &x ) < TINY) {
      // eye and target are in the same vertical
      z[2] += 0.0001;
      VecCross( up, &z, &x);
    }
    VecNormalize( & x);
    VecCross( &z, &x, &y);
    *pm[0][0] = x[0];
    *pm[0][1] = x[1];
    *pm[0][2] = x[2];
    *pm[0][3] = 0;
    *pm[1][0] = y[0];
    *pm[1][1] = y[1];
    *pm[1][2] = y[2];
    *pm[1][3] = 0;
    *pm[2][0] = z[0];
    *pm[2][1] = z[1];
    *pm[2][2] = z[2];
    *pm[2][3] = 0;
    *pm[3][0] = 0;
    *pm[3][1] = 0;
    *pm[3][2] = 0;
    *pm[3][3] = 1;
}

// Return transformation matrix (alternate)
double MtxTransform(PMTX pm, 
    double ax, double ay, double az, 
    double mx, double my, double mz,
    double tx, double ty, double tz) {
    ptr_check(pm, "MtxTransform");
    double sx = sin(ax);
    double cx = cos(ax);
    double sy = sin(ay);
    double cy = cos(ay);
    double sz = sin(az);
    double cz = cos(az);
    *pm[0][0] = mx * (cy * cz);
    *pm[0][1] = my * (sx * sy * cz - cx * sz);
    *pm[0][2] = mz * (cx * sy * cz + sx * sz);
    *pm[0][3] = tx;
    *pm[1][0] = mx * (cy * sz);
    *pm[1][1] = my * (sx * sy * sz + cx * cz);
    *pm[1][2] = mz * (cx * sy * sz - sx * cz);
    *pm[1][3] = ty;
    *pm[2][0] = mx * (-sy);
    *pm[2][1] = my * (sx * cy);
    *pm[2][2] = mz * (cx * cy);
    *pm[2][3] = tz;
    *pm[3][0] = 0;
    *pm[3][1] = 0;
    *pm[3][2] = 0;
    *pm[3][3] = 1;
}

// Concatenate two matrices (a x b -> c)
void MtxCat(PMTX ma, PMTX mb, PMTX mc) {
    ptr_check(ma, "MtxCat (ma)");
    ptr_check(mb, "MtxCat (mb)");
    // ptr_check(mc, "MtxCat (mc)");
    Mtx m;
    MtxZero( &m );
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            for (int k=0; k<4; k++) {
                m[i][j] += (*ma)[i][k] * (*mb)[k][j];
            }
        }
    }
    MtxCopy( &m, mc );
}

// Return transpose matrix (a^t -> b)
void MtxTranspose (PMTX ma, PMTX mb) {
    ptr_check(ma, "MtxTranspose (ma)");
    ptr_check(mb, "MtxTranspose (mb)");
    *mb[0][0] = *ma[0][0];
    *mb[0][1] = *ma[1][0];
    *mb[0][2] = *ma[2][0];
    *mb[0][3] = *ma[3][0];
    *mb[1][0] = *ma[0][1];
    *mb[1][1] = *ma[1][1];
    *mb[1][2] = *ma[2][1];
    *mb[1][3] = *ma[3][1];
    *mb[2][0] = *ma[0][2];
    *mb[2][1] = *ma[1][2];
    *mb[2][2] = *ma[2][2];
    *mb[2][3] = *ma[3][2];
    *mb[3][0] = *ma[0][3];
    *mb[3][1] = *ma[1][3];
    *mb[3][2] = *ma[2][3];
    *mb[3][3] = *ma[3][3];
}

// Return transformed vector (m x a -> b)
void MtxTransVector(PMTX pm, PVEC va, PVEC vb) {
    ptr_check(pm, "MtxTransVector (pm)");
    ptr_check(va, "MtxTransVector (va)");
    Vec u;
    u[0] =  *pm[0][0] * (*va)[0] +
            *pm[0][1] * (*va)[1] +
            *pm[0][2] * (*va)[2] +
            *pm[0][3];
    u[1] =  *pm[1][0] * (*va)[0] +
            *pm[1][1] * (*va)[1] +
            *pm[1][2] * (*va)[2] +
            *pm[1][3];
    u[2] =  *pm[2][0] * (*va)[0] +
            *pm[2][1] * (*va)[1] +
            *pm[2][2] * (*va)[2] +
            *pm[2][3];
    VecS(1 / (*pm)[3][3], &u, vb);
    VecCopy( &u, vb );
}

// Return transformed normal vector (m ⟂ a -> b)
void MtxTransNormal(PMTX pm, PVEC va, PVEC vb) {
    ptr_check(va, "MtxTransNormal (va)");
    Vec orig  = { 0, 0, 0 };
    Vec t1    = { 1, 0, 0 };
    Vec t2    = { 0, 0, 0 };
    VecNormalize( va );
    // Create a t1 vector not aligned with va
    double dot = VecDot( &t1, va );
    if (ABS(dot) > 0.8) {
        VecMake( &t1, 0, 1, 0 );
    }
    VecCross(va, &t1, &t2);   // Create t2
    VecCross(&t2, va, &t1);   // Create proper t1
    // Transform tangents
    MtxTransVector( pm, &t1, &t1 );
    MtxTransVector( pm, &t2, &t2 );
    MtxTransVector( pm, &orig, &orig);
    // Revise axes
    VecSub(&t1, &orig, &t1);
    VecSub(&t2, &orig, &t2);
    // Recreate normal
    VecCross(&t1, &t2, vb);
}

// Return inverse matrix (a^-1 => b)
void MtxInverse(PMTX ma, PMTX mb) {

    #define SIZE 4

	Mtx     y;
	int     i, j, indx[4];
	double  d, col[4];

	MtxCopy(ma, mb);                    // save original matrix
	ludcmp(*mb, indx, &d);              // matrix lu decomposition

	for (int j = 0; j < SIZE; j++) {    // matrix inversion
		for (int i = 0; i < SIZE; i++) {
			col[i] = 0.0;
		}
		col[j] = 1.0;
		lubksb(*mb, indx, col);
		for (int i = 0;i < SIZE; i++) {
			y[i][j] = col[i];
		}
	}
    
	MtxCopy(&y, mb);

    #undef SIZE

}

/* ---------------------- Helper Functions ---------------------- */

//
// LU backward substitution
// a			input matrix
// indx			row permutation record
// b			right hand vector
//
static void lubksb(Mtx a, int* indx, double* b) {

    #define SIZE 4

	int     i, j, ii=-1, ip;
	double   sum;

	for (i = 0;i < SIZE;i++) {
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii>=0) {
			for (j = ii;j <= i-1;j++) {
				sum -= a[i][j] * b[j];
			}
		}
		else if (sum != 0.0) {
			ii = i;
		}
		b[i] = sum;
	}
	for (i = SIZE-1;i >= 0;i--) {
		sum = b[i];
		for (j = i+1;j < SIZE;j++) {
			sum -= a[i][j] * b[j];
		}
		b[i] = sum/a[i][i];
	}

    #undef SIZE

}

//
// LU decomposition
//	a           input matrix. gets thrashed
//	indx        row permutation record
//	d           +/- 1.0 (even or odd # of row interchanges
//
static void ludcmp(Mtx a, int* indx, double* d) {

    #define SIZE 4

	double  vv[SIZE];               // implicit scale for each row
	double  big, dum, sum, tmp;
	int     i, imax, j, k;

	*d = 1.0;
	for (i = 0;i < SIZE;i++) {
		big = 0.0;
		for (j = 0;j < SIZE;j++) {
			if ((tmp = fabs(a[i][j])) > big) {
				big = tmp;
			}
		}
		if (big == 0.0) {
			last_call("Singular matrix", 2);
		}
		vv[i] = 1.0/big;
	}
	for (j = 0;j < SIZE;j++) {
		for (i = 0;i < j;i++) {
			sum = a[i][j];
			for (k = 0;k < i;k++) {
				sum -= a[i][k] * a[k][j];
			}
			a[i][j] = sum;
		}
		big = 0.0;
		for (i = j;i < SIZE;i++) {
			sum = a[i][j];
			for (k = 0;k < j;k++) {
				sum -= a[i][k]*a[k][j];
			}
			a[i][j] = sum;
			if ((dum = vv[i] * fabs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k = 0;k < SIZE;k++) {
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (a[j][j] == 0.0) {
			a[j][j] = 1.0e-20;      // can be 0.0 also...
		}
		if (j != SIZE-1) {
			dum = 1.0/a[j][j];
			for (i = j+1;i < SIZE;i++) {
				a[i][j] *= dum;
			}
		}
	}

    #undef SIZE

}

