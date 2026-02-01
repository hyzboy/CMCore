#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

// 测试计数器
static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            cout << "  ✓ PASS: " << message << endl; \
            test_passed++; \
        } else { \
            cout << "  ✗ FAIL: " << message << endl; \
            test_failed++; \
        } \
    } while(0)

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Find Operations" << endl;
    cout << "========================================" << endl;

    BidirectionalMap<int, string> bmap;
    bmap.Add(100, "hundred");
    bmap.Add(200, "two hundred");
    bmap.Add(300, "three hundred");

    // [1] 标准 Get 操作
    cout << "\n[1] Standard Get (KEY -> VALUE):" << endl;
    {
        string result;
        TEST_ASSERT(bmap.Get(100, result), "Get(100) succeeds");
        TEST_ASSERT(result == "hundred", "Get(100) returns 'hundred'");
        
        TEST_ASSERT(!bmap.Get(999, result), "Get(999) fails (key not found)");
    }

    // [2] 标准 GetByValue 操作
    cout << "\n[2] Standard GetByValue (VALUE -> KEY):" << endl;
    {
        int result;
        TEST_ASSERT(bmap.GetByValue("two hundred", result), "GetByValue('two hundred') succeeds");
        TEST_ASSERT(result == 200, "GetByValue('two hundred') returns 200");
        
        TEST_ASSERT(!bmap.GetByValue("nonexistent", result), "GetByValue('nonexistent') fails");
    }

    // [3] GetValuePointer（const 版本）
    cout << "\n[3] GetValuePointer (const):" << endl;
    {
        const string* ptr = bmap.GetValuePointer(100);
        TEST_ASSERT(ptr != nullptr, "GetValuePointer(100) returns non-null");
        TEST_ASSERT(*ptr == "hundred", "Pointer dereference returns correct value");
        
        const string* null_ptr = bmap.GetValuePointer(999);
        TEST_ASSERT(null_ptr == nullptr, "GetValuePointer(999) returns nullptr");
    }

    // [4] GetValuePointer（非 const 版本）
    cout << "\n[4] GetValuePointer (non-const):" << endl;
    {
        BidirectionalMap<int, string> mutable_bmap;
        mutable_bmap.Add(1, "one");
        
        string* ptr = mutable_bmap.GetValuePointer(1);
        TEST_ASSERT(ptr != nullptr, "GetValuePointer(1) returns non-null");
        *ptr = "ONE";  // 修改值
        
        string result;
        TEST_ASSERT(mutable_bmap.Get(1, result) && result == "ONE", "Modified value persists");
    }

    // [5] GetKeyPointer（const 版本）
    cout << "\n[5] GetKeyPointer (const):" << endl;
    {
        const int* ptr = bmap.GetKeyPointer("hundred");
        TEST_ASSERT(ptr != nullptr, "GetKeyPointer('hundred') returns non-null");
        TEST_ASSERT(*ptr == 100, "Pointer dereference returns correct key");
        
        const int* null_ptr = bmap.GetKeyPointer("nonexistent");
        TEST_ASSERT(null_ptr == nullptr, "GetKeyPointer('nonexistent') returns nullptr");
    }

    // [6] GetKeyPointer（非 const 版本）
    cout << "\n[6] GetKeyPointer (non-const):" << endl;
    {
        BidirectionalMap<int, string> mutable_bmap;
        mutable_bmap.Add(5, "five");
        
        int* ptr = mutable_bmap.GetKeyPointer("five");
        TEST_ASSERT(ptr != nullptr, "GetKeyPointer('five') returns non-null");
        // 注意：不应该修改 KEY（违反 API 约定），这里仅测试指针有效性
    }

    // [7] ContainsKey
    cout << "\n[7] ContainsKey:" << endl;
    TEST_ASSERT(bmap.ContainsKey(100), "ContainsKey(100) returns true");
    TEST_ASSERT(bmap.ContainsKey(200), "ContainsKey(200) returns true");
    TEST_ASSERT(!bmap.ContainsKey(999), "ContainsKey(999) returns false");
    TEST_ASSERT(!bmap.ContainsKey(0), "ContainsKey(0) returns false");

    // [8] ContainsValue
    cout << "\n[8] ContainsValue:" << endl;
    TEST_ASSERT(bmap.ContainsValue("hundred"), "ContainsValue('hundred') returns true");
    TEST_ASSERT(bmap.ContainsValue("two hundred"), "ContainsValue('two hundred') returns true");
    TEST_ASSERT(!bmap.ContainsValue("nonexistent"), "ContainsValue('nonexistent') returns false");
    TEST_ASSERT(!bmap.ContainsValue(""), "ContainsValue('') returns false");

    // [9] 连续查找相同的 KEY
    cout << "\n[9] Repeated key lookups:" << endl;
    {
        string r1, r2, r3;
        TEST_ASSERT(bmap.Get(100, r1) && bmap.Get(100, r2) && bmap.Get(100, r3), 
                   "Multiple Get(100) calls succeed");
        TEST_ASSERT(r1 == r2 && r2 == r3 && r1 == "hundred", 
                   "All results are consistent");
    }

    // [10] 连续查找相同的 VALUE
    cout << "\n[10] Repeated value lookups:" << endl;
    {
        int r1, r2, r3;
        TEST_ASSERT(bmap.GetByValue("hundred", r1) && 
                   bmap.GetByValue("hundred", r2) && 
                   bmap.GetByValue("hundred", r3), 
                   "Multiple GetByValue('hundred') calls succeed");
        TEST_ASSERT(r1 == r2 && r2 == r3 && r1 == 100, 
                   "All results are consistent");
    }

    // [11] 大量数据的查找性能概念验证
    cout << "\n[11] Find in larger dataset:" << endl;
    {
        BidirectionalMap<int, string> large_bmap;
        for (int i = 0; i < 1000; i++)
        {
            large_bmap.Add(i, "value_" + to_string(i));
        }
        
        string result;
        TEST_ASSERT(large_bmap.Get(500, result) && result == "value_500", 
                   "Found correct value in 1000-item map");
        TEST_ASSERT(large_bmap.Get(999, result) && result == "value_999", 
                   "Found last item in 1000-item map");
        TEST_ASSERT(!large_bmap.Get(1000, result), 
                   "Correctly returns false for non-existent key in 1000-item map");
    }

    // [12] 反向查找在大数据集中
    cout << "\n[12] Reverse find in larger dataset:" << endl;
    {
        BidirectionalMap<int, string> large_bmap;
        for (int i = 0; i < 1000; i++)
        {
            large_bmap.Add(i, "val_" + to_string(i));
        }
        
        int result;
        TEST_ASSERT(large_bmap.GetByValue("val_500", result) && result == 500, 
                   "Found correct key by value in 1000-item map");
        TEST_ASSERT(large_bmap.GetByValue("val_999", result) && result == 999, 
                   "Found last key by value in 1000-item map");
        TEST_ASSERT(!large_bmap.GetByValue("val_1000", result), 
                   "Correctly returns false for non-existent value in 1000-item map");
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
