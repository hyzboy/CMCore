#pragma once

/**
 * CN:  在这里我们自行实现了C标准库的一些字符和字符串处理函数，
 *      这个行为并不是为了追求性能，或是在炫技。而是因为不同编译器所附带的标准库在一些边界、非法值上处理不一致。
 * 
 *      比如你在一些字符串处理函数中传入非法长度或是nullptr，不同实现的标准库都有正确的错误处理，但错误的处理方式确不一定完全一样。
 * 
 * EN:  Here we have implemented some character and string handling functions from the C standard library ourselves.
 *      This is not done for performance or to show off skills,
 *      but because the standard libraries provided by different compilers handle certain edge cases and illegal values inconsistently.
 * 
 *      For example,
 *          if you pass an illegal length or nullptr to some string handling functions,
 *          different implementations of the standard library may have correct error handling,
 *          but the way they handle errors is not necessarily the same.
 */

#include <hgl/type/CharType.h>
#include <type_traits>
#include <cmath>

namespace hgl
{
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

        int len = hgl::strlen(str);
        if(len<=0) return 0;

        T *p=str+len-1;

        if(*p=='\n')return(hgl::stat_char(str,'\n'));
        else return(hgl::stat_char(str,'\n')+1);
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

            if(hgl::stricmp(list[index],str)==0)
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
     * @param str_len要查找的字节串长度(0表示不限定长度)
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
            len=hgl::strlen(str_array[i]);

            if(len!=str_len)continue;

            if(hgl::stricmp(str_array[i],str,len)==0)return(i);
        }

        return(-1);
    }

    /**
    * 判断一个字符串是否在当前字符串列表中
    */
    template<typename T> inline bool string_in_list(const T **list,const T *str)
    {
        return hgl::find_str_in_array(list,str)!=-1;
    }

    /**
     * 检测字符串是否符合代码命名规则（仅可使用字母和数字、下划线，不能使用数字开头）
     */
    template<typename T> inline const bool check_codestr(const T *str)
    {
        if(!str)return(false);

        if((!hgl::isalpha(*str))&&(*str!='_'))       //不是字母或下划线
            return(false);

        ++str;

        if(!(*str))                             //不能仅一个字符
            return(false);

        while(*str)
            if(!hgl::iscodechar(*str++))
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
                if(hgl::isspace(*str))
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
} // namespace hgl
