/**
 * ValueKVMapTest.cpp
 *
 * ValueKVMap<K,V> 紧凑有序 KV 映射测试：
 *   - Add 按 key 排序插入 / 重复 key 行为
 *   - Get/GetValuePointer/ContainsKey/ContainsValue
 *   - Change / operator[] 赋值
 *   - DeleteByKey / DeleteByValue / GetAndDelete
 *   - Find/FindPos
 *   - 迭代顺序（有序性）
 *   - Clear/Free
 */
#include<hgl/type/ValueKVMap.h>
#include"test_common.h"

using namespace hgl;

static void TestAddAndOrder()
{
    std::printf("\n=== ValueKVMap: Add and Ordering ===\n");

    ValueKVMap<int, int> map;

    TEST_ASSERT(map.IsEmpty(), "Initially empty");

    // 乱序添加，验证内部按 key 排序
    map.Add(30, 300);
    map.Add(10, 100);
    map.Add(20, 200);

    TEST_ASSERT(map.GetCount() == 3, "Count is 3");

    // 迭代顺序应有序
    int last_key = -1;
    bool ordered = true;
    for(const auto *p = map.begin(); p != map.end(); ++p)
    {
        if(p->key < last_key)
            ordered = false;
        last_key = p->key;
    }
    TEST_ASSERT(ordered, "Iteration is ascending by key");

    // 重复 key：Add 返回 nullptr，不覆盖原值
    map.Add(10, 999);
    TEST_ASSERT(map.GetCount() == 3, "Duplicate key does not add entry");
    int v = 0;
    map.Get(10, v);
    TEST_ASSERT(v == 100, "Duplicate key keeps original value");
}

static void TestGet()
{
    std::printf("\n=== ValueKVMap: Query ===\n");

    ValueKVMap<int, int> map;
    map.Add(1, 11);
    map.Add(2, 22);
    map.Add(3, 33);

    int v = 0;
    TEST_ASSERT(map.Get(2, v) && v == 22, "Get found");
    TEST_ASSERT(!map.Get(9, v), "Get not found");

    int *vp = map.GetValuePointer(2);
    TEST_ASSERT(vp != nullptr && *vp == 22, "GetValuePointer found");
    TEST_ASSERT(map.GetValuePointer(9) == nullptr, "GetValuePointer not found");

    TEST_ASSERT(map.ContainsKey(3), "ContainsKey found");
    TEST_ASSERT(!map.ContainsKey(4), "ContainsKey not found");
    TEST_ASSERT(map.ContainsValue(33), "ContainsValue found");
    TEST_ASSERT(!map.ContainsValue(44), "ContainsValue not found");

    TEST_ASSERT(map.Find(2) >= 0, "Find returns position");
    TEST_ASSERT(map.Find(9) == -1, "Find not found returns -1");

    int pos = -1;
    TEST_ASSERT(map.FindPos(3, pos) && pos >= 0, "FindPos found");
    TEST_ASSERT(!map.FindPos(9, pos), "FindPos not found");
}

static void TestModify()
{
    std::printf("\n=== ValueKVMap: Modify ===\n");

    ValueKVMap<int, int> map;
    map.Add(1, 10);
    map.Add(2, 20);

    // Change 已存在 key
    TEST_ASSERT(map.Change(1, 11), "Change on existing key");
    int v = 0;
    map.Get(1, v);
    TEST_ASSERT(v == 11, "Value updated after Change");

    // Change 不存在的 key
    TEST_ASSERT(!map.Change(9, 90), "Change on missing key fails");

    // operator[] 赋值
    map[2] = 22;
    map.Get(2, v);
    TEST_ASSERT(v == 22, "operator[] assignment");

    // GetAndDelete
    v = 0;
    TEST_ASSERT(map.GetAndDelete(1, v) && v == 11, "GetAndDelete succeeded");
    TEST_ASSERT(!map.ContainsKey(1), "Key removed after GetAndDelete");

    // DeleteByKey
    TEST_ASSERT(map.DeleteByKey(2), "DeleteByKey succeeded");
    TEST_ASSERT(map.IsEmpty(), "Empty after delete");

    // DeleteByValue
    map.Add(5, 55);
    map.Add(6, 66);
    TEST_ASSERT(map.DeleteByValue(55), "DeleteByValue succeeded");
    TEST_ASSERT(map.GetCount() == 1 && map.ContainsKey(6), "1 item left after DeleteByValue");
}

static void TestSerialAccess()
{
    std::printf("\n=== ValueKVMap: Serial Access (public API) ===\n");

    ValueKVMap<int, int> map;
    map.Add(10, 100);
    map.Add(20, 200);
    map.Add(30, 300);

    // 通过迭代顺序 + Find 位置验证有序访问
    const auto *p = map.begin();
    TEST_ASSERT(p != nullptr && p->key == 10 && p->value == 100, "begin is the minimum key");
    const auto *last = map.end() - 1;
    TEST_ASSERT(last->key == 30 && last->value == 300, "end-1 is the maximum key");

    // Find 返回的位置与迭代顺序一致（有序性）
    const int pos_20 = map.Find(20);
    TEST_ASSERT(pos_20 == 1, "Find(20) position is 1 (ordered)");
    const int pos_30 = map.Find(30);
    TEST_ASSERT(pos_30 == 2, "Find(30) position is 2 (ordered)");

    // 按 key 修改后顺序不变
    TEST_ASSERT(map.Change(20, 250), "Change(20)");
    int v = 0;
    TEST_ASSERT(map.Get(20, v) && v == 250, "Change effective");
}

static void TestClearFree()
{
    std::printf("\n=== ValueKVMap: Clear/Free ===\n");

    ValueKVMap<int, int> map;
    for(int i = 0; i < 50; i++)
        map.Add(i, i * 10);

    TEST_ASSERT(map.GetCount() == 50, "50 items");

    map.Clear();
    TEST_ASSERT(map.IsEmpty(), "Empty after Clear");

    map.Add(1, 1);
    TEST_ASSERT(map.GetCount() == 1, "Usable after Clear");

    map.Free();
    TEST_ASSERT(map.IsEmpty(), "Empty after Free");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestAddAndOrder();
    TestGet();
    TestModify();
    TestSerialAccess();
    TestClearFree();

    return TEST_SUMMARY();
}
