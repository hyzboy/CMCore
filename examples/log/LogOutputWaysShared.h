#pragma once

#include<hgl/log/Log.h>
#include<hgl/log/Logger.h>

EXTERN_LOGGER_MODULE(LogWaysDemo)

namespace example::log_demo
{
    bool InitLogSystem();

    void RunFreeFunctionLogs();
    void RunCrossUnitFreeFunctionLogs();
    void RunModuleDirectLogs();

    class Worker
    {
    public:
        OBJECT_LOGGER

        Worker();
        void RunMemberLogs();
    };
}
