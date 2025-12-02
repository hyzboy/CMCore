#pragma once

#ifdef _MSC_VER
#pragma warning(disable:4244)           // double -> int 精度丢失警告
#endif//_MSC_VER

#include<hgl/math/FastTriangle.h>
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>
#include<glm/gtc/epsilon.hpp>

namespace hgl
{
    enum class AXIS
    {
        X,Y,Z
    };

#define DEF_VECTOR(flag,glm_type)   using Vector1##flag=glm::glm_type##1; const Vector1##flag ZeroVector1##flag=Vector1##flag(0);         const Vector1##flag OneVector1##flag=Vector1##flag(1);\
                                    using Vector2##flag=glm::glm_type##2; const Vector2##flag ZeroVector2##flag=Vector2##flag(0,0);       const Vector2##flag OneVector2##flag=Vector2##flag(1,1);\
                                    using Vector3##flag=glm::glm_type##3; const Vector3##flag ZeroVector3##flag=Vector3##flag(0,0,0);     const Vector3##flag OneVector3##flag=Vector3##flag(1,1,1);\
                                    using Vector4##flag=glm::glm_type##4; const Vector4##flag ZeroVector4##flag=Vector4##flag(0,0,0,0);   const Vector4##flag OneVector4##flag=Vector4##flag(1,1,1,1);

    DEF_VECTOR(f,vec)
    DEF_VECTOR(d,dvec)
    DEF_VECTOR(b,bvec)

    DEF_VECTOR(i,ivec)
    DEF_VECTOR(u,uvec)

    DEF_VECTOR(i8,i8vec)
    DEF_VECTOR(i16,i16vec)
    DEF_VECTOR(i32,i32vec)
    DEF_VECTOR(i64,i64vec)

    DEF_VECTOR(u8,u8vec)
    DEF_VECTOR(u16,u16vec)
    DEF_VECTOR(u32,u32vec)
    DEF_VECTOR(u64,u64vec)

#undef DEF_VECTOR

    namespace AxisVector
    {
        const Vector3f X=Vector3f(1,0,0);
        const Vector3f Y=Vector3f(0,1,0);
        const Vector3f Z=Vector3f(0,0,1);
    };

    inline const Vector3f GetAxisVector(const AXIS &axis)
    {
        switch(axis)
        {
            case AXIS::X:return AxisVector::X;
            case AXIS::Y:return AxisVector::Y;
            case AXIS::Z:return AxisVector::Z;
        }

        return Vector3f(0,0,0);
    }

    inline bool operator == (const Vector2f &lhs,const Vector2f &rhs)
    {
        if(!IsNearlyEqual(lhs.x,rhs.x))return(false);
        if(!IsNearlyEqual(lhs.y,rhs.y))return(false);
        return(true);
    }

    inline bool operator != (const Vector2f &lhs,const Vector2f &rhs)
    {
        if(!IsNearlyEqual(lhs.x,rhs.x))return(true);
        if(!IsNearlyEqual(lhs.y,rhs.y))return(true);
        return(false);
    }

    inline bool operator == (const Vector3f &lhs,const Vector3f &rhs)
    {
        if(!IsNearlyEqual(lhs.x,rhs.x))return(false);
        if(!IsNearlyEqual(lhs.y,rhs.y))return(false);
        if(!IsNearlyEqual(lhs.z,rhs.z))return(false);
        return(true);
    }

    inline bool operator != (const Vector3f &lhs,const Vector3f &rhs)
    {
        if(!IsNearlyEqual(lhs.x,rhs.x))return(true);
        if(!IsNearlyEqual(lhs.y,rhs.y))return(true);
        if(!IsNearlyEqual(lhs.z,rhs.z))return(true);
        return(false);
    }

    inline bool operator == (const Vector4f &lhs,const Vector4f &rhs)
    {
        if(!IsNearlyEqual(lhs.x,rhs.x))return(false);
        if(!IsNearlyEqual(lhs.y,rhs.y))return(false);
        if(!IsNearlyEqual(lhs.z,rhs.z))return(false);
        if(!IsNearlyEqual(lhs.w,rhs.w))return(false);
        return(true);
    }

    inline bool operator != (const Vector4f &lhs,const Vector4f &rhs)
    {
        if(!IsNearlyEqual(lhs.x,rhs.x))return(true);
        if(!IsNearlyEqual(lhs.y,rhs.y))return(true);
        if(!IsNearlyEqual(lhs.z,rhs.z))return(true);
        if(!IsNearlyEqual(lhs.w,rhs.w))return(true);
        return(false);
    }

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

    template<typename T>
    inline T clamp(const T v,const T min_v,const T max_v)
    {
        if(v<min_v)return min_v;
        if(v>max_v)return max_v;
        return v;
    }

    template<typename T>
    inline const T clamp(const T in)
    {
        return clamp<double>(in,0.0f,1.0f);
    }

    template<> inline const uint8 clamp(const uint8 in)
    {
        return clamp<uint8>(in,0,0xFF);
    }

    template<> inline const uint16 clamp(const uint16 in)
    {
        return clamp<uint16>(in,0,0xFFFF);
    }

    template<typename T>
    inline T normalized(const T &v)
    {
        return glm::normalize(v);
    }

    template<typename T>
    inline void normalize(T &v)
    {
        v=glm::normalize(v);
    }

