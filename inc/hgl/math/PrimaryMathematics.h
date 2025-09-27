#ifndef HGL_Primary_Mathematics_INCLUDE
#define HGL_Primary_Mathematics_INCLUDE

#include<hgl/platform/Platform.h>
namespace hgl
{
    /**
     * 求一批数的合
     */
    template<typename R,typename T>
    void sum(R *result,const T *data,const int count)
    {
        if(!result || !data || count <= 0)return;

        *result=0;

        for(int i=0;i<count;i++)
        {
            *result+=*data;
            ++data;
        }
    }

    template<typename T,typename T2>
    inline constexpr T Clamp(const T &value,const T2 &min_value,const T2 &max_value)
    {
        if(value<min_value)return min_value;
        if(value>max_value)return max_value;

        return value;
    }

    template<typename T>
    inline constexpr T Clamp(const T &value)
    {
        return Clamp<T>(value,0,1);
    }

    inline constexpr uint8 ClampU8(const int &value)
    {
        if(value<0)return 0;
        if(value>0xFF)return 0xFF;
        return value;
    }

    inline constexpr uint16 ClampU16(const int value)
    {
        if(value<0)return 0;
        if(value>0xFFFF)return 0xFFFF;
        return value;
    }
}//namespace hgl
#endif//HGL_Primary_Mathematics_INCLUDE
