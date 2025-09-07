#pragma once

#include<hgl/type/String.h>

namespace hgl
{
    int             u16_to_u8(u8char *,int,const u16char *,const int=-1);                           ///<转换u16char *到utf8格式的u8char *
    int             u8_to_u16(u16char *,int,const u8char *,const int=-1);                           ///<转换utf8格式的u8char *到u16char *

    u8char *        u16_to_u8(const u16char *,const int,int &);                                     ///<转换u16char *到utf8格式的u8char *
    u16char *       u8_to_u16(const u8char *,const int,int &);                                      ///<转换utf8格式的u8char *到u16char *

    inline u8char * u16_to_u8(const u16char *str)
    {
        int len;
        return u16_to_u8(str,hgl::strlen<u16char>(str)+1,len);
    }

    inline u16char *u8_to_u16(const u8char *str)
    {
        int len;
        return u8_to_u16(str,hgl::strlen<u8char>(str)+1,len);
    }

    inline U16String to_u16(const u8char *u8_str,int length)
    {
        int wlen;
        u16char *ws=u8_to_u16(u8_str,length,wlen);

        return U16String::newOf(ws,wlen);
    }

    inline U16String to_u16(const U8String &u8str)
    {
        return to_u16(u8str.c_str(),u8str.Length());
    }

    inline U16String to_u16(const u8char *str)
    {
        int wlen;

        u16char *ws=u8_to_u16(str,hgl::strlen(str),wlen);

        return U16String::newOf(ws,wlen);
    }

    inline U8String to_u8(const u16char *wide_str,int length)
    {
        int ulen;

        u8char *us=u16_to_u8(wide_str,length,ulen);

        return U8String::newOf(us,ulen);
    }

    inline U8String to_u8(const U16String &ws)
    {
        return to_u8(ws.c_str(),ws.Length());
    }

#if HGL_OS == HGL_OS_Windows
    inline OSString ToOSString(const char *str){return to_u16((u8char *)str);}
    inline OSString ToOSString(const u8char *str){return to_u16(str);}
    inline OSString ToOSString(const U8String &str){return to_u16(str.c_str(), (int)(str.Length()));}

#define ToU16String ToOSString

    inline U8String ToU8String(const os_char *str){return to_u8(str,strlen(str));}
    inline U8String ToU8String(const OSString &str){return to_u8(str);}
#else
    inline OSString ToOSString(const char *str){return OSString(str);}
    inline OSString ToOSString(const U8String &str){return str;}

    inline U8String ToU8String(const os_char *str){return U8String(str);}
    inline U8String ToU8String(const OSString &str){return str;}
#endif//

    namespace endian
    {
        struct BOMFileHeader;
    }

    const endian::BOMFileHeader *ParseBOM(const void *input);
}//namespace hgl
