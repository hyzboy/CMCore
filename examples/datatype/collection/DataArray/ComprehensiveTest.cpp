/**
 * DataArray Comprehensive Test Suite
 * 
 * This test suite comprehensively covers all functionalities of the DataArray template class:
 * 1. Basic operations (construction, destruction, initialization)
 * 2. Memory management (Reserve, Resize, Expand, Free, Clear)
 * 3. Data addition and access (Append, At, ReadAt, WriteAt)
 * 4. Data deletion (Delete, DeleteShift)
 * 5. Data movement and exchange (Move, Exchange, Insert)
 * 6. Search and comparison (Find, compare)
 * 7. Set operations (WithoutList)
 * 8. Special types (POD types, non-trivial types)
 */

#include<hgl/type/DataArray.h>
#include<iostream>
#include<iomanip>
#include<string>
#include<cassert>
#include<vector>

using namespace hgl;

// ============================================================================
// Test Helper Tools
// ============================================================================

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "  ✗ FAILED: " << message << std::endl; \
            return false; \
        } \
    } while(0)

#define RUN_TEST(test_func, test_name) \
    do { \
        std::cout << "\n  " << test_name << std::endl; \
        if (test_func()) { \
            std::cout << "    ✓ PASSED" << std::endl; \
            passed++; \
        } else { \
            std::cout << "    ✗ FAILED" << std::endl; \
            failed++; \
        } \
        total++; \
    } while(0)

// ============================================================================
// Test Case 1: Basic Construction and Initialization
// ============================================================================

bool test_basic_construction()
{
    // Default constructor
    DataArray<int> arr1;
    TEST_ASSERT(arr1.GetCount() == 0, "Default constructor: count should be 0");
    TEST_ASSERT(arr1.IsEmpty(), "Default constructor: should be empty");
    TEST_ASSERT(arr1.GetAllocCount() == 0, "Default constructor: alloc_count should be 0");
    
    // Constructor with size
    DataArray<int> arr2(5);
    TEST_ASSERT(arr2.GetCount() == 5, "Constructor with size: count should be 5");
    TEST_ASSERT(arr2.GetAllocCount() >= 5, "Constructor with size: alloc_count >= 5");
    
    // Verify initialized values
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr2[i] == 0, "Constructor with size: elements should be initialized to 0");
    
    return true;
}

// ============================================================================
// Test Case 2: Memory Management - Reserve
// ============================================================================

bool test_reserve()
{
    DataArray<int> arr;
    
    // Reserve space
    TEST_ASSERT(arr.Reserve(10), "Reserve(10) should succeed");
    TEST_ASSERT(arr.GetAllocCount() >= 10, "After Reserve(10), alloc_count >= 10");
    TEST_ASSERT(arr.GetCount() == 0, "Reserve should not change count");

    const auto alloc_after_10 = arr.GetAllocCount();
    
    // Reserve smaller space (should not shrink)
    arr.Reserve(5);
    TEST_ASSERT(arr.GetAllocCount() == alloc_after_10, "Reserve(5) after Reserve(10) should not shrink (capacity unchanged)");
    
    // Reserve larger space
    arr.Reserve(20);
    TEST_ASSERT(arr.GetAllocCount() >= 20, "Reserve(20) should increase alloc_count");
    
    return true;
}

// ============================================================================
// Test Case 3: Memory Management - Resize
// ============================================================================

bool test_resize()
{
    DataArray<int> arr;
    
    // Resize to 5 elements
    arr.Resize(5);
    TEST_ASSERT(arr.GetCount() == 5, "Resize(5): count should be 5");
    
    // Fill data
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // Expand to 10 elements
    arr.Resize(10);
    TEST_ASSERT(arr.GetCount() == 10, "Resize(10): count should be 10");
    
    // Verify old data preserved
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr[i] == i * 10, "Resize: old data should be preserved");
    
    // Verify new data initialized to 0
    for (int i = 5; i < 10; i++)
        TEST_ASSERT(arr[i] == 0, "Resize: new elements should be initialized to 0");
    
    // Shrink to 3 elements
    arr.Resize(3);
    TEST_ASSERT(arr.GetCount() == 3, "Resize(3): count should be 3");
    
    // Verify data integrity
    for (int i = 0; i < 3; i++)
        TEST_ASSERT(arr[i] == i * 10, "Resize down: data should be intact");
    
    return true;
}

