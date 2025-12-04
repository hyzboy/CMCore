#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    //==================================================================================================
    // 位操作 / Bit Operations
    //==================================================================================================
    
    #define HGL_BIT(n)      (1<<(n))
    #define HGL_64BIT(n)    (1L<<(n))

    template<typename T>
    inline bool hgl_is_one(const T value,int off)
    {
        return value&(1<<off);
    }

    template<typename T>
    inline bool hgl_is_zero(const T value,int off)
    {
        return !(value&(1<<off));
    }

    template<typename T>
    inline void hgl_set_one(T &value,int off)
    {
        value|=(1<<off);
    }

    template<typename T>
    inline void hgl_set_zero(T &value,int off)
    {
        value&=~T(1<<off);
    }

    template<typename T>
    inline void hgl_set_bit(T &value,int off,bool b)
    {
        if(b)
            hgl_set_one(value,off);
        else
            hgl_set_zero(value,off);
    }

    template<typename T>
    inline int hgl_bit_count(const T value)
    {
        int count=0;
        T bit=1;

        for(int i=0;i<sizeof(T);i++)
        {
            if(value&bit)
                ++count;

            bit<<=1;
        }

        return count;
    }

    template<typename T>
    inline int hgl_bit_count(const T value,int size)
    {
        int count=0;
        T bit=1;

        for(int i=0;i<size;i++)
        {
            if(value&bit)
                ++count;

            bit<<=1;
        }

        return count;
    }

    template<typename T>
    constexpr int GetBitOffset(const T value)
    {
        int offset=0;
        T bit=1;

        for(int i=0;i<sizeof(T)*8;i++)
        {
            if(value&bit)
                return offset;

            bit<<=1;
            ++offset;
        }

        return -1;
    }
}//namespace hgl
