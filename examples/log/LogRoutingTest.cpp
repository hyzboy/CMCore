#include<hgl/log/Log.h>
#include<hgl/log/Logger.h>
#include<hgl/object/Object.h>

DEFINE_LOGGER_MODULE_WITH_LEVEL(LogRoutingTest,LogLevel::Info)

namespace
{
    struct RoutingTestObject{};

    class RoutingTestBase:public hgl::Object
    {
    public:

        HGL_OBJECT(RoutingTestBase,hgl::Object)

        int init_count=0;
        int release_count=0;

    protected:

        bool OnInit() override
        {
            ++init_count;
            return(true);
        }

        void OnRelease() override
        {
            ++release_count;
        }
    };//class RoutingTestBase

    class RoutingTestLeaf:public RoutingTestBase
    {
    public:

        HGL_OBJECT(RoutingTestLeaf,RoutingTestBase)
    };//class RoutingTestLeaf
}

int main()
{
    using namespace hgl;
    using namespace hgl::logger;

    if(!InitLogger(OS_TEXT("LogRoutingTest")))
        return 1;

    const LogCategoryId category=LogLogRoutingTest.GetCategoryId();
    const LogOutputterId console=FindLogOutputter(OS_TEXT("Console"));
    const LogOutputterId file=FindLogOutputter(OS_TEXT("File"));
    ObjectLogger first_object_logger(&typeid(RoutingTestObject));
    ObjectLogger second_object_logger(&typeid(RoutingTestObject));

    if(category==InvalidLogCategoryId||
       console==InvalidLogOutputterId||
       file==InvalidLogOutputterId||
       first_object_logger.GetLoggerId()==InvalidObjectLoggerId||
       first_object_logger.GetLoggerId()==second_object_logger.GetLoggerId()||
       LogLogRoutingTest.GetLoggerId()==first_object_logger.GetLoggerId()||
       !LogLogRoutingTest.GetLoggerInstanceName().IsEmpty()||
       FindLogCategory(OS_TEXT("LogRoutingTest"))!=category||
       SetLogCategoryOutputterLevel(category,InvalidLogOutputterId,LogLevel::Info))
        return 2;

    first_object_logger.SetLoggerInstanceName(OS_TEXT("test-instance"));
    if(first_object_logger.GetLoggerInstanceName()!=OS_TEXT("test-instance"))
        return 12;

    first_object_logger.Info(std::source_location::current(),U8_TEXT("object logger identity test"));

    const ObjectTypeInfo *type_info=RegisterObjectType(OS_TEXT("RoutingTestObject"),nullptr,LogLevel::Info);

    if(!type_info||
       type_info!=RegisterObjectType(OS_TEXT("RoutingTestObject"),nullptr,LogLevel::Info)||
       type_info->type_id!=ComputeObjectTypeId(OS_TEXT("RoutingTestObject"))||
       type_info->category_id==InvalidLogCategoryId||
       !type_info->IsDerivedFrom(type_info->type_id)||
       FindObjectType(type_info->type_id)!=type_info||
       GetObjectTypeCount()==0)
        return 13;

    const ObjectId object_id=AllocateObjectId();

    first_object_logger.BindObjectIdentity(object_id,type_info->type_id,InvalidLogCategoryId,type_info->type_name);

    if(!first_object_logger.IsBoundToObject()||
       first_object_logger.GetBoundObjectId()!=object_id||
       first_object_logger.GetBoundObjectTypeId()!=type_info->type_id||
       AllocateObjectId()<=object_id)
        return 14;

    RoutingTestLeaf object;

    object.SetInstanceName(OS_TEXT("leaf-instance"));

    const ObjectId leaf_object_id=object.GetObjectId();

    if(leaf_object_id==InvalidObjectId||
       !object.Init()||
       object.Init()!=true||                                            // 幂等
       !object.IsInitialized()||
       object.init_count!=1||
       object.GetTypeId()!=RoutingTestLeaf::StaticTypeInfo()->type_id||
       !object.IsA<RoutingTestBase>()||
       !object.IsA<hgl::Object>()||
       object.Cast<RoutingTestBase>()!=&object||
       object.GetInstanceName()!=OS_TEXT("leaf-instance")||
       object.GetLogger().GetBoundObjectId()!=leaf_object_id||
       object.GetLogger().GetBoundObjectTypeId()!=RoutingTestLeaf::StaticTypeInfo()->type_id)
        return 15;

    object.Release();
    object.Release();                                                     // 幂等

    if(!object.IsReleased()||object.release_count!=1)
        return 16;

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
