/**
 * MergeStringTest.cpp
 *
 * MergeString<T> 字符串列表合并测试：
 *   - 多行合并（指定换行符）
 *   - 空列表 / 空行
 *   - 与 SplitString 组合使用（往返验证）
 */
#include<hgl/type/MergeString.h>
#include<hgl/type/SplitString.h>
#include"test_common.h"

using namespace hgl;

static void TestBasic()
{
    std::printf("\n=== MergeString: Basic Merge ===\n");

    StringList<char> sl;

    sl.Add("hello");
    sl.Add("world");
    sl.Add("hgl");

    String<char> merged = MergeString(sl, String<char>("\n"));

    TEST_ASSERT(merged.Comp("hello\nworld\nhgl\n") == 0, "Merged by \\n (separator appended after each line)");
}

static void TestCustomSeparator()
{
    std::printf("\n=== MergeString: Custom Separator ===\n");

    StringList<char> sl;
    sl.Add("a");
    sl.Add("b");

    String<char> merged = MergeString(sl, String<char>("|"));
    TEST_ASSERT(merged.Comp("a|b|") == 0, "Custom separator merge");
}

static void TestEmpty()
{
    std::printf("\n=== MergeString: Empty List ===\n");

    StringList<char> sl;
    String<char> merged = MergeString(sl, String<char>("\n"));

    TEST_ASSERT(merged.IsEmpty(), "Empty list merges to empty string");

    // 含空字符串的行
    sl.Add("");
    sl.Add("x");
    String<char> merged2 = MergeString(sl, String<char>(","));
    TEST_ASSERT(merged2.Comp(",x,") == 0, "Empty line also gets separator");
}

static void TestRoundTrip()
{
    std::printf("\n=== MergeString: Round Trip with SplitString ===\n");

    // 合并后再拆回，验证内容一致
    StringList<char> sl;
    sl.Add("line1");
    sl.Add("line2");
    sl.Add("line3");

    String<char> merged = MergeString(sl, String<char>("\n"));

    StringList<char> back;
    int count = SplitToStringListByChar(back, merged.c_str(), merged.Length(), '\n');

    TEST_ASSERT(count == 3, "Split back to 3 lines");
    TEST_ASSERT(back[0].Comp("line1") == 0, "Line 1 matches");
    TEST_ASSERT(back[1].Comp("line2") == 0, "Line 2 matches");
    TEST_ASSERT(back[2].Comp("line3") == 0, "Line 3 matches");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestBasic();
    TestCustomSeparator();
    TestEmpty();
    TestRoundTrip();

    return TEST_SUMMARY();
}
