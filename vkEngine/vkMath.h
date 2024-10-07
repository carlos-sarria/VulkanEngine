#ifndef VKMATH_H
#define VKMATH_H

#include "vkStructs.h"

const float PI = 3.14159265359f;

class MATRIX
{
public:
    float* operator [] ( const int Row ) { return &f[Row*3]; }
    float f[9];
};

static const float fIdentity[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

inline void _matrixIdentity(MATRIX &mOut)
{
    mOut.f[ 0]=1.0f;	mOut.f[ 4]=0.0f;	mOut.f[ 8]=0.0f;	mOut.f[12]=0.0f;
    mOut.f[ 1]=0.0f;	mOut.f[ 5]=1.0f;	mOut.f[ 9]=0.0f;	mOut.f[13]=0.0f;
    mOut.f[ 2]=0.0f;	mOut.f[ 6]=0.0f;	mOut.f[10]=1.0f;	mOut.f[14]=0.0f;
    mOut.f[ 3]=0.0f;	mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;
}

inline void _matrixMultiply(
    MATRIX			&mOut,
    const MATRIX	&mA,
    const MATRIX	&mB)
{
    MATRIX mRet;

    /* Perform calculation on a dummy matrix (mRet) */
    mRet.f[ 0] = mA.f[ 0]*mB.f[ 0] + mA.f[ 1]*mB.f[ 4] + mA.f[ 2]*mB.f[ 8] + mA.f[ 3]*mB.f[12];
    mRet.f[ 1] = mA.f[ 0]*mB.f[ 1] + mA.f[ 1]*mB.f[ 5] + mA.f[ 2]*mB.f[ 9] + mA.f[ 3]*mB.f[13];
    mRet.f[ 2] = mA.f[ 0]*mB.f[ 2] + mA.f[ 1]*mB.f[ 6] + mA.f[ 2]*mB.f[10] + mA.f[ 3]*mB.f[14];
    mRet.f[ 3] = mA.f[ 0]*mB.f[ 3] + mA.f[ 1]*mB.f[ 7] + mA.f[ 2]*mB.f[11] + mA.f[ 3]*mB.f[15];

    mRet.f[ 4] = mA.f[ 4]*mB.f[ 0] + mA.f[ 5]*mB.f[ 4] + mA.f[ 6]*mB.f[ 8] + mA.f[ 7]*mB.f[12];
    mRet.f[ 5] = mA.f[ 4]*mB.f[ 1] + mA.f[ 5]*mB.f[ 5] + mA.f[ 6]*mB.f[ 9] + mA.f[ 7]*mB.f[13];
    mRet.f[ 6] = mA.f[ 4]*mB.f[ 2] + mA.f[ 5]*mB.f[ 6] + mA.f[ 6]*mB.f[10] + mA.f[ 7]*mB.f[14];
    mRet.f[ 7] = mA.f[ 4]*mB.f[ 3] + mA.f[ 5]*mB.f[ 7] + mA.f[ 6]*mB.f[11] + mA.f[ 7]*mB.f[15];

    mRet.f[ 8] = mA.f[ 8]*mB.f[ 0] + mA.f[ 9]*mB.f[ 4] + mA.f[10]*mB.f[ 8] + mA.f[11]*mB.f[12];
    mRet.f[ 9] = mA.f[ 8]*mB.f[ 1] + mA.f[ 9]*mB.f[ 5] + mA.f[10]*mB.f[ 9] + mA.f[11]*mB.f[13];
    mRet.f[10] = mA.f[ 8]*mB.f[ 2] + mA.f[ 9]*mB.f[ 6] + mA.f[10]*mB.f[10] + mA.f[11]*mB.f[14];
    mRet.f[11] = mA.f[ 8]*mB.f[ 3] + mA.f[ 9]*mB.f[ 7] + mA.f[10]*mB.f[11] + mA.f[11]*mB.f[15];

    mRet.f[12] = mA.f[12]*mB.f[ 0] + mA.f[13]*mB.f[ 4] + mA.f[14]*mB.f[ 8] + mA.f[15]*mB.f[12];
    mRet.f[13] = mA.f[12]*mB.f[ 1] + mA.f[13]*mB.f[ 5] + mA.f[14]*mB.f[ 9] + mA.f[15]*mB.f[13];
    mRet.f[14] = mA.f[12]*mB.f[ 2] + mA.f[13]*mB.f[ 6] + mA.f[14]*mB.f[10] + mA.f[15]*mB.f[14];
    mRet.f[15] = mA.f[12]*mB.f[ 3] + mA.f[13]*mB.f[ 7] + mA.f[14]*mB.f[11] + mA.f[15]*mB.f[15];

    /* Copy result to mOut */
    mOut = mRet;
}


inline void _matrixTranslation(
    MATRIX	&mOut,
    const float fX,
    const float fY,
    const float fZ)
{
    mOut.f[ 0]=1.0f;	mOut.f[ 4]=0.0f;	mOut.f[ 8]=0.0f;	mOut.f[12]=fX;
    mOut.f[ 1]=0.0f;	mOut.f[ 5]=1.0f;	mOut.f[ 9]=0.0f;	mOut.f[13]=fY;
    mOut.f[ 2]=0.0f;	mOut.f[ 6]=0.0f;	mOut.f[10]=1.0f;	mOut.f[14]=fZ;
    mOut.f[ 3]=0.0f;	mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;
}

inline void _matrixScaling(
    MATRIX	&mOut,
    const float fX,
    const float fY,
    const float fZ)
{
    mOut.f[ 0]=fX;		mOut.f[ 4]=0.0f;	mOut.f[ 8]=0.0f;	mOut.f[12]=0.0f;
    mOut.f[ 1]=0.0f;	mOut.f[ 5]=fY;		mOut.f[ 9]=0.0f;	mOut.f[13]=0.0f;
    mOut.f[ 2]=0.0f;	mOut.f[ 6]=0.0f;	mOut.f[10]=fZ;		mOut.f[14]=0.0f;
    mOut.f[ 3]=0.0f;	mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;
}
inline void _matrixRotationQ(
    MATRIX	&mOut,
    VEC4 quaternion)
{
    // Extract the values from Q
    float q0 = quaternion.x;
    float q1 = quaternion.y;
    float q2 = quaternion.z;
    float q3 = quaternion.w;

    // First row of the rotation matrix
    float r00 = 2.0f * (q0 * q0 + q1 * q1) - 1.0f;
    float r01 = 2.0f * (q1 * q2 - q0 * q3);
    float r02 = 2.0f * (q1 * q3 + q0 * q2);

    // Second row of the rotation matrix
    float r10 = 2.0f * (q1 * q2 + q0 * q3);
    float r11 = 2.0f * (q0 * q0 + q2 * q2) - 1.0f;
    float r12 = 2.0f * (q2 * q3 - q0 * q1);

    // Third row of the rotation matrix
    float r20 = 2.0f * (q1 * q3 - q0 * q2);
    float r21 = 2.0f * (q2 * q3 + q0 * q1);
    float r22 = 2.0f * (q0 * q0 + q3 * q3) - 1.0f;

    // 3x3 rotation matrix
    mOut.f[ 0]=r00;	mOut.f[ 4]=r01;	mOut.f[ 8]=r02;	mOut.f[12]=0.0f;
    mOut.f[ 1]=r10;	mOut.f[ 5]=r11;	mOut.f[ 9]=r12;	mOut.f[13]=0.0f;
    mOut.f[ 2]=r20;	mOut.f[ 6]=r21;	mOut.f[10]=r22;	mOut.f[14]=0.0f;
    mOut.f[ 3]=0.0f; mOut.f[ 7]=0.0f; mOut.f[11]=0.0f; mOut.f[15]=1.0f;
}

inline void _matrixRotationX(
    MATRIX	&mOut,
    const float fAngle)
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
    mOut.f[ 0]=1.0f;	mOut.f[ 4]=0.0f;	mOut.f[ 8]=0.0f;	mOut.f[12]=0.0f;
    mOut.f[ 1]=0.0f;	mOut.f[ 5]=fCosine;	mOut.f[ 9]=fSine;	mOut.f[13]=0.0f;
    mOut.f[ 2]=0.0f;	mOut.f[ 6]=-fSine;	mOut.f[10]=fCosine;	mOut.f[14]=0.0f;
    mOut.f[ 3]=0.0f;	mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;
}

