/**
 * UnorderedSetSetOperations.cpp
 * UnorderedSet 集合运算测试
 */

#include <hgl/type/UnorderedSet.h>
#include <iostream>
#include <cassert>

using namespace hgl;
using namespace std;

void PrintSet(const char* name, const UnorderedSet<int>& set)
{
    cout << name << ": {";
    bool first = true;
    set.Enum([&](int value) {
        if (!first) cout << ", ";
        cout << value;
        first = false;
    });
    cout << "}" << endl;
}

void TestUnion()
{
    cout << "\n=== Test 1: Union ===" << endl;

    UnorderedSet<int> set1, set2;

    // set1: {1, 2, 3}
    set1.Add(1); set1.Add(2); set1.Add(3);
    PrintSet("set1", set1);

    // set2: {2, 3, 4}
    set2.Add(2); set2.Add(3); set2.Add(4);
    PrintSet("set2", set2);

    // Union
    UnorderedSet<int> union_set;
    union_set.Add(1); union_set.Add(2); union_set.Add(3);
    int added = union_set.Union(set2);

    // union_set 应该是 {1, 2, 3, 4}
    PrintSet("union_set", union_set);
    assert(union_set.GetCount() == 4);
    assert(union_set.Contains(1));
    assert(union_set.Contains(4));
    assert(added == 1);  // 只添加了 4
    cout << "  ✓ Union works correctly" << endl;
}

void TestIntersection()
{
    cout << "\n=== Test 2: Intersection ===" << endl;

    UnorderedSet<int> set1, set2;

    // set1: {1, 2, 3}
    set1.Add(1); set1.Add(2); set1.Add(3);
    PrintSet("set1", set1);

    // set2: {2, 3, 4}
    set2.Add(2); set2.Add(3); set2.Add(4);
    PrintSet("set2", set2);

    // Intersection
    UnorderedSet<int> intersect_set;
    intersect_set.Add(1); intersect_set.Add(2); intersect_set.Add(3);
    int removed = intersect_set.Intersect(set2);

    // intersect_set 应该是 {2, 3}
    PrintSet("intersect_set", intersect_set);
    assert(intersect_set.GetCount() == 2);
    assert(intersect_set.Contains(2));
    assert(intersect_set.Contains(3));
    assert(!intersect_set.Contains(1));
    assert(removed == 1);  // 移除了 1
    cout << "  ✓ Intersection works correctly" << endl;
}

void TestDifference()
{
    cout << "\n=== Test 3: Difference ===" << endl;

    UnorderedSet<int> set1, set2;

    // set1: {1, 2, 3}
    set1.Add(1); set1.Add(2); set1.Add(3);
    PrintSet("set1", set1);

    // set2: {2, 3, 4}
    set2.Add(2); set2.Add(3); set2.Add(4);
    PrintSet("set2", set2);

    // Difference
    UnorderedSet<int> diff_set;
    diff_set.Add(1); diff_set.Add(2); diff_set.Add(3);
    int removed = diff_set.Difference(set2);

    // diff_set 应该是 {1}
    PrintSet("diff_set", diff_set);
    assert(diff_set.GetCount() == 1);
    assert(diff_set.Contains(1));
    assert(!diff_set.Contains(2));
    assert(!diff_set.Contains(3));
    assert(removed == 2);  // 移除了 2 和 3
    cout << "  ✓ Difference works correctly" << endl;
}

void TestComplexUnion()
{
    cout << "\n=== Test 4: Complex Union ===" << endl;

    UnorderedSet<int> set1, set2, set3;

    set1.Add(1); set1.Add(2);
    set2.Add(2); set2.Add(3);
    set3.Add(3); set3.Add(4);

    PrintSet("set1", set1);
    PrintSet("set2", set2);
    PrintSet("set3", set3);

    // 多次并集
    set1.Union(set2);
    PrintSet("set1 ∪ set2", set1);
    assert(set1.GetCount() == 3);  // {1, 2, 3}

    set1.Union(set3);
    PrintSet("set1 ∪ set2 ∪ set3", set1);
    assert(set1.GetCount() == 4);  // {1, 2, 3, 4}
    cout << "  ✓ Complex union works correctly" << endl;
}

