/**
 * UnorderedManagedSetTest.cpp
 * UnorderedManagedSet 测试 - 测试非平凡类型的对象管理（基于指针地址）
 *
 * 注意: UnorderedManagedSet 内部管理对象指针并负责delete
 */

#include<hgl/type/UnorderedManagedSet.h>
#include<string>
#include<iostream>
#include<iomanip>
#include<cassert>

using namespace hgl;

// ==================== 测试对象类型 ====================

// 供 UnorderedManagedSet 使用的非平凡类型
class ComplexTestObject
{
private:
    int id;
    int value;
    std::string name;   // 非平凡成员

public:
    // 默认构造函数
    ComplexTestObject() : id(0), value(0), name()
    {
        std::cout << "    [Ctor] ComplexTestObject() at " << std::hex << (void*)this
                  << std::dec << " (default)" << std::endl;
    }

    // 带参构造函数
    ComplexTestObject(int id_, int value_, std::string name_)
        : id(id_), value(value_), name(std::move(name_))
    {
        std::cout << "    [Ctor] ComplexTestObject(" << id << ", " << value << ", \"" << name
                  << "\") at " << std::hex << (void*)this << std::dec << std::endl;
    }

    // 拷贝构造函数
    ComplexTestObject(const ComplexTestObject& other)
        : id(other.id), value(other.value), name(other.name)
    {
        std::cout << "    [Copy Ctor] ComplexTestObject at " << std::hex << (void*)this
                  << " from " << (void*)&other << std::dec
                  << " (id=" << id << ")" << std::endl;
    }

    // 移动构造函数
    ComplexTestObject(ComplexTestObject&& other) noexcept
        : id(other.id), value(other.value), name(std::move(other.name))
    {
        std::cout << "    [Move Ctor] ComplexTestObject at " << std::hex << (void*)this
                  << " from " << (void*)&other << std::dec
                  << " (id=" << id << ")" << std::endl;
    }

    // 析构函数
    ~ComplexTestObject()
    {
        std::cout << "    [Dtor] ~ComplexTestObject(" << id << ", " << value << ", \"" << name
                  << "\") at " << std::hex << (void*)this << std::dec << std::endl;
    }

    // 拷贝赋值运算符
    ComplexTestObject& operator=(const ComplexTestObject& other)
    {
        if (this != &other) {
            id = other.id;
            value = other.value;
            name = other.name;
            std::cout << "    [Copy =] ComplexTestObject at " << std::hex << (void*)this
                      << " = " << (void*)&other << std::dec
                      << " (id=" << id << ")" << std::endl;
        }
        return *this;
    }

    // 移动赋值运算符
    ComplexTestObject& operator=(ComplexTestObject&& other) noexcept
    {
        if (this != &other) {
            id = other.id;
            value = other.value;
            name = std::move(other.name);
            std::cout << "    [Move =] ComplexTestObject at " << std::hex << (void*)this
                      << " = " << (void*)&other << std::dec
                      << " (id=" << id << ")" << std::endl;
        }
        return *this;
    }

    int GetId() const { return id; }
    int GetValue() const { return value; }
    const std::string& GetName() const { return name; }

    bool operator==(const ComplexTestObject& other) const
    {
        return id == other.id && value == other.value && name == other.name;
    }
};

// ==================== 测试函数 ====================

void TestBasicOperations()
{
    std::cout << "\n=== Test 1: Basic Add and GetCount ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    std::cout << "  Creating object A..." << std::endl;
    set.Add(new ComplexTestObject(1, 100, "A"));

    std::cout << "  Creating object B..." << std::endl;
    set.Add(new ComplexTestObject(2, 200, "B"));

    std::cout << "  Creating object C..." << std::endl;
    set.Add(new ComplexTestObject(3, 300, "C"));

    assert(set.GetCount() == 3);
    std::cout << "  ✓ Added 3 objects successfully" << std::endl;

    // Verify by enumeration
    int count = 0;
    set.Enum([&count](const ComplexTestObject& obj) {
        count++;
        std::cout << "    - ID: " << obj.GetId() << ", Value: " << obj.GetValue()
                  << ", Name: " << obj.GetName() << std::endl;
    });
    assert(count == 3);
    std::cout << "  ✓ All objects enumerated successfully" << std::endl;

    std::cout << "  Destroying set (should call destructors)..." << std::endl;
}

