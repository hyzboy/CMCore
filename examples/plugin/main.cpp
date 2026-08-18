#include<hgl/plugin/PlugInManager.h>
#include<hgl/plugin/PlugInInterface.h>
#include<hgl/platform/Platform.h>
#include<hgl/filesystem/FileSystem.h>
#include<cstdio>

using namespace hgl;
using namespace filesystem;

//P1 回归验证：须在 bad_plugin_dir 目录下运行(cd bad_plugin_dir 后执行本程序)。
//该目录含"坏插件文件"(非 PE 文本)，而真插件 DLL(CMP.Test.Good.DLL)与程序同目录，
//故 PlugInManager 的 findpath = [cwd(坏文件), cwd/Plug-ins, 程序目录(真DLL), 程序目录/Plug-ins]，
//可直接回归"坏文件在前、真 DLL 在后"的多路径查找场景。
int main()
{
    int failed=0;

    #define CHECK(cond,desc) \
        if(cond)printf("[PASS] %s\n",desc); \
        else{printf("[FAIL] %s\n",desc);++failed;}

    PlugInManager pm(OS_TEXT("Test"));

    //1. 未加载时:文件存在(坏文件) → NO_LOAD
    CHECK(pm.GetStatus(OS_TEXT("Good"))==PlugInStatus::NO_LOAD,"GetStatus(unloaded)=NO_LOAD");

    //2. Bug1 回归:findpath 中坏文件在前、真 DLL 在后,必须仍能加载成功
    PlugIn *pi=pm.LoadPlugin(OS_TEXT("Good"));
    CHECK(pi!=nullptr,"Bug1:bad file first, multi-path load still succeeds");

    if(pi)
    {
        int marker=0;
        CHECK(pi->GetInterface(1,&marker),"GetInterface(1) ok");
        CHECK(marker==0x504C5547,"interface marker correct");

        CHECK(pm.GetStatus(OS_TEXT("Good"))==PlugInStatus::COMPLETE,"GetStatus(loaded)=COMPLETE");
    }

    //3. Bug2 回归:显式加载坏插件(非 PE 文本文件)必须失败且不登记
    OSString bad_dir;
    GetCurrentPath(bad_dir);        //cwd = bad_plugin_dir

    OSString bad_fullpath=JoinPathWithFilename(bad_dir,OS_TEXT("CMP.Test.Bad.DLL"));

    CHECK(pm.LoadPlugin(OS_TEXT("Bad"),bad_fullpath)==nullptr,"Bug2:bad plugin load returns nullptr");

    //4. Bug3 回归:失败插件不登记 → GetStatus 不再误报 COMPLETE(修复前坏插件被静默登记而误报)
    CHECK(pm.GetStatus(OS_TEXT("Bad"))==PlugInStatus::NO_LOAD,"Bug3:failed plugin status=NO_LOAD (no false COMPLETE)");

    //5. 不存在的插件
    CHECK(pm.LoadPlugin(OS_TEXT("NoSuch"))==nullptr,"load non-existent returns nullptr");
    CHECK(pm.GetStatus(OS_TEXT("NoSuch"))==PlugInStatus::NONE,"GetStatus(non-existent)=NONE");

    //6. 扫描:坏插件目录下应有 Bad/Good 两个文件名
    OSStringList names;
    pm.Scan(names);
    CHECK(names.Find(OS_TEXT("Bad"))!=-1,"scan contains Bad");
    CHECK(names.Find(OS_TEXT("Good"))!=-1,"scan contains Good");

    //7. 卸载后状态回落
    CHECK(pm.UnloadPlugin(OS_TEXT("Good")),"unload Good");
    CHECK(pm.GetStatus(OS_TEXT("Good"))==PlugInStatus::NO_LOAD,"GetStatus(after unload)=NO_LOAD");

    printf("\n%s\n",failed?"SOME TESTS FAILED!":"ALL TESTS PASSED!");

    return failed?1:0;
}