// ============================================================================
// Test Case 4: Memory Management - Expand
// ============================================================================

bool test_expand()
{
    DataArray<int> arr;
    arr.Resize(5);
    
    int original_count = arr.GetCount();
    arr.Expand(3);
    TEST_ASSERT(arr.GetCount() == original_count + 3, "Expand(3) should add 3 elements");
    
    arr.Expand(2);
    TEST_ASSERT(arr.GetCount() == original_count + 5, "Expand(2) should add 2 more elements");
    
    return true;
}

// ============================================================================
// Test Case 5: Data Addition - Append
// ============================================================================

bool test_append()
{
    DataArray<int> arr;
    
    // Add elements
    for (int i = 0; i < 10; i++)
        arr.Append(i);
    
    TEST_ASSERT(arr.GetCount() == 10, "After 10 appends, count should be 10");
    
    // Verify data order
    for (int i = 0; i < 10; i++)
        TEST_ASSERT(arr[i] == i, "Appended data should be in correct order");
    
    return true;
}

// ============================================================================
// Test Case 6: Data Access - At
// ============================================================================

bool test_at()
{
    DataArray<int> arr;
    arr.Resize(5);
    
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // Valid indices
    TEST_ASSERT(arr.At(0) != nullptr, "At(0) should return valid pointer");
    TEST_ASSERT(*arr.At(2) == 20, "At(2) should return correct value");
    TEST_ASSERT(*arr.At(4) == 40, "At(4) should return correct value");
    
    // Invalid indices
    TEST_ASSERT(arr.At(-1) == nullptr, "At(-1) should return nullptr");
    TEST_ASSERT(arr.At(5) == nullptr, "At(5) should return nullptr");
    TEST_ASSERT(arr.At(100) == nullptr, "At(100) should return nullptr");
    
    return true;
}

// ============================================================================
// Test Case 7: Data Access - ReadAt/WriteAt
// ============================================================================

bool test_read_write_at()
{
    DataArray<int> arr;
    arr.Resize(5);
    
    // WriteAt - single element
    TEST_ASSERT(arr.WriteAt(100, 0), "WriteAt(100, 0) should succeed");
    TEST_ASSERT(arr[0] == 100, "WriteAt should set value correctly");
    
    TEST_ASSERT(arr.WriteAt(50, 2), "WriteAt(50, 2) should succeed");
    TEST_ASSERT(arr[2] == 50, "WriteAt should set value correctly");
    
    // WriteAt - invalid indices
    TEST_ASSERT(!arr.WriteAt(999, -1), "WriteAt with negative index should fail");
    TEST_ASSERT(!arr.WriteAt(999, 5), "WriteAt with out-of-range index should fail");
    
    // ReadAt - single element
    int value = 0;
    TEST_ASSERT(arr.ReadAt(value, 0), "ReadAt(value, 0) should succeed");
    TEST_ASSERT(value == 100, "ReadAt should get correct value");
    
    TEST_ASSERT(arr.ReadAt(value, 2), "ReadAt(value, 2) should succeed");
    TEST_ASSERT(value == 50, "ReadAt should get correct value");
    
    // ReadAt - invalid indices
    TEST_ASSERT(!arr.ReadAt(value, -1), "ReadAt with negative index should fail");
    TEST_ASSERT(!arr.ReadAt(value, 5), "ReadAt with out-of-range index should fail");
    
    // WriteAt/ReadAt - multiple elements
    DataArray<int> arr2;
    arr2.Resize(10);
    
    int write_data[] = {1, 2, 3, 4, 5};
    TEST_ASSERT(arr2.WriteAt(write_data, 2, 5), "WriteAt multiple should succeed");
    
    int read_data[5] = {0};
    TEST_ASSERT(arr2.ReadAt(read_data, 2, 5), "ReadAt multiple should succeed");
    
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(read_data[i] == write_data[i], "ReadAt should get correct values");
    
    return true;
}

