#include"LogOutputWaysShared.h"

USE_MODULE_LOGGER(LogWaysDemo)

namespace example::log_demo
{
    void RunCrossUnitFreeFunctionLogs()
    {
        FLogInfo(OS_TEXT("[free-cross-tu] output from another cpp but same module logger."));
    }
}
