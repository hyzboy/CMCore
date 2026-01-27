/**
 * ValueBuffer Boundary Cases and Performance Testing
 *
 * This test covers boundary conditions and special cases:
 * 1. Empty array operations
 * 2. Single element array operations
 * 3. Large-scale data operations
 * 4. Boundary index operations
 * 5. Memory overlap scenarios
 * 6. Performance benchmark testing
 */

#include<hgl/type/ValueBuffer.h>
#include<iostream>
#include<iomanip>
#include<chrono>
#include<vector>
#include<cstring>

using namespace hgl;

// ============================================================================
// Test Helper Tools
// ============================================================================

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "    ✗ " << message << std::endl; \
            return false; \
        } \
    } while(0)

template<typename T>
void print_array(const ValueBuffer<T>& arr, const char* label, int max_print = 10)
{
    std::cout << "  " << label << " [" << arr.GetCount() << "]: ";
    int count = std::min((int)arr.GetCount(), max_print);
    for (int i = 0; i < count; i++)
    {
        if (i > 0) std::cout << ",";
        std::cout << arr[i];
    }
    if (arr.GetCount() > max_print)
        std::cout << ",...(" << arr.GetCount() - max_print << " more)";
    std::cout << std::endl;
}

// ============================================================================
// Test Case 1: Empty Array Boundary
// ============================================================================

bool test_empty_array_boundary()
{
    std::cout << "\n[Test 1: Empty Array Boundary]" << std::endl;

    ValueBuffer<int> arr;

    // Operations on empty array
    TEST_ASSERT(arr.IsEmpty(), "new array should be empty");
    TEST_ASSERT(arr.GetCount() == 0, "empty array count should be 0");
    TEST_ASSERT(arr.last() == nullptr, "last() on empty array should return nullptr");
    TEST_ASSERT(arr.At(0) == nullptr, "At(0) on empty array should return nullptr");

    // Find should not crash
    TEST_ASSERT(arr.Find(999) < 0, "find in empty array should return -1");

    // Clear empty array
    arr.Clear();
    TEST_ASSERT(arr.GetCount() == 0, "after clear, array should still be empty");

    // Free empty array
    arr.Free();
    TEST_ASSERT(arr.GetCount() == 0, "after free, array should still be empty");

    return true;
}

// ============================================================================
// Test Case 2: Single Element Array
// ============================================================================

bool test_single_element_array()
{
    std::cout << "\n[Test 2: Single Element Array]" << std::endl;

    ValueBuffer<int> arr;
    arr.Append(42);

    TEST_ASSERT(arr.GetCount() == 1, "single element array count should be 1");
    TEST_ASSERT(arr[0] == 42, "element value should be correct");
    TEST_ASSERT(arr.last() != nullptr, "last() should point to the element");
    TEST_ASSERT(*arr.last() == 42, "last() value should be correct");

    TEST_ASSERT(arr.Find(42) == 0, "Find should locate the element");
    TEST_ASSERT(arr.At(0) != nullptr, "At(0) should return valid pointer");

    // Delete the only element
    arr.Delete(0);
    TEST_ASSERT(arr.GetCount() == 0, "after delete, should be empty");

    return true;
}

// ============================================================================
// Test Case 3: Boundary Index Operations
// ============================================================================

bool test_boundary_index_operations()
{
    std::cout << "\n[Test 3: Boundary Index Operations]" << std::endl;

    ValueBuffer<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;

    // Access first element
    TEST_ASSERT(arr.At(0) != nullptr, "At(0) should be valid");
    TEST_ASSERT(*arr.At(0) == 0, "first element should be correct");

    // Access last element
    TEST_ASSERT(arr.At(9) != nullptr, "At(9) should be valid");
    TEST_ASSERT(*arr.At(9) == 9, "last element should be correct");

    // Out of bounds access
    TEST_ASSERT(arr.At(-1) == nullptr, "At(-1) should return nullptr");
    TEST_ASSERT(arr.At(10) == nullptr, "At(10) should return nullptr");
    TEST_ASSERT(arr.At(1000) == nullptr, "At(1000) should return nullptr");

    // Delete first element
    arr.Delete(0);
    TEST_ASSERT(arr.GetCount() == 9, "after delete first, count should be 9");

    // Delete last element
    arr.Delete(8);
    TEST_ASSERT(arr.GetCount() == 8, "after delete last, count should be 8");

    // Insert at boundary
    int data[] = {99};
    arr.Insert(0, data, 1);
    TEST_ASSERT(arr[0] == 99, "insert at start should succeed");

    return true;
}

// ============================================================================
// Test Case 4: Large Scale Operations
// ============================================================================

