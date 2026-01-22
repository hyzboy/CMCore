#include <hgl/type/StringList.h>
#include <iostream>

using namespace hgl;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cout << "  [FAIL] " << msg << std::endl; \
            return false; \
        } \
        std::cout << "  [PASS] " << msg << std::endl; \
    } while(0)

bool test_basic_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: StringList - Basic Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    U8StringList list;

    std::cout << "[1.1] Add and Count:" << std::endl;
    list.Add(U8String(u8"Hello"));
    list.Add(U8String(u8"World"));
    list.Add(U8String(u8"Test"));
    TEST_ASSERT(list.GetCount() == 3, "List has 3 elements");
    TEST_ASSERT(!list.IsEmpty(), "List is not empty");

    std::cout << "\n[1.2] Access via operator[] (now throws exception for invalid index):" << std::endl;
    try {
        const U8String& str = list[0];
        TEST_ASSERT(str.Comp(u8"Hello") == 0, "First element is 'Hello'");
        TEST_ASSERT(list[1].Comp(u8"World") == 0, "Second element is 'World'");
        TEST_ASSERT(list[2].Comp(u8"Test") == 0, "Third element is 'Test'");
    } catch (const std::exception& e) {
        std::cout << "  [FAIL] Unexpected exception: " << e.what() << std::endl;
        return false;
    }

    std::cout << "\n[1.3] Access via At():" << std::endl;
    try {
        TEST_ASSERT(list.At(0).Comp(u8"Hello") == 0, "At(0) returns 'Hello'");
        TEST_ASSERT(list.At(2).Comp(u8"Test") == 0, "At(2) returns 'Test'");
    } catch (const std::exception& e) {
        std::cout << "  [FAIL] Unexpected exception: " << e.what() << std::endl;
        return false;
    }

    std::cout << "\n[1.4] GetString():" << std::endl;
    try {
        const U8String& str_ref = list.GetString(1);
        TEST_ASSERT(str_ref.Comp(u8"World") == 0, "GetString(1) returns 'World'");
    } catch (const std::exception& e) {
        std::cout << "  [FAIL] Unexpected exception: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool test_bounds_checking()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: StringList - Bounds Checking" << std::endl;
    std::cout << "========================================\n" << std::endl;

    U8StringList list;
    list.Add(U8String(u8"Item1"));
    list.Add(U8String(u8"Item2"));

    std::cout << "[2.1] operator[] throws for invalid index:" << std::endl;
    bool caught = false;
    try {
        list[100];
    } catch (const std::out_of_range&) {
        caught = true;
        std::cout << "  [PASS] Out-of-range exception thrown for operator[100]" << std::endl;
    }
    TEST_ASSERT(caught, "Exception thrown for out-of-range operator[]");

    std::cout << "\n[2.2] At() throws for invalid index:" << std::endl;
    caught = false;
    try {
        list.At(-1);
    } catch (const std::out_of_range&) {
        caught = true;
        std::cout << "  [PASS] Out-of-range exception thrown for At(-1)" << std::endl;
    }
    TEST_ASSERT(caught, "Exception thrown for negative index");

    std::cout << "\n[2.3] GetString() handles out of range correctly:" << std::endl;
    caught = false;
    try {
        list.GetString(100);
    } catch (const std::out_of_range&) {
        caught = true;
        std::cout << "  [PASS] Out-of-range exception thrown for GetString(100)" << std::endl;
    }
    TEST_ASSERT(caught, "Exception thrown for out-of-range GetString");

    return true;
}

bool test_insert_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: StringList - Insert Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    U8StringList list;
    list.Add(U8String(u8"First"));
    list.Add(U8String(u8"Last"));

    std::cout << "[3.1] Insert at beginning:" << std::endl;
    try {
        list.Insert(0, U8String(u8"Start"));
        TEST_ASSERT(list.GetCount() == 3, "Count is 3 after insert at 0");
        TEST_ASSERT(list[0].Comp(u8"Start") == 0, "Element at 0 is 'Start'");
        TEST_ASSERT(list[1].Comp(u8"First") == 0, "Element at 1 is 'First'");
    } catch (const std::exception& e) {
        std::cout << "  [FAIL] Exception: " << e.what() << std::endl;
        return false;
    }

    std::cout << "\n[3.2] Insert at end (FIXED: index <= count):" << std::endl;
    try {
        list.Insert(list.GetCount(), U8String(u8"End"));
        TEST_ASSERT(list.GetCount() == 4, "Count is 4 after insert at end");
        TEST_ASSERT(list[3].Comp(u8"End") == 0, "Element at 3 is 'End'");
    } catch (const std::exception& e) {
        std::cout << "  [FAIL] Exception: " << e.what() << std::endl;
        return false;
    }

    std::cout << "\n[3.3] Insert out of bounds throws exception:" << std::endl;
    bool caught = false;
    try {
        list.Insert(100, U8String(u8"Invalid"));
    } catch (const std::out_of_range&) {
        caught = true;
        std::cout << "  [PASS] Out-of-range exception thrown" << std::endl;
    }
    TEST_ASSERT(caught, "Exception thrown for invalid insert index");

    return true;
}

