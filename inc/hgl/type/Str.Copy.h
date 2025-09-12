#pragma once

namespace hgl
{
    /**
    * 复制字符串，并指定最大长度
    * @param dst 目标字符串
    * @param count 目标字符串最大多少个字符
    * @param src 源字符串
    * @return 字符串长度(<0表示出错)
    */
    template<typename T>
    inline const int strcpy(T *dst,int count,const T *src)
    {
        if(!dst)return(-1);

        if(!src||!(*src)||count<=0)
        {
            if(count>0)
                *dst=0;
            return(0);
        }

        if(dst==src)
            return(-1);

        T *start=dst;

        // 保留一个位置给终止符
        while(*src && count>1)
        {
            *dst=*src;
            ++dst;
            ++src;
            --count;
        }

        if(count>0)
            *dst=0;

        return int(dst-start);
    }

    /**
    * 复制字符串，并指定最大长度
    * @param dst 目标字符串
    * @param dst_count 目标字符串最大多少个字符
    * @param src 源字符串
    * @param count 最多复制多少个字符
    * @return 字符串长度(<0表示出错)
    */
    template<typename T>
    inline const int strcpy(T *dst,int dst_count,const T *src,int count)
    {
        if(!dst)return(-1);

        if(!src||!(*src)||dst_count<=0||count<=0)
        {
            //没什么好复制的
            *dst=0;
            return(0);
        }

        if(dst==src)
            return(-1);

        T *start=dst;

        // 至少保留1个结尾0
        while(*src&&dst_count>1&&count)
        {
            *dst=*src;
            ++dst;
            ++src;
            --dst_count;
            --count;
        }

        if(dst_count>0)
            *dst=0;

        return(dst-start);
    }

    /**
    * 复制一个字符到当前字符串后面
    * @param dst 目标字符串
    * @param max_count 目标字符串最大长度
    * @param ch 源字符
    */
    template<typename T>
    inline void strcat(T *dst,int max_count,const T ch)
    {
        if(!dst||!ch)return;

        // 移动到结尾，max_count 为总容量，逐步减少
        while(*dst && max_count>0)
        {
            ++dst;
            --max_count;
        }

        // 需要至少两个位置：一个字符+终止符
        if(max_count>1)
        {
            *dst++=ch;
            *dst=0;
        }
    }

    /**
    * 复制一个字符串到当前字符串后面，并指定最大长度
    * @param dst 目标字符串
    * @param max_count 目标字符串最大长度
    * @param src 要追加的源字符串
    * @param count 源字符串最大长度
    * @return 字符串长度(<0表示出错)
    */
    template<typename T>
    inline const int strcat(T *dst,int max_count,const T *src,int count)
    {
        if(!dst||!src||!(*src)||count<=0)return(-1);

        T *start=dst;

        // 前移到末尾
        while(*dst && max_count>0)
        {
            ++dst;
            --max_count;
        }

        // 复制，保留终止符
        while(*src && max_count>1 && count>0)
        {
            *dst=*src;
            ++dst;
            ++src;
            --count;
            --max_count;
        }

        if(max_count>0)
            *dst=0;

        return int(dst-start);
    }

    /**
    * 创建一个复制
    * @param str 要复制的字符串
    * @param size 字符串长度,-1表示自动测试
    * @return 复制出来的字符串
    */
    template<typename T>
    inline T *create_copy(const T *str,int size=-1)
    {
        if(!str)return(nullptr);

        T *copy;

        if(size==-1)size=hgl::strlen(str);

        if(size==0)return(nullptr);

        ++size;

        copy=new T[size];
        hgl::strcpy(copy,size,str);

        return(copy);
    }
}//namespace hgl
