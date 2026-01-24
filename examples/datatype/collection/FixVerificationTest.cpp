/**
 * 验证 ValueBuffer 修复后对 non-trivial 类型的支持
 */

#include<hgl/type/ValueBuffer.h>
#include<hgl/type/ObjectArray.h>
#include<hgl/type/ValueArray.h>
#include<iostream>
#include<string>

using namespace hgl;

class TestObject
{
public:
    int id;
    std::string name;

    static int aliveCount;

    TestObject() : id(0), name("")
    {
        aliveCount++;
        std::cout << "  [" << id << "] Default construct (alive=" << aliveCount << ")" << std::endl;
    }

    TestObject(int i, const std::string& n) : id(i), name(n)
    {
        aliveCount++;
        std::cout << "  [" << id << "] Construct '" << name << "' (alive=" << aliveCount << ")" << std::endl;
    }

    TestObject(const TestObject& other) : id(other.id), name(other.name)
    {
        aliveCount++;
        std::cout << "  [" << id << "] Copy construct from [" << other.id << "] (alive=" << aliveCount << ")" << std::endl;
    }

    TestObject& operator=(const TestObject& other)
    {
        std::cout << "  [" << id << "] Copy assign from [" << other.id << "]" << std::endl;
        id = other.id;
        name = other.name;
        return *this;
    }

    ~TestObject()
    {
        aliveCount--;
        std::cout << "  [" << id << "] Destruct '" << name << "' (alive=" << aliveCount << ")" << std::endl;
    }
};

int TestObject::aliveCount = 0;

void TestDataArray()
{
    std::cout << "\n=== ValueBuffer Test ===" << std::endl;

    std::cout << "\n1. Create and Resize:" << std::endl;
    ObjectArray<TestObject> arr;
    arr.Resize(3);
    std::cout << "   Resized to 3, aliveCount=" << TestObject::aliveCount << std::endl;

    std::cout << "\n2. WriteAt (should work now):" << std::endl;
    TestObject obj1(1, "First");
    arr.WriteAt(obj1, 0);

    TestObject obj2(2, "Second");
    arr.WriteAt(obj2, 1);

    TestObject obj3(3, "Third");
    arr.WriteAt(obj3, 2);

    std::cout << "\n3. Delete:" << std::endl;
    arr.Delete(1, 1);
    std::cout << "   After delete, count=" << arr.GetCount() << ", aliveCount=" << TestObject::aliveCount << std::endl;

    std::cout << "\n4. Free:" << std::endl;
    arr.Free();
    std::cout << "   After free, aliveCount=" << TestObject::aliveCount << " (should be 0)" << std::endl;
}

void TestArrayList()
{
    std::cout << "\n=== ValueArray Test ===" << std::endl;

    TestObject::aliveCount = 0;

    std::cout << "\n1. Add objects:" << std::endl;
    ObjectArray<TestObject> list;
    list.Add(TestObject(10, "Item10"));
    list.Add(TestObject(20, "Item20"));
    list.Add(TestObject(30, "Item30"));
    std::cout << "   After add, count=" << list.GetCount() << ", aliveCount=" << TestObject::aliveCount << std::endl;

    std::cout << "\n3. Delete:" << std::endl;
    list.Delete(0, 1);
    std::cout << "   After delete, count=" << list.GetCount() << ", aliveCount=" << TestObject::aliveCount << std::endl;

    std::cout << "\n4. Cleanup:" << std::endl;
}

int main()
{
    std::cout << "====================================" << std::endl;
    std::cout << "ValueBuffer/ValueArray Fix Verification" << std::endl;
    std::cout << "====================================" << std::endl;

    try
    {
        TestDataArray();

        std::cout << "\nFinal aliveCount after ValueBuffer test: " << TestObject::aliveCount << std::endl;
        if(TestObject::aliveCount != 0)
        {
            std::cout << "ERROR: Memory leak detected!" << std::endl;
            return 1;
        }

        TestArrayList();

        std::cout << "\nFinal aliveCount after ValueArray test: " << TestObject::aliveCount << std::endl;
        if(TestObject::aliveCount != 0)
        {
            std::cout << "ERROR: Memory leak detected!" << std::endl;
            return 1;
        }

        std::cout << "\n====================================" << std::endl;
        std::cout << "All tests passed! No crashes, no leaks." << std::endl;
        std::cout << "====================================" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\nEXCEPTION: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
