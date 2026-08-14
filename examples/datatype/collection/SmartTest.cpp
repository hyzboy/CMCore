/**
 * SmartTest.cpp
 *
 * Smart.h 智能指针测试：
 *   - SharedPtr：引用计数、拷贝/赋值/作用域释放、use_count/only/valid
 *   - WeakPtr：不影响生命周期、lock() 升级、对象销毁后过期
 *   - SharedArray：数组管理、下标访问
 *   - AutoDelete：RAII 释放、Finish 交出所有权
 *   - 对象销毁次数验证（杜绝泄漏/双重释放）
 */
#include<hgl/platform/Platform.h>
#include<hgl/type/Smart.h>
#include"test_common.h"

using namespace hgl;

namespace
{
    struct Tracked
    {
        static int alive;

        int value;

        explicit Tracked(int v):value(v) { ++alive; }
        ~Tracked() { --alive; }
    };

    int Tracked::alive = 0;
}//namespace

static void TestSharedPtrBasic()
{
    std::printf("\n=== Smart: SharedPtr Basic ===\n");

    TEST_ASSERT(Tracked::alive == 0, "No objects alive initially");

    {
        SharedPtr<Tracked> sp(new Tracked(42));

        TEST_ASSERT(sp.valid(), "valid");
        TEST_ASSERT(sp.use_count() == 1, "use_count == 1");
        TEST_ASSERT(sp.only(), "Unique owner");
        TEST_ASSERT(sp->value == 42, "operator->");
        TEST_ASSERT((*sp).value == 42, "operator*");
        TEST_ASSERT(sp.get() != nullptr, "get not null");

        // 拷贝：计数 +1
        SharedPtr<Tracked> sp2 = sp;
        TEST_ASSERT(sp.use_count() == 2, "use_count == 2 after copy");
        TEST_ASSERT(!sp.only(), "Not unique owner");
        TEST_ASSERT(sp.get() == sp2.get(), "Share the same object");

        // 裸指针比较（用 get() 避免 operator== 重载歧义）
        Tracked *raw = sp.get();
        TEST_ASSERT(sp.get() == raw, "Equal to raw pointer comparison");

        // 拷贝赋值
        SharedPtr<Tracked> sp3;
        sp3 = sp;
        TEST_ASSERT(sp.use_count() == 3, "use_count == 3 after copy assignment");

        // 作用域释放
        {
            SharedPtr<Tracked> sp4 = sp;
            TEST_ASSERT(sp.use_count() == 4, "Nested scope +1");
        }
        TEST_ASSERT(sp.use_count() == 3, "Nested scope destructor -1");
        TEST_ASSERT(Tracked::alive == 1, "Object still alive");
    }

    TEST_ASSERT(Tracked::alive == 0, "Object destroyed after all released");
}

static void TestSharedPtrAssignRaw()
{
    std::printf("\n=== Smart: SharedPtr Raw Pointer Assignment ===\n");

    {
        SharedPtr<Tracked> sp(new Tracked(1));
        TEST_ASSERT(Tracked::alive == 1, "1 alive");

        sp = new Tracked(2);            // 释放旧对象，接管新对象
        TEST_ASSERT(Tracked::alive == 1, "Still 1 alive after assignment");
        TEST_ASSERT(sp->value == 2, "Points to new object");
    }

    TEST_ASSERT(Tracked::alive == 0, "All destroyed after destructor");
}

static void TestWeakPtr()
{
    std::printf("\n=== Smart: WeakPtr ===\n");

    WeakPtr<Tracked> weak;

    {
        SharedPtr<Tracked> sp(new Tracked(7));

        weak = sp;                       // 弱引用绑定
        TEST_ASSERT(weak.valid(), "weak valid while object alive");
        TEST_ASSERT(weak.get() == sp.get(), "weak and shared point to same object");
        TEST_ASSERT(sp.use_count() == 1, "Weak reference does not affect strong count");

        // lock() 升级
        SharedPtr<Tracked> locked = weak.lock();
        TEST_ASSERT(locked.valid(), "lock succeeded");
        TEST_ASSERT(locked->value == 7, "Content correct after lock");
        TEST_ASSERT(sp.use_count() == 2, "Strong count +1 after lock");
    }

    // SharedPtr 全部销毁
    TEST_ASSERT(Tracked::alive == 0, "Object destroyed");
    TEST_ASSERT(!weak.valid(), "weak invalid after object destroyed");
    TEST_ASSERT(weak.get() == nullptr, "Expired weak get is null");

    // 过期后 lock() 返回无效 SharedPtr
    SharedPtr<Tracked> locked2 = weak.lock();
    TEST_ASSERT(!locked2.valid(), "lock invalid after expiry");
    TEST_ASSERT(locked2.get() == nullptr, "Expired lock get is null");
}

static void TestSharedArray()
{
    std::printf("\n=== Smart: SharedArray ===\n");

    {
        SharedArray<int> sa(new int[5]);
        sa[0] = 10;
        sa[4] = 50;

        TEST_ASSERT(sa[0] == 10 && sa[4] == 50, "operator[] access");

        SharedArray<int> sa2 = sa;
        TEST_ASSERT(sa2[0] == 10, "Copy shares array");

        // WeakArray 升级
        WeakArray<int> wa = sa;
        SharedArray<int> locked = wa.lock();
        TEST_ASSERT(locked.valid(), "WeakArray lock succeeded");
        locked[1] = 20;
        TEST_ASSERT(sa[1] == 20, "Modification visible after upgrade (same array)");
    }

    TEST_ASSERT(true, "Array scope ends");
}

static void TestAutoDelete()
{
    std::printf("\n=== Smart: AutoDelete ===\n");

    {
        AutoDelete<Tracked> ad(new Tracked(5));
        TEST_ASSERT(ad->value == 5, "operator->");
        TEST_ASSERT(!(!ad), "operator! on non-null");
        TEST_ASSERT(Tracked::alive == 1, "1 alive");
    }

    TEST_ASSERT(Tracked::alive == 0, "AutoDelete deletes object on destruction");

    {
        AutoDelete<Tracked> ad;
        ad = new Tracked(6);             // 赋值接管
        Tracked *p = ad.Finish();        // 交出所有权
        TEST_ASSERT(Tracked::alive == 1, "Object still alive after Finish");
        delete p;                        // 手动释放
        TEST_ASSERT(Tracked::alive == 0, "Manual release complete");
    }
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestSharedPtrBasic();
    TestSharedPtrAssignRaw();
    TestWeakPtr();
    TestSharedArray();
    TestAutoDelete();

    TEST_ASSERT(Tracked::alive == 0, "No final leak");

    return TEST_SUMMARY();
}
