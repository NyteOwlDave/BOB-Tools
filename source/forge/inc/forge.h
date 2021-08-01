
/*

    forge.h

    Shape forging math functions

*/

#ifndef FORGE_DEFINED
#define FORGE_DEFINED


#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Windows-like types
typedef int BOOL;
typedef void* PVOID;
typedef const void* PCVOID;
typedef char* PSTR;
typedef const char* PCSTR;
typedef unsigned long QWORD;
typedef QWORD* PQWORD;
typedef const QWORD* PCQWORD;
typedef unsigned int DWORD;
typedef DWORD* PDWORD;
typedef const DWORD* PCDWORD;
typedef unsigned short WORD;
typedef WORD* PWORD;
typedef const WORD* PCWORD;
typedef unsigned char BYTE;
typedef BYTE* PBYTE;
typedef const BYTE* PCBYTE;


// Vector structure
typedef double Vec[3];
typedef Vec* PVEC;

// Matrix structure
typedef double Mtx[4][4];
typedef Mtx* PMTX;

// Transform structure
typedef struct _transform_t {
    struct _transform_t* next;
    Mtx matrix;
} transform_t;
typedef transform_t* PTRANSFORM;

// Point structure
typedef struct _point_t {
    double x;
    double y;
    double z;
} point_t;
typedef point_t *PPOINT;

// Patch structure
typedef struct _patch_t {
    point_t tri_p[3];
    point_t tri_n[3];
    point_t normal;
} patch_t;
typedef patch_t* PPATCH;

// Ring structure
typedef struct _ring_t {
    point_t center;
    point_t normal;
    double max_radius;
    double min_radius;
} ring_t;
typedef ring_t* PRING;

// Cone structure
typedef struct _cone_t {
    point_t apex;
    point_t base;
    point_t normal;
    double apex_radius;
    double base_radius;
} cone_t;
typedef cone_t* PCONE;

// Sphere structure
typedef struct _sphere_t {
    point_t center;
    point_t normal;
    double radius;
    double fuzz;
} sphere_t;
typedef sphere_t* PSPHERE;

// Polygon structure
typedef struct _polygon_t {
    int points;
    point_t normal;
    point_t vtx[];
} polygon_t;
typedef polygon_t* PPOLYGON;


// Constants
#define FALSE 0
#define TRUE 1
#define TINY 1e-8
#define HUGE 1e+8
#define PI  3.1415926535897932
#define D2R (PI/180)
#define R2D (180/PI)
#define MAX_PATH 260
#define MAX_TOKEN 80

// Angle conversions
#define DEG2RAD(n) ((n)*D2R)
#define RAD2DEG(n) ((n)*R2D)

// Random number [0 ... 1]
#define RND() (rand()/RAND_MAX)

// Random number [0 ... n]
#define SRND(n) (n)*rnd())

// Random number [-n/2 ... +n/2]
#define CRND(n) (srnd(n)-((n)*0.5))

// Random number [-PI/2 ... +PI/2]
#define ARND() (crnd(PI));

// Absolute value
#define ABS(a)  (((a)<0)?(-(a)):(a))

// Sign [-1, +1]
#define SGN(a)  (((a)<0)?(-1):(1))

// Sign [-1, 0, +1]
#define SGNZ(a) (((a)<0)?(-1):(((a)>0)?(1):(0)))

// Minimum
#define MIN(a,b)    (((a)<(b))?(a):(b))

// Maximum
#define MAX(a,b)    (((a)<(b))?(b):(a))

// Median
#define MID (a,b,c) (((a)<=(b))?(((b)<=c)?(b):MAX(a,c)):(((a)<=(c))?(a):MAX(b,c)))


// Allocate memory
extern PVOID vmalloc(size_t size);

// Handle fatal errir
extern void last_call(PCSTR msg, int retcode);

// Pointer check
extern void ptr_check(PCVOID ptr, PCSTR msg);

// Invokes the command shell and passes in a command line
extern void Shell(PCSTR command);

// Invokes the command shell's DIR command
extern void Dir(PCSTR path);

// Invokes the command shell's CLS command
extern void Cls();

// Invokes the command shell's PAUSE command
extern void Pause();

// Verifies that a file exists
extern BOOL FileExists(PCSTR szPath);

// Verifies that a folder exists
extern BOOL FolderExists(PCSTR szPath);

// Displays the contents of a text file (stderr)
extern void ListFile(PCSTR path);