bool test_large_scale_operations()
{
    std::cout << "\n[Test 4: Large Scale Operations (10000 elements)]" << std::endl;

    ValueBuffer<int> arr;

    // Add large amount of elements
    std::cout << "  Adding 10000 elements..." << std::endl;
    for (int i = 0; i < 10000; i++)
        arr.Append(i);

    TEST_ASSERT(arr.GetCount() == 10000, "should have 10000 elements");

    // Verify data integrity
    std::cout << "  Verifying data integrity..." << std::endl;
    bool data_valid = true;
    for (int i = 0; i < 10000; i++)
    {
        if (arr[i] != i)
        {
            data_valid = false;
            break;
        }
    }
    TEST_ASSERT(data_valid, "all data should be correct");

    // Delete half
    std::cout << "  Deleting 5000 elements..." << std::endl;
    arr.Delete(0, 5000);
    TEST_ASSERT(arr.GetCount() == 5000, "should have 5000 elements");

    // Verify data after deletion
    std::cout << "  Verifying data after deletion..." << std::endl;
    for (int i = 0; i < (int)arr.GetCount(); i++)
    {
        if (arr[i] >= 5000)
            data_valid = false;
    }

    return true;
}

// ============================================================================
// Test Case 5: Multiple Reserve Operations
// ============================================================================

bool test_multiple_reserve_operations()
{
    std::cout << "\n[Test 5: Multiple Reserve Operations]" << std::endl;

    ValueBuffer<int> arr;

    // First reserve
    arr.Reserve(100);
    int64 alloc1 = arr.GetAllocCount();
    TEST_ASSERT(alloc1 >= 100, "first reserve should be >= 100");

    // Multiple expansions
    for (int i = 0; i < 5; i++)
    {
        arr.Reserve(alloc1 * 2);
        int64 alloc_new = arr.GetAllocCount();
        TEST_ASSERT(alloc_new >= alloc1, "reserve should be monotonically increasing");
        alloc1 = alloc_new;
    }

    // Data should still be empty (Reserve does not change count)
    TEST_ASSERT(arr.GetCount() == 0, "Reserve should not change count");

    // Now adding data should be fast
    for (int i = 0; i < 100; i++)
        arr.Append(i);

    TEST_ASSERT(arr.GetCount() == 100, "should successfully add 100 elements");

    return true;
}

// ============================================================================
// Test Case 6: Consecutive Delete Operations
// ============================================================================

bool test_consecutive_deletes()
{
    std::cout << "\n[Test 6: Consecutive Delete Operations]" << std::endl;

    ValueBuffer<int> arr;
    arr.Resize(20);
    for (int i = 0; i < 20; i++)
        arr[i] = i;

    // Delete from same position multiple times
    while (arr.GetCount() > 0)
    {
        arr.Delete(0);
        // Verify remaining data is valid
        for (int64 i = 0; i < arr.GetCount(); i++)
            if (arr[i] < 0 || arr[i] >= 20)
                return false;
    }

    TEST_ASSERT(arr.GetCount() == 0, "all elements should be deleted");

    return true;
}

// ============================================================================
// Test Case 7: Consecutive DeleteShift Operations
// ============================================================================

bool test_consecutive_delete_shifts()
{
    std::cout << "\n[Test 7: Consecutive DeleteShift Operations]" << std::endl;

    ValueBuffer<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;

    // Delete from back to front (avoid index shift issues)
    while (arr.GetCount() > 0)
    {
        arr.DeleteShift(0);
    }

    TEST_ASSERT(arr.GetCount() == 0, "all elements should be deleted");

    return true;
}

// ============================================================================
// Test Case 8: Random Access Pattern
// ============================================================================

bool test_random_access_pattern()
{
    std::cout << "\n[Test 8: Random Access Pattern]" << std::endl;

    ValueBuffer<int> arr;

    // Create array
    for (int i = 0; i < 100; i++)
        arr.Append(i * 10);

    // Random access verification
    TEST_ASSERT(*arr.At(0) == 0, "At(0) should be correct");
    TEST_ASSERT(*arr.At(50) == 500, "At(50) should be correct");
    TEST_ASSERT(*arr.At(99) == 990, "At(99) should be correct");

    // Using [] access
    TEST_ASSERT(arr[0] == 0, "operator[0] should be correct");
    TEST_ASSERT(arr[50] == 500, "operator[50] should be correct");
    TEST_ASSERT(arr[99] == 990, "operator[99] should be correct");

    return true;
}

// ============================================================================
// Test Case 9: Memory Overlap Scenarios (Move)
// ============================================================================

bool test_memory_overlap_scenarios()
{
    std::cout << "\n[Test 9: Memory Overlap Scenarios (Move)]" << std::endl;

    ValueBuffer<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;

    // Source and target ranges overlap (move backward)
    std::cout << "  Overlapping move (backward): Move(6, 2, 3)" << std::endl;
    arr.Move(6, 2, 3);
    // Result: 0,1,4,5,6,7,2,3,8,9
    TEST_ASSERT(arr.GetCount() == 10, "element count should remain");

    // Source and target ranges overlap (move forward)
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;

    std::cout << "  Overlapping move (forward): Move(2, 6, 3)" << std::endl;
    arr.Move(2, 6, 3);
    TEST_ASSERT(arr.GetCount() == 10, "element count should remain");

    return true;
}

