/**
 * PointerObjectPoolTest.cpp
 * 
 * 全面测试 PointerObjectPool<T> 类
 * 测试场景：
 *   - 基本操作（初始化、分配、释放）
 *   - non-trivial 类型（带构造/析构的类）
 *   - 对象生命周期管理
 *   - 内存泄漏检测
 *   - 自定义构造参数
 *   - 边界条件
 */
#include<hgl/platform/Platform.h>
#include<hgl/type/PointerObjectPool.h>
#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<memory>
#include<algorithm>

using namespace hgl;
using namespace std;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "  ✓ PASS: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  ✗ FAIL: " << message << std::endl; \
        } \
    } while(0)

// ==================== 测试用 non-trivial 类型 ====================

// 简单计数类
class SimpleObject
{
public:
    static int construct_count;
    static int destruct_count;

    int id;

    SimpleObject() : id(0)
    {
        construct_count++;
        cout << "    SimpleObject() constructed, total=" << construct_count << endl;
    }

    SimpleObject(int _id) : id(_id)
    {
        construct_count++;
        cout << "    SimpleObject(" << _id << ") constructed, total=" << construct_count << endl;
    }

    ~SimpleObject()
    {
        destruct_count++;
        cout << "    SimpleObject(id=" << id << ") destructed, total=" << destruct_count << endl;
    }

    void Reset(int new_id)
    {
        id = new_id;
    }
};

int SimpleObject::construct_count = 0;
int SimpleObject::destruct_count = 0;

// 复杂对象类
class ComplexObject
{
public:
    static int construct_count;
    static int destruct_count;

    int id;
    std::string name;
    std::vector<int> data;
    int* dynamic_ptr;

    ComplexObject() : id(0), name("default"), dynamic_ptr(new int(0))
    {
        construct_count++;
        cout << "    ComplexObject() default constructed" << endl;
    }

    ComplexObject(int _id, const std::string& _name) 
        : id(_id), name(_name), dynamic_ptr(new int(_id * 100))
    {
        construct_count++;
        for (int i = 0; i < 5; i++) {
            data.push_back(_id * 10 + i);
        }
        cout << "    ComplexObject(" << _id << ", " << _name << ") constructed" << endl;
    }

    ~ComplexObject()
    {
        destruct_count++;
        cout << "    ComplexObject(id=" << id << ", name=" << name << ") destructed" << endl;
        delete dynamic_ptr;
    }

    ComplexObject(const ComplexObject&) = delete;
    ComplexObject& operator=(const ComplexObject&) = delete;
};

int ComplexObject::construct_count = 0;
int ComplexObject::destruct_count = 0;

// 资源管理类
class ResourceHolder
{
public:
    static int construct_count;
    static int destruct_count;
    static int active_resources;

    int resource_id;
    bool is_loaded;

    ResourceHolder() : resource_id(0), is_loaded(false)
    {
        construct_count++;
        cout << "    ResourceHolder() default constructed" << endl;
    }

    ResourceHolder(int _id) : resource_id(_id), is_loaded(false)
    {
        construct_count++;
        cout << "    ResourceHolder(" << _id << ") constructed" << endl;
    }

    ~ResourceHolder()
    {
        destruct_count++;
        if (is_loaded) {
            Unload();
        }
        cout << "    ResourceHolder(id=" << resource_id << ") destructed" << endl;
    }

    void Load()
    {
        if (!is_loaded) {
            is_loaded = true;
            active_resources++;
            cout << "    Resource " << resource_id << " loaded, active=" << active_resources << endl;
        }
    }

    void Unload()
    {
        if (is_loaded) {
            is_loaded = false;
            active_resources--;
            cout << "    Resource " << resource_id << " unloaded, active=" << active_resources << endl;
        }
    }

    ResourceHolder(const ResourceHolder&) = delete;
    ResourceHolder& operator=(const ResourceHolder&) = delete;
};

int ResourceHolder::construct_count = 0;
int ResourceHolder::destruct_count = 0;
int ResourceHolder::active_resources = 0;

// ==================== TEST 1: 基本操作 ====================

