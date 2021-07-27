#ifndef HGL_ALGORITHM_MATH_FAST_TRIANGLE_FUNCTION_INCLUDE
#define HGL_ALGORITHM_MATH_FAST_TRIANGLE_FUNCTION_INCLUDE

#include<hgl/math/MathConst.h>
#include<math.h>
namespace hgl
{
    double Lsin(int angle);                                     ///<低精度sin计算,注意传入的参数为角度而非弧度
    double Lcos(int angle);                                     ///<低精度cos计算,注意传入的参数为角度而非弧度
    void Lsincos(int angle, double &s, double &c);              ///<低精度sin+cos计算,注意传入的参数为角度而非弧度

    /**
     * Approximates acos(x) with a max absolute error of 9.0x10^-3.
     * Valid in the range -1..1.
     */
    inline float Lacos(float x)
    {
        // Lagarde 2014, "Inverse trigonometric functions GPU optimization for AMD GCN architecture"
        // This is the approximation of degree 1, with a max absolute error of 9.0x10^-3
        float y = fabs(x);
        float p = -0.1565827 * y + 1.570796;
        p *= sqrt(1.0 - y);
        return x >= 0.0 ? p : HGL_PI - p;
    }

    /**
     * Approximates acos(x) with a max absolute error of 9.0x10^-3.
     * Valid only in the range 0..1.
     */
    inline float LacosPositive(float x)
    {
        float p = -0.1565827 * x + 1.570796;
        return p * sqrt(1.0 - x);
    }

    /**
     * 低精度atan函数
     */
    inline double Latan(double z)
    {
        constexpr double n1 = 0.97239411f;
        constexpr double n2 = -0.19194795f;

        return (n1 + n2 * z * z) * z;
    }

    double Latan2(double y, double x);                          ///<低精度atan2函数

    /**
     * 近似Pow2.2，使用此函数等于pow(x,2.2)
     */
    inline double LPow22(double x)
    {
        return x*(1.12*x - 0.12);
    }
}//namespace hgl
#endif//HGL_ALGORITHM_MATH_FAST_TRIANGLE_FUNCTION_INCLUDE
