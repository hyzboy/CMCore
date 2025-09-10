﻿#pragma once

#include<hgl/TypeFunc.h>
namespace hgl
{
    //  32  空格
    //  33  !
    //  34  "
    //  35  #
    //  36  $
    //  37  %
    //  38  &
    //  39  '
    //  40  (
    //  41  )
    //  42  *
    //  43  +
    //  44  ,
    //  45  -
    //  46  .
    //  47  /
    //  48  0123456789
    //  58  :
    //  59  ;
    //  60  <
    //  61  =
    //  62  >
    //  63  ?
    //  64  @
    //  65  ABCDEFGHIJKLMNOPQRSTUVWXYZ
    //  91  [
    //  92  \
    //  93  ]
    //  94  ^
    //  95  _
    //  96  `
    //  97  abcdefghijklmnopqrstuvwxyz
    //  123 {
    //  124 |
    //  125 }
    //  126 ~

    /**
     * 测试字符是否是emoji表情<br>
     * 参见https://unicode.org/Public/emoji/12.0/emoji-data.txt
     */
    template<typename T>
    inline const bool isemoji(const T &ch)
    {
        if(ch==0x23)return(true);           //#
        if(ch==0x2A)return(true);           //*
        if(ch>=0x30&&ch<=0x39)return(true); //0-9
        if(ch==0xA9)return(true);           //©
        if(ch==0xAE)return(true);           //®
        if(ch>=0x203C&&ch<=0x1FFFD)return(true);

        return(false);
    }

    /**
     * 测试当前字符是否为小写字母
     */
    template<typename T>
    inline const bool islower(const T &ch)
    {
        return(ch>='a'&&ch<='z');
    }

    /**
     * 测试当前字符是否为大写字母
     */
    template<typename T>
    inline const bool isupper(const T &ch)
    {
        return(ch>='A'&&ch<='Z');
    }

    /**
     * 测试当前字符是否为字母
     */
    template<typename T>
    inline const bool isalpha(const T &ch)
    {
        return(islower(ch)||isupper(ch));
    }

    /**
     * 测试当前字符是否为10进制数字
     */
    template<typename T>
    inline const bool isdigit(const T &ch)
    {
        return(ch>='0'&&ch<='9');
    }

    /**
     * 测试当前字符串是否为10进制数字以及小数点、正负符号、指数字符
     */
    template<typename T>
    inline const bool isfloat(const T &ch)
    {
        return isdigit(ch)
        ||ch=='-'
        ||ch=='+'
        ||ch=='.'
        ||ch=='E'
        ||ch=='e'
        ||ch=='f'
        ||ch=='F';
    }

    template<typename T>
    inline const bool isinteger(const T &ch)
    {
        return isdigit(ch)
        ||ch=='-'
        ||ch=='+';
    }

    /**
     * 测试当前字符是否为16进制数用字符(0-9,A-F)
     */
    template<typename T>
    inline const bool isxdigit(const T &ch)
    {
        return((ch>='0'&&ch<='9')
        ||(ch>='a'&&ch<='f')
        ||(ch>='A'&&ch<='F')
        ||ch=='x'||ch=='X');
    }

    /**
     * 测试当前字符串是否为16进制数用字符
     * @param str 字符串
     * @param length 字符串长度
     */
    template<typename T>
    inline const bool isxdigits(const T *str,int length)
    {
        if(!str||length<=0)
            return(false);

        while(*str&&length)
        {
            if(!isxdigit(*str))
                return(false);

            ++str;
            --length;
        }

        return(true);
    }

    /**
     * 是否为斜杠
     */
    template<typename T>
    inline const bool isslash(const T &ch)
    {
        if(ch=='\\')return(true);
        if(ch=='/')return(true);

        return(false);
    }

    template<typename T> inline const bool isspace(const T &);

    /**
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline const bool isspace(const u32char &ch)
    {
        return(ch==0
             ||ch==U' '              //半角空格
             ||ch==U32_FULL_WIDTH_SPACE   //全角空格
             ||ch==U'\a'
             ||ch==U'\b'
             ||ch==U'\f'
             ||ch==U'\n'
             ||ch==U'\r'
             ||ch==U'\t'
             ||ch==U'\v');
    }

    /**
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline const bool isspace(const u16char &ch)
    {
        return(ch==0
             ||ch==U16_TEXT(' ')    //半角空格
             ||ch==U16_FULL_WIDTH_SPACE   //全角空格
             ||ch==U16_TEXT('\a')
             ||ch==U16_TEXT('\b')
             ||ch==U16_TEXT('\f')
             ||ch==U16_TEXT('\n')
             ||ch==U16_TEXT('\r')
             ||ch==U16_TEXT('\t')
             ||ch==U16_TEXT('\v'));
    }

    /**
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline const bool isspace(const char &ch)
    {
        return(ch==0
             ||ch==' '              //半角空格
             ||ch=='\a'
             ||ch=='\b'
             ||ch=='\f'
             ||ch=='\n'
             ||ch=='\r'
             ||ch=='\t'
             ||ch=='\v');
    }

    #ifdef HGL_SUPPORT_CHAR8_T
    template<>
    inline const bool isspace(const char8_t &ch)
    {
        return(ch==0
             ||ch==u8' '              //半角空格
             ||ch==u8'\a'
             ||ch==u8'\b'
             ||ch==u8'\f'
             ||ch==u8'\n'
             ||ch==u8'\r'
             ||ch==u8'\t'
             ||ch==u8'\v');
    }
    #endif//char8_t

    /**
     * 测试当前字符是否为字母或数字
     */
    template<typename T>
    inline const bool isalnum(const T &ch)
    {
        return(isalpha(ch)||isdigit(ch));
    }

    /**
     * 测试当前字符是否为代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline const bool iscodechar(const T &ch)
    {
        return(isalnum(ch)||ch=='_');
    }

    /**
     * 测试当前字符是否不是代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline const bool notcodechar(const T &ch)
    {
        return(!iscodechar(ch));
    }

    /**
     * 测试当前字符是否为文件名可用字符(不含路径分隔符)
     */
    template<typename T>
    inline const bool isfilenamechar(const T &ch)
    {
        return(ch=='.'||iscodechar(ch));
    }

    /**
     * 测试当前字符是否不是文件名可用字符
     */
    template<typename T>
    inline const bool notfilenamechar(const T &ch)
    {
        return(!isfilenamechar(ch));
    }

