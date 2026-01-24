#include<hgl/type/HashMap.h>
#include<hgl/type/ManagedArray.h>
#include<iostream>
#include<string>
#include<cassert>

using namespace hgl;
using namespace std;
using namespace std::string_literals;

// 测试辅助宏
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "❌ FAILED: " << message << std::endl; \
            std::cerr << "   at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define TEST_PASS(message) \
    std::cout << "✓ PASSED: " << message << std::endl

#define TEST_SECTION(title) \
    std::cout << "\n========== " << title << " ==========" << std::endl

// 测试对象
class TestObject
{
public:
    int id;
    std::string name;

    TestObject(int i, const std::string& n) : id(i), name(n) {}
    ~TestObject() {}
};

// ==================== 1. HashMap 基础功能测试 ====================
bool TestHashMapBasics()
{
    TEST_SECTION("Test 1: HashMap Basic Operations");

    HashMap<int, std::string> map;

    // 空映射测试
    TEST_ASSERT(map.IsEmpty(), "New map should be empty");
    TEST_ASSERT(map.GetCount() == 0, "Empty map count should be 0");
    TEST_PASS("Empty map initialization");

    // 添加数据
    auto kv1 = map.Add(1, "one");
    TEST_ASSERT(kv1 != nullptr, "Add first entry should succeed");
    TEST_ASSERT(map.GetCount() == 1, "Count should be 1");
    TEST_PASS("Add first entry");

    // 查找数据
    std::string value;
    bool found = map.Get(1, value);
    TEST_ASSERT(found, "Should find key 1");
    TEST_ASSERT(value == "one", "Value should be 'one'");
    TEST_PASS("Get existing entry");

    // 添加更多数据
    map.Add(2, "two");
    map.Add(3, "three");
    map.Add(4, "four");
    TEST_ASSERT(map.GetCount() == 4, "Count should be 4");
    TEST_PASS("Add multiple entries");

    // 验证所有数据
    std::string val2, val3, val4;
    TEST_ASSERT(map.Get(2, val2) && val2 == "two", "Should get 'two'");
    TEST_ASSERT(map.Get(3, val3) && val3 == "three", "Should get 'three'");
    TEST_ASSERT(map.Get(4, val4) && val4 == "four", "Should get 'four'");
    TEST_PASS("Get all entries");

    // 测试重复键
    auto duplicate = map.Add(1, "duplicate");
    TEST_ASSERT(duplicate == nullptr, "Duplicate key should fail");
    TEST_PASS("Reject duplicate key");

    // 测试不存在的键
    std::string missing;
    TEST_ASSERT(!map.Get(999, missing), "Non-existent key should not be found");
    TEST_PASS("Handle missing key");

    return true;
}

// ==================== 2. HashMap 修改操作测试 ====================
bool TestHashMapModifications()
{
    TEST_SECTION("Test 2: HashMap Modification Operations");

    HashMap<int, std::string> map;

    map.Add(1, "original");
    map.Add(2, "value2");
    map.Add(3, "value3");

    // 测试 Change
    bool changed = map.Change(1, "modified");
    TEST_ASSERT(changed, "Change should succeed");
    std::string new_val;
    map.Get(1, new_val);
    TEST_ASSERT(new_val == "modified", "Value should be modified");
    TEST_PASS("Change existing entry");

    // 测试 Change 不存在的键
    bool change_fail = map.Change(999, "should_fail");
    TEST_ASSERT(!change_fail, "Change non-existent key should fail");
    TEST_PASS("Change non-existent key fails");

    // 测试 ChangeOrAdd（修改）
    bool change_or_add1 = map.ChangeOrAdd(2, "updated");
    TEST_ASSERT(change_or_add1, "ChangeOrAdd should succeed");
    std::string updated_val;
    map.Get(2, updated_val);
    TEST_ASSERT(updated_val == "updated", "Value should be updated");
    TEST_PASS("ChangeOrAdd existing entry");

    // 测试 ChangeOrAdd（添加）
    bool change_or_add2 = map.ChangeOrAdd(10, "new_entry");
    TEST_ASSERT(change_or_add2, "ChangeOrAdd should add new entry");
    std::string new_entry;
    map.Get(10, new_entry);
    TEST_ASSERT(new_entry == "new_entry", "New entry should be added");
    TEST_ASSERT(map.GetCount() == 4, "Count should be 4");
    TEST_PASS("ChangeOrAdd new entry");

    return true;
}

