/**
 * 修复验证：ObjectArray 对 non-trivial 类型的处理
 * 
 * 问题：Resize 后直接使用赋值会崩溃，因为对象未构造
 * 解决：需要先构造对象（placement new）再赋值
 */

#include<hgl/type/PtrArray.h>
#include<iostream>
#include<string>

using namespace hgl;

class SimpleClass
{
public:
    int id;
    std::string name;
    
    SimpleClass() : id(0), name("")
    {
        std::cout << "  Default construct" << std::endl;
    }
    
    SimpleClass(int i, const std::string& n) : id(i), name(n)
    {
        std::cout << "  Construct #" << id << std::endl;
    }
    
    SimpleClass& operator=(const SimpleClass& other)
    {
        std::cout << "  Assign " << other.id << " to " << id << std::endl;
        id = other.id;
        name = other.name;
        return *this;
    }
    
    ~SimpleClass()
    {
        std::cout << "  Destruct #" << id << std::endl;
    }
};

int main()
{
    std::cout << "=== Test: ValueBuffer with non-trivial type ===" << std::endl;
    
    std::cout << "\n1. Create ValueBuffer and Resize to 3:" << std::endl;
    PtrArray<SimpleClass> arr;
    arr.Resize(3);  // 分配内存但不构造对象！
    
    std::cout << "\n2. Try to WriteAt (will crash if objects not constructed):" << std::endl;
    SimpleClass obj(1, "Test");
    
    try
    {
        arr.WriteAt(obj, 0);  // 这里会崩溃！因为 items[0] 没有被构造
        std::cout << "  WriteAt succeeded (unexpected!)" << std::endl;
    }
    catch(...)
    {
        std::cout << "  WriteAt crashed (expected)" << std::endl;
    }
    
    std::cout << "\n=== Test completed ===" << std::endl;
    return 0;
}
