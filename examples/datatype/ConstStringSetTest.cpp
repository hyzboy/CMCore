#include<hgl/type/ConstStringSet.h>
#include<iostream>
#include<cassert>
#include<vector>

using namespace hgl;
using namespace std;

// 测试辅助宏
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "❌ FAILED: " << message << std::endl; \
            std::cerr << "   at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define TEST_PASS(message) \
    std::cout << "✓ PASSED: " << message << std::endl

// ==================== 1. 基础功能测试 ====================
bool TestBasicOperations()
{
    std::cout << "\n========== Test 1: Basic Operations ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    // 测试空集合
    TEST_ASSERT(css.IsEmpty(), "New set should be empty");
    TEST_ASSERT(css.GetCount() == 0, "Empty set count should be 0");
    TEST_PASS("Empty set initialization");
    
    // 添加第一个字符串
    int id1 = css.AddString(csv, "hello", 5);
    TEST_ASSERT(id1 == 0, "First string ID should be 0");
    TEST_ASSERT(csv.id == 0, "StringView ID should match");
    TEST_ASSERT(csv.length == 5, "StringView length should be 5");
    TEST_ASSERT(!css.IsEmpty(), "Set should not be empty after adding");
    TEST_ASSERT(css.GetCount() == 1, "Set count should be 1");
    TEST_PASS("Add first string");
    
    // 添加第二个字符串
    int id2 = css.AddString(csv, "world", 5);
    TEST_ASSERT(id2 == 1, "Second string ID should be 1");
    TEST_ASSERT(css.GetCount() == 2, "Set count should be 2");
    TEST_PASS("Add second string");
    
    // 验证字符串内容
    const char* str1 = css.GetString(0);
    const char* str2 = css.GetString(1);
    TEST_ASSERT(str1 != nullptr, "GetString(0) should not be null");
    TEST_ASSERT(str2 != nullptr, "GetString(1) should not be null");
    TEST_ASSERT(strcmp(str1, "hello") == 0, "String 0 should be 'hello'");
    TEST_ASSERT(strcmp(str2, "world") == 0, "String 1 should be 'world'");
    TEST_PASS("Retrieve strings by ID");
    
    return true;
}

// ==================== 2. 去重功能测试 ====================
bool TestDeduplication()
{
    std::cout << "\n========== Test 2: Deduplication ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    // 添加原始字符串
    int id1 = css.AddString(csv, "duplicate", 9);
    TEST_ASSERT(id1 == 0, "First add should return ID 0");
    
    // 重复添加相同字符串
    int id2 = css.AddString(csv, "duplicate", 9);
    TEST_ASSERT(id2 == 0, "Duplicate string should return same ID");
    TEST_ASSERT(css.GetCount() == 1, "Count should remain 1 for duplicate");
    TEST_PASS("Duplicate detection - exact match");
    
    // 添加不同字符串
    int id3 = css.AddString(csv, "different", 9);
    TEST_ASSERT(id3 == 1, "Different string should get new ID");
    TEST_ASSERT(css.GetCount() == 2, "Count should be 2");
    
    // 再次重复第一个
    int id4 = css.AddString(csv, "duplicate", 9);
    TEST_ASSERT(id4 == 0, "Should still return original ID");
    TEST_ASSERT(css.GetCount() == 2, "Count should remain 2");
    TEST_PASS("Multiple duplicate additions");
    
    // 测试长度不同但前缀相同
    int id5 = css.AddString(csv, "duplicates", 10);
    TEST_ASSERT(id5 == 2, "Longer string should get different ID");
    TEST_ASSERT(css.GetCount() == 3, "Count should be 3");
    TEST_PASS("Different length strings with same prefix");
    
    return true;
}

