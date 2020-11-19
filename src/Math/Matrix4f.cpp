#include<hgl/math/Matrix.h>

// Vulkan Cookbook
// ISBN: 9781786468154
// Packt Publishing Limited
//
// Author:   Pawel Lapinski
// LinkedIn: https://www.linkedin.com/in/pawel-lapinski-84522329
//
// Chapter: 10 Helper Recipes
// Recipe:  04 Preparing a perspective projection matrix
// Recipe:  05 Preparing an orthographic projection matrix

// 注：我方便用row矩阵，所以需要将Vulkan Cookbook中的矩转旋转参照

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
            2.0f / (right - left),  0.0f,                   0.0f,                   -(right + left) / (right - left),
            0.0f,                   2.0f / (bottom - top),  0.0f,                   -(bottom + top) / (bottom - top),
            0.0f,                   0.0f,                   1.0f / (znear - zfar),  znear / (znear - zfar),
            0.0f,                   0.0f,                   0.0f,                   1.0f);
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
        return Matrix4f(
            2.0f / width,   0.0f,           0.0f,                   -1,
            0.0f,           2.0f / height,  0.0f,                   -1,
            0.0f,           0.0f,           1.0f / (znear - zfar),  znear / (znear - zfar),
            0.0f,           0.0f,           0.0f,                   1.0f);
    }

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     */
    Matrix4f ortho(float width,float height)
    {
        return Matrix4f(
            2.0f / width,   0.0f,           0.0f,   -1,
            0.0f,           2.0f / height,  0.0f,   -1,
            0.0f,           0.0f,           -1.0f , 0.0f,
            0.0f,           0.0f,           0.0f,   1.0f);
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
        const float f = 1.0f / tan( hgl_ang2rad( 0.5f * field_of_view ) );

        return Matrix4f(
            f / aspect_ratio,   0.0f,   0.0f,                   0.0f,
            0.0f,               -f,     0.0f,                   0.0f,
            0.0f,               0.0f,   zfar / (znear - zfar), (znear * zfar) / (znear - zfar),
                                    //  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                                    //  某些引擎这两项会乘0.5，那是因为他们是 -1 to 1 的Z值设定，而我们是 0 to 1，所以这里不用乘
                                    //  同理，camera的znear为接近0的正数，zfar为一个较大的正数，默认使用16/256

            0.0f,               0.0f,   -1.0f,                  0.0f);
    }

    Matrix4f lookat(const Vector4f &eye,const Vector4f &target,const Vector4f &up)
    {
        Vector4f forward=target-eye;

        normalize(forward);

        Vector4f right=cross(forward,up);

        normalize(right);

        Vector4f nup=cross(right,forward);

        Matrix4f result(   right.x,        right.y,        right.z,         1.0f,
                             nup.x,          nup.y,          nup.z,         1.0f,
                        -forward.x,     -forward.y,     -forward.z/2.0f,    1.0f,
                              0.0f,           0.0f,           0.0f,         1.0f);
                                                    //  ^^^^^^
                                                    //  某些引擎这里为0.5，那是因为他们是 -1 to 1 的Z值设定，而我们是0 to 1，所以这里不用乘
                                                    //  同理，camera的znear为接近0的正数，zfar为一个较大的正数，默认使用16/256

        return result*translate(-eye.xyz());
    }
}//namespace hgl
