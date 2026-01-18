#include<hgl/type/DataArray.h>
#include<iostream>
#include<string>
#include<chrono>
#include<thread>

using namespace hgl;

class SimpleObj
{
public:
    int id;
    std::string name;

    static int constructCount;
    static int destructCount;
    static int moveCount;

    SimpleObj() : id(0), name("")
    {
        constructCount++;
        std::cout << "    [Construct #" << constructCount << "] default" << std::endl;
        std::cout << std::flush;
    }

    SimpleObj(int i, const std::string& n) : id(i), name(n)
    {
        constructCount++;
        std::cout << "    [Construct #" << constructCount << "] id=" << id << std::endl;
        std::cout << std::flush;
    }

    SimpleObj(const SimpleObj& other) : id(other.id), name(other.name)
    {
        constructCount++;
        std::cout << "    [Copy construct #" << constructCount << "] id=" << id << std::endl;
        std::cout << std::flush;
    }

    SimpleObj(SimpleObj&& other) noexcept : id(other.id), name(std::move(other.name))
    {
        moveCount++;
        std::cout << "    [Move construct #" << moveCount << "] id=" << id << std::endl;
        std::cout << std::flush;
    }

    SimpleObj& operator=(const SimpleObj& other)
    {
        id = other.id;
        name = other.name;
        return *this;
    }

    ~SimpleObj()
    {
        destructCount++;
        if(destructCount % 10 == 0)
        {
            std::cout << "    [Destruct #" << destructCount << "]" << std::endl;
            std::cout << std::flush;
        }
    }
};

int SimpleObj::constructCount = 0;
int SimpleObj::destructCount = 0;
int SimpleObj::moveCount = 0;

int main()
{
    std::cout << "=== TEST Move(7,3,4) ===" << std::endl;
    std::cout << std::flush;

    std::cout << "\n[1] Creating array..." << std::endl;
    std::cout << std::flush;

    DataArray<SimpleObj> arr;

    std::cout << "[2] Resizing to 10..." << std::endl;
    std::cout << std::flush;

    arr.Resize(10);

    std::cout << "[3] Filling array..." << std::endl;
    std::cout << std::flush;

    for(int i = 0; i < 10; i++)
    {
        std::cout << "  Setting [" << i << "]..." << std::endl;
        std::cout << std::flush;
        arr[i] = SimpleObj(i, "Item" + std::to_string(i));
    }

    std::cout << "\n[4] Before Move:" << std::endl;
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << "  [" << i << "]: {" << arr[i].id << ", " << arr[i].name << "}" << std::endl;

    std::cout << "\n[5] Calling Move(7, 3, 4)..." << std::endl;
    std::cout << "(Move elements [3,4,5,6] from position 3 to position 7)" << std::endl;
    std::cout << "About to call arr.Move()..." << std::endl;
    std::cout << std::flush;  // 强制刷新输出

    bool result = arr.Move(7, 3, 4);

    std::cout << "\n[6] Move returned: " << (result ? "true" : "false") << std::endl;
    std::cout << std::flush;

    std::cout << "\n[7] After Move:" << std::endl;
    for(int i = 0; i < arr.GetCount(); i++)
        std::cout << "  [" << i << "]: {" << arr[i].id << ", " << arr[i].name << "}" << std::endl;

    std::cout << "\n[8] Statistics:" << std::endl;
    std::cout << "  Constructs: " << SimpleObj::constructCount << std::endl;
    std::cout << "  Destructs: " << SimpleObj::destructCount << std::endl;
    std::cout << "  Moves: " << SimpleObj::moveCount << std::endl;

    std::cout << "\n[9] Exiting..." << std::endl;
    return 0;
}
