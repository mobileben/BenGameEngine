//
//  BGEMathTypes.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "BGEMathTypes.h"
#include <memory.h>
#include <math.h>

float BGEMathDegreesToRadians(float degrees)
{
    return degrees * M_PI_2;
}

float BGEMathRadiansToDegrees(float radians)
{
    return radians * ((double)180.0 * M_1_PI);
}

void BGEMatrix4Make(BGEMatrix4& matrix, float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
    matrix.m00 = m00;
    matrix.m01 = m01;
    matrix.m02 = m02;
    matrix.m03 = m03;

    matrix.m10 = m10;
    matrix.m11 = m11;
    matrix.m12 = m12;
    matrix.m13 = m13;
    
    matrix.m20 = m20;
    matrix.m21 = m21;
    matrix.m22 = m22;
    matrix.m23 = m23;

    matrix.m30 = m30;
    matrix.m31 = m31;
    matrix.m32 = m32;
    matrix.m33 = m33;
    
}

void BGEMatrix4MakeIdentify(BGEMatrix4& matrix)
{
    matrix.m00 = 1;
    matrix.m01 = 0;
    matrix.m02 = 0;
    matrix.m03 = 0;
    
    matrix.m10 = 0;
    matrix.m11 = 1;
    matrix.m12 = 0;
    matrix.m13 = 0;
    
    matrix.m20 = 0;
    matrix.m21 = 0;
    matrix.m22 = 1;
    matrix.m23 = 0;
    
    matrix.m30 = 0;
    matrix.m31 = 0;
    matrix.m32 = 0;
    matrix.m33 = 1;
}

void BGEMatrix4MakeWithArray(BGEMatrix4& matrix, float values[16])
{
    memcpy(matrix.m, values, sizeof(matrix.m));
}

void BGEMatrix4MakeRotation(BGEMatrix4& matrix, float radians, float x, float y, float z)
{
    float c = cosf(radians);
    float s = sinf(radians);
    float c1 = 1.0f - c;
    
    // build rotation matrix
    float r0 = x * x * c1 + c;
    float r1 = x * y * c1 + z * s;
    float r2 = x * z * c1 - y * s;
    float r4 = x * y * c1 - z * s;
    float r5 = y * y * c1 + c;
    float r6 = y * z * c1 + x * s;
    float r8 = x * z * c1 + y * s;
    float r9 = y * z * c1 - x * s;
    float r10= z * z * c1 + c;
    
    matrix.m[0] = r0;
    matrix.m[1] = r1;
    matrix.m[2] = r2;
    matrix.m[3] = 0;
    
    matrix.m[4] = r4;
    matrix.m[5] = r5;
    matrix.m[6] = r6;
    matrix.m[7] = 0;
    
    matrix.m[8] = r8;
    matrix.m[9] = r9;
    matrix.m[10]= r10;
    matrix.m[11] = 0;
    
    matrix.m[12]= 0;
    matrix.m[13]= 0;
    matrix.m[14]= 0;
    matrix.m[15] = 1;
}

void BGEMatrix4MakeRotationX(BGEMatrix4& matrix, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);
    
    matrix.m[0] = 1;
    matrix.m[1] = 0;
    matrix.m[2] = 0;
    matrix.m[3] = 0;
    
    matrix.m[4] = 0;
    matrix.m[5] = c;
    matrix.m[6] = -s;
    matrix.m[7] = 0;
    
    matrix.m[8] = 0;
    matrix.m[9] = s;
    matrix.m[10] = c;
    matrix.m[11] = 0;
    
    matrix.m[12] = 0;
    matrix.m[13] = 0;
    matrix.m[14] = 0;
    matrix.m[15] = 1;
}

