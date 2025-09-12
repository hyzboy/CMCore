#pragma once
#include <hgl/type/Str.Number.h>
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Search.h>
#include <hgl/type/Str.Copy.h>
namespace hgl
{

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
    template<typename T,typename I> inline const int parse_float_array(const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseFloatArray<T,I> pna;return hgl::parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

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
    template<typename T,typename I> inline const int parse_int_array  (const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseIntArray<T,I>   pna;return hgl::parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

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
    template<typename T,typename I> inline const int parse_uint_array (const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseUIntArray<T,I>  pna;return hgl::parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

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
    template<typename T,typename I> inline const int parse_hex_array  (const T *str,I *result,int max_count,const T end_char=0,const T **end_pointer=0){ParseHexArray<T,I>   pna;return hgl::parse_number_array<T,I>(&pna,str,result,max_count,end_char,end_pointer);} 

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
    template<typename T,typename I,typename SET> inline const int parse_float_array   (const T *str,const int len,SET &result_list){ParseFloatArray<T,I> pna;return hgl::parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

    /**
    * 解析一个由多个有符号整数组成的字符串，结果扔进指定列表中。任何非当前进制的字符串都将被视为分隔符。
    * 
    * \param str           要解晰的原始字符串
    * \param len           字符串长度
    * @param result_list   存放解晰结果的列表
    * \return              解晰出来的数据数量
    */
    template<typename T,typename I,typename SET> inline const int parse_int_array      (const T *str,const int len,SET &result_list){ParseIntArray<T,I>   pna;return hgl::parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

    /**
    * 解析一个由多个无符号整数组成的字符串，结果扔进指定列表中。任何非当前进制的字符串都将被视为分隔符。
    * 
    * \param str           要解晰的原始字符串
    * \param len           字符串长度
    * \param result_list   存放解晰结果的列表
    * \return              解晰出来的数据数量
    */
    template<typename T,typename I,typename SET> inline const int parse_uint_array     (const T *str,const int len,SET &result_list){ParseUIntArray<T,I>  pna;return hgl::parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

    /**
    * 解析一个由多个16进制整数组成的字符串，结果扔进指定列表中。任何非当前进制的字符串都将被视为分隔符。
    * 
    * \param str           要解晰的原始字符串
    * \param len           字符串长度
    * \param result_list   存放解晰结果的列表
    * \return              解晰出来的数据数量
    */
    template<typename T,typename I,typename SET> inline const int parse_hex_array      (const T *str,const int len,SET &result_list){ParseHexArray<T,I>   pna;return hgl::parse_number_array<T,I,SET>(&pna,str,len,result_list);} 

}//namespace hgl
