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
    Matrix4f OrthoMatrix( float left,
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
    Matrix4f OrthoMatrix(float width,float height,float znear,float zfar)
    {
        return OrthoMatrix(0.0f,width,height,0.0f,znear,zfar);
    }

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     */
    Matrix4f OrthoMatrix(float width,float height)
    {
        return OrthoMatrix(width,height,0.0f,1.0f);
    }

    /**
     * 生成一个透视矩阵
     * @param field_of_view 视野(角度)
     * @param aspect_ratio 宽高比
     * @param znear 近截面
     * @param zfar 远截面
     */
    Matrix4f PerspectiveMatrix( float field_of_view,
                                float aspect_ratio,
                                float znear,
                                float zfar)
    {
        float f = 1.0f / tanf( deg2rad( 0.5f * field_of_view ) );

        return Matrix4f(
          -f / aspect_ratio,
          0.0f,
          0.0f,
          0.0f,

          0.0f,
          -f,
          0.0f,
          0.0f,

          0.0f,
          0.0f,
          zfar/(znear-zfar),
          -1.0f,

          0.0f,
          0.0f,
          (znear * zfar) / (znear-zfar),
          0.0f
        );
    }

    Matrix4f LookAtMatrix(const Vector3f &eye,const Vector3f &target,const Vector3f &up)
    {
        Vector3f forward=normalize(target-eye);
        Vector3f right  =normalize(cross(forward,up));
        Vector3f nup    =cross(right,forward);

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
                        -forward.z,
                              0.0f,

                 -dot(eye,right  ),
                 -dot(eye,nup    ),
                  dot(eye,forward),
                              1.0f
        );

        //经查证，此代码完全等于glm::lookAtRH，无任何差别
    }

    Vector2i ProjectToScreen(
        const Vector3f &world_pos,
        const Matrix4f &view,
        const Matrix4f &projection,
        const Vector2u &vp_size)
    {
        // 1. 世界坐标 -> 裁剪空间
        Vector4f clip = projection * view * Vector4f(world_pos.x,world_pos.y,world_pos.z, 1.0f);       //需要转换到OPENGL坐标系

        // 2. 齐次除法，得到 NDC（注意Z为0~1）
        if (clip.w == 0.0f)
            return Vector2i(0, 0);

        Vector3f ndc;

        ndc.x = clip.x / clip.w;
        ndc.y = clip.y / clip.w;
        ndc.z = clip.z / clip.w; // 0~1

        // 3. NDC -> 屏幕空间
        float screen_x = (ndc.x + 1.0f) * 0.5f * float(vp_size.x);
        float screen_y = (ndc.y + 1.0f) * 0.5f * float(vp_size.y); // Y轴向下为正

        return Vector2i(screen_x, screen_y);
    }

    Vector3f UnProjectToWorld(
        const Vector2i &win_pos,
        const Matrix4f &view,
        const Matrix4f &projection,
        const Vector2u &vp_size)
    {
        // 1. 归一化到 NDC（[-1, 1]）
        float ndc_x = (2.0f * float(win_pos.x)) / float(vp_size.x) - 1.0f;
        float ndc_y = (2.0f * float(win_pos.y)) / float(vp_size.y) - 1.0f;
        // 这里假设在近平面（z=0），如需支持深度可加参数
        float ndc_z = 0.0f;

        Vector4f ndc_pos(ndc_x, ndc_y, ndc_z, 1.0f);

        // 2. 逆变换到世界空间
        Matrix4f inv_proj_view = inverse(projection * view);
        Vector4f world_pos = inv_proj_view * ndc_pos;

        // 3. 齐次除法
        if (world_pos.w == 0.0f)
            return Vector3f(0, 0, 0);

        return Vector3f(world_pos.x / world_pos.w, world_pos.y / world_pos.w, world_pos.z / world_pos.w);
    }

    // 函数用于从 glm::mat4 中提取平移、旋转和缩放
    bool DecomposeTransform(const Matrix4f & transform, Vector3f & outTranslation, Quatf & outRotation, Vector3f & outScale)
    {
        using namespace glm;
        mat4 LocalMatrix(transform);

        // 提取平移
        outTranslation = vec3(LocalMatrix[3]);

        // 提取缩放和旋转
        vec3 Row[3];

        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        // 提取缩放
        outScale.x = length(Row[0]);
        outScale.y = length(Row[1]);
        outScale.z = length(Row[2]);

        // 归一化行向量以提取旋转
        if (outScale.x) Row[0] /= outScale.x;
        if (outScale.y) Row[1] /= outScale.y;
        if (outScale.z) Row[2] /= outScale.z;

        // 提取旋转
        mat3 RotationMatrix(Row[0], Row[1], Row[2]);
        outRotation = quat_cast(RotationMatrix);

        return true;
    }

    /**
    * 计算一个方向旋转成另一个方向的变换矩阵
    */
    const Matrix4f GetRotateMatrix(const Vector3f &world_position,const Vector3f &old_direction,const Vector3f &new_direction)
    {
        Vector3f axis=glm::cross(old_direction,new_direction);

        if(glm::length2(axis)<0.0001)
            return Matrix4f(1.0f);

        axis=glm::normalize(axis);

        float angle=acos(glm::dot(old_direction,new_direction));

        return glm::rotate(Matrix4f(1.0f),angle,axis);
    }

    /**
    * 计算一个方向旋转成另一个方向的四元数
    */
    const Quatf GetRotateQuat(const Vector3f &world_position,const Vector3f &old_direction,const Vector3f &new_direction)
    {
        Vector3f axis=glm::cross(old_direction,new_direction);

        if(glm::length2(axis)<0.0001)
            return Quatf(1.0f,0.0f,0.0f,0.0f);

        axis=glm::normalize(axis);

        float angle=acos(glm::dot(old_direction,new_direction));

        return glm::angleAxis(angle,axis);
    }
}//namespace hgl
