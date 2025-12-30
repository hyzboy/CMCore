#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/matrix_decompose.hpp>
#include<hgl/math/MatrixTypes.h>
#include<hgl/math/Vector.h>

// Forward declare Quatf to avoid circular dependency
namespace hgl::math { using Quatf = glm::quat; }

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
        return glm::all(glm::epsilonEqual(m, Identity4f, 1e-5f));
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
    
}//namespace hgl::math
