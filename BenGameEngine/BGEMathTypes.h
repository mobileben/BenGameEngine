//
//  BGEMathTypes.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef BGEMathTypes_h
#define BGEMathTypes_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct {
        float x, y;
        float w, h;
    } BGERect;
    
    // Matrices are column major order
    // Convention for an element is m01, where 0 is the column and 1 is the row
    typedef union {
        struct {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
        float m[16];
    } BGEMatrix4;
    
    typedef union {
        struct {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
        };
        float m[9];
    } BGEMatrix3;
    
    typedef union {
        struct {
            float m00, m01;
            float m10, m11;
        };
        float m2[2][2];
        float m[4];
    } BGEMatrix2;
    
    typedef union  {
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        };
        struct {
            float s, t, p, q;
        };
        float v[4];
    } BGEVector4;
    
    typedef union  {
        struct {
            float x, y, z;
        };
        struct {
            float r, g, b;
        };
        struct {
            float s, t, p;
        };
        float v[3];
    } BGEVector3;
    
    typedef union  {
        struct {
            float x, y;
        };
        struct {
            float w, h;
        };
        struct {
            float s, t;
        };
        float v[2];
    } BGEVector2;

    typedef union {
        struct { BGEVector3 v; float s; };
        struct { float x, y, z, w; };
        float q[4];
    } BGEQuaternion;
    
    extern float BGEMathDegreesToRadians(float degrees);
    extern float BGEMathRadiansToDegrees(float radians);
    
    extern void BGEMatrix4Make(BGEMatrix4& matrix, float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33);
    
    extern void BGEMatrix4MakeIdentify(BGEMatrix4& matrix);
    
    extern void BGEMatrix4MakeWithArray(BGEMatrix4& matrix, float values[16]);
    
    extern void BGEMatrix4MakeRotation(BGEMatrix4& matrix, float radians, float x, float y, float z);
    extern void BGEMatrix4MakeRotationX(BGEMatrix4& matrix, float radians);
    extern void BGEMatrix4MakeRotationY(BGEMatrix4& matrix, float radians);
    extern void BGEMatrix4MakeRotationZ(BGEMatrix4& matrix, float radians);
    
    extern void BGEMatrix4MakeScale(BGEMatrix4& matrix, float sx, float sy, float sz);
    
    extern void BGEMatrix4MakeTranslation(BGEMatrix4& matrix, float tx, float ty, float tz);
    
    extern void BGEMatrix4MakeOrthographic(BGEMatrix4& matrix, float left, float right, float bottom, float top, float near, float far);
    
    extern void BGEMatrix4Multiply(BGEMatrix4& matrix, BGEMatrix4& left, BGEMatrix4& right);
    
    extern void BGEMatrix4Rotate(BGEMatrix4& matrix, float radians, float x, float y, float z);
    extern void BGEMatrix4RotateX(BGEMatrix4& matrix, float radians);
    extern void BGEMatrix4RotateY(BGEMatrix4& matrix, float radians);
    extern void BGEMatrix4RotateZ(BGEMatrix4& matrix, float radians);
    
    extern void BGEMatrix4Scale(BGEMatrix4& matrix, float sx, float sy, float sz);
    extern void BGEMatrix4Translate(BGEMatrix4& matrix, float tx, float ty, float tz);
    
    extern void BGEMatrix4MultiplyVector3(BGEVector3 &result, BGEMatrix4& matrix, BGEVector3 vector3);
    
    extern bool BGEMatrix2IsIdentity(BGEMatrix2& matrix);
    extern bool BGEMatrix3IsIdentity(BGEMatrix3& matrix);
    extern bool BGEMatrix4IsIdentity(BGEMatrix4& matrix);
    
#ifdef __cplusplus
}

#endif

#endif /* BGEMathTypes_h */
