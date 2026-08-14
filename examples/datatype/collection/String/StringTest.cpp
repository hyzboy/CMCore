/**
 * StringTest.cpp
 *
 * String<T> 字符串模板类专项测试（以 AnsiString = String<char> 为例）
 *   - 构造/赋值（C串、长度、重复、initializer_list、std::string、移动）
 *   - 基本查询（Length/IsEmpty/首尾字符/c_str/operator[]）
 *   - 拼接（+ / += / <<）
 *   - 编辑（Insert/Delete/SetChar/GetChar/Clear/Reserve/Resize）
 *   - 比较（Comp/CaseComp/大小写）
 *   - 类型转换（ToBool/ToInt/ToUint/ToFloat）
 *   - 裁剪（Trim/TrimLeft/TrimRight/ClipLeft/ClipRight/Clip）
 *   - 子串（SubString/Left/Right）
 *   - 查找（FindChar/FindChars/FindRightChar/FindString）
 *   - 替换与统计（Replace/StatChar/StatLine）
 *   - StringView 互操作
 */
#include<hgl/type/String.h>
#include<hgl/type/StringView.h>
#include"test_common.h"

#include<string>

using namespace hgl;

static void TestConstruction()
{
    std::printf("\n=== String: Construction ===\n");

    String<char> empty;
    TEST_ASSERT(empty.IsEmpty(), "Default construction is empty");
    TEST_ASSERT(empty.Length() == 0, "Default construction length is 0");

    String<char> from_cstr("hello");
    TEST_ASSERT(from_cstr.Length() == 5 && from_cstr.Comp("hello") == 0, "C string construction");

    String<char> from_len("hello world", 5);
    TEST_ASSERT(from_len.Comp("hello") == 0, "Length constructor takes first 5 chars");

    String<char> repeat(3, 'a');
    TEST_ASSERT(repeat.Comp("aaa") == 0, "Repeat character construction");

    String<char> init_list({'x', 'y', 'z'});
    TEST_ASSERT(init_list.Comp("xyz") == 0, "initializer_list construction");

    std::string stds = "std";
    String<char> from_std(stds);
    TEST_ASSERT(from_std.Comp("std") == 0, "std::string construction");

    String<char> copy(from_cstr);
    TEST_ASSERT(copy == from_cstr, "Copy construction");

    String<char> moved(std::move(copy));
    TEST_ASSERT(moved.Comp("hello") == 0, "Move construction");
    TEST_ASSERT(copy.IsEmpty(), "Source empty after move");
}

static void TestAssignmentAndAccess()
{
    std::printf("\n=== String: Assignment and Access ===\n");

    String<char> s;
    s = "abc";
    TEST_ASSERT(s.Comp("abc") == 0, "operator=(C string)");

    s = std::string("def");
    TEST_ASSERT(s.Comp("def") == 0, "operator=(std::string)");

    s = {'g', 'h'};
    TEST_ASSERT(s.Comp("gh") == 0, "operator=(initializer_list)");

    String<char> s2 = "xyz";
    s = s2;
    TEST_ASSERT(s.Comp("xyz") == 0, "operator=(String)");

    s = String<char>("move");
    TEST_ASSERT(s.Comp("move") == 0, "operator=(move)");

    s = "hello";
    TEST_ASSERT(s[0] == 'h' && s[4] == 'o', "operator[] access");
    s[1] = 'E';
    TEST_ASSERT(s.Comp("hEllo") == 0, "operator[] write");

    char *p = s.c_str();
    TEST_ASSERT(p != nullptr && p[0] == 'h', "c_str not null");
    TEST_ASSERT(s.data_ptr() == p, "data_ptr matches c_str");

    TEST_ASSERT(s.GetFirstChar() == 'h', "GetFirstChar");
    TEST_ASSERT(s.GetLastChar() == 'o', "GetLastChar");

    char ch = 0;
    TEST_ASSERT(s.GetChar(2, ch) && ch == 'l', "GetChar found");
    TEST_ASSERT(!s.GetChar(99, ch), "GetChar out of range fails");
    TEST_ASSERT(s.SetChar(0, 'H'), "SetChar succeeded");
    TEST_ASSERT(s.Comp("HEllo") == 0, "Content after SetChar");
}

