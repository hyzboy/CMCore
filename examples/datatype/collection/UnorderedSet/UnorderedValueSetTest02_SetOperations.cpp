/**
 * UnorderedValueSetTest02_SetOperations.cpp
 * 集合运算测试 - 测试 Union（并集）、Intersect（交集）、Difference（差集）
 */

#include<hgl/type/UnorderedValueSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;

void TestUnion()
{
    std::cout << "\n=== 测试并集（Union）===" << std::endl;

    UnorderedValueSet<int> set1;
    UnorderedValueSet<int> set2;

    // set1: {1, 2, 3, 4, 5}
    for(int i = 1; i <= 5; i++) {
        set1.Add(i);
    }

    // set2: {4, 5, 6, 7, 8}
    for(int i = 4; i <= 8; i++) {
        set2.Add(i);
    }

    std::cout << "   set1 大小: " << set1.GetCount() << std::endl;
    std::cout << "   set2 大小: " << set2.GetCount() << std::endl;

    // 计算 set1 ∪ set2
    int added = set1.Union(set2);
    std::cout << "   新增元素数: " << added << std::endl;
    std::cout << "   并集后 set1 大小: " << set1.GetCount() << std::endl;

    // 验证结果：set1 应包含 {1, 2, 3, 4, 5, 6, 7, 8}
    assert(added == 3);  // 6, 7, 8 是新增的
    assert(set1.GetCount() == 8);

    for(int i = 1; i <= 8; i++) {
        assert(set1.Contains(i));
    }

    std::cout << "   ✓ 并集元素验证通过" << std::endl;

    // 验证 set2 未被修改
    assert(set2.GetCount() == 5);
    std::cout << "   ✓ 原集合未被修改" << std::endl;

    // 测试与空集合的并集
    UnorderedValueSet<int> empty_set;
    int added_from_empty = set1.Union(empty_set);
    assert(added_from_empty == 0);
    assert(set1.GetCount() == 8);
    std::cout << "   ✓ 与空集合并集正确" << std::endl;
}

void TestIntersect()
{
    std::cout << "\n=== 测试交集（Intersect）===" << std::endl;

    UnorderedValueSet<int> set1;
    UnorderedValueSet<int> set2;

    // set1: {1, 2, 3, 4, 5, 6}
    for(int i = 1; i <= 6; i++) {
        set1.Add(i);
    }

    // set2: {4, 5, 6, 7, 8}
    for(int i = 4; i <= 8; i++) {
        set2.Add(i);
    }

    std::cout << "   set1 大小: " << set1.GetCount() << std::endl;
    std::cout << "   set2 大小: " << set2.GetCount() << std::endl;

    // 计算 set1 ∩ set2
    int deleted = set1.Intersect(set2);
    std::cout << "   删除元素数: " << deleted << std::endl;
    std::cout << "   交集后 set1 大小: " << set1.GetCount() << std::endl;

    // 验证结果：set1 应只包含 {4, 5, 6}
    assert(deleted == 3);  // 1, 2, 3 被删除
    assert(set1.GetCount() == 3);

    assert(set1.Contains(4));
    assert(set1.Contains(5));
    assert(set1.Contains(6));
    assert(!set1.Contains(1));
    assert(!set1.Contains(2));
    assert(!set1.Contains(3));

    std::cout << "   ✓ 交集元素验证通过" << std::endl;

    // 测试与空集合的交集
    UnorderedValueSet<int> set3;
    for(int i = 10; i <= 15; i++) {
        set3.Add(i);
    }
    UnorderedValueSet<int> empty_set;
    int deleted_all = set3.Intersect(empty_set);
    assert(deleted_all == 6);  // 所有元素都被删除
    assert(set3.GetCount() == 0);
    std::cout << "   ✓ 与空集合交集正确（结果为空）" << std::endl;

    // 测试无交集的情况
    UnorderedValueSet<int> set4, set5;
    set4.Add(1); set4.Add(2); set4.Add(3);
    set5.Add(7); set5.Add(8); set5.Add(9);
    int del = set4.Intersect(set5);
    assert(del == 3);
    assert(set4.GetCount() == 0);
    std::cout << "   ✓ 无交集情况正确处理" << std::endl;
}

