#include<hgl/type/SplitString.h>
#include<iostream>
#include<cassert>
#include<chrono>

using namespace hgl;
using namespace std;

// ==================== 测试辅助函数 ====================

void PrintStringList(const U8StringList &sl, const char *label)
{
    cout << label << " (count=" << sl.GetCount() << "): ";
    for(int i = 0; i < sl.GetCount(); i++)
    {
        cout << "\"" << (char*)sl[i].c_str() << "\" ";
    }
    cout << endl;
}

void AssertStringListEqual(const U8StringList &sl, const char **expected, int expected_count, const char *test_name = "")
{
    if(sl.GetCount() != expected_count)
    {
        cerr << "FAIL [" << test_name << "]: Expected " << expected_count << " items, got " << sl.GetCount() << endl;
        assert(false);
    }

    for(int i = 0; i < expected_count; i++)
    {
        if(strcmp((char*)sl[i].c_str(), expected[i]) != 0)
        {
            cerr << "FAIL [" << test_name << "]: Item " << i << " expected \"" << expected[i]
                 << "\", got \"" << (char*)sl[i].c_str() << "\"" << endl;
            assert(false);
        }
    }
}

// ==================== 1. 基础功能测试 ====================
bool TestBasicSplitBySpace()
{
    cout << "\n========== Test 1: Basic Split By Space ==========" << endl;

    U8StringList sl;
    U8String str = u8"hello world test";

    int count = SplitToStringListBySpace(sl, str);
    assert(count == 3);

    const char *expected[] = {"hello", "world", "test"};
    AssertStringListEqual(sl, expected, 3, "SplitBySpace");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 2. 单字符分隔测试 ====================
bool TestSplitByChar()
{
    cout << "\n========== Test 2: Split By Single Char ==========" << endl;

    U8StringList sl;
    U8String str = u8"apple,banana,cherry";

    int count = SplitToStringListByChar(sl, str, u8',');
    assert(count == 3);

    const char *expected[] = {"apple", "banana", "cherry"};
    AssertStringListEqual(sl, expected, 3, "SplitByChar");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 3. 多字符分隔测试 ====================
bool TestSplitByChars()
{
    cout << "\n========== Test 3: Split By Multiple Chars ==========" << endl;

    U8StringList sl;
    U8String str = u8"apple,banana;cherry:date";
    U8String separators = u8",:;";

    int count = SplitToStringListByChars(sl, str, separators);
    assert(count == 4);

    const char *expected[] = {"apple", "banana", "cherry", "date"};
    AssertStringListEqual(sl, expected, 4, "SplitByChars");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 4. 换行符分隔测试 ====================
bool TestSplitByEnter()
{
    cout << "\n========== Test 4: Split By Line Breaks ==========" << endl;

    U8StringList sl;
    U8String str = u8"line1\nline2\rline3\r\nline4";

    int count = SplitToStringListByEnter(sl, str);
    assert(count == 4);

    const char *expected[] = {"line1", "line2", "line3", "line4"};
    AssertStringListEqual(sl, expected, 4, "SplitByEnter");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 5. 边界条件：空字符串 ====================
bool TestEmptyString()
{
    cout << "\n========== Test 5: Empty String ==========" << endl;

    U8StringList sl;
    U8String str = u8"";

    int count = SplitToStringListBySpace(sl, str);
    // 空字符串应该返回-1或0（具体取决于实现）
    cout << "Empty string returns: " << count << endl;
    assert(count == -1 || count == 0);

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 6. 边界条件：无分隔符 ====================
bool TestNoSeparator()
{
    cout << "\n========== Test 6: No Separators ==========" << endl;

    U8StringList sl;
    U8String str = u8"hello";

    int count = SplitToStringListByChar(sl, str, u8',');
    assert(count == 1);
    assert(strcmp((char*)sl[0].c_str(), "hello") == 0);

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 7. 边界条件：仅分隔符 ====================
bool TestOnlySeparators()
{
    cout << "\n========== Test 7: Only Separators ==========" << endl;

    U8StringList sl;
    U8String str = u8",,,";

    int count = SplitToStringListByChar(sl, str, u8',');
    // 只有分隔符，应该没有有效字符串
    assert(count == 0);
    cout << "Only separators returns: " << count << " (no valid strings)" << endl;

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 8. 边界条件：以分隔符开头 ====================
bool TestStartWithSeparator()
{
    cout << "\n========== Test 8: Start With Separator ==========" << endl;

    U8StringList sl;
    U8String str = u8",apple,banana";

    int count = SplitToStringListByChar(sl, str, u8',');
    assert(count == 2);

    const char *expected[] = {"apple", "banana"};
    AssertStringListEqual(sl, expected, 2, "StartWithSeparator");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 9. 边界条件：以分隔符结尾 ====================
bool TestEndWithSeparator()
{
    cout << "\n========== Test 9: End With Separator ==========" << endl;

    U8StringList sl;
    U8String str = u8"apple,banana,";

    int count = SplitToStringListByChar(sl, str, u8',');
    assert(count == 2);

    const char *expected[] = {"apple", "banana"};
    AssertStringListEqual(sl, expected, 2, "EndWithSeparator");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 10. 连续分隔符 ====================
bool TestConsecutiveSeparators()
{
    cout << "\n========== Test 10: Consecutive Separators ==========" << endl;

    U8StringList sl;
    U8String str = u8"apple,,,,banana";

    int count = SplitToStringListByChar(sl, str, u8',');
    // 连续分隔符应该被跳过，只得到2个字符串
    assert(count == 2);

    const char *expected[] = {"apple", "banana"};
    AssertStringListEqual(sl, expected, 2, "ConsecutiveSeparators");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 11. 代码标识符拆分 ====================
bool TestSplitByCode()
{
    cout << "\n========== Test 11: Split By Code Identifiers ==========" << endl;

    U8StringList sl;
    U8String str = u8"var_name + var_value * 123";

    int count = SplitToStringListByCodes(sl, str);

    PrintStringList(sl, "Code identifiers");
    assert(count >= 3);

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 12. 数字拆分 ====================
bool TestSplitByDigit()
{
    cout << "\n========== Test 12: Split By Digits ==========" << endl;

    U8StringList sl;
    U8String str = u8"abc123def456ghi789";

    int count = SplitToStringListByDigit(sl, str);
    assert(count == 3);

    const char *expected[] = {"123", "456", "789"};
    AssertStringListEqual(sl, expected, 3, "SplitByDigit");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 13. 十六进制拆分 ====================
bool TestSplitByXDigit()
{
    cout << "\n========== Test 13: Split By Hex Digits ==========" << endl;

    U8StringList sl;
    U8String str = u8"0xABCD,0x1234,0xFFFF";

    int count = SplitToStringListByXDigit(sl, str);
    assert(count >= 3);

    PrintStringList(sl, "Hex values");
    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 14. 浮点数拆分 ====================
bool TestSplitByFloat()
{
    cout << "\n========== Test 14: Split By Float Numbers ==========" << endl;

    U8StringList sl;
    U8String str = u8"1.5 + 2.3 - 0.7 * 3.14";

    int count = SplitToStringListByFloat(sl, str);
    assert(count >= 4);

    PrintStringList(sl, "Float numbers");
    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 15. SpliteByString 测试 ====================
bool TestSpliteByString()
{
    cout << "\n========== Test 15: SpliteByString ==========" << endl;

    U8String str = u8"hello_game_world";
    U8String center = u8"_game_";
    U8String left, right;

    SpliteByString(str, center, &left, &right);

    assert(strcmp((char*)left.c_str(), "hello") == 0);
    assert(strcmp((char*)right.c_str(), "world") == 0);

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 16. SpliteByString - 部分匹配 ====================
bool TestSpliteByStringPartial()
{
    cout << "\n========== Test 16: SpliteByString - No Match ==========" << endl;

    U8String str = u8"hello world";
    U8String center = u8"xyz";
    U8String left, right;

    SpliteByString(str, center, &left, &right);

    assert(strcmp((char*)left.c_str(), "hello world") == 0);
    assert(right.IsEmpty());

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 17. 大字符串性能测试 ====================
bool TestLargeStringPerformance()
{
    cout << "\n========== Test 17: Large String Performance (10,000 items) ==========" << endl;

    U8StringList sl;
    U8String str = u8"";

    // 构建包含10,000个单词的字符串
    for(int i = 0; i < 10000; i++)
    {
        if(i > 0) str += u8" ";
        str += u8"word";
    }

    auto start = chrono::steady_clock::now();
    int count = SplitToStringListBySpace(sl, str);
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    assert(count == 10000);
    cout << "✓ Split 10,000 items in " << duration.count() << " ms" << endl;
    cout << "  Average: " << (double)duration.count() / count * 1000 << " us/item" << endl;

    return true;
}

// ==================== 18. 复杂分隔符组合 ====================
bool TestComplexSeparators()
{
    cout << "\n========== Test 18: Complex Separator Combinations ==========" << endl;

    U8StringList sl;
    U8String str = u8"<html><body>Hello</body></html>";
    U8String separators = u8"<>";

    int count = SplitToStringListByChars(sl, str, separators);

    PrintStringList(sl, "HTML tags");

    // 应该得到: html, body, Hello, /body, /html
    assert(count >= 5);

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 19. 多行文本处理 ====================
bool TestMultilineText()
{
    cout << "\n========== Test 19: Multiline Text Processing ==========" << endl;

    U8StringList sl;
    U8String str = u8"First line\nSecond line\r\nThird line\rFourth line";

    int count = SplitToStringListByEnter(sl, str);
    assert(count == 4);

    const char *expected[] = {"First line", "Second line", "Third line", "Fourth line"};
    AssertStringListEqual(sl, expected, 4, "MultilineText");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 20. CSV格式解析 ====================
bool TestCSVParsing()
{
    cout << "\n========== Test 20: CSV Format Parsing ==========" << endl;

    U8StringList sl;
    U8String str = u8"John,Smith,30,Engineer";

    int count = SplitToStringListByChar(sl, str, u8',');
    assert(count == 4);

    const char *expected[] = {"John", "Smith", "30", "Engineer"};
    AssertStringListEqual(sl, expected, 4, "CSVParsing");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 21. 路径分隔符解析 ====================
bool TestPathParsing()
{
    cout << "\n========== Test 21: Path Parsing ==========" << endl;

    U8StringList sl;
    U8String str = u8"Users/Admin/Documents/File.txt";

    int count = SplitToStringListByChar(sl, str, u8'/');
    assert(count >= 3);

    PrintStringList(sl, "Path components");
    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 22. URL解析 ====================
bool TestURLParsing()
{
    cout << "\n========== Test 22: URL Query Parsing ==========" << endl;

    U8StringList sl;
    U8String str = u8"name=John&age=30&city=NYC";

    int count = SplitToStringListByChar(sl, str, u8'&');
    assert(count == 3);

    const char *expected[] = {"name=John", "age=30", "city=NYC"};
    AssertStringListEqual(sl, expected, 3, "URLParsing");

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 23. 重复调用测试 ====================
bool TestRepeatedSplit()
{
    cout << "\n========== Test 23: Repeated Split Operations (1000x) ==========" << endl;

    U8StringList sl;
    auto start = chrono::steady_clock::now();

    for(int i = 0; i < 1000; i++)
    {
        sl.Clear();
        U8String str = u8"a,b,c,d,e";
        int count = SplitToStringListByChar(sl, str, u8',');
        assert(count == 5);
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ Performed 1000 split operations in " << duration.count() << " ms" << endl;
    return true;
}

// ==================== 24. 空值处理 ====================
bool TestNullHandling()
{
    cout << "\n========== Test 24: Null/Invalid Input Handling ==========" << endl;

    U8StringList sl;

    // Empty string using String object overload
    U8String emptyStr = u8"";
    int count = SplitToStringListBySpace(sl, emptyStr);
    assert(count == -1);

    // Zero-length string with pointer+size overload
    U8String empty = u8"";
    int count2 = SplitToStringListBySpace(sl, empty.c_str(), 0);
    assert(count2 == -1);

    cout << "✓ Empty input correctly returns -1" << endl;
    return true;
}

// ==================== 25. 单一字符串 ====================
bool TestSingleItem()
{
    cout << "\n========== Test 25: Single Item ==========" << endl;

    U8StringList sl;
    U8String str = u8"lonely";

    int count = SplitToStringListBySpace(sl, str);
    assert(count == 1);
    assert(strcmp((char*)sl[0].c_str(), "lonely") == 0);

    cout << "✓ PASSED" << endl;
    return true;
}

// ==================== 主测试运行器 ====================
int main(int, char**)
{
    cout << "╔════════════════════════════════════════════════════╗" << endl;
    cout << "║    SplitString Comprehensive Test Suite            ║" << endl;
    cout << "║    25 comprehensive test cases                      ║" << endl;
    cout << "╚════════════════════════════════════════════════════╝" << endl;

    int passed = 0;
    int failed = 0;

    struct Test {
        const char* name;
        bool (*func)();
    };

    Test tests[] = {
        {"Basic Split By Space", TestBasicSplitBySpace},
        {"Split By Single Char", TestSplitByChar},
        {"Split By Multiple Chars", TestSplitByChars},
        {"Split By Line Breaks", TestSplitByEnter},
        {"Empty String", TestEmptyString},
        {"No Separators", TestNoSeparator},
        {"Only Separators", TestOnlySeparators},
        {"Start With Separator", TestStartWithSeparator},
        {"End With Separator", TestEndWithSeparator},
        {"Consecutive Separators", TestConsecutiveSeparators},
        {"Split By Code", TestSplitByCode},
        {"Split By Digits", TestSplitByDigit},
        {"Split By Hex Digits", TestSplitByXDigit},
        {"Split By Float", TestSplitByFloat},
        {"SpliteByString", TestSpliteByString},
        {"SpliteByString Partial", TestSpliteByStringPartial},
        {"Large String Performance", TestLargeStringPerformance},
        {"Complex Separators", TestComplexSeparators},
        {"Multiline Text", TestMultilineText},
        {"CSV Parsing", TestCSVParsing},
        {"Path Parsing", TestPathParsing},
        {"URL Parsing", TestURLParsing},
        {"Repeated Operations", TestRepeatedSplit},
        {"Null Handling", TestNullHandling},
        {"Single Item", TestSingleItem}
    };

    for(const auto& test : tests)
    {
        try
        {
            if(test.func())
            {
                passed++;
            }
            else
            {
                failed++;
                cout << "✗ " << test.name << " - FAILED" << endl;
            }
        }
        catch(const exception& e)
        {
            failed++;
            cout << "✗ " << test.name << " - EXCEPTION: " << e.what() << endl;
        }
    }

    cout << "\n╔════════════════════════════════════════════════════╗" << endl;
    cout << "║             Test Summary                            ║" << endl;
    cout << "╠════════════════════════════════════════════════════╣" << endl;
    cout << "  Total Tests:  " << (passed + failed) << endl;
    cout << "  Passed:       " << passed << " ✓" << endl;
    cout << "  Failed:       " << failed << " ✗" << endl;
    cout << "╚════════════════════════════════════════════════════╝" << endl;

    return (failed == 0) ? 0 : 1;
}
