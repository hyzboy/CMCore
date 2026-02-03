/**
 * FlatUnorderedSetShardedSimple.cpp
 * 简单测试 - 验证 ShardedSet 基本功能
 */

#include <hgl/type/ShardedSet.h>
#include <iostream>

using namespace hgl;

int main()
{
    std::cout << "=== FlatUnorderedSet_Sharded 简单测试 ===" << std::endl;

    FlatUnorderedSet_Sharded<int> set;

    std::cout << "\n1. 测试添加元素:" << std::endl;
    bool r1 = set.Add(10);
    bool r2 = set.Add(20);
    bool r3 = set.Add(30);
    bool r4 = set.Add(10);

    std::cout << "   添加 10: " << (r1 ? "成功" : "失败") << std::endl;
    std::cout << "   添加 20: " << (r2 ? "成功" : "失败") << std::endl;
    std::cout << "   添加 30: " << (r3 ? "成功" : "失败") << std::endl;
    std::cout << "   重复添加 10: " << (r4 ? "成功" : "失败（预期）") << std::endl;
    std::cout << "   集合大小: " << set.GetCount() << std::endl;

    std::cout << "\n2. 测试查找元素:" << std::endl;
    std::cout << "   Contains(10): " << (set.Contains(10) ? "是" : "否") << std::endl;
    std::cout << "   Contains(20): " << (set.Contains(20) ? "是" : "否") << std::endl;
    std::cout << "   Contains(99): " << (set.Contains(99) ? "是" : "否（预期）") << std::endl;

    std::cout << "\n3. 枚举所有元素:" << std::endl;
    std::cout << "   ";
    set.Enum([](const int &value) {
        std::cout << value << " ";
    });
    std::cout << std::endl;

    std::cout << "\n4. 测试删除元素:" << std::endl;
    bool d1 = set.Delete(20);
    std::cout << "   删除 20: " << (d1 ? "成功" : "失败") << std::endl;
    std::cout << "   删除后大小: " << set.GetCount() << std::endl;
    std::cout << "   Contains(20): " << (set.Contains(20) ? "是" : "否（预期）") << std::endl;

    std::cout << "\n5. 测试清空:" << std::endl;
    set.Clear();
    std::cout << "   清空后大小: " << set.GetCount() << std::endl;
    std::cout << "   IsEmpty: " << (set.IsEmpty() ? "是" : "否") << std::endl;

    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}
