#include<hgl/type/ManagedArray.h>
#include<iostream>
#include<string>
#include<cstring>

using namespace hgl;

// 测试计数器
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "  [PASS]: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  [FAIL]: " << message << std::endl; \
        } \
    } while(0)

// 对象类用于ManagedArray测试
class TestObject
{
    int value;
    std::string label;

public:
    static int instance_count;

    TestObject() : value(0), label("default")
    {
        instance_count++;
        std::cout << "    [Construct] TestObject(" << value << ", \"" << label << "\") - total: " << instance_count << std::endl;
    }

    TestObject(int v, const char* l) : value(v), label(l)
    {
        instance_count++;
        std::cout << "    [Construct] TestObject(" << value << ", \"" << label << "\") - total: " << instance_count << std::endl;
    }

    ~TestObject()
    {
        instance_count--;
        std::cout << "    [Destruct] TestObject(" << value << ", \"" << label << "\") - remaining: " << instance_count << std::endl;
    }

    int GetValue() const { return value; }
    const char* GetLabel() const { return label.c_str(); }

    void SetValue(int v) { value = v; }
};

int TestObject::instance_count = 0;

// ========================================
// ManagedArray Tests
// ========================================

void test_objectlist_basic_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: ManagedArray - Basic Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[1.1] Create ManagedArray:" << std::endl;
    ManagedArray<TestObject> obj_list;
    TEST_ASSERT(obj_list.IsEmpty(), "New ManagedArray is empty");
    TEST_ASSERT(TestObject::instance_count == 0, "No instances created");

    std::cout << "\n[1.2] Add objects (manual new):" << std::endl;
    obj_list.Add(new TestObject(1, "First"));
    obj_list.Add(new TestObject(2, "Second"));
    obj_list.Add(new TestObject(3, "Third"));
    TEST_ASSERT(obj_list.GetCount() == 3, "Count is 3");
    TEST_ASSERT(TestObject::instance_count == 3, "3 instances exist");

    std::cout << "\n[1.3] Access objects:" << std::endl;
    TEST_ASSERT(obj_list[0]->GetValue() == 1, "First object value is 1");
    TEST_ASSERT(strcmp(obj_list[1]->GetLabel(), "Second") == 0, "Second object label correct");

    std::cout << "\n[1.4] Create objects in-place:" << std::endl;
    TestObject* obj = obj_list.Create(4, "Fourth");
    TEST_ASSERT(obj != nullptr, "Create returned valid pointer");
    TEST_ASSERT(obj_list.GetCount() == 4, "Count is 4");
    TEST_ASSERT(TestObject::instance_count == 4, "4 instances exist");
}

void test_objectlist_ownership()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: ManagedArray - Ownership Management" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    std::cout << "[2.1] DeleteAt (delete + remove):" << std::endl;
    {
        ManagedArray<TestObject> obj_list;
        obj_list.Add(new TestObject(1, "A"));
        obj_list.Add(new TestObject(2, "B"));
        obj_list.Add(new TestObject(3, "C"));

        std::cout << "  Before DeleteAt:" << std::endl;
        obj_list.DeleteAt(1);  // Should delete B and remove from list
        std::cout << "  After DeleteAt:" << std::endl;
        TEST_ASSERT(obj_list.GetCount() == 2, "Count is 2");
        TEST_ASSERT(TestObject::instance_count == 2, "1 instance deleted");
    }
    TEST_ASSERT(TestObject::instance_count == 0, "All instances cleaned up");

    std::cout << "\n[2.2] Unlink (remove without delete):" << std::endl;
    TestObject::instance_count = 0;
    {
        ManagedArray<TestObject> obj_list;
        TestObject* obj_to_keep = new TestObject(99, "KeepMe");
        obj_list.Add(obj_to_keep);
        obj_list.Add(new TestObject(2, "B"));

        std::cout << "  Unlinking first object:" << std::endl;
        obj_list.Unlink(0);  // Remove from list but don't delete
        TEST_ASSERT(obj_list.GetCount() == 1, "Count is 1");
        TEST_ASSERT(TestObject::instance_count == 2, "Both instances still exist");

        std::cout << "  Manually deleting unlinked object:" << std::endl;
        delete obj_to_keep;
        TEST_ASSERT(TestObject::instance_count == 1, "Manually deleted");
    }
    TEST_ASSERT(TestObject::instance_count == 0, "Remaining cleaned up");

    std::cout << "\n[2.3] Clear (delete all):" << std::endl;
    TestObject::instance_count = 0;
    {
        ManagedArray<TestObject> obj_list;
        for (int i = 0; i < 5; i++) {
            obj_list.Create(i, "Temp");
        }
        TEST_ASSERT(TestObject::instance_count == 5, "5 instances created");

        std::cout << "  Calling Clear():" << std::endl;
        obj_list.Clear();
        TEST_ASSERT(obj_list.GetCount() == 0, "List empty");
        TEST_ASSERT(TestObject::instance_count == 0, "All instances deleted");
    }
}

