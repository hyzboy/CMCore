#pragma once

#include<iostream>
#include<source_location>
#include<hgl/Charset.h>
#include<hgl/log/LogLevel.h>

namespace hgl::logger
{
    enum class LogTextEncoding
    {
        Native,
        UTF8,
        UTF16,
    };

    struct LogMessageMeta
    {
        const std::type_info *  object_type_info=nullptr;      ///<对象数据类型的type_info指针

        OSString                object_instance_name;          ///<对象实例名

        std::source_location    source_location;               ///<调用位置

        LogLevel                level=LogLevel::Info;          ///<日志级别

        LogTextEncoding         message_encoding=LogTextEncoding::Native; ///<正文编码
    };

    struct LogMessageText
    {
        const u8char *          message_u8=nullptr;            ///<UTF-8正文视图
        int                     message_u8_length=0;          ///<UTF-8正文长度

        const u16char *         message_u16=nullptr;           ///<UTF-16正文视图
        int                     message_u16_length=0;          ///<UTF-16正文长度
    };

    struct LogMessage
    {
        LogMessageMeta          meta;
        LogMessageText          text;
    };
}//namespace hgl::logger
