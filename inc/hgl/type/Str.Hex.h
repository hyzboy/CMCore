#pragma once
#include <hgl/type/Str.Number.h>
#include <hgl/type/Str.Length.h>
namespace hgl
{

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
            *dst = hgl::parse_number_char<16,T>(*src)<<4;
            ++src;
            *dst |= hgl::parse_number_char<16,T>(*src);
            ++src;
            ++dst;
        }
    }

    /**
    * 将一个16进制数值字符串转换成原始数据
    * @param str 16进制数值字符串
    * @param hc 解晰出来的原始数据存放处
    */
    template<typename T,typename HC>
    inline void ParseHexStr(HC &hc,const T *str)
    {
        hgl::ParseHexStr((uint8 *)&hc,str,sizeof(HC));
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

    template<typename T> inline void DataToLowerHexStr(T *str,const uint8 *src,const int size,const T gap_char=0){hgl::DataToHexStr<T>(str,src,size,LowerHexChar,gap_char);} 
    template<typename T> inline void DataToUpperHexStr(T *str,const uint8 *src,const int size,const T gap_char=0){hgl::DataToHexStr<T>(str,src,size,UpperHexChar,gap_char);} 

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
        return hgl::DataToHexStr(str,(const uint8 *)&hc,sizeof(hc),hexstr,gap_char);
    }

    template<typename T,typename HC> inline void ToUpperHexStr(T *str,const HC &hc,const T gap_char=0){hgl::DataToHexStr<T,HC>(str,hc,UpperHexChar,gap_char);} 
    template<typename T,typename HC> inline void ToLowerHexStr(T *str,const HC &hc,const T gap_char=0){hgl::DataToHexStr<T,HC>(str,hc,LowerHexChar,gap_char);} 

    template<typename T> inline void ToUpperHexStr(T *str,const void *data,const int size,const T gap_char=0){hgl::DataToHexStr<T>(str,(const uint8 *)data,size,UpperHexChar,gap_char);} 
    template<typename T> inline void ToLowerHexStr(T *str,const void *data,const int size,const T gap_char=0){hgl::DataToHexStr<T>(str,(const uint8 *)data,size,LowerHexChar,gap_char);} 

}//namespace hgl
