/**
 * DataArrayNew 功能测试
 * 验证新架构的所有功能
 */

#include <hgl/type/DataArrayNew.h>
#include <iostream>
#include <string>

using namespace hgl;

#define TEST_SECTION(name) std::cout << "\n=== " << name << " ===" << std::endl

void test_trivial_types()
{
    TEST_SECTION("Trivial Types Test");

    // 基本操作
    DataArray<int> arr;
    arr.Append(1);
    arr.Append(2);
    arr.Append(3);

    std::cout << "Count: " << arr.GetCount() << std::endl;
    std::cout << "Elements: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Resize
    arr.Resize(5);
    std::cout << "After Resize(5), count: " << arr.GetCount() << std::endl;

    // Insert
    int data[] = {10, 20};
    arr.Insert(2, data, 2);
    std::cout << "After Insert at 2: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Delete
    arr.Delete(3, 2);
    std::cout << "After Delete(3, 2): ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Find & Contains
    std::cout << "Find(10): " << arr.Find(10) << std::endl;
    std::cout << "Contains(10): " << (arr.Contains(10) ? "true" : "false") << std::endl;
    std::cout << "Contains(999): " << (arr.Contains(999) ? "true" : "false") << std::endl;

    // Exchange
    arr.Exchange(0, arr.GetCount() - 1);
    std::cout << "After Exchange(0, last): ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // ReadAt/WriteAt
    int value;
    arr.ReadAt(value, 0);
    std::cout << "ReadAt(0): " << value << std::endl;

    arr.WriteAt(999, 0);
    std::cout << "After WriteAt(999, 0): " << arr[0] << std::endl;

    // operator=
    DataArray<int> arr2;
    arr2 = arr;
    std::cout << "After arr2 = arr, arr2.count: " << arr2.GetCount() << std::endl;

    // initializer_list
    arr = {100, 200, 300};
    std::cout << "After arr = {100, 200, 300}: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // WithoutList
    DataArray<int> arr3;
    arr3.Append(100);
    arr3.Append(300);
    arr3.Append(500);

    DataArray<int> without;
    without.Append(300);

    DataArray<int> result;
    arr3.WithoutList(result, without);

    std::cout << "WithoutList result: ";
    for(int i = 0; i < result.GetCount(); i++)
        std::cout << result[i] << " ";
    std::cout << std::endl;
}

void test_non_trivial_types()
{
    TEST_SECTION("Non-Trivial Types Test (std::string)");

    DataArray<std::string> arr;
    arr.Append("Hello");
    arr.Append("World");
    arr.Append("!");

    std::cout << "Count: " << arr.GetCount() << std::endl;
    std::cout << "Elements: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Resize
    arr.Resize(5);
    arr[3] = "C++";
    arr[4] = "Test";

    std::cout << "After Resize and assign: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Insert - step by step
    std::cout << "About to Insert..." << std::endl;
    std::cout << "Current count: " << arr.GetCount() << ", alloc: " << arr.GetAllocCount() << std::endl;

    std::cout << "Creating test array..." << std::endl;
    std::string insert_data[] = {"Insert1", "Insert2"};
    std::cout << "Test data created: " << insert_data[0] << ", " << insert_data[1] << std::endl;

    std::cout << "Calling arr.Insert(2, insert_data, 2)..." << std::endl;
    std::cout.flush();

    bool insert_result = arr.Insert(2, insert_data, 2);

    std::cout << "Insert returned: " << (insert_result ? "true" : "false") << std::endl;
    std::cout.flush();

    int64 count_after = arr.GetCount();
    std::cout << "After Insert, count: " << count_after << std::endl;
    std::cout << "After Insert: ";
    for(int i = 0; i < count_after; i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Delete
    arr.DeleteShift(3, 2);

    std::cout << "After DeleteShift: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Find
    std::cout << "Find(\"World\"): " << arr.Find(std::string("World")) << std::endl;
    std::cout << "Contains(\"Hello\"): " << (arr.Contains(std::string("Hello")) ? "true" : "false") << std::endl;

    // operator=
    DataArray<std::string> arr2;
    arr2 = arr;
    std::cout << "After arr2 = arr, arr2[0]: " << arr2[0] << std::endl;

    // initializer_list
    arr = {"A", "B", "C"};
    std::cout << "After arr = {\"A\", \"B\", \"C\"}: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

void test_type_selection()
{
    TEST_SECTION("Type Selection Verification");

    // 验证类型选择是否正确
    DataArray<int> int_arr;
    DataArray<std::string> str_arr;

    std::cout << "DataArray<int> uses: "
              << (std::is_same_v<decltype(int_arr), TrivialDataArrayImpl<int>> ? "TrivialDataArrayImpl" : "NonTrivialDataArrayImpl")
              << std::endl;

    std::cout << "DataArray<std::string> uses: "
              << (std::is_same_v<decltype(str_arr), NonTrivialDataArrayImpl<std::string>> ? "NonTrivialDataArrayImpl" : "TrivialDataArrayImpl")
              << std::endl;
}

void test_move_operation()
{
    TEST_SECTION("Move Operation Test");

    DataArray<int> arr;
    for(int i = 0; i < 10; i++)
        arr.Append(i);

    std::cout << "Before Move: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Move elements from index 2 to index 7 (3 elements)
    arr.Move(7, 2, 3);

    std::cout << "After Move(7, 2, 3): ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

void test_string_move_operation()
{
    TEST_SECTION("String Move Operation Test");

    DataArray<std::string> arr;
    arr.Append("Apple");
    arr.Append("Banana");
    arr.Append("Cherry");
    arr.Append("Date");
    arr.Append("Elderberry");

    std::cout << "Before Move: ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // Move elements from index 1 to index 3 (2 elements: Banana, Cherry)
    arr.Move(3, 1, 2);

    std::cout << "After Move(3, 1, 2): ";
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "  DataArrayNew Feature Test" << std::endl;
    std::cout << "========================================" << std::endl;

    try
    {
        test_trivial_types();
        test_non_trivial_types();
        test_type_selection();
        test_move_operation();
        test_string_move_operation();

        std::cout << "\n========================================" << std::endl;
        std::cout << "  All Tests Completed Successfully!" << std::endl;
        std::cout << "========================================" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
