
#ifndef MTX_H_DEFINED
#define MTX_H_DEFINED


#define PI 		(3.14159265358979323844)
#define D2R		(PI/180)
#define R2D		(180/PI)

#define PTR_CHECK(p) ptrchk(p,__FUNCTION__)


/*----------------------------------------------------------------------*/

// Math Macros

#define ABS(a)		(((a)<0)?(-(a)):(a))
#define SGN(a)      ((a)<0?(-1):(1))
#define SGNZ(a)     ((a)<0?(-1):((a)?(1):(0)))
#define FLOOR(a)	((a)>0?(int)(a):-(int)(a))
#define CEILING(a)	((a)==(int)(a)?(a):(a)>0?1+(int)(a):-(1+(int)(-a))
#define ROUND(a)	((a)>0?(int)((a)+0.5):-(int)(0.5-a))

#define MIN(a,b)	(((a)<(b))?(a):(b))
#define MAX(a,b)	(((a)>(b))?(a):(b))
#define MID(a,b,c)  (((a)<(b))?(b):MIN(a,c))

// Random number [0.0 .. 1.0]
#define RND()			(((double) rand())/RAND_MAX)

// Random number [0.0 .. s]
#define SRND(s)			((s)*RND())

// Random number [b .. a+b]
#define ABRND(a,b)		(((a)*RND())+(b))

// Convert degrees to radians
#define DEG2RAD(x)     (((Flt)(x))*D2R)

// Convert radians to degrees
#define RAD2DEG(x)     (((Flt)(x))*R2D)

#define MakeVector(x, y, z, v)		\
	(v)[0]=(x);						\
	(v)[1]=(y);						\
	(v)[2]=(z)

#define VecNegate(a)	\
	(a)[0]=(-(a)[0]);	\
	(a)[1]=(-(a)[1]);	\
	(a)[2]=(-(a)[2])

#define VecDot(a,b)		\
	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

#define VecLen(a)		\
	(sqrt(VecDot(a,a)))

#define VecLenSqr(a)	\
	(VecDot(a,a))

#define VecCopy(a,b)	\
	(b)[0]=(a)[0];		\
	(b)[1]=(a)[1];		\
	(b)[2]=(a)[2]

#define VecAdd(a,b,c)		\
	(c)[0]=(a)[0]+(b)[0];	\
	(c)[1]=(a)[1]+(b)[1];	\
	(c)[2]=(a)[2]+(b)[2]

#define VecSub(a,b,c)		\
	(c)[0]=(a)[0]-(b)[0];	\
	(c)[1]=(a)[1]-(b)[1];	\
	(c)[2]=(a)[2]-(b)[2]

#define VecComb(A,a,B,b,c)			\
	(c)[0]=(A)*(a)[0]+(B)*(b)[0];	\
	(c)[1]=(A)*(a)[1]+(B)*(b)[1];	\
	(c)[2]=(A)*(a)[2]+(B)*(b)[2]

#define VecS(A,a,b)		\
	(b)[0]=(A)*(a)[0];	\
	(b)[1]=(A)*(a)[1];	\
	(b)[2]=(A)*(a)[2]

#define VecAddS(A,a,b,c)		\
	(c)[0]=(A)*(a)[0]+(b)[0];	\
	(c)[1]=(A)*(a)[1]+(b)[1];	\
	(c)[2]=(A)*(a)[2]+(b)[2]

#define VecMul(a,b,c)		\
	(c)[0]=(a)[0]*(b)[0];	\
	(c)[1]=(a)[1]*(b)[1];	\
	(c)[2]=(a)[2]*(b)[2]

#define VecCross(a,b,c)						\
	(c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1];		\
	(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2];		\
	(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0]

#define VecZero(v)		\
	(v)[0]=0.0;			\
	(v)[1]=0.0;			\
	(v)[2]=0.0

#define VecPrint(msg,v)		\
	fprintf(stderr,			\
	"%s: %g %g %g\n", msg,	\
	(v)[0],(v)[1],(v)[2])


typedef double Flt;
typedef Flt	Vec[3];
typedef Vec	Point;
typedef Vec	Color;
typedef Flt	Matrix[4][4];


extern void InitIdentityMat(Matrix mout);
extern void InitTranslateMat(Vec tv, Matrix mout);
extern void InitScaleMat(Vec sv, Matrix mout);
extern void InitRotateMat(Vec rv, Matrix mout);
extern void InitPatchMat(Vec a, Vec b, Vec c, Matrix mout);
extern void InitQuatMat(Vec v, Flt r, Matrix mout);
extern void InitViewMat(Vec eye, Vec at, Vec up, Matrix mout);
extern void TranslateMat(Vec tv, Matrix min, Matrix mout);
extern void ScaleMat(Vec sv, Matrix min, Matrix mout);
extern void RotateMat(Vec rv, Matrix min, Matrix mout);
extern void InverseMat(Matrix min, Matrix mout);
extern void CopyMat(Matrix min, Matrix mout);
extern void CatMat(Matrix ma, Matrix mb, Matrix mout);
extern void ApplyMat(Matrix mtx, Vec pin, Vec pout);
extern void ApplyNormalMat(Matrix mtx, Vec pin, Vec pout);


#endif // !MTX_H_DEFINED
