#pragma once

#include<hgl/type/String.h>
#include<vector>
#include<hgl/log/LogLevel.h>
#include<hgl/Charset.h>
#include<hgl/type/StdString.h>
#include<typeinfo>
#include<string>
#include<source_location>
#include<cstdarg>
#include<cstdint>

namespace hgl::logger
{
    /**
    * 对像日志输出类
    */
    class ObjectLogger
    {
        const std::type_info *object_type_info      =nullptr;          ///< 所属对象类型信息(可为空：全局/匿名logger)

        OSString               object_type_name;                       ///< 模块/类型名(来自typeid或手工命名)
        OSString               object_instance_name;                   ///< 实例名(可选，优先于类型名输出)

        std::vector<u8char>    log_buffer_u8;                         ///< UTF-8格式化临时缓冲区
        std::vector<u16char>   log_buffer_u16;                        ///< UTF-16格式化临时缓冲区

        uint64                 module_message_count    =0;             ///< 当前logger已输出消息计数(用于时间采样)
        uint32                 time_output_interval    =100;           ///< 时间输出间隔(每N条输出一次时间，默认100)
        LogLevel               source_location_min_level=LogLevel::Error;///< 输出source_location的最低级别(默认Error)
        bool                   output_module_name      =false;         ///< 是否输出模块名前缀(默认关闭)
        bool                   output_level_name       =false;         ///< 是否输出日志级别前缀(默认关闭)

    public:

        ObjectLogger();                                                ///< 默认构造：全局logger语义
        ObjectLogger(const std::type_info *);                          ///< 按类型构造：自动提取类型名
        ObjectLogger(const os_char *name);                             ///< 按名称构造：用于模块logger

        virtual ~ObjectLogger()=default;                               ///< 虚析构：支持多态使用

        const OSString &GetLoggerTypeName()const { return object_type_name; }                    ///< 获取logger类型名

        const size_t   GetLoggerTypeHash()const { return object_type_info?object_type_info->hash_code():0; } ///< 获取类型哈希(空则0)

        void           SetLoggerInstanceName(const OSString &name) { object_instance_name=name; } ///< 设置实例名(用于区分同类型实例)

        const OSString &GetLoggerInstanceName()const { return object_instance_name; }             ///< 获取实例名

        void           SetTimeOutputInterval(uint32 interval) { time_output_interval=interval>0?interval:1; } ///< 设置时间输出间隔(最小1)

        const uint32   GetTimeOutputInterval()const { return time_output_interval; }              ///< 获取时间输出间隔

        void           SetSourceLocationMinLevel(const LogLevel level) { source_location_min_level=level; }   ///< 设置source_location输出阈值

        const LogLevel GetSourceLocationMinLevel()const { return source_location_min_level; }     ///< 获取source_location输出阈值

        void           SetOutputModuleName(const bool value) { output_module_name=value; }         ///< 设置是否输出模块名

        const bool     IsOutputModuleName()const { return output_module_name; }                    ///< 查询模块名输出开关

        void           SetOutputLevelName(const bool value) { output_level_name=value; }           ///< 设置是否输出级别名

        const bool     IsOutputLevelName()const { return output_level_name; }                      ///< 查询级别名输出开关

public:

        void LogText(const std::source_location &sl,const LogLevel level,const std::string &text)
        {
            if(text.empty())
                return;

            const OSString os_text=ToOSString(text);
            LogString(sl,level,os_text.c_str(),static_cast<int>(os_text.Length()));
        }

    #define LOG_HELPER_DECL_STD_STRING(LEVEL_ENUM) \
        void LEVEL_ENUM(const std::source_location &sl,const std::string &text) \
        {   \
            LogText(sl,LogLevel::LEVEL_ENUM,text); \
        }

        LOG_HELPER_DECL_STD_STRING(Verbose)
        LOG_HELPER_DECL_STD_STRING(Debug)
        LOG_HELPER_DECL_STD_STRING(Info)
        LOG_HELPER_DECL_STD_STRING(Notice)
        LOG_HELPER_DECL_STD_STRING(Warning)
        LOG_HELPER_DECL_STD_STRING(Error)
        LOG_HELPER_DECL_STD_STRING(Fatal)

    #undef LOG_HELPER_DECL_STD_STRING

        void LogString(const std::source_location &,const LogLevel,const os_char *,const int);     ///< 核心输出入口：拼装前缀并下发到sink

        void LogPrintf(const std::source_location &,const LogLevel,const u8char *,va_list);        ///< UTF-8格式化输出(va_list)
        void LogPrintf(const std::source_location &,const LogLevel,const u16char *,va_list);       ///< UTF-16格式化输出(va_list)

    #ifdef HGL_SUPPORT_CHAR8_T
        void LogString(const std::source_location &,const LogLevel,const char *,const size_t);     ///< char*直写入口(兼容char8_t场景)
        void LogPrintf(const std::source_location &,const LogLevel,const char *,va_list);          ///< char*格式化入口(兼容char8_t场景)
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
        LOG_HELPER_DECL(Debug,CHAR_TYPE)    \
        LOG_HELPER_DECL(Info,CHAR_TYPE)       \
        LOG_HELPER_DECL(Notice,CHAR_TYPE)       \
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

#define OBJECT_LOGGER   ::hgl::logger::ObjectLogger Log{&typeid(*this)};

#ifdef _DEBUG
    #define LogVerbose(...) this->Log.Verbose   (std::source_location::current(),__VA_ARGS__);
    #define LogDebug(...)   this->Log.Debug     (std::source_location::current(),__VA_ARGS__);
#else
    #define LogVerbose(...) {}
    #define LogDebug(...)   {}
#endif//

