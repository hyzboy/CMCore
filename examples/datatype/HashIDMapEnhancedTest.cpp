#include<hgl/type/HashIDMap.h>
#include<iostream>
#include<cassert>
#include<vector>
#include<string>

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

// FNV-1a 哈希函数
uint64 ComputeFNV1aHash(const char* str, int length) {
    uint64 hash = 14695981039346656037ULL;
    for(int i = 0; i < length; i++) {
        hash ^= (uint64)str[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

// ==================== 测试新增的 Remove 功能 ====================
bool TestRemoveBasic()
{
    TEST_SECTION("Test Remove: Basic Operations");

    HashIDMap<4> map;
    uint64 hash1 = ComputeFNV1aHash("test", 4);
    
    // 添加一个映射
    map.Add(hash1, 100);
    int found = map.Find(hash1, [](int id) { return id == 100; });
    TEST_ASSERT(found == 100, "Should find ID 100");
    
    // 删除这个映射
    bool removed = map.Remove(hash1, 100);
    TEST_ASSERT(removed, "Remove should succeed");
    
    // 确认已删除
    int not_found = map.Find(hash1, [](int id) { return id == 100; });
    TEST_ASSERT(not_found == -1, "Should not find ID 100 after removal");
    TEST_ASSERT(map.GetQuickMapCount() == 0, "Quick map should be empty");
    TEST_PASS("Basic remove operation");
    
    return true;
}

bool TestRemoveWithCollision()
{
    TEST_SECTION("Test Remove: With Collision Handling");

    HashIDMap<4> map;
    uint64 collision_hash = 0x1234567890ABCDEFULL;
    
    // 添加多个ID到同一个哈希（模拟碰撞）
    map.Add(collision_hash, 1);
    map.Add(collision_hash, 2);
    map.Add(collision_hash, 3);
    map.Add(collision_hash, 4);
    
    TEST_ASSERT(map.GetCollisionCount() == 1, "Should have 1 collision");
    
    // 删除 quick_map 中的ID（应该提升一个collision slot中的ID）
    bool removed1 = map.Remove(collision_hash, 1);
    TEST_ASSERT(removed1, "Remove ID 1 should succeed");
    
    // 验证ID 2被提升到quick_map
    int found2 = map.Find(collision_hash, [](int id) { return id == 2; });
    TEST_ASSERT(found2 == 2, "ID 2 should be promoted to quick_map");
    
    // 验证ID 3和4仍在collision slot中
    int found3 = map.Find(collision_hash, [](int id) { return id == 3; });
    int found4 = map.Find(collision_hash, [](int id) { return id == 4; });
    TEST_ASSERT(found3 == 3, "ID 3 should still be findable");
    TEST_ASSERT(found4 == 4, "ID 4 should still be findable");
    
    TEST_ASSERT(map.GetCollisionCount() == 1, "Should still have 1 collision");
    TEST_PASS("Remove with promotion from collision slot");
    
    // 删除collision slot中的ID
    bool removed3 = map.Remove(collision_hash, 3);
    TEST_ASSERT(removed3, "Remove ID 3 should succeed");
    
    int not_found3 = map.Find(collision_hash, [](int id) { return id == 3; });
    TEST_ASSERT(not_found3 == -1, "Should not find ID 3 after removal");
    TEST_PASS("Remove from collision slot");
    
    // 继续删除直到collision slot为空
    map.Remove(collision_hash, 4);
    TEST_ASSERT(map.GetCollisionCount() == 0, "Collision slot should be removed when empty");
    
    // 删除最后一个
    map.Remove(collision_hash, 2);
    TEST_ASSERT(map.GetQuickMapCount() == 0, "Map should be empty");
    TEST_PASS("Remove all IDs clears collision and quick maps");
    
    return true;
}

bool TestRemoveNonExistent()
{
    TEST_SECTION("Test Remove: Non-existent IDs");

    HashIDMap<4> map;
    uint64 hash1 = ComputeFNV1aHash("test", 4);
    
    // 尝试删除不存在的映射
    bool removed = map.Remove(hash1, 999);
    TEST_ASSERT(!removed, "Remove non-existent should return false");
    
    // 添加一个ID，然后尝试删除不同的ID
    map.Add(hash1, 100);
    bool removed2 = map.Remove(hash1, 200);
    TEST_ASSERT(!removed2, "Remove different ID should return false");
    
    // 确认原ID仍存在
    int found = map.Find(hash1, [](int id) { return id == 100; });
    TEST_ASSERT(found == 100, "Original ID should still exist");
    TEST_PASS("Handle non-existent ID removal");
    
    return true;
}

// ==================== 测试新增的 Update 功能 ====================
bool TestUpdateBasic()
{
    TEST_SECTION("Test Update: Basic Operations");

    HashIDMap<4> map;
    uint64 hash1 = ComputeFNV1aHash("test", 4);
    
    // 添加一个映射
    map.Add(hash1, 100);
    
    // 更新ID
    bool updated = map.Update(hash1, 100, 200);
    TEST_ASSERT(updated, "Update should succeed");
    
    // 验证旧ID不存在
    int old_not_found = map.Find(hash1, [](int id) { return id == 100; });
    TEST_ASSERT(old_not_found == -1, "Old ID should not be found");
    
    // 验证新ID存在
    int new_found = map.Find(hash1, [](int id) { return id == 200; });
    TEST_ASSERT(new_found == 200, "New ID should be found");
    TEST_PASS("Basic update operation");
    
    return true;
}

bool TestUpdateInQuickMap()
{
    TEST_SECTION("Test Update: In Quick Map");

    HashIDMap<4> map;
    uint64 hash1 = ComputeFNV1aHash("test1", 5);
    uint64 hash2 = ComputeFNV1aHash("test2", 5);
    
    map.Add(hash1, 10);
    map.Add(hash2, 20);
    
    // 更新quick_map中的ID
    bool updated = map.Update(hash1, 10, 15);
    TEST_ASSERT(updated, "Update in quick_map should succeed");
    
    int found = map.Find(hash1, [](int id) { return id == 15; });
    TEST_ASSERT(found == 15, "Updated ID should be found");
    
    // 确保其他映射不受影响
    int found2 = map.Find(hash2, [](int id) { return id == 20; });
    TEST_ASSERT(found2 == 20, "Other mappings should not be affected");
    TEST_PASS("Update in quick map");
    
    return true;
}

bool TestUpdateInCollisionSlot()
{
    TEST_SECTION("Test Update: In Collision Slot");

    HashIDMap<4> map;
    uint64 collision_hash = 0x1234567890ABCDEFULL;
    
    // 创建碰撞
    map.Add(collision_hash, 1);
    map.Add(collision_hash, 2);
    map.Add(collision_hash, 3);
    
    // 更新collision slot中的ID
    bool updated = map.Update(collision_hash, 3, 30);
    TEST_ASSERT(updated, "Update in collision slot should succeed");
    
    // 验证
    int old_not_found = map.Find(collision_hash, [](int id) { return id == 3; });
    TEST_ASSERT(old_not_found == -1, "Old ID should not be found");
    
    int new_found = map.Find(collision_hash, [](int id) { return id == 30; });
    TEST_ASSERT(new_found == 30, "New ID should be found");
    TEST_PASS("Update in collision slot");
    
    return true;
}

bool TestUpdateNonExistent()
{
    TEST_SECTION("Test Update: Non-existent IDs");

    HashIDMap<4> map;
    uint64 hash1 = ComputeFNV1aHash("test", 4);
    
    // 尝试更新不存在的映射
    bool updated = map.Update(hash1, 100, 200);
    TEST_ASSERT(!updated, "Update non-existent hash should return false");
    
    // 添加一个ID，尝试更新不同的旧ID
    map.Add(hash1, 100);
    bool updated2 = map.Update(hash1, 999, 200);
    TEST_ASSERT(!updated2, "Update non-existent old_id should return false");
    
    // 确认原ID仍存在且未改变
    int found = map.Find(hash1, [](int id) { return id == 100; });
    TEST_ASSERT(found == 100, "Original ID should still exist");
    TEST_PASS("Handle non-existent ID update");
    
    return true;
}

// ==================== 测试动态碰撞槽（不再有固定上限） ====================
bool TestDynamicCollisionSlot()
{
    TEST_SECTION("Test Dynamic Collision Slot: No Hard Limit");

    HashIDMap<4> map;  // MAX_COLLISION = 4，但不再是硬限制
    uint64 collision_hash = 0xABCDEF1234567890ULL;
    
    // 添加超过 MAX_COLLISION 的ID数量
    const int num_ids = 10;  // 远超过 MAX_COLLISION
    for(int i = 0; i < num_ids; i++) {
        bool added = map.Add(collision_hash, i);
        TEST_ASSERT(added, ("Add ID " + std::to_string(i) + " should succeed").c_str());
    }
    
    TEST_ASSERT(map.GetCollisionCount() == 1, "Should have 1 collision");
    
    // 验证所有ID都能找到
    for(int i = 0; i < num_ids; i++) {
        int found = map.Find(collision_hash, [i](int id) { return id == i; });
        TEST_ASSERT(found == i, ("Should find ID " + std::to_string(i)).c_str());
    }
    
    // GetCollisionOverflowCount 现在返回超过MAX_COLLISION的碰撞数
    int overflow = map.GetCollisionOverflowCount();
    TEST_ASSERT(overflow == 1, "Should report 1 overflow (chain > MAX_COLLISION)");
    TEST_PASS("Dynamic collision slot handles unlimited IDs");
    
    return true;
}

bool TestDynamicCollisionWithRemove()
{
    TEST_SECTION("Test Dynamic Collision: With Remove");

    HashIDMap<4> map;
    uint64 collision_hash = 0xFEDCBA9876543210ULL;
    
    // 添加很多ID
    for(int i = 0; i < 20; i++) {
        map.Add(collision_hash, i);
    }
    
    // 删除一些ID
    for(int i = 0; i < 10; i++) {
        bool removed = map.Remove(collision_hash, i);
        TEST_ASSERT(removed, ("Remove ID " + std::to_string(i) + " should succeed").c_str());
    }
    
    // 验证删除的ID不存在
    for(int i = 0; i < 10; i++) {
        int found = map.Find(collision_hash, [i](int id) { return id == i; });
        TEST_ASSERT(found == -1, ("ID " + std::to_string(i) + " should not be found").c_str());
    }
    
    // 验证剩余的ID仍存在
    for(int i = 10; i < 20; i++) {
        int found = map.Find(collision_hash, [i](int id) { return id == i; });
        TEST_ASSERT(found == i, ("ID " + std::to_string(i) + " should still be found").c_str());
    }
    
    TEST_PASS("Dynamic collision with remove operations");
    
    return true;
}

// ==================== 测试边界情况 ====================
bool TestRemoveUpdateEdgeCases()
{
    TEST_SECTION("Test Remove/Update: Edge Cases");

    HashIDMap<4> map;
    uint64 hash1 = ComputeFNV1aHash("edge1", 5);
    uint64 hash2 = ComputeFNV1aHash("edge2", 5);
    
    // 边界：更新为相同的ID
    map.Add(hash1, 100);
    bool updated = map.Update(hash1, 100, 100);
    TEST_ASSERT(updated, "Update to same ID should succeed");
    int found = map.Find(hash1, [](int id) { return id == 100; });
    TEST_ASSERT(found == 100, "ID should still be 100");
    
    // 边界：删除后立即添加相同hash的不同ID
    map.Remove(hash1, 100);
    map.Add(hash1, 200);
    int found2 = map.Find(hash1, [](int id) { return id == 200; });
    TEST_ASSERT(found2 == 200, "New ID should be found");
    
    // 边界：在有碰撞的情况下删除quick_map中的ID
    uint64 collision_hash = 0x1111111111111111ULL;
    map.Add(collision_hash, 1);
    map.Add(collision_hash, 2);
    map.Add(collision_hash, 3);
    
    // 删除第一个（quick_map中的）
    map.Remove(collision_hash, 1);
    
    // 验证第二个被提升
    int* quick_id = map.Find(collision_hash, [](int id) { return id == 2; });
    // 注意：这里我们只能通过Find验证，因为GetValuePointer不是公开的
    int found_promoted = map.Find(collision_hash, [](int id) { return id == 2; });
    TEST_ASSERT(found_promoted == 2, "ID 2 should be promoted");
    
    TEST_PASS("Edge cases handled correctly");
    
    return true;
}

// ==================== 测试统计函数的正确性 ====================
bool TestStatisticsWithNewFeatures()
{
    TEST_SECTION("Test Statistics: With Remove/Update");

    HashIDMap<4> map;
    
    // 添加一些数据
    for(int i = 0; i < 10; i++) {
        std::string str = "item" + std::to_string(i);
        uint64 hash = ComputeFNV1aHash(str.c_str(), str.length());
        map.Add(hash, i);
    }
    
    int initial_count = map.GetQuickMapCount();
    TEST_ASSERT(initial_count == 10, "Should have 10 entries");
    
    // 删除一些
    for(int i = 0; i < 5; i++) {
        std::string str = "item" + std::to_string(i);
        uint64 hash = ComputeFNV1aHash(str.c_str(), str.length());
        map.Remove(hash, i);
    }
    
    int after_remove = map.GetQuickMapCount();
    TEST_ASSERT(after_remove == 5, "Should have 5 entries after removal");
    
    // 添加碰撞数据
    uint64 collision_hash = 0xAAAAAAAAAAAAAAAAULL;
    for(int i = 0; i < 8; i++) {
        map.Add(collision_hash, 100 + i);
    }
    
    TEST_ASSERT(map.GetCollisionCount() == 1, "Should have 1 collision");
    float avg_chain = map.GetAverageCollisionChainLength();
    TEST_ASSERT(avg_chain == 8.0f, "Average chain length should be 8.0");
    
    // 删除一些碰撞ID
    map.Remove(collision_hash, 100);
    map.Remove(collision_hash, 101);
    
    float new_avg_chain = map.GetAverageCollisionChainLength();
    TEST_ASSERT(new_avg_chain == 6.0f, "Average chain length should be 6.0 after removal");
    
    TEST_PASS("Statistics work correctly with new features");
    
    return true;
}

// ==================== 主测试函数 ====================
int main(int argc, char* argv[])
{
    std::cout << "========================================" << std::endl;
    std::cout << "HashIDMap Enhanced Features Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    bool all_passed = true;

    // 测试 Remove 功能
    all_passed &= TestRemoveBasic();
    all_passed &= TestRemoveWithCollision();
    all_passed &= TestRemoveNonExistent();
    
    // 测试 Update 功能
    all_passed &= TestUpdateBasic();
    all_passed &= TestUpdateInQuickMap();
    all_passed &= TestUpdateInCollisionSlot();
    all_passed &= TestUpdateNonExistent();
    
    // 测试动态碰撞槽
    all_passed &= TestDynamicCollisionSlot();
    all_passed &= TestDynamicCollisionWithRemove();
    
    // 测试边界情况
    all_passed &= TestRemoveUpdateEdgeCases();
    
    // 测试统计函数
    all_passed &= TestStatisticsWithNewFeatures();

    // 总结
    std::cout << "\n========================================" << std::endl;
    if(all_passed) {
        std::cout << "✅ ALL ENHANCED TESTS PASSED!" << std::endl;
    } else {
        std::cout << "❌ SOME ENHANCED TESTS FAILED!" << std::endl;
    }
    std::cout << "========================================" << std::endl;

    return all_passed ? 0 : 1;
}
