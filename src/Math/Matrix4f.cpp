#include<hgl/math/Matrix.h>

// Vulkan Cookbook
// ISBN: 9781786468154
// Packt Publishing Limited
// git: https://github.com/PacktPublishing/Vulkan-Cookbook.git
//
// Author:   Pawel Lapinski
// LinkedIn: https://www.linkedin.com/in/pawel-lapinski-84522329
//
// Chapter: 10 Helper Recipes
// Recipe:  04 Preparing a perspective projection matrix
// Recipe:  05 Preparing an orthographic projection matrix

namespace hgl
{
    Matrix4f ortho( float left,
                    float right,
                    float bottom,
                    float top,
                    float znear,
                    float zfar )
    {
        return Matrix4f(
          2.0f / (right - left),
          0.0f,
          0.0f,
          0.0f,

          0.0f,
          2.0f / (bottom - top),
          0.0f,
          0.0f,

          0.0f,
          0.0f,
          1.0f / (znear - zfar),
          0.0f,

          -(right + left) / (right - left),
          -(bottom + top) / (bottom - top),
          znear / (znear - zfar),
          1.0f
        );
    }

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     * @param znear 近平面z值
     * @param zfar 远平台z值
     */
    Matrix4f ortho(float width,float height,float znear,float zfar)
    {        
        return ortho(0.0f,width,height,0.0f,znear,zfar);
    }

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     */
    Matrix4f ortho(float width,float height)
    {    
        return ortho(width,height,0.0f,1.0f);
    }

    /**
     * 生成一个透视矩阵
     * @param aspect_ratio 宽高比
     * @param field_of_view 视野
     * @param znear 近截面
     * @param zfar 远截面
     */
    Matrix4f perspective(   float field_of_view,
                            float aspect_ratio,
                            float znear,
                            float zfar)
    {
        float f = 1.0f / tanf( deg2rad( 0.5f * field_of_view ) );

        return Matrix4f(
          f / aspect_ratio,
          0.0f,
          0.0f,
          0.0f,

          0.0f,
          -f,
          0.0f,
          0.0f,

          0.0f,
          0.0f,
          zfar / (znear - zfar),
          -1.0f,
      
          0.0f,
          0.0f,
          (znear * zfar) / (znear - zfar),
          0.0f
        );
    }

    Matrix4f lookat(const Vector3f &eye,const Vector3f &target,const Vector3f &up)
    {
        Vector3f forward=normalize(target-eye);
        Vector3f right=normalize(cross(forward,up));

        Vector3f nup=cross(right,forward);

        return Matrix4f(   right.x,
                             nup.x,
                        -forward.x,
                              0.0f,

                           right.y,
                             nup.y,
                        -forward.y,
                              0.0f,

                           right.z,
                             nup.z,
                   -forward.z/2.0f,
                              0.0f,

                 -dot(eye,right  ),
                 -dot(eye,nup    ),
                  dot(eye,forward),
                              1.0f
        );
    }
}//namespace hgl
