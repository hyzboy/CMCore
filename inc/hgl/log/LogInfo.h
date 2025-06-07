#pragma once

#include<iostream>
#include<source_location>
#include<hgl/CodePage.h>
#include<hgl/log/LogLevel.h>

namespace hgl
{
    namespace logger
    {        
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

        inline  void DebugLog(LogLevel ll,const U16String &str,const std::source_location scl)
        {
            Log(ll,str+U16_TEXT(">>LogFrom(\"")+
                to_u16((u8char *)scl.file_name())+U16_TEXT("\", (")+
                U16String::numberOf(scl.line())+U16_TEXT(",")+
                U16String::numberOf(scl.column())+U16_TEXT(") ,func:\"")+
                to_u16((u8char *)scl.function_name())+U16_TEXT("\")"));
        }

        inline  void DebugLog(LogLevel ll,const U8String &str,const std::source_location scl)
        {
            Log(ll,str+U8_TEXT(">>LogFrom(\"")+
                U8String((u8char *)scl.file_name())+U8_TEXT("\",(")+
                U8String::numberOf(scl.line())+U8_TEXT(",")+
                U8String::numberOf(scl.column())+U8_TEXT(") ,func:\"")+
                U8String((u8char *)scl.function_name())+U8_TEXT("\")"));
        }

        #define LOG_INFO(str)       {Log(LogLevel::Info,    str);}
        #define LOG_HINT(str)       {Log(LogLevel::Hint,    str);}
        #define LOG_PROBLEM(str)    {Log(LogLevel::Warning, str);}
        #define LOG_ERROR(str)      {Log(LogLevel::Error,   str);}

        #define RETURN_FALSE        {DebugLog(LogLevel::Verbose,OS_TEXT("return(false)"),                                   std::source_location::current());return(false);}
        #define RETURN_ERROR(v)     {DebugLog(LogLevel::Info,   OS_TEXT("return error(")+OSString::numberOf(v)+OS_TEXT(")"),std::source_location::current());return(v);}
        #define RETURN_ERROR_NULL   {DebugLog(LogLevel::Info,   OS_TEXT("return error(nullptr)"),                           std::source_location::current());return(nullptr);}

        #define RETURN_BOOL(proc)   {if(proc)return(true);RETURN_FALSE}

        #define IF_FALSE_RETURN(str)    if(!str)RETURN_FALSE;
    }//namespace logger

    using namespace logger;
}//namespace hgl
