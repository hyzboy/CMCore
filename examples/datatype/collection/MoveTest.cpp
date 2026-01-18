#include<hgl/type/DataArray.h>
#include<iostream>
#include<string>

using namespace hgl;

class TestObj
{
public:
    int id;
    std::string name;

    TestObj() : id(0), name("")
    {
        std::cout << "  TestObj() default constructor, id=" << id << std::endl;
    }

    TestObj(int i, const std::string& n) : id(i), name(n)
    {
        std::cout << "  TestObj(" << id << ", " << name << ") constructor" << std::endl;
    }

    TestObj(const TestObj& other) : id(other.id), name(other.name)
    {
        std::cout << "  TestObj copy constructor, id=" << id << std::endl;
    }

    TestObj(TestObj&& other) noexcept : id(other.id), name(std::move(other.name))
    {
        std::cout << "  TestObj move constructor, id=" << id << std::endl;
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
    std::cout << "=== DataArray Move Test ===" << std::endl;

    std::cout << "\n[1] Creating and filling array..." << std::endl;
    DataArray<TestObj> arr;
    arr.Resize(5);

    for(int i = 0; i < 5; i++)
    {
        std::cout << "Setting arr[" << i << "]..." << std::endl;
        arr[i] = TestObj(i, "Item" + std::to_string(i));
    }

    std::cout << "\n[2] Current array:" << std::endl;
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << "  arr[" << i << "] = {" << arr[i].id << ", " << arr[i].name << "}" << std::endl;

    std::cout << "\n[3] Calling Move(3, 1, 2)..." << std::endl;
    std::cout << "    (Moving elements [1,2] from position 1 to position 3)" << std::endl;

    bool result = arr.Move(3, 1, 2);

    std::cout << "    Move result: " << (result ? "SUCCESS" : "FAILED") << std::endl;

    std::cout << "\n[4] Array after Move:" << std::endl;
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << "  arr[" << i << "] = {" << arr[i].id << ", " << arr[i].name << "}" << std::endl;

    std::cout << "\n[5] Cleanup..." << std::endl;

    return 0;
}
