/**
 * UnorderedManagedSetTest.cpp
 * UnorderedManagedSet 测试 - 测试非平凡类型的对象管理
 * 
 * 注意: UnorderedManagedSet 内部管理对象指针并负责delete
 */

#include<hgl/type/UnorderedValueSet.h>
#include<hgl/type/UnorderedManagedSet.h>
#include<string>
#include<iostream>
#include<cassert>

using namespace hgl;

// ==================== 测试对象类型 ====================

// 供 UnorderedValueSet 使用的平凡可复制类型
struct TrivialTestObject
{
    int id = 0;
    int value = 0;

    bool operator==(const TrivialTestObject& other) const
    {
        return id == other.id && value == other.value;
    }
};

// 供 UnorderedManagedSet 使用的非平凡类型
class ComplexTestObject
{
private:
    int id;
    int value;
    std::string name;   // 非平凡成员，确保使用 UnorderedManagedSet

public:
    ComplexTestObject() : id(0), value(0), name() {}
    ComplexTestObject(int id_, int value_, std::string name_)
        : id(id_), value(value_), name(std::move(name_)) {}

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

    UnorderedValueSet<TrivialTestObject> set;

    set.Add({1, 100});
    set.Add({2, 200});
    set.Add({3, 300});

    assert(set.GetCount() == 3);
    std::cout << "  ✓ Added 3 objects successfully" << std::endl;

    // Verify by enumeration
    int count = 0;
    set.Enum([&count](const TrivialTestObject& obj) {
        count++;
        std::cout << "    - ID: " << obj.id << ", Value: " << obj.value << std::endl;
    });
    assert(count == 3);
    std::cout << "  ✓ All objects enumerated successfully" << std::endl;
}

void TestDelete()
{
    std::cout << "\n=== Test 2: Delete Operations ===" << std::endl;

    UnorderedValueSet<TrivialTestObject> set;

    set.Add({10, 1000});
    assert(set.GetCount() == 1);
    std::cout << "  ✓ Added object" << std::endl;

    bool deleted = set.Delete({10, 1000});
    assert(deleted == true);
    assert(set.GetCount() == 0);
    std::cout << "  ✓ Delete successful" << std::endl;
}

void TestUnlink()
{
    std::cout << "\n=== Test 3: Unlink Operations ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;
    ComplexTestObject* obj = new ComplexTestObject(20, 2000, "obj20");

    set.Add(obj);
    assert(set.GetCount() == 1);
    std::cout << "  ✓ Added object" << std::endl;

    bool unlinked = set.Unlink(obj);
    assert(unlinked == true);
    assert(set.GetCount() == 0);
    std::cout << "  ✓ Unlink successful" << std::endl;
    
    // 手动删除（因为 Unlink 不会删除对象）
    delete obj;
}

void TestGetOperations()
{
    std::cout << "\n=== Test 4: Get Operations ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    set.Add(new ComplexTestObject(111, 1111, "A"));
    set.Add(new ComplexTestObject(222, 2222, "B"));
    set.Add(new ComplexTestObject(333, 3333, "C"));

    ComplexTestObject first;
    bool got_first = set.GetFirst(first);
    assert(got_first == true);
    std::cout << "  ✓ GetFirst: ID=" << first.GetId() << ", Value=" << first.GetValue() << ", Name=" << first.GetName() << std::endl;

    ComplexTestObject last;
    bool got_last = set.GetLast(last);
    assert(got_last == true);
    std::cout << "  ✓ GetLast: ID=" << last.GetId() << ", Value=" << last.GetValue() << ", Name=" << last.GetName() << std::endl;

    ComplexTestObject by_index;
    bool got = set.Get(0, by_index);
    assert(got == true);
    std::cout << "  ✓ Get(0): ID=" << by_index.GetId() << ", Value=" << by_index.GetValue() << ", Name=" << by_index.GetName() << std::endl;
}

void TestClear()
{
    std::cout << "\n=== Test 5: Clear Operation ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    for(int i = 1; i <= 3; i++) {
        set.Add(new ComplexTestObject(i, i * 100, "obj" + std::to_string(i)));
    }

    assert(set.GetCount() == 3);
    std::cout << "  ✓ Added 3 objects" << std::endl;

    set.Clear();
    assert(set.GetCount() == 0);
    std::cout << "  ✓ Clear successful" << std::endl;
}

void TestIsEmpty()
{
    std::cout << "\n=== Test 6: IsEmpty Check ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;
    assert(set.IsEmpty());
    std::cout << "  ✓ New set is empty" << std::endl;

    set.Add(new ComplexTestObject(1, 100, "x"));
    assert(!set.IsEmpty());
    std::cout << "  ✓ Set is not empty after Add" << std::endl;

    set.Clear();
    assert(set.IsEmpty());
    std::cout << "  ✓ Set is empty after Clear" << std::endl;
}

void TestMultipleOperations()
{
    std::cout << "\n=== Test 7: Multiple Mixed Operations ===" << std::endl;

    UnorderedManagedSet<ComplexTestObject> set;

    ComplexTestObject* keep_ptr = nullptr;
    ComplexTestObject* delete_ptr = nullptr;
    for(int i = 1; i <= 5; i++) {
        ComplexTestObject* p = new ComplexTestObject(i, i * 10, "obj" + std::to_string(i));
        if(i == 5) keep_ptr = p;
        if(i == 3) delete_ptr = p;
        set.Add(p);
    }
    assert(set.GetCount() == 5);
    std::cout << "  ✓ Added 5 objects" << std::endl;

    bool deleted = set.Delete(delete_ptr);
    assert(deleted == true);
    assert(set.GetCount() == 4);
    std::cout << "  ✓ Deleted 1 object, count now 4" << std::endl;

    bool unlinked = set.Unlink(keep_ptr);
    assert(unlinked == true);
    assert(set.GetCount() == 3);
    std::cout << "  ✓ Unlinked 1 object, count now 3 (manual cleanup needed)" << std::endl;

    int count = 0;
    set.Enum([&count](const ComplexTestObject&) { count++; });
    assert(count == 3);
    std::cout << "  ✓ Verified remaining 3 objects via Enum" << std::endl;

    // 手动删除已解绑的对象
    delete keep_ptr;
}

int main()
{
    std::cout << "=== UnorderedManagedSet Test Suite ===" << std::endl;
    std::cout << "Using simplified objects to avoid complex destructor issues" << std::endl;

    try {
        TestBasicOperations();
        TestDelete();
        TestUnlink();
        TestGetOperations();
        TestClear();
        TestIsEmpty();
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
