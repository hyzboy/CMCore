/**
 * UnorderedSetComparisonOperators.cpp
 * UnorderedSet 比较运算测试
 */

#include <hgl/type/UnorderedSet.h>
#include <iostream>
#include <cassert>

using namespace hgl;
using namespace std;

void PrintSet(const char* name, const UnorderedSet<int>& set)
{
    cout << "  " << name << ": {";
    bool first = true;
    set.Enum([&](int value) {
        if (!first) cout << ", ";
        cout << value;
        first = false;
    });
    cout << "}" << endl;
}

void TestEqualityBasic()
{
    cout << "\n=== Test 1: Basic Equality ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(1); set2.Add(2); set2.Add(3);

    PrintSet("set1", set1);
    PrintSet("set2", set2);

    assert(set1 == set2);
    assert(!(set1 != set2));
    cout << "  ✓ Equal sets are recognized correctly" << endl;
}

void TestInequalityBasic()
{
    cout << "\n=== Test 2: Basic Inequality ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(1); set2.Add(2); set2.Add(4);

    PrintSet("set1", set1);
    PrintSet("set2", set2);

    assert(set1 != set2);
    assert(!(set1 == set2));
    cout << "  ✓ Unequal sets are recognized correctly" << endl;
}

void TestEqualityDifferentOrder()
{
    cout << "\n=== Test 3: Equality with Different Order ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(3); set2.Add(1); set2.Add(2);

    PrintSet("set1", set1);
    PrintSet("set2", set2);

    assert(set1 == set2);
    cout << "  ✓ Sets with same elements in different order are equal" << endl;
}

void TestEqualityDifferentCount()
{
    cout << "\n=== Test 4: Inequality with Different Count ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(1); set2.Add(2);

    PrintSet("set1", set1);
    PrintSet("set2", set2);

    assert(set1 != set2);
    cout << "  ✓ Sets with different counts are unequal" << endl;
}

void TestEqualityWithEmpty()
{
    cout << "\n=== Test 5: Equality with Empty Set ===" << endl;

    UnorderedSet<int> empty1, empty2;

    assert(empty1 == empty2);
    cout << "  ✓ Two empty sets are equal" << endl;

    UnorderedSet<int> non_empty;
    non_empty.Add(1);

    assert(empty1 != non_empty);
    cout << "  ✓ Empty and non-empty sets are unequal" << endl;
}

void TestEqualityAfterOperations()
{
    cout << "\n=== Test 6: Equality After Operations ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(1); set2.Add(2); set2.Add(3);

    assert(set1 == set2);
    cout << "  ✓ Sets are equal before modification" << endl;

    set1.Delete(2);
    assert(set1 != set2);
    cout << "  ✓ Sets become unequal after delete" << endl;

    set2.Delete(2);
    assert(set1 == set2);
    cout << "  ✓ Sets become equal again after same delete" << endl;
}

void TestEqualityAfterAdd()
{
    cout << "\n=== Test 7: Equality After Add ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1);
    assert(set1 != set2);
    cout << "  ✓ Sets are unequal (1 vs empty)" << endl;

    set2.Add(1);
    assert(set1 == set2);
    cout << "  ✓ Sets become equal after adding same element" << endl;

    set2.Add(2);
    assert(set1 != set2);
    cout << "  ✓ Sets become unequal after adding different element" << endl;

    set1.Add(2);
    assert(set1 == set2);
    cout << "  ✓ Sets become equal again after adding matching element" << endl;
}

void TestEqualitySubset()
{
    cout << "\n=== Test 8: Subset Inequality ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3); set1.Add(4);
    set2.Add(1); set2.Add(2); set2.Add(3);

    PrintSet("set1", set1);
    PrintSet("set2", set2);

    assert(set1 != set2);
    cout << "  ✓ Superset and subset are unequal" << endl;

    // set1 变为 set2 的子集
    set1.Delete(4);
    assert(set1 == set2);
    cout << "  ✓ Sets become equal when sizes match" << endl;
}

void TestEqualityWithStrings()
{
    cout << "\n=== Test 9: String Set Equality ===" << endl;

    UnorderedSet<std::string> set1, set2, set3;

    set1.Add("apple");
    set1.Add("banana");
    set1.Add("cherry");

    set2.Add("apple");
    set2.Add("banana");
    set2.Add("cherry");

    set3.Add("apple");
    set3.Add("banana");
    set3.Add("date");

    assert(set1 == set2);
    cout << "  ✓ String sets with same elements are equal" << endl;

    assert(set1 != set3);
    cout << "  ✓ String sets with different elements are unequal" << endl;
}

void TestSelfEquality()
{
    cout << "\n=== Test 10: Self Equality ===" << endl;

    UnorderedSet<int> set;

    set.Add(1); set.Add(2); set.Add(3);

    assert(set == set);
    assert(!(set != set));
    cout << "  ✓ Set is equal to itself" << endl;

    set.Delete(2);
    assert(set == set);
    cout << "  ✓ Set remains equal to itself after modification" << endl;
}

void TestComplexEquality()
{
    cout << "\n=== Test 11: Complex Equality ===" << endl;

    UnorderedSet<int> setA, setB, setC;

    // setA: {1, 2, 3, 4, 5}
    for (int i = 1; i <= 5; ++i)
        setA.Add(i);

    // setB: {5, 4, 3, 2, 1} (same elements, different order)
    for (int i = 5; i >= 1; --i)
        setB.Add(i);

    // setC: {1, 2, 3, 4, 5, 6}
    for (int i = 1; i <= 6; ++i)
        setC.Add(i);

    assert(setA == setB);
    cout << "  ✓ Sets with same elements in different order are equal" << endl;

    assert(setA != setC);
    cout << "  ✓ Sets with different sizes are unequal" << endl;

    // setC 删除 6
    setC.Delete(6);
    assert(setA == setC);
    cout << "  ✓ Sets become equal after matching operations" << endl;
}

void TestTransitivity()
{
    cout << "\n=== Test 12: Transitivity ===" << endl;

    UnorderedSet<int> setA, setB, setC;

    setA.Add(1); setA.Add(2); setA.Add(3);
    setB.Add(1); setB.Add(2); setB.Add(3);
    setC.Add(1); setC.Add(2); setC.Add(3);

    // 如果 A == B 且 B == C，则 A == C
    assert(setA == setB);
    assert(setB == setC);
    assert(setA == setC);
    cout << "  ✓ Transitivity holds" << endl;
}

int main()
{
    cout << "=== UnorderedSet Comparison Operators Test Suite ===" << endl;

    try {
        TestEqualityBasic();
        TestInequalityBasic();
        TestEqualityDifferentOrder();
        TestEqualityDifferentCount();
        TestEqualityWithEmpty();
        TestEqualityAfterOperations();
        TestEqualityAfterAdd();
        TestEqualitySubset();
        TestEqualityWithStrings();
        TestSelfEquality();
        TestComplexEquality();
        TestTransitivity();

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
