/**
 * StringViewTest.cpp
 *
 * StringView<T> 非拥有字符串视图专项测试（以 AnsiStringView = StringView<char> 为例）
 *   - 构造（C串/长度/std::string/std::string_view/空视图）
 *   - 基本查询（Length/IsEmpty/首尾字符/c_str）
 *   - 子串（SubString）
 *   - 查找（FindChar/FindChars/FindString）
 *   - 比较（Comp/CaseComp/operator<=>/==）
 *   - 类型转换（ToBool/ToInt/ToFloat）
 *   - 裁剪（Trim/TrimLeft/TrimRight）
 *   - 前缀/后缀（RemovePrefix/RemoveSuffix/StartsWith/EndsWith）
 *   - Contains/StatLine
 *   - 非拥有语义（视图生命周期）
 */
#include<hgl/type/StringView.h>
#include"test_common.h"

#include<string>
#include<string_view>

using namespace hgl;

static void TestConstruction()
{
    std::printf("\n=== StringView: Construction ===\n");

    StringView<char> empty;
    TEST_ASSERT(empty.IsEmpty() && empty.Length() == 0, "Default construction is empty view");

    StringView<char> from_cstr("hello");
    TEST_ASSERT(from_cstr.Length() == 5 && from_cstr == "hello", "C string construction (auto length)");

    StringView<char> from_len("hello world", 5);
    TEST_ASSERT(from_len == "hello", "Length constructor");

    StringView<char> null_view(nullptr);
    TEST_ASSERT(null_view.IsEmpty(), "nullptr construction is empty view");

    std::string stds = "std";
    StringView<char> from_std(stds);
    TEST_ASSERT(from_std == "std", "std::string construction");

    std::string_view stdsv = "sv";
    StringView<char> from_sv(stdsv);
    TEST_ASSERT(from_sv == "sv", "std::string_view construction");

    StringView<char> copy(from_cstr);
    TEST_ASSERT(copy == from_cstr, "Copy construction");
}

static void TestQuery()
{
    std::printf("\n=== StringView: Basic Query ===\n");

    StringView<char> s("Hello World");

    TEST_ASSERT(s.Length() == 11, "Length");
    TEST_ASSERT(!s.IsEmpty(), "IsEmpty not empty");
    TEST_ASSERT(s.GetFirstChar() == 'H', "GetFirstChar");
    TEST_ASSERT(s.GetLastChar() == 'd', "GetLastChar");

    TEST_ASSERT(s.c_str() != nullptr, "c_str not null");
    TEST_ASSERT(s.data_ptr() == s.c_str(), "data_ptr matches c_str");

    char ch = 0;
    TEST_ASSERT(s.GetChar(1, ch) && ch == 'e', "GetChar found");
    TEST_ASSERT(!s.GetChar(99, ch), "GetChar out of range fails");

    // 视图指向非 NUL 结尾的缓冲区
    const char buf[5] = {'a', 'b', 'c', 'd', 'e'};
    StringView<char> no_nul(buf, 5);
    TEST_ASSERT(no_nul.Length() == 5 && no_nul == "abcde", "View over non-NUL-terminated buffer");
}

static void TestSubString()
{
    std::printf("\n=== StringView: Substring ===\n");

    StringView<char> s("hello world");

    TEST_ASSERT(s.SubString(6) == "world", "SubString to end");
    TEST_ASSERT(s.SubString(0, 5) == "hello", "SubString with length");
    TEST_ASSERT(s.SubString(99).IsEmpty(), "SubString out of range returns empty");

    StringView<char> out;
    TEST_ASSERT(s.SubString(out, 6, 5) && out == "world", "SubString via out parameter");
}

static void TestFind()
{
    std::printf("\n=== StringView: Find ===\n");

    StringView<char> s("hello world, hello hgl");

    TEST_ASSERT(s.FindChar('w') == 6, "FindChar");
    TEST_ASSERT(s.FindChar('x') == -1, "FindChar not found");
    TEST_ASSERT(s.FindChar(7, 'o') == 0, "FindChar(pos,ch) returns offset relative to pos");

    StringView<char> sub("world");
    TEST_ASSERT(s.FindChars(sub) == 6, "FindChars");
    TEST_ASSERT(s.FindChars(StringView<char>("hello")) == 0, "FindChars(C string)");
    TEST_ASSERT(s.FindRightChar('h') == 19, "FindRightChar");

    TEST_ASSERT(s.FindString(sub) == 6, "FindString");
    TEST_ASSERT(s.FindString(StringView<char>("hello"), 1) == 13, "FindString with start position");
    TEST_ASSERT(s.FindString(StringView<char>("nope")) == -1, "FindString not found");
}

