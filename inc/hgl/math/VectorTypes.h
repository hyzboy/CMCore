#pragma once

#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

namespace hgl
{
    /**
     * 向量类型定义
     * 基于GLM库的向量类型封装
     */

    enum class AXIS
    {
        X,Y,Z
    };

    // 定义各种类型的向量
    #define DEF_VECTOR(flag,glm_type)   using Vector1##flag=glm::glm_type##1; const Vector1##flag ZeroVector1##flag=Vector1##flag(0);         const Vector1##flag OneVector1##flag=Vector1##flag(1);\
                                        using Vector2##flag=glm::glm_type##2; const Vector2##flag ZeroVector2##flag=Vector2##flag(0,0);       const Vector2##flag OneVector2##flag=Vector2##flag(1,1);\
                                        using Vector3##flag=glm::glm_type##3; const Vector3##flag ZeroVector3##flag=Vector3##flag(0,0,0);     const Vector3##flag OneVector3##flag=Vector3##flag(1,1,1);\
                                        using Vector4##flag=glm::glm_type##4; const Vector4##flag ZeroVector4##flag=Vector4##flag(0,0,0,0);   const Vector4##flag OneVector4##flag=Vector4##flag(1,1,1,1);

    // 浮点向量
    DEF_VECTOR(f,vec)       // Vector2f, Vector3f, Vector4f
    DEF_VECTOR(d,dvec)      // Vector2d, Vector3d, Vector4d

    // 布尔向量
    DEF_VECTOR(b,bvec)

    // 整型向量
    DEF_VECTOR(i,ivec)      // 默认int
    DEF_VECTOR(u,uvec)      // 默认uint

    // 特定位宽整型向量
    DEF_VECTOR(i8,i8vec)
    DEF_VECTOR(i16,i16vec)
    DEF_VECTOR(i32,i32vec)
    DEF_VECTOR(i64,i64vec)

    DEF_VECTOR(u8,u8vec)
    DEF_VECTOR(u16,u16vec)
    DEF_VECTOR(u32,u32vec)
    DEF_VECTOR(u64,u64vec)

    #undef DEF_VECTOR

    // 轴向量常量
    namespace AxisVector
    {
        const Vector3f X = Vector3f(1,0,0);
        const Vector3f Y = Vector3f(0,1,0);
        const Vector3f Z = Vector3f(0,0,1);
    };

    // 根据轴枚举获取向量
    inline const Vector3f GetAxisVector(const AXIS &axis)
    {
        switch(axis)
        {
            case AXIS::X: return AxisVector::X;
            case AXIS::Y: return AxisVector::Y;
            case AXIS::Z: return AxisVector::Z;
        }

        return Vector3f(0,0,0);
    }
}//namespace hgl
