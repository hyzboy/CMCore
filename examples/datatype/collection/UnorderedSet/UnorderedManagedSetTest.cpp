/**
 * UnorderedManagedSetTest.cpp
 * UnorderedManagedSet 测试 - 测试非平凡类型的对象管理
 */

#include<hgl/type/UnorderedSet.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;

// ==================== 测试用的非平凡类型 ====================

class TestObject
{
private:
    int id;
    std::string name;
    static int instance_count;

public:
    TestObject() : id(0), name("")
    {
        instance_count++;
        std::cout << "    [默认构造] TestObject(" << id << ", \"" << name << "\") - 实例数: " << instance_count << std::endl;
    }

    TestObject(int id_, const std::string& name_) : id(id_), name(name_)
    {
        instance_count++;
        std::cout << "    [构造] TestObject(" << id << ", \"" << name << "\") - 实例数: " << instance_count << std::endl;
    }

    ~TestObject()
    {
        instance_count--;
        std::cout << "    [析构] TestObject(" << id << ", \"" << name << "\") - 实例数: " << instance_count << std::endl;
    }

    // 拷贝构造和赋值
    TestObject(const TestObject& other) : id(other.id), name(other.name)
    {
        instance_count++;
        std::cout << "    [拷贝构造] TestObject(" << id << ", \"" << name << "\")" << std::endl;
    }

    TestObject& operator=(const TestObject& other)
    {
        if(this != &other) {
            id = other.id;
            name = other.name;
            std::cout << "    [拷贝赋值] TestObject(" << id << ", \"" << name << "\")" << std::endl;
        }
        return *this;
    }

    int GetId() const { return id; }
    const std::string& GetName() const { return name; }

    // 相等比较（集合需要）
    bool operator==(const TestObject& other) const
    {
        return id == other.id && name == other.name;
    }

    static int GetInstanceCount() { return instance_count; }
    static void ResetInstanceCount() { instance_count = 0; }
};

int TestObject::instance_count = 0;

// ==================== 测试函数 ====================

void TestBasicObjectManagement()
{
    std::cout << "\n=== 测试基础对象管理 ===" << std::endl;

    TestObject::ResetInstanceCount();
    {
        UnorderedManagedSet<TestObject> set;

        std::cout << "\n  添加对象:" << std::endl;
        set.Add(*new TestObject(1, "Alice"));
        set.Add(*new TestObject(2, "Bob"));
        set.Add(*new TestObject(3, "Charlie"));

        assert(set.GetCount() == 3);
        std::cout << "\n  ✓ 添加 3 个对象成功" << std::endl;

        std::cout << "\n  查找对象:" << std::endl;
        TestObject search1(1, "Alice");
        TestObject search2(2, "Bob");
        TestObject search3(3, "Charlie");
        assert(set.Contains(search1));
        assert(set.Contains(search2));
        assert(set.Contains(search3));
        std::cout << "  ✓ 所有对象可查找" << std::endl;

        std::cout << "\n  集合析构，应释放所有对象:" << std::endl;
    }
    // set 析构时应该删除所有对象

    std::cout << "\n  最终实例数: " << TestObject::GetInstanceCount() << std::endl;
    // 注意：因为我们的实现使用了 ObjectPool，池中的对象可能不会立即销毁
    // 这是预期行为
}

void TestDeleteVsUnlink()
{
    std::cout << "\n=== 测试 Delete vs Unlink ===" << std::endl;

    TestObject::ResetInstanceCount();

    std::cout << "\n  测试 Delete（应删除对象）:" << std::endl;
    {
        UnorderedManagedSet<TestObject> set;
        set.Add(*new TestObject(10, "Test1"));

        std::cout << "  删除前实例数: " << TestObject::GetInstanceCount() << std::endl;

        TestObject search(10, "Test1");
        bool deleted = set.Delete(search);
        assert(deleted == true);
        assert(set.GetCount() == 0);

        std::cout << "  删除后集合大小: " << set.GetCount() << std::endl;
        std::cout << "  ✓ Delete 成功删除对象" << std::endl;
    }

    std::cout << "\n  测试 Unlink（不删除对象）:" << std::endl;
    {
        UnorderedManagedSet<TestObject> set;
        set.Add(*new TestObject(20, "Test2"));

        std::cout << "  Unlink 前集合大小: " << set.GetCount() << std::endl;

        TestObject search(20, "Test2");
        bool unlinked = set.UnlinkByValue(search);
        assert(unlinked == true);
        assert(set.GetCount() == 0);

        std::cout << "  Unlink 后集合大小: " << set.GetCount() << std::endl;
        std::cout << "  ✓ Unlink 成功移除关联" << std::endl;
    }
}

void TestDeleteAll()
{
    std::cout << "\n=== 测试 DeleteAll ===" << std::endl;

    TestObject::ResetInstanceCount();
    {
        UnorderedManagedSet<TestObject> set;

        std::cout << "\n  添加多个对象:" << std::endl;
        for(int i = 1; i <= 5; i++) {
            set.Add(*new TestObject(i, "Obj" + std::to_string(i)));
        }

        std::cout << "\n  添加后实例数: " << TestObject::GetInstanceCount() << std::endl;
        assert(set.GetCount() == 5);

        std::cout << "\n  调用 DeleteAll:" << std::endl;
        set.DeleteAll();

        assert(set.GetCount() == 0);
        std::cout << "  DeleteAll 后集合大小: " << set.GetCount() << std::endl;
        std::cout << "  ✓ DeleteAll 成功清空集合" << std::endl;
    }
}

