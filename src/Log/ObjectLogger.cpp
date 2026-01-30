#include<hgl/log/Log.h>
#include<hgl/log/LogMessage.h>
#include<hgl/Charset.h>
#include<hgl/type/StdString.h>
#include<cstdarg>

namespace hgl::logger
{
    void LogOutput(const LogMessage &msg);

    namespace
    {
        constexpr const std::string_view LogLevelName[]=
        {
            "Verbose",
            "Debug",
            "Info",
            "Notice",
            "Warning",
            "Error",
            "Fatal"
        };

        constexpr const std::u16string_view LogLevelNameU16[]=
        {
            u"Verbose",
            u"Debug",
            u"Info",
            u"Notice",
            u"Warning",
            u"Error",
            u"Fatal"
        };
    }//namespace

    ObjectLogger GlobalLogger;///<全局日志对象
}//namespace hgl::logger

namespace hgl::logger
{
    ObjectLogger::ObjectLogger()
    {
        object_type_info=nullptr;
        object_type_name=OSString(OS_TEXT("Global"));
        object_instance_name.Clear();
    }

    ObjectLogger::ObjectLogger(const std::type_info *info):object_type_info(info)
    {
        object_type_name=info?ToOSString(info->name()):OSString(OS_TEXT("?"));

        object_instance_name.Clear();
    }

    ObjectLogger::ObjectLogger(const os_char *name)
    {
        object_type_info=nullptr;

        object_instance_name=name;
    }

    void ObjectLogger::LogString(const std::source_location &sl,const LogLevel level,const os_char *str,const int size)
    {
        LogMessage msg;

        msg.object_type_info    =object_type_info;
        msg.object_instance_name=object_instance_name;
        msg.source_location     =sl;
        msg.level               =level;
        msg.message             =str;
        msg.message_length      =size;

        LogOutput(msg);
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
            log_buffer_u8.resize(len+1);

            std::vsnprintf((char *)log_buffer_u8.data(),len+1,(const char *)fmt,args);

        #if HGL_OS == HGL_OS_Windows

            int u16len=get_utf16_length(UTF8CharSet,(const char *)log_buffer_u8.data(),len)+1;

            log_buffer_u16.resize(u16len);

            u16len=u8_to_u16(log_buffer_u16.data(),u16len,log_buffer_u8.data(),len);

            LogString(sl,level,log_buffer_u16.data(),u16len);
        #else
            LogString(sl,level,log_buffer_u8.data(),len);
        #endif//HGL_OS == HGL_OS_Windows
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
            log_buffer_u16.resize(len+1);
            std::vswprintf(log_buffer_u16.data(),len+1,fmt,args);

        #if HGL_OS == HGL_OS_Windows
            LogString(sl,level,log_buffer_u16.data(),len);
        #else

            int u8len=get_utf8_length(UTF16LECharSet,log_buffer_u16.data(),len)+1;

            log_buffer_u8.resize(u8len);

            u8len=u16_to_u8(log_buffer_u8.data(),u8len,log_buffer_u16.data(),len);

            LogString(sl,level,log_buffer_u8.data(),u8len);
        #endif
        }
    }
}//namespace hgl::logger
