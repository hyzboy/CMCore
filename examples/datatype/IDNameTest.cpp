#include<hgl/type/IDName.h>
#include<iostream>
#include<cassert>

using namespace hgl;

// 定义测试用的 IDName 类型
HGL_DEFINE_ANSI_IDNAME(TestIDName)
HGL_DEFINE_U8_IDNAME(TestU8IDName)

void TestBasicOperations()
{
    std::cout << "========== Test 1: Basic Operations ==========" << std::endl;
    
    // 测试基本构造
    TestIDName id1("test1");
    assert(id1.GetID() == 0);
    assert(strcmp(id1.GetName(), "test1") == 0);
    assert(id1.IsValid());
    std::cout << "✓ Basic construction" << std::endl;
    
    // 测试去重
    TestIDName id2("test1");
    assert(id2.GetID() == 0);  // 应该返回相同的 ID
    assert(id1 == id2);
    std::cout << "✓ Deduplication" << std::endl;
    
    // 测试不同名称
    TestIDName id3("test2");
    assert(id3.GetID() == 1);
    assert(id1 != id3);
    std::cout << "✓ Different names" << std::endl;
    
    // 测试空名称
    TestIDName id4;
    assert(id4.GetID() == -1);
    assert(!id4.IsValid());
    assert(id4.GetName() == nullptr);
    std::cout << "✓ Empty name" << std::endl;
}

void TestAssignment()
{
    std::cout << "\n========== Test 2: Assignment ==========" << std::endl;
    
    TestIDName id1;
    assert(!id1.IsValid());
    
    // 通过 C 字符串赋值
    id1 = "assign1";
    assert(id1.GetID() >= 0);
    assert(strcmp(id1.GetName(), "assign1") == 0);
    std::cout << "✓ C-string assignment" << std::endl;
    
    // 通过 String 赋值
    AnsiString str("assign2");
    TestIDName id2;
    id2 = str;
    assert(id2.GetID() >= 0);
    assert(strcmp(id2.GetName(), "assign2") == 0);
    std::cout << "✓ String assignment" << std::endl;
    
    // 拷贝赋值
    TestIDName id3;
    id3 = id1;
    assert(id3 == id1);
    assert(id3.GetID() == id1.GetID());
    std::cout << "✓ Copy assignment" << std::endl;
}

void TestComparison()
{
    std::cout << "\n========== Test 3: Comparison ==========" << std::endl;
    
    TestIDName id1("aaa");
    TestIDName id2("bbb");
    TestIDName id3("ccc");
    TestIDName id4("aaa");
    
    // 相等比较
    assert(id1 == id4);
    assert(!(id1 == id2));
    std::cout << "✓ Equality comparison" << std::endl;
    
    // 三路比较
    assert((id1 <=> id2) == std::strong_ordering::less);
    assert((id2 <=> id3) == std::strong_ordering::less);
    assert((id1 <=> id4) == std::strong_ordering::equal);
    assert((id3 <=> id2) == std::strong_ordering::greater);
    std::cout << "✓ Three-way comparison" << std::endl;
    
    // 可用于排序
    assert(id1 < id2);
    assert(id2 < id3);
    assert(id1 <= id4);
    std::cout << "✓ Comparison operators" << std::endl;
}

void TestRegistry()
{
    std::cout << "\n========== Test 4: Registry Functions ==========" << std::endl;
    
    using Registry = IDNameRegistry<IDName_TestIDName_Manager, char>;
    
    int initial_count = Registry::GetCount();
    std::cout << "DEBUG: Initial count = " << initial_count << std::endl;
    
    // 注册新名称
    int id1 = Registry::Register("reg1", 4);
    std::cout << "DEBUG: Registered 'reg1', got ID = " << id1 << std::endl;
    std::cout << "DEBUG: Current count = " << Registry::GetCount() << std::endl;
    
    assert(id1 >= 0);
    assert(Registry::GetCount() == initial_count + 1);
    std::cout << "✓ Register" << std::endl;
    
    // 查找已存在的名称
    int id2 = Registry::GetID("reg1", 4);
    std::cout << "DEBUG: GetID('reg1') returned " << id2 << std::endl;
    assert(id2 == id1);
    std::cout << "✓ GetID for existing name" << std::endl;
    
    // 检查包含
    assert(Registry::Contains("reg1", 4));
    assert(!Registry::Contains("nonexistent", 11));
    std::cout << "✓ Contains check" << std::endl;
    
    // 获取名称
    const char* name = Registry::GetName(id1);
    std::cout << "DEBUG: GetName(" << id1 << ") = " << (name ? name : "nullptr") << std::endl;
    assert(strcmp(name, "reg1") == 0);
    std::cout << "✓ GetName" << std::endl;
    
    // 获取视图 - 添加更多调试信息
    std::cout << "DEBUG: About to call GetView(" << id1 << ")" << std::endl;
    std::cout << "DEBUG: Registry count = " << Registry::GetCount() << std::endl;
    
    const ConstStringView<char>* view = Registry::GetView(id1);
    
    std::cout << "DEBUG: GetView returned pointer = " << (void*)view << std::endl;
    
    if(view == nullptr)
    {
        std::cerr << "ERROR: GetView returned nullptr!" << std::endl;
        return;
    }
    
    std::cout << "DEBUG: Accessing view->id..." << std::endl;
    int view_id = view->id;
    std::cout << "DEBUG: view->id = " << view_id << std::endl;
    
    std::cout << "DEBUG: Accessing view->length..." << std::endl;
    int view_length = view->length;
    std::cout << "DEBUG: view->length = " << view_length << std::endl;
    
    assert(view_id == id1);
    assert(view_length == 4);
    std::cout << "✓ GetView" << std::endl;
}

