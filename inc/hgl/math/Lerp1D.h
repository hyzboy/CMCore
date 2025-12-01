#pragma once

#include<hgl/math/Vector.h>
#include<hgl/math/LerpType.h>

namespace hgl
{
    namespace graph
    {
        // ==================== Function Pointer Type Definitions ====================

        /// 2-point float Lerp function pointer type (Linear, Cosine, Cubic, Hermite)
        typedef float (*LerpFunc2PointFloat)(const float, const float, const float);

        /// Quadratic Bezier float function pointer type
        typedef float (*LerpFuncQuadBezierFloat)(const float, const float, const float, const float);

        /// 4-point float Lerp function pointer type (Bezier, CatmullRom, BSpline)
        typedef float (*LerpFunc4PointFloat)(const float, const float, const float, const float, const float);

        // ==================== Interpolation Functions ====================

        /**
         * @brief Linear interpolation template
         * @tparam T The data type to interpolate (must support +, -, * operators)
         * @param from Starting value
         * @param to Ending value
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpLinear(const T &from, const T &to, const float t)
        {
            return from + (to - from) * t;
        }

        /**
         * @brief Cosine interpolation template
         * Uses smooth cosine curve for easing
         * @tparam T The data type to interpolate
         * @param from Starting value
         * @param to Ending value
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpCos(const T &from, const T &to, const float t)
        {
            float t2 = (1.0f - cos(t * HGL_PI)) / 2.0f;
            return from * (1.0f - t2) + to * t2;
        }

        /**
         * @brief Cubic Hermite interpolation template
         * Smooth cubic easing curve
         * @tparam T The data type to interpolate
         * @param from Starting value
         * @param to Ending value
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpCubic(const T &from, const T &to, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return from * (2.0f * t3 - 3.0f * t2 + 1.0f) + to * (3.0f * t2 - 2.0f * t3);
        }

        /**
         * @brief Cubic ease interpolation template (alias for LerpCubic)
         * @tparam T The data type to interpolate
         * @param from Starting value
         * @param to Ending value
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpCubicEase(const T &from, const T &to, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return from * (2.0f * t3 - 3.0f * t2 + 1.0f) + to * (3.0f * t2 - 2.0f * t3);
        }

        /**
         * @brief Quadratic Bezier interpolation template
         * Interpolates between two points with one control point
         * @tparam T The data type to interpolate
         * @param p0 Starting point
         * @param p1 Control point
         * @param p2 Ending point
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpQuadraticBezier(const T &p0, const T &p1, const T &p2, const float t)
        {
            float t2 = t * t;
            float it = 1.0f - t;
            float it2 = it * it;

            return p0 * it2 + p1 * 2.0f * it * t + p2 * t2;
        }

        /**
         * @brief Cubic Bezier interpolation template
         * Interpolates between two points with two control points
         * @tparam T The data type to interpolate
         * @param p0 Starting point
         * @param p1 First control point
         * @param p2 Second control point
         * @param p3 Ending point
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpBezier(const T &p0, const T &p1, const T &p2, const T &p3, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            float it = 1.0f - t;
            float it2 = it * it;
            float it3 = it2 * it;

            return p0 * it3 + p1 * 3.0f * it2 * t + p2 * 3.0f * it * t2 + p3 * t3;
        }

        /**
         * @brief Catmull-Rom spline interpolation template
         * Smooth curve passing through all control points
         * @tparam T The data type to interpolate
         * @param p0 First control point (before start)
         * @param p1 Starting point
         * @param p2 Ending point
         * @param p3 Last control point (after end)
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpCatmullRom(const T &p0, const T &p1, const T &p2, const T &p3, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;

            return p0 * ((-t3 + 2.0f * t2 - t) * 0.5f)
                 + p1 * ((3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f)
                 + p2 * ((-3.0f * t3 + 4.0f * t2 + t) * 0.5f)
                 + p3 * ((t3 - t2) * 0.5f);
        }

        /**
         * @brief B-Spline interpolation template
         * Smooth uniform B-spline curve
         * @tparam T The data type to interpolate
         * @param p0 First control point
         * @param p1 Second control point
         * @param p2 Third control point
         * @param p3 Fourth control point
         * @param t Interpolation parameter (0.0 to 1.0)
         * @return Interpolated value
         */
        template<typename T>
        inline T LerpBSpline(const T &p0, const T &p1, const T &p2, const T &p3, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;

            return p0 * ((-t3 + 3.0f * t2 - 3.0f * t + 1.0f) / 6.0f)
                 + p1 * ((3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f)
                 + p2 * ((-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f)
                 + p3 * (t3 / 6.0f);
        }

        // ==================== Specializations for common types ====================

        /**
         * @brief Linear interpolation for float
         */
        inline float LerpLinear(const float from, const float to, const float t)
        {
            return from + (to - from) * t;
        }

        /**
         * @brief Cosine interpolation for float
         */
        inline float LerpCos(const float from, const float to, const float t)
        {
            float t2 = (1.0f - cos(t * HGL_PI)) / 2.0f;
            return from * (1.0f - t2) + to * t2;
        }

        /**
         * @brief Cubic Hermite interpolation for float
         */
        inline float LerpCubic(const float from, const float to, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return from * (2.0f * t3 - 3.0f * t2 + 1.0f) + to * (3.0f * t2 - 2.0f * t3);
        }

        /**
         * @brief Cubic ease interpolation for float
         */
        inline float LerpCubicEase(const float from, const float to, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return from * (2.0f * t3 - 3.0f * t2 + 1.0f) + to * (3.0f * t2 - 2.0f * t3);
        }

        /**
         * @brief Quadratic Bezier interpolation for float
         */
        inline float LerpQuadraticBezier(const float p0, const float p1, const float p2, const float t)
        {
            float t2 = t * t;
            float it = 1.0f - t;
            float it2 = it * it;

            return p0 * it2 + p1 * 2.0f * it * t + p2 * t2;
        }

        /**
         * @brief Cubic Bezier interpolation for float
         */
        inline float LerpBezier(const float p0, const float p1, const float p2, const float p3, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;
            float it = 1.0f - t;
            float it2 = it * it;
            float it3 = it2 * it;

            return p0 * it3 + p1 * 3.0f * it2 * t + p2 * 3.0f * it * t2 + p3 * t3;
        }

        /**
         * @brief Catmull-Rom spline interpolation for float
         */
        inline float LerpCatmullRom(const float p0, const float p1, const float p2, const float p3, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;

            return p0 * ((-t3 + 2.0f * t2 - t) * 0.5f)
                 + p1 * ((3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f)
                 + p2 * ((-3.0f * t3 + 4.0f * t2 + t) * 0.5f)
                 + p3 * ((t3 - t2) * 0.5f);
        }

        /**
         * @brief B-Spline interpolation for float
         */
        inline float LerpBSpline(const float p0, const float p1, const float p2, const float p3, const float t)
        {
            float t2 = t * t;
            float t3 = t2 * t;

            return p0 * ((-t3 + 3.0f * t2 - 3.0f * t + 1.0f) / 6.0f)
                 + p1 * ((3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f)
                 + p2 * ((-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f)
                 + p3 * (t3 / 6.0f);
        }

        // ==================== Function Pointer Getters ====================

        /**
         * @brief Get 2-point Lerp function pointer (Linear, Cosine, Cubic, Hermite)
         * @param type Interpolation type
         * @return Function pointer to the corresponding Lerp function, or nullptr if invalid type
         */
        inline LerpFunc2PointFloat GetLerpFuncPointer(LerpType type)
        {
            switch (type)
            {
                case LerpType::Linear:      return LerpLinear;
                case LerpType::Cos:         return LerpCos;
                case LerpType::Cubic:       return LerpCubic;
                case LerpType::Hermite:     return LerpCubic;  // Hermite is alias for Cubic
                default:                    return nullptr;
            }
        }

        /**
         * @brief Get Quadratic Bezier function pointer
         * @param type Interpolation type
         * @return Function pointer to LerpQuadraticBezier if type matches, nullptr otherwise
         */
        inline LerpFuncQuadBezierFloat GetLerpQuadraticBezierFuncPointer(LerpType type)
        {
            if (type == LerpType::QuadraticBezier)
                return LerpQuadraticBezier;
            return nullptr;
        }

        /**
         * @brief Get 4-point Lerp function pointer (Bezier, CatmullRom, BSpline)
         * @param type Interpolation type
         * @return Function pointer to the corresponding Lerp function, or nullptr if invalid type
         */
        inline LerpFunc4PointFloat GetLerpBezierFuncPointer(LerpType type)
        {
            switch (type)
            {
                case LerpType::Bezier:      return LerpBezier;
                case LerpType::CatmullRom:  return LerpCatmullRom;
                case LerpType::BSpline:     return LerpBSpline;
                default:                    return nullptr;
            }
        }

    }//namespace graph
}//namespace hgl