void BGEMatrix4MakeRotationY(BGEMatrix4& matrix, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);
    
    matrix.m[0] = c;
    matrix.m[1] = 0;
    matrix.m[2] = s;
    matrix.m[3] = 0;
    
    matrix.m[4] = 0;
    matrix.m[5] = 1;
    matrix.m[6] = 0;
    matrix.m[7] = 0;
    
    matrix.m[8] = -s;
    matrix.m[9] = 0;
    matrix.m[10] = c;
    matrix.m[11] = 0;
    
    matrix.m[12] = 0;
    matrix.m[13] = 0;
    matrix.m[14] = 0;
    matrix.m[15] = 1;
}

void BGEMatrix4MakeRotationZ(BGEMatrix4& matrix, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);
    
    matrix.m[0] = c;
    matrix.m[1] = s;
    matrix.m[2] = 0;
    matrix.m[3] = 0;
    
    matrix.m[4] = -s;
    matrix.m[5] = c;
    matrix.m[6] = 0;
    matrix.m[7] = 0;
    
    matrix.m[8] = 0;
    matrix.m[9] = 0;
    matrix.m[10] = 1;
    matrix.m[11] = 0;
    
    matrix.m[12] = 0;
    matrix.m[13] = 0;
    matrix.m[14] = 0;
    matrix.m[15] = 1;
}

void BGEMatrix4MakeScale(BGEMatrix4& matrix, float sx, float sy, float sz)
{
    BGEMatrix4MakeIdentify(matrix);
    
    matrix.m[0] = sx;
    matrix.m[5] = sy;
    matrix.m[10] = sz;
}

void BGEMatrix4MakeTranslation(BGEMatrix4& matrix, float tx, float ty, float tz)
{
    BGEMatrix4MakeIdentify(matrix);
    
    matrix.m[12] = tx;
    matrix.m[13] = ty;
    matrix.m[14] = tz;
}

void BGEMatrix4MakeOrthographic(BGEMatrix4& matrix, float left, float right, float bottom, float top, float near, float far)
{
    matrix.m[0] = 2.0 / (right - left);
    matrix.m[1] = 0;
    matrix.m[2] = 0;
    matrix.m[3] = 0;

    matrix.m[4] = 0;
    matrix.m[5] = 2 / (top - bottom);
    matrix.m[6] = 0;
    matrix.m[7] = 0;

    matrix.m[8] = 0;
    matrix.m[9] = 0;
    matrix.m[10] = -2 / (far - near);
    matrix.m[11] = 0;

    matrix.m[12] = -(right + left) / (right - left);
    matrix.m[13] = -(top + bottom) / (top - bottom);
    matrix.m[14] = -(far + near) / (far - near);
    matrix.m[15] = 1;
}

void BGEMatrix4Multiply(BGEMatrix4& matrix, BGEMatrix4& left, BGEMatrix4& right)
{
    // First Column
    matrix.m[0] = left.m[0]*right.m[0] + left.m[4]*right.m[1] + left.m[8]*right.m[2] + left.m[12]*right.m[3];
    matrix.m[1] = left.m[1]*right.m[0] + left.m[5]*right.m[1] + left.m[9]*right.m[2] + left.m[13]*right.m[3];
    matrix.m[2] = left.m[2]*right.m[0] + left.m[6]*right.m[1] + left.m[10]*right.m[2] + left.m[14]*right.m[3];
    matrix.m[3] = left.m[3]*right.m[0] + left.m[7]*right.m[1] + left.m[11]*right.m[2] + left.m[15]*right.m[3];
    
    // Second Column
    matrix.m[4] = left.m[0]*right.m[4] + left.m[4]*right.m[5] + left.m[8]*right.m[6] + left.m[12]*right.m[7];
    matrix.m[5] = left.m[1]*right.m[4] + left.m[5]*right.m[5] + left.m[9]*right.m[6] + left.m[13]*right.m[7];
    matrix.m[6] = left.m[2]*right.m[4] + left.m[6]*right.m[5] + left.m[10]*right.m[6] + left.m[14]*right.m[7];
    matrix.m[7] = left.m[3]*right.m[4] + left.m[7]*right.m[5] + left.m[11]*right.m[6] + left.m[15]*right.m[7];
    
    // Third Column
    matrix.m[8] = left.m[0]*right.m[8] + left.m[4]*right.m[9] + left.m[8]*right.m[10] + left.m[12]*right.m[11];
    matrix.m[9] = left.m[1]*right.m[8] + left.m[5]*right.m[9] + left.m[9]*right.m[10] + left.m[13]*right.m[11];
    matrix.m[10] = left.m[2]*right.m[8] + left.m[6]*right.m[9] + left.m[10]*right.m[10] + left.m[14]*right.m[11];
    matrix.m[11] = left.m[3]*right.m[8] + left.m[7]*right.m[9] + left.m[11]*right.m[10] + left.m[15]*right.m[11];
    
    // Fourth Column
    matrix.m[12] = left.m[0]*right.m[12] + left.m[4]*right.m[13] + left.m[8]*right.m[14] + left.m[12]*right.m[15];
    matrix.m[13] = left.m[1]*right.m[12] + left.m[5]*right.m[13] + left.m[9]*right.m[14] + left.m[13]*right.m[15];
    matrix.m[14] = left.m[2]*right.m[12] + left.m[6]*right.m[13] + left.m[10]*right.m[14] + left.m[14]*right.m[15];
    matrix.m[15] = left.m[3]*right.m[12] + left.m[7]*right.m[13] + left.m[11]*right.m[14] + left.m[15]*right.m[15];
}

