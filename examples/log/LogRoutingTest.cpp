#include<hgl/log/Log.h>
#include<hgl/log/Logger.h>

DEFINE_LOGGER_MODULE_WITH_LEVEL(LogRoutingTest,LogLevel::Info)

int main()
{
    using namespace hgl;
    using namespace hgl::logger;

    if(!InitLogger(OS_TEXT("LogRoutingTest")))
        return 1;

    const LogCategoryId category=LogLogRoutingTest.GetCategoryId();
    const LogOutputterId console=FindLogOutputter(OS_TEXT("Console"));
    const LogOutputterId file=FindLogOutputter(OS_TEXT("File"));

    if(category==InvalidLogCategoryId||
       console==InvalidLogOutputterId||
       file==InvalidLogOutputterId||
       FindLogCategory(OS_TEXT("LogRoutingTest"))!=category||
       SetLogCategoryOutputterLevel(category,InvalidLogOutputterId,LogLevel::Info))
        return 2;

    if(!IsLogEnabledForOutputter(category,console,LogLevel::Info)||
       IsLogEnabledForOutputter(category,console,LogLevel::Debug))
        return 3;

    if(!SetLogOutputterLevel(console,LogLevel::Warning)||
       !SetLogOutputterLevel(file,LogLevel::Fatal))
        return 4;

    if(IsLogEnabledForOutputter(category,console,LogLevel::Notice)||
       !IsLogEnabledForOutputter(category,file,LogLevel::Fatal))
        return 5;

    if(!SetLogCategoryOutputterLevel(category,file,LogLevel::Verbose)||
       !IsLogEnabledForOutputter(category,file,LogLevel::Info)||
       IsLogEnabledForOutputter(category,console,LogLevel::Info))
        return 6;

    MLogInfo(LogRoutingTest,U8_TEXT("HGL_LOG_ROUTING_FILE_ONLY"));

    if(!SetLogCategoryOutputterLevel(category,console,LogLevel::Debug)||
       !IsLogEnabledForOutputter(category,console,LogLevel::Info))
        return 7;

    if(!ClearLogCategoryOutputterLevel(category,console)||
       !IsLogEnabledForOutputter(category,console,LogLevel::Warning)||
       !ClearLogCategoryOutputterLevel(category,file))
        return 8;

    if(!SetLogCategoryLevel(category,LogLevel::Fatal)||
       IsLogEnabled(category,LogLevel::Error))
        return 9;

    MLogInfo(LogRoutingTest,U8_TEXT("filtered message should not be formatted"));

    if(!SetLogCategoryLevel(category,LogLevel::Verbose)||
       !SetLogOutputterLevel(file,LogLevel::Verbose)||
       !IsLogEnabled(category,LogLevel::Info))
        return 10;

    if(!SetLogOutputterLevel(console,LogLevel::Fatal)||
       !SetLogOutputterLevel(file,LogLevel::Verbose)||
       !IsLogEnabled(category,LogLevel::Info))
        return 11;

    return 0;
}