void TestPointerDeduplication()
{
    std::cout << "\n=== Test 2: Pointer Address Deduplication ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    std::cout << "  Creating ptr1..." << std::endl;
    ComplexTestObject* ptr1 = new ComplexTestObject(10, 1000, "same");

    std::cout << "  Creating ptr2..." << std::endl;
    ComplexTestObject* ptr2 = new ComplexTestObject(10, 1000, "same");

    std::cout << "  ptr1 address: " << std::hex << (void*)ptr1 << std::dec << std::endl;
    std::cout << "  ptr2 address: " << std::hex << (void*)ptr2 << std::dec << std::endl;

    assert(set.Add(ptr1) == true);
    assert(set.GetCount() == 1);
    std::cout << "  ✓ Added ptr1" << std::endl;

    assert(set.Add(ptr2) == true);
    assert(set.GetCount() == 2);
    std::cout << "  ✓ Added ptr2 (different address, both exist)" << std::endl;

    assert(set.Add(ptr1) == false);
    assert(set.GetCount() == 2);
    std::cout << "  ✓ Cannot add ptr1 again (duplicate address)" << std::endl;

    std::cout << "  Destroying set..." << std::endl;
}

void TestDelete()
{
    std::cout << "\n=== Test 3: Delete Operations ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    std::cout << "  Creating object..." << std::endl;
    ComplexTestObject* ptr = new ComplexTestObject(20, 2000, "obj20");
    std::cout << "  Object address: " << std::hex << (void*)ptr << std::dec << std::endl;

    set.Add(ptr);
    assert(set.GetCount() == 1);
    std::cout << "  ✓ Added object" << std::endl;

    std::cout << "  Calling Delete (should destroy object)..." << std::endl;
    bool deleted = set.Delete(ptr);
    assert(deleted == true);
    assert(set.GetCount() == 0);
    std::cout << "  ✓ Delete successful" << std::endl;

    std::cout << "  Destroying set..." << std::endl;
}

void TestUnlink()
{
    std::cout << "\n=== Test 4: Unlink Operations ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    std::cout << "  Creating object..." << std::endl;
    ComplexTestObject* obj = new ComplexTestObject(30, 3000, "obj30");
    std::cout << "  Object address: " << std::hex << (void*)obj << std::dec << std::endl;

    set.Add(obj);
    assert(set.GetCount() == 1);
    std::cout << "  ✓ Added object" << std::endl;

    std::cout << "  Calling Unlink (should NOT destroy object)..." << std::endl;
    bool unlinked = set.Unlink(obj);
    assert(unlinked == true);
    assert(set.GetCount() == 0);
    std::cout << "  ✓ Unlink successful" << std::endl;

    std::cout << "  Manually deleting unlinked object..." << std::endl;
    delete obj;

    std::cout << "  Destroying set..." << std::endl;
}

void TestGetOperations()
{
    std::cout << "\n=== Test 5: Get Operations ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    set.Add(new ComplexTestObject(111, 1111, "A"));
    set.Add(new ComplexTestObject(222, 2222, "B"));
    set.Add(new ComplexTestObject(333, 3333, "C"));

    const std::vector<int> &ids = set.GetActiveView();
    assert((int)ids.size() == 3);

    const ComplexTestObject* first = set.At(ids[0]);
    assert(first);
    std::cout << "  ✓ First: ID=" << first->GetId() << ", Value=" << first->GetValue()
              << ", Name=" << first->GetName() << " at " << std::hex << (void*)first << std::dec << std::endl;

    const ComplexTestObject* last = set.At(ids[(int)ids.size() - 1]);
    assert(last);
    std::cout << "  ✓ Last: ID=" << last->GetId() << ", Value=" << last->GetValue()
              << ", Name=" << last->GetName() << " at " << std::hex << (void*)last << std::dec << std::endl;

    std::cout << "  Destroying set..." << std::endl;
}