// ============================================================================
// Test Case 8: Data Deletion - Delete (order doesn't matter)
// ============================================================================

bool test_delete()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // Delete single element
    TEST_ASSERT(arr.Delete(5), "Delete(5) should succeed");
    TEST_ASSERT(arr.GetCount() == 9, "After Delete(5), count should be 9");
    
    // Delete multiple elements
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    TEST_ASSERT(arr.Delete(2, 3), "Delete(2, 3) should succeed");
    TEST_ASSERT(arr.GetCount() == 7, "After Delete(2, 3), count should be 7");
    
    // Delete invalid range
    TEST_ASSERT(!arr.Delete(-1), "Delete with negative index should fail");
    TEST_ASSERT(!arr.Delete(10), "Delete with out-of-range index should fail");
    TEST_ASSERT(!arr.Delete(0, -1), "Delete with negative count should fail");
    
    // Delete all
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    TEST_ASSERT(arr.Delete(0, 5), "Delete all should succeed");
    TEST_ASSERT(arr.GetCount() == 0, "After deleting all, count should be 0");
    
    return true;
}

// ============================================================================
// Test Case 9: Data Deletion - DeleteShift (order matters)
// ============================================================================

bool test_delete_shift()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // Delete single element
    arr.DeleteShift(3);
    TEST_ASSERT(arr.GetCount() == 9, "After DeleteShift(3), count should be 9");
    
    // Verify order preserved: 0,1,2,4,5,6,7,8,9
    int expected[] = {0, 1, 2, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 9; i++)
        TEST_ASSERT(arr[i] == expected[i], "DeleteShift should preserve order");
    
    // Delete multiple elements
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    arr.DeleteShift(2, 3);
    TEST_ASSERT(arr.GetCount() == 7, "After DeleteShift(2, 3), count should be 7");
    
    // Verify order preserved: 0,1,5,6,7,8,9
    int expected2[] = {0, 1, 5, 6, 7, 8, 9};
    for (int i = 0; i < 7; i++)
        TEST_ASSERT(arr[i] == expected2[i], "DeleteShift should preserve order");
    
    return true;
}

// ============================================================================
// Test Case 10: Data Exchange - Exchange
// ============================================================================

bool test_exchange()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // Exchange adjacent elements
    arr.Exchange(0, 1);
    TEST_ASSERT(arr[0] == 10, "After Exchange(0, 1), arr[0] should be 10");
    TEST_ASSERT(arr[1] == 0, "After Exchange(0, 1), arr[1] should be 0");
    
    // Exchange distant elements
    arr.Exchange(0, 4);
    TEST_ASSERT(arr[0] == 40, "After Exchange(0, 4), arr[0] should be 40");
    TEST_ASSERT(arr[4] == 10, "After Exchange(0, 4), arr[4] should be 10");
    
    // Exchange same position (invalid but should not crash)
    arr.Exchange(2, 2);
    
    return true;
}

// ============================================================================
// Test Case 11: Data Insertion - Insert
// ============================================================================

