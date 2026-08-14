/**
 * StringViewListTest.cpp
 *
 * StringViewList<CharT> 字符串视图列表测试（以 char 为例）：
 *   - FromString 按行分割（\n、\r\n、空行、无结尾换行、结尾换行）
 *   - GetCount/GetString/operator[] 行访问与越界
 *   - begin/end 迭代
 *   - 生命周期：视图引用列表持有的原文
 *   - Clear
 */
#include<hgl/type/StringViewList.h>
#include"test_common.h"

using namespace hgl;

static void TestBasicSplit()
{
    std::printf("\n=== StringViewList: Basic Split ===\n");

    String<char> text = "line1\nline2\nline3";
    StringViewList<char> list;
    list.FromString(text);

    TEST_ASSERT(list.GetCount() == 3, "3 lines");
    TEST_ASSERT(!list.IsEmpty(), "Not empty");

    TEST_ASSERT(list.GetString(0) == "line1", "Line 1");
    TEST_ASSERT(list.GetString(1) == "line2", "Line 2");
    TEST_ASSERT(list.GetString(2) == "line3", "Line 3");

    // operator[]
    TEST_ASSERT(list[0] == "line1" && list[2] == "line3", "operator[] access");

    // 越界返回空视图
    TEST_ASSERT(list.GetString(3).IsEmpty(), "Out of range returns empty view");
    TEST_ASSERT(list.GetString(-1).IsEmpty(), "Negative index returns empty view");
}

static void TestCRLF()
{
    std::printf("\n=== StringViewList: CRLF Handling ===\n");

    String<char> text = "line1\r\nline2\r\nline3";
    StringViewList<char> list;
    list.FromString(text);

    TEST_ASSERT(list.GetCount() == 3, "CRLF text has 3 lines");
    TEST_ASSERT(list.GetString(0) == "line1", "CRLF strips \\r");
    TEST_ASSERT(list.GetString(1) == "line2", "CRLF line 2");
    TEST_ASSERT(list.GetString(2) == "line3", "CRLF line 3");
}

static void TestEdgeCases()
{
    std::printf("\n=== StringViewList: Edge Cases ===\n");

    // 空文本
    String<char> empty_text = "";
    StringViewList<char> list;
    list.FromString(empty_text);
    TEST_ASSERT(list.GetCount() == 0, "Empty text has 0 lines");
    TEST_ASSERT(list.IsEmpty(), "Empty list IsEmpty");

    // 结尾换行：不产生空行
    String<char> trail = "a\nb\n";
    StringViewList<char> list2;
    list2.FromString(trail);
    TEST_ASSERT(list2.GetCount() == 2, "Trailing newline produces no empty line");

    // 中间空行：保留为空视图
    String<char> middle = "a\n\nb";
    StringViewList<char> list3;
    list3.FromString(middle);
    TEST_ASSERT(list3.GetCount() == 3, "Middle empty line kept as a line");
    TEST_ASSERT(list3.GetString(1).IsEmpty(), "Empty line is empty view");

    // 单行无换行
    String<char> single = "only";
    StringViewList<char> list4;
    list4.FromString(single);
    TEST_ASSERT(list4.GetCount() == 1 && list4.GetString(0) == "only", "Single line without newline");

    // 重复 FromString 会覆盖旧内容
    list4.FromString("a\nb");
    TEST_ASSERT(list4.GetCount() == 2 && list4.GetString(0) == "a", "Repeated FromString overwrites");
}

static void TestIteration()
{
    std::printf("\n=== StringViewList: Iteration ===\n");

    String<char> text = "x\ny\nz";
    StringViewList<char> list;
    list.FromString(text);

    // range-for
    int count = 0;
    int sum = 0;
    for(const auto &line : list)
    {
        ++count;
        sum += (int)line.Length();
    }

    TEST_ASSERT(count == 3, "Iterates 3 lines");
    TEST_ASSERT(sum == 3, "Each line length 1");

    // const 版本迭代
    const StringViewList<char> &clist = list;
    int ccount = 0;
    for(auto it = clist.cbegin(); it != clist.cend(); ++it)
        ++ccount;
    TEST_ASSERT(ccount == 3, "const iteration 3 lines");
}

static void TestLifetime()
{
    std::printf("\n=== StringViewList: View Lifetime ===\n");

    // 视图指向列表内部持有的原文，列表销毁后视图失效（由调用方保证生命周期）
    String<char> text = "alpha\nbeta";
    StringViewList<char> list;
    list.FromString(text);

    StringView<char> first = list.GetString(0);
    TEST_ASSERT(first == "alpha", "View content correct");

    // 修改原始 String 不影响已解析的列表（FromString 持有自己的副本）
    text = "changed";
    TEST_ASSERT(list.GetString(0) == "alpha", "List holds its own copy of text");

    // Clear 后行数归零
    list.Clear();
    TEST_ASSERT(list.IsEmpty() && list.GetCount() == 0, "Empty after Clear");
    TEST_ASSERT(list.GetString(0).IsEmpty(), "Access after Clear returns empty view");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestBasicSplit();
    TestCRLF();
    TestEdgeCases();
    TestIteration();
    TestLifetime();

    return TEST_SUMMARY();
}