inline void _matrixRotationY(
    MATRIX	&mOut,
    const float fAngle)
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
    mOut.f[ 0]=fCosine;		mOut.f[ 4]=0.0f;	mOut.f[ 8]=-fSine;		mOut.f[12]=0.0f;
    mOut.f[ 1]=0.0f;		mOut.f[ 5]=1.0f;	mOut.f[ 9]=0.0f;		mOut.f[13]=0.0f;
    mOut.f[ 2]=fSine;		mOut.f[ 6]=0.0f;	mOut.f[10]=fCosine;		mOut.f[14]=0.0f;
    mOut.f[ 3]=0.0f;		mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;		mOut.f[15]=1.0f;
}

inline void _matrixRotationZ(
    MATRIX	&mOut,
    const float fAngle)
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
    mOut.f[ 0]=fCosine;		mOut.f[ 4]=fSine;	mOut.f[ 8]=0.0f;	mOut.f[12]=0.0f;
    mOut.f[ 1]=-fSine;		mOut.f[ 5]=fCosine;	mOut.f[ 9]=0.0f;	mOut.f[13]=0.0f;
    mOut.f[ 2]=0.0f;		mOut.f[ 6]=0.0f;	mOut.f[10]=1.0f;	mOut.f[14]=0.0f;
    mOut.f[ 3]=0.0f;		mOut.f[ 7]=0.0f;	mOut.f[11]=0.0f;	mOut.f[15]=1.0f;
}

