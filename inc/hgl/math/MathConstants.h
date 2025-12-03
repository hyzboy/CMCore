/**
 * MathConstants.h - 基础数学常量
 * 
 * 包含：
 * - 自然常数 e 及其对数
 * - 圆周率 π 及其派生常量
 * - 根号 2 及其派生常量
 */

#pragma once

namespace hgl
{
    // ==================== 自然常数 e ====================
    
    constexpr const double HGL_E          = 2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274;        ///<欧拉数(自然对数的底数)
    constexpr const double HGL_LOG2E      = 1.44269504088896340736;                              ///<log₂(e)
    constexpr const double HGL_LOG10E     = 0.434294481903251827651;                             ///<log₁₀(e)
    constexpr const double HGL_LN2        = 0.693147180559945309417;                             ///<ln(2)
    constexpr const double HGL_LN10       = 2.30258509299404568402;                              ///<ln(10)

    // ==================== 圆周率 π ====================
    
    constexpr const double HGL_PI         = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068;        ///<圆周率 π
    constexpr const double HGL_PI_2       = 1.57079632679489661923;                              ///<π/2
    constexpr const double HGL_PI_4       = 0.785398163397448309616;                             ///<π/4
    constexpr const double HGL_PI_3_4     = 2.356194490192344928938;                             ///<3π/4
    constexpr const double HGL_1_PI       = 0.318309886183790671538;                             ///<1/π
    constexpr const double HGL_2_PI       = 0.636619772367581343076;                             ///<2/π
    constexpr const double HGL_2_SQRTPI   = 1.12837916709551257390;                              ///<2/√π

    // ==================== 根号常量 ====================
    
    constexpr const double HGL_SQRT2      = 1.41421356237309504880168872420969807;              ///<√2
    constexpr const double HGL_SQRT3      = 1.73205080756887729352744634150587236694280525381038; ///<√3
    constexpr const double HGL_SQRT1_2    = 0.707106781186547524401;                             ///<√(1/2) = 1/√2

    // ==================== 黄金比例 ====================
    
    constexpr const double HGL_PHI        = 1.61803398874989484820458683436563811772030917980576; ///<黄金比例 φ = (1 + √5)/2

    // ==================== 特殊数学常数 ====================
    
    constexpr const double HGL_EULER_GAMMA = 0.57721566490153286060651209008240243104215933593992; ///<欧拉-马斯凯罗尼常数 γ
    constexpr const double HGL_CATALAN_G   = 0.91596559417721901505460351493238411077414937428167; ///<卡塔兰常数 G
    constexpr const double HGL_APERY_ZETA3 = 1.20205690315959428539973816151144999076498629234049; ///<阿培里常数 ζ(3)
}//namespace hgl
