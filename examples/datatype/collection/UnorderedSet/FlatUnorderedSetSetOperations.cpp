/**
 * UnOrderedValueSetTest02_SetOperations.cpp
 * 集合运算测试 - 测试 Union（并集）、Intersect（交集）、Difference（差集）
 */

#include<hgl/type/FlatUnorderedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;

void TestUnion()
{
    std::cout << "\n=== Test Union ===" << std::endl;

    FlatUnorderedSet<int> set1;
    FlatUnorderedSet<int> set2;

    // set1: {1, 2, 3, 4, 5}
    for(int i = 1; i <= 5; i++) {
        set1.Add(i);
    }

    // set2: {4, 5, 6, 7, 8}
    for(int i = 4; i <= 8; i++) {
        set2.Add(i);
    }

    std::cout << "   set1 size: " << set1.GetCount() << std::endl;
    std::cout << "   set2 size: " << set2.GetCount() << std::endl;

    // 计算 set1 ∪ set2
    int added = set1.Union(set2);
    std::cout << "   New elements added: " << added << std::endl;
    std::cout << "   set1 size after union: " << set1.GetCount() << std::endl;

    // 验证结果：set1 应包含 {1, 2, 3, 4, 5, 6, 7, 8}
    assert(added == 3);  // 6, 7, 8 是新增的
    assert(set1.GetCount() == 8);

    for(int i = 1; i <= 8; i++) {
        assert(set1.Contains(i));
    }

    std::cout << "   ✓ Union elements verified" << std::endl;

    // 验证 set2 未被修改
    assert(set2.GetCount() == 5);
    std::cout << "   ✓ Original set unmodified" << std::endl;

    // 测试与空集合的并集
    FlatUnorderedSet<int> empty_set;
    int added_from_empty = set1.Union(empty_set);
    assert(added_from_empty == 0);
    assert(set1.GetCount() == 8);
    std::cout << "   ✓ Union with empty set correct" << std::endl;
}

void TestIntersect()
{
    std::cout << "\n=== Test Intersect ===" << std::endl;

    FlatUnorderedSet<int> set1;
    FlatUnorderedSet<int> set2;

    // set1: {1, 2, 3, 4, 5, 6}
    for(int i = 1; i <= 6; i++) {
        set1.Add(i);
    }

    // set2: {4, 5, 6, 7, 8}
    for(int i = 4; i <= 8; i++) {
        set2.Add(i);
    }

    std::cout << "   set1 size: " << set1.GetCount() << std::endl;
    std::cout << "   set2 size: " << set2.GetCount() << std::endl;

    // 计算 set1 ∩ set2
    int deleted = set1.Intersect(set2);
    std::cout << "   Deleted element count: " << deleted << std::endl;
    std::cout << "   set1 size after intersect: " << set1.GetCount() << std::endl;

    // 验证结果：set1 应只包含 {4, 5, 6}
    assert(deleted == 3);  // 1, 2, 3 被删除
    assert(set1.GetCount() == 3);

    assert(set1.Contains(4));
    assert(set1.Contains(5));
    assert(set1.Contains(6));
    assert(!set1.Contains(1));
    assert(!set1.Contains(2));
    assert(!set1.Contains(3));

    std::cout << "   ✓ Intersect elements verified" << std::endl;

    // 测试与空集合的交集
    FlatUnorderedSet<int> set3;
    for(int i = 10; i <= 15; i++) {
        set3.Add(i);
    }
    FlatUnorderedSet<int> empty_set;
    int deleted_all = set3.Intersect(empty_set);
    assert(deleted_all == 6);  // 所有元素都被删除
    assert(set3.GetCount() == 0);
    std::cout << "   ✓ Intersect with empty set correct (result empty)" << std::endl;

    // 测试无交集的情况
    FlatUnorderedSet<int> set4, set5;
    set4.Add(1); set4.Add(2); set4.Add(3);
    set5.Add(7); set5.Add(8); set5.Add(9);
    int del = set4.Intersect(set5);
    assert(del == 3);
    assert(set4.GetCount() == 0);
    std::cout << "   ✓ Disjoint sets handled correctly" << std::endl;
}

