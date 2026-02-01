/**
 * FixedValuePoolTest.cpp
 * 
 * 全面测试 FixedValuePool<T> 类
 * 测试场景：
 *   - 基本操作（初始化、分配、释放）
 *   - trivial 类型（POD 结构体、基本类型）
 *   - 边界条件（满池、空池、无效指针）
 *   - 内存安全性
 *   - 性能特征
 */

#include<hgl/type/FixedValuePool.h>
#include<iostream>
#include<cassert>
#include<cstring>
#include<string>
#include<vector>
#include<algorithm>
#include<random>

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

// ==================== 测试用 trivial 类型 ====================

struct Point2D
{
    float x;
    float y;
    
    Point2D() : x(0.0f), y(0.0f) {}
    Point2D(float _x, float _y) : x(_x), y(_y) {}
};

struct Vector3D
{
    double x, y, z;
    
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
};

struct GameEntity
{
    int id;
    float health;
    float armor;
    bool active;
    
    GameEntity() : id(0), health(100.0f), armor(0.0f), active(false) {}
};

// ==================== TEST 1: 基本操作 ====================

void test_basic_operations()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: FixedValuePool<int> - Basic Operations" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<int> pool;

    cout << "[1.1] Initialize pool with capacity 10:" << endl;
    bool init_result = pool.Init(10);
    TEST_ASSERT(init_result, "Pool initialization succeeded");

    cout << "\n[1.2] Acquire objects:" << endl;
    int* obj1 = pool.Acquire();
    TEST_ASSERT(obj1 != nullptr, "First acquire succeeded");
    *obj1 = 42;

    int* obj2 = pool.Acquire();
    TEST_ASSERT(obj2 != nullptr, "Second acquire succeeded");
    *obj2 = 100;

    TEST_ASSERT(obj1 != obj2, "Different objects have different addresses");
    TEST_ASSERT(*obj1 == 42, "First object value preserved");
    TEST_ASSERT(*obj2 == 100, "Second object value preserved");

    cout << "\n[1.3] Release objects:" << endl;
    bool release1 = pool.Release(obj1);
    TEST_ASSERT(release1, "First release succeeded");

    bool release2 = pool.Release(obj2);
    TEST_ASSERT(release2, "Second release succeeded");

    cout << "\n[1.4] Reacquire objects (should reuse):" << endl;
    int* obj3 = pool.Acquire();
    TEST_ASSERT(obj3 != nullptr, "Reacquire succeeded");
    TEST_ASSERT(obj3 == obj2 || obj3 == obj1, "Reused released object");
}

// ==================== TEST 2: 边界条件 ====================

void test_boundary_conditions()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Boundary Conditions" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<int> pool;
    pool.Init(5);

    cout << "[2.1] Fill pool to capacity:" << endl;
    vector<int*> objects;
    for (int i = 0; i < 5; i++) {
        int* obj = pool.Acquire();
        TEST_ASSERT(obj != nullptr, "Acquire " + std::to_string(i + 1) + " succeeded");
        if (obj) {
            *obj = i * 10;
            objects.push_back(obj);
        }
    }

    cout << "\n[2.2] Attempt to acquire from full pool:" << endl;
    int* overflow = pool.Acquire();
    TEST_ASSERT(overflow == nullptr, "Acquire from full pool returns nullptr");

    cout << "\n[2.3] Release one and reacquire:" << endl;
    bool release_ok = pool.Release(objects[2]);
    TEST_ASSERT(release_ok, "Release succeeded");

    int* reacquired = pool.Acquire();
    TEST_ASSERT(reacquired != nullptr, "Reacquire after release succeeded");
    TEST_ASSERT(reacquired == objects[2], "Reacquired same object");

    cout << "\n[2.4] Release invalid pointers:" << endl;
    int external_value = 999;
    bool invalid_release = pool.Release(&external_value);
    TEST_ASSERT(!invalid_release, "Release external pointer fails");

    bool null_release = pool.Release(nullptr);
    TEST_ASSERT(!null_release, "Release nullptr fails");

    cout << "\n[2.5] Release all objects:" << endl;
    for (auto obj : objects) {
        pool.Release(obj);
    }
    pool.Release(reacquired);
}

// ==================== TEST 3: POD 结构体 ====================

void test_pod_structs()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: FixedValuePool<Point2D> - POD Structs" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<Point2D> pool;
    pool.Init(20);

    cout << "[3.1] Create points:" << endl;
    vector<Point2D*> points;
    for (int i = 0; i < 10; i++) {
        Point2D* p = pool.Acquire();
        TEST_ASSERT(p != nullptr, "Point acquire " + std::to_string(i) + " succeeded");
        if (p) {
            p->x = static_cast<float>(i);
            p->y = static_cast<float>(i * 2);
            points.push_back(p);
        }
    }

    cout << "\n[3.2] Verify point values:" << endl;
    bool all_correct = true;
    for (size_t i = 0; i < points.size(); i++) {
        if (points[i]->x != static_cast<float>(i) || 
            points[i]->y != static_cast<float>(i * 2)) {
            all_correct = false;
            break;
        }
    }
    TEST_ASSERT(all_correct, "All point values are correct");

    cout << "\n[3.3] Modify and verify:" << endl;
    points[5]->x = 999.0f;
    points[5]->y = -123.0f;
    TEST_ASSERT(points[5]->x == 999.0f, "Modified x value");
    TEST_ASSERT(points[5]->y == -123.0f, "Modified y value");

    cout << "\n[3.4] Release points:" << endl;
    for (auto p : points) {
        pool.Release(p);
    }
}

