#pragma once

#include<string>
#include<hgl/type/String.h>
#include<hgl/Charset.h>

namespace hgl
{
    #if HGL_OS == HGL_OS_Windows
    inline hgl::OSString ToOSString(const std::string &str)
    {
        return hgl::to_u16((hgl::u8char *)str.c_str(),(int)(str.length()));
    }

    inline hgl::OSString ToOSString(const std::wstring &str)
    {
        return hgl::OSString(str.c_str(),(int)str.length());
    }

    inline hgl::U8String ToU8String(const std::wstring &str)
    {
        return hgl::to_u8(str.c_str(),(int)str.length());
    }

    inline std::string ToStdString(const hgl::OSString &str)
    {
        hgl::U8String u8_str=hgl::to_u8(str);

        return std::string((char *)u8_str.c_str(),str.Length());
    }

    inline std::wstring ToStdWString(const hgl::OSString &str)
    {
        return std::wstring(str.c_str(),str.Length());
    }
    #else
    inline hgl::OSString ToOSString(const std::string &str)
    {
        return hgl::OSString(str.c_str(),str.size());
    }

    inline hgl::OSString ToOSString(const std::wstring &str)
    {
        return hgl::to_u8(str.c_str(),str.size());
    }

    inline std::string ToStdString(const hgl::OSString &str)
    {
        return std::string(str.c_str(),str.Length());
    }

    inline std::wstring ToStdWString(const OSString &str)
    {
        hgl::WideString w_str=hgl::to_wide<sizeof(wchar_t)>(str);

        return std::wstring(w_str.c_str(),w_str.Length());
    }
    #endif//

    inline hgl::U8String ToU8String(const std::string &str)
    {
        return hgl::U8String((hgl::u8char *)str.c_str(),int(str.size()));
    }
}