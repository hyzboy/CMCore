#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    //==================================================================================================
    // 枚举类工具 / Enum Class Utilities
    //==================================================================================================
    
    #define ENUM_CLASS_RANGE(begin,end)     BEGIN_RANGE=begin,END_RANGE=end,RANGE_SIZE=(END_RANGE-BEGIN_RANGE)+1

    template<typename T> constexpr const int ToInt(const T &ec){return (int)ec;}
    template<typename T> constexpr const T FromInt(int ec){return (enum class T)ec;}

    template<typename T> constexpr const size_t RangeSize(){return T::RANGE_SIZE;}

    template<typename T>
    inline bool RangeCheck(const T &value)
    {
        return (value>=T::BEGIN_RANGE&&value<=T::END_RANGE);
    }

    #define ENUM_CLASS_FOR(ECTYPE,ctype,value)    \
            for(ctype value= (ctype)ECTYPE::BEGIN_RANGE;    \
                      value<=(ctype)ECTYPE::END_RANGE;  \
                      value++)

    #define RANGE_CHECK_RETURN(value,return_value)      if(!RangeCheck(value))return(return_value);
    #define RANGE_CHECK_RETURN_ZERO(value)              if(!RangeCheck(value))return(0);
    #define RANGE_CHECK_RETURN_FALSE(value)             if(!RangeCheck(value))return(false);
    #define RANGE_CHECK_RETURN_NULLPTR(value)           if(!RangeCheck(value))return(nullptr);
}//namespace hgl
