#ifndef HGL_LOGINFO_INCLUDE
#define HGL_LOGINFO_INCLUDE

#include<iostream>
#include<hgl/CodePage.h>
#include<hgl/SourceCodeLocation.h>

namespace hgl
{
    namespace logger
    {
        enum class LogLevel
        {
            Verbose,
            Info,
            Hint,
            Warning,
            Error,
            FatalError,
        };//enum class LogLevel
        
        bool InitLogger(const OSString &app_name);
        
        void Log(LogLevel level,const u16char *str,int size);
        void Log(LogLevel level,const u8char *str,int size);

        inline  void Log(LogLevel ll,const U16String &str)
        {
            Log(ll,str.c_str(),str.Length());
        }

        inline  void Log(LogLevel ll,const U8String &str)
        {
            Log(ll,str.c_str(),str.Length());
        }

        #ifdef HGL_SUPPORT_CHAR8_T
        inline  void Log(LogLevel ll,const AnsiString &str)
        {
            std::cout<<str.c_str()<<std::endl;
        }
        #endif//HGL_SUPPORT_CHAR8_T

        inline  void DebugLog(LogLevel ll,const U16String &str,const char *filename,int line,const char *funcname)
        {
            Log(ll,str+U16_TEXT(">>LogFrom(\"")+to_u16((u8char *)filename)+U16_TEXT("\", ")+U16String::numberOf(line)+U16_TEXT(" line,func:\"")+to_u16((u8char *)funcname)+U16_TEXT("\")"));
        }

        inline  void DebugLog(LogLevel ll,const U8String &str,const char *filename,int line,const char *funcname)
        {
            Log(ll,str+U8_TEXT(">>LogFrom(\"")+U8String((u8char *)filename)+U8_TEXT("\", ")+U8String::numberOf(line)+U8_TEXT(" line,func:\"")+U8String((u8char *)funcname)+U8_TEXT("\")"));
        }

        #define LOG_INFO(str)       {Log(LogLevel::Info,    str);}
        #define LOG_HINT(str)       {Log(LogLevel::Hint,    str);}
        #define LOG_PROBLEM(str)    {Log(LogLevel::Warning, str);}
        #define LOG_ERROR(str)      {Log(LogLevel::Error,   str);}

        #define RETURN_FALSE        {DebugLog(LogLevel::Verbose,OS_TEXT("return(false)"),                                   __FILE__,__LINE__,__HGL_FUNC__);return(false);}
        #define RETURN_ERROR(v)     {DebugLog(LogLevel::Info,   OS_TEXT("return error(")+OSString::numberOf(v)+OS_TEXT(")"),__FILE__,__LINE__,__HGL_FUNC__);return(v);}
        #define RETURN_ERROR_NULL   {DebugLog(LogLevel::Info,   OS_TEXT("return error(nullptr)"),                           __FILE__,__LINE__,__HGL_FUNC__);return(nullptr);}

        #define RETURN_BOOL(proc)   {if(proc)return(true);RETURN_FALSE}

        #define IF_FALSE_RETURN(str)    if(!str)RETURN_FALSE;
    }//namespace logger

    using namespace logger;
}//namespace hgl
#endif//HGL_LOGINFO_INCLUDE
