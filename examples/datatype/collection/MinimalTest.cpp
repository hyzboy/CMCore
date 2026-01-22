#include<hgl/type/ObjectArray.h>
#include<iostream>
#include<string>

using namespace hgl;

class TestObj
{
public:
    int id;
    std::string name;

    TestObj() : id(0), name("default")
    {
        std::cout << "  TestObj() default constructor, id=" << id << std::endl;
    }

    TestObj(int i, const std::string& n) : id(i), name(n)
    {
        std::cout << "  TestObj(" << i << ", " << n << ") constructor" << std::endl;
    }

    TestObj(const TestObj& other) : id(other.id), name(other.name)
    {
        std::cout << "  TestObj copy constructor, id=" << id << std::endl;
    }

    TestObj& operator=(const TestObj& other)
    {
        std::cout << "  TestObj operator=, from id=" << other.id << " to id=" << id << std::endl;
        id = other.id;
        name = other.name;
        return *this;
    }

    ~TestObj()
    {
        std::cout << "  ~TestObj(), id=" << id << ", name=" << name << std::endl;
    }
};

int main()
{
    std::cout << "=== Minimal DataArray Test ===" << std::endl;

    std::cout << "\n[1] Creating DataArray..." << std::endl;
    ObjectArray<TestObj> arr;

    std::cout << "\n[2] Resizing to 3..." << std::endl;
    arr.Resize(3);
    std::cout << "  After Resize: count=" << arr.GetCount() << ", alloc=" << arr.GetAllocCount() << std::endl;

    std::cout << "\n[3] Setting arr[0]..." << std::endl;
    arr[0] = TestObj(1, "First");

    std::cout << "\n[4] Setting arr[1]..." << std::endl;
    arr[1] = TestObj(2, "Second");

    std::cout << "\n[5] Setting arr[2]..." << std::endl;
    arr[2] = TestObj(3, "Third");

    std::cout << "\n[6] Reading values..." << std::endl;

    for(int i = 0; i < arr.GetCount(); i++)
    {
        std::cout << "  arr[" << i << "] = {" << arr[i].id << ", " << arr[i].name << "}" << std::endl;
    }

    std::cout << "\n[7] Cleanup (scope exit)..." << std::endl;

    return 0;
}
