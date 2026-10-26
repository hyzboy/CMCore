#pragma once

#include<hgl/type/String.h>
#include<hgl/log/LogLevel.h>

namespace hgl::logger
{
    using LogCategoryId=uint32;
    using LogOutputterId=uint32;
    using ObjectLoggerId=uint64;

    constexpr LogCategoryId InvalidLogCategoryId=0;
    constexpr LogOutputterId InvalidLogOutputterId=0;
    constexpr ObjectLoggerId InvalidObjectLoggerId=0;

    LogCategoryId RegisterLogCategory(const OSString &,LogLevel);
    LogCategoryId FindLogCategory(const OSString &);
    bool SetLogCategoryLevel(LogCategoryId,LogLevel);
    bool SetLogCategoryOutputterLevel(LogCategoryId,LogOutputterId,LogLevel);
    bool ClearLogCategoryOutputterLevel(LogCategoryId,LogOutputterId);

    LogOutputterId FindLogOutputter(const OSString &);
    bool SetLogOutputterLevel(LogOutputterId,LogLevel);

    bool IsLogEnabled(LogCategoryId,LogLevel);
    bool IsLogEnabledForOutputter(LogCategoryId,LogOutputterId,LogLevel);
}//namespace hgl::logger
