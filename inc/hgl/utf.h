#pragma once

#include<hgl/type/String.h>

namespace hgl
{
    int get_u16_to_u8_length(const u16char *src,int src_size);
    int get_u8_to_u16_length(const u8char *src,int src_size);

    int             u16_to_u8(u8char *,int,const u16char *,const int=-1);                           ///<转换u16char *到utf8格式的u8char *
    int             u8_to_u16(u16char *,int,const u8char *,const int=-1);                           ///<转换utf8格式的u8char *到u16char *

    u8char *        u16_to_u8(const u16char *,const int,int &);                                     ///<转换u16char *到utf8格式的u8char *
    u16char *       u8_to_u16(const u8char *,const int,int &);                                      ///<转换utf8格式的u8char *到u16char *

    inline u8char * u16_to_u8(const u16char *str)
    {
        int len;
        return u16_to_u8(str,::hgl::strlen<u16char>(str)+1,len);
    }

    inline u16char *u8_to_u16(const u8char *str)
    {
        int len;
        return u8_to_u16(str,::hgl::strlen<u8char>(str)+1,len);
    }

    // 将 UTF-8 缓冲转换成 U16String (length 为源UTF-8字符/字节数，不含终止符)
    inline U16String to_u16(const u8char *u8_str,int length)
    {
        if(!u8_str||length<=0)
            return U16String();

        int dst_len=get_u8_to_u16_length(u8_str,length);
        if(dst_len<=0)
            return U16String();

        U16String out;
        u16char *buf=out.Resize(dst_len);      // 分配目标长度
        if(!buf)
            return U16String();

        u8_to_u16(buf,dst_len,u8_str,length);  // 填充
        return out;
    }

    inline U16String to_u16(const U8String &u8str)
    {
        return to_u16(u8str.c_str(),u8str.Length());
    }

    inline U16String to_u16(const u8char *str)
    {
        if(!str)
            return U16String();
        int src_len=::hgl::strlen(str);
        return to_u16(str,src_len);
    }

    // 将 UTF-16 缓冲转换成 U8String (length 为源UTF-16代码单元数，不含终止符)
    inline U8String to_u8(const u16char *wide_str,int length)
    {
        if(!wide_str||length<=0)
            return U8String();

        int dst_len=get_u16_to_u8_length(wide_str,length);
        if(dst_len<=0)
            return U8String();

        U8String out;
        u8char *buf=out.Resize(dst_len);
        if(!buf)
            return U8String();

        u16_to_u8(buf,dst_len,wide_str,length);
        return out;
    }

    inline U8String to_u8(const U16String &ws)
    {
        return to_u8(ws.c_str(),ws.Length());
    }

#if HGL_OS == HGL_OS_Windows
    inline OSString ToOSString(const char *str){return to_u16((u8char *)str);}      // char 视为 utf8
    inline OSString ToOSString(const u8char *str){return to_u16(str);}              // utf8 -> u16
    inline OSString ToOSString(const U8String &str){return to_u16(str.c_str(), (int)(str.Length()));}

#define ToU16String ToOSString

    inline U8String ToU8String(const os_char *str){return to_u8(str,strlen(str));}
    inline U8String ToU8String(const OSString &str){return to_u8(str);}             // u16 -> utf8
#else
    inline OSString ToOSString(const char *str){return OSString(str);}              // 非Windows直接用UTF-8
    inline OSString ToOSString(const U8String &str){return str;}

    inline U8String ToU8String(const os_char *str){return U8String(str);}           // 已经是utf8
    inline U8String ToU8String(const OSString &str){return str;}
#endif//

    namespace endian
    {
        struct BOMFileHeader;
    }

    const endian::BOMFileHeader *ParseBOM(const void *input);
}//namespace hgl
