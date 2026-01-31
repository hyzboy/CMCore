/**
 * UnOrderedValueSetTest01_BasicOperations.cpp
 * 基础操作测试 - 全面测试 FlatUnorderedValueSet 的增删改查
 */

#include<hgl/type/FlatUnorderedValueSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;

void TestAddOperations()
{
    std::cout << "\n=== 测试添加操作 ===" << std::endl;

    FlatUnorderedValueSet<int> set;

    // 单个添加
    assert(set.Add(100) == true);
    assert(set.Add(200) == true);
    assert(set.Add(300) == true);
    assert(set.GetCount() == 3);
    std::cout << "✓ 单个添加成功" << std::endl;

    // 重复添加应失败
    assert(set.Add(100) == false);
    assert(set.GetCount() == 3);
    std::cout << "✓ 重复添加正确拒绝" << std::endl;

    // 批量添加
    int batch[] = {400, 500, 600, 100};  // 100 已存在
    int added = set.Add(batch, 4);
    assert(added == 3);  // 只添加了 3 个新元素
    assert(set.GetCount() == 6);
    std::cout << "✓ 批量添加成功（跳过重复项）" << std::endl;

    // 验证所有元素
    assert(set.Contains(100));
    assert(set.Contains(200));
    assert(set.Contains(300));
    assert(set.Contains(400));
    assert(set.Contains(500));
    assert(set.Contains(600));
    std::cout << "✓ 所有元素验证通过" << std::endl;
}

void TestFindOperations()
{
    std::cout << "\n=== 测试查找操作 ===" << std::endl;

    FlatUnorderedValueSet<int> set;
    set.Add(10);
    set.Add(20);
    set.Add(30);

    // Find 返回索引
    int idx1 = set.Find(10);
    int idx2 = set.Find(20);
    int idx3 = set.Find(30);
    int idx4 = set.Find(99);

    assert(idx1 >= 0);
    assert(idx2 >= 0);
    assert(idx3 >= 0);
    assert(idx4 == -1);
    std::cout << "✓ Find 返回正确索引" << std::endl;

    // Contains 返回布尔值
    assert(set.Contains(10) == true);
    assert(set.Contains(99) == false);
    std::cout << "✓ Contains 返回正确" << std::endl;

    // GetItem 通过索引获取
    int item;

    if(set.Get(idx1,item))
    {
        assert(item == 10);
        std::cout << "✓ GetItem 通过索引获取成功" << std::endl;
    }

    // Get 通过索引获取值
    int value;
    assert(set.Get(idx2, value) == true);
    assert(value == 20);
    assert(set.Get(999, value) == false);
    std::cout << "✓ Get 通过索引获取值成功" << std::endl;
}

void TestDeleteOperations()
{
    std::cout << "\n=== 测试删除操作 ===" << std::endl;

    FlatUnorderedValueSet<int> set;
    for(int i = 1; i <= 10; i++) {
        set.Add(i * 10);
    }
    assert(set.GetCount() == 10);

    // 单个删除
    assert(set.Delete(50) == true);
    assert(set.GetCount() == 9);
    assert(set.Contains(50) == false);
    std::cout << "✓ 单个删除成功" << std::endl;

    // 删除不存在的元素
    assert(set.Delete(999) == false);
    assert(set.GetCount() == 9);
    std::cout << "✓ 删除不存在元素正确处理" << std::endl;

    // 通过索引删除
    int idx = set.Find(30);
    assert(idx >= 0);
    assert(set.DeleteAt(idx) == true);
    assert(set.Contains(30) == false);
    assert(set.GetCount() == 8);
    std::cout << "✓ 通过索引删除成功" << std::endl;

    // 批量删除
    int to_delete[] = {10, 20, 70};
    int deleted = set.Delete(to_delete, 3);
    assert(deleted == 3);
    assert(set.GetCount() == 5);
    assert(!set.Contains(10) && !set.Contains(20) && !set.Contains(70));
    std::cout << "✓ 批量删除成功" << std::endl;
}