// ============================================================================
// Performance Benchmark Testing
// ============================================================================

bool test_performance_benchmarks()
{
    std::cout << "\n[Performance Benchmark Testing]" << std::endl;

    // Benchmark 1: Append performance
    {
        std::cout << "  Benchmark 1: Append 100000 elements..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        ValueBuffer<int> arr;
        for (int i = 0; i < 100000; i++)
            arr.Append(i);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "    Time: " << duration.count() << " ms" << std::endl;
        std::cout << "    Rate: " << (100000.0 / duration.count() * 1000) << " ops/sec" << std::endl;

        TEST_ASSERT(arr.GetCount() == 100000, "should add 100000 elements");
    }

    // Benchmark 2: Reserve + Append performance
    {
        std::cout << "  Benchmark 2: Reserve + Append 100000 elements..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();

        ValueBuffer<int> arr;
        arr.Reserve(100000);
        for (int i = 0; i < 100000; i++)
            arr.Append(i);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "    Time: " << duration.count() << " ms" << std::endl;
        std::cout << "    Rate: " << (100000.0 / duration.count() * 1000) << " ops/sec" << std::endl;
    }

    // Benchmark 3: Random access performance
    {
        std::cout << "  Benchmark 3: Random access 1000000 times..." << std::endl;

        ValueBuffer<int> arr;
        for (int i = 0; i < 1000; i++)
            arr.Append(i);

        auto start = std::chrono::high_resolution_clock::now();

        int sum = 0;
        for (int i = 0; i < 1000000; i++)
            sum += arr[i % arr.GetCount()];

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "    Time: " << duration.count() << " ms" << std::endl;
        std::cout << "    Rate: " << (1000000.0 / duration.count() * 1000) << " ops/sec" << std::endl;
        std::cout << "    (sum=" << sum << " - prevent compiler optimization)" << std::endl;
    }

    // Benchmark 4: Delete performance
    {
        std::cout << "  Benchmark 4: Delete operations..." << std::endl;

        ValueBuffer<int> arr;
        for (int i = 0; i < 10000; i++)
            arr.Append(i);

        auto start = std::chrono::high_resolution_clock::now();

        while (arr.GetCount() > 0)
            arr.Delete(0);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "    Time to delete 10000 elements: " << duration.count() << " ms" << std::endl;
    }

    return true;
}

// ============================================================================
// Test Case 10: Array Comparisons
// ============================================================================

bool test_array_comparisons()
{
    std::cout << "\n[Test 10: Array Comparisons]" << std::endl;

    ValueBuffer<int> arr1, arr2, arr3;

    // Same content, different objects
    for (int i = 0; i < 5; i++)
    {
        arr1.Append(i * 10);
        arr2.Append(i * 10);
    }
    TEST_ASSERT(arr1 == arr2, "arrays with same content should be equal");

    // Different sizes - arr1 has more elements but smaller first element
    arr3.Append(100);
    TEST_ASSERT(arr1 < arr3, "array with smaller first element should compare less");

    // Same size, different content
    ValueBuffer<int> arr4, arr5;
    arr4.Append(1);
    arr4.Append(2);
    arr5.Append(1);
    arr5.Append(3);
    TEST_ASSERT(arr4 != arr5, "arrays with different content should not be equal");
    TEST_ASSERT(arr4 < arr5, "arr4[1]=2 should be less than arr5[1]=3");

    return true;
}

// ============================================================================
// Main Test Function
// ============================================================================

int main(int argc, char** argv)
{
    int passed = 0, failed = 0;

    std::cout << "========================================" << std::endl;
    std::cout << "  ValueBuffer Boundary and Performance Test" << std::endl;
    std::cout << "========================================" << std::endl;

    if (test_empty_array_boundary()) { passed++; } else { failed++; }
    if (test_single_element_array()) { passed++; } else { failed++; }
    if (test_boundary_index_operations()) { passed++; } else { failed++; }
    if (test_large_scale_operations()) { passed++; } else { failed++; }
    if (test_multiple_reserve_operations()) { passed++; } else { failed++; }
    if (test_consecutive_deletes()) { passed++; } else { failed++; }
    if (test_consecutive_delete_shifts()) { passed++; } else { failed++; }
    if (test_random_access_pattern()) { passed++; } else { failed++; }
    if (test_memory_overlap_scenarios()) { passed++; } else { failed++; }
    if (test_performance_benchmarks()) { passed++; } else { failed++; }
    if (test_array_comparisons()) { passed++; } else { failed++; }

    // Summary of test results
    std::cout << "\n========================================" << std::endl;
    std::cout << "  Test Results Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Total tests: " << (passed + failed) << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    std::cout << "========================================" << std::endl;

    return (failed == 0) ? 0 : 1;
}