    #define LogInfo(...)    this->Log.Info      (std::source_location::current(),__VA_ARGS__);
    #define LogNotice(...)  this->Log.Notice    (std::source_location::current(),__VA_ARGS__);
    #define LogWarning(...) this->Log.Warning   (std::source_location::current(),__VA_ARGS__);
    #define LogError(...)   this->Log.Error     (std::source_location::current(),__VA_ARGS__);
    #define LogFatal(...)   this->Log.Fatal     (std::source_location::current(),__VA_ARGS__);

    //全局日志对象的快捷方式

#ifdef _DEBUG
    #define GLogVerbose(...)    {::hgl::logger::GlobalLogger.Verbose   (std::source_location::current(),__VA_ARGS__);}
    #define GLogDebug(...)      {::hgl::logger::GlobalLogger.Debug     (std::source_location::current(),__VA_ARGS__);}
#else
    #define GLogVerbose(...)    {}
    #define GLogDebug(...)      {}
#endif//

    #define GLogInfo(...)       {::hgl::logger::GlobalLogger.Info      (std::source_location::current(),__VA_ARGS__);}
    #define GLogNotice(...)     {::hgl::logger::GlobalLogger.Notice    (std::source_location::current(),__VA_ARGS__);}
    #define GLogWarning(...)    {::hgl::logger::GlobalLogger.Warning   (std::source_location::current(),__VA_ARGS__);}
    #define GLogError(...)      {::hgl::logger::GlobalLogger.Error     (std::source_location::current(),__VA_ARGS__);}
    #define GLogFatal(...)      {::hgl::logger::GlobalLogger.Fatal     (std::source_location::current(),__VA_ARGS__);}

    #define DEFINE_LOGGER_MODULE(name) namespace hgl::logger{::hgl::logger::ObjectLogger Log##name(OS_TEXT(#name));}
    #define EXTERN_LOGGER_MODULE(name) namespace hgl::logger{extern ::hgl::logger::ObjectLogger Log##name;}

    // 非类成员函数/静态函数日志绑定：
    // 用法：
    //   1) 在某处定义模块对象：DEFINE_LOGGER_MODULE(Render)
    //   2) 在需要输出的.cpp顶部绑定：USE_MODULE_LOGGER(Render)
    //   3) 在自由函数中直接使用：FLogInfo(...), FLogError(...)
    #define USE_MODULE_LOGGER(name) \
        namespace hgl::logger{extern ::hgl::logger::ObjectLogger Log##name;} \
        static inline ::hgl::logger::ObjectLogger &GetModuleLogger(){return ::hgl::logger::Log##name;}

#ifdef _DEBUG
    #define FLogVerbose(...)   {GetModuleLogger().Verbose(std::source_location::current(),__VA_ARGS__);}
    #define FLogDebug(...)     {GetModuleLogger().Debug  (std::source_location::current(),__VA_ARGS__);}
#else
    #define FLogVerbose(...)   {}
    #define FLogDebug(...)     {}
#endif//

    #define FLogInfo(...)      {GetModuleLogger().Info   (std::source_location::current(),__VA_ARGS__);}
    #define FLogNotice(...)    {GetModuleLogger().Notice (std::source_location::current(),__VA_ARGS__);}
    #define FLogWarning(...)   {GetModuleLogger().Warning(std::source_location::current(),__VA_ARGS__);}
    #define FLogError(...)     {GetModuleLogger().Error  (std::source_location::current(),__VA_ARGS__);}
    #define FLogFatal(...)     {GetModuleLogger().Fatal  (std::source_location::current(),__VA_ARGS__);}

#ifdef _DEBUG
    #define MLogVerbose(name,...)   {::hgl::logger::Log##name.Verbose(std::source_location::current(),__VA_ARGS__);}
    #define MLogDebug(name,...)     {::hgl::logger::Log##name.Debug  (std::source_location::current(),__VA_ARGS__);}
#else
    #define MLogVerbose(name,...)   {}
    #define MLogDebug(name,...)     {}
#endif//

    #define MLogInfo(name,...)      {::hgl::logger::Log##name.Info   (std::source_location::current(),__VA_ARGS__);}
    #define MLogNotice(name,...)    {::hgl::logger::Log##name.Notice (std::source_location::current(),__VA_ARGS__);}
    #define MLogWarning(name,...)   {::hgl::logger::Log##name.Warning(std::source_location::current(),__VA_ARGS__);}
    #define MLogError(name,...)     {::hgl::logger::Log##name.Error  (std::source_location::current(),__VA_ARGS__);}
    #define MLogFatal(name,...)     {::hgl::logger::Log##name.Fatal  (std::source_location::current(),__VA_ARGS__);}

    #define RETURN_FALSE            {GLogVerbose(   OS_TEXT("return(false)"));return(false);}
    #define RETURN_ERROR(v)         {GLogInfo(      OS_TEXT("return error(")+OSString::numberOf(v)+OS_TEXT(")"));return(v);}
    #define RETURN_ERROR_NULL       {GLogInfo(      OS_TEXT("return error(nullptr)"));return(nullptr);}

    #define RETURN_BOOL(proc)       {if(proc)return(true);RETURN_FALSE}

    #define IF_FALSE_RETURN(str)    if(!str)RETURN_FALSE;
}//namespace hgl::logger
