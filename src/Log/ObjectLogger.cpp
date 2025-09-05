#include<hgl/log/ObjectLogger.h>
#include<cstdarg>

#include<hgl/log/LogInfo.h>

namespace hgl::logger
{
    static ObjectLogger GlobalLogger(nullptr);///<全局日志对象

    void ObjectLogger::LogString(const std::source_location &sl,const LogLevel level,const char *str,const size_t size)
    {
        Log(level,(u8char *)str,size);     //临时版本
    }

    void ObjectLogger::LogString(const std::source_location &sl,const LogLevel level,const u16char *str,const size_t size)
    {
        Log(level,str,size);     //临时版本
    }

    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const char *fmt,...)
    {
        if(!fmt||!*fmt)return;
        va_list args;
        va_start(args,fmt);
        LogPrintfV(sl,level,fmt,args);
        va_end(args);
    }

    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const u16char *fmt,...)
    {
        if(!fmt||!*fmt)return;
        va_list args;
        va_start(args,fmt);
        LogPrintfV(sl,level,fmt,args);
        va_end(args);
    }

    void ObjectLogger::LogPrintfV(const std::source_location &sl,const LogLevel level,const char *fmt, va_list args)
    {
        if(!fmt||!*fmt)return;
        va_list args_copy;
        va_copy(args_copy,args);
        const int len=std::vsnprintf(nullptr,0,fmt,args_copy);
        va_end(args_copy);

        if(len>0)
        {
            std::string buf(len+1,'\0');
            std::vsnprintf(buf.data(),buf.size(),fmt,args);

            LogString(sl,level,buf.data(),buf.size());
        }
    }

    void ObjectLogger::LogPrintfV(const std::source_location &sl,const LogLevel level,const u16char *fmt, va_list args)
    {
        if(!fmt||!*fmt)return;
        va_list args_copy;
        va_copy(args_copy,args);
        const int len=std::vswprintf(nullptr,0,fmt,args_copy);
        va_end(args_copy);

        if(len>0)
        {
            std::wstring buf(len+1,u'\0');
            std::vswprintf(buf.data(),buf.size(),fmt,args);
            LogString(sl,level,buf.data(),buf.size());
        }
    }
}//namespace hgl::logger
