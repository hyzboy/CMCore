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
    }//namespace logger

    using namespace logger;
}//namespace hgl