    /**
     * 测试当前字符是否为BASE64编码字符
     */
    template<typename T>
    inline const bool isbase64(const T &c)
    {
        return (c == 43 || // +
        (c >= 47 && c <= 57) || // /-9
        (c >= 65 && c <= 90) || // A-Z
        (c >= 97 && c <= 122)); // a-z
    }

    /**
     * 如果当前字符为大写英文字符，则转换为小写
     */
    template<typename T>
    inline const T tolower(const T ch)
    {
        if(ch>='A'&&ch<='Z')
            return ch+('a'-'A');
        else
            return ch;
    }

    /**
     * 如果当前字符为小写英文字符，则转换为大写
     */
    template<typename T>
    inline const T toupper(const T ch)
    {
        if(ch>='a'&&ch<='z')
            return ch+('A'-'a');
        else
            return ch;
    }

    /**
     * 比较两个字符的大小(英文不区分大小写)
     */
    template<typename S,typename D>
    inline const int chricmp(S src,D dst)
    {
        return tolower(src)-tolower(dst);
    }

    /**
     * 对宽字符串计算字串长度
     * @param str 要计算长度的字符串指针
     * @return 字符串长度
     */
    template<typename T>
    inline const int strlen(const T *str)
    {
        if(str&&*str)
        {
            const T *start=str;

            while(*str)
                ++str;

            return int(str-start);
        }

        return(0);
    }

