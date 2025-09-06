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
    }//namespace logger

    using namespace logger;
}//namespace hgl
