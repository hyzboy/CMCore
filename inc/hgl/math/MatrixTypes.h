#pragma once

#include<glm/glm.hpp>

namespace hgl::math
{
    // Matrix type aliases
    using Matrix2f = glm::mat2;
    using Matrix2d = glm::dmat2;
    
    using Matrix3f = glm::mat3;
    using Matrix3d = glm::dmat3;
    
    using Matrix4f = glm::mat4;
    using Matrix4d = glm::dmat4;
    
    // Common matrix constants
    const Matrix4f Identity4f = Matrix4f(1.0f);
    const Matrix3f Identity3f = Matrix3f(1.0f);
    const Matrix2f Identity2f = Matrix2f(1.0f);
}//namespace hgl::math