    /**
     * 对宽字符串计算字串长度
     * @param str 要计算长度的字符串指针
     * @param max_len 最大长度
     * @return 字符串长度
     */
    template<typename T>
    inline const int strlen(const T *str,uint max_len)
    {
        if(str&&*str)
        {
            const T *start=str;

            do
            {
                ++str;
                --max_len;

            }while(max_len>0&&*str);

            return int(str-start);
        }

        return(0);
    }

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
            //没什么好复制的
            *dst=0;
            return(0);
        }

        if(dst==src)
            return(-1);

        if(!src||!(*src))
        {
            *dst=0;
            return(-2);
        }

        T *start=dst;

        while(*src&&count)
        {
            *dst=*src;
            ++dst;
            ++src;
            --count;
        }

        if(count>0)
            *dst=0;

        return(dst-start);
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

        while(*src&&dst_count&&count)
        {
            *dst=*src;
            ++dst;
            ++src;
            --dst_count;
            --count;
        }

        if(dst_count)
            *dst=0;

        return(dst-start);
    }

    /**
     * 在字符串str1内查找另一个字符串str2
     * @param str1 完整的字符串
     * @param size1 str1最大查找字符
     * @param str2 要查找的字符串
     * @param size2 str2长度
     * @return str2在str1中所在位置的指针
     */
    template<typename T1,typename T2>
    inline T1 *strstr(T1 *str1,const int size1,const T2 *str2,const int size2)
    {
        if(!str1||!str2)return(nullptr);
        if(!*str1||!*str2)return(nullptr);
        if(size1<=0)return(nullptr);
        if(size2<=0)return(nullptr);

        T1 *cp = str1;
        T1 *end= str1+size1-size2;
        T1 *s1, *s2;
        int s;

        while (*cp&&cp<=end)
        {
            s1 = cp;
            s2 = (T1 *)str2;

            s=size2;
            while ( s && !(*s1-*s2) )
                ++s1, ++s2,--s;

            if(!s)
                return(cp);

            ++cp;
        }

        return(nullptr);
    }


    /**
     * 在字符串str1内查找另一个字符串str2(从后向前)
     * @param str1 完整的字符串
     * @param size1 str1最大查找字符
     * @param str2 要查找的字符串
     * @param size2 str2长度
     * @return str2在str1中所在位置的指针
     */
    template<typename T1,typename T2>
    inline T1 *strrstr(T1 *str1,const int size1,const T2 *str2,const int size2)
    {
        if(!str1||!str2)return(nullptr);
        if(!*str1||!*str2)return(nullptr);
        if(size1<=0)return(nullptr);
        if(size2<=0)return(nullptr);

        T1 *cp = str1+size1-size2;
        T1 *s1, *s2;
        int s;

        while (*cp&&cp>str1)
        {
            s1 = cp;
            s2 = (T1 *)str2;

            s=size2;
            while ( s && !(*s1-*s2) )
                ++s1, ++s2,--s;

            if (!s)
                return(cp);

            --cp;
        }

        return(nullptr);
    }


    /**
     * 在字符串str1内查找另一个字符串str2,忽略大小写
     * @param str1 完整的字符串
     * @param size1 str1的长度
     * @param str2 要查找的字符串
     * @param size2 str2的长度
     * @return str2在str1中所在位置的指针
     * @return nullptr 没有找到
     */
    template<typename T1,typename T2>
    inline T1 *stristr(T1 *str1,const int size1,T2 *str2,const int size2)
    {
        if(!str1||!str2)return(nullptr);
        if(!*str1||!*str2)return(nullptr);
        if(size1<=0)return(nullptr);
        if(size2<=0)return(nullptr);

        T1 *cp = (T1 *) str1;
        T1 *s1;
        T2 *s2;
        int remain = size1;

        if ( !*str2)
            return (T1 *)str1;

        while (*cp && remain > 0)
        {
            s1 = cp;
            s2 = (T2 *) str2;
            int s = size2;

            while ( *s1 && *s2 && s > 0 )
            {
                if(*s1!=*s2)
                {
                    if(*s1>='a'&&*s1<='z')
                    {
                        if(*s1!=*s2-('A'-'a'))
                            break;
                    }
                    else
                        if(*s1>='A'&&*s1<='Z')
                        {
                            if(*s1!=*s2+('A'-'a'))
                                break;
                        }
                        else
                            break;
                }

                s1++, s2++; --s;
            }

            if (!*s2 || s == 0)
                return(cp);

            ++cp; --remain;
        }

        return(nullptr);
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

        while(*dst&&max_count)
        {
            ++dst;          //找到结束
            --max_count;
        }

        if(max_count>=0)
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

        while(*dst&&max_count)
        {
            ++dst;          //找到结束
            --max_count;
        }

        while(*src&&max_count&&count)
        {
            *dst=*src;
            ++dst;
            ++src;
            --count;
            --max_count;
        }

        if(max_count>=0)
            *dst=0;

        return(dst-start);
    }

    /**
     * 在字符串中查找某个字符
     * @param str 字符串
     * @param ch 字符
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC ch)
    {
        if(!str||!(*str)||ch==0)return(nullptr);

        while(*str)
        {
            if(*str==ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
     * 在字符串中查找多个字符中的任意一个,指定字符串最大长度
     * @param str 字符串
     * @param ch 字符
     * @param n 字符串长度
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC ch,int n)
    {
        if(!str||!(*str)||ch==0||n<=0)return(nullptr);

        while(*str&&n--)
        {
            if(*str==ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
     * 在字符串中查找多个字符中的任意一个
     * @param str 字符串
     * @param ch 字符列表
     * @param ch_count 字符个数
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC *ch,const int ch_count)
    {
        if(!str||!(*str)||!ch||!(*ch)||ch_count<=0)return(nullptr);

        while(*str)
        {
            if(strchr(ch,*str,ch_count))
                return(str);

            ++str;
        }

        return(nullptr);
    }

    /**
     * 在字符串中查找排除指定字符外的第一个字符
     * @param str 字符串
     * @param ch 要排除的字符
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC ch)
    {
        if(!str||!(*str)||ch==0)return(nullptr);

        while(*str)
        {
            if(*str!=ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
     * 在字符串中查找排除指定字符外的第一个字符,指定字符串最大长度
     * @param str 字符串
     * @param ch 要排除的字符
     * @param n 字符串长度
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC ch,int n)
    {
        if(!str||!(*str)||ch==0||n<=0)return(nullptr);

        while(*str&&n--)
        {
            if(*str!=ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
     * 在字符串中查找在字符串中查找排除指定字符外的第一个字符
     * @param str 字符串
     * @param ch 要排除的字符列表
     * @param ch_count 字符个数
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC *ch,const int ch_count)
    {
        if(!str||!(*str)||!ch||!(*ch)||ch_count<=0)return(nullptr);

        while(*str)
        {
            if(!strchr(ch,*str,ch_count))
                return(str);

            ++str;
        }

        return(nullptr);
    }

    /**
     * 在字符串中从结尾处开始查找某个字符
     * @param str 字符串
     * @param len 字符串长度
     * @param ch 字符
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const TC ch)
    {
        if(!str||!(*str)||len<=0||ch==0)return(nullptr);

        TS *ep=str+len-1;

        while(ep>=str)
        {
            if(*ep==ch)
                return ep;

            --ep;
        }

        return(nullptr);
    }

    /**
     * 在字符串中从结尾处开始查找某个字符
     * @param str 字符串
     * @param len 字符串长度
     * @param ch 字符
     * @param ch_count 字符个数
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const TC *ch,const int ch_count)
    {
        if(!str||!(*str)||len<=0||!ch||!(*ch)||ch_count<=0)return(nullptr);

        TS *ep=str+len-1;

        while(ep>=str)
        {
            if(strchr(ch,*ep,ch_count))
                return ep;

            --ep;
        }

        return(nullptr);
    }

    /**
     * 在字符串中从结尾处开始查找某个字符
     * @param str 字符串
     * @param len 字符串长度
     * @param off 起始查找位置(倒数)
     * @param ch 字符
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const int off,const TC ch)
    {
        if(!str||!(*str)||len<=0||off>=len||ch==0)return(nullptr);

        TS *ep=str+len-1-off;

        while(ep>=str)
        {
            if(*ep==ch)
                return ep;

            --ep;
        }

        return(nullptr);
    }

    /**
     * 在字符串中从结尾处开始查找某个字符
     * @param str 字符串
     * @param len 字符串长度
     * @param off 起始查找位置(倒数)
     * @param ch 字符
     * @param ch_count 字符个数
     * @return 查找到的位置指针
     */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const int off,const TC *ch,const int ch_count)
    {
        if(!str||!(*str)||len<=0||off>=len||!ch||!(*ch)||ch_count<=0)return(nullptr);

        TS *ep=str+len-1-off;

        while(ep>=str)
        {
            if(strchr(ch,*ep,ch_count))
                return ep;

            --ep;
        }

        return(nullptr);
    }

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
            gap=chricmp(*src,*dst);

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
            gap=chricmp(*src,*dst);

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
            gap=chricmp(*src,*dst);

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
            if(!isalnum(*src)){++src;continue;}
            if(!isalnum(*dst)){++dst;continue;}

            gap=chricmp(*src,*dst);

            if(gap)
                return gap;

            ++src;
            ++dst;
        }

        return(*src-*dst);
    }

    /**
     * 截去字符串前端所有的指定字符
     * @param src 源字符串指针
     * @param len 源字符串长度(同样用于返回结果字符串长度)
     * @param trimfunc 截取字符判断函数(默认isspace<T>)
     * @return 新的字符串起始指针
     */
    template<typename T>
    inline const T *trimleft(const T *src,int &len,const bool (*trimfunc)(const T &)=isspace<T>)
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
    inline const T *trimright(const T *src,int &len,const bool (*trimfunc)(const T &)=isspace<T>)
    {
        const T *p=src+len-1;

        while(trimfunc(*p)&&len)
        {
            p--;
            len--;
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
    inline const T *trim(const T *src,int &len,const bool (*trimfunc)(const T &)=isspace<T>)
    {
        const T *sp=src;
        const T *ep=src+len-1;

        while(*sp&&trimfunc(*sp)&&len)
        {
            ++sp;
            --len;
        }

        while(trimfunc(*ep)&&len)
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
    inline const T *clipleft(const T *src,int &len,const bool (*clipfunc)(const T &)=isspace<T>)
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
    inline const T *clipright(const T *src,int &len,const bool (*clipfunc)(const T &)=isspace<T>)
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

    /**
     * 给一个文件名更改扩展名
     * @param old_filename 原始文件名称
     * @param new_filename 新文件名称
     * @param max_len 文件名最大长度以
     * @param new_extname 新扩展名(不带.)
     */
    template<typename T>
    inline void replace_extname(T *new_filename,const T *old_filename,int max_len,const T *new_extname,const T &separative_char=T('.'))
    {
        const int old_len=hgl::strlen(old_filename);
        const T *p=hgl::strrchr(old_filename,old_len,separative_char);

        if(p)
        {
            strcpy(new_filename,max_len,old_filename,p-old_filename+1);
            strcpy(new_filename+(p-old_filename+1),max_len,new_extname);
        }
        else
        {
            strcpy(new_filename,max_len,old_filename,old_len);
            new_filename[old_len]='.';
            strcpy(new_filename+old_len+1,max_len-old_len,new_extname);
        }
    }

    /**
     * 给一个文件名更改扩展名
     * @param filename 文件名称
     * @param new_extname 新扩展名(不带.)
     */
    template<typename T>
    inline void replace_extname(T *filename,const T *new_extname)
    {
        T *p=strrchr(filename,u'.');

        if(p)
        {
            strcpy(p+1,new_extname);
        }
        else
        {
            strcat(filename,u'.');
            strcat(filename,new_extname);
        }
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
        if(!str)return(0);

        T *copy;

        if(size==-1)size=strlen(str);

        if(size==0)return(0);

        ++size;

        copy=new T[size];
        strcpy(copy,str,size);

        return(copy);
    }

    /**
     * 将一个字符串中的某个字符全部替换为另一个字符
     * @param string 要处理的字符串
     * @param sch 要被替换掉的字符
     * @param tch 替换后的字符
     * @return 替换的次数
     */
    template<typename T>
    inline const int replace(T *str,const T tch,const T sch)
    {
        if(!str)
            return(0);

        int count=0;

        while(*str)
        {
            if(*str==sch)
            {
                *str=tch;
                ++count;
            }

            ++str;
        }

        return(count);
    }

    /**
     * 将一个字符串中的字母全部改为大写
     * @param src 要处理的字符串
     */
    template<typename T>
    inline void toupper(T *str)
    {
        if(!str)return;

        while(*str)
        {
            if(*str>='a'&&*str<='z')
                *str-=32;

            ++str;
        }
    }

    /**
     * 将一个字符串中的字母全部改为小写
     * @param src 要处理的字符串
     */
    template<typename T>
    inline void tolower(T *str)
    {
        if(!str)return;

        while(*str)
        {
            if(*str>='A'&&*str<='Z')
                *str+=32;

            ++str;
        }
    }

    /**
     * 将一个字符串中的字母全部改为大写
     * @param src 要处理的字符串
     */
    template<typename T>
    inline void toupper(const T *src,T *dst)
    {
        if(!dst)return;

        if(!src)
        {
            *dst=0;
            return;
        }

        do
        {
            if(*src>='a'&&*src<='z')
                *dst=*src-32;
            else
                *dst=*src;

            ++src;
            ++dst;
        }while(*src);

        *dst=0;
    }

    /**
     * 将一个字符串中的字母全部改为小写
     * @param src 要处理的字符串
     */
    template<typename T>
    inline void tolower(const T *src,T *dst)
    {
        if(!dst)return;

        if(!src)
        {
            *dst=0;
            return;
        }

        do
        {
            if(*src>='A'&&*src<='Z')
                *dst=*src+32;
            else
                *dst=*src;

            ++src;
            ++dst;
        }while(*src);

        *dst=0;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成小写
    */
    template<typename T>
    inline const uint lower_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source>='A'&&*source<='Z')
                *target=*source+32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
        }

        *target=0;
        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写
    */
    template<typename T>
    inline const uint upper_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source>='a'&&*source<='z')
                *target=*source-32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
        }

        *target=0;
        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成小写
    */
    template<typename T>
    inline const uint lower_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source>='A'&&*source<='Z')
                *target=*source+32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
            --source_max;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写
    */
    template<typename T>
    inline const uint upper_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source>='a'&&*source<='z')
                *target=*source-32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
            --source_max;
        }

        *target=0;
        return count;
    }

    /**
    * 复制一个字符串，将字符串全部转换成小写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint lower_clip_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source!=' ')
            {
                if(*source>='A'&&*source<='Z')
                    *target=*source+32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint upper_clip_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source!=' ')
            {
                if(*source>='a'&&*source<='z')
                    *target=*source-32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，将字符串全部转换成小写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint lower_clip_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source!=' ')
            {
                if(*source>='A'&&*source<='Z')
                    *target=*source+32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
            --source_max;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint upper_clip_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source!=' ')
            {
                if(*source>='a'&&*source<='z')
                    *target=*source-32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
            --source_max;
        }

        *target=0;
        return count;
    }

    /**
     * 统计在字符串中某个字符的出现次数
     * @param str 字符串
     * @param ch
     * @return 出现次数
     */
    template<typename T>
    inline const int stat_char(T *str,T ch)
    {
        if(!str)return(0);

        int count=0;

        while(*str)
        {
            if(*str==ch)
                ++count;

            ++str;
        }

        return(count);
    }

    /**
     * 统计一个字符串的行数
     */
    template<typename T>
    inline const int stat_line(T *str)
    {
        if(!str)return(0);

        T *p=str+strlen(str)-1;

        if(*p=='\n')return(stat_char(str,'\n'));
        else return(stat_char(str,'\n')+1);
    }

    template<typename R,typename S>
    inline const bool stoi(S *str,R &result)
    {
        if(!str)
        {
            result=0;
            return(false);
        }

        result=0;
        bool zf=true;

        if(*str=='+')
            ++str;

        if(*str=='-')
        {
            zf=false;
            ++str;
        }

        while(*str&&*str>='0'&&*str<='9')
        {
            result*=10;
            result+=(*str-'0');

            ++str;
        }

        if(!zf)
            result=-result;

        return(true);
    }

    template<typename R,typename S>
    inline const bool stoi(S *str,int size,R &result)
    {
        if(!str||size<=0)
        {
            result=0;
            return(false);
        }

        result=0;
        bool zf=true;

        if(*str=='+')
        {
            ++str;
            --size;
        }

        if(*str=='-')
        {
            zf=false;
            ++str; // fixed: advance past '-' instead of decrementing
            --size;
        }

        while(size>0&&*str&&*str>='0'&&*str<='9')
        {
            result*=10;
            result+=(*str-'0');

            ++str;
            --size;
        }

        if(!zf)
            result=-result;

        return(true);
    }

    template<typename R,typename S>
    inline const bool stou(S *str,R &result)
    {
        if(!str)
        {
            result=0;
            return(false);
        }

        result=0;

        while(*str&&*str>='0'&&*str<='9')
        {
            result*=10;
            result+=(*str-'0');

            ++str;
        }

        return(true);
    }

    template<typename R,typename S>
    inline const bool stou(S *str,int size,R &result)
    {
        if(!str||size<=0)
        {
            result=0;
            return(false);
        }

        result=0;

        while(size>0&&*str&&*str>='0'&&*str<='9')
        {
            result*=10;
            result+=(*str-'0');

            ++str;
            --size;
        }

        return(true);
    }

    template<typename R,typename S>
    inline const bool xtou(S *str,R &result)
    {
        if(!str)
        {
            result=0;
            return(false);
        }

        result=0;

        while(*str&&isxdigit(*str))
        {
            result*=16;

            if(*str>='0'&&*str<='9')
                result+=(*str-'0');
            else
                if(*str>='a'&&*str<='f')
                    result+=(*str-'a')+10;
                else
                    if(*str>='A'&&*str<='F')
                        result+=(*str-'A')+10;

                    ++str;
        }

        return(true);
    }

    template<typename R,typename S>
    inline const bool xtou(S *str,int size,R &result)
    {
        if(!str||size<=0)
        {
            result=0;
            return(false);
        }

        result=0;

        while(*str&&isxdigit(*str))
        {
            result*=16;

            if(*str>='0'&&*str<='9')
                result+=(*str-'0');
            else
                if(*str>='a'&&*str<='f')
                    result+=(*str-'a')+10;
                else
                    if(*str>='A'&&*str<='F')
                        result+=(*str-'A')+10;

                    ++str;
                --size;
        }

        return(true);
    }

    template<typename R,typename S>
    inline const bool stof(S *str,R &result)
    {
        if(!str)
        {
            result=0;
            return(false);
        }

        bool zf=true;

        result=0;

        if(*str=='+')
            ++str;

        if(*str=='-')
        {
            zf=false;
            ++str;
        }

        while(*str&&*str>='0'&&*str<='9')
        {
            result*=10;
            result+=(*str-'0');

            ++str;
        }

        if(*str!='.')
        {
            if(!zf)
                result=-result;

            return(true);
        }

        ++str;  //跳过小数点

        R pos=0.1f;

        while(*str&&*str>='0'&&*str<='9')
        {
            result+=R(*str-'0')*pos;

            pos*=0.1f;

            ++str;
        }

        if(!zf)
            result=-result;

        return(true);
    }

    template<typename R,typename S>
    inline const bool stof(S *str,int size,R &result)
    {
        if(!str||size<=0)
        {
            result=0;
            return(false);
        }

        bool zf=true;

        result=0;

        if(*str=='+')
        {
            ++str;
            --size;
        }

        if(*str=='-')
        {
            zf=false;
            ++str;
            --size;
        }

        while(size>0&&*str&&*str>='0'&&*str<='9')
        {
            result*=10;
            result+=(*str-'0');

            ++str;
            --size;
        }

        if(*str!='.')
        {
            if(!zf)
                result=-result;

            return(true);
        }

        ++str;  //跳过小数点
        --size;

        R pos=0.1f;

        while(size>0&&*str&&*str>='0'&&*str<='9')
        {
            result+=R(*str-'0')*pos;

            pos*=0.1f;

            ++str;
            --size;
        }

        if(!zf)
            result=-result;

        return(true);
    }

    /**
     * 转换带指数的字符串到数值变量(如"1.0123e-10")
     */
    template<typename R,typename S>
    inline const bool etof(S *str,R &result)
    {
        double temp;

        if(!stof(str,temp))
            return(false);

        while(*str!='e'&&*str!='E')
        {
            if(!*str)
            {
                result=R(temp);
                return(true);
            }

            ++str;
        }

        if(*str=='e'||*str=='E')
        {
            double power;

            if(stof(str+1,power))
            {
                result=R(temp*pow(10,power));

                return(true);
            }
        }

        result=R(temp);
        return(false);
    }

    /**
     * 转换一个字符串到bool型
     * @param str 要转换的字符串
     * @return 转换后的值
     */
    template<typename T>
    inline const bool stob(T *str,bool &value)
    {
        if(!str)
        {
            value=false;
            return(false);
        }

        if(*str=='T'||*str=='t'         //true/false
            ||*str=='Y'||*str=='y'          //yes/no
        ||*str=='M'||*str=='m'          //male/women
        ||*str=='1')                    //1/0
        {
            value=true;
            return(true);
        }
        else
        {
            value=false;
            return(false);
        }
    }

    /**
     * 转换一个有符号整数到字符串
     * @param str 转换后的字符串存放处
     * @param size 存放处可容纳的字符数
     * @param num 要转换的数值
     * @return 转换后的字符串长度
     */
    template<typename T,typename I>
    inline const int itos_rl(T *str,int size,const I num)
    {
        if(!str||size<=0)return(-1);

        T *p=str;
        T buf[sizeof(I)*8],*bp;

        I value=num;

        if(value<0)
        {
            *p++='-';
            --size;

            value=-value;
        }

        bp=buf;

        while(true)
        {
            *bp++=(value%10)+'0';

            if ((value = value / 10) == 0)
                break;
        }

        while(bp--!=buf&&size--)
            *p++=*bp;

        if(size)
            *p=0;

        return(p-str);
    }

    /**
     * 转换一个有符号整数到字符串
     * @param str 转换后的字符串存放处
     * @param size 存放处可容纳的字符数
     * @param num 要转换的数值
     * @return 转换后的字符串
     */
    template<typename T,typename I>
    inline T *itos(T *str,const int size,const I num)
    {
        itos_rl(str,size,num);
        return str;
    }

    /**
     * 转换一个无符号整数到字符串
     * @param str 转换后的字符串存放处
     * @param size 存放处可容纳的字符数
     * @param value 要转换的数值
     * @return 转换后的字符串
     */
    template<typename T,typename U>
    inline T *utos(T *str,int size,U value)
    {
        if(!str||size<=0)return(nullptr);

        T *p=str;
        T buf[sizeof(U)*8],*bp;

        bp=buf;

        while(true)
        {
            *bp++=(value%10)+'0';

            if ((value = value / 10) == 0)
                break;
        }

        while(bp--!=buf&&size--)
            *p++=*bp;

        if(size)
            *p=0;

        return(str);
    }

    /**
    * 转换一个无符号整数到字符串(指定进制)
    * @param str 转换后的字符串存放处
    * @param size 存放处可容纳的字符数
    * @param value 要转换的数值
    * @param base 数值进制
    * @param upper 字母用大写
    * @return 转换后的字符串
    */
    template<typename T,typename U>
    inline T *utos(T *str,int size,U value,const uint base,bool upper=true)
    {
        if(!str||size<=0)return(nullptr);

        T *p=str;
        T buf[sizeof(U)*8],*bp;
        uint m;

        const uchar A=upper?'A':'a';

        bp=buf;

        while(true)
        {
            m=value%base;

            if(m<10)
                *bp++=m+'0';
            else
                *bp++=m-10+A;

            value/=base;                

            if(!value)
                break;
        }

        while(bp--!=buf&&size--)
            *p++=*bp;

        if(size)
            *p=0;

        return(str);
    }

    template<int BITS> struct htos_bits
    {
        template<typename T,typename U> static T *conv(T *str,int size,U value,bool upper);
    };

    template<> struct htos_bits<1>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return utos(str,size,*(uint8  *)&value,16,upper);}};
    template<> struct htos_bits<2>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return utos(str,size,*(uint16 *)&value,16,upper);}};
    template<> struct htos_bits<4>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return utos(str,size,*(uint32 *)&value,16,upper);}};
    template<> struct htos_bits<8>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return utos(str,size,*(uint64 *)&value,16,upper);}};

    /**
     * 转换一个无符号整数到字符串(以16进制表示)
     * @param str 转换后的字符串存放处
     * @param size 存放处可容纳的字符数
     * @param value 要转换的数值
     * @param upper 大小写
     * @return 转换后的字符串
     */
    template<typename T,typename U>
    inline T *htos(T *str,int size,U value,bool upper=true)
    {
        return htos_bits<sizeof(U)>::template conv<T,U>(str,size,value,upper);
    }

    /**
     * 转换一个无符号整数到字符串(以大写16进制表示)
     * @param str 转换后的字符串存放处
     * @param size 存放处可容纳的字符数
     * @param value 要转换的数值
     * @return 转换后的字符串
     */
    template<typename T,typename U> inline T *htos_upper(T *str,int size,U value){return htos<T,U>(str,size,value,true);} 

    /**
     * 转换一个无符号整数到字符串(以小写16进制表示)
     * @param str 转换后的字符串存放处
     * @param size 存放处可容纳的字符数
     * @param value 要转换的数值
     * @return 转换后的字符串
     */
    template<typename T,typename U> inline T *htos_lower(T *str,int size,U value){return htos<T,U>(str,size,value,false);} 

    /**
     * 转换一个浮点数到字符串
     * @param str 字符串保存指针
     * @param size 字符串保存空间长度
     * @param fsize 小数部分保留长度
     * @param value 要转换的值
     */
    template<typename T,typename F>
    inline T *ftos(T *str,int size,int fsize,F value)
    {
        const long integer=(long)value;     //整数部分

        T *p=str;

        if(integer==0&&value<0)             //如果为-0.xx这种，integer会为0所以不增加-号
            *p++='-';

        itos(p,size,integer);

        int len=strlen(str);

        if(len>=size)
            return(str);

        p=str+len;

        value-=integer;                     //保留小数部分

        if(value<0)
            value=-value;

        const float min_value=pow(0.1,fsize);

        if(value<min_value)
        {
            *p=0;
            return(str);
        }

        *p++='.';                           //加小数点
        ++len;

        while(value>min_value&&len<size&&fsize--)
        {
            value*=10;                      //让最上一位变成个位数
            *p++='0'+int(value);

            value-=int(value);              //减去整数部分
            ++len;
        }

        *p=0;
        return(str);
    }

    template<typename T,typename F>
    inline T *ftos(T *str,int size,F value)
    {
        return ftos(str,size,4,value);
    }

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
        return getbetween(str,str_length,edge_char,edge_char,between_length);
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
        return getbetween(str,hgl::strlen(str),start_char,end_char,between_length);
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
        return getbetween(str,hgl::strlen(str),edge_char,edge_char,between_length);
    }

    /**
     * 转换一个整数到计算机表示法
     * @param str 字符串保存指针
     * @param str_max_length 字符串保存空间长度
     * @param value 要转换的值
     * @return 是否转换成功
     */
    template<typename T,typename V>
    inline const bool int_to_size(T *str,int str_max_length,V value)
    {
        const T name[]={'K','M','G','T','P','E','Z','Y','B',0};

        //ps2014,现有磁带机单机柜容量百PB，按组破EB。以每十年破一个的速度，2034年将接近使用Y。届时将按条件扩大。

        double new_value=value;
        int pos=0;

        while(new_value>=1024&&str_max_length--)
        {
            pos++;
            new_value/=1024;

            if(name[pos]==0)
                return(false);  //太大了
        }

        const float f=new_value;

        ftos(str,str_max_length,2,f);

        T *p=str+strlen(str);

        *p++=' ';
        if(pos)
            *p++=name[pos-1];

        *p++='B';
        *p=0;

        return(true);
    }

    /**
     * 查找一个字符串在一个字符串列表中的序列号
     * @param list 对应的字符串列表,以0结尾,如:const char *target_list[]={"self","line","circle","\0"};
     * @param str 要查找的字节串
     * @return 返回查找出的序号,-1表示失败
     */
    template<typename T>
    inline const int find_str_in_array(const T **list,const T *str)
    {
        if(!str||!list)return(-1);

        int index=0;

        do
        {
            if(*list[index]==0)
                return(-1);

            if(stricmp(list[index],str)==0)
                return index;

            ++index;
        }while(*list[index]);

        return(-1);
    }
    
    /**
     * 查找一个字符串在一个字符串列表中的序列号
     * @param count 字符串列表中的字符串数量
     * @param str_array 对应的字符串列表
     * @param str 要查找的字节串
     * @param str_len 要查找的字节串长度(0表示不限定长度)
     * @return 返回查找出的序号,-1表示失败
     */
    template<typename T>
    inline const int find_str_in_array(int count,const T **str_array,const T *str,int str_len=0)
    {
        if(str_len<=0)
            str_len=hgl::strlen(str);

        int len;

        for(int i=0;i<count;i++)
        {
            len=strlen(str_array[i]);

            if(len!=str_len)continue;

            if(stricmp(str_array[i],str,len)==0)return(i);
        }

        return(-1);
    }

    /**
    * 判断一个字符串是否在当前字符串列表中
    */
    template<typename T> inline bool string_in_list(const T **list,const T *str)
    {
        return find_str_in_array(list,str)!=-1;
    }

    /**
     * 从字符串中解晰数值阵列,如"1,2,3"或"1 2 3"
     */
    template<typename C,typename N> struct ParseNumberArray
    {
    public:

        virtual bool IsChar(const C ch)=0;
        virtual bool ToNumber(const C *,N &)=0;
    };

    template<typename C,typename N> struct ParseIntArray:public ParseNumberArray<C,N>
    {
        virtual bool IsChar(const C ch) override{return hgl::isinteger(ch);} 
        virtual bool ToNumber(const C *str,N &result) override{return hgl::stoi(str,result);} 
    };

    template<typename C,typename N> struct ParseUIntArray:public ParseNumberArray<C,N>
    {
        virtual bool IsChar(const C ch) override{return hgl::isdigit(ch);} 
        virtual bool ToNumber(const C *str,N &result) override{return hgl::stou(str,result);} 
    };

    template<typename C,typename N> struct ParseFloatArray:public ParseNumberArray<C,N>
    {
        virtual bool IsChar(const C ch) override{return hgl::isfloat(ch);} 
        virtual bool ToNumber(const C *str,N &result) override{return hgl::etof(str,result);} 
    };

    template<typename C,typename N> struct ParseHexArray:public ParseNumberArray<C,N>
    {
        virtual bool IsChar(const C ch) override{return hgl::isxdigit(ch);} 
        virtual bool ToNumber(const C *str,N &result) override{return hgl::xtou(str,result);} 
    };

    /**
     * 解析数值阵列字符串到数组,如"1,2,3"或"1 2 3"
     * @param str 要解析的字符串
     * @param result 结果数组
     * @param max_count 最大个数
     * @param end_char 结尾字符,默认为0
     * @param end_pointer 结束指针
     * @return 解晰出来的数据数量
     */
    template<typename C,typename N>
    inline int parse_number_array(ParseNumberArray<C,N> *pna,const C *str,N *result,int max_count,const C end_char=0,const C **end_pointer=nullptr)
    {
        if(!str||!result||max_count<=0)return(-1);

        const C *p,*sp;
        int count=0;

        sp=str;
        p=sp;

        while(*p&&*p!=end_char)
        {
            if(pna->IsChar(*p))
            {
                p++;
                continue;
            }

            if(pna->ToNumber(sp,*result))
            {
                ++count;
                --max_count;

                if(max_count<=0)
                {
                    if(end_pointer)
                        *end_pointer=p;

                    return(count);
                }

                ++result;

                ++p;
                sp=p;

                continue;
            }

            if(end_pointer)
                *end_pointer=p;

            return count;
        }

        if(p>sp)
        {
            pna->ToNumber(sp,*result);
            ++count;
        }

        if(end_pointer)
            *end_pointer=p;

        return(count);
    }

    /**
     * 解析一个由多个浮点数组成的字符串，结果扔进指定数组中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param result        存放解晰结果的数组
     * \param max_count     数组最大个数量限制
     * \param end_char      结尾字符，默认为0
     * \param end_pointer   字符串结束指针
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I> inline const int parse_float_array(const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseFloatArray<T,I> pna;return parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

    /**
     * 解析一个由多个有符号整数组成的字符串，结果扔进指定数组中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param result        存放解晰结果的数组
     * \param max_count     数组最大个数量限制
     * \param end_char      结尾字符，默认为0
     * \param end_pointer   字符串结束指针
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I> inline const int parse_int_array  (const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseIntArray<T,I>   pna;return parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

    /**
     * 解析一个由多个无符号整数组成的字符串，结果扔进指定数组中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param result        存放解晰结果的数组
     * \param max_count     数组最大个数量限制
     * \param end_char      结尾字符，默认为0
     * \param end_pointer   字符串结束指针
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I> inline const int parse_uint_array (const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseUIntArray<T,I>  pna;return parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

    /**
     * 解析一个由多个16进制整数组成的字符串，结果扔进指定数组中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param result        存放解晰结果的数组
     * \param max_count     数组最大个数量限制
     * \param end_char      结尾字符，默认为0
     * \param end_pointer   字符串结束指针
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I> inline const int parse_hex_array  (const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseHexArray<T,I>   pna;return parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

    /**
     * 解析数值阵列字符串到数组,如"1,2,3"或"1 2 3"
     * @param str 要解析的字符串
     * @param str_len 字符串的长度
     * @param result_list 结果数组
     * @return 解晰出来的数据数量
     * @return -1 出错
     */
    template<typename T,typename I,typename SET>
    inline const int parse_number_array(ParseNumberArray<T,I> *pna,const T *str,const int str_len,SET &result_list)
    {
        if(!str||str_len<=0)return(-1);

        const T *p,*sp;
        int len=str_len;
        int count=0;
        I result;

        sp=str;
        p=sp;

        while(*p&&len)
        {
            --len;
            if(pna->IsChar(*p))
            {
                p++;
                continue;
            }

            if(pna->ToNumber(sp,result))
            {
                ++count;
                result_list.Add(result);

                ++p;
                sp=p;

                continue;
            }

            return count;
        }

        if(p>sp)
        {
            pna->ToNumber(sp,result);
            result_list.Add(result);
            ++count;
        }

        return(count);
    }

    /**
     * 解析一个由多个浮点数组成的字符串，结果扔进指定列表中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param len           字符串长度
     * \param result_list   存放解晰结果的列表
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I,typename SET> inline const int parse_float_array   (const T *str,const int len,SET &result_list){ParseFloatArray<T,I> pna;return parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

    /**
     * 解析一个由多个有符号整数组成的字符串，结果扔进指定列表中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param len           字符串长度
     * \param result_list   存放解晰结果的列表
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I,typename SET> inline const int parse_int_array      (const T *str,const int len,SET &result_list){ParseIntArray<T,I>   pna;return parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

    /**
     * 解析一个由多个无符号整数组成的字符串，结果扔进指定列表中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param len           字符串长度
     * \param result_list   存放解晰结果的列表
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I,typename SET> inline const int parse_uint_array     (const T *str,const int len,SET &result_list){ParseUIntArray<T,I>  pna;return parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

    /**
     * 解析一个由多个16进制整数组成的字符串，结果扔进指定列表中。任何非当前进制的字符串都将被视为分隔符。
     * 
     * \param str           要解晰的原始字符串
     * \param len           字符串长度
     * \param result_list   存放解晰结果的列表
     * \return              解晰出来的数据数量
     */
    template<typename T,typename I,typename SET> inline const int parse_hex_array      (const T *str,const int len,SET &result_list){ParseHexArray<T,I>   pna;return parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

    /**
     * 检测字符串是否符合代码命名规则（仅可使用字母和数字、下划线，不能使用数字开头）
     */
    template<typename T> inline const bool check_codestr(const T *str)
    {
        if(!str)return(false);

        if((!isalpha(*str))&&(*str!='_'))       //不是字母或下划线
            return(false);

        ++str;

        if(!(*str))                             //不能仅一个字符
            return(false);

        while(*str)
            if(!iscodechar(*str++))
                return(false);

            return(true);
    }

    /**
     * 检测字符串是否包含不可程序使用字符
     */
    template<typename T> inline const bool check_error_char(const T *str)
    {
        if(!str)return(false);

        //const char err_chr[]=u8R"( <>,/\|?%$#@`':"*&!)";
        constexpr char err_chr[] = { ' ',
            '<',
            '>',
            ',',
            '/',
            '\\',
            '|',
            '?',
            '%',
            '$',
            '#',
            '@',
            '`',
            '\'',
            ':',
            '"',
            '*',
            '&',
            '!',
            0};
            const char *sp;

            while(*str)
            {
                if(isspace(*str))
                    return(false);

                sp=err_chr;

                while(*sp)
                {
                    if(*str==*sp)
                        return(false);

                    ++sp;
                }

                ++str;
            }

            return(true);
    }

    /**
     * 解晰一个10进制字符到数值
     * @param ch 字符
     * @param num 进制
     * @return 解析出来的值
     */
    inline const int parse_dec_number_char(const int ch)
    {
        if(ch>='0'&&ch<='9')
            return ch-'0';

        return 0;
    }

    /**
     * 解析一个多进制字符到数值
     * @param ch 字符
     * @return 解析出来的值
     */
    template<uint NUM,typename T>
    inline const int parse_number_char(const T ch)
    {
        if(ch>='0'&&ch<='9')
            return ch-'0';

        if(ch>='a'&&ch<=('z'+NUM-11))
            return 10+ch-'a';

        if(ch>='A'&&ch<=('Z'+NUM-11))
            return 10+ch-'A';

        return(0);
    }

    /**
     * 解晰一个16进制数值字符串
     * @param dst 解晰出来的原始数据存入处
     * @param src 用来解晰的16进制数值字符串
     * @param size 原始数据字节数/2
     */
    template<typename T>
    inline void ParseHexStr(uint8 *dst,const T *src,const int size)
    {
        for(int i=0;i<size;i++)
        {
            *dst =parse_number_char<16,T>(*src)<<4;
            ++src;
            *dst|=parse_number_char<16,T>(*src);
            ++src;
            ++dst;
        }
    }

    /**
     * 将一个16进制数值字符串转换成原始数据
     * @param str 16进制数值字符串
     * @param hc 解晰出来的原始数据存入处
     */
    template<typename T,typename HC>
    inline void ParseHexStr(HC &hc,const T *str)
    {
        ParseHexStr((uint8 *)&hc,str,sizeof(HC));
    }

    template<typename T,typename U>
    inline void Hex2String(T *str,U value,bool upper=true)
    {
        const uchar A=upper?'A':'a';
        const uint size=sizeof(U)*2;
        const uint8 *sp=(uint8 *)&value;
              T *tp=str;
        uint l,r;

        for(uint i=0;i<sizeof(U);i++)
        {
            l=((*sp)&0xF0)>>4;
            r= (*sp)&0x0F;

            ++sp;

            if(l<10)*tp++=l+'0';
               else *tp++=l-10+A;

            if(r<10)*tp++=r+'0';
               else *tp++=r-10+A;
        }

        *tp=0;
    }

    /**
     * 将一串原始数据转换成16进制数值字符串
     * @param str 16进制数值字符串存入处
     * @param src 原始的数据
     * @param size 原始数据字节长度
     * @param hexstr 用于转换的16进制字符
     * @param gap_char 间隔字符
     */
    template<typename T>
    inline void DataToHexStr(T *str,const uint8 *src,const int size,const char *hexstr,const T gap_char=0)
    {
        int i;

        for(i=0;i<size;i++)
        {
            if(i&&gap_char)
            {
                *str=gap_char;
                ++str;
            }

            *str=hexstr[((*src)&0xF0)>>4];
            ++str;
            *str=hexstr[ (*src)&0x0F    ];
            ++str;

            ++src;
        }

        *str=0;
    }

    template<typename T> inline void DataToLowerHexStr(T *str,const uint8 *src,const int size,const T gap_char=0){DataToHexStr<T>(str,src,size,LowerHexChar,gap_char);} 
    template<typename T> inline void DataToUpperHexStr(T *str,const uint8 *src,const int size,const T gap_char=0){DataToHexStr<T>(str,src,size,UpperHexChar,gap_char);} 

    /**
     * 将一串原始数据转转成16进制数值字符串
     * @param str 16进制数值字符串存入处
     * @param hc 原始的数据
     * @param hexstr 用于转换的16进制字符
     * @param gap_char 间隔字符
     */
    template<typename T,typename HC>
    inline void DataToHexStr(T *str,const HC &hc,const T *hexstr,const T gap_char=0)
    {
        return DataToHexStr(str,(const uint8 *)&hc,sizeof(hc),hexstr,gap_char);
    }

    template<typename T,typename HC> inline void ToUpperHexStr(T *str,const HC &hc,const T gap_char=0){DataToHexStr<T,HC>(str,hc,UpperHexChar,gap_char);} 
    template<typename T,typename HC> inline void ToLowerHexStr(T *str,const HC &hc,const T gap_char=0){DataToHexStr<T,HC>(str,hc,LowerHexChar,gap_char);} 

    template<typename T> inline void ToUpperHexStr(T *str,const void *data,const int size,const T gap_char=0){DataToHexStr<T>(str,(const uint8 *)data,size,UpperHexChar,gap_char);} 
    template<typename T> inline void ToLowerHexStr(T *str,const void *data,const int size,const T gap_char=0){DataToHexStr<T>(str,(const uint8 *)data,size,LowerHexChar,gap_char);} 
}//namespace hgl