void TestComplexIntersection()
{
    cout << "\n=== Test 5: Complex Intersection ===" << endl;

    UnorderedSet<int> set1, set2, set3;

    set1.Add(1); set1.Add(2); set1.Add(3); set1.Add(4);
    set2.Add(2); set2.Add(3); set2.Add(5);
    set3.Add(3); set3.Add(6);

    PrintSet("set1", set1);
    PrintSet("set2", set2);
    PrintSet("set3", set3);

    // 多次交集
    set1.Intersect(set2);
    PrintSet("set1 ∩ set2", set1);
    assert(set1.GetCount() == 2);  // {2, 3}

    set1.Intersect(set3);
    PrintSet("set1 ∩ set2 ∩ set3", set1);
    assert(set1.GetCount() == 1);  // {3}
    cout << "  ✓ Complex intersection works correctly" << endl;
}

void TestUnionWithEmpty()
{
    cout << "\n=== Test 6: Union with Empty Set ===" << endl;

    UnorderedSet<int> empty, set1;

    set1.Add(1); set1.Add(2); set1.Add(3);

    int added = empty.Union(set1);
    assert(empty.GetCount() == 3);
    assert(added == 3);
    PrintSet("empty ∪ set1", empty);
    cout << "  ✓ Union with empty set works correctly" << endl;
}

void TestIntersectionWithEmpty()
{
    cout << "\n=== Test 7: Intersection with Empty Set ===" << endl;

    UnorderedSet<int> set1, empty;

    set1.Add(1); set1.Add(2); set1.Add(3);

    int removed = set1.Intersect(empty);
    assert(set1.IsEmpty());
    assert(removed == 3);
    PrintSet("set1 ∩ empty", set1);
    cout << "  ✓ Intersection with empty set works correctly" << endl;
}

void TestDifferenceWithEmpty()
{
    cout << "\n=== Test 8: Difference with Empty Set ===" << endl;

    UnorderedSet<int> set1, empty;

    set1.Add(1); set1.Add(2); set1.Add(3);

    int removed = set1.Difference(empty);
    assert(set1.GetCount() == 3);  // 没有任何变化
    assert(removed == 0);
    PrintSet("set1 - empty", set1);
    cout << "  ✓ Difference with empty set works correctly" << endl;
}

void TestUnionWithSelf()
{
    cout << "\n=== Test 9: Union with Self ===" << endl;

    UnorderedSet<int> set1;

    set1.Add(1); set1.Add(2); set1.Add(3);

    int added = set1.Union(set1);
    assert(set1.GetCount() == 3);
    assert(added == 0);  // 没有新增
    PrintSet("set1 ∪ set1", set1);
    cout << "  ✓ Union with self works correctly" << endl;
}

void TestIntersectionWithSelf()
{
    cout << "\n=== Test 10: Intersection with Self ===" << endl;

    UnorderedSet<int> set1;

    set1.Add(1); set1.Add(2); set1.Add(3);

    int removed = set1.Intersect(set1);
    assert(set1.GetCount() == 3);
    assert(removed == 0);  // 没有移除
    PrintSet("set1 ∩ set1", set1);
    cout << "  ✓ Intersection with self works correctly" << endl;
}

void TestStringSetOperations()
{
    cout << "\n=== Test 11: String Set Operations ===" << endl;

    UnorderedSet<std::string> animals1, animals2;

    animals1.Add("cat");
    animals1.Add("dog");
    animals1.Add("bird");

    animals2.Add("dog");
    animals2.Add("fish");
    animals2.Add("bird");

    // Union
    UnorderedSet<std::string> all_animals;
    all_animals.Add("cat"); all_animals.Add("dog"); all_animals.Add("bird");
    all_animals.Union(animals2);

    assert(all_animals.GetCount() == 4);
    assert(all_animals.Contains("fish"));
    cout << "  ✓ String set union works" << endl;

    // Intersection
    UnorderedSet<std::string> common;
    common.Add("cat"); common.Add("dog"); common.Add("bird");
    common.Intersect(animals2);

    assert(common.GetCount() == 2);
    assert(common.Contains("dog"));
    assert(common.Contains("bird"));
    cout << "  ✓ String set intersection works" << endl;
}

int main()
{
    cout << "=== UnorderedSet Set Operations Test Suite ===" << endl;

    try {
        TestUnion();
        TestIntersection();
        TestDifference();
        TestComplexUnion();
        TestComplexIntersection();
        TestUnionWithEmpty();
        TestIntersectionWithEmpty();
        TestDifferenceWithEmpty();
        TestUnionWithSelf();
        TestIntersectionWithSelf();
        TestStringSetOperations();

        cout << "\n=== All tests passed successfully ✅ ===" << endl;
        return 0;
    }
    catch (const exception& e) {
        cerr << "\n❌ Test failed with exception: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "\n❌ Test failed with unknown exception" << endl;
        return 1;
    }
}