inline void _matrixTranspose(
    MATRIX			&mOut,
    const MATRIX	&mIn)
{
    MATRIX	mTmp;

    mTmp.f[ 0]=mIn.f[ 0];	mTmp.f[ 4]=mIn.f[ 1];	mTmp.f[ 8]=mIn.f[ 2];	mTmp.f[12]=mIn.f[ 3];
    mTmp.f[ 1]=mIn.f[ 4];	mTmp.f[ 5]=mIn.f[ 5];	mTmp.f[ 9]=mIn.f[ 6];	mTmp.f[13]=mIn.f[ 7];
    mTmp.f[ 2]=mIn.f[ 8];	mTmp.f[ 6]=mIn.f[ 9];	mTmp.f[10]=mIn.f[10];	mTmp.f[14]=mIn.f[11];
    mTmp.f[ 3]=mIn.f[12];	mTmp.f[ 7]=mIn.f[13];	mTmp.f[11]=mIn.f[14];	mTmp.f[15]=mIn.f[15];

    mOut = mTmp;
}

inline void _matrixInverse(
    MATRIX			&mOut,
    const MATRIX	&mIn)
{
    MATRIX	mDummyMatrix;
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
        /* Matrix M has no inverse */
        Log(true, "Matrix has no inverse : singular matrix\n");
        return;
    }
    else
    {
        /* Calculate inverse(A) = adj(A) / det(A) */
        det_1 = 1.0 / det_1;
        mDummyMatrix.f[ 0] =   ( mIn.f[ 5] * mIn.f[10] - mIn.f[ 9] * mIn.f[ 6] ) * (float)det_1;
        mDummyMatrix.f[ 1] = - ( mIn.f[ 1] * mIn.f[10] - mIn.f[ 9] * mIn.f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 2] =   ( mIn.f[ 1] * mIn.f[ 6] - mIn.f[ 5] * mIn.f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 4] = - ( mIn.f[ 4] * mIn.f[10] - mIn.f[ 8] * mIn.f[ 6] ) * (float)det_1;
        mDummyMatrix.f[ 5] =   ( mIn.f[ 0] * mIn.f[10] - mIn.f[ 8] * mIn.f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 6] = - ( mIn.f[ 0] * mIn.f[ 6] - mIn.f[ 4] * mIn.f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 8] =   ( mIn.f[ 4] * mIn.f[ 9] - mIn.f[ 8] * mIn.f[ 5] ) * (float)det_1;
        mDummyMatrix.f[ 9] = - ( mIn.f[ 0] * mIn.f[ 9] - mIn.f[ 8] * mIn.f[ 1] ) * (float)det_1;
        mDummyMatrix.f[10] =   ( mIn.f[ 0] * mIn.f[ 5] - mIn.f[ 4] * mIn.f[ 1] ) * (float)det_1;

        /* Calculate -C * inverse(A) */
        mDummyMatrix.f[12] = - ( mIn.f[12] * mDummyMatrix.f[ 0] + mIn.f[13] * mDummyMatrix.f[ 4] + mIn.f[14] * mDummyMatrix.f[ 8] );
        mDummyMatrix.f[13] = - ( mIn.f[12] * mDummyMatrix.f[ 1] + mIn.f[13] * mDummyMatrix.f[ 5] + mIn.f[14] * mDummyMatrix.f[ 9] );
        mDummyMatrix.f[14] = - ( mIn.f[12] * mDummyMatrix.f[ 2] + mIn.f[13] * mDummyMatrix.f[ 6] + mIn.f[14] * mDummyMatrix.f[10] );

        /* Fill in last row */
        mDummyMatrix.f[ 3] = 0.0f;
        mDummyMatrix.f[ 7] = 0.0f;
        mDummyMatrix.f[11] = 0.0f;
        mDummyMatrix.f[15] = 1.0f;
    }

    /* Copy contents of dummy matrix in pfMatrix */
    mOut = mDummyMatrix;
}