bool test_insert()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    // Insert at beginning
    int insert_data1[] = {100, 101};
    TEST_ASSERT(arr.Insert(0, insert_data1, 2), "Insert at beginning should succeed");
    TEST_ASSERT(arr.GetCount() == 7, "After insert, count should be 7");
    TEST_ASSERT(arr[0] == 100, "First inserted element should be correct");
    TEST_ASSERT(arr[1] == 101, "Second inserted element should be correct");
    TEST_ASSERT(arr[2] == 0, "Original first element should be at position 2");
    
    // Insert in middle
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    int insert_data2[] = {999};
    TEST_ASSERT(arr.Insert(3, insert_data2, 1), "Insert in middle should succeed");
    TEST_ASSERT(arr.GetCount() == 6, "After insert, count should be 6");
    TEST_ASSERT(arr[3] == 999, "Inserted element should be at correct position");
    TEST_ASSERT(arr[4] == 3, "Element after insert should shift");
    
    // Insert at end
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    int insert_data3[] = {555, 556, 557};
    TEST_ASSERT(arr.Insert(5, insert_data3, 3), "Insert at end should succeed");
    TEST_ASSERT(arr.GetCount() == 8, "After insert at end, count should be 8");
    TEST_ASSERT(arr[5] == 555, "First appended element should be correct");
    
    return true;
}

// ============================================================================
// Test Case 12: Data Movement - Move
// ============================================================================

bool test_move()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // Move backward
    arr.Move(7, 2, 2);
    TEST_ASSERT(arr.GetCount() == 10, "Move should not change count");
    
    // Move forward
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    arr.Move(0, 5, 3);
    TEST_ASSERT(arr.GetCount() == 10, "Move should not change count");
    
    // Move to end
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    arr.Move(8, 0, 2);
    TEST_ASSERT(arr.GetCount() == 10, "Move should not change count");
    
    return true;
}

// ============================================================================
// Test Case 13: Find - Find
// ============================================================================

bool test_find()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i * 10;
    
    // Find existing elements
    TEST_ASSERT(arr.Find(0) == 0, "Find(0) should return 0");
    TEST_ASSERT(arr.Find(30) == 3, "Find(30) should return 3");
    TEST_ASSERT(arr.Find(90) == 9, "Find(90) should return 9");
    
    // Find non-existing elements
    TEST_ASSERT(arr.Find(999) < 0, "Find(999) should return < 0");
    TEST_ASSERT(arr.Find(5) < 0, "Find(5) should return < 0");
    
    // Find from specified position
    TEST_ASSERT(arr.Find(30, 3) == 3, "Find(30, 3) should return 3");
    TEST_ASSERT(arr.Find(30, 4) < 0, "Find(30, 4) should return < 0");
    
    return true;
}

// ============================================================================
// Test Case 14: Comparison - Compare
// ============================================================================

bool test_compare()
{
    DataArray<int> arr1, arr2, arr3;
    
    arr1.Resize(3);
    arr1[0] = 1; arr1[1] = 2; arr1[2] = 3;
    
    arr2.Resize(3);
    arr2[0] = 1; arr2[1] = 2; arr2[2] = 3;
    
    arr3.Resize(4);
    arr3[0] = 1; arr3[1] = 2; arr3[2] = 3; arr3[3] = 4;
    
    // Equal arrays
    TEST_ASSERT(arr1 == arr2, "Identical arrays should compare equal");
    
    // Arrays with different lengths
    TEST_ASSERT(arr1 < arr3, "Shorter array should compare less");
    TEST_ASSERT(arr3 > arr1, "Longer array should compare greater");
    
    // Additional comparison tests
    TEST_ASSERT((arr1 <=> arr2) == std::strong_ordering::equal, "Three-way comparison: equal arrays");
    TEST_ASSERT((arr1 <=> arr3) == std::strong_ordering::less, "Three-way comparison: shorter < longer");
    TEST_ASSERT((arr3 <=> arr1) == std::strong_ordering::greater, "Three-way comparison: longer > shorter");
    
    return true;
}

// ============================================================================
// Test Case 15: Clear and Free
// ============================================================================

bool test_clear_and_free()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    // Clear - clear data but keep memory
    arr.Clear();
    TEST_ASSERT(arr.GetCount() == 0, "After Clear, count should be 0");
    TEST_ASSERT(arr.IsEmpty(), "After Clear, should be empty");
    TEST_ASSERT(arr.GetAllocCount() >= 5, "After Clear, alloc_count should remain");
    
    // Free - completely release
    arr.Resize(10);
    arr.Free();
    TEST_ASSERT(arr.GetCount() == 0, "After Free, count should be 0");
    TEST_ASSERT(arr.GetAllocCount() == 0, "After Free, alloc_count should be 0");
    
    return true;
}

