#pragma once

#include<hgl/math/Vector.h>

namespace hgl
{
    /*
    * 根据三角形的三边长计算三角形的面积
    */
    inline float TriangleArea(const float a,const float b,const float c)
    {
        const float s=(a+b+c)*0.5f;

        return sqrt(s*(s-a)*(s-b)*(s-c));
    }

    inline float TriangleArea(const Vector2f &a,const Vector2f &b,const Vector2f &c)
    {
        return fabs((a.x-c.x)*(b.y-c.y)-(a.y-c.y)*(b.x-c.x))*0.5f;
    }

    inline float TriangleArea(const Vector3f &a,const Vector3f &b,const Vector3f &c)
    {
        return glm::length(glm::cross(b-a,c-a))*0.5f;
    }

    inline float RectangleArea(const float width,const float height)
    {
        return width*height;
    }

    inline float RectangleArea(const Vector2f &size)
    {
        return size.x*size.y;
    }
    
    /**
     * 正圆面积计算
     * @param radius 半径
     */
    inline float CircleArea(const float radius)
    {
        return HGL_PI*radius*radius;
    }
    
    /**
     * 椭圆面积计算
     * @param l_radius 长半径
     * @param s_radius 短半径
     */
    inline float EllipseArea(const float horizontal_radius,const float vertical_radius)
    {
        return HGL_PI*horizontal_radius*vertical_radius;
    }

    inline float EllipseArea(const Vector2f &radius)
    {
        return HGL_PI*radius.x*radius.y;
    }

    inline float SectorArea(const float radius,const float angle)
    {
        return HGL_PI*radius*radius*angle/360.0f;
    }

    inline float QuadArea(const Vector3f &a,const Vector3f &b,const Vector3f &c,const Vector3f &d)
    {
        return TriangleArea(a,b,c)+TriangleArea(a,c,d);
    }
}//namespace hgl