void test_basic_operations()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: PointerObjectPool<SimpleObject> - Basic Operations" << endl;
    cout << "========================================\n" << endl;

    SimpleObject::construct_count = 0;
    SimpleObject::destruct_count = 0;

    {
        PointerObjectPool<SimpleObject> pool;

        cout << "[1.1] Initialize pool:" << endl;
        bool init_result = pool.Init();
        TEST_ASSERT(init_result, "Pool initialization succeeded");

        cout << "\n[1.2] Pool is initially empty:" << endl;
        TEST_ASSERT(pool.GetIdleCount() == 0, "Initial idle count is 0");

        cout << "\n[1.3] Acquire from empty pool returns nullptr:" << endl;
        SimpleObject* obj1 = pool.Acquire();
        TEST_ASSERT(obj1 == nullptr, "Acquire from empty pool returns nullptr");

        cout << "\n[1.4] Add objects to pool:" << endl;
        pool.AddObject(new SimpleObject(100));
        pool.AddObject(new SimpleObject(200));
        pool.AddObject(new SimpleObject(300));
        TEST_ASSERT(pool.GetIdleCount() == 3, "Idle count is 3 after adding objects");

        cout << "\n[1.5] Acquire objects:" << endl;
        SimpleObject* acquired1 = pool.Acquire();
        TEST_ASSERT(acquired1 != nullptr, "First acquire succeeded");
        TEST_ASSERT(pool.GetIdleCount() == 2, "Idle count decreased to 2");

        SimpleObject* acquired2 = pool.Acquire();
        TEST_ASSERT(acquired2 != nullptr, "Second acquire succeeded");
        TEST_ASSERT(pool.GetIdleCount() == 1, "Idle count decreased to 1");

        cout << "\n[1.6] Release objects:" << endl;
        bool release1 = pool.Release(acquired1);
        TEST_ASSERT(release1, "First release succeeded");
        TEST_ASSERT(pool.GetIdleCount() == 2, "Idle count increased to 2");

        bool release2 = pool.Release(acquired2);
        TEST_ASSERT(release2, "Second release succeeded");
        TEST_ASSERT(pool.GetIdleCount() == 3, "Idle count increased to 3");

        cout << "\n[1.7] Pool destructor will clean up objects..." << endl;
    }

    cout << "\n[1.8] Verify all objects destructed:" << endl;
    TEST_ASSERT(SimpleObject::construct_count == SimpleObject::destruct_count,
                "All constructed objects were destructed");
    cout << "    Constructs: " << SimpleObject::construct_count 
         << ", Destructs: " << SimpleObject::destruct_count << endl;
}

// ==================== TEST 2: 自定义构造参数 ====================

void test_custom_construction()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Custom Object Construction" << endl;
    cout << "========================================\n" << endl;

    ComplexObject::construct_count = 0;
    ComplexObject::destruct_count = 0;

    {
        PointerObjectPool<ComplexObject> pool;
        pool.Init();

        cout << "[2.1] Add objects with custom constructors:" << endl;
        pool.AddObject(new ComplexObject(1, "Player"));
        pool.AddObject(new ComplexObject(2, "Enemy"));
        pool.AddObject(new ComplexObject(3, "NPC"));

        cout << "\n[2.2] Acquire and verify object properties:" << endl;
        ComplexObject* obj1 = pool.Acquire();
        TEST_ASSERT(obj1 != nullptr, "Object acquired");
        TEST_ASSERT(!obj1->name.empty(), "Object has non-empty name");
        TEST_ASSERT(obj1->data.size() == 5, "Object data vector has 5 elements");
        TEST_ASSERT(obj1->dynamic_ptr != nullptr, "Object has dynamic memory");

        cout << "    Object: id=" << obj1->id << ", name=" << obj1->name << endl;

        ComplexObject* obj2 = pool.Acquire();
        TEST_ASSERT(obj2 != nullptr, "Second object acquired");
        TEST_ASSERT(obj1->id != obj2->id, "Different objects have different IDs");

        cout << "\n[2.3] Release objects:" << endl;
        pool.Release(obj1);
        pool.Release(obj2);
    }

    cout << "\n[2.4] Verify memory cleanup:" << endl;
    TEST_ASSERT(ComplexObject::construct_count == ComplexObject::destruct_count,
                "All objects properly destructed");
    cout << "    Constructs: " << ComplexObject::construct_count 
         << ", Destructs: " << ComplexObject::destruct_count << endl;
}

// ==================== TEST 3: 对象重用 ====================

