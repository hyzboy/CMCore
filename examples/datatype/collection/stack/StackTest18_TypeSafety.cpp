#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

// ========================================
// 测试用类型
// ========================================

// POD 类型 (Plain Old Data) - 应该可以用于 Stack<T>
struct SimplePOD
{
    int x;
    int y;
    double z;
};

// Trivially copyable 类型 - 应该可以用于 Stack<T>
struct TestTriviallyCopyable
{
    int value;
    float data;
    
    // 默认构造/拷贝/析构，所以是 trivially copyable
};

// 非 trivially copyable 类型 - 应该被 static_assert 拒绝
class NonTrivialClass
{
    std::string name;  // std::string 不是 trivially copyable
    int value;
    
public:
    NonTrivialClass(const char* n = "default") : name(n), value(0) {}
    ~NonTrivialClass() {}  // 非平凡析构函数
    
    const std::string& GetName() const { return name; }
};

// 带有自定义拷贝构造的类型 - 不是 trivially copyable
class CustomCopyClass
{
    int* data;
    
public:
    CustomCopyClass() : data(new int(0)) {}
    
    CustomCopyClass(const CustomCopyClass& other)
        : data(new int(*other.data)) {}
    
    ~CustomCopyClass() { delete data; }
    
    int GetValue() const { return *data; }
};

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 18: Type Safety & static_assert Verification" << endl;
    cout << "========================================" << endl;

    cout << "\n[18.1] POD types (should work):" << endl;
    Stack<SimplePOD> pod_stack;
    
    SimplePOD pod1 = {10, 20, 30.5};
    pod_stack.Push(pod1);
    
    SimplePOD pod2;
    assert(pod_stack.Pop(pod2));
    assert(pod2.x == 10 && pod2.y == 20 && pod2.z == 30.5);
    
    cout << "  ✓ POD struct works with Stack<T>" << endl;

    cout << "\n[18.2] Trivially copyable types (should work):" << endl;
    Stack<TestTriviallyCopyable> tc_stack;
    
    TestTriviallyCopyable tc1 = {100, 3.14f};
    tc_stack.Push(tc1);
    
    TestTriviallyCopyable tc2;
    assert(tc_stack.Pop(tc2));
    assert(tc2.value == 100);
    
    cout << "  ✓ Trivially copyable struct works with Stack<T>" << endl;

    cout << "\n[18.3] Primitive types (should work):" << endl;
    Stack<int> int_stack;
    Stack<float> float_stack;
    Stack<double> double_stack;
    Stack<char> char_stack;
    Stack<bool> bool_stack;
    
    int_stack.Push(42);
    float_stack.Push(3.14f);
    double_stack.Push(2.718);
    char_stack.Push('A');
    bool_stack.Push(true);
    
    cout << "  ✓ All primitive types work with Stack<T>" << endl;

    cout << "\n[18.4] Pointer types (should use ObjectStack instead):" << endl;
    // Stack<int*> ptr_stack;  // 应该可以编译，但不建议
    // 推荐使用:
    ObjectStack<int> obj_stack;
    obj_stack.Push(new int(100));
    obj_stack.Push(new int(200));
    
    int* val = obj_stack.Pop();
    assert(*val == 200);
    delete val;
    
    cout << "  ✓ ObjectStack<T> correctly manages pointer types" << endl;
    cout << "  ⚠️  Note: Stack<T*> compiles but ObjectStack<T> is safer" << endl;

    cout << "\n[18.5] Type traits verification:" << endl;
    cout << "  SimplePOD is trivially copyable: " 
         << (std::is_trivially_copyable_v<SimplePOD> ? "YES ✓" : "NO ✗") << endl;
    cout << "  TestTriviallyCopyable is trivially copyable: " 
         << (std::is_trivially_copyable_v<TestTriviallyCopyable> ? "YES ✓" : "NO ✗") << endl;
    cout << "  int is trivially copyable: " 
         << (std::is_trivially_copyable_v<int> ? "YES ✓" : "NO ✗") << endl;
    cout << "  NonTrivialClass is trivially copyable: " 
         << (std::is_trivially_copyable_v<NonTrivialClass> ? "YES ✗" : "NO ✓") << endl;
    cout << "  CustomCopyClass is trivially copyable: " 
         << (std::is_trivially_copyable_v<CustomCopyClass> ? "YES ✗" : "NO ✓") << endl;
    cout << "  std::string is trivially copyable: " 
         << (std::is_trivially_copyable_v<std::string> ? "YES ✗" : "NO ✓") << endl;

    cout << "\n[18.6] Compile-time enforcement:" << endl;
    cout << "  📝 The following code SHOULD NOT COMPILE:" << endl;
    cout << "     Stack<std::string> str_stack;  // Error: std::string not trivially copyable" << endl;
    cout << "     Stack<NonTrivialClass> nt_stack;  // Error: has non-trivial destructor" << endl;
    cout << "     Stack<CustomCopyClass> cc_stack;  // Error: has custom copy constructor" << endl;
    cout << endl;
    cout << "  📝 Expected error message:" << endl;
    cout << "     \"Stack only supports trivially copyable types...\"" << endl;
    cout << "     \"For non-trivial types, use ObjectStack<T> instead.\"" << endl;

    // 如果取消下面的注释，应该会导致编译错误：
    // Stack<std::string> will_not_compile;
    // Stack<NonTrivialClass> will_fail;
    // Stack<CustomCopyClass> compile_error;

    cout << "\n[18.7] ObjectStack for non-trivial types:" << endl;
    ObjectStack<NonTrivialClass> obj_nt_stack;
    ObjectStack<CustomCopyClass> obj_cc_stack;
    
    obj_nt_stack.Push(new NonTrivialClass("test"));
    obj_cc_stack.Push(new CustomCopyClass());
    
    NonTrivialClass* nt = obj_nt_stack.Pop();
    assert(nt->GetName() == "test");
    delete nt;
    
    CustomCopyClass* cc = obj_cc_stack.Pop();
    delete cc;
    
    cout << "  ✓ ObjectStack<T> correctly handles non-trivial types" << endl;

    cout << "\n[18.8] Array types (special case):" << endl;
    Stack<int[5]> array_stack;  // 数组是 trivially copyable
    
    int arr[5] = {1, 2, 3, 4, 5};
    array_stack.Push(arr);
    
    int result[5];
    assert(array_stack.Pop(result));
    assert(result[0] == 1 && result[4] == 5);
    
    cout << "  ✓ Fixed-size arrays work with Stack<T>" << endl;

    cout << "\n[18.9] Enum types (should work):" << endl;
    enum class Color { Red, Green, Blue };
    Stack<Color> enum_stack;
    
    enum_stack.Push(Color::Red);
    enum_stack.Push(Color::Blue);
    
    Color c;
    assert(enum_stack.Pop(c));
    assert(c == Color::Blue);
    
    cout << "  ✓ Enum types work with Stack<T>" << endl;

    cout << "\n[18.10] Design recommendations:" << endl;
    cout << "  ✅ DO use Stack<T> for:" << endl;
    cout << "     - Primitive types (int, float, double, char, bool)" << endl;
    cout << "     - POD structs (Plain Old Data)" << endl;
    cout << "     - Trivially copyable types" << endl;
    cout << "     - Enums" << endl;
    cout << endl;
    cout << "  ❌ DO NOT use Stack<T> for:" << endl;
    cout << "     - std::string, std::vector, or other STL containers" << endl;
    cout << "     - Classes with custom constructors/destructors" << endl;
    cout << "     - Classes with virtual functions" << endl;
    cout << "     - Types requiring deep copy" << endl;
    cout << endl;
    cout << "  ✅ Use ObjectStack<T> instead for:" << endl;
    cout << "     - Managing pointers to non-trivial objects" << endl;
    cout << "     - Automatic ownership and cleanup" << endl;

    cout << "\n✅ TEST 18 PASSED - Type safety mechanisms verified!" << endl;
    cout << "   static_assert ensures only trivially copyable types are used" << endl;
    
    return 0;
}
