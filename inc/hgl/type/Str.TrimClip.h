#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
namespace hgl
{

    /**
    * 截去字符串前端所有的指定字符
    * @param src 源字符串指针
    * @param len 源字符串长度(同样用于返回结果字符串长度)
    * @param trimfunc 截取字符判断函数(默认isspace<T>)
    * @return 新的字符串起始指针
    */
    template<typename T>
    inline const T *trimleft(const T *src,int &len,const bool (*trimfunc)(const T &)=hgl::isspace<T>)
    {
        const T *p=src;

        while(*p&&trimfunc(*p)&&len)
        {
            p++;
            len--;
        }

        if(len<=0)
            return(nullptr);

        return p;
    }

    /**
    * 截去字符串尾端所有的指定字符
    * @param src 源字符串指针
    * @param len 源字符串长度(同样用于返回结果字符串长度)
    * @param trimfunc 截取字符判断函数(默认isspace<T>)
    * @return 新的字符串起始指针
    */
    template<typename T>
    inline const T *trimright(const T *src,int &len,const bool (*trimfunc)(const T &)=hgl::isspace<T>)
    {
        if(!src || len<=0)
            return nullptr;

        const T *p=src+len-1;

        while(len>0 && trimfunc(*p))
        {
            --p;
            --len;
        }

        if(len<=0)
            return(nullptr);

        return src;
    }

    /**
    * 截去字符串前端和尾端的所有指定字符
    * @param src 源字符串指针
    * @param len 源字符串长度(同样用于返回结果字符串长度)
    * @param trimfunc 截取字符判断函数(默认isspace<T>)
    * @return 新的字符串起始指针
    */
    template<typename T>
    inline const T *trim(const T *src,int &len,const bool (*trimfunc)(const T &)=hgl::isspace<T>)
    {
        if(!src||len<=0)
            return nullptr;

        const T *sp=src;
        const T *ep=src+len-1;

        while(*sp&&trimfunc(*sp)&&len)
        {
            ++sp;
            --len;
        }

        while(len>0 && trimfunc(*ep))
        {
            --ep;
            --len;
        }

        if(len<=0)
            return(nullptr);

        return sp;
    }

    /**
    * 截取字符串前端的字符串
    * @param src 源字符串指针
    * @param len 源字符串长度(同样用于返回结果字符串长度)
    * @param clipfunc 不可用字符判断函数(默认isspace<T>)
    * @return 新的字符串起始指针
    */
    template<typename T>
    inline const T *clipleft(const T *src,int &len,const bool (*clipfunc)(const T &)=hgl::isspace<T>)
    {
        // corrected boundary checks
        if(!src || len<=0) return(nullptr);

        const T *p=src;
        const T *end = src + len;

        while(p<end && *p && !clipfunc(*p))
            ++p;

        if(p==src)
            return(nullptr);

        len = int(p - src);
        return src;
    }

    /**
    * 截去字符串尾端的字符串
    * @param src 源字符串指针
    * @param len 源字符串长度(同样用于返回结果字符串长度)
    * @param clipfunc 不可用字符判断函数(默认isspace<T>)
    * @return 新的字符串起始指针
    */
    template<typename T>
    inline const T *clipright(const T *src,int &len,const bool (*clipfunc)(const T &)=hgl::isspace<T>)
    {
        // corrected boundary checks
        if(!src || len<=0) return(nullptr);

        const T *p=src+len-1;
        const T *begin = src;

        while(p>=begin && !clipfunc(*p))
            --p;

        if(p<begin)
            return(nullptr);

        len = int((src+len) - (p+1));
        return p+1;
    }
}//namespace hgl
