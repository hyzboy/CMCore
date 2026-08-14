/**
 * VersionDataTest.cpp
 *
 * VersionData<T> 版本数据模板测试：
 *   - 赋值触发版本更新
 *   - IsNewestVersion 状态机
 *   - GetNewestVersionData 触发 MakeNewestData
 *   - had_version 优化路径
 */
#include<hgl/platform/Platform.h>
#include<hgl/type/VersionData.h>
#include"test_common.h"

using namespace hgl;

namespace
{
    // 派生类：MakeNewestData 对数据做变换（模拟重算）
    struct DoublingData : public VersionData<int>
    {
        using VersionData<int>::operator=;   // 避免派生类隐式 operator= 隐藏基类 operator=(const T&)

        int make_count = 0;

        DoublingData():VersionData<int>(0) {}
        explicit DoublingData(const int &init):VersionData<int>(init) {}

        void MakeNewestData(int &data) override
        {
            data *= 2;
            ++make_count;
        }
    };
}//namespace

static void TestVersionFlow()
{
    std::printf("\n=== VersionData: Version State Machine ===\n");

    DoublingData vd(5);

    TEST_ASSERT(vd.IsNewestVersion(), "Initially newest version");

    // 赋值：cur_version 落后于 version
    vd = 7;
    TEST_ASSERT(!vd.IsNewestVersion(), "Not newest after assignment");
    TEST_ASSERT(vd.GetCurrentVersionData() == 7, "Current data updated");
    TEST_ASSERT(vd.GetCurrentVersion() != vd.GetNewestVersion(), "Version number lags behind");

    // 获取最新数据 -> 触发 MakeNewestData
    int result = 0;
    const uint32 ver = vd.GetNewestVersionData(result);
    TEST_ASSERT(result == 14, "MakeNewestData effective (7*2=14)");
    TEST_ASSERT(vd.IsNewestVersion(), "Newest after update");
    TEST_ASSERT(ver == vd.GetNewestVersion(), "Returns latest version");
    TEST_ASSERT(vd.make_count == 1, "MakeNewestData called 1 time");

    // 再次获取：已是最新，不再触发
    result = 0;
    vd.GetNewestVersionData(result);
    TEST_ASSERT(result == 14, "Result unchanged");
    TEST_ASSERT(vd.make_count == 1, "MakeNewestData not triggered again");
}

static void TestUpdateNewestData()
{
    std::printf("\n=== VersionData: UpdateNewestData ===\n");

    DoublingData vd(10);

    TEST_ASSERT(!vd.UpdateNewestData(), "UpdateNewestData returns false when newest");

    vd = 20;
    TEST_ASSERT(vd.UpdateNewestData(), "Returns true when not newest");
    TEST_ASSERT(vd.GetCurrentVersionData() == 40, "Data updated to 20*2=40");
    TEST_ASSERT(vd.make_count == 1, "MakeNewestData called 1 time");

    TEST_ASSERT(!vd.UpdateNewestData(), "Second call returns false");
}

static void TestHadVersion()
{
    std::printf("\n=== VersionData: had_version Optimization ===\n");

    DoublingData vd(1);
    vd = 5;                              // cur_ver=0, ver=1

    // 先触发一次更新，让 cur_version 追平
    int result = 0;
    vd.GetNewestVersionData(result);     // 5*2=10, cur_ver=1
    TEST_ASSERT(result == 10, "First update 5*2=10");
    TEST_ASSERT(vd.make_count == 1, "MakeNewestData called 1 time");

    const uint32 v1 = vd.GetNewestVersion();   // 1

    // 连续两次赋值产生新版本（赋值后 cur_version = 赋值前的 version）
    vd = 7;                              // cur_ver=1, ver=2
    vd = 9;                              // cur_ver=2, ver=3
    const uint32 v3 = vd.GetNewestVersion();   // 3

    // 持有更旧的版本号 v1（< cur_version=2）：触发更新
    result = 0;
    const uint32 ver = vd.GetNewestVersionData(result, v1);
    TEST_ASSERT(result == 18, "Old version triggers update (9*2=18)");
    TEST_ASSERT(ver == v3, "Returns latest version");
    TEST_ASSERT(vd.make_count == 2, "MakeNewestData called 2 times");

    // 持有最新版本号 v3：不触发更新（该路径不写 result，直接返回当前版本号）
    result = 0;
    const uint32 ver_latest = vd.GetNewestVersionData(result, v3);
    TEST_ASSERT(ver_latest == v3, "Latest version returned directly");
    TEST_ASSERT(vd.make_count == 2, "MakeNewestData not called again");
}

static void TestSameValueNoUpdate()
{
    std::printf("\n=== VersionData: Same Value No Update ===\n");

    DoublingData vd(3);
    const uint32 v0 = vd.GetNewestVersion();

    vd = 3;                         // 与当前数据相同
    TEST_ASSERT(vd.GetNewestVersion() == v0, "Same value does not bump version");
    TEST_ASSERT(vd.IsNewestVersion(), "Still newest version");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestVersionFlow();
    TestUpdateNewestData();
    TestHadVersion();
    TestSameValueNoUpdate();

    return TEST_SUMMARY();
}