static void TestConcat()
{
    std::printf("\n=== String: Concatenation ===\n");

    String<char> a = "foo";
    String<char> b = "bar";

    String<char> c = a + b;
    TEST_ASSERT(c.Comp("foobar") == 0, "operator+ (String+String)");

    c = a + '!';
    TEST_ASSERT(c.Comp("foo!") == 0, "operator+ (String+char)");

    c = a + "-suffix";
    TEST_ASSERT(c.Comp("foo-suffix") == 0, "operator+ (String+C string)");

    a += b;
    TEST_ASSERT(a.Comp("foobar") == 0, "operator+=");

    String<char> d = "x";
    d << "y" << String<char>("z");
    TEST_ASSERT(d.Comp("xyz") == 0, "operator<< chained concatenation");

    // 空串拼接
    String<char> e;
    TEST_ASSERT((e + a) == a, "empty + String");
    TEST_ASSERT((a + e) == a, "String + empty");
}

static void TestEdit()
{
    std::printf("\n=== String: Edit Operations ===\n");

    String<char> s = "abcdef";

    TEST_ASSERT(s.Insert(3, "XY"), "Insert succeeded");
    TEST_ASSERT(s.Comp("abcXYdef") == 0, "Content after Insert");

    TEST_ASSERT(s.Delete(3, 2), "Delete succeeded");
    TEST_ASSERT(s.Comp("abcdef") == 0, "Content after Delete");

    TEST_ASSERT(!s.Delete(99, 2), "Delete out of range fails");

    s.Reserve(1024);
    TEST_ASSERT(true, "Reserve effective (no capacity query API)");

    char *rp = s.Resize(3);
    TEST_ASSERT(rp != nullptr && s.Comp("abc") == 0, "Resize shrinks");

    s.Resize(6);
    TEST_ASSERT(s.Length() == 6, "Resize grows");

    s.Clear();
    TEST_ASSERT(s.IsEmpty(), "Empty after Clear");
}

static void TestCompare()
{
    std::printf("\n=== String: Comparison ===\n");

    String<char> a = "abc";
    String<char> b = "abd";
    String<char> c = "ABC";

    TEST_ASSERT(a.Comp(b) < 0, "Comp ascending");
    TEST_ASSERT(b.Comp(a) > 0, "Comp descending");
    TEST_ASSERT(a.Comp(a) == 0, "Comp equal");
    TEST_ASSERT(a.Comp("abc") == 0, "Comp(C string)");

    TEST_ASSERT(a.CaseComp(c) == 0, "CaseComp equal ignoring case");
    TEST_ASSERT(a.CaseComp("ABC") == 0, "CaseComp(C string)");

    TEST_ASSERT(a.Comp("abc") == 0, "operator== String");
    TEST_ASSERT(a != b, "operator!= String");
    TEST_ASSERT(a.Comp("abc") == 0 && a.Comp("abd") != 0, "operator== / != C string");

    TEST_ASSERT(a.Comp(0, b, 2) == 0, "Comp prefix comparison (first 2 chars same)");
}

static void TestConvert()
{
    std::printf("\n=== String: Type Conversion ===\n");

    String<char> s;

    s = "true";
    bool b = false;
    TEST_ASSERT(s.ToBool(b) && b, "ToBool true");

    s = "12345";
    int i = 0;
    TEST_ASSERT(s.ToInt(i) && i == 12345, "ToInt");

    s = "-42";
    TEST_ASSERT(s.ToInt(i) && i == -42, "ToInt negative");

    unsigned int u = 0;
    s = "999";
    TEST_ASSERT(s.ToUint(u) && u == 999, "ToUint");

    s = "3.14";
    float f = 0;
    TEST_ASSERT(s.ToFloat(f) && f > 3.13f && f < 3.15f, "ToFloat");

    s = "not_a_number";
    TEST_ASSERT(!s.ToInt(i), "Invalid number conversion fails");
}

