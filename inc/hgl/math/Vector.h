#pragma once

#include<glm/glm.hpp>

namespace hgl::math
{
    // Vector type aliases
    using Vector2f = glm::vec2;
    using Vector2d = glm::dvec2;
    using Vector2i = glm::ivec2;
    using Vector2ui = glm::uvec2;
    
    using Vector3f = glm::vec3;
    using Vector3d = glm::dvec3;
    using Vector3i = glm::ivec3;
    using Vector3ui = glm::uvec3;
    
    using Vector4f = glm::vec4;
    using Vector4d = glm::dvec4;
    using Vector4i = glm::ivec4;
    using Vector4ui = glm::uvec4;
    
    // Common vector constants
    constexpr Vector3f ZeroVector3f = Vector3f(0.0f, 0.0f, 0.0f);
    constexpr Vector3f OneVector3f = Vector3f(1.0f, 1.0f, 1.0f);
    
    constexpr Vector2f ZeroVector2f = Vector2f(0.0f, 0.0f);
    constexpr Vector2f OneVector2f = Vector2f(1.0f, 1.0f);
    
    constexpr Vector4f ZeroVector4f = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    constexpr Vector4f OneVector4f = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
}//namespace hgl::math
