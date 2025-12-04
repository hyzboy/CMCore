#pragma once

#include<hgl/platform/Platform.h>
#include<cstring>

namespace hgl
{
    //==================================================================================================
    // 构造 / Construction
    //==================================================================================================
    
    template<typename T>
    inline void hgl_call_construct(T *obj)      //呼叫构造函数
    {
        new (static_cast<void *>(obj)) T();
    }

    //==================================================================================================
    // 内存操作 - 复制 / Memory Operations - Copy
    //==================================================================================================
    
    /**
     * 同类型复制
     */
    template<typename T>
    inline void hgl_cpy(T &dst,const T &src)
    {
        std::memcpy(&dst,&src,sizeof(T));
    }

    /**
     * 数据类型转换赋值
     */
    template<typename S,typename D>
    inline void typeconv(D *dst,const S *src,const size_t count)
    {
        for(size_t i=0;i<count;i++)
        {
            *dst=D(*src);
            ++dst;
            ++src;
        }
    }

    /**
     * 同类型数据块复制
     */
    template<typename T>
    inline void hgl_cpy(T *dst,const T *src,const size_t count)
    {
        std::memcpy(dst,src,count*sizeof(T));
    }

    /**
     * 同类型数据块移动
     */
    template<typename T>
    inline void hgl_move(T *dst,const T *src,const size_t count)
    {
        std::memmove(dst,src,count*sizeof(T));
    }

    //==================================================================================================
    // 内存操作 - 赋值 / Memory Operations - Set
    //==================================================================================================
    
    /**
     * 指定类型数据块赋值
     */
    template<typename T>
    inline void hgl_set(T *data,const T value,const size_t count)
    {
        if(!data||count<=0)return;

        for(size_t i=0;i<count;i++)
        {
            *data=value;
            ++data;
        }
    }

    template<>
    inline void hgl_set<uint8>(uint8 *data,const uint8 value,const size_t count)
    {
        if(!data||count<=0)return;

        std::memset(data,value,count);
    }

    template<typename T>
    inline void hgl_set(T *data,const T *src,const size_t count)
    {
        if(!data||!src||count<=0)return;

        for(size_t i=0;i<count;i++)
        {
            std::memcpy(data,src,sizeof(T));
            ++data;
        }
    }

    //==================================================================================================
    // 内存操作 - 清零 / Memory Operations - Zero
    //==================================================================================================
    
    /**
     * 指定类型数据清0
     */
    template<typename T>
    inline void hgl_zero(T &data)
    {
        std::memset(&data,0,sizeof(T));
    }

    /**
     * 指定类型数据块清0
     */
    template<typename T>
    inline void hgl_zero(T *data,const size_t count)
    {
        std::memset(data,0,count*sizeof(T));
    }

    //==================================================================================================
    // 内存操作 - 比较 / Memory Operations - Compare
    //==================================================================================================
    
    template<typename T>
    inline int hgl_cmp(const T &a,const T &b)
    {
        return std::memcmp(&a,&b,sizeof(T));
    }

    template<typename T>
    inline int hgl_cmp(const T *a,const T *b,const size_t count)
    {
        return std::memcmp(a,b,count*sizeof(T));
    }
}//namespace hgl
