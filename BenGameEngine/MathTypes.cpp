//
//  MathTypes.cpp
//  GamePlayground
//
//  Created by Benjamin Lee on 2/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#include "MathTypes.h"
#include <memory.h>
#include <math.h>

namespace BGE {
    bool operator==(const Rect& lhs, const Rect& rhs) {
        if (lhs.x != rhs.x) {
            return false;
        }
        
        if (lhs.y != rhs.y) {
            return false;
        }
        
        if (lhs.w != rhs.w) {
            return false;
        }
        
        if (lhs.h != rhs.h) {
            return false;
        }
        
        return true;
    }
    
    bool operator!=(const Rect& lhs, const Rect& rhs) {
        if (lhs.x != rhs.x) {
            return true;
        }
        
        if (lhs.y != rhs.y) {
            return true;
        }
        
        if (lhs.w != rhs.w) {
            return true;
        }
        
        if (lhs.h != rhs.h) {
            return true;
        }
        
        return false;
    }

    bool operator==(const Matrix4& lhs, const Matrix4& rhs) {
        for (auto i=0;i<sizeof(lhs.m)/sizeof(float);++i) {
            if (lhs.m[i] != rhs.m[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix4& lhs, const Matrix4& rhs) {
        for (auto i=0;i<sizeof(lhs.m)/sizeof(float);++i) {
            if (lhs.m[i] != rhs.m[i]) {
                return true;
            }
        }
        return false;
    }

    bool operator==(const Matrix3& lhs, const Matrix3& rhs) {
        for (auto i=0;i<sizeof(lhs.m)/sizeof(float);++i) {
            if (lhs.m[i] != rhs.m[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix3& lhs, const Matrix3& rhs) {
        for (auto i=0;i<sizeof(lhs.m)/sizeof(float);++i) {
            if (lhs.m[i] != rhs.m[i]) {
                return true;
            }
        }
        return false;
    }

    bool operator==(const Matrix2& lhs, const Matrix2& rhs) {
        for (auto i=0;i<sizeof(lhs.m)/sizeof(float);++i) {
            if (lhs.m[i] != rhs.m[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix2& lhs, const Matrix2& rhs) {
        for (auto i=0;i<sizeof(lhs.m)/sizeof(float);++i) {
            if (lhs.m[i] != rhs.m[i]) {
                return true;
            }
        }
        return false;
    }

    bool operator==(const Vector4& lhs, const Vector4& rhs) {
        if (lhs.x != rhs.x) {
            return false;
        }
        if (lhs.y != rhs.y) {
            return false;
        }
        if (lhs.z != rhs.z) {
            return false;
        }
        if (lhs.w != rhs.w) {
            return false;
        }
        
        return true;
    }
    
    bool operator!=(const Vector4& lhs, const Vector4& rhs) {
        if (lhs.x != rhs.x) {
            return true;
        }
        if (lhs.y != rhs.y) {
            return true;
        }
        if (lhs.z != rhs.z) {
            return true;
        }
        if (lhs.w != rhs.w) {
            return true;
        }
        
        return false;
    }
    
    bool operator==(const Vector3& lhs, const Vector3& rhs) {
        if (lhs.x != rhs.x) {
            return false;
        }
        if (lhs.y != rhs.y) {
            return false;
        }
        if (lhs.z != rhs.z) {
            return false;
        }
        
        return true;
    }
    
    bool operator!=(const Vector3& lhs, const Vector3& rhs) {
        if (lhs.x != rhs.x) {
            return true;
        }
        if (lhs.y != rhs.y) {
            return true;
        }
        if (lhs.z != rhs.z) {
            return true;
        }
        
        return false;
    }
    
    bool operator==(const Vector2& lhs, const Vector2& rhs) {
        if (lhs.x != rhs.x) {
            return false;
        }
        if (lhs.y != rhs.y) {
            return false;
        }
        
        return true;
    }
    
    bool operator!=(const Vector2& lhs, const Vector2& rhs) {
        if (lhs.x != rhs.x) {
            return true;
        }
        if (lhs.y != rhs.y) {
            return true;
        }
        
        return false;
    }

    bool operator==(const ColorMatrix& lhs, const ColorMatrix& rhs) {
        for (auto i=0;i<20;i++) {
            if (lhs.c[i] != rhs.c[i]) {
                return false;
            }
        }
        
        return true;
    }
    
    bool operator!=(const ColorMatrix& lhs, const ColorMatrix& rhs) {
        for (auto i=0;i<20;i++) {
            if (lhs.c[i] != rhs.c[i]) {
                return true;
            }
        }
        
        return false;
    }
    
    void ColorMatrixMakeIdentify(ColorMatrix &matrix) {
        Matrix4MakeIdentify(matrix.matrix);
        matrix.offset.r = matrix.offset.g = matrix.offset.b = matrix.offset.a = 0;
    }
    
    void ColorMatrixMultiply(ColorMatrix& matrix, ColorMatrix& left, ColorMatrix& right) {
        // First Column
        matrix.c[0] = left.c[0]*right.c[0] + left.c[4]*right.c[1] + left.c[8]*right.c[2] + left.c[12]*right.c[3];
        matrix.c[1] = left.c[1]*right.c[0] + left.c[5]*right.c[1] + left.c[9]*right.c[2] + left.c[13]*right.c[3];
        matrix.c[2] = left.c[2]*right.c[0] + left.c[6]*right.c[1] + left.c[10]*right.c[2] + left.c[14]*right.c[3];
        matrix.c[3] = left.c[3]*right.c[0] + left.c[7]*right.c[1] + left.c[11]*right.c[2] + left.c[15]*right.c[3];
        matrix.c[16] = left.c[16]*right.c[0] + left.c[17]*right.c[1] + left.c[18]*right.c[2] + left.c[19]*right.c[3] + right.c[16];
        
        // Second Column
        matrix.c[4] = left.c[0]*right.c[4] + left.c[4]*right.c[5] + left.c[8]*right.c[6] + left.c[12]*right.c[7];
        matrix.c[5] = left.c[1]*right.c[4] + left.c[5]*right.c[5] + left.c[9]*right.c[6] + left.c[13]*right.c[7];
        matrix.c[6] = left.c[2]*right.c[4] + left.c[6]*right.c[5] + left.c[10]*right.c[6] + left.c[14]*right.c[7];
        matrix.c[7] = left.c[3]*right.c[4] + left.c[7]*right.c[5] + left.c[11]*right.c[6] + left.c[15]*right.c[7];
        matrix.c[17] = left.c[16]*right.c[4] + left.c[17]*right.c[5] + left.c[18]*right.c[6] + left.c[19]*right.c[7] + right.c[17];
        
        // Third Column
        matrix.c[8] = left.c[0]*right.c[8] + left.c[4]*right.c[9] + left.c[8]*right.c[10] + left.c[12]*right.c[11];
        matrix.c[9] = left.c[1]*right.c[8] + left.c[5]*right.c[9] + left.c[9]*right.c[10] + left.c[13]*right.c[11];
        matrix.c[10] = left.c[2]*right.c[8] + left.c[6]*right.c[9] + left.c[10]*right.c[10] + left.c[14]*right.c[11];
        matrix.c[11] = left.c[3]*right.c[8] + left.c[7]*right.c[9] + left.c[11]*right.c[10] + left.c[15]*right.c[11];
        matrix.c[18] = left.c[16]*right.c[8] + left.c[17]*right.c[9] + left.c[18]*right.c[10] + left.c[19]*right.c[11] + right.c[18];
        
        // Fourth Column
        matrix.c[12] = left.c[0]*right.c[12] + left.c[4]*right.c[13] + left.c[8]*right.c[14] + left.c[12]*right.c[15];
        matrix.c[13] = left.c[1]*right.c[12] + left.c[5]*right.c[13] + left.c[9]*right.c[14] + left.c[13]*right.c[15];
        matrix.c[14] = left.c[2]*right.c[12] + left.c[6]*right.c[13] + left.c[10]*right.c[14] + left.c[14]*right.c[15];
        matrix.c[15] = left.c[3]*right.c[12] + left.c[7]*right.c[13] + left.c[11]*right.c[14] + left.c[15]*right.c[15];
        matrix.c[19] = left.c[16]*right.c[12] + left.c[17]*right.c[13] + left.c[18]*right.c[14] + left.c[19]*right.c[15] + right.c[19];
    }

    ColorMatrix operator*(ColorMatrix& lhs, ColorMatrix& rhs) {
        ColorMatrix matrix;

        // First Column
        matrix.c[0] = lhs.c[0]*rhs.c[0] + lhs.c[4]*rhs.c[1] + lhs.c[8]*rhs.c[2] + lhs.c[12]*rhs.c[3];
        matrix.c[1] = lhs.c[1]*rhs.c[0] + lhs.c[5]*rhs.c[1] + lhs.c[9]*rhs.c[2] + lhs.c[13]*rhs.c[3];
        matrix.c[2] = lhs.c[2]*rhs.c[0] + lhs.c[6]*rhs.c[1] + lhs.c[10]*rhs.c[2] + lhs.c[14]*rhs.c[3];
        matrix.c[3] = lhs.c[3]*rhs.c[0] + lhs.c[7]*rhs.c[1] + lhs.c[11]*rhs.c[2] + lhs.c[15]*rhs.c[3];
        matrix.c[16] = lhs.c[16]*rhs.c[0] + lhs.c[17]*rhs.c[1] + lhs.c[18]*rhs.c[2] + lhs.c[19]*rhs.c[3] + rhs.c[16];
        
        // Second Column
        matrix.c[4] = lhs.c[0]*rhs.c[4] + lhs.c[4]*rhs.c[5] + lhs.c[8]*rhs.c[6] + lhs.c[12]*rhs.c[7];
        matrix.c[5] = lhs.c[1]*rhs.c[4] + lhs.c[5]*rhs.c[5] + lhs.c[9]*rhs.c[6] + lhs.c[13]*rhs.c[7];
        matrix.c[6] = lhs.c[2]*rhs.c[4] + lhs.c[6]*rhs.c[5] + lhs.c[10]*rhs.c[6] + lhs.c[14]*rhs.c[7];
        matrix.c[7] = lhs.c[3]*rhs.c[4] + lhs.c[7]*rhs.c[5] + lhs.c[11]*rhs.c[6] + lhs.c[15]*rhs.c[7];
        matrix.c[17] = lhs.c[16]*rhs.c[4] + lhs.c[17]*rhs.c[5] + lhs.c[18]*rhs.c[6] + lhs.c[19]*rhs.c[7] + rhs.c[17];
        
        // Third Column
        matrix.c[8] = lhs.c[0]*rhs.c[8] + lhs.c[4]*rhs.c[9] + lhs.c[8]*rhs.c[10] + lhs.c[12]*rhs.c[11];
        matrix.c[9] = lhs.c[1]*rhs.c[8] + lhs.c[5]*rhs.c[9] + lhs.c[9]*rhs.c[10] + lhs.c[13]*rhs.c[11];
        matrix.c[10] = lhs.c[2]*rhs.c[8] + lhs.c[6]*rhs.c[9] + lhs.c[10]*rhs.c[10] + lhs.c[14]*rhs.c[11];
        matrix.c[11] = lhs.c[3]*rhs.c[8] + lhs.c[7]*rhs.c[9] + lhs.c[11]*rhs.c[10] + lhs.c[15]*rhs.c[11];
        matrix.c[18] = lhs.c[16]*rhs.c[8] + lhs.c[17]*rhs.c[9] + lhs.c[18]*rhs.c[10] + lhs.c[19]*rhs.c[11] + rhs.c[18];
        
        // Fourth Column
        matrix.c[12] = lhs.c[0]*rhs.c[12] + lhs.c[4]*rhs.c[13] + lhs.c[8]*rhs.c[14] + lhs.c[12]*rhs.c[15];
        matrix.c[13] = lhs.c[1]*rhs.c[12] + lhs.c[5]*rhs.c[13] + lhs.c[9]*rhs.c[14] + lhs.c[13]*rhs.c[15];
        matrix.c[14] = lhs.c[2]*rhs.c[12] + lhs.c[6]*rhs.c[13] + lhs.c[10]*rhs.c[14] + lhs.c[14]*rhs.c[15];
        matrix.c[15] = lhs.c[3]*rhs.c[12] + lhs.c[7]*rhs.c[13] + lhs.c[11]*rhs.c[14] + lhs.c[15]*rhs.c[15];
        matrix.c[19] = lhs.c[16]*rhs.c[12] + lhs.c[17]*rhs.c[13] + lhs.c[18]*rhs.c[14] + lhs.c[19]*rhs.c[15] + rhs.c[19];

        return matrix;
    }

    bool operator==(const ColorTransform& lhs, const ColorTransform& rhs) {
        if (lhs.c[0] != rhs.c[0]) {
            return false;
        }
        if (lhs.c[1] != rhs.c[1]) {
            return false;
        }
        if (lhs.c[2] != rhs.c[2]) {
            return false;
        }
        if (lhs.c[3] != rhs.c[3]) {
            return false;
        }
        if (lhs.c[4] != rhs.c[4]) {
            return false;
        }
        if (lhs.c[5] != rhs.c[5]) {
            return false;
        }
        if (lhs.c[6] != rhs.c[6]) {
            return false;
        }
        if (lhs.c[7] != rhs.c[7]) {
            return false;
        }
        if (lhs.c[8] != rhs.c[8]) {
            return false;
        }

        return true;
    }
    
    bool operator!=(const ColorTransform& lhs, const ColorTransform& rhs) {
        if (lhs.c[0] != rhs.c[0]) {
            return true;
        }
        if (lhs.c[1] != rhs.c[1]) {
            return true;
        }
        if (lhs.c[2] != rhs.c[2]) {
            return true;
        }
        if (lhs.c[3] != rhs.c[3]) {
            return true;
        }
        if (lhs.c[4] != rhs.c[4]) {
            return true;
        }
        if (lhs.c[5] != rhs.c[5]) {
            return true;
        }
        if (lhs.c[6] != rhs.c[6]) {
            return true;
        }
        if (lhs.c[7] != rhs.c[7]) {
            return true;
        }
        if (lhs.c[8] != rhs.c[8]) {
            return true;
        }
        
        return false;
    }
    
    ColorTransform operator*(ColorTransform& lhs, ColorTransform& rhs) {
        ColorTransform xform;
        
        xform.multiplier.r = lhs.multiplier.r * rhs.multiplier.r;
        xform.multiplier.g = lhs.multiplier.g * rhs.multiplier.g;
        xform.multiplier.b = lhs.multiplier.b * rhs.multiplier.b;
        xform.multiplier.a = lhs.multiplier.a * rhs.multiplier.a;
        
        xform.offset.r = lhs.offset.r + lhs.multiplier.r * rhs.offset.r;
        xform.offset.g = lhs.offset.g + lhs.multiplier.g * rhs.offset.g;
        xform.offset.b = lhs.offset.b + lhs.multiplier.b * rhs.offset.b;
        xform.offset.a = lhs.offset.a + lhs.multiplier.a * rhs.offset.a;
        
        return xform;
    }

    void ColorTransformMakeIdentity(ColorTransform &transform) {
        transform.multiplier.r = 1;
        transform.multiplier.g = 1;
        transform.multiplier.b = 1;
        transform.multiplier.a = 1;
        
        transform.offset.r = 0;
        transform.offset.g = 0;
        transform.offset.b = 0;
        transform.offset.a = 0;
    }

    float MathDegreesToRadians(float degrees)
    {
        return degrees * M_PI_2;
    }
    
    float MathRadiansToDegrees(float radians)
    {
        return radians * ((double)180.0 * M_1_PI);
    }
    
    void Matrix4Make(Matrix4& matrix, float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
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
    
    void Matrix4MakeIdentify(Matrix4& matrix)
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
    
    void Matrix4MakeWithArray(Matrix4& matrix, float values[16])
    {
        memcpy(matrix.m, values, sizeof(matrix.m));
    }
    
    void Matrix4MakeRotation(Matrix4& matrix, float radians, float x, float y, float z)
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
    
    void Matrix4MakeRotationX(Matrix4& matrix, float radians)
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
    
    void Matrix4MakeRotationY(Matrix4& matrix, float radians)
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
    
    void Matrix4MakeRotationZ(Matrix4& matrix, float radians)
    {
        float c = cosf(radians);
        float s = sinf(radians);
        
        matrix.m[0] = c;
        matrix.m[1] = -s;
        matrix.m[2] = 0;
        matrix.m[3] = 0;
        
        matrix.m[4] = s;
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
    
    void Matrix4MakeScale(Matrix4& matrix, float sx, float sy, float sz)
    {
        Matrix4MakeIdentify(matrix);
        
        matrix.m[0] = sx;
        matrix.m[5] = sy;
        matrix.m[10] = sz;
    }
    
    void Matrix4MakeFlashSkew(Matrix4& matrix, float sx, float sy) {
        float c_x = cosf(sx);
        float s_x = sinf(sx);
        float c_y = cosf(sy);
        float s_y = sinf(sy);

        matrix.m[0] = c_y;
        matrix.m[1] = -s_y;
        matrix.m[2] = 0;
        matrix.m[3] = 0;
        
        matrix.m[4] = s_x;
        matrix.m[5] = c_x;
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

    void Matrix4MakeTranslation(Matrix4& matrix, float tx, float ty, float tz)
    {
        Matrix4MakeIdentify(matrix);
        
        matrix.m[12] = tx;
        matrix.m[13] = ty;
        matrix.m[14] = tz;
    }
    
    void Matrix4MakeOrthographic(Matrix4& matrix, float left, float right, float bottom, float top, float near, float far)
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
    
    void Matrix4Multiply(Matrix4& matrix, Matrix4& left, Matrix4& right)
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
    
    
    Matrix4 operator*(Matrix4& left, Matrix4& right) {
        Matrix4 matrix;

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

        return matrix;
    }

    void Matrix4Rotate(Matrix4& matrix, float radians, float x, float y, float z)
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
    
    void Matrix4RotateX(Matrix4& matrix, float radians)
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
    
    void Matrix4RotateY(Matrix4& matrix, float radians)
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
    
    void Matrix4RotateZ(Matrix4& matrix, float radians)
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
    
    void Matrix4Scale(Matrix4& matrix, float sx, float sy, float sz)
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
    
    void Matrix4Translate(Matrix4& matrix, float tx, float ty, float tz)
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
    
    void Matrix4MultiplyVector3(Vector3 &result, Matrix4& matrix, Vector3 v3)
    {
        result.x = matrix.m[0]*v3.x + matrix.m[4]*v3.y + matrix.m[8]*v3.z + matrix.m[12];
        result.y = matrix.m[1]*v3.x + matrix.m[5]*v3.y + matrix.m[9]*v3.z + matrix.m[13];
        result.z = matrix.m[2]*v3.x + matrix.m[6]*v3.y + matrix.m[10]*v3.z + matrix.m[14];
    }
    
    void Matrix4MultiplyVector2(Vector2 &result, Matrix4& matrix, Vector2 v2) {
        result.x = matrix.m[0]*v2.x + matrix.m[4]*v2.y + matrix.m[12];
        result.y = matrix.m[1]*v2.x + matrix.m[5]*v2.y + matrix.m[13];
    }

    bool Matrix2IsIdentity(Matrix2& matrix) {
        if (matrix.m00 != 1) {
            return false;
        }
        
        if (matrix.m01 != 0) {
            return false;
        }
        
        if (matrix.m10 != 0) {
            return false;
        }
        
        if (matrix.m11 != 1) {
            return false;
        }
        
        return true;
    }
    
    bool Matrix3IsIdentity(Matrix3& matrix) {
        if (matrix.m00 != 1) {
            return false;
        }
        
        if (matrix.m01 != 0) {
            return false;
        }
        
        if (matrix.m02 != 0) {
            return false;
        }
        
        if (matrix.m10 != 0) {
            return false;
        }
        
        if (matrix.m11 != 1) {
            return false;
        }
        
        if (matrix.m12 != 0) {
            return false;
        }
        
        if (matrix.m20 != 0) {
            return false;
        }
        
        if (matrix.m21 != 0) {
            return false;
        }
        
        if (matrix.m22 != 1) {
            return false;
        }
        
        return true;
    }
    
    bool Matrix4IsIdentity(Matrix4& matrix) {
        if (matrix.m00 != 1) {
            return false;
        }
        
        if (matrix.m01 != 0) {
            return false;
        }
        
        if (matrix.m02 != 0) {
            return false;
        }
        
        if (matrix.m03 != 0) {
            return false;
        }
        
        if (matrix.m10 != 0) {
            return false;
        }
        
        if (matrix.m11 != 1) {
            return false;
        }
        
        if (matrix.m12 != 0) {
            return false;
        }
        
        if (matrix.m13 != 0) {
            return false;
        }
        
        if (matrix.m20 != 0) {
            return false;
        }
        
        if (matrix.m21 != 0) {
            return false;
        }
        
        if (matrix.m22 != 1) {
            return false;
        }
        
        if (matrix.m23 != 0) {
            return false;
        }
        
        if (matrix.m30 != 0) {
            return false;
        }
        
        if (matrix.m31 != 0) {
            return false;
        }
        
        if (matrix.m32 != 0) {
            return false;
        }
        
        if (matrix.m33 != 1) {
            return false;
        }
        
        return true;
    }

    // Some helpful arguments in regards to
    // https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
    // http://floating-point-gui.de/errors/comparison/
    // https://bitbashing.io/comparing-floats.html
    // https://randomascii.wordpress.com/2012/06/26/doubles-are-not-floats-so-dont-compare-them/
    // https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
    //
    // Keep in mind that float has 7 decimal digits of precision whereas double has 15.


    // ulpsDistance from https://bitbashing.io/comparing-floats.html
    static int32_t ulpsDistance(const float a, const float b)
    {
        // Save work if the floats are equal.
        // Also handles +0 == -0
        if (a == b) return 0;

        const auto max =
        std::numeric_limits<int32_t>::max();

        // Max distance for NaN
        if (isnan(a) || isnan(b)) return max;

        // If one's infinite and they're not equal, max distance.
        if (isinf(a) || isinf(b)) return max;

        int32_t ia, ib;
        memcpy(&ia, &a, sizeof(float));
        memcpy(&ib, &b, sizeof(float));

        // Don't compare differently-signed floats.
        if ((ia < 0) != (ib < 0)) return max;

        // Return the absolute value of the distance in ULPs.
        int32_t distance = ia - ib;
        if (distance < 0) distance = -distance;
        return distance;
    }

    // ulpsDistance from https://bitbashing.io/comparing-floats.html
    static int64_t ulpsDistance(const double a, const double b)
    {
        // Save work if the doubles are equal.
        // Also handles +0 == -0
        if (a == b) return 0;

        const auto max =
        std::numeric_limits<int64_t>::max();

        // Max distance for NaN
        if (isnan(a) || isnan(b)) return max;

        // If one's infinite and they're not equal, max distance.
        if (isinf(a) || isinf(b)) return max;

        int64_t ia, ib;
        memcpy(&ia, &a, sizeof(double));
        memcpy(&ib, &b, sizeof(double));

        // Don't compare differently-signed floats.
        if ((ia < 0) != (ib < 0)) return max;

        // Return the absolute value of the distance in ULPs.
        int64_t distance = ia - ib;
        if (distance < 0) distance = -distance;
        return distance;
    }

    bool nearlyZero(float a, float epsilon) {
        if (a < 0.0F) {
            a = -a;
        }
        return a <= epsilon;
    }

    bool nearlyZero(double a, double epsilon) {
        if (a < 0.0) {
            a = -a;
        }
        return a <= epsilon;
    }

    bool nearlyEqual(float a, float b, float epsilon, int32_t ulpsEpsilon) {
        const auto diff = fabs(a-b);
        if (diff <= epsilon) {
            return true;
        }
        return ulpsDistance(a, b) <= ulpsEpsilon;
    }

    bool nearlyEqual(double a, double b, double epsilon, int64_t ulpsEpsilon) {
        const auto diff = fabs(a-b);
        if (diff <= epsilon) {
            return true;
        }
        return ulpsDistance(a, b) <= ulpsEpsilon;
    }

    bool notNearlyZero(float a, float epsilon) {
        if (a < 0.0F) {
            a = -a;
        }
        return a > epsilon;
    }

    bool notNearlyZero(double a, double epsilon) {
        if (a < 0.0) {
            a = -a;
        }
        return a > epsilon;
    }

    bool notNearlyEqual(float a, float b, float epsilon, int32_t ulpsEpsilon) {
        const auto diff = fabs(a-b);
        if (diff <= epsilon) {
            return false;
        }
        return ulpsDistance(a, b) > ulpsEpsilon;
    }

    bool notNearlyEqual(double a, double b, double epsilon, int64_t ulpsEpsilon) {
        const auto diff = fabs(a-b);
        if (diff <= epsilon) {
            return false;
        }
        return ulpsDistance(a, b) > ulpsEpsilon;
    }

    bool nearlyGreaterThanOrEqual(float a, float b, float epsilon, int32_t ulpsEpsilon) {
        if (a > b) {
            return true;
        }
        return nearlyEqual(a, b, epsilon, ulpsEpsilon);
    }

    bool nearlyGreaterThanOrEqual(double a, double b, double epsilon, int64_t ulpsEpsilon) {
        if (a > b) {
            return true;
        }
        return nearlyEqual(a, b, epsilon, ulpsEpsilon);
    }

    bool nearlyLessThanOrEqual(float a, float b, float epsilon, int32_t ulpsEpsilon) {
        if (a < b) {
            return true;
        }
        return nearlyEqual(a, b, epsilon, ulpsEpsilon);
    }

    bool nearlyLessThanOrEqual(double a, double b, double epsilon, int64_t ulpsEpsilon) {
        if (a < b) {
            return true;
        }
        return nearlyEqual(a, b, epsilon, ulpsEpsilon);
    }
}