inline void _matrixNormalize(
    VEC3		&vOut,
    const VEC3	&vIn)
{
    float	f;
    double temp;

    temp = (double)(vIn.x * vIn.x + vIn.y * vIn.y + vIn.z * vIn.z);
    temp = 1.0 / sqrt(temp);
    f = (float)temp;

    vOut.x = vIn.x * f;
    vOut.y = vIn.y * f;
    vOut.z = vIn.z * f;
}


inline void _matrixCrossProduct(
    VEC3		&vOut,
    const VEC3	&v1,
    const VEC3	&v2)
{
    VEC3 result;

    /* Perform calculation on a dummy VECTOR (result) */
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;

    /* Copy result in pOut */
    vOut = result;
}

inline void _matrixLookAtLH(
    MATRIX			&mOut,
    const VEC3	&vEye,
    const VEC3	&vAt,
    const VEC3	&vUp)
{
    VEC3 f, s, u;
    MATRIX	t;

    f.x = vEye.x - vAt.x;
    f.y = vEye.y - vAt.y;
    f.z = vEye.z - vAt.z;

    _matrixNormalize(f, f);
    _matrixCrossProduct(s, f, vUp);
    _matrixNormalize(s, s);
    _matrixCrossProduct(u, s, f);
    _matrixNormalize(u, u);

    mOut.f[ 0] = s.x;
    mOut.f[ 1] = u.x;
    mOut.f[ 2] = -f.x;
    mOut.f[ 3] = 0;

    mOut.f[ 4] = s.y;
    mOut.f[ 5] = u.y;
    mOut.f[ 6] = -f.y;
    mOut.f[ 7] = 0;

    mOut.f[ 8] = s.z;
    mOut.f[ 9] = u.z;
    mOut.f[10] = -f.z;
    mOut.f[11] = 0;

    mOut.f[12] = 0;
    mOut.f[13] = 0;
    mOut.f[14] = 0;
    mOut.f[15] = 1;

    _matrixTranslation(t, -vEye.x, -vEye.y, -vEye.z);
    _matrixMultiply(mOut, t, mOut);
}