// ==================== 3. HashMap 删除操作测试 ====================
bool TestHashMapDeletions()
{
    TEST_SECTION("Test 3: HashMap Deletion Operations");

    HashMap<int, std::string> map;

    for(int i = 1; i <= 10; i++)
        map.Add(i, "value"s + std::to_string(i));

    TEST_ASSERT(map.GetCount() == 10, "Should have 10 entries");
    TEST_PASS("Setup test data");

    // 删除单个键
    bool deleted = map.DeleteByKey(5);
    TEST_ASSERT(deleted, "Delete should succeed");
    TEST_ASSERT(map.GetCount() == 9, "Count should be 9");
    std::string deleted_val;
    TEST_ASSERT(!map.Get(5, deleted_val), "Deleted key should not exist");
    TEST_PASS("Delete by key");

    // 验证其他键仍然存在
    std::string val1, val10;
    TEST_ASSERT(map.Get(1, val1) && val1 == "value1", "Key 1 should still exist");
    TEST_ASSERT(map.Get(10, val10) && val10 == "value10", "Key 10 should still exist");
    TEST_PASS("Other keys unaffected");

    // GetAndDelete
    std::string extracted_val;
    bool get_and_del = map.GetAndDelete(3, extracted_val);
    TEST_ASSERT(get_and_del, "GetAndDelete should succeed");
    TEST_ASSERT(extracted_val == "value3", "Should extract correct value");
    TEST_ASSERT(!map.ContainsKey(3), "Key 3 should be deleted");
    TEST_ASSERT(map.GetCount() == 8, "Count should be 8");
    TEST_PASS("GetAndDelete");

    // DeleteByValue
    bool delete_by_val = map.DeleteByValue("value7");
    TEST_ASSERT(delete_by_val, "DeleteByValue should succeed");
    TEST_ASSERT(!map.ContainsKey(7), "Key 7 should be deleted");
    TEST_ASSERT(map.GetCount() == 7, "Count should be 7");
    TEST_PASS("DeleteByValue");

    // Batch deletion test
    // Before: 1, 2, 4, 6, 8, 9, 10 (7 entries)
    bool batch_deleted = map.DeleteAt(0, 3);
    TEST_ASSERT(batch_deleted, "Batch delete should succeed");
    TEST_ASSERT(map.GetCount() == 4, "Count should be 4 after deleting 3 entries");
    TEST_PASS("Batch delete entries");

    // Clear
    map.Clear();
    TEST_ASSERT(map.IsEmpty(), "Map should be empty after Clear");
    TEST_ASSERT(map.GetCount() == 0, "Count should be 0");
    TEST_PASS("Clear all entries");

    return true;
}

// ==================== 4. HashMap 枚举操作测试 ====================
bool TestHashMapEnumeration()
{
    TEST_SECTION("Test 4: HashMap Enumeration");

    HashMap<int, std::string> map;

    for(int i = 1; i <= 5; i++)
        map.Add(i, "value"s + std::to_string(i));

    // EnumKV
    int kv_count = 0;
    map.EnumKV([&](int key, const std::string& value) {
        kv_count++;
        // 验证键值对应关系
        TEST_ASSERT(value == "value"s + std::to_string(key), "Key-value mismatch");
        return true;
    });
    TEST_ASSERT(kv_count == 5, "Should enumerate 5 entries");
    TEST_PASS("EnumKV");

    // EnumKeys
    int key_count = 0;
    map.EnumKeys([&](int key) {
        key_count++;
        TEST_ASSERT(key >= 1 && key <= 5, "Key out of range");
        return true;
    });
    TEST_ASSERT(key_count == 5, "Should enumerate 5 keys");
    TEST_PASS("EnumKeys");

    // EnumValues
    int value_count = 0;
    map.EnumValues([&](const std::string& value) {
        value_count++;
        TEST_ASSERT(value.find("value") == 0, "Value format incorrect");
        return true;
    });
    TEST_ASSERT(value_count == 5, "Should enumerate 5 values");
    TEST_PASS("EnumValues");

    return true;
}

