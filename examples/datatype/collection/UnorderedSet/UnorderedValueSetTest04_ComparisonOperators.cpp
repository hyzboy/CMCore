/**
 * UnOrderedValueSetTest04_ComparisonOperators.cpp
 * 比较运算符测试 - 测试 operator== 和 operator!=
 */

#include<hgl/type/UnorderedValueSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;

void TestEqualityBasic()
{
    std::cout << "\n=== 测试基础相等比较 ===" << std::endl;

    UnorderedValueSet<int> set1, set2;

    // 两个空集合应该相等
    assert(set1 == set2);
    assert(!(set1 != set2));
    std::cout << "  ✓ 空集合相等" << std::endl;

    // 添加相同元素
    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(1); set2.Add(2); set2.Add(3);
    assert(set1 == set2);
    assert(!(set1 != set2));
    std::cout << "  ✓ 相同元素集合相等" << std::endl;

    // 添加顺序不影响相等性（无序集合特性）
    UnorderedValueSet<int> set3, set4;
    set3.Add(1); set3.Add(2); set3.Add(3);
    set4.Add(3); set4.Add(1); set4.Add(2);  // 不同顺序
    assert(set3 == set4);
    std::cout << "  ✓ 添加顺序不影响相等性" << std::endl;
}

void TestInequalityBasic()
{
    std::cout << "\n=== 测试基础不等比较 ===" << std::endl;

    UnorderedValueSet<int> set1, set2;

    // 不同大小的集合
    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(1); set2.Add(2);
    assert(set1 != set2);
    assert(!(set1 == set2));
    std::cout << "  ✓ 不同大小集合不相等" << std::endl;

    // 相同大小但不同元素
    UnorderedValueSet<int> set3, set4;
    set3.Add(1); set3.Add(2); set3.Add(3);
    set4.Add(1); set4.Add(2); set4.Add(4);
    assert(set3 != set4);
    std::cout << "  ✓ 相同大小但不同元素不相等" << std::endl;

    // 完全不同的元素
    UnorderedValueSet<int> set5, set6;
    set5.Add(1); set5.Add(2); set5.Add(3);
    set6.Add(7); set6.Add(8); set6.Add(9);
    assert(set5 != set6);
    std::cout << "  ✓ 完全不同元素不相等" << std::endl;
}

void TestEmptyVsNonEmpty()
{
    std::cout << "\n=== 测试空集合与非空集合 ===" << std::endl;

    UnorderedValueSet<int> empty_set;
    UnorderedValueSet<int> non_empty_set;
    non_empty_set.Add(1);

    assert(empty_set != non_empty_set);
    assert(!(empty_set == non_empty_set));
    std::cout << "  ✓ 空集合与非空集合不相等" << std::endl;
}

void TestSelfComparison()
{
    std::cout << "\n=== 测试自我比较 ===" << std::endl;

    UnorderedValueSet<int> set;
    set.Add(1); set.Add(2); set.Add(3);

    // 自己与自己比较应该相等
    assert(set == set);
    assert(!(set != set));
    std::cout << "  ✓ 自我比较相等" << std::endl;
}

void TestLargeSetComparison()
{
    std::cout << "\n=== 测试大集合比较 ===" << std::endl;

    UnorderedValueSet<int> set1, set2;

    // 添加大量相同元素
    const int COUNT = 10000;
    for(int i = 0; i < COUNT; i++) {
        set1.Add(i);
        set2.Add(i);
    }

    assert(set1 == set2);
    std::cout << "  ✓ 大集合（" << COUNT << " 元素）相等比较正确" << std::endl;

    // 修改一个元素
    set2.Delete(5000);
    set2.Add(99999);
    assert(set1 != set2);
    std::cout << "  ✓ 大集合不等比较正确" << std::endl;
}

void TestComparisonAfterOperations()
{
    std::cout << "\n=== 测试操作后的比较 ===" << std::endl;

    UnorderedValueSet<int> set1, set2, set3;

    // 初始化 set1 和 set2
    for(int i = 1; i <= 5; i++) {
        set1.Add(i);
        set2.Add(i);
    }
    assert(set1 == set2);
    std::cout << "  初始状态相等" << std::endl;

    // 删除后比较
    set1.Delete(3);
    assert(set1 != set2);
    set2.Delete(3);
    assert(set1 == set2);
    std::cout << "  ✓ 删除操作后比较正确" << std::endl;

    // 添加后比较
    set1.Add(10);
    assert(set1 != set2);
    set2.Add(10);
    assert(set1 == set2);
    std::cout << "  ✓ 添加操作后比较正确" << std::endl;

    // Clear 后比较
    set1.Clear();
    assert(set1 != set2);
    set2.Clear();
    assert(set1 == set2);
    std::cout << "  ✓ 清空操作后比较正确" << std::endl;
}

void TestDifferentTypes()
{
    std::cout << "\n=== 测试不同数据类型的比较 ===" << std::endl;

    // uint64 类型
    UnorderedValueSet<uint64> set1, set2;
    set1.Add(1000000000ULL);
    set1.Add(2000000000ULL);
    set2.Add(1000000000ULL);
    set2.Add(2000000000ULL);
    assert(set1 == set2);
    std::cout << "  ✓ uint64 类型比较正确" << std::endl;

    // float 类型
    UnorderedValueSet<float> fset1, fset2;
    fset1.Add(1.5f);
    fset1.Add(2.5f);
    fset2.Add(1.5f);
    fset2.Add(2.5f);
    assert(fset1 == fset2);
    std::cout << "  ✓ float 类型比较正确" << std::endl;

    // char 类型
    UnorderedValueSet<char> cset1, cset2;
    cset1.Add('a'); cset1.Add('b'); cset1.Add('c');
    cset2.Add('a'); cset2.Add('b'); cset2.Add('c');
    assert(cset1 == cset2);
    std::cout << "  ✓ char 类型比较正确" << std::endl;
}

void TestSubsetRelation()
{
    std::cout << "\n=== 测试子集关系（非标准功能，用比较运算符检测）===" << std::endl;

    UnorderedValueSet<int> subset, superset;

    // subset: {1, 2, 3}
    subset.Add(1); subset.Add(2); subset.Add(3);

    // superset: {1, 2, 3, 4, 5}
    superset.Add(1); superset.Add(2); superset.Add(3);
    superset.Add(4); superset.Add(5);

    // 子集不等于超集
    assert(subset != superset);
    std::cout << "  ✓ 子集与超集不相等（大小不同）" << std::endl;

    // 检查是否所有 subset 元素都在 superset 中（手动实现子集检测）
    bool is_subset = true;
    for(int i = 1; i <= 3; i++) {
        if(!superset.Contains(i)) {
            is_subset = false;
            break;
        }
    }
    assert(is_subset == true);
    std::cout << "  ✓ 可以手动检测子集关系" << std::endl;
}

int main()
{
    std::cout << "=== UnorderedValueSet 比较运算符测试 ===" << std::endl;

    TestEqualityBasic();
    TestInequalityBasic();
    TestEmptyVsNonEmpty();
    TestSelfComparison();
    TestLargeSetComparison();
    TestComparisonAfterOperations();
    TestDifferentTypes();
    TestSubsetRelation();

    std::cout << "\n=== 所有比较运算符测试通过 ✓ ===" << std::endl;
    return 0;
}