void BGEMatrix4Rotate(BGEMatrix4& matrix, float radians, float x, float y, float z)
{
    float c = cosf(radians);
    float s = sinf(radians);    // sine
    float c1 = 1.0f - c;
    
    float m0 = matrix.m[0],  m4 = matrix.m[4],  m8 = matrix.m[8],  m12 = matrix.m[12],
    m1 = matrix.m[1],  m5 = matrix.m[5],  m9 = matrix.m[9],  m13 = matrix.m[13],
    m2 = matrix.m[2],  m6 = matrix.m[6],  m10 = matrix.m[10], m14 = matrix.m[14];
    
    float r0 = x * x * c1 + c;
    float r1 = x * y * c1 + z * s;
    float r2 = x * z * c1 - y * s;
    float r4 = x * y * c1 - z * s;
    float r5 = y * y * c1 + c;
    float r6 = y * z * c1 + x * s;
    float r8 = x * z * c1 + y * s;
    float r9 = y * z * c1 - x * s;
    float r10= z * z * c1 + c;
    
    matrix.m[0] = r0 * m0 + r4 * m1 + r8 * m2;
    matrix.m[1] = r1 * m0 + r5 * m1 + r9 * m2;
    matrix.m[2] = r2 * m0 + r6 * m1 + r10* m2;
    matrix.m[4] = r0 * m4 + r4 * m5 + r8 * m6;
    matrix.m[5] = r1 * m4 + r5 * m5 + r9 * m6;
    matrix.m[6] = r2 * m4 + r6 * m5 + r10* m6;
    matrix.m[8] = r0 * m8 + r4 * m9 + r8 * m10;
    matrix.m[9] = r1 * m8 + r5 * m9 + r9 * m10;
    matrix.m[10]= r2 * m8 + r6 * m9 + r10* m10;
    matrix.m[12]= r0 * m12+ r4 * m13+ r8 * m14;
    matrix.m[13]= r1 * m12+ r5 * m13+ r9 * m14;
    matrix.m[14]= r2 * m12+ r6 * m13+ r10* m14;
}

void BGEMatrix4RotateX(BGEMatrix4& matrix, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);
    float m1 = matrix.m[1],  m2 = matrix.m[2],
    m5 = matrix.m[5],  m6 = matrix.m[6],
    m9 = matrix.m[9],  m10= matrix.m[10],
    m13= matrix.m[13], m14= matrix.m[14];
    
    matrix.m[1] = m1 * c + m2 *-s;
    matrix.m[2] = m1 * s + m2 * c;
    matrix.m[5] = m5 * c + m6 *-s;
    matrix.m[6] = m5 * s + m6 * c;
    matrix.m[9] = m9 * c + m10*-s;
    matrix.m[10]= m9 * s + m10* c;
    matrix.m[13]= m13* c + m14*-s;
    matrix.m[14]= m13* s + m14* c;
}