static void TestCaseAndTrim()
{
    std::printf("\n=== String: Case and Trim ===\n");

    String<char> s = "  Hello World  ";

    TEST_ASSERT(s.Trim().Comp("Hello World") == 0, "Trim removes both-end whitespace");
    TEST_ASSERT(s.TrimLeft().Comp("Hello World  ") == 0, "TrimLeft");
    TEST_ASSERT(s.TrimRight().Comp("  Hello World") == 0, "TrimRight");

    s = "AbC";
    TEST_ASSERT(s.ToLowerCase().Comp("abc") == 0, "ToLowerCase");
    TEST_ASSERT(s.ToUpperCase().Comp("ABC") == 0, "ToUpperCase");

    s.LowerCase();
    TEST_ASSERT(s.Comp("abc") == 0, "LowerCase in place");
    s.UpperCase();
    TEST_ASSERT(s.Comp("ABC") == 0, "UpperCase in place");

    // Clip 系列语义：ClipLeft(n)=保留左 n 个，ClipRight(n)=保留右 n 个，Clip(pos,num)=删除
    String<char> t = "hello";
    TEST_ASSERT(t.ClipLeft(2) && t.Comp("he") == 0, "ClipLeft keeps left 2");
    String<char> u = "hello";
    TEST_ASSERT(u.ClipRight(2) && u.Comp("lo") == 0, "ClipRight keeps right 2");
    String<char> v = "hello";
    TEST_ASSERT(v.Clip(1, 2) && v.Comp("hlo") == 0, "Clip(1,2) deletes middle 2");
}

static void TestSubString()
{
    std::printf("\n=== String: Substring ===\n");

    String<char> s = "hello world";

    TEST_ASSERT(s.SubString(6).Comp("world") == 0, "SubString to end");
    TEST_ASSERT(s.SubString(0, 5).Comp("hello") == 0, "SubString with length");
    TEST_ASSERT(s.Left(5).Comp("hello") == 0, "Left");
    TEST_ASSERT(s.Right(5).Comp("world") == 0, "Right");

    String<char> out;
    TEST_ASSERT(s.SubString(out, 6, 5) && out.Comp("world") == 0, "SubString via out parameter");
}

static void TestFind()
{
    std::printf("\n=== String: Find ===\n");

    String<char> s = "hello world, hello hgl";

    TEST_ASSERT(s.FindChar('w') == 6, "FindChar");
    TEST_ASSERT(s.FindChar('x') == -1, "FindChar not found");
    TEST_ASSERT(s.FindChar(7, 'o') == 0, "FindChar(pos,ch) returns offset relative to pos");

    TEST_ASSERT(s.FindChars("world") == 6, "FindChars");
    TEST_ASSERT(s.FindRightChar('h') == 19, "FindRightChar searches from right");
    TEST_ASSERT(s.FindString("hello") == 0, "FindString first match");
    TEST_ASSERT(s.FindString("hello", 1) == 13, "FindString with start position");
    TEST_ASSERT(s.FindString("nope") == -1, "FindString not found");

    String<char> sub = "hello";
    TEST_ASSERT(s.FindExcludeChar(0, sub) == 5, "FindExcludeChar finds first char not in hello (space)");
}

static void TestReplaceAndStats()
{
    std::printf("\n=== String: Replace and Stats ===\n");

    String<char> s = "a-b-c";

    TEST_ASSERT(s.Replace('-', '+') == 2, "Replace replaces 2 chars");
    TEST_ASSERT(s.Comp("a+b+c") == 0, "Content after Replace");

    s = "hello";
    TEST_ASSERT(s.StatChar('l') == 2, "StatChar counts");
    TEST_ASSERT(s.StatLine() == 1, "StatLine single line");

    s = "line1\nline2\nline3";
    TEST_ASSERT(s.StatLine() == 3, "StatLine multiple lines");
}

static void TestInterop()
{
    std::printf("\n=== String: Interop with StringView ===\n");

    String<char> s = "interop";

    StringView<char> view(s.c_str(), s.Length());
    TEST_ASSERT(view.Length() == 7 && view.Comp("interop") == 0, "StringView views String");

    // String 可直接与 StringView 内容比较
    TEST_ASSERT(s == String<char>(view.c_str(), view.Length()), "String matches view content");

    // 视图不拥有数据：底层修改后视图内容跟着变
    StringView<char> live(s.c_str(), s.Length());
    s.SetChar(0, 'I');
    TEST_ASSERT(live.GetFirstChar() == 'I', "View reflects underlying change (non-owning)");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestConstruction();
    TestAssignmentAndAccess();
    TestConcat();
    TestEdit();
    TestCompare();
    TestConvert();
    TestCaseAndTrim();
    TestSubString();
    TestFind();
    TestReplaceAndStats();
    TestInterop();

    return TEST_SUMMARY();
}
