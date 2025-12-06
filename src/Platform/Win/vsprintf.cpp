#include<hgl/platform/os/vsprintf.h>
#include<stdio.h>
namespace hgl
{
    template<> int vsprintf<char>(char *buffer,const size_t buf_max,const char *format,va_list va)
    {
        return vsprintf_s(buffer,buf_max,format,va);
    }

    template<> int vsprintf<wchar_t>(wchar_t *buffer,const size_t  buf_max,const wchar_t *format,va_list va)
    {
        return vswprintf_s(buffer,buf_max,format,va);
    }
}//namespace hgl
