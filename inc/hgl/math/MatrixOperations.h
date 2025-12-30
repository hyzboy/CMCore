#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/matrix_decompose.hpp>
#include<hgl/math/MatrixTypes.h>
#include<hgl/math/Vector.h>
#include<hgl/math/Quaternion.h>

namespace hgl::math
{
    // ========================================
    // Matrix inverse and transpose operations
    // ========================================
    
    /**
     * Calculate the inverse of a matrix (PascalCase wrapper for glm::inverse)
     */
    template<typename T>
    inline T Inverse(const T& m)
    {
        return glm::inverse(m);
    }
    
    /**
     * Calculate the transpose of a matrix (PascalCase wrapper for glm::transpose)
     */
    template<typename T>
    inline T Transpose(const T& m)
    {
        return glm::transpose(m);
    }
    
    // ========================================
    // Matrix creation and transformation helpers
    // ========================================
    
    /**
     * Create a translation matrix
     */
    inline Matrix4f TranslateMatrix(const Vector3f& v)
    {
        return glm::translate(Matrix4f(1.0f), v);
    }
    
    /**
     * Create a scale matrix
     */
    inline Matrix4f ScaleMatrix(const Vector3f& v)
    {
        return glm::scale(Matrix4f(1.0f), v);
    }
    
    /**
     * Create a rotation matrix from axis and angle
     */
    inline Matrix4f AxisRotate(float angle, const Vector3f& axis)
    {
        return glm::rotate(Matrix4f(1.0f), angle, axis);
    }
    
    /**
     * Create a look-at view matrix
     */
    inline Matrix4f LookAtMatrix(const Vector3f& eye, const Vector3f& center, const Vector3f& up)
    {
        return glm::lookAt(eye, center, up);
    }
    
    // ========================================
    // Matrix identity and comparison helpers
    // ========================================
    
    /**
     * Check if a matrix is nearly equal to identity matrix
     */
    inline bool IsIdentityMatrix(const Matrix4f& m)
    {
        constexpr float epsilon = 1e-5f;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                float expected = (col == row) ? 1.0f : 0.0f;
                if (glm::abs(m[col][row] - expected) > epsilon)
                    return false;
            }
        }
        return true;
    }
    
    /**
     * Decompose a 4x4 transformation matrix into translation, rotation, and scale
     */
    inline bool DecomposeTransform(const Matrix4f& m, Vector3f& translation, Quatf& rotation, Vector3f& scale)
    {
        Vector3f skew;
        Vector4f perspective;
        return glm::decompose(m, scale, rotation, translation, skew, perspective);
    }

    // ========================================
    // Vector transformation helpers
    // ========================================

    /**
     * Transform position (considers translation)
     */
    inline Vector3f TransformPosition(const Matrix4f& m, const Vector3f& v)
    {
        return Vector3f(m * Vector4f(v, 1.0f));
    }

    /**
     * Transform direction (ignores translation)
     */
    inline Vector3f TransformDirection(const Matrix4f& m, const Vector3f& v)
    {
        return Vector3f(m * Vector4f(v, 0.0f));
    }

    /**
     * Transform normal (uses inverse transpose matrix)
     */
    inline Vector3f TransformNormal(const Matrix4f& m, const Vector3f& v)
    {
        return Normalized(Vector3f(Transpose(Inverse(m)) * Vector4f(v, 0.0f)));
    }

    /**
     * Transform normal (3x3 matrix version)
     */
    inline Vector3f TransformNormal(const Matrix3f& m, const Vector3f& v)
    {
        return Normalized(m * v);
    }

}//namespace hgl::math