// ==================== TEST 4: 复杂结构体 ====================

void test_complex_structs()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: FixedValuePool<GameEntity> - Complex Structs" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<GameEntity> pool;
    pool.Init(50);

    cout << "[4.1] Create entities:" << endl;
    vector<GameEntity*> entities;
    for (int i = 0; i < 30; i++) {
        GameEntity* entity = pool.Acquire();
        TEST_ASSERT(entity != nullptr, "Entity acquire succeeded");
        if (entity) {
            entity->id = 1000 + i;
            entity->health = 100.0f - (i * 2.5f);
            entity->armor = i * 1.5f;
            entity->active = (i % 2 == 0);
            entities.push_back(entity);
        }
    }

    cout << "\n[4.2] Verify entity data:" << endl;
    TEST_ASSERT(entities[0]->id == 1000, "First entity ID correct");
    TEST_ASSERT(entities[29]->id == 1029, "Last entity ID correct");
    TEST_ASSERT(entities[10]->active, "Even-indexed entity active");
    TEST_ASSERT(!entities[11]->active, "Odd-indexed entity inactive");

    cout << "\n[4.3] Simulate entity lifecycle:" << endl;
    // "Kill" some entities and release them
    vector<GameEntity*> dead_entities;
    for (size_t i = 0; i < entities.size(); i++) {
        if (entities[i]->health < 50.0f) {
            entities[i]->active = false;
            dead_entities.push_back(entities[i]);
        }
    }
    
    for (auto entity : dead_entities) {
        pool.Release(entity);
        // Remove from entities vector
        entities.erase(remove(entities.begin(), entities.end(), entity), entities.end());
    }
    
    TEST_ASSERT(dead_entities.size() > 0, "Some entities died");
    TEST_ASSERT(entities.size() < 30, "Living entities count reduced");

    cout << "\n[4.4] Spawn new entities (reusing slots):" << endl;
    for (size_t i = 0; i < dead_entities.size(); i++) {
        GameEntity* new_entity = pool.Acquire();
        TEST_ASSERT(new_entity != nullptr, "New entity spawned");
        if (new_entity) {
            new_entity->id = 2000 + i;
            new_entity->health = 100.0f;
            new_entity->armor = 50.0f;
            new_entity->active = true;
            entities.push_back(new_entity);
        }
    }

    cout << "\n[4.5] Cleanup:" << endl;
    for (auto entity : entities) {
        pool.Release(entity);
    }
}

// ==================== TEST 5: 内存对齐和访问 ====================

void test_memory_alignment()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: Memory Alignment and Access" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<Vector3D> pool;
    pool.Init(15);

    cout << "[5.1] Acquire all slots:" << endl;
    vector<Vector3D*> vectors;
    for (int i = 0; i < 15; i++) {
        Vector3D* v = pool.Acquire();
        TEST_ASSERT(v != nullptr, "Vector acquire succeeded");
        if (v) {
            v->x = i * 1.0;
            v->y = i * 2.0;
            v->z = i * 3.0;
            vectors.push_back(v);
        }
    }

    cout << "\n[5.2] Verify pointer uniqueness and validity:" << endl;
    // Check all pointers are unique
    bool all_unique = true;
    for (size_t i = 0; i < vectors.size(); i++) {
        for (size_t j = i + 1; j < vectors.size(); j++) {
            if (vectors[i] == vectors[j]) {
                all_unique = false;
                break;
            }
        }
        if (!all_unique) break;
    }
    TEST_ASSERT(all_unique, "All pointers are unique");
    
    // Check all pointers are within pool range
    Vector3D* raw_data = pool.GetRawData();
    bool all_in_range = true;
    for (auto v : vectors) {
        if (v < raw_data || v >= raw_data + 15) {
            all_in_range = false;
            break;
        }
    }
    TEST_ASSERT(all_in_range, "All pointers are within pool range");

    cout << "\n[5.3] Verify data integrity:" << endl;
    bool data_intact = true;
    for (size_t i = 0; i < vectors.size(); i++) {
        if (vectors[i]->x != i * 1.0 || 
            vectors[i]->y != i * 2.0 || 
            vectors[i]->z != i * 3.0) {
            data_intact = false;
            break;
        }
    }
    TEST_ASSERT(data_intact, "All vector data intact");

    cout << "\n[5.4] Release in random order:" << endl;
    vector<size_t> indices;
    for (size_t i = 0; i < vectors.size(); i++) {
        indices.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    for (size_t idx : indices) {
        pool.Release(vectors[idx]);
    }
}