// ==================== 3. 边界条件测试 ====================
bool TestEdgeCases()
{
    std::cout << "\n========== Test 3: Edge Cases ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    // 测试空指针
    int id_null = css.AddString(csv, nullptr, 5);
    TEST_ASSERT(id_null == -1, "Null pointer should return -1");
    TEST_PASS("Null pointer handling");
    
    // 测试零长度
    int id_zero = css.AddString(csv, "test", 0);
    TEST_ASSERT(id_zero == -1, "Zero length should return -1");
    TEST_PASS("Zero length handling");
    
    // 测试负长度
    int id_neg = css.AddString(csv, "test", -1);
    TEST_ASSERT(id_neg == -1, "Negative length should return -1");
    TEST_PASS("Negative length handling");
    
    // 测试单字符
    int id_single = css.AddString(csv, "a", 1);
    TEST_ASSERT(id_single == 0, "Single character should work");
    TEST_ASSERT(strcmp(css.GetString(0), "a") == 0, "Single char should be 'a'");
    TEST_PASS("Single character string");
    
    // 测试长字符串
    char long_str[1000];
    for(int i = 0; i < 999; i++)
        long_str[i] = 'x';
    long_str[999] = '\0';
    
    int id_long = css.AddString(csv, long_str, 999);
    TEST_ASSERT(id_long == 1, "Long string should work");
    TEST_ASSERT(strlen(css.GetString(1)) == 999, "Long string length should be 999");
    TEST_PASS("Long string (999 chars)");
    
    // 测试特殊字符
    int id_special = css.AddString(csv, "hello\nworld\t!", 13);
    TEST_ASSERT(id_special == 2, "Special characters should work");
    TEST_PASS("Special characters");
    
    return true;
}

// ==================== 4. 查询功能测试 ====================
bool TestQueryFunctions()
{
    std::cout << "\n========== Test 4: Query Functions ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    // 准备测试数据
    css.AddString(csv, "apple", 5);
    css.AddString(csv, "banana", 6);
    css.AddString(csv, "cherry", 6);
    
    // 测试 Contains
    TEST_ASSERT(css.Contains("apple", 5), "Should contain 'apple'");
    TEST_ASSERT(css.Contains("banana", 6), "Should contain 'banana'");
    TEST_ASSERT(css.Contains("cherry", 6), "Should contain 'cherry'");
    TEST_ASSERT(!css.Contains("orange", 6), "Should not contain 'orange'");
    TEST_ASSERT(!css.Contains("appl", 4), "Should not contain partial 'appl'");
    TEST_PASS("Contains() function");
    
    // 测试 GetID
    TEST_ASSERT(css.GetID("apple", 5) == 0, "ID of 'apple' should be 0");
    TEST_ASSERT(css.GetID("banana", 6) == 1, "ID of 'banana' should be 1");
    TEST_ASSERT(css.GetID("cherry", 6) == 2, "ID of 'cherry' should be 2");
    TEST_ASSERT(css.GetID("orange", 6) == -1, "ID of non-existent should be -1");
    TEST_PASS("GetID() function");
    
    // 测试 GetString
    TEST_ASSERT(strcmp(css.GetString(0), "apple") == 0, "String 0 should be 'apple'");
    TEST_ASSERT(strcmp(css.GetString(1), "banana") == 0, "String 1 should be 'banana'");
    TEST_ASSERT(strcmp(css.GetString(2), "cherry") == 0, "String 2 should be 'cherry'");
    TEST_ASSERT(css.GetString(3) == nullptr, "Invalid ID should return nullptr");
    TEST_ASSERT(css.GetString(-1) == nullptr, "Negative ID should return nullptr");
    TEST_PASS("GetString() function");
    
    // 测试 GetStringView
    const ConstAnsiStringView* view0 = css.GetStringView(0);
    TEST_ASSERT(view0 != nullptr, "GetStringView(0) should not be null");
    TEST_ASSERT(view0->id == 0, "View ID should be 0");
    TEST_ASSERT(view0->length == 5, "View length should be 5");
    TEST_ASSERT(strcmp(view0->GetString(), "apple") == 0, "View string should be 'apple'");
    
    const ConstAnsiStringView* view_invalid = css.GetStringView(999);
    TEST_ASSERT(view_invalid == nullptr, "Invalid ID should return nullptr");
    TEST_PASS("GetStringView() function");
    
    // 测试 operator[]
    const ConstAnsiStringView* view_op = css[1];
    TEST_ASSERT(view_op != nullptr, "operator[1] should not be null");
    TEST_ASSERT(strcmp(view_op->GetString(), "banana") == 0, "operator[1] should return 'banana'");
    TEST_PASS("operator[] function");
    
    return true;
}

