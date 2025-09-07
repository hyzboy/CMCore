#pragma once

#include<iostream>
#include<source_location>
#include<hgl/Charset.h>
#include<hgl/log/LogLevel.h>

namespace hgl::logger
{
    struct LogMessage
    {
        const std::type_info *  object_type_info;     ///<对象数据类型的type_info指针

        OSString                object_instance_name;

        std::source_location    source_location;

        LogLevel                level;

        const os_char *         message;
        int                     message_length;
    };
}//namespace hgl::logger
