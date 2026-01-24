/**
 * 快速测试脚本
 * 用于快速验证ArrayList和ValueBuffer的基本功能
 */

#include<hgl/type/ObjectList.h>
#include<iostream>
#include<string>

using namespace hgl;

// 简单的非平凡类
class TestClass
{
public:
    int id;
    std::string name;

    TestClass() : id(0), name("") { }
    TestClass(int i, const std::string& n) : id(i), name(n) { }
};

void TestArrayListBasic()
{
    std::cout << "=== ArrayList Basic Test ===" << std::endl;

    // 测试int类型
    ArrayList<int> intList;
    intList.Add(10);
    intList.Add(20);
    intList.Add(30);

    std::cout << "Int ArrayList: ";
    for(int i = 0; i < intList.GetCount(); i++)
        std::cout << *intList.At(i) << " ";
    std::cout << std::endl;

    // 测试RepeatAdd
    intList.RepeatAdd(99, 3);
    std::cout << "After RepeatAdd(99, 3): ";
    for(int i = 0; i < intList.GetCount(); i++)
        std::cout << *intList.At(i) << " ";
    std::cout << std::endl;

    // 测试Delete
    intList.Delete(0, 2);
    std::cout << "After Delete(0, 2): ";
    for(int i = 0; i < intList.GetCount(); i++)
        std::cout << *intList.At(i) << " ";
    std::cout << std::endl;
}

void TestDataArrayBasic()
{
    std::cout << "\n=== ValueBuffer Basic Test ===" << std::endl;

    ValueBuffer<int> arr;
    arr.Resize(5);

    for(int64 i = 0; i < 5; i++)
        arr[i] = i * 10;

    std::cout << "Initial: ";
    for(int64 i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // 测试Delete
    arr.Delete(1, 2);
    std::cout << "After Delete(1, 2): ";
    for(int64 i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    // 测试Move
    arr.Resize(8);
    for(int64 i = 3; i < 8; i++)
        arr[i] = i * 10;

    std::cout << "Before Move: ";
    for(int64 i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;

    arr.Move(0, 5, 2);
    std::cout << "After Move(0, 5, 2): ";
    for(int64 i = 0; i < arr.GetCount(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

void TestNonTrivialType()
{
    std::cout << "\n=== Non-Trivial Type Test ===" << std::endl;

    ObjectList<TestClass> list;

    // 使用新的便捷方法：直接添加对象值，ObjectList会自动创建堆副本
    std::cout << "[1] Add objects with values (auto heap allocation):" << std::endl;
    list.Add(TestClass(1, "First"));
    list.Add(TestClass(2, "Second"));
    list.Add(TestClass(3, "Third"));

    std::cout << "TestClass ObjectList: ";
    for(int i = 0; i < list.GetCount(); i++)
    {
        const TestClass* obj = *list.At(i);  // Dereference to get pointer
        std::cout << "{" << obj->id << ":" << obj->name << "} ";
    }
    std::cout << std::endl;

    // 测试RepeatAdd - 也使用便捷方法
    std::cout << "\n[2] RepeatAdd objects with values:" << std::endl;
    TestClass repeated(99, "Repeated");
    list.RepeatAdd(repeated, 2);

    std::cout << "After RepeatAdd(99, 2): ";
    for(int i = 0; i < list.GetCount(); i++)
    {
        const TestClass* obj = *list.At(i);  // Dereference to get pointer
        std::cout << "{" << obj->id << ":" << obj->name << "} ";
    }
    std::cout << std::endl;

    // 测试Get
    std::cout << "\n[3] Get object by index:" << std::endl;
    TestClass* obj = nullptr;
    if(list.Get(0, obj))
    {
        std::cout << "First object: {" << obj->id << ":" << obj->name << "}" << std::endl;
    }

    // 测试迭代器
    std::cout << "\n[4] Iterate using Iterator:" << std::endl;
    for(auto it = list.begin(); it != list.end(); ++it)
    {
        TestClass* item = *it;
        std::cout << "{" << item->id << ":" << item->name << "} ";
    }
    std::cout << std::endl;

    // 测试删除
    std::cout << "\n[5] Delete object at index 1:" << std::endl;
    list.DeleteAtOwn(1);  // Delete and destroy
    std::cout << "After delete: ";
    for(int i = 0; i < list.GetCount(); i++)
    {
        const TestClass* obj = *list.At(i);  // Dereference to get pointer
        std::cout << "{" << obj->id << ":" << obj->name << "} ";
    }
    std::cout << std::endl;

    std::cout << "Final count: " << list.GetCount() << std::endl;
    // list destructor will automatically delete all objects
}

int main()
{
    std::cout << "====================================" << std::endl;
    std::cout << "Quick ArrayList/ValueBuffer Test" << std::endl;
    std::cout << "====================================" << std::endl;

    try
    {
        TestArrayListBasic();
        TestDataArrayBasic();
        TestNonTrivialType();

        std::cout << "\n====================================" << std::endl;
        std::cout << "All tests completed successfully!" << std::endl;
        std::cout << "====================================" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
