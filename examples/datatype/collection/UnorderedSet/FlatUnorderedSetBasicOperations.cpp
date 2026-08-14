/**
 * UnOrderedValueSetTest01_BasicOperations.cpp
 * 基础操作测试 - 全面测试 FlatUnorderedSet 的增删改查
 */

#include<hgl/type/FlatUnorderedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;

void TestAddOperations()
{
    std::cout << "\n=== Test Add operations ===" << std::endl;

    FlatUnorderedSet<int> set;

    // 单个添加
    assert(set.Add(100) == true);
    assert(set.Add(200) == true);
    assert(set.Add(300) == true);
    assert(set.GetCount() == 3);
    std::cout << "✓ Single add succeeded" << std::endl;

    // 重复添加应失败
    assert(set.Add(100) == false);
    assert(set.GetCount() == 3);
    std::cout << "✓ Duplicate add correctly rejected" << std::endl;

    // 批量添加
    int batch[] = {400, 500, 600, 100};  // 100 已存在
    int added = set.Add(batch, 4);
    assert(added == 3);  // 只添加了 3 个新元素
    assert(set.GetCount() == 6);
    std::cout << "✓ Batch add succeeded (duplicates skipped)" << std::endl;

    // 验证所有元素
    assert(set.Contains(100));
    assert(set.Contains(200));
    assert(set.Contains(300));
    assert(set.Contains(400));
    assert(set.Contains(500));
    assert(set.Contains(600));
    std::cout << "✓ All elements verified" << std::endl;
}

void TestFindOperations()
{
    std::cout << "\n=== Test Find operations ===" << std::endl;

    FlatUnorderedSet<int> set;
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
    std::cout << "✓ Find returns correct index" << std::endl;

    // Contains 返回布尔值
    assert(set.Contains(10) == true);
    assert(set.Contains(99) == false);
    std::cout << "✓ Contains returns correct" << std::endl;

    // GetItem 通过索引获取
    int item;

    if(set.Get(idx1,item))
    {
        assert(item == 10);
        std::cout << "✓ GetItem by index succeeded" << std::endl;
    }

    // Get 通过索引获取值
    int value;
    assert(set.Get(idx2, value) == true);
    assert(value == 20);
    assert(set.Get(999, value) == false);
    std::cout << "✓ Get value by index succeeded" << std::endl;
}

void TestDeleteOperations()
{
    std::cout << "\n=== Test Delete operations ===" << std::endl;

    FlatUnorderedSet<int> set;
    for(int i = 1; i <= 10; i++) {
        set.Add(i * 10);
    }
    assert(set.GetCount() == 10);

    // 单个删除
    assert(set.Delete(50) == true);
    assert(set.GetCount() == 9);
    assert(set.Contains(50) == false);
    std::cout << "✓ Single delete succeeded" << std::endl;

    // 删除不存在的元素
    assert(set.Delete(999) == false);
    assert(set.GetCount() == 9);
    std::cout << "✓ Delete of missing element handled correctly" << std::endl;

    // 通过索引删除
    int idx = set.Find(30);
    assert(idx >= 0);
    assert(set.DeleteAt(idx) == true);
    assert(set.Contains(30) == false);
    assert(set.GetCount() == 8);
    std::cout << "✓ Delete by index succeeded" << std::endl;

    // 批量删除
    int to_delete[] = {10, 20, 70};
    int deleted = set.Delete(to_delete, 3);
    assert(deleted == 3);
    assert(set.GetCount() == 5);
    assert(!set.Contains(10) && !set.Contains(20) && !set.Contains(70));
    std::cout << "✓ Batch delete succeeded" << std::endl;
}

void TestGetOperations()
{
    std::cout << "\n=== Test Get operations ===" << std::endl;

    FlatUnorderedSet<int> set;
    set.Add(111);
    set.Add(222);
    set.Add(333);

    // GetFirst
    int first;
    assert(set.GetFirst(first) == true);
    assert(first == 111 || first == 222 || first == 333);
    std::cout << "✓ GetFirst succeeded: " << first << std::endl;

    // GetLast
    int last;
    assert(set.GetLast(last) == true);
    assert(last == 111 || last == 222 || last == 333);
    std::cout << "✓ GetLast succeeded: " << last << std::endl;

    // 空集合测试
    FlatUnorderedSet<int> empty_set;
    assert(empty_set.GetFirst(first) == false);
    assert(empty_set.GetLast(last) == false);
    std::cout << "✓ GetFirst/GetLast return false on empty set" << std::endl;
}

void TestIteratorOperations()
{
    std::cout << "\n=== Test iterator operations ===" << std::endl;

    FlatUnorderedSet<int> set;
    set.Add(10);
    set.Add(20);
    set.Add(30);

    // 使用 range-based for 循环
    std::cout << "   Range-based for loop: ";
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
    std::cout << "✓ Range-based for loop succeeded" << std::endl;

    // 使用传统迭代器
    std::cout << "   Traditional iterator traversal: ";
    count = 0;
    for (auto it = set.begin(); it != set.end(); ++it) {
        std::cout << *it << " ";
        count++;
    }
    std::cout << std::endl;
    assert(count == 3);
    std::cout << "✓ Traditional iterator traversal succeeded" << std::endl;

    // 使用 cbegin/cend
    std::cout << "   cbegin/cend traversal: ";
    count = 0;
    for (auto it = set.cbegin(); it != set.cend(); ++it) {
        std::cout << *it << " ";
        count++;
    }
    std::cout << std::endl;
    assert(count == 3);
    std::cout << "✓ cbegin/cend traversal succeeded" << std::endl;

    // Enum 枚举
    std::cout << "   Enum enumeration: ";
    int enum_count = 0;
    set.Enum([&](const int& value) {
        std::cout << value << " ";
        enum_count++;
    });
    std::cout << std::endl;
    assert(enum_count == 3);
    std::cout << "✓ Enum enumeration succeeded" << std::endl;
}

void TestClearOperations()
{
    std::cout << "\n=== Test Clear operations ===" << std::endl;

    FlatUnorderedSet<int> set;
    for(int i = 1; i <= 100; i++) {
        set.Add(i);
    }
    assert(set.GetCount() == 100);

    // Clear（保留内存）
    set.Clear();
    assert(set.GetCount() == 0);
    assert(set.IsEmpty() == true);
    std::cout << "✓ Clear succeeded" << std::endl;

    // 清空后可以继续添加
    set.Add(999);
    assert(set.GetCount() == 1);
    assert(set.Contains(999) == true);
    std::cout << "✓ Can add after Clear" << std::endl;

    // Free（释放内存）
    set.Free();
    assert(set.GetCount() == 0);
    assert(set.IsEmpty() == true);
    std::cout << "✓ Free released memory" << std::endl;
}

int main()
{
    std::cout << "=== FlatUnorderedSet Basic Operations Test ===" << std::endl;

    TestAddOperations();
    TestFindOperations();
    TestDeleteOperations();
    TestGetOperations();
    TestIteratorOperations();
    TestClearOperations();

    std::cout << "\n=== All basic operation tests passed ✓ ===" << std::endl;
    return 0;
}