void BGEMatrix4RotateY(BGEMatrix4& matrix, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);
    float m0 = matrix.m[0],  m2 = matrix.m[2],
    m4 = matrix.m[4],  m6 = matrix.m[6],
    m8 = matrix.m[8],  m10 = matrix.m[10],
    m12 = matrix.m[12], m14 = matrix.m[14];
    
    matrix.m[0] = m0 * c + m2 * s;
    matrix.m[2] = m0 *-s + m2 * c;
    matrix.m[4] = m4 * c + m6 * s;
    matrix.m[6] = m4 *-s + m6 * c;
    matrix.m[8] = m8 * c + m10* s;
    matrix.m[10]= m8 *-s + m10* c;
    matrix.m[12]= m12* c + m14* s;
    matrix.m[14]= m12*-s + m14* c;
}

void BGEMatrix4RotateZ(BGEMatrix4& matrix, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);
    float m0 = matrix.m[0],  m1 = matrix.m[1],
    m4 = matrix.m[4],  m5 = matrix.m[5],
    m8 = matrix.m[8],  m9 = matrix.m[9],
    m12 = matrix.m[12], m13 = matrix.m[13];
    
    matrix.m[0] = m0 * c + m1 *-s;
    matrix.m[1] = m0 * s + m1 * c;
    matrix.m[4] = m4 * c + m5 *-s;
    matrix.m[5] = m4 * s + m5 * c;
    matrix.m[8] = m8 * c + m9 *-s;
    matrix.m[9] = m8 * s + m9 * c;
    matrix.m[12]= m12* c + m13*-s;
    matrix.m[13]= m12* s + m13* c;
}

void BGEMatrix4Scale(BGEMatrix4& matrix, float sx, float sy, float sz)
{
    matrix.m[0] *= sx;
    matrix.m[1] *= sy;
    matrix.m[2] *= sz;
    
    matrix.m[8] *= sx;
    matrix.m[9] *= sy;
    matrix.m[10]*= sz;
    
    matrix.m[4] *= sx;
    matrix.m[5] *= sy;
    matrix.m[6] *= sz;
    
    
    matrix.m[12] *= sx;
    matrix.m[13] *= sy;
    matrix.m[14] *= sz;
}

void BGEMatrix4Translate(BGEMatrix4& matrix, float tx, float ty, float tz)
{
    matrix.m[0] += matrix.m[3] * tx;
    matrix.m[1] += matrix.m[3] * ty;
    matrix.m[2] += matrix.m[3] * tz;
    
    matrix.m[4] += matrix.m[7] * tx;
    matrix.m[5] += matrix.m[7] * ty;
    matrix.m[6] += matrix.m[7] * tz;
    
    matrix.m[8] += matrix.m[11]* tx;
    matrix.m[9] += matrix.m[11]* ty;
    matrix.m[10] += matrix.m[11]* tz;
    
    matrix.m[12] += matrix.m[15]* tx;
    matrix.m[13] += matrix.m[15]* ty;
    matrix.m[14] += matrix.m[15]* tz;
}

void BGEMatrix4MultiplyVector3(BGEVector3 &result, BGEMatrix4& matrix, BGEVector3 v3)
{
    result.x = matrix.m[0]*v3.x + matrix.m[4]*v3.y + matrix.m[8]*v3.z;
    result.y = matrix.m[1]*v3.x + matrix.m[5]*v3.y + matrix.m[9]*v3.z;
    result.z = matrix.m[2]*v3.x + matrix.m[6]*v3.y + matrix.m[10]*v3.z;
}
