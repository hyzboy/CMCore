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

        // ==================== Easing Functions ====================

        // --- EaseIn Functions ---

        /**
         * @brief Quadratic ease-in interpolation (t^2)
         */
        inline float LerpEaseInQuad(const float from, const float to, const float t)
        {
            float eased = t * t;
            return from + (to - from) * eased;
        }

        /**
         * @brief Cubic ease-in interpolation (t^3)
         */
        inline float LerpEaseInCubic(const float from, const float to, const float t)
        {
            float eased = t * t * t;
            return from + (to - from) * eased;
        }

        /**
         * @brief Quartic ease-in interpolation (t^4)
         */
        inline float LerpEaseInQuart(const float from, const float to, const float t)
        {
            float eased = t * t * t * t;
            return from + (to - from) * eased;
        }

        /**
         * @brief Quintic ease-in interpolation (t^5)
         */
        inline float LerpEaseInQuint(const float from, const float to, const float t)
        {
            float eased = t * t * t * t * t;
            return from + (to - from) * eased;
        }

        /**
         * @brief Exponential ease-in interpolation
         */
        inline float LerpEaseInExpo(const float from, const float to, const float t)
        {
            float eased = (t == 0.0f) ? 0.0f : pow(2.0f, 10.0f * t - 10.0f);
            return from + (to - from) * eased;
        }

        /**
         * @brief Circular ease-in interpolation
         */
        inline float LerpEaseInCirc(const float from, const float to, const float t)
        {
            float eased = 1.0f - sqrt(1.0f - t * t);
            return from + (to - from) * eased;
        }

        // --- EaseOut Functions ---

        /**
         * @brief Quadratic ease-out interpolation
         */
        inline float LerpEaseOutQuad(const float from, const float to, const float t)
        {
            float eased = 1.0f - (1.0f - t) * (1.0f - t);
            return from + (to - from) * eased;
        }

        /**
         * @brief Cubic ease-out interpolation
         */
        inline float LerpEaseOutCubic(const float from, const float to, const float t)
        {
            float eased = 1.0f - pow(1.0f - t, 3.0f);
            return from + (to - from) * eased;
        }

        /**
         * @brief Quartic ease-out interpolation
         */
        inline float LerpEaseOutQuart(const float from, const float to, const float t)
        {
            float eased = 1.0f - pow(1.0f - t, 4.0f);
            return from + (to - from) * eased;
        }

        /**
         * @brief Quintic ease-out interpolation
         */
        inline float LerpEaseOutQuint(const float from, const float to, const float t)
        {
            float eased = 1.0f - pow(1.0f - t, 5.0f);
            return from + (to - from) * eased;
        }

        /**
         * @brief Exponential ease-out interpolation
         */
        inline float LerpEaseOutExpo(const float from, const float to, const float t)
        {
            float eased = (t == 1.0f) ? 1.0f : 1.0f - pow(2.0f, -10.0f * t);
            return from + (to - from) * eased;
        }

        /**
         * @brief Circular ease-out interpolation
         */
        inline float LerpEaseOutCirc(const float from, const float to, const float t)
        {
            float eased = sqrt(1.0f - (t - 1.0f) * (t - 1.0f));
            return from + (to - from) * eased;
        }

        // --- EaseInOut Functions ---

        /**
         * @brief Quadratic ease-in-out interpolation
         */
        inline float LerpEaseInOutQuad(const float from, const float to, const float t)
        {
            float eased = (t < 0.5f) ? 2.0f * t * t : 1.0f - pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Cubic ease-in-out interpolation
         */
        inline float LerpEaseInOutCubic(const float from, const float to, const float t)
        {
            float eased = (t < 0.5f) ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Quartic ease-in-out interpolation
         */
        inline float LerpEaseInOutQuart(const float from, const float to, const float t)
        {
            float eased = (t < 0.5f) ? 8.0f * t * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Quintic ease-in-out interpolation
         */
        inline float LerpEaseInOutQuint(const float from, const float to, const float t)
        {
            float eased = (t < 0.5f) ? 16.0f * t * t * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Exponential ease-in-out interpolation
         */
        inline float LerpEaseInOutExpo(const float from, const float to, const float t)
        {
            float eased = (t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f : 
                         (t < 0.5f) ? pow(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Circular ease-in-out interpolation
         */
        inline float LerpEaseInOutCirc(const float from, const float to, const float t)
        {
            float eased = (t < 0.5f) ? (1.0f - sqrt(1.0f - pow(2.0f * t, 2.0f))) / 2.0f : 
                         (sqrt(1.0f - pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
            return from + (to - from) * eased;
        }

        // --- Special Functions ---

        /**
         * @brief Smooth step interpolation (3t^2 - 2t^3)
         */
        inline float LerpSmoothStep(const float from, const float to, const float t)
        {
            float eased = t * t * (3.0f - 2.0f * t);
            return from + (to - from) * eased;
        }

        /**
         * @brief Smoother step interpolation (6t^5 - 15t^4 + 10t^3)
         */
        inline float LerpSmootherStep(const float from, const float to, const float t)
        {
            float eased = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
            return from + (to - from) * eased;
        }

        /**
         * @brief Elastic ease-in interpolation
         */
        inline float LerpEaseInElastic(const float from, const float to, const float t)
        {
            float eased = (t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f :
                         -pow(2.0f, 10.0f * t - 10.0f) * sin((t * 10.0f - 10.75f) * (2.0f * HGL_PI) / 3.0f);
            return from + (to - from) * eased;
        }

        /**
         * @brief Elastic ease-out interpolation
         */
        inline float LerpEaseOutElastic(const float from, const float to, const float t)
        {
            float eased = (t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f :
                         pow(2.0f, -10.0f * t) * sin((t * 10.0f - 0.75f) * (2.0f * HGL_PI) / 3.0f) + 1.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Elastic ease-in-out interpolation
         */
        inline float LerpEaseInOutElastic(const float from, const float to, const float t)
        {
            float eased = (t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f :
                         (t < 0.5f) ? -(pow(2.0f, 20.0f * t - 10.0f) * sin((20.0f * t - 11.125f) * (2.0f * HGL_PI) / 4.5f)) / 2.0f :
                         (pow(2.0f, -20.0f * t + 10.0f) * sin((20.0f * t - 11.125f) * (2.0f * HGL_PI) / 4.5f)) / 2.0f + 1.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Back ease-in interpolation (overshoot at start)
         */
        inline float LerpEaseInBack(const float from, const float to, const float t)
        {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            float eased = c3 * t * t * t - c1 * t * t;
            return from + (to - from) * eased;
        }

        /**
         * @brief Back ease-out interpolation (overshoot at end)
         */
        inline float LerpEaseOutBack(const float from, const float to, const float t)
        {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            float eased = 1.0f + c3 * pow(t - 1.0f, 3.0f) + c1 * pow(t - 1.0f, 2.0f);
            return from + (to - from) * eased;
        }

        /**
         * @brief Back ease-in-out interpolation
         */
        inline float LerpEaseInOutBack(const float from, const float to, const float t)
        {
            const float c1 = 1.70158f;
            const float c2 = c1 * 1.525f;
            float eased = (t < 0.5f) ? (pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f :
                         (pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
            return from + (to - from) * eased;
        }

        /**
         * @brief Bounce ease-in interpolation
         */
        inline float LerpEaseInBounce(const float from, const float to, const float t)
        {
            float eased = 1.0f - LerpEaseOutBounce(0.0f, 1.0f, 1.0f - t);
            return from + (to - from) * eased;
        }

        /**
         * @brief Bounce ease-out interpolation
         */
        inline float LerpEaseOutBounce(const float from, const float to, const float t)
        {
            const float n1 = 7.5625f;
            const float d1 = 2.75f;

            float eased;
            if (t < 1.0f / d1)
            {
                eased = n1 * t * t;
            }
            else if (t < 2.0f / d1)
            {
                eased = n1 * (t -= 1.5f / d1) * t + 0.75f;
            }
            else if (t < 2.5f / d1)
            {
                eased = n1 * (t -= 2.25f / d1) * t + 0.9375f;
            }
            else
            {
                eased = n1 * (t -= 2.625f / d1) * t + 0.984375f;
            }
            return from + (to - from) * eased;
        }

        /**
         * @brief Bounce ease-in-out interpolation
         */
        inline float LerpEaseInOutBounce(const float from, const float to, const float t)
        {
            float eased = (t < 0.5f) ? (1.0f - LerpEaseOutBounce(0.0f, 1.0f, 1.0f - 2.0f * t)) / 2.0f :
                         (1.0f + LerpEaseOutBounce(0.0f, 1.0f, 2.0f * t - 1.0f)) / 2.0f;
            return from + (to - from) * eased;
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
