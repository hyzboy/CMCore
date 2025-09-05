#pragma once

#include<hgl/type/String.h>
#include<hgl/log/LogLevel.h>
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

        U8String object_instance_name;

    public:

        ObjectLogger(const std::type_info *info):object_type_info(info)
        {
            object_instance_name.Clear();
        }

        virtual ~ObjectLogger()=default;

        const std::string &GetLoggerTypeName()const
        {
            return object_type_info->name();
        }

        const size_t GetLoggerTypeHash()const
        {
            return object_type_info->hash_code();
        }

        void SetLoggerInstanceName(const U8String &name)
        {
            object_instance_name=name;
        }

        const U8String &GetLoggerInstanceName()const
        {
            return object_instance_name;
        }

public:

        void LogString(const std::source_location &,const LogLevel,const char *,const size_t);
        void LogString(const std::source_location &,const LogLevel,const u16char *,const size_t);

        void LogPrintf(const std::source_location &,const LogLevel,const char *fmt,...);
        void LogPrintf(const std::source_location &,const LogLevel,const u16char *fmt,...);

        // va_list based helpers used by variadic wrappers
        void LogPrintfV(const std::source_location &,const LogLevel,const char *fmt, va_list args);
        void LogPrintfV(const std::source_location &,const LogLevel,const u16char *fmt, va_list args);

    #define SINGLE_LETTER_HELPER_DECL(LEVEL_ENUM) \
        void LEVEL_ENUM(const std::source_location &sl,const char *fmt,...) \
        {   \
            if(!fmt||!*fmt) return; \
        \
            va_list args;   \
        \
            va_start(args,fmt); \
            LogPrintfV(sl,LogLevel::LEVEL_ENUM,fmt,args);  \
            va_end(args);   \
        }   \
        \
        void LEVEL_ENUM(const std::source_location &sl,const u16char *fmt,...)  \
        {   \
            if(!fmt||!*fmt) return; \
        \
            va_list args;   \
        \
            va_start(args,fmt); \
            LogPrintfV(sl,LogLevel::LEVEL_ENUM,fmt,args);   \
            va_end(args);   \
        }

        SINGLE_LETTER_HELPER_DECL(Verbose)
        SINGLE_LETTER_HELPER_DECL(Info)
        SINGLE_LETTER_HELPER_DECL(Hint)
        SINGLE_LETTER_HELPER_DECL(Warning)
        SINGLE_LETTER_HELPER_DECL(Error)
        SINGLE_LETTER_HELPER_DECL(Fatal)

    #undef SINGLE_LETTER_HELPER_DECL
    };//ObjectLogger

    extern ObjectLogger GlobalLogger;///<全局日志对象

    #define LogVerbose(...) this->Log.Verbose   (std::source_location::current(),__VA_ARGS__);
    #define LogInfo(...)    this->Log.Info      (std::source_location::current(),__VA_ARGS__);
    #define LogHint(...)    this->Log.Hint      (std::source_location::current(),__VA_ARGS__);
    #define LogWarning(...) this->Log.Warning   (std::source_location::current(),__VA_ARGS__);
    #define LogError(...)   this->Log.Error     (std::source_location::current(),__VA_ARGS__);
    #define LogFatal(...)   this->Log.Fatal     (std::source_location::current(),__VA_ARGS__);

    //全局日志对象的快捷方式
    #define GLogVerbose(...) hgl::logger::GlobalLogger.Verbose   (std::source_location::current(),__VA_ARGS__);
    #define GLogInfo(...)    hgl::logger::GlobalLogger.Info      (std::source_location::current(),__VA_ARGS__);
    #define GLogHint(...)    hgl::logger::GlobalLogger.Hint      (std::source_location::current(),__VA_ARGS__);
    #define GLogWarning(...) hgl::logger::GlobalLogger.Warning   (std::source_location::current(),__VA_ARGS__);
    #define GLogError(...)   hgl::logger::GlobalLogger.Error     (std::source_location::current(),__VA_ARGS__);
    #define GLogFatal(...)   hgl::logger::GlobalLogger.Fatal     (std::source_location::current(),__VA_ARGS__);

#define OBJECT_LOGGER hgl::logger::ObjectLogger Log{&typeid(*this)};
}//namespace hgl::logger
