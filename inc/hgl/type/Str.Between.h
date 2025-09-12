#pragma once
#include <hgl/type/Str.Length.h>

namespace hgl
{
    /**
    * 从一个字符串中截取两个指定字符中间的子串
    * @param str 源字符串
    * @param str_length 源字符串字度
    * @param start_char 起始字符
    * @param end_char 结束字符
    * @param between_length 截取的子串长度
    * @return 截取的子串起始指针
    * @return nullptr 错误
    */
    template<typename T>
    inline const T *getbetween(const T *str,int str_length,const T start_char,const T end_char,int *between_length)
    {
        if(!str||!*str||str_length<=0)
            return(nullptr);

        const T *start=nullptr;

        while(*str&&str_length>0)
        {
            --str_length;

            if(*str==start_char)
            {
                ++str;
                start=str;
                break;
            }

            ++str;
        }

        if(!start)return(nullptr);

        while(*str&&str_length>0)
        {
            --str_length;

            if(*str==end_char)
            {
                *between_length=str-start;
                return(start);
            }

            ++str;
        }

        return(nullptr);
    }

    /**
    * 从一个字符串中截取指定字符中间的子串
    * @param str 源字符串
    * @param str_length 源字符串字度
    * @param edge_char 边界字符
    * @param between_length 截取的子串长度
    * @return 截取的子串起始指针
    * @return nullptr 错误
    */
    template<typename T>
    inline const T *getbetween(const T *str,int str_length,const T edge_char,int *between_length)
    {
        return hgl::getbetween(str,str_length,edge_char,edge_char,between_length);
    }

    /**
    * 从一个字符串中截取两个指定字符中间的子串
    * @param str 源字符串
    * @param start_char 起始字符
    * @param end_char 结束字符
    * @param between_length 截取的子串长度
    * @return 截取的子串起始指针
    * @return nullptr 错误
    */
    template<typename T>
    inline const T *getbetween(const T *str,const T start_char,const T end_char,int *between_length)
    {
        return hgl::getbetween(str,hgl::strlen(str),start_char,end_char,between_length);
    }

    /**
    * 从一个字符串中截取指定字符中间的子串
    * @param str 源字符串
    * @param edge_char 边界字符
    * @param between_length 截取的子串长度
    * @return 截取的子串起始指针
    * @return nullptr 错误
    */
    template<typename T>
    inline const T *getbetween(const T *str,const T edge_char,int *between_length)
    {
        return hgl::getbetween(str,hgl::strlen(str),edge_char,edge_char,between_length);
    }
}//namespace hgl