    inline Vector3f cross(const Vector3f &v1,const Vector3f &v2)
    {
        return glm::cross(v1,v2);
    }

    inline Vector4f cross(const Vector4f &v1,const Vector4f &v2)
    {
        Vector3f v31=v1;
        Vector3f v32=v2;

        Vector4f result=Vector4f(glm::cross(v31,v32),1.0f);

        return result;
    }

    template<typename T>
    inline float dot(const T &v1,const T &v2)
    {
        return glm::dot(v1,v2);
    }

    //template<typename T>
    //inline float dot2(const T &v)
    //{
    //    return v.Dot(v);
    //}

    //inline float ray_angle_cos(const Ray &ray,const vec &pos)
    //{
    //    return ray.dir.Dot((pos-ray.pos).Normalized());
    //}

    inline float length_squared(const Vector2f &v)
    {
        return (v.x*v.x) + (v.y*v.y);
    }

    inline float length_squared_2d(const Vector3f &v)
    {
        return (v.x*v.x) + (v.y*v.y);
    }

    inline float length_squared(const Vector3f &v)
    {
        return (v.x*v.x) + (v.y*v.y) + (v.z*v.z);
    }

    inline float length_squared(const Vector4f &v)
    {
        return (v.x*v.x) + (v.y*v.y) + (v.z*v.z);
    }

    template<typename T>
    inline float length(const T &v)
    {
        return sqrt(length_squared(v));
    }

    inline float length_2d(const Vector3f &v)
    {
        return sqrt(length_squared_2d(v));
    }

    template<typename T1, typename T2>
    inline float length(const T1 &v1, const T2 &v2)
    {
        return sqrt(length_squared(v1-v2));
    }

    template<typename T1, typename T2>
    inline float length_squared(const T1 &v1, const T2 &v2)
    {
        return length_squared(v1-v2);
    }

    template<typename T1, typename T2>
    inline float length_squared_2d(const T1 &v1, const T2 &v2)
    {
        const double x = double(v1.x) - double(v2.x);
        const double y = double(v1.y) - double(v2.y);

        return x*x + y*y;
    }

    template<typename T1, typename T2>
    inline float length_2d(const T1 &v1, const T2 &v2)
    {
        return sqrt(length_squared_2d(v1, v2));
    }

    template<typename T>
    inline T lerp(const T &from,const T &to,const float alpha)
    {
        return from + (to - from) * alpha;
    }

    inline bool IsNearlyEqual(const Vector2f &v1,const Vector2f &v2,const float err=HGL_FLOAT_ERROR)
    {
        if(!IsNearlyEqual(v1.x,v2.x,err))return(false);
        if(!IsNearlyEqual(v1.y,v2.y,err))return(false);

        return(true);
    }

    inline bool IsNearlyEqual(const Vector2d &v1,const Vector2d &v2,const double err=HGL_DOUBLE_ERROR)
    {
        if(!IsNearlyEqual(v1.x,v2.x,err))return(false);
        if(!IsNearlyEqual(v1.y,v2.y,err))return(false);

        return(true);
    }

    inline bool IsNearlyEqual(const Vector3f &v1,const Vector3f &v2,const float err=HGL_FLOAT_ERROR)
    {
        if(!IsNearlyEqual(v1.x,v2.x,err))return(false);
        if(!IsNearlyEqual(v1.y,v2.y,err))return(false);
        if(!IsNearlyEqual(v1.z,v2.z,err))return(false);

        return(true);
    }

    inline bool IsNearlyEqual(const Vector3d &v1,const Vector3d &v2,const double err=HGL_DOUBLE_ERROR)
    {
        if(!IsNearlyEqual(v1.x,v2.x,err))return(false);
        if(!IsNearlyEqual(v1.y,v2.y,err))return(false);
        if(!IsNearlyEqual(v1.z,v2.z,err))return(false);

        return(true);
    }

    template<typename T,glm::qualifier Q>
    inline const bool IsNearlyZero(const glm::vec<2,T,Q> &v)
    {
        return IsNearlyZero(v.x)
             &&IsNearlyZero(v.y);
    }

    template<typename T,glm::qualifier Q>
    inline const bool IsNearlyZero(const glm::vec<3,T,Q> &v)
    {
        return IsNearlyZero(v.x)
             &&IsNearlyZero(v.y)
             &&IsNearlyZero(v.z);
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
        //        double nx,ny;

        //      as=sin(ang*(HGL_PI/180.0f));
        //      ac=cos(ang*(HGL_PI/180.0f));
        //sincos(ang*(HGL_PI/180.0f),&as,&ac);      //在80x87指令上，sin/cos是一个指令同时得出sin和cos，所以可以这样做
        Lsincos(ang, as, ac);                         //低精度sin/cos计算

        result.x = center.x + ((source.x - center.x)*ac - (source.y - center.y)*as);
        result.y = center.y + ((source.x - center.x)*as + (source.y - center.y)*ac);
    }

    inline const Vector3f PolarToVector(float yaw,float pitch)
    {
        return glm::normalize(  Vector3f(   cos(yaw  )*cos(pitch),
                                            sin(yaw  )*cos(pitch),
                                            sin(pitch)));
    }

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

    inline const Vector3f LerpDirection(const Vector3f &old_direction,const Vector3f &new_direction,const float alpha)
    {
        return glm::normalize(old_direction*(1.0f-alpha)+new_direction*alpha);
    }

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
}//namespace hgl