// ==================== TEST 6: 双重释放检测 ====================

void test_double_release()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Double Release Detection" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<int> pool;
    pool.Init(10);

    cout << "[6.1] Acquire and release normally:" << endl;
    int* obj = pool.Acquire();
    TEST_ASSERT(obj != nullptr, "Acquire succeeded");
    *obj = 777;

    bool first_release = pool.Release(obj);
    TEST_ASSERT(first_release, "First release succeeded");

    cout << "\n[6.2] Attempt double release:" << endl;
    // Note: Current implementation may not prevent this
    // This test documents the behavior
    bool second_release = pool.Release(obj);
    cout << "  Double release result: " << (second_release ? "succeeded (no protection)" : "failed (protected)") << endl;
}

// ==================== TEST 7: 大容量测试 ====================

void test_large_capacity()
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Large Capacity Pool" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<int> pool;
    pool.Init(10000);

    cout << "[7.1] Acquire 10000 objects:" << endl;
    vector<int*> objects;
    int failed_count = 0;
    for (int i = 0; i < 10000; i++) {
        int* obj = pool.Acquire();
        if (obj) {
            *obj = i;
            objects.push_back(obj);
        } else {
            failed_count++;
        }
    }
    TEST_ASSERT(objects.size() == 10000, "All 10000 objects acquired");
    TEST_ASSERT(failed_count == 0, "No acquisition failures");

    cout << "\n[7.2] Verify sample values:" << endl;
    TEST_ASSERT(*objects[0] == 0, "First object value correct");
    TEST_ASSERT(*objects[5000] == 5000, "Middle object value correct");
    TEST_ASSERT(*objects[9999] == 9999, "Last object value correct");

    cout << "\n[7.3] Release all objects:" << endl;
    for (auto obj : objects) {
        pool.Release(obj);
    }
}

// ==================== TEST 8: 零大小池 ====================

void test_zero_size_pool()
{
    cout << "\n========================================" << endl;
    cout << "TEST 8: Zero Size Pool" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<int> pool;
    
    cout << "[8.1] Initialize with size 0:" << endl;
    bool init_result = pool.Init(0);
    cout << "  Init with size 0: " << (init_result ? "succeeded" : "failed") << endl;

    cout << "\n[8.2] Attempt to acquire from size-0 pool:" << endl;
    int* obj = pool.Acquire();
    TEST_ASSERT(obj == nullptr, "Acquire from size-0 pool returns nullptr");
}

// ==================== TEST 9: GetRawData 测试 ====================

void test_raw_data_access()
{
    cout << "\n========================================" << endl;
    cout << "TEST 9: Raw Data Access" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<int> pool;
    pool.Init(100);

    cout << "[9.1] Get raw data pointer:" << endl;
    int* raw_data = pool.GetRawData();
    TEST_ASSERT(raw_data != nullptr, "Raw data pointer is not null");

    cout << "\n[9.2] Acquire objects and verify they're in raw data range:" << endl;
    vector<int*> objects;
    for (int i = 0; i < 10; i++) {
        int* obj = pool.Acquire();
        if (obj) {
            objects.push_back(obj);
        }
    }

    bool all_in_range = true;
    for (auto obj : objects) {
        if (obj < raw_data || obj >= raw_data + 100) {
            all_in_range = false;
            break;
        }
    }
    TEST_ASSERT(all_in_range, "All acquired objects are within raw data range");

    for (auto obj : objects) {
        pool.Release(obj);
    }
}

// ==================== TEST 10: 交替分配释放 ====================

void test_alternating_operations()
{
    cout << "\n========================================" << endl;
    cout << "TEST 10: Alternating Acquire/Release" << endl;
    cout << "========================================\n" << endl;

    FixedValuePool<Point2D> pool;
    pool.Init(5);

    cout << "[10.1] Alternating acquire and release pattern:" << endl;
    vector<Point2D*> active_points;
    
    for (int cycle = 0; cycle < 20; cycle++) {
        // Acquire 2-3 points
        int acquire_count = 2 + (cycle % 2);
        for (int i = 0; i < acquire_count && active_points.size() < 5; i++) {
            Point2D* p = pool.Acquire();
            if (p) {
                p->x = static_cast<float>(cycle);
                p->y = static_cast<float>(i);
                active_points.push_back(p);
            }
        }

        // Release 1-2 points
        int release_count = 1 + (cycle % 2);
        for (int i = 0; i < release_count && !active_points.empty(); i++) {
            pool.Release(active_points.back());
            active_points.pop_back();
        }
    }

    TEST_ASSERT(true, "Alternating operations completed without crash");

    // Cleanup
    for (auto p : active_points) {
        pool.Release(p);
    }
}

// ==================== Main ====================

int os_main(int, os_char**)
{
    test_basic_operations();
    test_boundary_conditions();
    test_pod_structs();
    test_complex_structs();
    test_memory_alignment();
    test_double_release();
    test_large_capacity();
    test_zero_size_pool();
    test_raw_data_access();
    test_alternating_operations();

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
