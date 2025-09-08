#pragma once

#include<hgl/type/String.h>
#include<hgl/type/DataArray.h>
#include<hgl/log/LogLevel.h>
#include<hgl/Charset.h>
#include<typeinfo>
#include<string>
#include<source_location>
#include<cstdarg>

namespace hgl::logger
{
    /**
    * 对像日志输出类
    */
    class ObjectLogger
    {
        const std::type_info *object_type_info;     ///<对象数据类型的type_info指针

        OSString object_type_name;
        OSString object_instance_name;

        DataArray<u8char> log_buffer_u8;
        DataArray<u16char> log_buffer_u16;

    public:

        ObjectLogger();
        ObjectLogger(const std::type_info *);
        ObjectLogger(const os_char *name);

        virtual ~ObjectLogger()=default;

        const OSString &GetLoggerTypeName()const
        {
            return object_type_name;
        }

        const size_t GetLoggerTypeHash()const
        {
            return object_type_info->hash_code();
        }

        void SetLoggerInstanceName(const OSString &name)
        {
            object_instance_name=name;
        }

        const OSString &GetLoggerInstanceName()const
        {
            return object_instance_name;
        }

public:

        void LogString(const std::source_location &,const LogLevel,const os_char *,const int);

        // va_list based helpers used by variadic wrappers
        void LogPrintf(const std::source_location &,const LogLevel,const u8char *,va_list);
        void LogPrintf(const std::source_location &,const LogLevel,const u16char *,va_list);
    
    #ifdef HGL_SUPPORT_CHAR8_T
        void LogString(const std::source_location &,const LogLevel,const char *,const size_t);
        void LogPrintf(const std::source_location &,const LogLevel,const char *,va_list);
    #endif//HGL_SUPPORT_CHAR8_T

    #define LOG_HELPER_DECL(LEVEL_ENUM,CHAR_TYPE) \
        void LEVEL_ENUM(const std::source_location &sl,const CHAR_TYPE *fmt,...) \
        {   \
            if(!fmt||!*fmt) return; \
        \
            va_list args;   \
        \
            va_start(args,fmt); \
            LogPrintf(sl,LogLevel::LEVEL_ENUM,fmt,args);  \
            va_end(args);   \
        }

    #define LOG_HELPER_DECL_CHAR(CHAR_TYPE)   \
        LOG_HELPER_DECL(Verbose,CHAR_TYPE)    \
        LOG_HELPER_DECL(Info,CHAR_TYPE)       \
        LOG_HELPER_DECL(Hint,CHAR_TYPE)       \
        LOG_HELPER_DECL(Warning,CHAR_TYPE)    \
        LOG_HELPER_DECL(Error,CHAR_TYPE)      \
        LOG_HELPER_DECL(Fatal,CHAR_TYPE)

        LOG_HELPER_DECL_CHAR(u8char)
        LOG_HELPER_DECL_CHAR(u16char)

        #ifdef HGL_SUPPORT_CHAR8_T
            LOG_HELPER_DECL_CHAR(char)
        #endif//HGL_SUPPORT_CHAR8_T

    #undef LOG_HELPER_DECL_CHAR
    #undef LOG_HELPER_DECL
    };//ObjectLogger

    extern ObjectLogger GlobalLogger;///<全局日志对象

#define OBJECT_LOGGER   hgl::logger::ObjectLogger Log{&typeid(*this)};

#ifdef _DEBUG
    #define LogVerbose(...) this->Log.Verbose   (std::source_location::current(),__VA_ARGS__);
    #define LogInfo(...)    this->Log.Info      (std::source_location::current(),__VA_ARGS__);
    #define LogHint(...)    this->Log.Hint      (std::source_location::current(),__VA_ARGS__);
#else
    #define LogVerbose(...)
    #define LogInfo(...)
    #define LogHint(...)
#endif//

    #define LogWarning(...) this->Log.Warning   (std::source_location::current(),__VA_ARGS__);
    #define LogError(...)   this->Log.Error     (std::source_location::current(),__VA_ARGS__);
    #define LogFatal(...)   this->Log.Fatal     (std::source_location::current(),__VA_ARGS__);

    //全局日志对象的快捷方式

#ifdef _DEBUG
    #define GLogVerbose(...) hgl::logger::GlobalLogger.Verbose   (std::source_location::current(),__VA_ARGS__);
    #define GLogInfo(...)    hgl::logger::GlobalLogger.Info      (std::source_location::current(),__VA_ARGS__);
    #define GLogHint(...)    hgl::logger::GlobalLogger.Hint      (std::source_location::current(),__VA_ARGS__);
#else
    #define GLogVerbose(...)
    #define GLogInfo(...)
    #define GLogHint(...)
#endif//

    #define GLogWarning(...) hgl::logger::GlobalLogger.Warning   (std::source_location::current(),__VA_ARGS__);
    #define GLogError(...)   hgl::logger::GlobalLogger.Error     (std::source_location::current(),__VA_ARGS__);
    #define GLogFatal(...)   hgl::logger::GlobalLogger.Fatal     (std::source_location::current(),__VA_ARGS__);

    #define DEFINE_LOGGER_MODULE(name) namespace hgl::logger{hgl::logger::ObjectLogger Log##name(OS_TEXT(#name));}
    #define EXTERN_LOGGER_MODULE(name) namespace hgl::logger{extern hgl::logger::ObjectLogger Log##name;}

#ifdef _DEBUG
    #define MLogVerbose(name,...)   {hgl::logger::Log##name.Verbose(std::source_location::current(),__VA_ARGS__);}
    #define MLogInfo(name,...)      {hgl::logger::Log##name.Info   (std::source_location::current(),__VA_ARGS__);}
    #define MLogHint(name,...)      {hgl::logger::Log##name.Hint   (std::source_location::current(),__VA_ARGS__);}
#else
    #define MLogVerbose(name,...)
    #define MLogInfo(name,...)
    #define MLogHint(name,...)
#endif//

    #define MLogWarning(name,...)   {hgl::logger::Log##name.Warning(std::source_location::current(),__VA_ARGS__);}
    #define MLogError(name,...)     {hgl::logger::Log##name.Error  (std::source_location::current(),__VA_ARGS__);}
    #define MLogFatal(name,...)     {hgl::logger::Log##name.Fatal  (std::source_location::current(),__VA_ARGS__);}

    #define RETURN_FALSE            {GLogVerbose(   OS_TEXT("return(false)"));return(false);}
    #define RETURN_ERROR(v)         {GLogInfo(      OS_TEXT("return error(")+OSString::numberOf(v)+OS_TEXT(")"));return(v);}
    #define RETURN_ERROR_NULL       {GLogInfo(      OS_TEXT("return error(nullptr)"));return(nullptr);}

    #define RETURN_BOOL(proc)       {if(proc)return(true);RETURN_FALSE}

    #define IF_FALSE_RETURN(str)    if(!str)RETURN_FALSE;
}//namespace hgl::logger