void TestDifference()
{
    std::cout << "\n=== Test Difference ===" << std::endl;

    FlatUnorderedSet<int> set1;
    FlatUnorderedSet<int> set2;

    // set1: {1, 2, 3, 4, 5, 6}
    for(int i = 1; i <= 6; i++) {
        set1.Add(i);
    }

    // set2: {4, 5, 6, 7, 8}
    for(int i = 4; i <= 8; i++) {
        set2.Add(i);
    }

    std::cout << "   set1 size: " << set1.GetCount() << std::endl;
    std::cout << "   set2 size: " << set2.GetCount() << std::endl;

    // 计算 set1 - set2
    int deleted = set1.Difference(set2);
    std::cout << "   Deleted element count: " << deleted << std::endl;
    std::cout << "   set1 size after difference: " << set1.GetCount() << std::endl;

    // 验证结果：set1 应只包含 {1, 2, 3}
    assert(deleted == 3);  // 4, 5, 6 被删除
    assert(set1.GetCount() == 3);

    assert(set1.Contains(1));
    assert(set1.Contains(2));
    assert(set1.Contains(3));
    assert(!set1.Contains(4));
    assert(!set1.Contains(5));
    assert(!set1.Contains(6));

    std::cout << "   ✓ Difference elements verified" << std::endl;

    // 测试与空集合的差集
    FlatUnorderedSet<int> set3;
    for(int i = 10; i <= 15; i++) {
        set3.Add(i);
    }
    FlatUnorderedSet<int> empty_set;
    int del_none = set3.Difference(empty_set);
    assert(del_none == 0);  // 没有元素被删除
    assert(set3.GetCount() == 6);
    std::cout << "   ✓ Difference with empty set correct (no deletion)" << std::endl;

    // 测试无共同元素的差集
    FlatUnorderedSet<int> set4, set5;
    set4.Add(1); set4.Add(2); set4.Add(3);
    set5.Add(7); set5.Add(8); set5.Add(9);
    int del = set4.Difference(set5);
    assert(del == 0);
    assert(set4.GetCount() == 3);
    std::cout << "   ✓ Difference with no common elements correct" << std::endl;
}

void TestCombinedOperations()
{
    std::cout << "\n=== Test combined operations ===" << std::endl;

    // 创建三个集合
    FlatUnorderedSet<int> A, B, C;

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

    std::cout << "   ✓ (A ∪ B) ∩ C computed correctly" << std::endl;

    // 测试对称差集：(A - B) ∪ (B - A)
    FlatUnorderedSet<int> X, Y;
    for(int i = 1; i <= 5; i++) X.Add(i);  // X = {1, 2, 3, 4, 5}
    for(int i = 3; i <= 7; i++) Y.Add(i);  // Y = {3, 4, 5, 6, 7}

    FlatUnorderedSet<int> X_copy, Y_copy;
    for(int i = 1; i <= 5; i++) X_copy.Add(i);
    for(int i = 3; i <= 7; i++) Y_copy.Add(i);

    X_copy.Difference(Y);  // X_copy = {1, 2}
    Y_copy.Difference(X);  // Y_copy = {6, 7}
    X_copy.Union(Y_copy);  // X_copy = {1, 2, 6, 7}

    assert(X_copy.GetCount() == 4);
    assert(X_copy.Contains(1) && X_copy.Contains(2));
    assert(X_copy.Contains(6) && X_copy.Contains(7));
    assert(!X_copy.Contains(3) && !X_copy.Contains(4) && !X_copy.Contains(5));

    std::cout << "   ✓ Symmetric difference (A-B) ∪ (B-A) correct" << std::endl;
}

int main()
{
    std::cout << "=== FlatUnorderedSet Set Operations Test ===" << std::endl;

    TestUnion();
    TestIntersect();
    TestDifference();
    TestCombinedOperations();

    std::cout << "\n=== All set operation tests passed ✓ ===" << std::endl;
    return 0;
}
