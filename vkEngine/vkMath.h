#ifndef VKMATH_H
#define VKMATH_H

#include <math.h>
#include <string.h>
#include <assert.h>

const float PI = 3.14159265359f;

static const float fIdentity[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

typedef struct
{
    union{
        float x;
        float u;
    };
    union{
        float y;
        float v;
    };
} VEC2;

typedef struct
{
    float x;	/*!< x coordinate */
    float y;	/*!< y coordinate */
    float z;	/*!< z coordinate */
} VEC3;

typedef struct
{
    float x;	/*!< x coordinate */
    float y;	/*!< y coordinate */
    float z;	/*!< z coordinate */
    float w;	/*!< w coordinate */
} VEC4;

class MATRIX
{
public:
    float* operator [] ( const int Row ) { return &f[Row*4]; }
    MATRIX operator*(MATRIX const& m) { matrixMultiply(*this, m); return *this;}
    VEC4 operator*(VEC4 const& v) { return vectorMultiply(v);}
    float f[16];

public:

void matrixIdentity()
{
    this->f[ 0]=1.0f;	this->f[ 4]=0.0f;	this->f[ 8]=0.0f;	this->f[12]=0.0f;
    this->f[ 1]=0.0f;	this->f[ 5]=1.0f;	this->f[ 9]=0.0f;	this->f[13]=0.0f;
    this->f[ 2]=0.0f;	this->f[ 6]=0.0f;	this->f[10]=1.0f;	this->f[14]=0.0f;
    this->f[ 3]=0.0f;	this->f[ 7]=0.0f;	this->f[11]=0.0f;	this->f[15]=1.0f;
};

void matrixMultiply(const MATRIX &mA, const MATRIX &mB)
{
    MATRIX mOut;

    /* Perform calculation on a dummy matrix (mRet) */
    mOut.f[ 0] = mA.f[ 0]*mB.f[ 0] + mA.f[ 1]*mB.f[ 4] + mA.f[ 2]*mB.f[ 8] + mA.f[ 3]*mB.f[12];
    mOut.f[ 1] = mA.f[ 0]*mB.f[ 1] + mA.f[ 1]*mB.f[ 5] + mA.f[ 2]*mB.f[ 9] + mA.f[ 3]*mB.f[13];
    mOut.f[ 2] = mA.f[ 0]*mB.f[ 2] + mA.f[ 1]*mB.f[ 6] + mA.f[ 2]*mB.f[10] + mA.f[ 3]*mB.f[14];
    mOut.f[ 3] = mA.f[ 0]*mB.f[ 3] + mA.f[ 1]*mB.f[ 7] + mA.f[ 2]*mB.f[11] + mA.f[ 3]*mB.f[15];

    mOut.f[ 4] = mA.f[ 4]*mB.f[ 0] + mA.f[ 5]*mB.f[ 4] + mA.f[ 6]*mB.f[ 8] + mA.f[ 7]*mB.f[12];
    mOut.f[ 5] = mA.f[ 4]*mB.f[ 1] + mA.f[ 5]*mB.f[ 5] + mA.f[ 6]*mB.f[ 9] + mA.f[ 7]*mB.f[13];
    mOut.f[ 6] = mA.f[ 4]*mB.f[ 2] + mA.f[ 5]*mB.f[ 6] + mA.f[ 6]*mB.f[10] + mA.f[ 7]*mB.f[14];
    mOut.f[ 7] = mA.f[ 4]*mB.f[ 3] + mA.f[ 5]*mB.f[ 7] + mA.f[ 6]*mB.f[11] + mA.f[ 7]*mB.f[15];

    mOut.f[ 8] = mA.f[ 8]*mB.f[ 0] + mA.f[ 9]*mB.f[ 4] + mA.f[10]*mB.f[ 8] + mA.f[11]*mB.f[12];
    mOut.f[ 9] = mA.f[ 8]*mB.f[ 1] + mA.f[ 9]*mB.f[ 5] + mA.f[10]*mB.f[ 9] + mA.f[11]*mB.f[13];
    mOut.f[10] = mA.f[ 8]*mB.f[ 2] + mA.f[ 9]*mB.f[ 6] + mA.f[10]*mB.f[10] + mA.f[11]*mB.f[14];
    mOut.f[11] = mA.f[ 8]*mB.f[ 3] + mA.f[ 9]*mB.f[ 7] + mA.f[10]*mB.f[11] + mA.f[11]*mB.f[15];

    mOut.f[12] = mA.f[12]*mB.f[ 0] + mA.f[13]*mB.f[ 4] + mA.f[14]*mB.f[ 8] + mA.f[15]*mB.f[12];
    mOut.f[13] = mA.f[12]*mB.f[ 1] + mA.f[13]*mB.f[ 5] + mA.f[14]*mB.f[ 9] + mA.f[15]*mB.f[13];
    mOut.f[14] = mA.f[12]*mB.f[ 2] + mA.f[13]*mB.f[ 6] + mA.f[14]*mB.f[10] + mA.f[15]*mB.f[14];
    mOut.f[15] = mA.f[12]*mB.f[ 3] + mA.f[13]*mB.f[ 7] + mA.f[14]*mB.f[11] + mA.f[15]*mB.f[15];

    memcpy(this->f, mOut.f, sizeof(mOut.f));
};

VEC4 vectorMultiply(const VEC4 &mA)
{
    VEC4 mRet;

    /* Perform calculation on a dummy matrix (mRet) */
    mRet.x = mA.x*this->f[ 0] + mA.y*this->f[ 4] + mA.z*this->f[ 8] + mA.w*this->f[12];
    mRet.y = mA.x*this->f[ 1] + mA.y*this->f[ 5] + mA.z*this->f[ 9] + mA.w*this->f[13];
    mRet.z = mA.x*this->f[ 2] + mA.y*this->f[ 6] + mA.z*this->f[10] + mA.w*this->f[14];
    mRet.w = mA.x*this->f[ 3] + mA.y*this->f[ 7] + mA.z*this->f[11] + mA.w*this->f[15];

    return mRet;
};

void matrixTranslation(const float fX, const float fY, const float fZ)
{
    this->f[ 0]=1.0f;	this->f[ 4]=0.0f;	this->f[ 8]=0.0f;	this->f[12]=fX;
    this->f[ 1]=0.0f;	this->f[ 5]=1.0f;	this->f[ 9]=0.0f;	this->f[13]=fY;
    this->f[ 2]=0.0f;	this->f[ 6]=0.0f;	this->f[10]=1.0f;	this->f[14]=fZ;
    this->f[ 3]=0.0f;	this->f[ 7]=0.0f;	this->f[11]=0.0f;	this->f[15]=1.0f;
};

void matrixScaling(const float fX, const float fY, const float fZ)
{
    this->f[ 0]=fX;		this->f[ 4]=0.0f;	this->f[ 8]=0.0f;	this->f[12]=0.0f;
    this->f[ 1]=0.0f;	this->f[ 5]=fY;		this->f[ 9]=0.0f;	this->f[13]=0.0f;
    this->f[ 2]=0.0f;	this->f[ 6]=0.0f;	this->f[10]=fZ;		this->f[14]=0.0f;
    this->f[ 3]=0.0f;	this->f[ 7]=0.0f;	this->f[11]=0.0f;	this->f[15]=1.0f;
};

void matrixRotationQ(VEC4 &quaternion)
{
    // GLTF XYZW order (Blender is WXYZ)
    // Disable Y+ Up when exporting from Blender as Blender uses Z+ UP
    float qX = quaternion.x;
    float qY = quaternion.y;
    float qZ = quaternion.z;
    float qW = quaternion.w;

    this->f[ 0] = (1 - (2 * qY * qY) - (2 * qZ * qZ));
    this->f[ 4] = ((2 * qX * qY) - (2 * qZ * qW));
    this->f[ 8] = ((2* qX * qZ) + (2 * qY * qW));
    this->f[12] = 0.0f;

    this->f[ 1] = ((2 * qX * qY) + (2 * qZ * qW));
    this->f[ 5] = (1 - (2 * qX * qX) - (2 * qZ *qZ));
    this->f[ 9] = ((2 * qY * qZ) - (2 * qX * qW));
    this->f[13] = 0.0f;

    this->f[ 2] = ((2 * qX * qZ) - (2 * qY * qW));
    this->f[ 6] = ((2 * qY * qZ) + (2 * qX * qW));
    this->f[10] = (1 - (2 * qX * qX) - (2 * qY * qY));
    this->f[14] = 0.0f;

    this->f[3] = 0.0f;
    this->f[7] = 0.0f;
    this->f[11] = 0.0f;
    this->f[15] = 1.0f;
};

void matrixRotationX(const float fAngle)
{
    float		fCosine, fSine;

    /* Precompute cos and sin */
#if defined(BUILD_DX)
    fCosine	= (float)cos(-fAngle);
    fSine	= (float)sin(-fAngle);
#else
    fCosine	= (float)cos(fAngle);
    fSine	= (float)sin(fAngle);
#endif

    /* Create the trigonometric matrix corresponding to X Rotation */
    this->f[ 0]=1.0f;	this->f[ 4]=0.0f;	this->f[ 8]=0.0f;	this->f[12]=0.0f;
    this->f[ 1]=0.0f;	this->f[ 5]=fCosine;this->f[ 9]=fSine;	this->f[13]=0.0f;
    this->f[ 2]=0.0f;	this->f[ 6]=-fSine;	this->f[10]=fCosine;this->f[14]=0.0f;
    this->f[ 3]=0.0f;	this->f[ 7]=0.0f;	this->f[11]=0.0f;	this->f[15]=1.0f;
};

void matrixRotationY(const float fAngle)
{
    float		fCosine, fSine;

    /* Precompute cos and sin */
#if defined(BUILD_DX)
    fCosine	= (float)cos(-fAngle);
    fSine	= (float)sin(-fAngle);
#else
    fCosine	= (float)cos(fAngle);
    fSine	= (float)sin(fAngle);
#endif

    /* Create the trigonometric matrix corresponding to Y Rotation */
    this->f[ 0]=fCosine;	this->f[ 4]=0.0f;	this->f[ 8]=-fSine;		this->f[12]=0.0f;
    this->f[ 1]=0.0f;		this->f[ 5]=1.0f;	this->f[ 9]=0.0f;		this->f[13]=0.0f;
    this->f[ 2]=fSine;		this->f[ 6]=0.0f;	this->f[10]=fCosine;	this->f[14]=0.0f;
    this->f[ 3]=0.0f;		this->f[ 7]=0.0f;	this->f[11]=0.0f;		this->f[15]=1.0f;
};

void matrixRotationZ(const float fAngle)
{
    float		fCosine, fSine;

    /* Precompute cos and sin */
#if defined(BUILD_DX)
    fCosine =	(float)cos(-fAngle);
    fSine =		(float)sin(-fAngle);
#else
    fCosine =	(float)cos(fAngle);
    fSine =		(float)sin(fAngle);
#endif

    /* Create the trigonometric matrix corresponding to Z Rotation */
    this->f[ 0]=fCosine;	this->f[ 4]=fSine;	this->f[ 8]=0.0f;	this->f[12]=0.0f;
    this->f[ 1]=-fSine;		this->f[ 5]=fCosine;this->f[ 9]=0.0f;	this->f[13]=0.0f;
    this->f[ 2]=0.0f;		this->f[ 6]=0.0f;	this->f[10]=1.0f;	this->f[14]=0.0f;
    this->f[ 3]=0.0f;		this->f[ 7]=0.0f;	this->f[11]=0.0f;	this->f[15]=1.0f;
};

void matrixTranspose(const MATRIX &mIn)
{
    MATRIX mOut;

    mOut.f[ 0]=mIn.f[ 0];	mOut.f[ 4]=mIn.f[ 1];	mOut.f[ 8]=mIn.f[ 2];	mOut.f[12]=mIn.f[ 3];
    mOut.f[ 1]=mIn.f[ 4];	mOut.f[ 5]=mIn.f[ 5];	mOut.f[ 9]=mIn.f[ 6];	mOut.f[13]=mIn.f[ 7];
    mOut.f[ 2]=mIn.f[ 8];	mOut.f[ 6]=mIn.f[ 9];	mOut.f[10]=mIn.f[10];	mOut.f[14]=mIn.f[11];
    mOut.f[ 3]=mIn.f[12];	mOut.f[ 7]=mIn.f[13];	mOut.f[11]=mIn.f[14];	mOut.f[15]=mIn.f[15];

    memcpy(this->f, mOut.f, sizeof(mOut.f));
};

void matrixInverse(const MATRIX &mIn)
{
    MATRIX mOut;
    double		det_1;
    double		pos, neg, temp;

    /* Calculate the determinant of submatrix A and determine if the
       the matrix is singular as limited by the double precision
       floating-point data representation. */
    pos = neg = 0.0;
    temp =  mIn.f[ 0] * mIn.f[ 5] * mIn.f[10];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp =  mIn.f[ 4] * mIn.f[ 9] * mIn.f[ 2];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp =  mIn.f[ 8] * mIn.f[ 1] * mIn.f[ 6];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp = -mIn.f[ 8] * mIn.f[ 5] * mIn.f[ 2];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp = -mIn.f[ 4] * mIn.f[ 1] * mIn.f[10];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp = -mIn.f[ 0] * mIn.f[ 9] * mIn.f[ 6];
    if (temp >= 0.0) pos += temp; else neg += temp;
    det_1 = pos + neg;

    /* Is the submatrix A singular? */
    if ((det_1 == 0.0) || (fabs(det_1 / (pos - neg)) < 1.0e-15))
    {
        return;
    }
    else
    {
        /* Calculate inverse(A) = adj(A) / det(A) */
        det_1 = 1.0 / det_1;
        mOut.f[ 0] =   ( mIn.f[ 5] * mIn.f[10] - mIn.f[ 9] * mIn.f[ 6] ) * (float)det_1;
        mOut.f[ 1] = - ( mIn.f[ 1] * mIn.f[10] - mIn.f[ 9] * mIn.f[ 2] ) * (float)det_1;
        mOut.f[ 2] =   ( mIn.f[ 1] * mIn.f[ 6] - mIn.f[ 5] * mIn.f[ 2] ) * (float)det_1;
        mOut.f[ 4] = - ( mIn.f[ 4] * mIn.f[10] - mIn.f[ 8] * mIn.f[ 6] ) * (float)det_1;
        mOut.f[ 5] =   ( mIn.f[ 0] * mIn.f[10] - mIn.f[ 8] * mIn.f[ 2] ) * (float)det_1;
        mOut.f[ 6] = - ( mIn.f[ 0] * mIn.f[ 6] - mIn.f[ 4] * mIn.f[ 2] ) * (float)det_1;
        mOut.f[ 8] =   ( mIn.f[ 4] * mIn.f[ 9] - mIn.f[ 8] * mIn.f[ 5] ) * (float)det_1;
        mOut.f[ 9] = - ( mIn.f[ 0] * mIn.f[ 9] - mIn.f[ 8] * mIn.f[ 1] ) * (float)det_1;
        mOut.f[10] =   ( mIn.f[ 0] * mIn.f[ 5] - mIn.f[ 4] * mIn.f[ 1] ) * (float)det_1;

        /* Calculate -C * inverse(A) */
        mOut.f[12] = - ( mIn.f[12] * mOut.f[ 0] + mIn.f[13] * mOut.f[ 4] + mIn.f[14] * mOut.f[ 8] );
        mOut.f[13] = - ( mIn.f[12] * mOut.f[ 1] + mIn.f[13] * mOut.f[ 5] + mIn.f[14] * mOut.f[ 9] );
        mOut.f[14] = - ( mIn.f[12] * mOut.f[ 2] + mIn.f[13] * mOut.f[ 6] + mIn.f[14] * mOut.f[10] );

        /* Fill in last row */
        mOut.f[ 3] = 0.0f;
        mOut.f[ 7] = 0.0f;
        mOut.f[11] = 0.0f;
        mOut.f[15] = 1.0f;
    }

    memcpy(this->f, mOut.f, sizeof(mOut.f));
};

VEC3 matrixNormalize(const VEC3 &vIn)
{
    VEC3 vOut;
    float	f;
    double temp;

    temp = (double)(vIn.x * vIn.x + vIn.y * vIn.y + vIn.z * vIn.z);
    if(temp!=0.0f)
    {
        temp = 1.0 / sqrt(temp);
        f = (float)temp;
    }
    else
    {
        f = 0.0f;
    }

    vOut.x = vIn.x * f;
    vOut.y = vIn.y * f;
    vOut.z = vIn.z * f;

    return vOut;
};


VEC3 matrixCrossProduct(const VEC3 &v1, const VEC3 &v2)
{
    VEC3 vOut;

    vOut.x = v1.y * v2.z - v1.z * v2.y;
    vOut.y = v1.z * v2.x - v1.x * v2.z;
    vOut.z = v1.x * v2.y - v1.y * v2.x;

    return vOut;
};

void matrixLookAtLH(const VEC3 &vEye, const VEC3 &vAt, const VEC3 &vUp)
{
    VEC3 f, s, u;
    MATRIX	t;

    f.x = vEye.x - vAt.x;
    f.y = vEye.y - vAt.y;
    f.z = vEye.z - vAt.z;

    f = matrixNormalize(f);
    s = matrixCrossProduct(f, vUp);
    s = matrixNormalize(s);
    u = matrixCrossProduct(s, f);
    u = matrixNormalize(u);

    this->f[ 0] = s.x;
    this->f[ 1] = u.x;
    this->f[ 2] = -f.x;
    this->f[ 3] = 0;

    this->f[ 4] = s.y;
    this->f[ 5] = u.y;
    this->f[ 6] = -f.y;
    this->f[ 7] = 0;

    this->f[ 8] = s.z;
    this->f[ 9] = u.z;
    this->f[10] = -f.z;
    this->f[11] = 0;

    this->f[12] = 0;
    this->f[13] = 0;
    this->f[14] = 0;
    this->f[15] = 1;

    t.matrixTranslation(-vEye.x, -vEye.y, -vEye.z);
    this->matrixMultiply(t, *this);
};

void matrixLookAtRH(const VEC3 &vEye, const VEC3 &vAt, const VEC3 &vUp)
{
    VEC3 f, s, u;
    MATRIX	t, p;

    f.x = vAt.x - vEye.x;
    f.y = vAt.y - vEye.y;
    f.z = vAt.z - vEye.z;

    f = matrixNormalize(f);
    s = matrixCrossProduct(f, vUp);
    s = matrixNormalize(s);
    u = matrixCrossProduct(s, f);
    u = matrixNormalize(u);

    p.f[ 0] = s.x;
    p.f[ 1] = u.x;
    p.f[ 2] = -f.x;
    p.f[ 3] = 0;

    p.f[ 4] = s.y;
    p.f[ 5] = u.y;
    p.f[ 6] = -f.y;
    p.f[ 7] = 0;

    p.f[ 8] = s.z;
    p.f[ 9] = u.z;
    p.f[10] = -f.z;
    p.f[11] = 0;

    p.f[12] = 0;
    p.f[13] = 0;
    p.f[14] = 0;
    p.f[15] = 1;

    t.matrixTranslation(-vEye.x, -vEye.y, -vEye.z);
    matrixMultiply(t, p);
};

void matrixVRotate ()
{
    MATRIX mRotation;
    mRotation.matrixRotationZ(-90.0f*PI/180.0f);
    this->matrixMultiply(*this, mRotation);
}

void matrixPerspectiveFovLH(const float fFOVy, const float fAspect, const float fNear, const float fFar, const bool bRotate)
{
    float f, n, fRealAspect;

    if (bRotate)
        fRealAspect = 1.0f / fAspect;
    else
        fRealAspect = fAspect;

    // cotangent(a) == 1.0f / tan(a);
    f = 1.0f / (float)tan(fFOVy * 0.5f);
    n = 1.0f / (fFar - fNear);

    this->f[ 0] = f / fRealAspect;
    this->f[ 1] = 0;
    this->f[ 2] = 0;
    this->f[ 3] = 0;

    this->f[ 4] = 0;
    this->f[ 5] = f;
    this->f[ 6] = 0;
    this->f[ 7] = 0;

    this->f[ 8] = 0;
    this->f[ 9] = 0;
    this->f[10] = fFar * n;
    this->f[11] = 1;

    this->f[12] = 0;
    this->f[13] = 0;
    this->f[14] = -fFar * fNear * n;
    this->f[15] = 0;

    if (bRotate) matrixVRotate();
};

void matrixPerspectiveFovRH(const float fFOVy, const float fAspect, const float fNear, const float fFar, const bool bRotate)
{
    float f, n, fRealAspect;

    if (bRotate)
        fRealAspect = 1.0f / fAspect;
    else
        fRealAspect = fAspect;

    // cotangent(a) == 1.0f / tan(a);
    f = 1.0f / (float)tan(fFOVy * 0.5f);
    n = 1.0f / (fNear - fFar);

    this->f[ 0] = f / fRealAspect;
    this->f[ 1] = 0;
    this->f[ 2] = 0;
    this->f[ 3] = 0;

    this->f[ 4] = 0;
    this->f[ 5] = f;
    this->f[ 6] = 0;
    this->f[ 7] = 0;

    this->f[ 8] = 0;
    this->f[ 9] = 0;
    this->f[10] = (fFar + fNear) * n;
    this->f[11] = -1;

    this->f[12] = 0;
    this->f[13] = 0;
    this->f[14] = (2 * fFar * fNear) * n;
    this->f[15] = 0;

    if (bRotate) matrixVRotate();
};

void matrixOrthoLH(const float w, const float h, const float zn, const float zf, const bool  bRotate)
{
    this->f[ 0] = 2 / w;
    this->f[ 1] = 0;
    this->f[ 2] = 0;
    this->f[ 3] = 0;

    this->f[ 4] = 0;
    this->f[ 5] = 2 / h;
    this->f[ 6] = 0;
    this->f[ 7] = 0;

    this->f[ 8] = 0;
    this->f[ 9] = 0;
    this->f[10] = 1 / (zf - zn);
    this->f[11] = zn / (zn - zf);

    this->f[12] = 0;
    this->f[13] = 0;
    this->f[14] = 0;
    this->f[15] = 1;

    if (bRotate) matrixVRotate();
};

void matrixOrthoRH(const float w, const float h, const float zn, const float zf, const bool  bRotate)
{
    this->f[ 0] = 2 / w;
    this->f[ 1] = 0;
    this->f[ 2] = 0;
    this->f[ 3] = 0;

    this->f[ 4] = 0;
    this->f[ 5] = 2 / h;
    this->f[ 6] = 0;
    this->f[ 7] = 0;

    this->f[ 8] = 0;
    this->f[ 9] = 0;
    this->f[10] = 1 / (zn - zf);
    this->f[11] = zn / (zn - zf);

    this->f[12] = 0;
    this->f[13] = 0;
    this->f[14] = 0;
    this->f[15] = 1;

    if (bRotate) matrixVRotate();
};

VEC3 matrixLerp(const VEC3 &v1, const VEC3 &v2, const float s)
{
    VEC3 vOut;

    vOut.x = v1.x + s * (v2.x - v1.x);
    vOut.y = v1.y + s * (v2.y - v1.y);
    vOut.z = v1.z + s * (v2.z - v1.z);

    return vOut;
};

float matrixDotProduct(const VEC3 &v1, const VEC3 &v2)
{
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
};

float matrixLength(const VEC3 &vIn)
{
    double temp;

    temp = (double)(vIn.x * vIn.x + vIn.y * vIn.y + vIn.z * vIn.z);
    return (float) sqrt(temp);
};

void matrixLinearEqSolve(
    float		* const pRes,
    float		** const pSrc,	// 2D array of floats. 4 Eq linear problem is 5x4 matrix, constants in first column.
    const int	nCnt)
{
    int		i, j, k;
    float	f;

    if(nCnt == 1)
    {
        assert(pSrc[0][1] != 0);
        pRes[0] = pSrc[0][0] / pSrc[0][1];
        return;
    }

    // Loop backwards in an attempt avoid the need to swap rows
    i = nCnt;
    while(i)
    {
        --i;

        if(pSrc[i][nCnt] != 0)
        {
            // Row i can be used to zero the other rows; let's move it to the bottom
            if(i != (nCnt-1))
            {
                for(j = 0; j <= nCnt; ++j)
                {
                    // Swap the two values
                    f = pSrc[nCnt-1][j];
                    pSrc[nCnt-1][j] = pSrc[i][j];
                    pSrc[i][j] = f;
                }
            }

            // Now zero the last columns of the top rows
            for(j = 0; j < (nCnt-1); ++j)
            {
                assert(pSrc[nCnt-1][nCnt] != 0);
                f = pSrc[j][nCnt] / pSrc[nCnt-1][nCnt];

                // No need to actually calculate a zero for the final column
                for(k = 0; k < nCnt; ++k)
                {
                    pSrc[j][k] -= f * pSrc[nCnt-1][k];
                }
            }

            break;
        }
    }

    // Solve the top-left sub matrix
    matrixLinearEqSolve(pRes, pSrc, nCnt - 1);

    // Now calc the solution for the bottom row
    f = pSrc[nCnt-1][0];
    for(k = 1; k < nCnt; ++k)
    {
        f -= pSrc[nCnt-1][k] * pRes[k-1];
    }
    assert(pSrc[nCnt-1][nCnt] != 0);
    f /= pSrc[nCnt-1][nCnt];
    pRes[nCnt-1] = f;
};

void matrixInverseEx(const MATRIX &mIn)
{
    float *ppfRows[4];
    float pfRes[4];
    float pfIn[20];
    int   i, j;

    for(i = 0; i < 4; ++i)
        ppfRows[i] = &pfIn[i * 5];

    /* Solve 4 sets of 4 linear equations */
    for(i = 0; i < 4; ++i)
    {
        for(j = 0; j < 4; ++j)
        {
            ppfRows[j][0] = fIdentity[i + 4 * j];
            memcpy(&ppfRows[j][1], &mIn.f[j * 4], 4 * sizeof(float));
        }

        matrixLinearEqSolve(pfRes, (float**)ppfRows, 4);

        for(j = 0; j < 4; ++j)
        {
            this->f[i + 4 * j] = pfRes[j];
        }
    }
};

}; // End of MATRIX

#endif // VKMATH_H