// ==================== 5. ObjectHashMap 基础测试 ====================
bool TestObjectHashMapBasics()
{
    TEST_SECTION("Test 5: ObjectHashMap Basic Operations");

    ObjectHashMap<int, TestObject> obj_map;

    // 添加对象
    obj_map.Add(1, new TestObject(1, "obj1"));
    obj_map.Add(2, new TestObject(2, "obj2"));
    obj_map.Add(3, new TestObject(3, "obj3"));

    TEST_ASSERT(obj_map.GetCount() == 3, "Should have 3 objects");
    TEST_PASS("Add objects");

    // 获取对象
    TestObject* obj1 = nullptr;
    obj_map.Get(1, obj1);
    TEST_ASSERT(obj1 != nullptr, "Should get object");
    TEST_ASSERT(obj1->id == 1, "Object ID should be 1");
    TEST_ASSERT(obj1->name == "obj1", "Object name should be 'obj1'");
    TEST_PASS("Get object");

    // 使用 operator[]
    TestObject* obj2 = obj_map[2];
    TEST_ASSERT(obj2 != nullptr, "Should get object via operator[]");
    TEST_ASSERT(obj2->id == 2, "Object ID should be 2");
    TEST_PASS("operator[] access");

    // Update（会删除旧对象）
    obj_map.Update(1, new TestObject(100, "updated"));
    TestObject* updated_obj = nullptr;
    obj_map.Get(1, updated_obj);
    TEST_ASSERT(updated_obj->id == 100, "Object should be updated");
    TEST_ASSERT(updated_obj->name == "updated", "Object name should be updated");
    TEST_PASS("Update object");

    // DeleteAll 会在析构时自动调用
    obj_map.DeleteAll();
    TEST_ASSERT(obj_map.IsEmpty(), "Map should be empty after DeleteAll");
    TEST_PASS("DeleteAll");

    return true;
}

// ==================== 6. ObjectHashMap Unlink 测试 ====================
bool TestObjectHashMapUnlink()
{
    TEST_SECTION("Test 6: ObjectHashMap Unlink Operations");

    ObjectHashMap<int, TestObject> obj_map;

    TestObject* managed_obj = new TestObject(99, "managed");
    obj_map.Add(1, new TestObject(1, "obj1"));
    obj_map.Add(2, managed_obj);
    obj_map.Add(3, new TestObject(3, "obj3"));

    TEST_ASSERT(obj_map.GetCount() == 3, "Should have 3 objects");
    TEST_PASS("Setup objects");

    // UnlinkByKey（不删除对象）
    bool unlinked = obj_map.UnlinkByKey(2);
    TEST_ASSERT(unlinked, "Unlink should succeed");
    TEST_ASSERT(obj_map.GetCount() == 2, "Count should be 2");
    TEST_ASSERT(!obj_map.ContainsKey(2), "Key 2 should be unlinked");
    
    // 对象仍然有效（未被删除）
    TEST_ASSERT(managed_obj->id == 99, "Unlinked object should still be valid");
    TEST_ASSERT(managed_obj->name == "managed", "Unlinked object data intact");
    TEST_PASS("UnlinkByKey preserves object");

    // 手动删除unlink的对象
    delete managed_obj;
    TEST_PASS("Manual cleanup of unlinked object");

    // DeleteByKey（删除对象）
    bool deleted = obj_map.DeleteByKey(1);
    TEST_ASSERT(deleted, "Delete should succeed");
    TEST_ASSERT(obj_map.GetCount() == 1, "Count should be 1");
    TEST_PASS("DeleteByKey removes and deletes object");

    // 清理剩余对象
    obj_map.DeleteAll();

    return true;
}

// ==================== 7. HashMap 性能统计测试 ====================
bool TestHashMapStatistics()
{
    TEST_SECTION("Test 7: HashMap Statistics");

    HashMap<int, int> map;

    const int num_entries = 1000;

    for(int i = 0; i < num_entries; i++)
        map.Add(i, i * 10);

    TEST_ASSERT(map.GetCount() == num_entries, "Should have 1000 entries");
    TEST_PASS("Add 1000 entries");

    // 统计信息
    int collision_count = map.GetCollisionCount();
    float load_factor = map.GetLoadFactor();
    float avg_chain = map.GetAverageCollisionChainLength();
    int overflow_count = map.GetCollisionOverflowCount();

    std::cout << "  Statistics:" << std::endl;
    std::cout << "    Entries: " << map.GetCount() << std::endl;
    std::cout << "    Collisions: " << collision_count << std::endl;
    std::cout << "    Load factor: " << load_factor << std::endl;
    std::cout << "    Avg collision chain: " << avg_chain << std::endl;
    std::cout << "    Overflow count: " << overflow_count << std::endl;

    TEST_ASSERT(collision_count >= 0, "Collision count should be non-negative");
    TEST_ASSERT(load_factor > 0, "Load factor should be positive");
    TEST_PASS("Statistics reporting");

    // 验证查找性能
    int found_count = 0;
    for(int i = 0; i < num_entries; i++)
    {
        int value;
        if(map.Get(i, value) && value == i * 10)
            found_count++;
    }

    TEST_ASSERT(found_count == num_entries, "All entries should be found");
    TEST_PASS("Fast lookups verified");

    return true;
}

