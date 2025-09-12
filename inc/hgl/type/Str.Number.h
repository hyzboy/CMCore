#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <cstdint>
#include <cstring>
namespace hgl
{

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

        while(*str&&hgl::is_hex_digit(*str))
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

        if(!hgl::stof(str,temp))
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

            if(hgl::stof(str+1,power))
            {
                result=R(temp*std::pow(10,power));

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

        using UnsignedI = typename std::make_unsigned<I>::type;
        UnsignedI uvalue;

        if(value<0)
        {
            *p++='-';
            --size;

            // Safely convert negative value to unsigned to avoid UB when value is the minimum representable
            uvalue = static_cast<UnsignedI>(-(value + 1)) + 1;
        }
        else
        {
            uvalue = static_cast<UnsignedI>(value);
        }

        bp=buf;

        while(true)
        {
            *bp++ = static_cast<T>(static_cast<int>(uvalue % 10) + '0');

            if ((uvalue = uvalue / 10) == 0)
                break;
        }

        while(bp--!=buf && size--)
            *p++=*bp;

        if(size)
            *p=0;

        return (p - str);
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
        hgl::itos_rl(str,size,num);
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

    template<> struct htos_bits<1>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return hgl::utos(str,size,*(uint8  *)&value,16,upper);}};
    template<> struct htos_bits<2>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return hgl::utos(str,size,*(uint16 *)&value,16,upper);}};
    template<> struct htos_bits<4>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return hgl::utos(str,size,*(uint32 *)&value,16,upper);}};
    template<> struct htos_bits<8>{template<typename T,typename U> static inline T *conv(T *str,int size,U value,bool upper){return hgl::utos(str,size,*(uint64 *)&value,16,upper);}};

    /**
    * 转换一个无符号整数到字符串(以16进制表示)
    * @param str 转换后的字符串存放处
    * @param size 存放处可容纳的字符数
    * @param value 要转换的值
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
    template<typename T,typename U> inline T *htos_upper(T *str,int size,U value){return hgl::htos<T,U>(str,size,value,true);} 

    /**
    * 转换一个无符号整数到字符串(以小写16进制表示)
    * @param str 转换后的字符串存放处
    * @param size 存放处可容纳的字符数
    * @param value 要转换的数值
    * @return 转换后的字符串
    */
    template<typename T,typename U> inline T *htos_lower(T *str,int size,U value){return hgl::htos<T,U>(str,size,value,false);} 

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

        hgl::itos(p,size,integer);

        int len=hgl::strlen(str);

        if(len>=size)
            return(str);

        p=str+len;

        value-=integer;                     //保留小数部分

        if(value<0)
            value=-value;

        const float min_value=std::pow(0.1,fsize);

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
        return hgl::ftos(str,size,4,value);
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

        hgl::ftos(str,str_max_length,2,f);

        T *p=str+hgl::strlen(str);

        *p++=' ';
        if(pos)
            *p++=name[pos-1];

        *p++='B';
        *p=0;

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

        if(NUM>10)
        {
            // Correct upper bounds: 'a' + (NUM-11) and 'A' + (NUM-11)
            T lower_max = static_cast<T>(static_cast<int>('a') + int(NUM - 11));
            T upper_max = static_cast<T>(static_cast<int>('A') + int(NUM - 11));

            if(ch>='a' && ch<=lower_max)
                return 10 + (ch - 'a');

            if(ch>='A' && ch<=upper_max)
                return 10 + (ch - 'A');
        }

        return 0;
    }
}//namespace hgl
