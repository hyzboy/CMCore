#pragma once

#include<hgl/platform/Platform.h>
#include<cstring>

namespace hgl
{
    //==================================================================================================
    // 交换与比较 / Swap and Comparison
    //==================================================================================================
    
    template<typename T> inline void hgl_swap(T &x,T &y)
    {
        T t;

        std::memcpy(&t,&x,sizeof(T));
        std::memcpy(&x,&y,sizeof(T));
        std::memcpy(&y,&t,sizeof(T));
    }
    
    template<typename T> T hgl_clamp(const T &cur,const T &min_value,const T &max_value)
    {
        if(cur<min_value)return min_value;
        if(cur>max_value)return max_value;

        return cur;
    }

    template<typename T> inline T hgl_abs(const T &v){return (v>=0?v:-v);}

    template<typename T> inline void get_min(T &a,const T &b)
    {
        if(a>b)
            a=b;
    }

    template<typename T> inline void get_max(T &a,const T &b)
    {
        if(a<b)
            a=b;
    }

    template<typename T> inline T hgl_min(const T &a,const T &b){return (a>b?b:a);}
    template<typename T> inline T hgl_max(const T &a,const T &b){return (a>b?a:b);}

    template<typename T> inline T hgl_min(const T *data,int count,T min_value)
    {
        T value=min_value;

        for(int i=0;i<count;i++)
        {
            if(*data<value)
                value=*data;

            ++data;
        }

        return(value);
    }

    template<typename T> inline T hgl_max(const T *data,int count,T max_value)
    {
        T value=max_value;

        for(int i=0;i<count;i++)
        {
            if(*data>value)
                value=*data;

            ++data;
        }

        return(value);
    }
}//namespace hgl
