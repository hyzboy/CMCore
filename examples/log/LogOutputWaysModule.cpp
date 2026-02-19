#include"LogOutputWaysShared.h"

DEFINE_LOGGER_MODULE(LogWaysDemo)
USE_MODULE_LOGGER(LogWaysDemo)

namespace example::log_demo
{
    bool InitLogSystem()
    {
        return hgl::logger::InitLogger(OS_TEXT("LogOutputWaysExample"));
    }

    static void LocalStaticHelper()
    {
        FLogInfo(OS_TEXT("[free-static] output from static helper in same cpp."));
    }

    void RunFreeFunctionLogs()
    {
        FLogInfo(OS_TEXT("[free] output from non-member function via FLog..."));
        FLogWarning(OS_TEXT("[free] warning from non-member function."));
        LocalStaticHelper();
    }

    void RunModuleDirectLogs()
    {
        MLogNotice(LogWaysDemo,OS_TEXT("[module-direct] output via MLog... without FLog binding."));
    }

    Worker::Worker()
    {
        Log.SetLoggerInstanceName(OS_TEXT("Worker#1"));
        Log.SetTimeOutputInterval(20);
        Log.SetSourceLocationMinLevel(hgl::LogLevel::Error);
        Log.SetOutputModuleName(true);
        Log.SetOutputLevelName(true);
    }

    void Worker::RunMemberLogs()
    {
        LogInfo(OS_TEXT("[member] class member output via LogInfo."));
        LogNotice(OS_TEXT("[member] source_location is still hidden under Error level threshold."));
        LogError(OS_TEXT("[member] error log will include source_location by current policy."));
    }
}