// ==================== 5. 迭代器测试 ====================
bool TestIterators()
{
    std::cout << "\n========== Test 5: Iterators ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    // 添加测试数据
    std::vector<std::string> test_strings = {"first", "second", "third", "fourth"};
    for(const auto& str : test_strings)
    {
        css.AddString(csv, str.c_str(), str.length());
    }
    
    // 测试迭代器遍历
    int count = 0;
    for(auto it = css.begin(); it != css.end(); ++it)
    {
        const ConstAnsiStringView* view = *it;
        TEST_ASSERT(view != nullptr, "Iterator should not return null");
        TEST_ASSERT(view->id == count, "ID should match iteration order");
        count++;
    }
    TEST_ASSERT(count == 4, "Should iterate over all 4 strings");
    TEST_PASS("Iterator traversal");
    
    // 验证迭代器内容
    auto it = css.begin();
    TEST_ASSERT(strcmp((*it)->GetString(), "first") == 0, "First item should be 'first'");
    ++it;
    TEST_ASSERT(strcmp((*it)->GetString(), "second") == 0, "Second item should be 'second'");
    ++it;
    TEST_ASSERT(strcmp((*it)->GetString(), "third") == 0, "Third item should be 'third'");
    ++it;
    TEST_ASSERT(strcmp((*it)->GetString(), "fourth") == 0, "Fourth item should be 'fourth'");
    TEST_PASS("Iterator content verification");
    
    return true;
}

// ==================== 6. Clear 和状态测试 ====================
bool TestClearAndState()
{
    std::cout << "\n========== Test 6: Clear and State ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    // 添加数据
    css.AddString(csv, "test1", 5);
    css.AddString(csv, "test2", 5);
    css.AddString(csv, "test3", 5);
    
    TEST_ASSERT(css.GetCount() == 3, "Should have 3 strings");
    TEST_ASSERT(!css.IsEmpty(), "Should not be empty");
    
    // 清空
    css.Clear();
    TEST_ASSERT(css.GetCount() == 0, "Count should be 0 after Clear");
    TEST_ASSERT(css.IsEmpty(), "Should be empty after Clear");
    TEST_ASSERT(css.GetTotalLength() == 0, "Total length should be 0");
    TEST_PASS("Clear() function");
    
    // 清空后重新添加
    int id = css.AddString(csv, "new_string", 10);
    TEST_ASSERT(id == 0, "First string after clear should get ID 0");
    TEST_ASSERT(css.GetCount() == 1, "Count should be 1");
    TEST_ASSERT(strcmp(css.GetString(0), "new_string") == 0, "Should get correct string");
    TEST_PASS("Re-add after clear");
    
    return true;
}

// ==================== 7. 统计信息测试 ====================
bool TestStatistics()
{
    std::cout << "\n========== Test 7: Statistics ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    css.AddString(csv, "a", 1);      // length: 1
    css.AddString(csv, "ab", 2);     // length: 2
    css.AddString(csv, "abc", 3);    // length: 3
    
    TEST_ASSERT(css.GetCount() == 3, "Count should be 3");
    
    // 总长度 = 1 + 1 + 2 + 1 + 3 + 1 = 9 (包括每个字符串末尾的 \0)
    int total_length = css.GetTotalLength();
    TEST_ASSERT(total_length == 9, "Total length should be 9");
    TEST_PASS("GetTotalLength()");
    
    int total_bytes = css.GetTotalBytes();
    TEST_ASSERT(total_bytes == 9 * sizeof(char), "Total bytes calculation");
    TEST_PASS("GetTotalBytes()");
    
    return true;
}

// ==================== 8. 性能测试 ====================
bool TestPerformance()
{
    std::cout << "\n========== Test 8: Performance Test ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    const int NUM_STRINGS = 10000;
    
    // 生成唯一字符串并添加
    std::cout << "Adding " << NUM_STRINGS << " unique strings..." << std::endl;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[32];
        int len = snprintf(buffer, sizeof(buffer), "string_%d", i);
        int id = css.AddString(csv, buffer, len);
        TEST_ASSERT(id == i, "ID should match index");
    }
    
    TEST_ASSERT(css.GetCount() == NUM_STRINGS, "Should have all strings");
    TEST_PASS("Large scale addition");
    
    // 测试查询性能
    std::cout << "Testing query performance..." << std::endl;
    bool all_found = true;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[32];
        int len = snprintf(buffer, sizeof(buffer), "string_%d", i);
        
        if(!css.Contains(buffer, len))
        {
            all_found = false;
            break;
        }
        
        int id = css.GetID(buffer, len);
        if(id != i)
        {
            all_found = false;
            break;
        }
    }
    TEST_ASSERT(all_found, "All strings should be found");
    TEST_PASS("Large scale query");
    
    // 测试去重性能
    std::cout << "Testing deduplication performance..." << std::endl;
    for(int i = 0; i < 1000; i++)
    {
        char buffer[32];
        int len = snprintf(buffer, sizeof(buffer), "string_%d", i % 100);
        css.AddString(csv, buffer, len);
    }
    TEST_ASSERT(css.GetCount() == NUM_STRINGS, "Count should not increase for duplicates");
    TEST_PASS("Large scale deduplication");
    
    return true;
}