void TestClear()
{
    std::cout << "\n=== Test 6: Clear Operation ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    for(int i = 1; i <= 3; i++) {
        std::cout << "  Creating object " << i << "..." << std::endl;
        set.Add(new ComplexTestObject(i, i * 100, "obj" + std::to_string(i)));
    }

    assert(set.GetCount() == 3);
    std::cout << "  ✓ Added 3 objects" << std::endl;

    std::cout << "  Calling Free (should destroy all objects)..." << std::endl;
    set.Free();
    assert(set.GetCount() == 0);
    std::cout << "  ✓ Free successful" << std::endl;

    std::cout << "  Destroying set..." << std::endl;
}

void TestIsEmpty()
{
    std::cout << "\n=== Test 7: IsEmpty Check ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;
    assert(set.IsEmpty());
    std::cout << "  ✓ New set is empty" << std::endl;

    set.Add(new ComplexTestObject(1, 100, "x"));
    assert(!set.IsEmpty());
    std::cout << "  ✓ Set is not empty after Add" << std::endl;

    std::cout << "  Calling Free..." << std::endl;
    set.Free();
    assert(set.IsEmpty());
    std::cout << "  ✓ Set is empty after Free" << std::endl;

    std::cout << "  Destroying set..." << std::endl;
}

void TestContains()
{
    std::cout << "\n=== Test 8: Contains Check (by pointer address) ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    std::cout << "  Creating ptr1..." << std::endl;
    ComplexTestObject* ptr1 = new ComplexTestObject(40, 4000, "test");
    std::cout << "  ptr1 address: " << std::hex << (void*)ptr1 << std::dec << std::endl;

    std::cout << "  Creating ptr2..." << std::endl;
    ComplexTestObject* ptr2 = new ComplexTestObject(40, 4000, "test");
    std::cout << "  ptr2 address: " << std::hex << (void*)ptr2 << std::dec << std::endl;

    set.Add(ptr1);

    assert(set.Contains(ptr1) == true);
    std::cout << "  ✓ Contains ptr1" << std::endl;

    assert(set.Contains(ptr2) == false);
    std::cout << "  ✓ Does not contain ptr2 (different address)" << std::endl;

    std::cout << "  Manually deleting ptr2..." << std::endl;
    delete ptr2;

    std::cout << "  Destroying set..." << std::endl;
}

void TestMultipleOperations()
{
    std::cout << "\n=== Test 9: Multiple Mixed Operations ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    ComplexTestObject* keep_ptr = nullptr;
    ComplexTestObject* delete_ptr = nullptr;
    for(int i = 1; i <= 5; i++) {
        std::cout << "  Creating object " << i << "..." << std::endl;
        ComplexTestObject* p = new ComplexTestObject(i, i * 10, "obj" + std::to_string(i));
        if(i == 5) keep_ptr = p;
        if(i == 3) delete_ptr = p;
        set.Add(p);
    }
    assert(set.GetCount() == 5);
    std::cout << "  ✓ Added 5 objects" << std::endl;

    std::cout << "  Deleting object 3..." << std::endl;
    bool deleted = set.Delete(delete_ptr);
    assert(deleted == true);
    assert(set.GetCount() == 4);
    std::cout << "  ✓ Deleted 1 object, count now 4" << std::endl;

    std::cout << "  Unlinking object 5..." << std::endl;
    bool unlinked = set.Unlink(keep_ptr);
    assert(unlinked == true);
    assert(set.GetCount() == 3);
    std::cout << "  ✓ Unlinked 1 object, count now 3" << std::endl;

    int count = 0;
    set.Enum([&count](const ComplexTestObject&) { count++; });
    assert(count == 3);
    std::cout << "  ✓ Verified remaining 3 objects via Enum" << std::endl;

    std::cout << "  Manually deleting unlinked object..." << std::endl;
    delete keep_ptr;

    std::cout << "  Destroying set..." << std::endl;
}

int main()
{
    std::cout << "=== UnorderedManagedSet Test Suite (Pointer Address-Based) ===" << std::endl;
    std::cout << "=== With Object Lifetime Tracking ===" << std::endl;

    try {
        TestBasicOperations();
        TestPointerDeduplication();
        TestDelete();
        TestUnlink();
        TestGetOperations();
        TestClear();
        TestIsEmpty();
        TestContains();
        TestMultipleOperations();

        std::cout << "\n=== All tests completed successfully ✓ ===" << std::endl;
        return 0;
    }
    catch(const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch(...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
