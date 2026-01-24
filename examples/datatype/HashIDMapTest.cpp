#include<hgl/type/HashIDMap.h>
#include<iostream>
#include<cassert>
#include<vector>
#include<string>
#include<chrono>
#include<random>
#include<unordered_set>
#include<algorithm>

using namespace hgl;
using namespace std;

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

// 测试数据结构
struct TestData {
    int id;
    std::string value;

    TestData(int i, const std::string& v) : id(i), value(v) {}
};

// FNV-1a 哈希函数（与 ConstStringSet 一致）
uint64 ComputeHash(const char* str, int length) {
    uint64 hash = 14695981039346656037ULL;
    for(int i = 0; i < length; i++) {
        hash ^= (uint64)str[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

// ==================== 1. 基础功能测试 ====================
bool TestBasicOperations()
{
    TEST_SECTION("Test 1: Basic Operations");

    HashIDMap<4> map;

    // 测试空映射
    TEST_ASSERT(map.IsEmpty(), "New map should be empty");
    TEST_ASSERT(map.GetQuickMapCount() == 0, "Empty map quick count should be 0");
    TEST_ASSERT(map.GetCollisionCount() == 0, "Empty map collision count should be 0");
    TEST_PASS("Empty map initialization");

    // 添加第一个映射
    uint64 hash1 = ComputeHash("hello", 5);
    bool added1 = map.Add(hash1, 0);
    TEST_ASSERT(added1, "First add should succeed");
    TEST_ASSERT(!map.IsEmpty(), "Map should not be empty after adding");
    TEST_ASSERT(map.GetQuickMapCount() == 1, "Quick map count should be 1");
    TEST_PASS("Add first mapping");

    // 查找第一个映射
    std::vector<TestData> data;
    data.emplace_back(0, "hello");

    auto verify_func = [&](int id) -> bool {
        return id >= 0 && id < (int)data.size() && data[id].value == "hello";
    };

    int found_id = map.Find(hash1, verify_func);
    TEST_ASSERT(found_id == 0, "Should find ID 0");
    TEST_PASS("Find existing mapping");

    // 添加第二个不同哈希的映射
    uint64 hash2 = ComputeHash("world", 5);
    bool added2 = map.Add(hash2, 1);
    data.emplace_back(1, "world");

    TEST_ASSERT(added2, "Second add should succeed");
    TEST_ASSERT(map.GetQuickMapCount() == 2, "Quick map count should be 2");
    TEST_ASSERT(map.GetCollisionCount() == 0, "No collisions yet");
    TEST_PASS("Add second mapping with different hash");

    // 查找两个映射
    int found1 = map.Find(hash1, [&](int id) { return data[id].value == "hello"; });
    int found2 = map.Find(hash2, [&](int id) { return data[id].value == "world"; });
    TEST_ASSERT(found1 == 0, "Should find hello at ID 0");
    TEST_ASSERT(found2 == 1, "Should find world at ID 1");
    TEST_PASS("Find multiple mappings");

    // 测试未找到的情况
    uint64 hash_missing = ComputeHash("missing", 7);
    int not_found = map.Find(hash_missing, [&](int id) { return false; });
    TEST_ASSERT(not_found == -1, "Should not find non-existent hash");
    TEST_PASS("Handle missing hash");

    return true;
}

// ==================== 2. 哈希碰撞测试 ====================
bool TestHashCollisions()
{
    TEST_SECTION("Test 2: Hash Collision Handling");

    HashIDMap<4> map;
    std::vector<TestData> data;

    // 使用相同的哈希值添加多个ID（模拟碰撞）
    uint64 collision_hash = 0x1234567890ABCDEFULL;

    // 第一个ID
    bool added1 = map.Add(collision_hash, 0);
    data.emplace_back(0, "string0");
    TEST_ASSERT(added1, "First collision add should succeed");
    TEST_ASSERT(map.GetCollisionCount() == 0, "No collision yet");
    TEST_PASS("Add first ID to collision hash");

    // 第二个ID（触发碰撞）
    bool added2 = map.Add(collision_hash, 1);
    data.emplace_back(1, "string1");
    TEST_ASSERT(added2, "Second collision add should succeed");
    TEST_ASSERT(map.GetCollisionCount() == 1, "Should have 1 collision");
    TEST_PASS("Add second ID triggers collision");

    // 第三个和第四个ID
    bool added3 = map.Add(collision_hash, 2);
    bool added4 = map.Add(collision_hash, 3);
    data.emplace_back(2, "string2");
    data.emplace_back(3, "string3");
    TEST_ASSERT(added3 && added4, "Third and fourth adds should succeed");
    TEST_PASS("Add more IDs to collision slot");

    // 第五个ID（应该失败，因为槽位满了）
    bool added5 = map.Add(collision_hash, 4);
    TEST_ASSERT(!added5, "Fifth add should fail (slot full)");
    TEST_ASSERT(map.GetCollisionOverflowCount() == 1, "Should have 1 overflow");
    TEST_PASS("Detect collision slot overflow");

    // 验证所有已添加的ID都能正确查找
    for(int i = 0; i < 4; i++) {
        int found = map.Find(collision_hash, [&](int id) {
            return id == i && data[id].value == ("string" + std::to_string(i));
        });
        TEST_ASSERT(found == i, ("Should find ID " + std::to_string(i)).c_str());
    }
    TEST_PASS("Find all IDs in collision slot");

    // 验证第五个ID确实没有被添加
    int found5 = map.Find(collision_hash, [&](int id) { return id == 4; });
    TEST_ASSERT(found5 == -1, "Should not find ID 4 (was not added)");
    TEST_PASS("Verify overflow ID not added");

    return true;
}

// ==================== 3. 清空和重置测试 ====================
bool TestClearAndReset()
{
    TEST_SECTION("Test 3: Clear and Reset");

    HashIDMap<4> map;

    // 添加一些映射
    for(int i = 0; i < 10; i++) {
        std::string str = "test" + std::to_string(i);
        uint64 hash = ComputeHash(str.c_str(), str.length());
        map.Add(hash, i);
    }

    TEST_ASSERT(!map.IsEmpty(), "Map should not be empty");
    TEST_ASSERT(map.GetQuickMapCount() > 0, "Should have entries");
    TEST_PASS("Add multiple entries");

    // 清空
    map.Clear();
    TEST_ASSERT(map.IsEmpty(), "Map should be empty after clear");
    TEST_ASSERT(map.GetQuickMapCount() == 0, "Quick map should be empty");
    TEST_ASSERT(map.GetCollisionCount() == 0, "Collision map should be empty");
    TEST_PASS("Clear operation");

    // 清空后再添加
    uint64 hash = ComputeHash("new", 3);
    bool added = map.Add(hash, 100);
    TEST_ASSERT(added, "Should be able to add after clear");
    int found = map.Find(hash, [](int id) { return id == 100; });
    TEST_ASSERT(found == 100, "Should find new entry after clear");
    TEST_PASS("Add after clear");

    return true;
}

// ==================== 4. 统计接口测试 ====================
bool TestStatistics()
{
    TEST_SECTION("Test 4: Statistics Interface");

    HashIDMap<4> map;
    std::vector<std::pair<uint64, int>> entries;

    // 添加10个不同的哈希值
    for(int i = 0; i < 10; i++) {
        std::string str = "unique" + std::to_string(i);
        uint64 hash = ComputeHash(str.c_str(), str.length());
        map.Add(hash, i);
        entries.emplace_back(hash, i);
    }

    TEST_ASSERT(map.GetQuickMapCount() == 10, "Should have 10 quick entries");
    TEST_ASSERT(map.GetCollisionCount() == 0, "No collisions with unique hashes");
    float load_factor = map.GetLoadFactor(10);
    TEST_ASSERT(load_factor == 1.0f, "Load factor should be 1.0");
    TEST_PASS("Statistics with no collisions");

    // 添加碰撞数据（相同哈希）
    uint64 collision_hash = 0xABCDEF1234567890ULL;
    for(int i = 10; i < 14; i++) {
        map.Add(collision_hash, i);
    }

    TEST_ASSERT(map.GetCollisionCount() == 1, "Should have 1 collision");
    TEST_ASSERT(map.GetAverageCollisionChainLength() == 4.0f, "Average chain length should be 4.0");
    TEST_PASS("Statistics with collisions");

    // 测试负载因子
    float new_load_factor = map.GetLoadFactor(14);
    TEST_ASSERT(new_load_factor > 1.0f, "Load factor should be > 1.0 with collisions");
    std::cout << "  Load factor: " << new_load_factor << std::endl;
    TEST_PASS("Load factor calculation");

    return true;
}

// ==================== 5. 边界条件测试 ====================
bool TestBoundaryConditions()
{
    TEST_SECTION("Test 5: Boundary Conditions");

    HashIDMap<4> map;

    // 测试负ID
    uint64 hash1 = ComputeHash("negative", 8);
    map.Add(hash1, -1);
    int found_neg = map.Find(hash1, [](int id) { return id == -1; });
    TEST_ASSERT(found_neg == -1, "Should find negative ID");
    TEST_PASS("Handle negative ID");

    // 测试零ID
    uint64 hash2 = ComputeHash("zero", 4);
    map.Add(hash2, 0);
    int found_zero = map.Find(hash2, [](int id) { return id == 0; });
    TEST_ASSERT(found_zero == 0, "Should find zero ID");
    TEST_PASS("Handle zero ID");

    // 测试大ID
    uint64 hash3 = ComputeHash("large", 5);
    map.Add(hash3, 1000000);
    int found_large = map.Find(hash3, [](int id) { return id == 1000000; });
    TEST_ASSERT(found_large == 1000000, "Should find large ID");
    TEST_PASS("Handle large ID");

    // 测试哈希值为0
    uint64 hash_zero = 0;
    map.Add(hash_zero, 99);
    int found_hash_zero = map.Find(hash_zero, [](int id) { return id == 99; });
    TEST_ASSERT(found_hash_zero == 99, "Should handle hash value 0");
    TEST_PASS("Handle hash value 0");

    // 测试最大哈希值
    uint64 hash_max = UINT64_MAX;
    map.Add(hash_max, 88);
    int found_hash_max = map.Find(hash_max, [](int id) { return id == 88; });
    TEST_ASSERT(found_hash_max == 88, "Should handle max hash value");
    TEST_PASS("Handle max hash value");

    return true;
}

// ==================== 6. 验证函数测试 ====================
bool TestVerifyFunction()
{
    TEST_SECTION("Test 6: Verify Function Variants");

    HashIDMap<4> map;
    std::vector<TestData> data;

    // 添加测试数据
    for(int i = 0; i < 5; i++) {
        std::string str = "item" + std::to_string(i);
        uint64 hash = ComputeHash(str.c_str(), str.length());
        map.Add(hash, i);
        data.emplace_back(i, str);
    }

    // 测试简单验证函数
    uint64 hash0 = ComputeHash("item0", 5);
    int found0 = map.Find(hash0, [](int id) { return id == 0; });
    TEST_ASSERT(found0 == 0, "Simple lambda should work");
    TEST_PASS("Simple verify function");

    // 测试复杂验证函数（捕获外部变量）
    std::string target = "item2";
    uint64 hash2 = ComputeHash("item2", 5);
    int found2 = map.Find(hash2, [&](int id) {
        return id >= 0 && id < (int)data.size() && data[id].value == target;
    });
    TEST_ASSERT(found2 == 2, "Complex verify function should work");
    TEST_PASS("Complex verify function with capture");

    // 测试总是返回false的验证函数
    int not_found = map.Find(hash0, [](int id) { return false; });
    TEST_ASSERT(not_found == -1, "Always-false verify should not find");
    TEST_PASS("Always-false verify function");

    // 测试多条件验证函数
    uint64 hash3 = ComputeHash("item3", 5);
    int found3 = map.Find(hash3, [&](int id) {
        return id > 2 && id < 5 && data[id].value.find("item") == 0;
    });
    TEST_ASSERT(found3 == 3, "Multi-condition verify should work");
    TEST_PASS("Multi-condition verify function");

    return true;
}

// ==================== 7. 性能测试 ====================
bool TestPerformance()
{
    TEST_SECTION("Test 7: Performance Test");

    HashIDMap<4> map;
    std::vector<std::pair<uint64, int>> entries;

    const int num_entries = 10000;

    // 生成测试数据
    std::cout << "  Generating " << num_entries << " test entries..." << std::endl;
    for(int i = 0; i < num_entries; i++) {
        std::string str = "performance_test_string_" + std::to_string(i);
        uint64 hash = ComputeHash(str.c_str(), str.length());
        entries.emplace_back(hash, i);
    }

    // 测试添加性能
    auto start_add = std::chrono::high_resolution_clock::now();
    for(const auto& entry : entries) {
        map.Add(entry.first, entry.second);
    }
    auto end_add = std::chrono::high_resolution_clock::now();
    auto duration_add = std::chrono::duration_cast<std::chrono::microseconds>(end_add - start_add);

    std::cout << "  Added " << num_entries << " entries in " << duration_add.count() << " μs" << std::endl;
    std::cout << "  Average add time: " << (double)duration_add.count() / num_entries << " μs per entry" << std::endl;
    TEST_PASS("Add performance");

    // 测试查找性能
    auto start_find = std::chrono::high_resolution_clock::now();
    int found_count = 0;
    for(const auto& entry : entries) {
        int found = map.Find(entry.first, [&](int id) { return id == entry.second; });
        if(found == entry.second)
            found_count++;
    }
    auto end_find = std::chrono::high_resolution_clock::now();
    auto duration_find = std::chrono::duration_cast<std::chrono::microseconds>(end_find - start_find);

    TEST_ASSERT(found_count == num_entries, "All entries should be found");
    std::cout << "  Found " << found_count << " entries in " << duration_find.count() << " μs" << std::endl;
    std::cout << "  Average find time: " << (double)duration_find.count() / num_entries << " μs per entry" << std::endl;
    TEST_PASS("Find performance");

    // 输出统计信息
    std::cout << "\n  Statistics:" << std::endl;
    std::cout << "    Quick map count: " << map.GetQuickMapCount() << std::endl;
    std::cout << "    Collision count: " << map.GetCollisionCount() << std::endl;
    std::cout << "    Collision rate: " << (100.0f * map.GetCollisionCount() / map.GetQuickMapCount()) << "%" << std::endl;
    std::cout << "    Avg collision chain: " << map.GetAverageCollisionChainLength() << std::endl;
    std::cout << "    Overflow count: " << map.GetCollisionOverflowCount() << std::endl;
    std::cout << "    Load factor: " << map.GetLoadFactor(num_entries) << std::endl;

    return true;
}

// ==================== 8. 压力测试 - 随机数据 ====================
bool TestStressRandom()
{
    TEST_SECTION("Test 8: Stress Test - Random Data");

    HashIDMap<8> map;  // 使用更大的碰撞槽位
    std::vector<std::pair<uint64, int>> entries;

    const int num_entries = 50000;

    // 生成随机数据
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64> hash_dist;
    std::uniform_int_distribution<int> id_dist(0, 1000000);

    std::cout << "  Generating " << num_entries << " random entries..." << std::endl;
    std::unordered_set<uint64> used_hashes;

    for(int i = 0; i < num_entries; i++) {
        uint64 hash;
        do {
            hash = hash_dist(gen);
        } while(used_hashes.count(hash) > 0);  // 确保唯一

        used_hashes.insert(hash);
        int id = i;
        entries.emplace_back(hash, id);
    }

    // 批量添加
    std::cout << "  Adding entries..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    int success_count = 0;
    for(const auto& entry : entries) {
        if(map.Add(entry.first, entry.second))
            success_count++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "  Successfully added " << success_count << " / " << num_entries
              << " entries in " << duration.count() << " ms" << std::endl;
    TEST_PASS("Random data addition");

    // 验证查找
    std::cout << "  Verifying lookups..." << std::endl;
    int verify_count = 0;
    for(const auto& entry : entries) {
        int found = map.Find(entry.first, [&](int id) { return id == entry.second; });
        if(found == entry.second)
            verify_count++;
    }

    TEST_ASSERT(verify_count >= success_count * 0.99, "At least 99% should be findable");
    std::cout << "  Verified " << verify_count << " / " << success_count << " entries" << std::endl;
    TEST_PASS("Random data verification");

    // 统计信息
    std::cout << "\n  Stress Test Statistics:" << std::endl;
    std::cout << "    Quick map count: " << map.GetQuickMapCount() << std::endl;
    std::cout << "    Collision count: " << map.GetCollisionCount() << std::endl;
    std::cout << "    Collision rate: " << (100.0f * map.GetCollisionCount() / map.GetQuickMapCount()) << "%" << std::endl;
    std::cout << "    Avg collision chain: " << map.GetAverageCollisionChainLength() << std::endl;
    std::cout << "    Overflow count: " << map.GetCollisionOverflowCount() << std::endl;
    std::cout << "    Load factor: " << map.GetLoadFactor(num_entries) << std::endl;

    return true;
}

// ==================== 9. 压力测试 - 碰撞场景 ====================
bool TestStressCollisions()
{
    TEST_SECTION("Test 9: Stress Test - Collision Scenarios");

    HashIDMap<16> map;  // 大碰撞槽位

    const int num_collision_groups = 1000;
    const int ids_per_group = 10;

    std::cout << "  Creating " << num_collision_groups << " collision groups..." << std::endl;
    std::cout << "  Each group has " << ids_per_group << " IDs with same hash" << std::endl;

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64> hash_dist;

    int total_added = 0;
    int total_overflow = 0;

    auto start = std::chrono::high_resolution_clock::now();

    for(int group = 0; group < num_collision_groups; group++) {
        uint64 collision_hash = hash_dist(gen);

        for(int id = 0; id < ids_per_group; id++) {
            int actual_id = group * ids_per_group + id;
            bool added = map.Add(collision_hash, actual_id);
            if(added)
                total_added++;
            else
                total_overflow++;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "  Total added: " << total_added << std::endl;
    std::cout << "  Total overflow: " << total_overflow << std::endl;
    std::cout << "  Time: " << duration.count() << " ms" << std::endl;

    TEST_ASSERT(total_added >= num_collision_groups, "At least one per group should be added");
    TEST_PASS("Collision stress test completion");

    // 统计信息
    std::cout << "\n  Collision Stress Statistics:" << std::endl;
    std::cout << "    Quick map count: " << map.GetQuickMapCount() << std::endl;
    std::cout << "    Collision count: " << map.GetCollisionCount() << std::endl;
    std::cout << "    Avg collision chain: " << map.GetAverageCollisionChainLength() << std::endl;
    std::cout << "    Overflow count: " << map.GetCollisionOverflowCount() << std::endl;

    return true;
}

// ==================== 10. 综合场景测试 ====================
bool TestRealWorldScenario()
{
    TEST_SECTION("Test 10: Real-World Scenario");

    HashIDMap<4> map;
    std::vector<std::string> strings;

    // 模拟实际字符串集合（类似文件路径、标识符等）
    const char* prefixes[] = {
        "src/", "include/", "test/", "build/", "doc/",
        "lib/", "bin/", "res/", "config/", "script/"
    };

    const char* names[] = {
        "main", "utils", "core", "test", "helper",
        "manager", "handler", "service", "controller", "model"
    };

    const char* extensions[] = {
        ".cpp", ".h", ".hpp", ".c", ".txt",
        ".md", ".json", ".xml", ".yaml", ".conf"
    };

    std::cout << "  Generating realistic string dataset..." << std::endl;

    // 生成组合字符串
    for(const auto* prefix : prefixes) {
        for(const auto* name : names) {
            for(const auto* ext : extensions) {
                std::string str = std::string(prefix) + name + ext;
                strings.push_back(str);
            }
        }
    }

    std::cout << "  Generated " << strings.size() << " strings" << std::endl;

    // 添加到映射
    for(size_t i = 0; i < strings.size(); i++) {
        uint64 hash = ComputeHash(strings[i].c_str(), strings[i].length());
        map.Add(hash, (int)i);
    }

    TEST_PASS("Add realistic string set");

    // 随机查找测试
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, strings.size() - 1);

    const int num_lookups = 10000;
    int found_count = 0;

    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < num_lookups; i++) {
        size_t idx = dist(gen);
        const std::string& target = strings[idx];
        uint64 hash = ComputeHash(target.c_str(), target.length());

        int found = map.Find(hash, [&](int id) {
            return id >= 0 && id < (int)strings.size() && strings[id] == target;
        });

        if(found == (int)idx)
            found_count++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    TEST_ASSERT(found_count == num_lookups, "All lookups should succeed");
    std::cout << "  Completed " << num_lookups << " random lookups in "
              << duration.count() << " μs" << std::endl;
    std::cout << "  Average lookup time: " << (double)duration.count() / num_lookups
              << " μs" << std::endl;
    TEST_PASS("Random lookup performance");

    // 统计信息
    std::cout << "\n  Real-World Scenario Statistics:" << std::endl;
    std::cout << "    Total strings: " << strings.size() << std::endl;
    std::cout << "    Quick map count: " << map.GetQuickMapCount() << std::endl;
    std::cout << "    Collision count: " << map.GetCollisionCount() << std::endl;
    std::cout << "    Collision rate: " << (100.0f * map.GetCollisionCount() / map.GetQuickMapCount()) << "%" << std::endl;
    std::cout << "    Load factor: " << map.GetLoadFactor(strings.size()) << std::endl;

    return true;
}

// ==================== 主测试函数 ====================
int main(int argc, char* argv[])
{
    std::cout << "========================================" << std::endl;
    std::cout << "HashIDMap Comprehensive Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    bool all_passed = true;

    // 运行所有测试
    all_passed &= TestBasicOperations();
    all_passed &= TestHashCollisions();
    all_passed &= TestClearAndReset();
    all_passed &= TestStatistics();
    all_passed &= TestBoundaryConditions();
    all_passed &= TestVerifyFunction();
    all_passed &= TestPerformance();
    all_passed &= TestStressRandom();
    all_passed &= TestStressCollisions();
    all_passed &= TestRealWorldScenario();

    // 总结
    std::cout << "\n========================================" << std::endl;
    if(all_passed) {
        std::cout << "✅ ALL TESTS PASSED!" << std::endl;
    } else {
        std::cout << "❌ SOME TESTS FAILED!" << std::endl;
    }
    std::cout << "========================================" << std::endl;

    return all_passed ? 0 : 1;
}