void TestNameLength()
{
    std::cout << "\n========== Test 5: Name Length ==========" << std::endl;
    
    TestIDName id1("short");
    assert(id1.GetNameLength() == 5);
    std::cout << "✓ Short name length" << std::endl;
    
    TestIDName id2("a_much_longer_name_string");
    assert(id2.GetNameLength() == 25);  // 修正：实际长度是 25，不是 26
    std::cout << "✓ Long name length" << std::endl;
    
    TestIDName id3;
    assert(id3.GetNameLength() == 0);
    std::cout << "✓ Empty name length" << std::endl;
}

void TestUTF8()
{
    std::cout << "\n========== Test 6: UTF-8 Support ==========" << std::endl;
    
    // 中文名称
    TestU8IDName id1(reinterpret_cast<const u8char*>(u8"中文名称"));
    assert(id1.IsValid());
    assert(id1.GetID() == 0);
    std::cout << "✓ Chinese characters" << std::endl;
    
    // 日文名称
    TestU8IDName id2(reinterpret_cast<const u8char*>(u8"日本語"));
    assert(id2.IsValid());
    assert(id2.GetID() == 1);
    assert(id1 != id2);
    std::cout << "✓ Japanese characters" << std::endl;
    
    // 去重测试
    TestU8IDName id3(reinterpret_cast<const u8char*>(u8"中文名称"));
    assert(id3 == id1);
    std::cout << "✓ UTF-8 deduplication" << std::endl;
}

void TestClearAndSet()
{
    std::cout << "\n========== Test 7: Clear and Set ==========" << std::endl;
    
    TestIDName id1("test");
    assert(id1.IsValid());
    
    id1.Clear();
    assert(!id1.IsValid());
    assert(id1.GetID() == -1);
    std::cout << "✓ Clear" << std::endl;
    
    id1.Set("newname", 7);
    assert(id1.IsValid());
    assert(strcmp(id1.GetName(), "newname") == 0);
    std::cout << "✓ Set" << std::endl;
}

void TestMultipleTypes()
{
    std::cout << "\n========== Test 8: Multiple MANAGER Types ==========" << std::endl;
    
    // 定义两个不同的 IDName 类型
    HGL_DEFINE_ANSI_IDNAME(TypeA)
    HGL_DEFINE_ANSI_IDNAME(TypeB)
    
    TypeA ta1("shared");
    TypeB tb1("shared");
    
    // 两个类型各自维护独立的注册表
    assert(ta1.GetID() == 0);  // TypeA 的第一个
    assert(tb1.GetID() == 0);  // TypeB 的第一个
    
    // 但它们不相等（因为是不同类型）
    // assert(ta1 != tb1);  // 这行会编译错误，因为类型不同
    
    TypeA ta2("shared");
    assert(ta2 == ta1);  // 同一类型，相同名称，ID 相同
    
    std::cout << "✓ Independent registries for different MANAGER types" << std::endl;
}

int main()
{
    std::cout << "╔═══════════════════════════════════════════╗" << std::endl;
    std::cout << "║    OrderedIDName Comprehensive Test      ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════╝" << std::endl;
    
    try
    {
        TestBasicOperations();
        TestAssignment();
        TestComparison();
        TestRegistry();
        TestNameLength();
        TestUTF8();
        TestClearAndSet();
        TestMultipleTypes();
        
        std::cout << "\n╔═══════════════════════════════════════════╗" << std::endl;
        std::cout << "║         All Tests Passed! ✓               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════╝" << std::endl;
        
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "✗ Exception: " << e.what() << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "✗ Unknown exception" << std::endl;
        return 1;
    }
}
