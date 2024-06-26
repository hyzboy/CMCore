﻿#ifndef HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE
#define HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE

#include<hgl/math/Vector.h>
#include<hgl/TypeFunc.h>
//注：GLM/CML(OpenGLMode)是列矩阵,计算坐标matrix*pos
//   而MGL是行矩阵，需要反过来pos*matrix

#include<glm/gtc/matrix_transform.hpp>

namespace hgl
{
    using Quat4f=glm::quat;

    using Matrix2f=glm::mat2;
    using Matrix3f=glm::mat3;
    using Matrix4f=glm::mat4;
    using Matrix2x4f=glm::mat2x4;
    using Matrix3x4f=glm::mat3x4;
    using Matrix4x2f=glm::mat4x2;
    using Matrix4x3f=glm::mat4x3;

    constexpr const Matrix2f Identity2f=Matrix2f(1.0f);
    constexpr const Matrix3f Identity3f=Matrix3f(1.0f);
    constexpr const Matrix4f Identity4f=Matrix4f(1.0f);
    constexpr const Matrix3f Identity3x4f=Matrix3x4f(1.0f);

    inline Matrix4f inverse(const Matrix4f &m)
    {
        return glm::inverse(m);
    }

    inline Matrix4f transpose(const Matrix4f &m)
    {
        return glm::transpose(m);
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
    Matrix4f lookat(const Vector3f &eye,const Vector3f &target,const Vector3f &up);

    inline Matrix4f translate(const Vector3f &v)
    {
        return glm::translate(Matrix4f(1.0f),v);
    }

    inline Matrix4f translate(float x,float y,float z)
    {
        return glm::translate(Matrix4f(1.0f),Vector3f(x,y,z));
    }

    inline Matrix4f translate(float x,float y)
    {
        return translate(x,y,1.0f);
    }

    inline Matrix4f scale(const Vector3f &v)
    {
        return glm::scale(Matrix4f(1.0f),v);
    }

    inline Matrix4f scale(float x,float y,float z)
    {
        return glm::scale(Matrix4f(1.0f),Vector3f(x,y,z));
    }

    inline Matrix4f scale(float x,float y)
    {
        return scale(x,y,1.0f);
    }

    inline Matrix4f scale(float s)
    {
        return glm::scale(Matrix4f(1.0f),Vector3f(s,s,s));
    }

    inline Matrix4f rotate(float angle,const Vector3f &axis)
    {
        return glm::rotate(Matrix4f(1.0f),angle,axis);
    }

    inline Matrix4f rotate(float angle,float x,float y,float z)
    {
        return glm::rotate(Matrix4f(1.0f),angle,Vector3f(x,y,z));
    }
    
    inline Matrix4f rotate(float angle,float x,float y)
    {
        return rotate(angle,x,y,1.0f);
    }

    inline Matrix4f rotate(float angle,const Vector4f &axis)
    {
        return rotate(angle,Vector3f(axis.x,axis.y,axis.z));
    }

    inline Vector3f rotate(const Vector3f &v3f,float angle,const Vector3f &axis)
    {
		Vector4f result = rotate(angle, axis)*Vector4f(v3f, 1.0f);

		return Vector3f(result.x,result.y,result.z);
    }
}//namespace hgl
#endif//HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE
