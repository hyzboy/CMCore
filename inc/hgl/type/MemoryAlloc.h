#pragma once

#include<hgl/platform/Platform.h>
#include<cstring>

namespace hgl
{
    //==================================================================================================
    // 内存分配 / Memory Allocation
    //==================================================================================================
    
    /**
     * 分配指定类型数据块并清0
     */
    template<typename T>
    inline T *hgl_zero_new()
    {
        T *data=new T;
        std::memset(data,0,sizeof(T));
        return data;
    }

    /**
     * 分配指定类型数据块并清0
     */
    template<typename T>
    inline T *hgl_zero_new(const size_t count)
    {
        if(count<=0)return(nullptr);

        T *data=new T[count];
        std::memset(data,0,count*sizeof(T));
        return data;
    }

    template<typename T>
    inline T *hgl_new_copy(const T *src,const size_t count)
    {
        if(!src||count<=0)return(nullptr);

        T *data=new T[count];
        std::memcpy(data,src,count*sizeof(T));
        return data;
    }

    template<typename T>
    inline T *hgl_new_copy(const T *src)
    {
        if(!src)return(nullptr);

        T *data=new T;
        std::memcpy(data,src,sizeof(T));
        return data;
    }

    //==================================================================================================
    // 数组内存管理 / Array Memory Management
    //==================================================================================================
    
    template<typename T>
    inline T *array_alloc(const uint count)
    {
        return (T *)hgl_malloc(count*sizeof(T));
    }

    template<typename T>
    inline T *array_realloc(T *origin,const uint count)
    {
        return (T *)hgl_realloc(origin,count*sizeof(T));
    }

    template<typename T>
    inline void array_free(T *items)
    {
        hgl_free(items);
    }
}//namespace hgl