void TestDifference()
{
    std::cout << "\n=== 测试差集（Difference）===" << std::endl;

    UnorderedValueSet<int> set1;
    UnorderedValueSet<int> set2;

    // set1: {1, 2, 3, 4, 5, 6}
    for(int i = 1; i <= 6; i++) {
        set1.Add(i);
    }

    // set2: {4, 5, 6, 7, 8}
    for(int i = 4; i <= 8; i++) {
        set2.Add(i);
    }

    std::cout << "   set1 大小: " << set1.GetCount() << std::endl;
    std::cout << "   set2 大小: " << set2.GetCount() << std::endl;

    // 计算 set1 - set2
    int deleted = set1.Difference(set2);
    std::cout << "   删除元素数: " << deleted << std::endl;
    std::cout << "   差集后 set1 大小: " << set1.GetCount() << std::endl;

    // 验证结果：set1 应只包含 {1, 2, 3}
    assert(deleted == 3);  // 4, 5, 6 被删除
    assert(set1.GetCount() == 3);

    assert(set1.Contains(1));
    assert(set1.Contains(2));
    assert(set1.Contains(3));
    assert(!set1.Contains(4));
    assert(!set1.Contains(5));
    assert(!set1.Contains(6));

    std::cout << "   ✓ 差集元素验证通过" << std::endl;

    // 测试与空集合的差集
    UnorderedValueSet<int> set3;
    for(int i = 10; i <= 15; i++) {
        set3.Add(i);
    }
    UnorderedValueSet<int> empty_set;
    int del_none = set3.Difference(empty_set);
    assert(del_none == 0);  // 没有元素被删除
    assert(set3.GetCount() == 6);
    std::cout << "   ✓ 与空集合差集正确（无删除）" << std::endl;

    // 测试无共同元素的差集
    UnorderedValueSet<int> set4, set5;
    set4.Add(1); set4.Add(2); set4.Add(3);
    set5.Add(7); set5.Add(8); set5.Add(9);
    int del = set4.Difference(set5);
    assert(del == 0);
    assert(set4.GetCount() == 3);
    std::cout << "   ✓ 无共同元素差集正确" << std::endl;
}

void TestCombinedOperations()
{
    std::cout << "\n=== 测试组合运算 ===" << std::endl;

    // 创建三个集合
    UnorderedValueSet<int> A, B, C;

    // A = {1, 2, 3, 4}
    for(int i = 1; i <= 4; i++) A.Add(i);

    // B = {3, 4, 5, 6}
    for(int i = 3; i <= 6; i++) B.Add(i);

    // C = {5, 6, 7, 8}
    for(int i = 5; i <= 8; i++) C.Add(i);

    // 测试：(A ∪ B) ∩ C
    A.Union(B);  // A = {1, 2, 3, 4, 5, 6}
    assert(A.GetCount() == 6);

    A.Intersect(C);  // A = {5, 6}
    assert(A.GetCount() == 2);
    assert(A.Contains(5));
    assert(A.Contains(6));

    std::cout << "   ✓ (A ∪ B) ∩ C 计算正确" << std::endl;

    // 测试对称差集：(A - B) ∪ (B - A)
    UnorderedValueSet<int> X, Y;
    for(int i = 1; i <= 5; i++) X.Add(i);  // X = {1, 2, 3, 4, 5}
    for(int i = 3; i <= 7; i++) Y.Add(i);  // Y = {3, 4, 5, 6, 7}

    UnorderedValueSet<int> X_copy, Y_copy;
    for(int i = 1; i <= 5; i++) X_copy.Add(i);
    for(int i = 3; i <= 7; i++) Y_copy.Add(i);

    X_copy.Difference(Y);  // X_copy = {1, 2}
    Y_copy.Difference(X);  // Y_copy = {6, 7}
    X_copy.Union(Y_copy);  // X_copy = {1, 2, 6, 7}

    assert(X_copy.GetCount() == 4);
    assert(X_copy.Contains(1) && X_copy.Contains(2));
    assert(X_copy.Contains(6) && X_copy.Contains(7));
    assert(!X_copy.Contains(3) && !X_copy.Contains(4) && !X_copy.Contains(5));

    std::cout << "   ✓ 对称差集 (A-B) ∪ (B-A) 计算正确" << std::endl;
}

int main()
{
    std::cout << "=== UnorderedValueSet 集合运算测试 ===" << std::endl;

    TestUnion();
    TestIntersect();
    TestDifference();
    TestCombinedOperations();

    std::cout << "\n=== 所有集合运算测试通过 ✓ ===" << std::endl;
    return 0;
}
