#include<hgl/type/OrderedManagedSet.h>
#include<iostream>
#include<string>

using namespace hgl;

struct SimpleData
{
    int id;
    std::string name;

    SimpleData() : id(0)
    {
        std::cout << "    Default construct" << std::endl;
    }

    SimpleData(int i, const char* n) : id(i), name(n)
    {
        std::cout << "    Construct id=" << id << " name=" << name << std::endl;
    }

    SimpleData(const SimpleData& other) : id(other.id), name(other.name)
    {
        std::cout << "    Copy construct from id=" << other.id << std::endl;
    }

    SimpleData(SimpleData&& other) noexcept : id(other.id), name(std::move(other.name))
    {
        std::cout << "    Move construct from id=" << other.id << std::endl;
    }

    SimpleData& operator=(const SimpleData& other)
    {
        if (this != &other) {
            id = other.id;
            name = other.name;
            std::cout << "    Copy assign id=" << id << std::endl;
        }
        return *this;
    }

    ~SimpleData()
    {
        std::cout << "    Destruct id=" << id << std::endl;
    }

    bool operator<(const SimpleData& other) const { return id < other.id; }
    bool operator>(const SimpleData& other) const { return id > other.id; }
    bool operator==(const SimpleData& other) const { return id == other.id; }
};

int main()
{
    std::cout << "=== Simple OrderedValueSet Test ===" << std::endl;

    {
        std::cout << "\n[1] Creating OrderedValueSet..." << std::endl;
        OrderedManagedSet<SimpleData> set;

        std::cout << "\n[2] Creating first object..." << std::endl;
        SimpleData obj1(10, "First");

        std::cout << "\n[3] Adding to set (count=" << set.GetCount() << ")..." << std::endl;
        int64 pos = set.Add(obj1);
        std::cout << "    Added at position: " << pos << std::endl;
        std::cout << "    Set count: " << set.GetCount() << std::endl;

        std::cout << "\n[4] Checking object in set..." << std::endl;
        SimpleData retrieved;
        if(set.Get(0, retrieved)) {
            std::cout << "    Retrieved: id=" << retrieved.id << " name=" << retrieved.name << std::endl;
        }

        std::cout << "\n[5] Creating second object..." << std::endl;
        SimpleData obj2(5, "Second");

        std::cout << "\n[6] Adding to set (count=" << set.GetCount() << ")..." << std::endl;
        pos = set.Add(obj2);
        std::cout << "    Added at position: " << pos << std::endl;
        std::cout << "    Set count: " << set.GetCount() << std::endl;

        std::cout << "\n[7] Creating third object..." << std::endl;
        SimpleData obj3(15, "Third");

        std::cout << "\n[8] Adding to set (count=" << set.GetCount() << ")..." << std::endl;
        pos = set.Add(obj3);
        std::cout << "    Added at position: " << pos << std::endl;
        std::cout << "    Set count: " << set.GetCount() << std::endl;

        std::cout << "\n[9] Cleanup..." << std::endl;
    }

    std::cout << "\n[10] Done!" << std::endl;
    return 0;
}
