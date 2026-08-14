/**
 * ActiveObjectManagerTest.cpp
 *
 * ActiveObjectManager<ID,T> 活跃对象管理器测试：
 *   - GetOrCreate 创建对象
 *   - 引用计数：多次获取 / ObjectRef 拷贝、移动、赋值
 *   - Release 归零后移入闲置（OnIdle 语义），再次获取重新激活
 *   - Get 不存在返回无效引用
 *   - ClearActive/ClearIdle/析构清理
 *
 * 注：OnCreate/OnActive/OnIdle/OnClear 回调因 ObjectItem 为私有类型
 *     （public 虚函数带私有参数），派生类无法重写——这是该类的设计缺陷，
 *     此处只能通过公开 API（GetActiveCount/GetIdleCount）间接验证状态迁移。
 */
#include<hgl/platform/Platform.h>
#include<hgl/type/ActiveObjectManager.h>
#include"test_common.h"

#include<string>

using namespace hgl;

namespace
{
    struct Obj
    {
        int value;

        Obj(int v):value(v) {}
    };

    // ActiveObjectManager 构造函数为 protected，需派生后使用
    struct TestManager : public ActiveObjectManager<int, Obj>
    {
        TestManager() = default;
    };
}//namespace

static void TestGetOrCreate()
{
    std::printf("\n=== ActiveObjectManager: GetOrCreate ===\n");

    TestManager mgr;

    {
        auto ref = mgr.GetOrCreate(1, 42);

        TEST_ASSERT(ref.IsValid(), "GetOrCreate returns valid ref");
        TEST_ASSERT(ref->value == 42, "operator-> accesses object");
        TEST_ASSERT(mgr.GetActiveCount() == 1, "Active count 1");
        TEST_ASSERT(mgr.GetTotalCount() == 1, "Total count 1");

        // 再次 GetOrCreate 同一个 id：不重建，引用计数 +1
        auto ref2 = mgr.GetOrCreate(1, 999);
        TEST_ASSERT(ref2->value == 42, "Reuses existing object (args ignored)");
        TEST_ASSERT(mgr.GetActiveCount() == 1, "Still 1 active object");

        // 拷贝构造：计数 +1
        auto ref3 = ref2;
        TEST_ASSERT(ref3.IsValid(), "Copied ref valid");
        TEST_ASSERT(mgr.GetActiveCount() == 1, "Copy does not affect active count");
    }

    // 所有引用析构后：ref_count 归零 -> 移入闲置
    TEST_ASSERT(mgr.GetActiveCount() == 0, "Active count 0 after all refs released");
    TEST_ASSERT(mgr.GetIdleCount() == 1, "Object moved to idle map");
    TEST_ASSERT(mgr.GetTotalCount() == 1, "Object not destroyed, total unchanged");
}

static void TestReactivate()
{
    std::printf("\n=== ActiveObjectManager: Idle Reactivation ===\n");

    TestManager mgr;

    {
        auto ref = mgr.GetOrCreate(1, 100);
        TEST_ASSERT(ref->value == 100, "Create and get");
    }

    // 进入闲置
    TEST_ASSERT(mgr.IsIdle(1) && !mgr.IsActive(1), "Idle after release");

    {
        // 重新激活
        auto ref = mgr.GetOrCreate(1, 555);
        TEST_ASSERT(ref->value == 100, "Reactivation keeps original object (new args ignored)");
        TEST_ASSERT(mgr.IsActive(1) && !mgr.IsIdle(1), "Active after reactivation");
    }

    TEST_ASSERT(mgr.GetIdleCount() == 1, "Back to idle after release again");
}

static void TestGetMissing()
{
    std::printf("\n=== ActiveObjectManager: Get Missing id ===\n");

    TestManager mgr;

    auto ref = mgr.Get(99);
    TEST_ASSERT(!ref.IsValid(), "Missing id returns invalid ref");
    TEST_ASSERT(ref.operator->() == nullptr, "Invalid ref operator-> returns nullptr");
    TEST_ASSERT(mgr.GetActiveCount() == 0 && mgr.GetIdleCount() == 0, "No side effects");

    ref.Release();
    TEST_ASSERT(!ref.IsValid(), "Invalid after explicit Release");
}

static void TestRefCounting()
{
    std::printf("\n=== ActiveObjectManager: Ref Counting Details ===\n");

    TestManager mgr;

    {
        auto a = mgr.GetOrCreate(1, 1);   // obj1: ref=1
        auto b = a;                        // obj1: ref=2
        auto c = a;                        // obj1: ref=3

        TEST_ASSERT(mgr.GetActiveCount() == 1, "3 refs, 1 object still active");

        auto d = mgr.GetOrCreate(2, 2);    // obj2: ref=1
        TEST_ASSERT(mgr.GetActiveCount() == 2, "Two objects active");

        d = a;                             // 释放 obj2 引用(归零->闲置)，接管 obj1 引用
        TEST_ASSERT(mgr.GetActiveCount() == 1, "Copy assignment releases old ref");
        TEST_ASSERT(mgr.GetIdleCount() == 1, "obj2 moved to idle");
        TEST_ASSERT(d->value == 1, "Points to obj1 after assignment");

        c = std::move(b);                  // 释放 c 的旧引用，接管 b（移动赋值）
        TEST_ASSERT(mgr.GetActiveCount() == 1, "Still active after move assignment");
    }

    // obj1 的全部引用(a,b,c,d)已释放 -> 闲置
    TEST_ASSERT(mgr.GetActiveCount() == 0, "No active after all released");
    TEST_ASSERT(mgr.GetIdleCount() == 2, "obj1 and obj2 both idle");
}

static void TestClear()
{
    std::printf("\n=== ActiveObjectManager: ClearActive/ClearIdle/Destructor ===\n");

    TestManager *mgr = new TestManager();

    {
        auto ref = mgr->GetOrCreate(1, 1);
        TEST_ASSERT(mgr->GetTotalCount() == 1, "1 object");
    }
    // 现在在闲置表

    {
        auto ref = mgr->GetOrCreate(2, 2);   // 活跃
    }
    // 2 号进入闲置，1、2 都在闲置表

    {
        auto ref3 = mgr->GetOrCreate(3, 3);      // 活跃中

        TEST_ASSERT(mgr->GetActiveCount() == 1 && mgr->GetIdleCount() == 2, "1 active, 2 idle");
    }   // ref3 在此析构，3 号进入闲置，避免 ClearActive 后悬垂

    TEST_ASSERT(mgr->GetIdleCount() == 3, "Object 3 also idle");

    mgr->ClearActive();   // 活跃表为空（3 个对象都在闲置），无操作
    TEST_ASSERT(mgr->GetTotalCount() == 3, "Total unchanged after ClearActive (all idle)");

    mgr->ClearIdle();
    TEST_ASSERT(mgr->GetTotalCount() == 0, "All cleared");

    // 析构时无残留对象
    delete mgr;
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestGetOrCreate();
    TestReactivate();
    TestGetMissing();
    TestRefCounting();
    TestClear();

    return TEST_SUMMARY();
}