void test_object_reuse()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: Object Reuse" << endl;
    cout << "========================================\n" << endl;

    SimpleObject::construct_count = 0;
    SimpleObject::destruct_count = 0;

    {
        PointerObjectPool<SimpleObject> pool;
        pool.Init();

        cout << "[3.1] Add 5 objects:" << endl;
        for (int i = 0; i < 5; i++) {
            pool.AddObject(new SimpleObject(i * 10));
        }
        int initial_constructs = SimpleObject::construct_count;

        cout << "\n[3.2] Acquire-Release cycle (50 iterations):" << endl;
        for (int cycle = 0; cycle < 50; cycle++) {
            SimpleObject* obj = pool.Acquire();
            if (obj) {
                obj->Reset(cycle);
                pool.Release(obj);
            }
        }

        cout << "\n[3.3] Verify no new constructions:" << endl;
        TEST_ASSERT(SimpleObject::construct_count == initial_constructs,
                    "No new objects constructed during reuse");
        TEST_ASSERT(SimpleObject::destruct_count == 0,
                    "No objects destructed during reuse");
    }

    cout << "\n[3.4] Final cleanup verification:" << endl;
    TEST_ASSERT(SimpleObject::construct_count == SimpleObject::destruct_count,
                "All objects cleaned up");
}

// ==================== TEST 4: 边界条件 ====================

void test_boundary_conditions()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Boundary Conditions" << endl;
    cout << "========================================\n" << endl;

    SimpleObject::construct_count = 0;
    SimpleObject::destruct_count = 0;

    {
        PointerObjectPool<SimpleObject> pool;
        pool.Init();

        cout << "[4.1] Release nullptr:" << endl;
        bool null_release = pool.Release(nullptr);
        TEST_ASSERT(!null_release, "Release nullptr returns false");

        cout << "\n[4.2] Add nullptr:" << endl;
        bool null_add = pool.AddObject(nullptr);
        TEST_ASSERT(!null_add, "AddObject nullptr returns false");
        TEST_ASSERT(pool.GetIdleCount() == 0, "Idle count still 0");

        cout << "\n[4.3] Exhaust pool:" << endl;
        pool.AddObject(new SimpleObject(1));
        pool.AddObject(new SimpleObject(2));

        SimpleObject* obj1 = pool.Acquire();
        SimpleObject* obj2 = pool.Acquire();
        SimpleObject* obj3 = pool.Acquire();  // Pool now empty

        TEST_ASSERT(obj1 != nullptr, "First acquire succeeded");
        TEST_ASSERT(obj2 != nullptr, "Second acquire succeeded");
        TEST_ASSERT(obj3 == nullptr, "Third acquire from empty pool returns nullptr");
        TEST_ASSERT(pool.GetIdleCount() == 0, "Pool is empty");

        cout << "\n[4.4] Release and reacquire:" << endl;
        pool.Release(obj1);
        TEST_ASSERT(pool.GetIdleCount() == 1, "One object returned");

        SimpleObject* obj4 = pool.Acquire();
        TEST_ASSERT(obj4 == obj1, "Reacquired same object");
        TEST_ASSERT(pool.GetIdleCount() == 0, "Pool empty again");

        pool.Release(obj1);
        pool.Release(obj2);
    }
}

// ==================== TEST 5: 资源生命周期 ====================

void test_resource_lifecycle()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: Resource Lifecycle Management" << endl;
    cout << "========================================\n" << endl;

    ResourceHolder::construct_count = 0;
    ResourceHolder::destruct_count = 0;
    ResourceHolder::active_resources = 0;

    {
        PointerObjectPool<ResourceHolder> pool;
        pool.Init();

        cout << "[5.1] Create resource holders:" << endl;
        for (int i = 0; i < 10; i++) {
            pool.AddObject(new ResourceHolder(1000 + i));
        }

        cout << "\n[5.2] Acquire and load resources:" << endl;
        vector<ResourceHolder*> active_holders;
        for (int i = 0; i < 5; i++) {
            ResourceHolder* holder = pool.Acquire();
            if (holder) {
                holder->Load();
                active_holders.push_back(holder);
            }
        }
        TEST_ASSERT(ResourceHolder::active_resources == 5, "5 resources loaded");

        cout << "\n[5.3] Unload and release:" << endl;
        for (auto holder : active_holders) {
            holder->Unload();
            pool.Release(holder);
        }
        TEST_ASSERT(ResourceHolder::active_resources == 0, "All resources unloaded");

        cout << "\n[5.4] Reacquire and verify resource state:" << endl;
        ResourceHolder* reused = pool.Acquire();
        TEST_ASSERT(reused != nullptr, "Reacquired resource holder");
        TEST_ASSERT(!reused->is_loaded, "Reused holder is unloaded");
        
        reused->Load();
        TEST_ASSERT(ResourceHolder::active_resources == 1, "Resource loaded");
        reused->Unload();
        pool.Release(reused);

        cout << "\n[5.5] Pool cleanup..." << endl;
    }

    cout << "\n[5.6] Verify cleanup:" << endl;
    TEST_ASSERT(ResourceHolder::construct_count == ResourceHolder::destruct_count,
                "All resources destructed");
    TEST_ASSERT(ResourceHolder::active_resources == 0,
                "No resources leaked");
    cout << "    Constructs: " << ResourceHolder::construct_count 
         << ", Destructs: " << ResourceHolder::destruct_count << endl;
}

