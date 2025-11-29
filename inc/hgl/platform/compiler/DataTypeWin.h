#pragma once

namespace hgl
{
    using  int8     =  signed __int8 ;  ///<有符号 8位整型
    using uint8     =unsigned __int8 ;  ///<无符号 8位整型
    using  int16    =  signed __int16;  ///<有符号16位整型
    using uint16    =unsigned __int16;  ///<无符号16位整型
    using  int32    =  signed __int32;  ///<有符号32位整型
    using uint32    =unsigned __int32;  ///<无符号32位整型
    using  int64    =  signed __int64;  ///<有符号64位整型
    using uint64    =unsigned __int64;  ///<无符号64位整型
    
    using half_float=uint16;
    using float32   =float;
    using float64   =double;
    
    #if defined(__cpp_char8_t)||defined(__cpp_lib_char8_t)
        #define HGL_SUPPORT_CHAR8_T
        using u8char    =char8_t;
    #else
        using u8char    =char;
    #endif
}//namespace hgl
