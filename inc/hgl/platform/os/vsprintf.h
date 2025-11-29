#ifndef HGL_VSPRINTF_INCLUDE
#define HGL_VSPRINTF_INCLUDE

#include<stdarg.h>
namespace hgl
{
    template<typename T> int vsprintf(T *,const size_t,const T *,va_list va);
}//namespace hgl
#endif//HGL_VSPRINTF_INCLUDE