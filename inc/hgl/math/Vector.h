#ifndef HGL_ALGORITHM_MATH_VECTOR_INCLUDE
#define HGL_ALGORITHM_MATH_VECTOR_INCLUDE

#ifdef _MSC_VER
#pragma warning(disable:4244)           // double -> int 精度丢失警告
#endif//_MSC_VER

#include<hgl/math/FastTriangle.h>
#include<glm/glm.hpp>

namespace hgl
{
    using Vector2f=glm::vec2;
    using Vector3f=glm::vec3;
    using Vector4f=glm::vec4;

    using Vector2i=glm::ivec2;
    using Vector3i=glm::ivec3;
    using Vector4i=glm::ivec4;

    using Vector2u=glm::uvec2;
    using Vector3u=glm::uvec3;
    using Vector4u=glm::uvec4;

    inline bool operator == (const Vector2f &lhs,const Vector2f &rhs)
    {
        if(lhs.x!=rhs.x)return(false);
        if(lhs.y!=rhs.y)return(false);
        return(true);
    }

    inline bool operator != (const Vector2f &lhs,const Vector2f &rhs)
    {
        if(lhs.x!=rhs.x)return(true);
        if(lhs.y!=rhs.y)return(true);
        return(false);
    }

    inline bool operator == (const Vector3f &lhs,const Vector3f &rhs)
    {
        if(lhs.x!=rhs.x)return(false);
        if(lhs.y!=rhs.y)return(false);
        if(lhs.z!=rhs.z)return(false);
        return(true);
    }

    inline bool operator != (const Vector3f &lhs,const Vector3f &rhs)
    {
        if(lhs.x!=rhs.x)return(true);
        if(lhs.y!=rhs.y)return(true);
        if(lhs.z!=rhs.z)return(true);
        return(false);
    }

    inline bool operator == (const Vector4f &lhs,const Vector4f &rhs)
    {
        if(lhs.x!=rhs.x)return(false);
        if(lhs.y!=rhs.y)return(false);
        if(lhs.z!=rhs.z)return(false);
        if(lhs.w!=rhs.w)return(false);
        return(true);
    }

    inline bool operator != (const Vector4f &lhs,const Vector4f &rhs)
    {
        if(lhs.x!=rhs.x)return(true);
        if(lhs.y!=rhs.y)return(true);
        if(lhs.z!=rhs.z)return(true);
        if(lhs.w!=rhs.w)return(true);
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
    inline T clamp(const T &v,const T &min_v,const T &max_v)
    {
        if(v<min_v)return min_v;
        if(v>max_v)return max_v;
        return v;
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

    inline Vector2f to(const Vector2f &start, const Vector2f &end, float pos)
    {
        return Vector2f(start.x + (end.x - start.x)*pos,
                        start.y + (end.y - start.y)*pos);
    }

    inline Vector3f to(const Vector3f &start, const Vector3f &end, float pos)
    {
        return Vector3f(start.x + (end.x - start.x)*pos,
                        start.y + (end.y - start.y)*pos,
                        start.z + (end.z - start.z)*pos);
    }

    template<typename T>
    inline void to_2d(T &result, const T &start, const T &end, float pos)
    {
        result.x = start.x + (end.x - start.x)*pos;
        result.y = start.y + (end.y - start.y)*pos;
    }

    inline bool IsNearlyEqual(const float v1,const float v2,const float gap)
    {
        return fabsf(v1-v2)<=gap;
    }

    inline bool IsNearlyEqual(const Vector2f &v1,const Vector2f &v2,const float gap)
    {
        return length_squared_2d(v1,v2)<=(gap*gap);
    }

    inline bool IsNearlyEqual(const Vector3f &v1,const Vector3f &v2,const float gap)
    {
        return length_squared(v1,v2)<=(gap*gap);
    }

    inline float ray_angle_cos(const Vector3f &ray_dir, const Vector3f &ray_pos, const Vector3f &pos)
    {
        return dot(ray_dir, normalized(pos - ray_pos));
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
}//namespace hgl
#endif//HGL_ALGORITHM_MATH_VECTOR_INCLUDE
