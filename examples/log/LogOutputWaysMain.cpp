#include"LogOutputWaysShared.h"
#include<hgl/type/String.h>

int main()
{
    if(!example::log_demo::InitLogSystem())
        return -1;

    GLogInfo(OS_TEXT("[global] output via global logger macro GLogInfo."));
    GLogInfo(hgl::U16String(U16_TEXT("[global] direct UTF-16 log body.")));

    example::log_demo::RunFreeFunctionLogs();
    example::log_demo::RunCrossUnitFreeFunctionLogs();
    example::log_demo::RunModuleDirectLogs();

    example::log_demo::Worker worker;
    worker.RunMemberLogs();

    return 0;
}
