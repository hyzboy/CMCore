#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/type/String.h>
#include<hgl/SourceCodeLocation.h>
#include<hgl/log/LogInfo.h>
#include<cstdio>
#include<cwchar>
#include<cstdarg>

namespace hgl
{
    constexpr const size_t LOG_TMP_BUFFER_SIZE=4096;

    /**
    * Android风格的日志输出类
    */
    class LogOutput
    {
        char    tmp_fmt     [LOG_TMP_BUFFER_SIZE];
        u8char  tmp_fmt_u8  [LOG_TMP_BUFFER_SIZE];
        u16char tmp_fmt_u16 [LOG_TMP_BUFFER_SIZE];

    public:

        LogOutput()=default;
        virtual ~LogOutput()=default;

    public:

        void output(LogLevel level,const char *     str,const int len){logger::Log(level,(u8char *)str,len);}
        void output(LogLevel level,const u8char *   str,const int len){logger::Log(level,(u8char *)str,len);}
        void output(LogLevel level,const u16char *  str,const int len){logger::Log(level,(u16char *)str,len);}

    #define DEFINE_LOG_OUTPUT_CHAR(ch,level) \
        \
        void ch(const char *     str,const int len  ){output(LogLevel::level,   str,len);} \
        void ch(const char *     str                ){ch(str,strlen(str));}   \
        void ch(const AnsiString &      str         ){ch(str.c_str(),str.GetLength());}   \
        void ch(const AnsiStringView &  sv          ){ch(sv.c_str(),sv.GetLength());}

    #define DEFINE_LOG_OUTPUT_CHAR8(ch,level) \
        \
        void ch(const u8char *   str,const int len  ){output(LogLevel::level,   str,len);} \
        void ch(const u8char *   str                ){ch(str,strlen(str));}   \
        void ch(const U8String &    str             ){ch(str.c_str(),str.GetLength());}   \
        void ch(const U8StringView &sv              ){ch(sv.c_str(),sv.GetLength());}

    #define DEFINE_LOG_OUTPUT_CHAR16(ch,level) \
        \
        void ch(const u16char *  str,const int len  ){output(LogLevel::level,   str,len);} \
        void ch(const u16char *  str                ){ch(str,strlen(str));}   \
        void ch(const U16String &    str            ){ch(str.c_str(),str.GetLength());}   \
        void ch(const U16StringView &sv             ){ch(sv.c_str(),sv.GetLength());}

    #ifdef HGL_SUPPORT_CHAR8_T
        #define DEFINE_LOG_OUTPUT(ch,level) DEFINE_LOG_OUTPUT_CHAR(ch,level)    \
                                            DEFINE_LOG_OUTPUT_CHAR8(ch,level)   \
                                            DEFINE_LOG_OUTPUT_CHAR16(ch,level)
    #else
        #define DEFINE_LOG_OUTPUT(ch,level) DEFINE_LOG_OUTPUT_CHAR(ch,level)    \
                                            DEFINE_LOG_OUTPUT_CHAR16(ch,level)
    #endif//HGL_SUPPORT_CHAR8_T
        
        DEFINE_LOG_OUTPUT(v,Verbose)
        DEFINE_LOG_OUTPUT(i,Info)
        DEFINE_LOG_OUTPUT(h,Hint)
        DEFINE_LOG_OUTPUT(w,Warning)
        DEFINE_LOG_OUTPUT(e,Error)
        DEFINE_LOG_OUTPUT(fe,FatalError)
    };//class LogOutput
}//namespace hgl
