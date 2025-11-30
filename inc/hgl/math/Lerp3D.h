#pragma once

#include<hgl/math/Vector.h>

namespace hgl
{
    namespace graph
    {
        inline Vector3f LerpLinear(const Vector3f &from,const Vector3f &to,const float t)
        {
            return from+(to-from)*t;
        }

        inline Vector3f LerpCos(const Vector3f &from,const Vector3f &to,const float t)
        {
            float t2=(1.0f-cos(t*HGL_PI))/2.0f;

            return from*(1.0f-t2)+to*t2;
        }

        inline Vector3f LerpCubic(const Vector3f &from,const Vector3f &to,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return from*(2.0f*t3-3.0f*t2+1.0f)+to*(3.0f*t2-2.0f*t3);
        }

        inline Vector3f LerpCubicEase(const Vector3f &from,const Vector3f &to,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return from*(2.0f*t3-3.0f*t2+1.0f)+to*(3.0f*t2-2.0f*t3);
        }

        inline Vector3f LerpBezier(const Vector3f &from,const Vector3f &cp1,const Vector3f &cp2,const Vector3f &to,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            float it=1.0f-t;
            float it2=it*it;
            float it3=it2*it;

            return from*it3+cp1*3.0f*it2*t+cp2*3.0f*it*t2+to*t3;
        }

        inline Vector3f LerpCatmullRom(const Vector3f &p0,const Vector3f &p1,const Vector3f &p2,const Vector3f &p3,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return   p0*((-t3+2.0f*t2-t)*0.5f)
                    +p1*((3.0f*t3-5.0f*t2+2.0f)*0.5f)
                    +p2*((-3.0f*t3+4.0f*t2+t)*0.5f)
                    +p3*((t3-t2)*0.5f);
        }

        inline Vector3f LerpBSpline(const Vector3f &p0,const Vector3f &p1,const Vector3f &p2,const Vector3f &p3,const float t)
        {
            float t2=t*t;
            float t3=t2*t;

            return   p0*((-t3+3.0f*t2-3.0f*t+1.0f)/6.0f)
                    +p1*((3.0f*t3-6.0f*t2+4.0f)/6.0f)
                    +p2*((-3.0f*t3+3.0f*t2+3.0f*t+1.0f)/6.0f)
                    +p3*(t3)/6.0f;
        }
    }//namespace graph
}//namespace hgl
