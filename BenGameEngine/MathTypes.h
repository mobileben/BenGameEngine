//
//  MathTypes.h
//  GamePlayground
//
//  Created by Benjamin Lee on 2/10/16.
//  Copyright © 2016 2n Productions. All rights reserved.
//

#ifndef MathTypes_h
#define MathTypes_h

#include <stdio.h>
#include <functional>

namespace BGE {
    typedef struct {
        float x, y;
        float w, h;
    } Rect;
    
    bool operator==(const Rect& lhs, const Rect& rhs);
    bool operator!=(const Rect& lhs, const Rect& rhs);

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
    } Matrix4;
    
    typedef union {
        struct {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
        };
        float m[9];
    } Matrix3;
    
    typedef union {
        struct {
            float m00, m01;
            float m10, m11;
        };
        float m2[2][2];
        float m[4];
    } Matrix2;
    
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
    } Vector4;

    bool operator==(const Vector4& lhs, const Vector4& rhs);
    bool operator!=(const Vector4& lhs, const Vector4& rhs);

    typedef Vector4 Color;
    
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
    } Vector3;
    
    bool operator==(const Vector3& lhs, const Vector3& rhs);
    bool operator!=(const Vector3& lhs, const Vector3& rhs);

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
    } Vector2;
    
    bool operator==(const Vector2& lhs, const Vector2& rhs);
    bool operator!=(const Vector2& lhs, const Vector2& rhs);

    typedef union {
        struct { Vector3 v; float s; };
        struct { float x, y, z, w; };
        float q[4];
    } Quaternion;
    
    typedef union {
        struct {
            Matrix4 matrix;
            Color offset;
        };
        float c[20];
    } ColorMatrix;
    
    bool operator==(const ColorMatrix& lhs, const ColorMatrix& rhs);
    bool operator!=(const ColorMatrix& lhs, const ColorMatrix& rhs);

    extern void ColorMatrixMakeIdentify(ColorMatrix &matrix);
    extern void ColorMatrixMultiply(ColorMatrix& matrix, ColorMatrix& left, ColorMatrix& right);
    ColorMatrix operator*(ColorMatrix& lhs, ColorMatrix& rhs);

    typedef union {
        struct {
            Color multiplier;
            Color offset;
            float color;
        };
        float c[9];
    } ColorTransform;
    
    bool operator==(const ColorTransform& lhs, const ColorTransform& rhs);
    bool operator!=(const ColorTransform& lhs, const ColorTransform& rhs);

    extern void ColorTransformMakeIdentity(ColorTransform &transform);
    
    extern float MathDegreesToRadians(float degrees);
    extern float MathRadiansToDegrees(float radians);
    
    extern void Matrix4Make(Matrix4& matrix, float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33);
    
    extern void Matrix4MakeIdentify(Matrix4& matrix);
    
    extern void Matrix4MakeWithArray(Matrix4& matrix, float values[16]);
    
    extern void Matrix4MakeRotation(Matrix4& matrix, float radians, float x, float y, float z);
    extern void Matrix4MakeRotationX(Matrix4& matrix, float radians);
    extern void Matrix4MakeRotationY(Matrix4& matrix, float radians);
    extern void Matrix4MakeRotationZ(Matrix4& matrix, float radians);
    
    extern void Matrix4MakeScale(Matrix4& matrix, float sx, float sy, float sz);
    
    extern void Matrix4MakeTranslation(Matrix4& matrix, float tx, float ty, float tz);
    
    extern void Matrix4MakeOrthographic(Matrix4& matrix, float left, float right, float bottom, float top, float near, float far);
    
    extern void Matrix4Multiply(Matrix4& matrix, Matrix4& left, Matrix4& right);
    
    extern void Matrix4Rotate(Matrix4& matrix, float radians, float x, float y, float z);
    extern void Matrix4RotateX(Matrix4& matrix, float radians);
    extern void Matrix4RotateY(Matrix4& matrix, float radians);
    extern void Matrix4RotateZ(Matrix4& matrix, float radians);
    
    extern void Matrix4Scale(Matrix4& matrix, float sx, float sy, float sz);
    extern void Matrix4Translate(Matrix4& matrix, float tx, float ty, float tz);
    
    extern void Matrix4MultiplyVector3(Vector3 &result, Matrix4& matrix, Vector3 vector3);
        
    Matrix4 operator*(Matrix4& lhs, Matrix4& rhs);

    extern bool Matrix2IsIdentity(Matrix2& matrix);
    extern bool Matrix3IsIdentity(Matrix3& matrix);
    extern bool Matrix4IsIdentity(Matrix4& matrix);
}

namespace std {
    template <>
    struct hash<BGE::Rect> {
        size_t operator()(const BGE::Rect &r) const {
            size_t prime = 31;
            size_t result = 1;
            
            result = prime * result + hash<float>()(r.x);
            result = prime * result + hash<float>()(r.y);
            result = prime * result + hash<float>()(r.w);
            result = prime * result + hash<float>()(r.h);
            
            return result;
        }
    };

    template <>
    struct hash<BGE::Vector2> {
        size_t operator()(const BGE::Vector2 &v) const {
            size_t prime = 31;
            size_t result = 1;
            
            result = prime * result + hash<float>()(v.x);
            result = prime * result + hash<float>()(v.y);
            
            return result;
        }
    };
    
    template <>
    struct hash<BGE::Vector3> {
        size_t operator()(const BGE::Vector3 &v) const {
            size_t prime = 31;
            size_t result = 1;
            
            result = prime * result + hash<float>()(v.x);
            result = prime * result + hash<float>()(v.y);
            result = prime * result + hash<float>()(v.z);
            
            return result;
        }
    };
    
    template <>
    struct hash<BGE::Vector4> {
        size_t operator()(const BGE::Vector4 &v) const {
            size_t prime = 31;
            size_t result = 1;
            
            result = prime * result + hash<float>()(v.x);
            result = prime * result + hash<float>()(v.y);
            result = prime * result + hash<float>()(v.z);
            result = prime * result + hash<float>()(v.w);
            
            return result;
        }
    };
    
    template <>
    struct hash<BGE::ColorTransform> {
        size_t operator()(const BGE::ColorTransform &c) const {
            size_t prime = 31;
            size_t result = 1;
            
            result = prime * result + hash<float>()(c.c[0]);
            result = prime * result + hash<float>()(c.c[1]);
            result = prime * result + hash<float>()(c.c[2]);
            result = prime * result + hash<float>()(c.c[3]);
            result = prime * result + hash<float>()(c.c[4]);
            result = prime * result + hash<float>()(c.c[5]);
            result = prime * result + hash<float>()(c.c[6]);
            result = prime * result + hash<float>()(c.c[7]);
            result = prime * result + hash<float>()(c.c[8]);
            
            return result;
        }
    };
    
    template <>
    struct hash<BGE::ColorMatrix> {
        size_t operator()(const BGE::ColorMatrix &c) const {
            size_t prime = 31;
            size_t result = 1;
            
            for (auto i=0;i<20;i++) {
                result = prime * result + hash<float>()(c.c[i]);
            }
            
            return result;
        }
    };
}

#endif /* MathTypes_h */