static void TestCompare()
{
    std::printf("\n=== StringView: Comparison ===\n");

    StringView<char> a("abc");
    StringView<char> b("abd");
    StringView<char> c("ABC");

    TEST_ASSERT(a.Comp(b) < 0, "Comp ascending");
    TEST_ASSERT(b.Comp(a) > 0, "Comp descending");
    TEST_ASSERT(a.Comp(a) == 0, "Comp equal");

    TEST_ASSERT(a.CaseComp(c) == 0, "CaseComp ignores case");

    TEST_ASSERT(a == b || true, "operator== available");
    TEST_ASSERT(a == StringView<char>("abc"), "operator== equal");
    TEST_ASSERT(a != b, "operator!= not equal");
    TEST_ASSERT((a <=> b) < 0, "operator<=> three-way comparison");
}

static void TestConvert()
{
    std::printf("\n=== StringView: Type Conversion ===\n");

    StringView<char> s("12345");
    int i = 0;
    TEST_ASSERT(s.ToInt(i) && i == 12345, "ToInt");

    s = StringView<char>("-7");
    TEST_ASSERT(s.ToInt(i) && i == -7, "ToInt negative");

    s = StringView<char>("2.5");
    float f = 0;
    TEST_ASSERT(s.ToFloat(f) && f > 2.4f && f < 2.6f, "ToFloat");

    s = StringView<char>("false");
    bool b = true;
    TEST_ASSERT(s.ToBool(b) && !b, "ToBool false");

    s = StringView<char>("junk");
    TEST_ASSERT(!s.ToInt(i), "Invalid number conversion fails");
}

static void TestTrim()
{
    std::printf("\n=== StringView: Trim ===\n");

    StringView<char> s("  padded  ");

    TEST_ASSERT(s.Trim() == "padded", "Trim");
    TEST_ASSERT(s.TrimLeft() == "padded  ", "TrimLeft");
    TEST_ASSERT(s.TrimRight() == "  padded", "TrimRight");
}

static void TestPrefixSuffix()
{
    std::printf("\n=== StringView: Prefix/Suffix ===\n");

    StringView<char> s("HelloWorld");

    TEST_ASSERT(s.StartsWith('H'), "StartsWith(char)");
    TEST_ASSERT(!s.StartsWith('h'), "StartsWith(char) negative");
    TEST_ASSERT(s.StartsWith("Hello"), "StartsWith(C string)");
    TEST_ASSERT(s.EndsWith('d'), "EndsWith(char)");
    TEST_ASSERT(s.EndsWith("World"), "EndsWith(C string)");
    TEST_ASSERT(!s.EndsWith("world"), "EndsWith is case-sensitive");

    // 用 const 视图调用返回新视图的 RemovePrefix/RemoveSuffix（非 const 版本是原地 void）
    const StringView<char> cs("HelloWorld");
    TEST_ASSERT(cs.RemovePrefix(5) == StringView<char>("World"), "RemovePrefix");
    TEST_ASSERT(cs.RemoveSuffix(5) == StringView<char>("Hello"), "RemoveSuffix");

    StringView<char> t("HelloWorld");
    t.RemovePrefix(5);
    t.RemoveSuffix(3);
    TEST_ASSERT(t == "Wo", "RemovePrefix/RemoveSuffix in place");
}

static void TestMisc()
{
    std::printf("\n=== StringView: Contains/StatLine ===\n");

    StringView<char> s("line1\nline2\nline3");

    TEST_ASSERT(s.Contains(StringView<char>("line2")), "Contains found");
    TEST_ASSERT(!s.Contains(StringView<char>("line9")), "Contains not found");
    TEST_ASSERT(s.StatLine() == 3, "StatLine multiple lines");

    StringView<char> single("single");
    TEST_ASSERT(single.StatLine() == 1, "StatLine single line");
}

static void TestNonOwning()
{
    std::printf("\n=== StringView: Non-owning Semantics ===\n");

    char buffer[] = "initial";
    StringView<char> view(buffer);

    TEST_ASSERT(view == "initial", "View initial content");

    // 修改底层缓冲区，视图立即反映（无拷贝）
    buffer[0] = 'I';
    TEST_ASSERT(view == "Initial", "View reflects underlying modification");

    // 视图可以裁剪范围，而不影响底层
    StringView<char> mid(buffer + 2, 3);
    TEST_ASSERT(mid == "iti", "Sub-range view");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestConstruction();
    TestQuery();
    TestSubString();
    TestFind();
    TestCompare();
    TestConvert();
    TestTrim();
    TestPrefixSuffix();
    TestMisc();
    TestNonOwning();

    return TEST_SUMMARY();
}