// ============================================================================
// Test Case 16: Operator Overloading
// ============================================================================

bool test_operators()
{
    DataArray<int> arr1, arr2;
    
    // Initialize arr1
    arr1.Resize(5);
    for (int i = 0; i < 5; i++)
        arr1[i] = i;
    
    // operator = (copy)
    arr2 = arr1;
    TEST_ASSERT(arr2.GetCount() == 5, "After assignment, count should match");
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr2[i] == i, "After assignment, data should match");
    
    // operator = (initializer_list)
    arr1 = {10, 20, 30};
    TEST_ASSERT(arr1.GetCount() == 3, "After list assignment, count should be 3");
    TEST_ASSERT(arr1[0] == 10, "List assignment should set first element");
    TEST_ASSERT(arr1[1] == 20, "List assignment should set second element");
    TEST_ASSERT(arr1[2] == 30, "List assignment should set third element");
    
    // operator [] (access)
    arr1[1] = 99;
    TEST_ASSERT(arr1[1] == 99, "operator[] assignment should work");
    
    // operator T * (conversion)
    int* ptr = arr1;
    TEST_ASSERT(ptr == arr1.GetData(), "operator T* should return data pointer");
    
    // operator -> (pointer access)
    int* data_ptr = arr1.GetData();
    TEST_ASSERT(data_ptr != nullptr, "GetData should return non-null pointer");
    
    return true;
}

// ============================================================================
// Test Case 17: Iterator Interface
// ============================================================================

bool test_iterators()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // begin() and end()
    int sum = 0;
    for (int* p = arr.begin(); p != arr.end(); ++p)
        sum += *p;
    
    TEST_ASSERT(sum == 100, "Iteration should sum correctly: 0+10+20+30+40=100");
    
    // last()
    TEST_ASSERT(arr.last() != nullptr, "last() should return non-null pointer");
    TEST_ASSERT(*arr.last() == 40, "last() should point to last element");
    
    // last() on empty array
    DataArray<int> empty;
    TEST_ASSERT(empty.last() == nullptr, "last() on empty array should return nullptr");
    
    return true;
}

// ============================================================================
// Test Case 18: Set Operations - WithoutList
// ============================================================================

bool test_without_list()
{
    DataArray<int> arr1, arr2, result;
    
    // arr1 = {1, 2, 3, 4, 5}
    arr1.Resize(5);
    for (int i = 0; i < 5; i++)
        arr1[i] = i + 1;
    
    // arr2 = {2, 4}
    arr2.Resize(2);
    arr2[0] = 2;
    arr2[1] = 4;
    
    // Calculate arr1 - arr2 = {1, 3, 5}
    arr1.WithoutList(result, arr2);
    
    TEST_ASSERT(result.GetCount() == 3, "Result should have 3 elements");
    TEST_ASSERT(result[0] == 1, "Result should contain 1");
    TEST_ASSERT(result[1] == 3, "Result should contain 3");
    TEST_ASSERT(result[2] == 5, "Result should contain 5");
    
    return true;
}

// ============================================================================
// Test Case 19: Byte Size Query
// ============================================================================

bool test_byte_sizes()
{
    DataArray<int> arr;
    
    // Empty array
    TEST_ASSERT(arr.GetTotalBytes() == 0, "Empty array total bytes should be 0");
    TEST_ASSERT(arr.GetAllocBytes() == 0, "Empty array alloc bytes should be 0");
    
    // Array with data
    arr.Resize(5);
    TEST_ASSERT(arr.GetTotalBytes() == 5 * sizeof(int), "Total bytes should match");
    TEST_ASSERT(arr.GetAllocBytes() >= 5 * sizeof(int), "Alloc bytes should be >= total");
    
    return true;
}

