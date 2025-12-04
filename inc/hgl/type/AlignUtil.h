#pragma once

#include<hgl/platform/Platform.h>
#include<bit>

#if __cplusplus >= 202302L  //C++23
#include <memory>
#endif//

namespace hgl
{
    //==================================================================================================
    // 对齐计算 / Alignment Calculation
    //==================================================================================================
    
    /**
     * 求数值对齐后的值
     */
    template<typename T> constexpr const inline T hgl_align(const T value,const T alignment)
    {    
        return ((value+alignment-1)/alignment)*alignment;
    }

    /**
     * 求数值对齐后的值(仅支持2次幂)
     */
    template<typename T> constexpr const inline T hgl_align_pow2(const T value,const T alignment)
    {
    #if defined(__cpp_lib_align) && __cpp_lib_align >= 202306L
        return std::align_up(value,alignment);
    #else
        const T align_size=alignment-1;

        return (value+align_size)&(~align_size);
    #endif
    }

    /**
     * 取适合正巧大于当前数的2次幂值
     */
    template<typename T> constexpr const inline T power_to_2(T value)
    {
        T result=1;

        while(result<value)
            result<<=1;

        return(result);
    }

    template<typename T> constexpr const inline T power_to_2_down(T value)
    {
        return std::bit_floor(value);
    }

    /**
     * 向上对齐
     */
    template<typename T> constexpr const inline T align_up(T val, T alignment)      // align val to the next multiple of alignment
    {
        return (val + alignment - (T)1) & ~(alignment - (T)1);
    }

    /**
     * 向下对齐
     */
    template<typename T> constexpr const inline T align_down(T val, T alignment)    // align val to the previous multiple of alignment
    {
        return val & ~(alignment - (T)1);
    }

    /**
     * 求对齐数量
     */
    template<typename T> constexpr const inline T divide_rounding_up(T x, T y)
    {
        return (x + y - (T)1) / y;
    }
}//namespace hgl