void test_objectlist_iterator()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: ManagedArray - Iterator Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    ManagedArray<TestObject> obj_list;
    for (int i = 1; i <= 5; i++) {
        obj_list.Create(i * 10, "Item");
    }

    std::cout << "[3.1] Forward iteration:" << std::endl;
    int sum = 0;
    for (auto it = obj_list.begin(); it != obj_list.end(); ++it) {
        sum += (*it)->GetValue();
    }
    TEST_ASSERT(sum == 150, "Sum via iterator is 150");

    std::cout << "\n[3.2] Range-based for loop:" << std::endl;
    sum = 0;
    for (TestObject* obj : obj_list) {
        sum += obj->GetValue();
    }
    TEST_ASSERT(sum == 150, "Range-for sum is 150");

    std::cout << "\n[3.3] Modify via iterator:" << std::endl;
    for (auto it = obj_list.begin(); it != obj_list.end(); ++it) {
        (*it)->SetValue((*it)->GetValue() + 1);
    }
    TEST_ASSERT(obj_list[0]->GetValue() == 11, "Modified via iterator");
}

void test_objectlist_edge_cases()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: ManagedArray - Edge Cases" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    std::cout << "[4.1] Empty ManagedArray:" << std::endl;
    ManagedArray<TestObject> empty;
    TEST_ASSERT(!empty.DeleteAt(0), "DeleteAt on empty fails");
    TEST_ASSERT(TestObject::instance_count == 0, "No leaks");

    std::cout << "\n[4.2] Single object:" << std::endl;
    empty.Create(42, "Solo");
    TEST_ASSERT(empty.GetCount() == 1, "Count is 1");
    empty.DeleteAt(0);
    TEST_ASSERT(empty.IsEmpty(), "Empty after delete");
    TEST_ASSERT(TestObject::instance_count == 0, "Instance deleted");

    std::cout << "\n[4.3] DeleteRange:" << std::endl;
    for (int i = 0; i < 10; i++) {
        empty.Create(i, "Range");
    }
    TEST_ASSERT(TestObject::instance_count == 10, "10 instances");

    empty.DeleteRange(3, 4);  // Delete indices 3-6
    TEST_ASSERT(empty.GetCount() == 6, "6 remaining");
    TEST_ASSERT(TestObject::instance_count == 6, "4 instances deleted");

    std::cout << "\n[4.4] Free vs Clear:" << std::endl;
    empty.Free();
    TEST_ASSERT(empty.GetCount() == 0, "Free empties list");
    TEST_ASSERT(TestObject::instance_count == 0, "All deleted");
}

void test_objectlist_mixed_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: ManagedArray - Mixed Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    ManagedArray<TestObject> obj_list;

    std::cout << "[5.1] Mix of Add and Create:" << std::endl;
    obj_list.Add(new TestObject(1, "Manual"));
    obj_list.Create(2, "InPlace");
    obj_list.Add(new TestObject(3, "Manual2"));
    TEST_ASSERT(obj_list.GetCount() == 3, "Count is 3");
    TEST_ASSERT(TestObject::instance_count == 3, "3 instances");

    std::cout << "\n[5.2] Mix of Unlink and DeleteAt:" << std::endl;
    TestObject* keep_ref = obj_list[0];
    obj_list.Unlink(0);  // Remove first without delete
    TEST_ASSERT(TestObject::instance_count == 3, "Still 3 instances");

    obj_list.DeleteAt(0);  // Delete second (now at index 0)
    TEST_ASSERT(TestObject::instance_count == 2, "Down to 2 instances");

    std::cout << "\n[5.3] Cleanup:" << std::endl;
    delete keep_ref;
    TEST_ASSERT(TestObject::instance_count == 1, "Manual delete worked");
}

void test_objectlist_stress()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: ManagedArray - Stress Test" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    std::cout << "[6.1] Create 1000 objects:" << std::endl;
    {
        ManagedArray<TestObject> obj_list;
        for (int i = 0; i < 1000; i++) {
            obj_list.Create(i, "Stress");
        }
        TEST_ASSERT(obj_list.GetCount() == 1000, "Count is 1000");
        TEST_ASSERT(TestObject::instance_count == 1000, "1000 instances");

        std::cout << "\n[6.2] Delete every other object:" << std::endl;
        for (int i = 999; i >= 0; i -= 2) {
            obj_list.DeleteAt(i);
        }
        TEST_ASSERT(obj_list.GetCount() == 500, "Count is 500");
        TEST_ASSERT(TestObject::instance_count == 500, "500 instances remain");

        std::cout << "\n[6.3] Scope exit (auto cleanup):" << std::endl;
    }
    TEST_ASSERT(TestObject::instance_count == 0, "All cleaned up");
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "ManagedArray Comprehensive Test Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ManagedArray tests
    test_objectlist_basic_operations();
    test_objectlist_ownership();
    test_objectlist_iterator();
    test_objectlist_edge_cases();
    test_objectlist_mixed_operations();
    test_objectlist_stress();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Tests Passed: " << tests_passed << std::endl;
    std::cout << "Tests Failed: " << tests_failed << std::endl;
    std::cout << "Total Tests: " << (tests_passed + tests_failed) << std::endl;

    if (tests_failed == 0) {
        std::cout << "\nALL TESTS PASSED!" << std::endl;
    } else {
        std::cout << "\nSOME TESTS FAILED" << std::endl;
    }

    return tests_failed > 0 ? 1 : 0;
}
