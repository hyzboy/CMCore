#include<hgl/log/ObjectLogger.h>
#include<cstdarg>

#include<hgl/log/LogInfo.h>

namespace hgl::logger
{
    namespace
    {
        constexpr const std::string_view LogLevelName[]=
        {
            "Verbose",
            "Info",
            "Hint",
            "Warning",
            "Error",
            "Fatal"
        };

        constexpr const std::u16string_view LogLevelNameU16[]=
        {
            u"Verbose",
            u"Info",
            u"Hint",
            u"Warning",
            u"Error",
            u"Fatal"
        };
    }//namespace

    static ObjectLogger GlobalLogger(nullptr);///<全局日志对象

    void ObjectLogger::LogString(const std::source_location &sl,const LogLevel level,const u8char *str,const size_t size)
    {
        Log(level,(u8char *)str,size);     //临时版本
    }

    void ObjectLogger::LogString(const std::source_location &sl,const LogLevel level,const u16char *str,const size_t size)
    {
        Log(level,str,size);     //临时版本
    }

    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const u8char *fmt, va_list args)
    {
        if(!fmt||!*fmt)return;
        va_list args_copy;
        va_copy(args_copy,args);
        const int len=std::vsnprintf(nullptr,0,(char *)fmt,args_copy);
        va_end(args_copy);

        if(len>0)
        {
            std::string front=std::string(LogLevelName[size_t(level)].data())
                             +std::string("[")+std::string(object_type_info->name())+std::string("]");

            std::string buf(len+1,'\0');
            std::vsnprintf(buf.data(),buf.size(),(const char *)fmt,args);

            LogString(sl,level,front.data(),front.size());
            LogString(sl,level,buf.data(),buf.size());
        }
    }

#ifdef HGL_SUPPORT_CHAR8_T
    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const char *fmt, va_list args)
    {
        LogPrintf(sl,level,(const u8char *)fmt,args);
    }
#endif//HGL_SUPPORT_CHAR8_T

    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const u16char *fmt, va_list args)
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

            std::string front=std::string(LogLevelName[size_t(level)].data())
                             +std::string("[")+std::string(object_type_info->name())+std::string("]");

            LogString(sl,level,front.data(),front.size());
            LogString(sl,level,buf.data(),buf.size());
        }
    }
}//namespace hgl::logger
