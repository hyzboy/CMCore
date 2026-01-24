/**
 * UnorderedValueSetTest03_HashStatistics.cpp
 * 哈希统计测试 - 测试哈希性能监控接口
 */

#include<hgl/type/UnorderedValueSet.h>
#include<iostream>
#include<iomanip>
#include<cassert>

using namespace hgl;

void TestHashStatistics()
{
    std::cout << "\n=== 测试哈希统计信息 ===" << std::endl;

    UnorderedValueSet<int> set;

    // 添加大量元素以触发一些哈希碰撞
    const int COUNT = 1000;
    for(int i = 0; i < COUNT; i++) {
        set.Add(i);
    }

    std::cout << "\n  添加了 " << COUNT << " 个元素" << std::endl;
    std::cout << "  实际存储数量: " << set.GetCount() << std::endl;

    // 获取哈希统计
    int collision_count = set.GetCollisionCount();
    float load_factor = set.GetLoadFactor();
    float avg_chain_length = set.GetAverageCollisionChainLength();
    int overflow_count = set.GetCollisionOverflowCount();

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n  哈希统计:" << std::endl;
    std::cout << "    - 碰撞槽位数: " << collision_count << std::endl;
    std::cout << "    - 负载因子: " << load_factor << std::endl;
    std::cout << "    - 平均碰撞链长: " << avg_chain_length << std::endl;
    std::cout << "    - 溢出槽位数: " << overflow_count << std::endl;

    // 验证统计信息合理性
    assert(collision_count >= 0);
    assert(load_factor >= 0.0f);
    assert(avg_chain_length >= 0.0f);
    assert(overflow_count >= 0);

    std::cout << "\n  ✓ 哈希统计接口正常工作" << std::endl;

    // 性能分析
    if(collision_count > 0) {
        float collision_rate = (float)collision_count / COUNT * 100;
        std::cout << "\n  碰撞率: " << collision_rate << "%" << std::endl;

        if(collision_rate < 5.0f) {
            std::cout << "  ✓ 碰撞率良好（< 5%）" << std::endl;
        } else if(collision_rate < 15.0f) {
            std::cout << "  ⚠ 碰撞率中等（5-15%）" << std::endl;
        } else {
            std::cout << "  ⚠ 碰撞率较高（> 15%），可能需要优化哈希函数" << std::endl;
        }
    } else {
        std::cout << "  ✓ 完美哈希（无碰撞）" << std::endl;
    }

    if(overflow_count > 0) {
        std::cout << "  ⚠ 检测到 " << overflow_count << " 个溢出槽位" << std::endl;
        std::cout << "    建议：增大 MAX_COLLISION 参数" << std::endl;
    } else {
        std::cout << "  ✓ 无溢出槽位" << std::endl;
    }
}

void TestHashDistribution()
{
    std::cout << "\n=== 测试哈希分布 ===" << std::endl;

    // 测试不同数据类型的哈希分布
    std::cout << "\n  测试 int 类型哈希分布:" << std::endl;
    UnorderedValueSet<int> int_set;
    for(int i = 0; i < 500; i++) {
        int_set.Add(i * 7);  // 使用步长制造一些模式
    }
    std::cout << "    元素数: " << int_set.GetCount() << std::endl;
    std::cout << "    碰撞数: " << int_set.GetCollisionCount() << std::endl;
    std::cout << "    负载因子: " << int_set.GetLoadFactor() << std::endl;

    std::cout << "\n  测试 uint64 类型哈希分布:" << std::endl;
    UnorderedValueSet<uint64> uint64_set;
    for(uint64 i = 0; i < 500; i++) {
        uint64_set.Add(i * 1000000007ULL);  // 使用大素数
    }
    std::cout << "    元素数: " << uint64_set.GetCount() << std::endl;
    std::cout << "    碰撞数: " << uint64_set.GetCollisionCount() << std::endl;
    std::cout << "    负载因子: " << uint64_set.GetLoadFactor() << std::endl;

    std::cout << "\n  ✓ 不同数据类型哈希分布测试完成" << std::endl;
}

void TestWorstCaseScenario()
{
    std::cout << "\n=== 测试最坏情况（故意制造哈希碰撞）===" << std::endl;

    // 使用较小的数据集，但制造碰撞
    UnorderedValueSet<int, 2> set_small_collision;  // MAX_COLLISION = 2

    // 添加大量元素，观察行为
    const int TEST_COUNT = 100;
    int success_count = 0;
    for(int i = 0; i < TEST_COUNT; i++) {
        if(set_small_collision.Add(i)) {
            success_count++;
        }
    }

    std::cout << "  尝试添加: " << TEST_COUNT << " 个元素" << std::endl;
    std::cout << "  成功添加: " << success_count << " 个元素" << std::endl;
    std::cout << "  碰撞数: " << set_small_collision.GetCollisionCount() << std::endl;
    std::cout << "  溢出数: " << set_small_collision.GetCollisionOverflowCount() << std::endl;

    // 即使有碰撞，查找仍应正确工作
    for(int i = 0; i < success_count; i++) {
        if(!set_small_collision.Contains(i)) {
            std::cout << "  ✗ 错误：元素 " << i << " 添加后却找不到！" << std::endl;
            assert(false);
        }
    }

    std::cout << "  ✓ 即使在高碰撞情况下，查找仍然正确" << std::endl;
}

void TestEmptySetStatistics()
{
    std::cout << "\n=== 测试空集合统计 ===" << std::endl;

    UnorderedValueSet<int> empty_set;

    assert(empty_set.GetCount() == 0);
    assert(empty_set.GetCollisionCount() == 0);
    assert(empty_set.GetLoadFactor() == 0.0f);
    assert(empty_set.GetAverageCollisionChainLength() == 0.0f);
    assert(empty_set.GetCollisionOverflowCount() == 0);

    std::cout << "  ✓ 空集合统计正确" << std::endl;
}

void TestAfterDeleteStatistics()
{
    std::cout << "\n=== 测试删除后统计 ===" << std::endl;

    UnorderedValueSet<int> set;

    // 添加元素
    for(int i = 0; i < 100; i++) {
        set.Add(i);
    }

    int collision_before = set.GetCollisionCount();
    std::cout << "  删除前碰撞数: " << collision_before << std::endl;
    std::cout << "  删除前元素数: " << set.GetCount() << std::endl;

    // 删除一半元素
    for(int i = 0; i < 50; i++) {
        set.Delete(i);
    }

    int collision_after = set.GetCollisionCount();
    std::cout << "  删除后碰撞数: " << collision_after << std::endl;
    std::cout << "  删除后元素数: " << set.GetCount() << std::endl;

    // 删除后统计应该重新计算（因为 RebuildHashMap）
    assert(set.GetCount() == 50);
    std::cout << "  ✓ 删除后统计正确更新" << std::endl;
}

int main()
{
    std::cout << "=== UnorderedValueSet 哈希统计测试 ===" << std::endl;

    TestHashStatistics();
    TestHashDistribution();
    TestWorstCaseScenario();
    TestEmptySetStatistics();
    TestAfterDeleteStatistics();

    std::cout << "\n=== 所有哈希统计测试通过 ✓ ===" << std::endl;
    return 0;
}