inline void _matrixLookAtRH(
    MATRIX			&mOut,
    const VEC3	&vEye,
    const VEC3	&vAt,
    const VEC3	&vUp)
{
    VEC3 f, s, u;
    MATRIX	t;

    f.x = vAt.x - vEye.x;
    f.y = vAt.y - vEye.y;
    f.z = vAt.z - vEye.z;

    _matrixNormalize(f, f);
    _matrixCrossProduct(s, f, vUp);
    _matrixNormalize(s, s);
    _matrixCrossProduct(u, s, f);
    _matrixNormalize(u, u);

    mOut.f[ 0] = s.x;
    mOut.f[ 1] = u.x;
    mOut.f[ 2] = -f.x;
    mOut.f[ 3] = 0;

    mOut.f[ 4] = s.y;
    mOut.f[ 5] = u.y;
    mOut.f[ 6] = -f.y;
    mOut.f[ 7] = 0;

    mOut.f[ 8] = s.z;
    mOut.f[ 9] = u.z;
    mOut.f[10] = -f.z;
    mOut.f[11] = 0;

    mOut.f[12] = 0;
    mOut.f[13] = 0;
    mOut.f[14] = 0;
    mOut.f[15] = 1;

    _matrixTranslation(t, -vEye.x, -vEye.y, -vEye.z);
    _matrixMultiply(mOut, t, mOut);
}

inline void _matrixPerspectiveFovLH(
    MATRIX	&mOut,
    const float	fFOVy,
    const float	fAspect,
    const float	fNear,
    const float	fFar,
    const bool  bRotate)
{
    float f, n, fRealAspect;

    if (bRotate)
        fRealAspect = 1.0f / fAspect;
    else
        fRealAspect = fAspect;

    // cotangent(a) == 1.0f / tan(a);
    f = 1.0f / (float)tan(fFOVy * 0.5f);
    n = 1.0f / (fFar - fNear);

    mOut.f[ 0] = f / fRealAspect;
    mOut.f[ 1] = 0;
    mOut.f[ 2] = 0;
    mOut.f[ 3] = 0;

    mOut.f[ 4] = 0;
    mOut.f[ 5] = f;
    mOut.f[ 6] = 0;
    mOut.f[ 7] = 0;

    mOut.f[ 8] = 0;
    mOut.f[ 9] = 0;
    mOut.f[10] = fFar * n;
    mOut.f[11] = 1;

    mOut.f[12] = 0;
    mOut.f[13] = 0;
    mOut.f[14] = -fFar * fNear * n;
    mOut.f[15] = 0;

    if (bRotate)
    {
        MATRIX mRotation, mTemp = mOut;
        _matrixRotationZ(mRotation, 90.0f*PI/180.0f);
        _matrixMultiply(mOut, mTemp, mRotation);
    }
}

inline void _matrixPerspectiveFovRH(
    MATRIX	&mOut,
    const float	fFOVy,
    const float	fAspect,
    const float	fNear,
    const float	fFar,
    const bool  bRotate)
{
    float f, n, fRealAspect;

    if (bRotate)
        fRealAspect = 1.0f / fAspect;
    else
        fRealAspect = fAspect;

    // cotangent(a) == 1.0f / tan(a);
    f = 1.0f / (float)tan(fFOVy * 0.5f);
    n = 1.0f / (fNear - fFar);

    mOut.f[ 0] = f / fRealAspect;
    mOut.f[ 1] = 0;
    mOut.f[ 2] = 0;
    mOut.f[ 3] = 0;

    mOut.f[ 4] = 0;
    mOut.f[ 5] = f;
    mOut.f[ 6] = 0;
    mOut.f[ 7] = 0;

    mOut.f[ 8] = 0;
    mOut.f[ 9] = 0;
    mOut.f[10] = (fFar + fNear) * n;
    mOut.f[11] = -1;

    mOut.f[12] = 0;
    mOut.f[13] = 0;
    mOut.f[14] = (2 * fFar * fNear) * n;
    mOut.f[15] = 0;

    if (bRotate)
    {
        MATRIX mRotation, mTemp = mOut;
        _matrixRotationZ(mRotation, -90.0f*PI/180.0f);
        _matrixMultiply(mOut, mTemp, mRotation);
    }
}

