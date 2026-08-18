#define __MAKE_PLUGIN__

#include<hgl/plugin/PlugIn.h>
#include<hgl/plugin/PlugInInterface.h>
#include<hgl/platform/Platform.h>

using namespace hgl;

//P1 回归测试用微型插件：接口版本 ver=1，GetInterface 向 data 写入标记 0x504C5547('GULP')，
//GetCapability 上报 FileExtensions = {"tst"}，用于验证扫描/探测/加载全链路。
static const char *test_extensions[]={"tst"};

static uint32 GetPlugInVersion()
{
    return 1;
}

static u16char *GetPlugInIntro()
{
    static u16char intro[]=U16_TEXT("CMCore P1 regression test plugin");
    return intro;
}

static bool GetPlugInInterface(uint32 ver,void *data)
{
    if(ver!=1)return false;

    if(!data)return false;

    *static_cast<int *>(data)=0x504C5547;       //'GULP'

    return true;
}

static bool GetCapability(uint32 id,void *data)
{
    if(id==uint32(PlugInCapability::FileExtensions))
    {
        PlugInFileExtensions *fe=static_cast<PlugInFileExtensions *>(data);

        fe->count=1;
        fe->items=test_extensions;

        return true;
    }

    return false;
}

static PlugInInterface pii=
{
    nullptr,            //ClosePlugIn

    GetPlugInVersion,
    GetPlugInIntro,

    GetPlugInInterface,
    GetCapability
};

HGL_PLUGIN_FUNC PlugInInterface *InitPlugIn()
{
    return &pii;
}
