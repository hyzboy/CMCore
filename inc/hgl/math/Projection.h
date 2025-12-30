#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<hgl/math/MatrixTypes.h>

namespace hgl::math
{
    // ========================================
    // Projection matrix helpers
    // ========================================
    
    /**
     * Create a perspective projection matrix
     */
    inline Matrix4f PerspectiveMatrix(float fovy, float aspect, float zNear, float zFar)
    {
        return glm::perspective(fovy, aspect, zNear, zFar);
    }
    
    /**
     * Create an orthographic projection matrix
     */
    inline Matrix4f OrthographicMatrix(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        return glm::ortho(left, right, bottom, top, zNear, zFar);
    }
    
    /**
     * Create a 2D orthographic projection matrix
     */
    inline Matrix4f Ortho2DMatrix(float left, float right, float bottom, float top)
    {
        return glm::ortho(left, right, bottom, top);
    }
    
}//namespace hgl::math