// ==================== 9. UTF-8 和多字节测试 ====================
bool TestUTF8()
{
    std::cout << "\n========== Test 9: UTF-8 Support ==========" << std::endl;
    
    ConstU8StringSet css;
    ConstU8StringView csv;
    
    // 中文字符串
    const u8char* chinese = reinterpret_cast<const u8char*>(u8"你好世界");
    int len_chinese = strlen(reinterpret_cast<const char*>(chinese));
    int id1 = css.AddString(csv, chinese, len_chinese);
    TEST_ASSERT(id1 == 0, "Chinese string should be added");
    TEST_PASS("Chinese characters");
    
    // 日文字符串
    const u8char* japanese = reinterpret_cast<const u8char*>(u8"こんにちは");
    int len_japanese = strlen(reinterpret_cast<const char*>(japanese));
    int id2 = css.AddString(csv, japanese, len_japanese);
    TEST_ASSERT(id2 == 1, "Japanese string should be added");
    TEST_PASS("Japanese characters");
    
    // Emoji
    const u8char* emoji = reinterpret_cast<const u8char*>(u8"😀🎉🚀");
    int len_emoji = strlen(reinterpret_cast<const char*>(emoji));
    int id3 = css.AddString(csv, emoji, len_emoji);
    TEST_ASSERT(id3 == 2, "Emoji string should be added");
    TEST_PASS("Emoji characters");
    
    // 验证检索
    TEST_ASSERT(css.Contains(chinese, len_chinese), "Should find Chinese");
    TEST_ASSERT(css.Contains(japanese, len_japanese), "Should find Japanese");
    TEST_ASSERT(css.Contains(emoji, len_emoji), "Should find Emoji");
    TEST_PASS("UTF-8 query");
    
    return true;
}

// ==================== 10. 文件保存测试 ====================
bool TestFileSave()
{
    std::cout << "\n========== Test 10: File Save ==========" << std::endl;
    
    ConstAnsiStringSet css;
    ConstAnsiStringView csv;
    
    css.AddString(csv, "apple", 5);
    css.AddString(csv, "banana", 6);
    css.AddString(csv, "cherry", 6);
    css.AddString(csv, "date", 4);
    
    // 保存不带ID
    bool result1 = SaveToTextFile(OS_TEXT("test_output.txt"), &css, false, false);
    TEST_ASSERT(result1, "Save without ID should succeed");
    TEST_PASS("Save to file without ID");
    
    // 保存带ID
    bool result2 = SaveToTextFile(OS_TEXT("test_output_id.txt"), &css, true, false);
    TEST_ASSERT(result2, "Save with ID should succeed");
    TEST_PASS("Save to file with ID");
    
    std::cout << "Output files: test_output.txt, test_output_id.txt" << std::endl;
    
    return true;
}

// ==================== 主测试运行器 ====================
int main(int, char**)
{
    std::cout << "╔═══════════════════════════════════════════╗" << std::endl;
    std::cout << "║  ConstStringSet Comprehensive Test Suite ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════╝" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    struct Test {
        const char* name;
        bool (*func)();
    };
    
    Test tests[] = {
        {"Basic Operations", TestBasicOperations},
        {"Deduplication", TestDeduplication},
        {"Edge Cases", TestEdgeCases},
        {"Query Functions", TestQueryFunctions},
        {"Iterators", TestIterators},
        {"Clear and State", TestClearAndState},
        {"Statistics", TestStatistics},
        {"Performance", TestPerformance},
        {"UTF-8 Support", TestUTF8},
        {"File Save", TestFileSave}
    };
    
    for(const auto& test : tests)
    {
        try
        {
            if(test.func())
            {
                passed++;
                std::cout << "✓ " << test.name << " - ALL PASSED" << std::endl;
            }
            else
            {
                failed++;
                std::cout << "✗ " << test.name << " - FAILED" << std::endl;
            }
        }
        catch(const std::exception& e)
        {
            failed++;
            std::cout << "✗ " << test.name << " - EXCEPTION: " << e.what() << std::endl;
        }
    }
    
    std::cout << "\n╔═══════════════════════════════════════════╗" << std::endl;
    std::cout << "║             Test Summary                  ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════╣" << std::endl;
    std::cout << "  Total Tests:  " << (passed + failed) << std::endl;
    std::cout << "  Passed:       " << passed << " ✓" << std::endl;
    std::cout << "  Failed:       " << failed << " ✗" << std::endl;
    std::cout << "╚═══════════════════════════════════════════╝" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}