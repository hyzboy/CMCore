#pragma once

#include<hgl/math/Vector.h>
#include<hgl/math/LerpType.h>

namespace hgl
{
    namespace graph
    {
        // ==================== Function Pointer Type Definitions ====================

        /// 2-point Vector3f Lerp function pointer type (Linear, Cosine, Cubic, Hermite)
        typedef Vector3f (*LerpFunc2PointVec3f)(const Vector3f&, const Vector3f&, const float);

        /// 4-point Vector3f Lerp function pointer type (Bezier, CatmullRom, BSpline)
        typedef Vector3f (*LerpFunc4PointVec3f)(const Vector3f&, const Vector3f&, const Vector3f&, const Vector3f&, const float);

        // ==================== Interpolation Functions ====================

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

        // ==================== Function Pointer Getters ====================

        /**
         * @brief Get 2-point Lerp function pointer for Vector3f (Linear, Cosine, Cubic, Hermite)
         * @param type Interpolation type
         * @return Function pointer to the corresponding Lerp function, or nullptr if invalid type
         */
        inline LerpFunc2PointVec3f GetLerpFuncPointer(LerpType type)
        {
            switch (type)
            {
                case LerpType::Linear:      return LerpLinear;
                case LerpType::Cos:         return LerpCos;
                case LerpType::Cubic:       return LerpCubic;
                case LerpType::Hermite:     return LerpCubicEase;  // Hermite is alias for Cubic
                default:                    return nullptr;
            }
        }

        /**
         * @brief Get 4-point Lerp function pointer for Vector3f (Bezier, CatmullRom, BSpline)
         * @param type Interpolation type
         * @return Function pointer to the corresponding Lerp function, or nullptr if invalid type
         */
        inline LerpFunc4PointVec3f GetLerpBezierFuncPointer(LerpType type)
        {
            switch (type)
            {
                case LerpType::Bezier:      return LerpBezier;
                case LerpType::CatmullRom:  return LerpCatmullRom;
                case LerpType::BSpline:     return LerpBSpline;
                default:                    return nullptr;
            }
        }

        // ==================== Semantic Lerp Wrappers ====================

        /**
         * @brief Linear interpolation for 3D positions
         * Interpolates between two position points in 3D space
         * @param from Starting position
         * @param to Ending position
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated position
         */
        inline Vector3f LerpPosition(const Vector3f &from, const Vector3f &to, const float t)
        {
            return LerpLinear(from, to, t);
        }

        /**
         * @brief Cosine interpolation for 3D positions with smooth easing
         * @param from Starting position
         * @param to Ending position
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated position
         */
        inline Vector3f LerpPositionSmooth(const Vector3f &from, const Vector3f &to, const float t)
        {
            return LerpCos(from, to, t);
        }

        /**
         * @brief Cubic interpolation for 3D positions with smooth easing
         * @param from Starting position
         * @param to Ending position
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated position
         */
        inline Vector3f LerpPositionCubic(const Vector3f &from, const Vector3f &to, const float t)
        {
            return LerpCubic(from, to, t);
        }

        /**
         * @brief Linear interpolation for 3D directions with normalization
         * Normalizes the result to maintain unit length for direction vectors
         * @param from Starting direction (should be normalized)
         * @param to Ending direction (should be normalized)
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated normalized direction
         */
        inline Vector3f LerpDirection(const Vector3f &from, const Vector3f &to, const float t)
        {
            Vector3f result = LerpLinear(from, to, t);
            return result.Normalize();  // Ensure unit length
        }

        /**
         * @brief Cosine interpolation for 3D directions with normalization
         * Provides smoother direction transitions with automatic normalization
         * @param from Starting direction (should be normalized)
         * @param to Ending direction (should be normalized)
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated normalized direction
         */
        inline Vector3f LerpDirectionSmooth(const Vector3f &from, const Vector3f &to, const float t)
        {
            Vector3f result = LerpCos(from, to, t);
            return result.Normalize();  // Ensure unit length
        }

        /**
         * @brief Cubic interpolation for 3D directions with normalization
         * Provides smooth cubic easing for direction interpolation with normalization
         * @param from Starting direction (should be normalized)
         * @param to Ending direction (should be normalized)
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated normalized direction
         */
        inline Vector3f LerpDirectionCubic(const Vector3f &from, const Vector3f &to, const float t)
        {
            Vector3f result = LerpCubic(from, to, t);
            return result.Normalize();  // Ensure unit length
        }

        /**
         * @brief Bezier interpolation for 3D directions with normalization
         * Smooth Bezier curve interpolation with automatic normalization
         * @param p0 Starting direction point
         * @param p1 First control point
         * @param p2 Second control point
         * @param p3 Ending direction point
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated normalized direction
         */
        inline Vector3f LerpDirectionBezier(const Vector3f &p0, const Vector3f &p1, const Vector3f &p2, const Vector3f &p3, const float t)
        {
            Vector3f result = LerpBezier(p0, p1, p2, p3, t);
            return result.Normalize();  // Ensure unit length
        }

        /**
         * @brief Linear interpolation for 3D RGB colors
         * Interpolates color values directly in RGB space
         * @param from Starting color
         * @param to Ending color
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated color
         */
        inline Vector3f LerpColor(const Vector3f &from, const Vector3f &to, const float t)
        {
            return LerpLinear(from, to, t);
        }

        /**
         * @brief Cosine interpolation for 3D RGB colors with smooth easing
         * Provides smoother color transitions
         * @param from Starting color
         * @param to Ending color
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated color
         */
        inline Vector3f LerpColorSmooth(const Vector3f &from, const Vector3f &to, const float t)
        {
            return LerpCos(from, to, t);
        }

        /**
         * @brief Cubic interpolation for 3D RGB colors with smooth easing
         * @param from Starting color
         * @param to Ending color
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated color
         */
        inline Vector3f LerpColorCubic(const Vector3f &from, const Vector3f &to, const float t)
        {
            return LerpCubic(from, to, t);
        }

        /**
         * @brief Bezier interpolation for 3D RGB colors
         * Smooth color transition along a Bezier curve
         * @param c0 Starting color
         * @param c1 First control color
         * @param c2 Second control color
         * @param c3 Ending color
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated color
         */
        inline Vector3f LerpColorBezier(const Vector3f &c0, const Vector3f &c1, const Vector3f &c2, const Vector3f &c3, const float t)
        {
            return LerpBezier(c0, c1, c2, c3, t);
        }

    }//namespace graph
}//namespace hgl