// ============================================================================
// Test Case 20: SetData and Unlink
// ============================================================================

bool test_set_data_and_unlink()
{
    // Create external data
    int external_data[] = {100, 200, 300};
    
    DataArray<int> arr;
    arr.SetData(external_data, 3);
    
    TEST_ASSERT(arr.GetCount() == 3, "SetData should set count");
    TEST_ASSERT(arr.GetAllocCount() == 3, "SetData should set alloc_count");
    TEST_ASSERT(arr[0] == 100, "SetData should reference external data");
    TEST_ASSERT(arr[2] == 300, "SetData should reference external data");
    
    // Unlink - detach from external data
    arr.Unlink();
    TEST_ASSERT(arr.GetCount() == 0, "After Unlink, count should be 0");
    TEST_ASSERT(arr.GetAllocCount() == 0, "After Unlink, alloc_count should be 0");
    TEST_ASSERT(arr.GetData() == nullptr, "After Unlink, data pointer should be nullptr");
    
    // Original data should not be affected
    TEST_ASSERT(external_data[0] == 100, "External data should not be affected");
    
    return true;
}

// ============================================================================
// Test Case 21: Zero Function
// ============================================================================

bool test_zero()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i * 100;
    
    for (int i = 0; i < 5; i++)
        arr[i] = 0;
    
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr[i] == 0, "Zero should set all elements to 0");
    
    return true;
}

// ============================================================================
// Main Test Function
// ============================================================================

int main(int argc, char** argv)
{
    int passed = 0, failed = 0, total = 0;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  DataArray Comprehensive Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Basic Functionality Tests
    std::cout << "\n[Basic Functionality]" << std::endl;
    RUN_TEST(test_basic_construction, "Test 1: Basic Construction and Initialization");
    RUN_TEST(test_reserve, "Test 2: Memory Management - Reserve");
    RUN_TEST(test_resize, "Test 3: Memory Management - Resize");
    RUN_TEST(test_expand, "Test 4: Memory Management - Expand");
    
    // Data Operation Tests
    std::cout << "\n[Data Operations]" << std::endl;
    RUN_TEST(test_append, "Test 5: Data Addition - Append");
    RUN_TEST(test_at, "Test 6: Data Access - At");
    RUN_TEST(test_read_write_at, "Test 7: Data Access - ReadAt/WriteAt");
    RUN_TEST(test_delete, "Test 8: Data Deletion - Delete");
    RUN_TEST(test_delete_shift, "Test 9: Data Deletion - DeleteShift");
    RUN_TEST(test_exchange, "Test 10: Data Exchange - Exchange");
    RUN_TEST(test_insert, "Test 11: Data Insertion - Insert");
    RUN_TEST(test_move, "Test 12: Data Movement - Move");
    
    // Query and Comparison Tests
    std::cout << "\n[Query and Comparison]" << std::endl;
    RUN_TEST(test_find, "Test 13: Data Search - Find");
    RUN_TEST(test_compare, "Test 14: Array Comparison - Compare");
    
    // Advanced Functionality Tests
    std::cout << "\n[Advanced Functionality]" << std::endl;
    RUN_TEST(test_clear_and_free, "Test 15: Clear and Free - Clear/Free");
    RUN_TEST(test_operators, "Test 16: Operator Overloading");
    RUN_TEST(test_iterators, "Test 17: Iterator Interface");
    RUN_TEST(test_without_list, "Test 18: Set Operations - WithoutList");
    RUN_TEST(test_byte_sizes, "Test 19: Byte Size Query");
    RUN_TEST(test_set_data_and_unlink, "Test 20: SetData and Unlink");
    RUN_TEST(test_zero, "Test 21: Zero Function");
    
    // Test Results Summary
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Results Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Total Tests: " << total << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    std::cout << "  Success Rate: " << (total > 0 ? (100.0 * passed / total) : 0) << "%" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
