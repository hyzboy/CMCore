#ifndef HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE
#define HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE

#include<hgl/math/Vector.h>
#include<hgl/TypeFunc.h>
//注：GLM/CML(OpenGLMode)是列矩阵,计算坐标matrix*pos
//   而MGL是行矩阵，需要反过来pos*matrix

namespace hgl
{
    using Matrix3f=float3x3;
    using Matrix4f=float4x4;
    using Matrix3x4f=float3x4;

    inline Matrix4f identity()
    {
        return Matrix4f::identity;
    }

    inline Matrix4f inverse(const Matrix4f &m)
    {
        return m.Inverted();
    }

    /**
     * 生成一个正角视图矩阵
     * @param left 左
     * @param right 右
     * @param top 顶
     * @param bottom 底
     * @param znear 近平面z值
     * @param zfar 远平台z值
     */
    Matrix4f ortho( float left,
                    float right,
                    float bottom,
                    float top,
                    float znear,
                    float zfar );

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     * @param znear 近平面z值
     * @param zfar 远平台z值
     */
    Matrix4f ortho(float width,float height,float znear,float zfar);

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     */
    Matrix4f ortho(float width,float height);

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
                            float zfar);

    /**
     * 产生一个lookat变换矩阵
     * @param eye 眼睛位置
     * @param target 目标位置
     * @param up 向上向量
     */
    Matrix4f lookat(const Vector4f &eye,const Vector4f &target,const Vector4f &up);

    inline Matrix4f translate(const Vector3f &v)
    {
        return Matrix4f::Translate(v);
    }

    inline Matrix4f translate(float x,float y,float z)
    {
        return Matrix4f::Translate(x,y,z);
    }

    inline Matrix4f translate(float x,float y)
    {
        return Matrix4f::Translate(x,y,1.0f);
    }

    inline Matrix4f scale(const Vector3f &v)
    {
        return Matrix4f::Scale(v,Vector3f::zero);
    }

    inline Matrix4f scale(float x,float y,float z)
    {
        return Matrix4f::Scale(Vector3f(x,y,z),Vector3f::zero);
    }

    inline Matrix4f scale(float x,float y)
    {
        return Matrix4f::Scale(Vector3f(x,y,1.0f),Vector3f::zero);
    }

    inline Matrix4f scale(float s)
    {
        return Matrix4f::Scale(Vector3f(s,s,s),Vector3f::zero);
    }

    inline Matrix4f rotate(float angle,const Vector3f &axis)
    {
        return Matrix4f::RotateAxisAngle(axis.Normalized(),angle);
    }

    inline Matrix4f rotate(float angle,float x,float y,float z)
    {
        return rotate(angle,Vector3f(x,y,z));
    }
    
    inline Matrix4f rotate(float angle,float x,float y)
    {
        return rotate(angle,Vector3f(x,y,1.0f));
    }

    inline Matrix4f rotate(float angle,const Vector4f &axis)
    {
        return rotate(angle,Vector3f(axis.x,axis.y,axis.z));
    }

    inline Vector3f rotate(const Vector3f &v3f,float angle,const Vector3f &axis)
    {
        Vector4f result=rotate(angle,axis)*Vector4f(v3f,1.0f);

        return result.xyz();
    }
}//namespace hgl
#endif//HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE
