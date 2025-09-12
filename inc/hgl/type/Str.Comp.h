#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Case.h>

namespace hgl
{
    /**
    * 比较两个字符串的大小
    * @param src 要比较的字符串
    * @param dst 要比较的字符串
    * @return -1 src <  dst
    * @return  0 src == dst
    * @return +1 src >  dst
    */
    template<typename S,typename D>
    inline const int strcmp(S *src,D *dst)
    {
        if(!src)
        {
            if(!dst)return(0);
            else return(-1);
        }

        if(!dst)return(1);

        while((*src)&&(*dst)&&(*src==*dst))
        {
            ++src;
            ++dst;
        }

        return(*src-*dst);
    }

    /**
    * 比较两个字符串的大小
    * @param src 要比较的字符串1
    * @param src_size 要比较的字符串1长度
    * @param dst 要比较的字符串2
    * @param dst_size 要比较的字符串2
    * @return -1 src <  dst
    * @return  0 src == dst
    * @return +1 src >  dst
    */
    template<typename S,typename D>
    inline const int strcmp(S *src,int src_size,D *dst,int dst_size)
    {
        if(!src)
        {
            if(!dst)return(0);
            else return(-1);
        }

        if(!dst)return(1);

        if(src_size<=0)
        {
            if(dst_size<=0)return(0);
            else return(-1);
        }

        if(dst_size<=0)return(1);

        while(src_size&&dst_size)
        {
            if(*src!=*dst)
                return(*src-*dst);

            ++src;
            ++dst;
            --src_size;
            --dst_size;
        }

        return src_size-dst_size;
    }

    /**
    * 比较两个字符串的大小,并指定最大比较长度
    * @param src,dst 要比较的字符串
    * @param count 比较的最大长度
    * @return -1 src <  dst
    * @return  0 src == dst
    * @return +1 src >  dst
    */
    template<typename S,typename D>
    inline const int strcmp(S *src,D *dst,int count)
    {
        if(count<=0)return(0);

        if(!src)
        {
            if(!dst)return(0);
            else return(-1);
        }

        if(!dst)return(1);

        while(count&&(*src)&&(*dst))
        {
            if(*src!=*dst)
                return(*src-*dst);

            ++src;
            ++dst;
            --count;
        }

        if(count==0)
            return(0);

        return(*src-*dst);
    }

    /**
    * 比较两个字符串的大小(英文不区大小写)
    * @param src,dst 要比较的字符串
    * @return -1 src <  dst
    * @return  0 src == dst
    * @return +1 src >  dst
    */
    template<typename S,typename D>
    inline const int stricmp(S *src,D *dst)
    {
        if(!src)
        {
            if(!dst)return(0);
            else return(-1);
        }

        if(!dst)return(1);

        int gap;

        while(*src&&*dst)
        {
            gap=hgl::compare_char_icase(*src,*dst);

            if(gap)
                return gap;

            ++src;
            ++dst;
        }

        return(int(*src)-int(*dst));
    }

    /**
    * 比较两个字符串的大小(英文不区大小写)
    * @param src 要比较的字符串1
    * @param src_size 要比较的字符串1长度
    * @param dst 要比较的字符串2
    * @param dst_size 要比较的字符串2
    * @return -1 src <  dst
    * @return  0 src == dst
    * @return +1 src >  dst
    */
    template<typename S,typename D>
    inline const int stricmp(S *src,int src_size,D *dst,int dst_size)
    {
        if(!src)
        {
            if(!dst)return(0);
            else return(-1);
        }

        if(!dst)return(1);

        if(src_size<=0)
        {
            if(dst_size<=0)return(0);
            else return(-1);
        }

        if(dst_size<=0)return(1);

        int gap;

        while(src_size&&dst_size)
        {
            gap=hgl::compare_char_icase(*src,*dst);

            if(gap)
                return gap;

            ++src;
            ++dst;
            --src_size;
            --dst_size;
        }

        return src_size-dst_size;
    }

    /**
    * 比较两个字符串的大小(英文不区大小写)
    * @param src,dst 要比较的字符串
    * @param count 比较的最大长度
    * @return -1 src <  dst
    * @return  0 src == dst
    * @return +1 src >  dst
    */
    template<typename S,typename D>
    inline const int stricmp(S *src,D *dst,int count)
    {
        if(!src)
        {
            if(!dst)return(0);
            else return(-1);
        }

        if(!dst)return(1);

        int gap;

        while((count)&&(*src)&&(*dst))
        {
            gap=hgl::compare_char_icase(*src,*dst);

            if(gap)
                return gap;

            ++src;
            ++dst;
            --count;
        }

        if(count==0)
            return(0);

        return(*src-*dst);
    }

    /**
    * 字符集专用比较函数,只比较字母与数字，无视各种符号，无视大小写
    */
    template<typename S,typename D>
    inline const int charset_cmp(S *src,D *dst)
    {
        if(!src)
        {
            if(!dst)return(0);
            else return(-1);
        }

        if(!dst)return(1);

        int gap;

        while(*src&&*dst)
        {
            if(!hgl::is_alpha_numeric(*src)){++src;continue;}
            if(!hgl::is_alpha_numeric(*dst)){++dst;continue;}

            gap=hgl::compare_char_icase(*src,*dst);

            if(gap)
                return gap;

            ++src;
            ++dst;
        }

        return(*src-*dst);
    }
}//namespace hgl
