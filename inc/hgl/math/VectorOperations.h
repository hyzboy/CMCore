#pragma once

#include<glm/glm.hpp>
#include<glm/gtx/norm.hpp>
#include<hgl/math/VectorTypes.h>

namespace hgl::math
{
    // ========================================
    // Vector normalization operations
    // ========================================
    
    /**
     * Return a normalized copy of the vector (PascalCase wrapper for glm::normalize)
     */
    template<typename T>
    inline T Normalized(const T& v)
    {
        return glm::normalize(v);
    }
    
    /**
     * Normalize a vector in-place (PascalCase wrapper)
     */
    template<typename T>
    inline void Normalize(T& v)
    {
        v = glm::normalize(v);
    }
    
    // ========================================
    // Vector cross and dot products
    // ========================================
    
    /**
     * Calculate cross product of two 3D vectors (PascalCase wrapper for glm::cross)
     */
    template<typename T>
    inline T Cross(const T& v1, const T& v2)
    {
        return glm::cross(v1, v2);
    }
    
    /**
     * Calculate dot product of two vectors (PascalCase wrapper for glm::dot)
     */
    template<typename T>
    inline auto Dot(const T& v1, const T& v2) -> decltype(glm::dot(v1, v2))
    {
        return glm::dot(v1, v2);
    }
    
    // ========================================
    // Vector length operations
    // ========================================
    
    /**
     * Calculate length (magnitude) of a vector (PascalCase wrapper for glm::length)
     */
    template<typename T>
    inline auto Length(const T& v) -> decltype(glm::length(v))
    {
        return glm::length(v);
    }
    
    /**
     * Calculate squared length of a vector (PascalCase wrapper for glm::length2)
     */
    template<typename T>
    inline auto LengthSquared(const T& v) -> decltype(glm::length2(v))
    {
        return glm::length2(v);
    }
    
    /**
     * Calculate 2D length (ignoring Z component) of a 3D vector
     */
    template<typename T>
    inline auto Length2D(const glm::vec<3, T, glm::defaultp>& v) -> T
    {
        return glm::length(glm::vec<2, T, glm::defaultp>(v.x, v.y));
    }
    
    /**
     * Calculate squared 2D length (ignoring Z component) of a 3D vector
     */
    template<typename T>
    inline auto LengthSquared2D(const glm::vec<3, T, glm::defaultp>& v) -> T
    {
        return glm::length2(glm::vec<2, T, glm::defaultp>(v.x, v.y));
    }
    
}//namespace hgl::math