// ==================== TEST 6: 大规模对象池 ====================

void test_large_pool()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Large Object Pool" << endl;
    cout << "========================================\n" << endl;

    SimpleObject::construct_count = 0;
    SimpleObject::destruct_count = 0;

    {
        PointerObjectPool<SimpleObject> pool;
        pool.Init();

        cout << "[6.1] Add 1000 objects:" << endl;
        for (int i = 0; i < 1000; i++) {
            pool.AddObject(new SimpleObject(i));
        }
        TEST_ASSERT(pool.GetIdleCount() == 1000, "1000 objects in pool");
        TEST_ASSERT(SimpleObject::construct_count == 1000, "1000 objects constructed");

        cout << "\n[6.2] Acquire all objects:" << endl;
        vector<SimpleObject*> objects;
        for (int i = 0; i < 1000; i++) {
            SimpleObject* obj = pool.Acquire();
            if (obj) {
                objects.push_back(obj);
            }
        }
        TEST_ASSERT(objects.size() == 1000, "All 1000 objects acquired");
        TEST_ASSERT(pool.GetIdleCount() == 0, "Pool is empty");

        cout << "\n[6.3] Release half, reacquire half:" << endl;
        for (size_t i = 0; i < 500; i++) {
            pool.Release(objects[i]);
        }
        TEST_ASSERT(pool.GetIdleCount() == 500, "500 objects released");

        vector<SimpleObject*> reacquired;
        for (int i = 0; i < 500; i++) {
            SimpleObject* obj = pool.Acquire();
            if (obj) {
                reacquired.push_back(obj);
            }
        }
        TEST_ASSERT(reacquired.size() == 500, "500 objects reacquired");
        TEST_ASSERT(pool.GetIdleCount() == 0, "Pool empty again");

        cout << "\n[6.4] Release all:" << endl;
        for (size_t i = 500; i < objects.size(); i++) {
            pool.Release(objects[i]);
        }
        for (auto obj : reacquired) {
            pool.Release(obj);
        }
        TEST_ASSERT(pool.GetIdleCount() == 1000, "All 1000 objects back in pool");

        cout << "\n[6.5] Pool cleanup..." << endl;
    }

    cout << "\n[6.6] Verify final state:" << endl;
    TEST_ASSERT(SimpleObject::construct_count == 1000, "1000 constructions");
    TEST_ASSERT(SimpleObject::destruct_count == 1000, "1000 destructions");
}

// ==================== TEST 7: 多次初始化 ====================

void test_multiple_init()
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Multiple Init Calls" << endl;
    cout << "========================================\n" << endl;

    PointerObjectPool<SimpleObject> pool;

    cout << "[7.1] First init:" << endl;
    bool init1 = pool.Init();
    TEST_ASSERT(init1, "First init succeeded");

    cout << "\n[7.2] Second init (should be safe):" << endl;
    bool init2 = pool.Init();
    TEST_ASSERT(init2, "Second init succeeded");
}

// ==================== TEST 8: 交替操作模式 ====================