// Routines to print various common data types (stderr)
extern void Print(PCSTR msg);
extern void PrintFlag(PCSTR msg, BOOL flag);
extern void PrintText(PCSTR msg, PCSTR txt);
extern void PrintTextArray(PCSTR msg, PCSTR* pa);
extern void PrintInteger(PCSTR msg, int num);
extern void PrintFloat(PCSTR msg, double num);
extern void PrintHex(PCSTR msg, DWORD num);
extern void PrintVector(PCSTR msg, Vec v);
extern void PrintMatrix(PCSTR msg, Mtx m);
extern void PrintAddress(PCSTR msg, const void* p);


/* ---------------------- Scalar Functions ---------------------- */

// Gamma
extern double gamma_correct(double n, double nMax, double gamma);

// Linear interpolation
extern double lerp(double n,double a, double b);

/* ---------------------- Vector Functions ---------------------- */

extern void VecZero(PVEC v);
extern void VecMake(PVEC v, double x, double y, double z);
extern PVEC VecCreate(double x, double y, double z);
extern PVEC VecClone(PVEC vin);
extern void VecNegate(PVEC v);
extern double VecDot(PVEC va, PVEC vb);
extern void VecCross(PVEC va, PVEC vb, PVEC vc);
extern double VecLenSqr(PVEC v);
extern double VecLen(PVEC v);
extern void VecCopy(PVEC va, PVEC vb);
extern void VecAdd(PVEC va, PVEC vb, PVEC vc);
extern void VecSub(PVEC va, PVEC vb, PVEC vc);
extern void VecMul(PVEC va, PVEC vb, PVEC vc);
extern void VecComb(double a, PVEC va, double b, PVEC vb, PVEC vc);
extern void VecS(double a, PVEC va, PVEC vb);
extern void VecAddS(double a, PVEC va, PVEC vb, PVEC vc);
extern double VecNormal (PVEC va, PVEC vb);
extern double VecNormalize(PVEC v);
extern void VecMidpoint2(PVEC va, PVEC vb, PVEC vc);
extern void VecMidpoint3(PVEC va, PVEC vb, PVEC vc, PVEC vd);
extern double VecManhatten(PVEC v);

/* ---------------------- Matrix Functions ---------------------- */

extern PMTX MtxCreate(void);
extern PMTX MtxClone(PMTX pm);
extern void MtxCopy(PMTX ma, PMTX mb);
extern void MtxZero(PMTX pm);
extern void MtxIdentity(PMTX pm);
extern void MtxScale(PMTX pm, double sx, double sy, double sz);
extern void MtxTranslate(PMTX pm, double tx, double ty, double tz);
extern void MtxRotateX(PMTX pm, double angle);
extern void MtxRotateY(PMTX pm, double angle);
extern void MtxRotateZ(PMTX pm, double angle);
extern void MtxRotate(PMTX pm, double ax, double ay, double az);
extern void MtxRotateDebug(PMTX pm, double ax, double ay, double az);
extern void MtxQuat(PMTX pm, double x, double y, double z, double angle);
extern void MtxLookat(PMTX pm, PVEC eye, PVEC target, PVEC up);
extern double MtxTransform(PMTX pm, 
    double ax, double ay, double az, 
    double mx, double my, double mz,
    double tx, double ty, double tz);
extern void MtxCat(PMTX ma, PMTX mb, PMTX mc);
extern void MtxTranspose (PMTX ma, PMTX mb);
extern void MtxInverse(PMTX ma, PMTX mb);
extern void MtxTransVector(PMTX pm, PVEC va, PVEC vb);
extern void MtxTransNormal(PMTX pm, PVEC va, PVEC vb);

/* ---------------------- Point Functions ---------------------- */

extern PPOINT PointCreate(void);
extern void PointToVec(PPOINT ppoint, PVEC pv);
extern void PointFromVec(PVEC pv, PPOINT ppoint);
extern double PointNormalize(PPOINT ppoint);

/* ---------------------- Patch Functions ---------------------- */

extern PPATCH PatchCreate(void);
extern void PatchCalcNormal(PPATCH ppatch);
extern void PatchCompose(PPATCH ppatch);

/* ---------------------- Polygon Functions ---------------------- */

extern PPOLYGON PolyCreate(int num_verts);
extern void PolyCalcNormal(PPOLYGON ppoly);
extern void PolyCompose(PPOLYGON ppoly);

/* ---------------------- Ring Functions ---------------------- */

extern PRING RingCreate(void);
extern void RingCalcNormal(PRING pring);
extern void RingCompose(PRING pring);

/* ---------------------- Cone Functions ---------------------- */

extern PCONE ConeCreate(void);
extern void ConeCalcNormal(PCONE pcone);
extern void ConeCompose(PCONE pcone);

/* ---------------------- Sphere Functions ---------------------- */

extern PSPHERE SphereCreate(void);
extern void SphereCalcNormal(PSPHERE pcone);
extern void SphereCompose(PSPHERE psphere);


#endif // !FORGE_DEFINED