inline void _matrixOrthoLH(
    MATRIX	&mOut,
    const float w,
    const float h,
    const float zn,
    const float zf,
    const bool  bRotate)
{
    mOut.f[ 0] = 2 / w;
    mOut.f[ 1] = 0;
    mOut.f[ 2] = 0;
    mOut.f[ 3] = 0;

    mOut.f[ 4] = 0;
    mOut.f[ 5] = 2 / h;
    mOut.f[ 6] = 0;
    mOut.f[ 7] = 0;

    mOut.f[ 8] = 0;
    mOut.f[ 9] = 0;
    mOut.f[10] = 1 / (zf - zn);
    mOut.f[11] = zn / (zn - zf);

    mOut.f[12] = 0;
    mOut.f[13] = 0;
    mOut.f[14] = 0;
    mOut.f[15] = 1;

    if (bRotate)
    {
        MATRIX mRotation, mTemp = mOut;
        _matrixRotationZ(mRotation, -90.0f*PI/180.0f);
        _matrixMultiply(mOut, mRotation, mTemp);
    }
}

inline void _matrixOrthoRH(
    MATRIX	&mOut,
    const float w,
    const float h,
    const float zn,
    const float zf,
    const bool  bRotate)
{
    mOut.f[ 0] = 2 / w;
    mOut.f[ 1] = 0;
    mOut.f[ 2] = 0;
    mOut.f[ 3] = 0;

    mOut.f[ 4] = 0;
    mOut.f[ 5] = 2 / h;
    mOut.f[ 6] = 0;
    mOut.f[ 7] = 0;

    mOut.f[ 8] = 0;
    mOut.f[ 9] = 0;
    mOut.f[10] = 1 / (zn - zf);
    mOut.f[11] = zn / (zn - zf);

    mOut.f[12] = 0;
    mOut.f[13] = 0;
    mOut.f[14] = 0;
    mOut.f[15] = 1;

    if (bRotate)
    {
        MATRIX mRotation, mTemp = mOut;
        _matrixRotationZ(mRotation, -90.0f*PI/180.0f);
        _matrixMultiply(mOut, mRotation, mTemp);
    }
}

inline void _matrixLerp(
    VEC3		&vOut,
    const VEC3	&v1,
    const VEC3	&v2,
    const float	s)
{
    vOut.x = v1.x + s * (v2.x - v1.x);
    vOut.y = v1.y + s * (v2.y - v1.y);
    vOut.z = v1.z + s * (v2.z - v1.z);
}

inline float _matrixDotProduct(
    const VEC3	&v1,
    const VEC3	&v2)
{
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

inline float _matrixLength(
    const VEC3	&vIn)
{
    double temp;

    temp = (double)(vIn.x * vIn.x + vIn.y * vIn.y + vIn.z * vIn.z);
    return (float) sqrt(temp);
}

inline void _matrixLinearEqSolve(
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
    _matrixLinearEqSolve(pRes, pSrc, nCnt - 1);

    // Now calc the solution for the bottom row
    f = pSrc[nCnt-1][0];
    for(k = 1; k < nCnt; ++k)
    {
        f -= pSrc[nCnt-1][k] * pRes[k-1];
    }
    assert(pSrc[nCnt-1][nCnt] != 0);
    f /= pSrc[nCnt-1][nCnt];
    pRes[nCnt-1] = f;
}

inline void _matrixInverseEx(
    MATRIX			&mOut,
    const MATRIX	&mIn)
{
    MATRIX		mTmp = {0};
    float 			*ppfRows[4];
    float 			pfRes[4];
    float 			pfIn[20];
    int				i, j;

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

        _matrixLinearEqSolve(pfRes, (float**)ppfRows, 4);

        for(j = 0; j < 4; ++j)
        {
            mTmp.f[i + 4 * j] = pfRes[j];
        }
    }

    mOut = mTmp;
}

#endif // VKMATH_H
