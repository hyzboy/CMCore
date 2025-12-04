#pragma once

#include<hgl/math/VectorTypes.h>
#include<hgl/math/VectorOperations.h>
#include<hgl/math/FastTriangle.h>
#include<hgl/math/TrigConstants.h>
#include<hgl/type/CompareUtil.h>

namespace hgl
{
    /**
     * 向量工具函数
     * 包括：转换、几何计算、射线相关等
     */

    // ==================== 向量维度转换 ====================

    inline void vec3to2(Vector2f &dst,const Vector3f &src)
    {
        dst.x=src.x;
        dst.y=src.y;
    }

    inline Vector2f vec3to2(const Vector3f &src)
    {
        return Vector2f(src.x,src.y);
    }

    inline void vec2to3(Vector3f &dst,const Vector2f &src,const float z)
    {
        dst.x=src.x;
        dst.y=src.y;
        dst.z=z;
    }

    inline Vector3f vec2to3(const Vector2f &src,const float z)
    {
        return Vector3f(src.x,src.y,z);
    }

    // ==================== 向量极值 ====================

    inline const Vector3f MinVector(const Vector3f &v1,const Vector3f &v2)
    {
        return Vector3f(hgl_min(v1.x,v2.x),
                        hgl_min(v1.y,v2.y),
                        hgl_min(v1.z,v2.z));
    }

    inline const Vector4f MinVector(const Vector4f &v1,const Vector4f &v2)
    {
        return Vector4f(hgl_min(v1.x,v2.x),
                        hgl_min(v1.y,v2.y),
                        hgl_min(v1.z,v2.z),
                        hgl_min(v1.w,v2.w));
    }

    inline const Vector3f MaxVector(const Vector3f &v1,const Vector3f &v2)
    {
        return Vector3f(hgl_max(v1.x,v2.x),
                        hgl_max(v1.y,v2.y),
                        hgl_max(v1.z,v2.z));
    }

    inline const Vector4f MaxVector(const Vector4f &v1,const Vector4f &v2)
    {
        return Vector4f(hgl_max(v1.x,v2.x),
                        hgl_max(v1.y,v2.y),
                        hgl_max(v1.z,v2.z),
                        hgl_max(v1.w,v2.w));
    }

    // ==================== 方向插值 ====================

    inline const Vector3f LerpDirection(const Vector3f &old_direction,const Vector3f &new_direction,const float alpha)
    {
        return glm::normalize(old_direction*(1.0f-alpha)+new_direction*alpha);
    }

    // ==================== 角度计算 ====================

    /**
    * 求两个方向之间的弧度
    */
    template<typename T>
    inline float CalculateRadian(const T &a,const T &b)
    {
        float ma=glm::length(a);
        float mb=glm::length(b);

        if(IsNearlyZero(ma)||IsNearlyZero(mb))
            return 0.0f;

        return glm::clamp(glm::dot(a,b)/(ma*mb),-1.0f,1.0f);
    }

    /**
    * 求两个方向之间的角度
    */
    template<typename T>
    inline float CalculateAngle(const T &a,const T &b)
    {
        return std::acos(CalculateRadian(a,b));
    }

    /**
    * 计算射线与点的夹角(cos)
    */
    inline float ray_intersection_angle_cos(const Vector3f &ray_dir, const Vector3f &ray_pos, const Vector3f &pos)
    {
        return dot(ray_dir, normalized(pos - ray_pos));
    }

    /**
    * 计算射线与点的夹角(弧度)
    */
    inline float ray_intersection_angle_radian(const Vector3f &ray_dir, const Vector3f &ray_pos, const Vector3f &pos)
    {
        double val=dot(ray_dir, normalized(pos - ray_pos));

        if(val>1)return 0;
        if(val<-1)return HGL_PI;
    
        return acos(val);
    }

    /**
     * 计算射线与点的夹角(角度)
     */
    inline float ray_intersection_angle_degree(const Vector3f &ray_dir, const Vector3f &ray_pos, const Vector3f &pos)
    {
        double val=dot(ray_dir, normalized(pos - ray_pos));

        if(val>1)return 0;
        if(val<-1)return 180;
    
        return rad2deg(acos(val));
    }

    // ==================== 2D旋转 ====================

    /**
    * 做一个2D旋转计算
    * @param result 结果
    * @param source 原始点坐标
    * @param center 圆心坐标
    * @param ang 旋转角度
    */
    template<typename T1, typename T2, typename T3>
    inline void rotate2d(T1 &result, const T2 &source, const T3 &center, const double ang)
    {
        double as, ac;

        Lsincos(ang, as, ac);

        result.x = center.x + ((source.x - center.x)*ac - (source.y - center.y)*as);
        result.y = center.y + ((source.x - center.x)*as + (source.y - center.y)*ac);
    }

    // ==================== 极坐标转换 ====================

    inline const Vector3f PolarToVector(float yaw,float pitch)
    {
        return glm::normalize(  Vector3f(   cos(yaw  )*cos(pitch),
                                            sin(yaw  )*cos(pitch),
                                            sin(pitch)));
    }

    // ==================== 向量类型转换 ====================

    inline const Vector4f PointVector(const Vector3f &v)
    {
        return Vector4f(v,1.0f);
    }

    inline const Vector4f DirectionVector(const Vector3f &v)
    {
        return Vector4f(v,0.0f);
    }

    inline const Vector3f xyz(const Vector4f &v)
    {
        return Vector3f(v.x,v.y,v.z);
    }
}//namespace hgl