void TestGetOperations()
{
    std::cout << "\n=== 测试获取操作 ===" << std::endl;

    FlatUnorderedValueSet<int> set;
    set.Add(111);
    set.Add(222);
    set.Add(333);

    // GetFirst
    int first;
    assert(set.GetFirst(first) == true);
    assert(first == 111 || first == 222 || first == 333);
    std::cout << "✓ GetFirst 成功: " << first << std::endl;

    // GetLast
    int last;
    assert(set.GetLast(last) == true);
    assert(last == 111 || last == 222 || last == 333);
    std::cout << "✓ GetLast 成功: " << last << std::endl;

    // 空集合测试
    FlatUnorderedValueSet<int> empty_set;
    assert(empty_set.GetFirst(first) == false);
    assert(empty_set.GetLast(last) == false);
    std::cout << "✓ 空集合 GetFirst/GetLast 正确返回 false" << std::endl;
}

void TestIteratorOperations()
{
    std::cout << "\n=== 测试迭代器操作 ===" << std::endl;

    FlatUnorderedValueSet<int> set;
    set.Add(10);
    set.Add(20);
    set.Add(30);

    // 使用 range-based for 循环
    std::cout << "   Range-based for 循环: ";
    int count = 0;
    int sum = 0;
    for (const auto& value : set) {
        std::cout << value << " ";
        sum += value;
        count++;
    }
    std::cout << std::endl;
    assert(count == 3);
    assert(sum == 60);
    std::cout << "✓ Range-based for 循环遍历成功" << std::endl;

    // 使用传统迭代器
    std::cout << "   传统迭代器遍历: ";
    count = 0;
    for (auto it = set.begin(); it != set.end(); ++it) {
        std::cout << *it << " ";
        count++;
    }
    std::cout << std::endl;
    assert(count == 3);
    std::cout << "✓ 传统迭代器遍历成功" << std::endl;

    // 使用 cbegin/cend
    std::cout << "   cbegin/cend 遍历: ";
    count = 0;
    for (auto it = set.cbegin(); it != set.cend(); ++it) {
        std::cout << *it << " ";
        count++;
    }
    std::cout << std::endl;
    assert(count == 3);
    std::cout << "✓ cbegin/cend 遍历成功" << std::endl;

    // Enum 枚举
    std::cout << "   Enum 枚举: ";
    int enum_count = 0;
    set.Enum([&](const int& value) {
        std::cout << value << " ";
        enum_count++;
    });
    std::cout << std::endl;
    assert(enum_count == 3);
    std::cout << "✓ Enum 枚举成功" << std::endl;
}

void TestClearOperations()
{
    std::cout << "\n=== 测试清空操作 ===" << std::endl;

    FlatUnorderedValueSet<int> set;
    for(int i = 1; i <= 100; i++) {
        set.Add(i);
    }
    assert(set.GetCount() == 100);

    // Clear（保留内存）
    set.Clear();
    assert(set.GetCount() == 0);
    assert(set.IsEmpty() == true);
    std::cout << "✓ Clear 清空成功" << std::endl;

    // 清空后可以继续添加
    set.Add(999);
    assert(set.GetCount() == 1);
    assert(set.Contains(999) == true);
    std::cout << "✓ 清空后可继续添加" << std::endl;

    // Free（释放内存）
    set.Free();
    assert(set.GetCount() == 0);
    assert(set.IsEmpty() == true);
    std::cout << "✓ Free 释放内存成功" << std::endl;
}

int main()
{
    std::cout << "=== FlatUnorderedValueSet 基础操作测试 ===" << std::endl;

    TestAddOperations();
    TestFindOperations();
    TestDeleteOperations();
    TestGetOperations();
    TestIteratorOperations();
    TestClearOperations();

    std::cout << "\n=== 所有基础操作测试通过 ✓ ===" << std::endl;
    return 0;
}