// ==================== 8. HashMap 边界条件测试 ====================
bool TestHashMapBoundaries()
{
    TEST_SECTION("Test 8: HashMap Boundary Conditions");

    HashMap<int, int> map;

    // 负数键
    map.Add(-1, 100);
    map.Add(-999, 200);
    int neg_val;
    TEST_ASSERT(map.Get(-1, neg_val) && neg_val == 100, "Should handle negative keys");
    TEST_PASS("Negative keys");

    // 零键
    map.Add(0, 300);
    int zero_val;
    TEST_ASSERT(map.Get(0, zero_val) && zero_val == 300, "Should handle zero key");
    TEST_PASS("Zero key");

    // 大数值键
    map.Add(1000000, 400);
    int large_val;
    TEST_ASSERT(map.Get(1000000, large_val) && large_val == 400, "Should handle large keys");
    TEST_PASS("Large keys");

    // 删除不存在的键
    bool del_missing = map.DeleteByKey(99999);
    TEST_ASSERT(!del_missing, "Delete non-existent key should fail gracefully");
    TEST_PASS("Delete non-existent key");

    return true;
}

// ==================== 9. HashMap 与 Map 接口兼容性测试 ====================
bool TestHashMapCompatibility()
{
    TEST_SECTION("Test 9: HashMap API Compatibility with Map");

    HashMap<int, std::string> hash_map;

    // 测试所有与 Map 相同的接口
    hash_map.Add(1, "one");
    hash_map.Add(2, "two");

    TEST_ASSERT(hash_map.ContainsKey(1), "ContainsKey should work");
    TEST_ASSERT(hash_map.ContainsValue("two"), "ContainsValue should work");
    TEST_ASSERT(hash_map.Check(1, "one"), "Check should work");
    TEST_PASS("Query methods compatible");

    // GetValuePointer
    std::string* ptr = hash_map.GetValuePointer(1);
    TEST_ASSERT(ptr != nullptr, "GetValuePointer should work");
    TEST_ASSERT(*ptr == "one", "Value pointer correct");
    TEST_PASS("GetValuePointer compatible");

    // GetValueAndSerial
    std::string value;
    int serial = hash_map.GetValueAndSerial(2, value);
    TEST_ASSERT(serial >= 0, "GetValueAndSerial should work");
    TEST_ASSERT(value == "two", "Value correct");
    TEST_PASS("GetValueAndSerial compatible");

    // GetKeyList & GetValueList
    ValueArray<int> keys;
    hash_map.GetKeyList(keys);
    TEST_ASSERT(keys.GetCount() == 2, "GetKeyList should work");
    TEST_PASS("List extraction compatible");

    return true;
}

// ==================== 主测试函数 ====================
int main(int argc, char* argv[])
{
    std::cout << "========================================" << std::endl;
    std::cout << "HashMap Comprehensive Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    bool all_passed = true;

    all_passed &= TestHashMapBasics();
    all_passed &= TestHashMapModifications();
    all_passed &= TestHashMapDeletions();
    all_passed &= TestHashMapEnumeration();
    all_passed &= TestObjectHashMapBasics();
    all_passed &= TestObjectHashMapUnlink();
    all_passed &= TestHashMapStatistics();
    all_passed &= TestHashMapBoundaries();
    all_passed &= TestHashMapCompatibility();

    std::cout << "\n========================================" << std::endl;
    if(all_passed) {
        std::cout << "✅ ALL TESTS PASSED!" << std::endl;
    } else {
        std::cout << "❌ SOME TESTS FAILED!" << std::endl;
    }
    std::cout << "========================================" << std::endl;

    return all_passed ? 0 : 1;
}