void test_alternating_pattern()
{
    cout << "\n========================================" << endl;
    cout << "TEST 8: Alternating Operation Pattern" << endl;
    cout << "========================================\n" << endl;

    SimpleObject::construct_count = 0;
    SimpleObject::destruct_count = 0;

    {
        PointerObjectPool<SimpleObject> pool;
        pool.Init();

        cout << "[8.1] Initial batch:" << endl;
        for (int i = 0; i < 10; i++) {
            pool.AddObject(new SimpleObject(i));
        }

        cout << "\n[8.2] Pattern: acquire 3, release 2, repeat:" << endl;
        vector<SimpleObject*> active;
        
        for (int cycle = 0; cycle < 20; cycle++) {
            // Acquire up to 3
            for (int i = 0; i < 3 && pool.GetIdleCount() > 0; i++) {
                SimpleObject* obj = pool.Acquire();
                if (obj) {
                    active.push_back(obj);
                }
            }

            // Release 2
            for (int i = 0; i < 2 && !active.empty(); i++) {
                pool.Release(active.back());
                active.pop_back();
            }
        }

        TEST_ASSERT(true, "Pattern completed without crash");

        cout << "\n[8.3] Cleanup active objects:" << endl;
        for (auto obj : active) {
            pool.Release(obj);
        }
    }

    cout << "\n[8.4] Verify cleanup:" << endl;
    TEST_ASSERT(SimpleObject::construct_count == SimpleObject::destruct_count,
                "All objects properly cleaned up");
}

// ==================== TEST 9: 外部对象释放 ====================

void test_external_pointer_release()
{
    cout << "\n========================================" << endl;
    cout << "TEST 9: External Pointer Release" << endl;
    cout << "========================================\n" << endl;

    SimpleObject::construct_count = 0;
    SimpleObject::destruct_count = 0;

    {
        PointerObjectPool<SimpleObject> pool;
        pool.Init();

        pool.AddObject(new SimpleObject(100));

        cout << "[9.1] Create external object:" << endl;
        SimpleObject* external = new SimpleObject(999);
        cout << "    External object created" << endl;

        cout << "\n[9.2] Try to release external pointer to pool:" << endl;
        // The pool will accept it (current implementation doesn't track ownership)
        bool release_result = pool.Release(external);
        cout << "    Release result: " << (release_result ? "accepted" : "rejected") << endl;

        if (release_result) {
            cout << "    Note: Pool accepted external pointer (no ownership tracking)" << endl;
            // Don't delete external manually since pool owns it now
        } else {
            delete external;
        }
    }

    cout << "\n[9.3] Cleanup completed" << endl;
}

// ==================== TEST 10: 压力测试 ====================

void test_stress()
{
    cout << "\n========================================" << endl;
    cout << "TEST 10: Stress Test" << endl;
    cout << "========================================\n" << endl;

    SimpleObject::construct_count = 0;
    SimpleObject::destruct_count = 0;

    {
        PointerObjectPool<SimpleObject> pool;
        pool.Init();

        cout << "[10.1] Stress test: 10000 acquire/release cycles:" << endl;
        
        // Populate pool
        for (int i = 0; i < 50; i++) {
            pool.AddObject(new SimpleObject(i));
        }

        vector<SimpleObject*> active;
        int total_acquires = 0;
        int total_releases = 0;

        for (int cycle = 0; cycle < 10000; cycle++) {
            // Randomly acquire or release
            if ((cycle % 3 == 0) && pool.GetIdleCount() > 0) {
                SimpleObject* obj = pool.Acquire();
                if (obj) {
                    obj->Reset(cycle);
                    active.push_back(obj);
                    total_acquires++;
                }
            }

            if ((cycle % 2 == 0) && !active.empty()) {
                pool.Release(active.back());
                active.pop_back();
                total_releases++;
            }
        }

        cout << "    Total acquires: " << total_acquires << endl;
        cout << "    Total releases: " << total_releases << endl;
        cout << "    Still active: " << active.size() << endl;

        TEST_ASSERT(true, "Stress test completed");

        // Cleanup
        for (auto obj : active) {
            pool.Release(obj);
        }
    }

    cout << "\n[10.2] Verify final state:" << endl;
    TEST_ASSERT(SimpleObject::construct_count == SimpleObject::destruct_count,
                "All objects properly destructed");
}

// ==================== Main ====================

int os_main(int, os_char**)
{
    test_basic_operations();
    test_custom_construction();
    test_object_reuse();
    test_boundary_conditions();
    test_resource_lifecycle();
    test_large_pool();
    test_multiple_init();
    test_alternating_pattern();
    test_external_pointer_release();
    test_stress();

    cout << "\n========================================" << endl;
    cout << "TEST SUMMARY" << endl;
    cout << "========================================" << endl;
    cout << "Passed: " << tests_passed << endl;
    cout << "Failed: " << tests_failed << endl;
    
    if (tests_failed == 0) {
        cout << "\n✅ ALL TESTS PASSED!" << endl;
    } else {
        cout << "\n❌ SOME TESTS FAILED!" << endl;
    }

    return tests_failed > 0 ? 1 : 0;
}
