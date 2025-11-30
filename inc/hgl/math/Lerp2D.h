#pragma once

#include<hgl/math/Vector.h>

namespace hgl
{
    namespace graph
    {
        inline float LerpLinear(const float from,const float to,const float t)
        {
            return from+(to-from)*t;
        }

        inline float LerpCos(const float from,const float to,const float t)
        {
            float t2=(1.0f-cos(t*HGL_PI))/2.0f;

            return from*(1.0f-t2)+to*t2;
        }

        inline float LerpCubic(const float from,const float to,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return from*(2.0f*t3-3.0f*t2+1.0f)+to*(3.0f*t2-2.0f*t3);
        }

        inline float LerpCubicEase(const float from,const float to,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return from*(2.0f*t3-3.0f*t2+1.0f)+to*(3.0f*t2-2.0f*t3);
        }

        inline Vector2f LerpLinear(const Vector2f &from,const Vector2f &to,const float t)
        {
            return from+(to-from)*t;
        }

        inline Vector2f LerpCos(const Vector2f &from,const Vector2f &to,const float t)
        {
            float t2=(1.0f-cos(t*HGL_PI))/2.0f;

            return from*(1.0f-t2)+to*t2;
        }

        inline Vector2f LerpCubic(const Vector2f &from,const Vector2f &to,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return from*(2.0f*t3-3.0f*t2+1.0f)+to*(3.0f*t2-2.0f*t3);
        }

        inline Vector2f LerpCubicEase(const Vector2f &from,const Vector2f &to,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return from*(2.0f*t3-3.0f*t2+1.0f)+to*(3.0f*t2-2.0f*t3);
        }

        inline Vector2f LerpBezier(const Vector2f &p0,const Vector2f &p1,const Vector2f &p2,const Vector2f &p3,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            float t_1=1.0f-t;
            float t2_1=t_1*t_1;
            float t3_1=t2_1*t_1;

            return p0*t3_1+3.0f*p1*t*t2_1+3.0f*p2*t2*t_1+p3*t3;
        }

        inline Vector2f LerpCatmullRom(const Vector2f &p0,const Vector2f &p1,const Vector2f &p2,const Vector2f &p3,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            float t_1=1.0f-t;
            float t2_1=t_1*t_1;
            float t3_1=t2_1*t_1;

            return   p0*((-t3+2.0f*t2-t)*0.5f)
                    +p1*((3.0f*t3-5.0f*t2+2.0f)*0.5f)
                    +p2*((-3.0f*t3+4.0f*t2+t)*0.5f)
                    +p3*((t3-t2)*0.5f);
        }

        inline Vector2f LerpBSpline(const Vector2f &p0,const Vector2f &p1,const Vector2f &p2,const Vector2f &p3,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            float t_1=1.0f-t;
            float t2_1=t_1*t_1;
            float t3_1=t2_1*t_1;

            return   p0*((-t3+3.0f*t2-3.0f*t+1.0f)/6.0f)
                    +p1*((3.0f*t3-6.0f*t2+4.0f)/6.0f)
                    +p2*((-3.0f*t3+3.0f*t2+3.0f*t+1.0f)/6.0f)
                    +p3*(t3)/6.0f;
        }
    }//namespace graph
}//namespace hgl