bool test_find_and_contains()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: StringList - Find and Contains" << std::endl;
    std::cout << "========================================\n" << std::endl;

    U8StringList list;
    list.Add(U8String(u8"Apple"));
    list.Add(U8String(u8"Banana"));
    list.Add(U8String(u8"Cherry"));

    std::cout << "[4.1] Find existing elements:" << std::endl;
    TEST_ASSERT(list.Find(U8String(u8"Apple")) == 0, "Find 'Apple' returns 0");
    TEST_ASSERT(list.Find(U8String(u8"Banana")) == 1, "Find 'Banana' returns 1");
    TEST_ASSERT(list.Find(U8String(u8"Cherry")) == 2, "Find 'Cherry' returns 2");

    std::cout << "\n[4.2] Find non-existing element:" << std::endl;
    TEST_ASSERT(list.Find(U8String(u8"Orange")) == -1, "Find 'Orange' returns -1");

    std::cout << "\n[4.3] Contains (new method):" << std::endl;
    TEST_ASSERT(list.Contains(U8String(u8"Apple")), "Contains 'Apple'");
    TEST_ASSERT(list.Contains(U8String(u8"Banana")), "Contains 'Banana'");
    TEST_ASSERT(!list.Contains(U8String(u8"Orange")), "Does not contain 'Orange'");

    return true;
}

bool test_delete_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: StringList - Delete Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    U8StringList list;
    list.Add(U8String(u8"One"));
    list.Add(U8String(u8"Two"));
    list.Add(U8String(u8"Three"));
    list.Add(U8String(u8"Four"));

    std::cout << "[5.1] Delete by index:" << std::endl;
    list.Delete(1);  // Remove "Two" - NOTE: Delete may swap with last element
    TEST_ASSERT(list.GetCount() == 3, "Count is 3 after deleting index 1");
    TEST_ASSERT(list[0].Comp(u8"One") == 0, "Element 0 is still 'One'");
    // After Delete(1), element could be shifted or swapped with last
    TEST_ASSERT(list[1].Comp(u8"Three") == 0 || list[1].Comp(u8"Four") == 0, "Element 1 updated after delete");

    std::cout << "\n[5.2] Delete by string:" << std::endl;
    list.Delete(U8String(u8"Four"));
    TEST_ASSERT(list.GetCount() == 2, "Count is 2 after deleting 'Four'");

    return true;
}

bool test_copy_and_reserve()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: StringList - CopyFrom and Reserve" << std::endl;
    std::cout << "========================================\n" << std::endl;

    U8StringList list1;
    list1.Add(U8String(u8"String1"));
    list1.Add(U8String(u8"String2"));
    list1.Add(U8String(u8"String3"));

    std::cout << "[6.1] CopyFrom (new method):" << std::endl;
    U8StringList list2;
    list2.CopyFrom(list1);
    TEST_ASSERT(list2.GetCount() == 3, "Copied list has 3 elements");
    TEST_ASSERT(list2[0].Comp(u8"String1") == 0, "First element copied");
    TEST_ASSERT(list2[2].Comp(u8"String3") == 0, "Last element copied");

    std::cout << "\n[6.2] Reserve (new method):" << std::endl;
    U8StringList list3;
    list3.Reserve(100);
    TEST_ASSERT(list3.IsEmpty(), "Reserved list is still empty");
    for (int i = 0; i < 50; i++) {
        list3.Add(U8String(u8"Item"));
    }
    TEST_ASSERT(list3.GetCount() == 50, "Added 50 items after reserve");

    return true;
}

bool test_iterator()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 7: StringList - Iterator" << std::endl;
    std::cout << "========================================\n" << std::endl;

    U8StringList list;
    list.Add(U8String(u8"First"));
    list.Add(U8String(u8"Second"));
    list.Add(U8String(u8"Third"));

    std::cout << "[7.1] Range-for loop:" << std::endl;
    int count = 0;
    for (auto* str : list) {
        count++;
        TEST_ASSERT(str != nullptr, "Iterator returns non-null pointer");
    }
    TEST_ASSERT(count == 3, "Iterated 3 elements");

    std::cout << "\n[7.2] Verify iterator content:" << std::endl;
    auto it = list.begin();
    TEST_ASSERT((*it)->Comp(u8"First") == 0, "First iterator element");
    ++it;
    TEST_ASSERT((*it)->Comp(u8"Second") == 0, "Second iterator element");

    return true;
}

bool test_edge_cases()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 8: StringList - Edge Cases" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[8.1] Empty string handling:" << std::endl;
    U8StringList list;
    list.Add(U8String(u8""));
    TEST_ASSERT(list.GetCount() == 1, "Empty string added");
    TEST_ASSERT(list[0].Length() == 0, "First element is empty");

    std::cout << "\n[8.2] Clear operation:" << std::endl;
    list.Add(U8String(u8"Test"));
    list.Clear();
    TEST_ASSERT(list.IsEmpty(), "List empty after Clear");
    TEST_ASSERT(list.GetCount() == 0, "Count is 0 after Clear");

    std::cout << "\n[8.3] AddUnique:" << std::endl;
    list.Add(U8String(u8"Unique1"));
    int pos1 = list.AddUnique(U8String(u8"Unique2"));
    int pos2 = list.AddUnique(U8String(u8"Unique1"));  // Duplicate
    TEST_ASSERT(pos1 >= 0, "AddUnique adds new string");
    TEST_ASSERT(pos2 == 0, "AddUnique returns existing index for duplicate");
    TEST_ASSERT(list.GetCount() == 2, "Count is 2 (duplicate not added)");

    return true;
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "StringList Comprehensive Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    bool all_passed = true;

    all_passed &= test_basic_operations();
    all_passed &= test_bounds_checking();
    all_passed &= test_insert_operations();
    all_passed &= test_find_and_contains();
    all_passed &= test_delete_operations();
    all_passed &= test_copy_and_reserve();
    all_passed &= test_iterator();
    all_passed &= test_edge_cases();

    std::cout << "\n========================================" << std::endl;
    if (all_passed) {
        std::cout << "ALL TESTS PASSED!" << std::endl;
    } else {
        std::cout << "SOME TESTS FAILED!" << std::endl;
    }
    std::cout << "========================================" << std::endl;

    return all_passed ? 0 : 1;
}
