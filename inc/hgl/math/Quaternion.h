#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>
#include<hgl/math/VectorTypes.h>
#include<hgl/math/MatrixTypes.h>

namespace hgl::math
{
    // Quaternion type aliases
    using Quatf = glm::quat;
    using Quatd = glm::dquat;
    
    // Common quaternion constants
    const Quatf IdentityQuatf = Quatf(1.0f, 0.0f, 0.0f, 0.0f);
    
    // ========================================
    // Quaternion operations
    // ========================================
    
    /**
     * Create a quaternion from axis and angle
     */
    inline Quatf RotationQuat(float angle, const Vector3f& axis)
    {
        return glm::angleAxis(angle, axis);
    }
    
    /**
     * Convert quaternion to a 4x4 matrix
     */
    inline Matrix4f ToMatrix(const Quatf& q)
    {
        return glm::mat4_cast(q);
    }
    
    /**
     * Spherical linear interpolation between two quaternions
     */
    inline Quatf SLerpQuat(const Quatf& from, const Quatf& to, float t)
    {
        return glm::slerp(from, to, t);
    }
    
    /**
     * Extract axis and angle from a quaternion
     */
    inline void ExtractedQuat(const Quatf& q, Vector3f& axis, float& angle)
    {
        angle = glm::angle(q);
        axis = glm::axis(q);
    }
    
    // ========================================
    // Axis vectors and enum
    // ========================================
    
    enum class AXIS
    {
        X,
        Y,
        Z
    };
    
    namespace AxisVector
    {
        const Vector3f X = Vector3f(1.0f, 0.0f, 0.0f);
        const Vector3f Y = Vector3f(0.0f, 1.0f, 0.0f);
        const Vector3f Z = Vector3f(0.0f, 0.0f, 1.0f);
    }
    
    /**
     * Get axis vector from AXIS enum
     */
    inline Vector3f GetAxisVector(AXIS axis)
    {
        switch(axis)
        {
            case AXIS::X: return AxisVector::X;
            case AXIS::Y: return AxisVector::Y;
            case AXIS::Z: return AxisVector::Z;
            default: return AxisVector::Z;
        }
    }
    
    // ========================================
    // Comparison helpers
    // ========================================
    
    /**
     * Check if a value is nearly zero
     */
    template<typename T>
    inline bool IsNearlyZero(T value, T epsilon = static_cast<T>(1e-5))
    {
        return glm::abs(value) < epsilon;
    }
    
    /**
     * Check if a vector is nearly zero
     */
    template<typename T>
    inline bool IsNearlyZero(const glm::vec<3, T, glm::defaultp>& v, T epsilon = static_cast<T>(1e-5))
    {
        return glm::length(v) < epsilon;
    }
    
    /**
     * Check if two values are nearly equal
     */
    template<typename T>
    inline bool IsNearlyEqual(T a, T b, T epsilon = static_cast<T>(1e-5))
    {
        return glm::abs(a - b) < epsilon;
    }
    
    /**
     * Check if two vectors are nearly equal
     */
    template<typename T, glm::length_t L, typename Q>
    inline bool IsNearlyEqual(const glm::vec<L, T, Q>& a, const glm::vec<L, T, Q>& b, T epsilon = static_cast<T>(1e-5))
    {
        return glm::all(glm::epsilonEqual(a, b, epsilon));
    }
    
    /**
     * Check if two quaternions are nearly equal
     */
    inline bool IsNearlyEqual(const Quatf& a, const Quatf& b, float epsilon = 1e-5f)
    {
        return glm::all(glm::epsilonEqual(a, b, epsilon));
    }
    
    /**
     * Check if two matrices are nearly equal
     */
    template<glm::length_t C, glm::length_t R, typename T, typename Q>
    inline bool IsNearlyEqual(const glm::mat<C, R, T, Q>& a, const glm::mat<C, R, T, Q>& b, T epsilon = static_cast<T>(1e-5))
    {
        return glm::all(glm::epsilonEqual(a, b, epsilon));
    }
    
}//namespace hgl::math