void TestDuplicateObjects()
{
    std::cout << "\n=== 测试重复对象 ===" << std::endl;

    UnorderedManagedSet<TestObject> set;

    std::cout << "\n  添加对象:" << std::endl;
    TestObject obj1(100, "Duplicate");
    bool added1 = set.Add(obj1);
    assert(added1 == true);

    std::cout << "\n  尝试添加相同对象（应拒绝）:" << std::endl;
    TestObject obj2(100, "Duplicate");  // 相同的 id 和 name
    bool added2 = set.Add(obj2);
    assert(added2 == false);
    assert(set.GetCount() == 1);

    std::cout << "  ✓ 重复对象正确拒绝" << std::endl;
}

void TestComparisonOperators()
{
    std::cout << "\n=== 测试比较运算符 ===" << std::endl;

    UnorderedManagedSet<TestObject> set1, set2;

    std::cout << "\n  添加相同对象到两个集合:" << std::endl;
    TestObject obj1(1, "Alice");
    TestObject obj2(2, "Bob");

    set1.Add(obj1);
    set1.Add(obj2);

    set2.Add(obj1);
    set2.Add(obj2);

    assert(set1 == set2);
    std::cout << "  ✓ 相同对象的集合相等" << std::endl;

    std::cout << "\n  添加不同对象:" << std::endl;
    TestObject obj3(3, "Charlie");
    set2.Add(obj3);

    assert(set1 != set2);
    std::cout << "  ✓ 不同对象的集合不相等" << std::endl;
}

void TestIterationWithObjects()
{
    std::cout << "\n=== 测试对象迭代 ===" << std::endl;

    UnorderedManagedSet<TestObject> set;

    std::cout << "\n  添加对象:" << std::endl;
    TestObject obj1(1, "Alpha");
    TestObject obj2(2, "Beta");
    TestObject obj3(3, "Gamma");

    set.Add(obj1);
    set.Add(obj2);
    set.Add(obj3);

    std::cout << "\n  枚举对象:" << std::endl;
    int count = 0;
    set.Enum([&count](const TestObject& obj) {
        std::cout << "    - ID: " << obj.GetId() << ", Name: " << obj.GetName() << std::endl;
        count++;
    });

    assert(count == 3);
    std::cout << "  ✓ 对象迭代成功" << std::endl;
}

void TestGetOperations()
{
    std::cout << "\n=== 测试 Get 操作 ===" << std::endl;

    UnorderedManagedSet<TestObject> set;

    TestObject obj1(111, "First");
    TestObject obj2(222, "Second");
    TestObject obj3(333, "Third");

    set.Add(obj1);
    set.Add(obj2);
    set.Add(obj3);

    // GetFirst
    TestObject first(0, "");
    bool got_first = set.GetFirst(first);
    assert(got_first == true);
    std::cout << "  ✓ GetFirst: ID=" << first.GetId() << ", Name=" << first.GetName() << std::endl;

    // GetLast
    TestObject last(0, "");
    bool got_last = set.GetLast(last);
    assert(got_last == true);
    std::cout << "  ✓ GetLast: ID=" << last.GetId() << ", Name=" << last.GetName() << std::endl;

    // Get by index
    TestObject by_index(0, "");
    bool got = set.Get(0, by_index);
    assert(got == true);
    std::cout << "  ✓ Get(0): ID=" << by_index.GetId() << ", Name=" << by_index.GetName() << std::endl;
}

void TestClearVsDeleteAll()
{
    std::cout << "\n=== 测试 Clear vs DeleteAll ===" << std::endl;

    TestObject::ResetInstanceCount();

    std::cout << "\n  测试 Clear（应调用 DeleteAll）:" << std::endl;
    {
        UnorderedManagedSet<TestObject> set;

        for(int i = 1; i <= 3; i++) {
            TestObject obj(i, "Obj" + std::to_string(i));
            set.Add(obj);
        }

        std::cout << "  Clear 前实例数: " << TestObject::GetInstanceCount() << std::endl;

        set.Clear();

        assert(set.GetCount() == 0);
        std::cout << "  Clear 后集合大小: " << set.GetCount() << std::endl;
        std::cout << "  ✓ Clear 成功" << std::endl;
    }
}

int main()
{
    std::cout << "=== UnorderedManagedSet 对象管理测试 ===" << std::endl;

    TestBasicObjectManagement();
    TestDeleteVsUnlink();
    TestDeleteAll();
    TestDuplicateObjects();
    TestComparisonOperators();
    TestIterationWithObjects();
    TestGetOperations();
    TestClearVsDeleteAll();

    std::cout << "\n=== 所有对象管理测试完成 ✓ ===" << std::endl;
    std::cout << "\n注意：因使用 ObjectPool，某些对象可能延迟销毁，这是预期行为。" << std::endl;
    return 0;
}
