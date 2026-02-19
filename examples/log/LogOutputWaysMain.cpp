#include"LogOutputWaysShared.h"

int main()
{
    if(!example::log_demo::InitLogSystem())
        return -1;

    GLogInfo(OS_TEXT("[global] output via global logger macro GLogInfo."));

    example::log_demo::RunFreeFunctionLogs();
    example::log_demo::RunCrossUnitFreeFunctionLogs();
    example::log_demo::RunModuleDirectLogs();

    example::log_demo::Worker worker;
    worker.RunMemberLogs();

    return 0;
}
