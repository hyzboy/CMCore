#include<hgl/type/ValueArray.h>
#include<hgl/type/ObjectList.h>
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

// List 类型别名（代码库中常用）
template<typename T>
using List = ValueArray<T>;

// 非平凡类型用于测试
struct ComplexItem
{
    int id;
    std::string name;
    int* data;

    static int construct_count;
    static int destruct_count;

    ComplexItem() : id(0), data(nullptr)
    {
        construct_count++;
    }

    ComplexItem(int i, const char* n) : id(i), name(n), data(new int(i * 10))
    {
        construct_count++;
    }

    ComplexItem(const ComplexItem& other) : id(other.id), name(other.name)
    {
        construct_count++;
        data = other.data ? new int(*other.data) : nullptr;
    }

    ComplexItem(ComplexItem&& other) noexcept : id(other.id), name(std::move(other.name)), data(other.data)
    {
        construct_count++;
        other.data = nullptr;
    }

    ComplexItem& operator=(const ComplexItem& other)
    {
        if (this != &other) {
            id = other.id;
            name = other.name;
            delete data;
            data = other.data ? new int(*other.data) : nullptr;
        }
        return *this;
    }

    bool operator==(const ComplexItem& other) const
    {
        return id == other.id && name == other.name;
    }

    ~ComplexItem()
    {
        destruct_count++;
        delete data;
    }
};

int ComplexItem::construct_count = 0;
int ComplexItem::destruct_count = 0;

// 对象类用于ObjectList测试
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

// 辅助函数：打印List
template<typename T>
void PrintList(const char* label, const List<T>& list)
{
    std::cout << "    " << label << " [count=" << list.GetCount() << "]: ";

    if (list.IsEmpty()) {
        std::cout << "(empty)";
    } else {
        for (int i = 0; i < list.GetCount() && i < 10; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << *list.At(i);  // ValueArray::At() returns T*
        }
        if (list.GetCount() > 10) std::cout << " ... (" << (list.GetCount() - 10) << " more)";
    }
    std::cout << std::endl;
}

// ========================================
// ValueArray/List Tests
// ========================================

void test_list_basic_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: List - Basic Operations (int)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[1.1] Create empty list:" << std::endl;
    List<int> list;
    TEST_ASSERT(list.IsEmpty(), "New list is empty");
    TEST_ASSERT(list.GetCount() == 0, "Count is 0");

    std::cout << "\n[1.2] Add elements:" << std::endl;
    list.Add(10);
    list.Add(20);
    list.Add(30);
    TEST_ASSERT(list.GetCount() == 3, "Count is 3");
    TEST_ASSERT(*list.At(0) == 10, "First element is 10");
    TEST_ASSERT(*list.At(1) == 20, "Second element is 20");
    TEST_ASSERT(*list.At(2) == 30, "Third element is 30");
    PrintList("After add", list);

    std::cout << "\n[1.3] Insert elements:" << std::endl;
    list.Insert(0, 5);  // Insert at beginning
    TEST_ASSERT(*list.At(0) == 5, "Inserted at beginning");
    list.Insert(2, 15); // Insert in middle
    TEST_ASSERT(*list.At(2) == 15, "Inserted in middle");
    PrintList("After insert", list);

    std::cout << "\n[1.4] Access elements:" << std::endl;
    TEST_ASSERT(*list.At(0) == 5, "At(0) returns 5");
    TEST_ASSERT(*list.At(list.GetCount()-1) == 30, "At(last) returns 30");

    std::cout << "\n[1.5] Find elements:" << std::endl;
    TEST_ASSERT(list.Find(20) != -1, "Find(20) returns valid index");
    TEST_ASSERT(list.Find(999) == -1, "Find(999) returns -1");
    TEST_ASSERT(list.Contains(20), "Contains(20) returns true");
    TEST_ASSERT(!list.Contains(999), "Contains(999) returns false");
}

void test_list_delete_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: List - Delete Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    List<int> list;
    for (int i = 0; i < 10; i++) {
        list.Add(i * 10);
    }
    PrintList("Initial", list);

    std::cout << "\n[2.1] Delete single element:" << std::endl;
    bool deleted = list.Delete(5);
    TEST_ASSERT(deleted, "Delete succeeded");
    TEST_ASSERT(list.GetCount() == 9, "Count decreased");
    PrintList("After delete pos 5", list);

    std::cout << "\n[2.2] Delete range:" << std::endl;
    deleted = list.Delete(2, 3);
    TEST_ASSERT(deleted, "Delete range succeeded");
    TEST_ASSERT(list.GetCount() == 6, "Count is 6");
    PrintList("After delete range", list);

    std::cout << "\n[2.3] DeleteShift (shifts elements):" << std::endl;
    list.Clear();
    for (int i = 0; i < 5; i++) list.Add(i);
    list.DeleteShift(2);
    TEST_ASSERT(*list.At(2) == 3, "Element shifted correctly");
    PrintList("After DeleteShift", list);

    std::cout << "\n[2.4] DeleteByValue:" << std::endl;
    int val_to_delete = 3;
    list.DeleteByValue(val_to_delete);
    TEST_ASSERT(!list.Contains(3), "Value deleted");
    PrintList("After DeleteByValue", list);
}

void test_list_batch_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: List - Batch Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    List<int> list;

    std::cout << "[3.1] Add batch from array:" << std::endl;
    int batch[] = {10, 20, 30, 40, 50};
    int start_idx = list.Add(batch, 5);
    TEST_ASSERT(start_idx == 0, "Add returns starting index 0");
    TEST_ASSERT(list.GetCount() == 5, "Count is 5");
    PrintList("After batch add", list);

    std::cout << "\n[3.2] Add from another list:" << std::endl;
    List<int> list2;
    list2.Add(60);
    list2.Add(70);
    list.Add(list2);
    TEST_ASSERT(list.GetCount() == 7, "Count is 7");
    PrintList("After add list", list);

    std::cout << "\n[3.3] Initializer list:" << std::endl;
    List<int> list3 = {100, 200, 300};
    TEST_ASSERT(list3.GetCount() == 3, "Initializer list worked");
    PrintList("Initializer list", list3);

    std::cout << "\n[3.4] Copy assignment:" << std::endl;
    List<int> list4;
    list4 = list3;
    TEST_ASSERT(list4.GetCount() == 3, "Copy succeeded");
    TEST_ASSERT(*list4.At(0) == 100, "First element copied");
}

void test_list_iterator()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: List - Iterator Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    List<int> list = {10, 20, 30, 40, 50};

    std::cout << "[4.1] Forward iteration:" << std::endl;
    int sum = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        sum += *it;
    }
    TEST_ASSERT(sum == 150, "Sum via iterator is 150");

    std::cout << "\n[4.2] Range-based for loop:" << std::endl;
    sum = 0;
    for (int val : list) {
        sum += val;
    }
    TEST_ASSERT(sum == 150, "Range-for sum is 150");

    std::cout << "\n[4.3] Modify via iterator:" << std::endl;
    for (auto it = list.begin(); it != list.end(); ++it) {
        *it += 1;
    }
    TEST_ASSERT(*list.At(0) == 11, "Modified via iterator");
    PrintList("After modification", list);
}

void test_list_sorting()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: List - Sorting Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    List<int> list = {50, 20, 80, 10, 40};
    PrintList("Before sort", list);

    std::cout << "\n[5.1] Manual sorting verification:" << std::endl;
    std::cout << "  Note: ValueArray does not have built-in QSort" << std::endl;
    std::cout << "  Elements can be sorted using std::sort on data()" << std::endl;
    TEST_ASSERT(list.GetCount() == 5, "List has 5 elements");

    std::cout << "\n[5.2] Find operations:" << std::endl;
    TEST_ASSERT(list.Find(80) != -1, "Find(80) succeeds");
    TEST_ASSERT(list.Find(999) == -1, "Find(999) fails");
}

void test_list_complex_type()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: List - Non-Trivial Type (ComplexItem)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    ComplexItem::construct_count = 0;
    ComplexItem::destruct_count = 0;

    {
        std::cout << "[6.1] Add complex items:" << std::endl;
        ObjectList<ComplexItem> list;

        list.Add(ComplexItem(1, "First"));
        list.Add(ComplexItem(2, "Second"));
        list.Add(ComplexItem(3, "Third"));

        TEST_ASSERT(list.GetCount() == 3, "Count is 3");
        TEST_ASSERT((*list.At(0))->id == 1, "First item id is 1");        
        std::cout << "\n[6.2] Delete complex item:" << std::endl;
        list.DeleteAtOwn(1);
        TEST_ASSERT(list.GetCount() == 2, "Count is 2");

        std::cout << "\n[6.3] Scope exit (cleanup):" << std::endl;
    }

    std::cout << "\n[6.4] Memory safety check:" << std::endl;
    TEST_ASSERT(ComplexItem::construct_count == ComplexItem::destruct_count,
                "All constructed items were destructed");
    std::cout << "    Constructs: " << ComplexItem::construct_count
              << ", Destructs: " << ComplexItem::destruct_count << std::endl;
}

void test_list_edge_cases()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 7: List - Edge Cases" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[7.1] Empty list operations:" << std::endl;
    List<int> empty;
    TEST_ASSERT(!empty.Delete(0), "Delete from empty fails");
    TEST_ASSERT(empty.Find(42) == -1, "Find in empty returns -1");

    std::cout << "\n[7.2] Single element:" << std::endl;
    empty.Add(999);
    TEST_ASSERT(empty.GetCount() == 1, "Count is 1");
    TEST_ASSERT(*empty.At(0) == 999, "Single element correct");

    std::cout << "\n[7.3] Reserve and capacity:" << std::endl;
    List<int> list;
    TEST_ASSERT(list.Reserve(100), "Reserve succeeded");
    TEST_ASSERT(list.GetAllocCount() >= 100, "Allocated at least 100");
    TEST_ASSERT(list.GetCount() == 0, "Count still 0");

    std::cout << "\n[7.4] Clear vs Free:" << std::endl;
    for (int i = 0; i < 10; i++) list.Add(i);
    list.Clear();
    TEST_ASSERT(list.GetCount() == 0, "Clear empties list");
    TEST_ASSERT(list.GetAllocCount() > 0, "Memory still allocated");

    list.Free();
    TEST_ASSERT(list.GetAllocCount() == 0, "Free releases memory");
}

// ========================================
// ObjectList Tests
// ========================================

void test_objectlist_basic_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 8: ObjectList - Basic Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[8.1] Create ObjectList:" << std::endl;
    ObjectList<TestObject> obj_list;
    TEST_ASSERT(obj_list.IsEmpty(), "New ObjectList is empty");
    TEST_ASSERT(TestObject::instance_count == 0, "No instances created");

    std::cout << "\n[8.2] Add objects (manual new):" << std::endl;
    obj_list.Add(new TestObject(1, "First"));
    obj_list.Add(new TestObject(2, "Second"));
    obj_list.Add(new TestObject(3, "Third"));
    TEST_ASSERT(obj_list.GetCount() == 3, "Count is 3");
    TEST_ASSERT(TestObject::instance_count == 3, "3 instances exist");

    std::cout << "\n[8.3] Access objects:" << std::endl;
    TEST_ASSERT(obj_list[0]->GetValue() == 1, "First object value is 1");
    TEST_ASSERT(strcmp(obj_list[1]->GetLabel(), "Second") == 0, "Second object label correct");

    std::cout << "\n[8.4] Create objects in-place:" << std::endl;
    TestObject* obj = obj_list.Create(4, "Fourth");
    TEST_ASSERT(obj != nullptr, "Create returned valid pointer");
    TEST_ASSERT(obj_list.GetCount() == 4, "Count is 4");
    TEST_ASSERT(TestObject::instance_count == 4, "4 instances exist");
}

void test_objectlist_ownership()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 9: ObjectList - Ownership Management" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    std::cout << "[9.1] DeleteAtOwn (delete + remove):" << std::endl;
    {
        ObjectList<TestObject> obj_list;
        obj_list.Add(new TestObject(1, "A"));
        obj_list.Add(new TestObject(2, "B"));
        obj_list.Add(new TestObject(3, "C"));

        std::cout << "  Before DeleteAtOwn:" << std::endl;
        obj_list.DeleteAtOwn(1);  // Should delete B and remove from list
        std::cout << "  After DeleteAtOwn:" << std::endl;
        TEST_ASSERT(obj_list.GetCount() == 2, "Count is 2");
        TEST_ASSERT(TestObject::instance_count == 2, "1 instance deleted");
    }
    TEST_ASSERT(TestObject::instance_count == 0, "All instances cleaned up");

    std::cout << "\n[9.2] Unlink (remove without delete):" << std::endl;
    TestObject::instance_count = 0;
    {
        ObjectList<TestObject> obj_list;
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

    std::cout << "\n[9.3] Clear (delete all):" << std::endl;
    TestObject::instance_count = 0;
    {
        ObjectList<TestObject> obj_list;
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
    std::cout << "TEST 10: ObjectList - Iterator Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    ObjectList<TestObject> obj_list;
    for (int i = 1; i <= 5; i++) {
        obj_list.Create(i * 10, "Item");
    }

    std::cout << "[10.1] Forward iteration:" << std::endl;
    int sum = 0;
    for (auto it = obj_list.begin(); it != obj_list.end(); ++it) {
        sum += (*it)->GetValue();
    }
    TEST_ASSERT(sum == 150, "Sum via iterator is 150");

    std::cout << "\n[10.2] Range-based for loop:" << std::endl;
    sum = 0;
    for (TestObject* obj : obj_list) {
        sum += obj->GetValue();
    }
    TEST_ASSERT(sum == 150, "Range-for sum is 150");

    std::cout << "\n[10.3] Modify via iterator:" << std::endl;
    for (auto it = obj_list.begin(); it != obj_list.end(); ++it) {
        (*it)->SetValue((*it)->GetValue() + 1);
    }
    TEST_ASSERT(obj_list[0]->GetValue() == 11, "Modified via iterator");
}

void test_objectlist_edge_cases()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 11: ObjectList - Edge Cases" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    std::cout << "[11.1] Empty ObjectList:" << std::endl;
    ObjectList<TestObject> empty;
    TEST_ASSERT(!empty.DeleteAtOwn(0), "DeleteAtOwn on empty fails");
    TEST_ASSERT(TestObject::instance_count == 0, "No leaks");

    std::cout << "\n[11.2] Single object:" << std::endl;
    empty.Create(42, "Solo");
    TEST_ASSERT(empty.GetCount() == 1, "Count is 1");
    empty.DeleteAtOwn(0);
    TEST_ASSERT(empty.IsEmpty(), "Empty after delete");
    TEST_ASSERT(TestObject::instance_count == 0, "Instance deleted");

    std::cout << "\n[11.3] DeleteRangeOwn:" << std::endl;
    for (int i = 0; i < 10; i++) {
        empty.Create(i, "Range");
    }
    TEST_ASSERT(TestObject::instance_count == 10, "10 instances");

    empty.DeleteRangeOwn(3, 4);  // Delete indices 3-6
    TEST_ASSERT(empty.GetCount() == 6, "6 remaining");
    TEST_ASSERT(TestObject::instance_count == 6, "4 instances deleted");

    std::cout << "\n[11.4] Free vs Clear:" << std::endl;
    empty.Free();
    TEST_ASSERT(empty.GetCount() == 0, "Free empties list");
    TEST_ASSERT(TestObject::instance_count == 0, "All deleted");
}

void test_objectlist_mixed_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 12: ObjectList - Mixed Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    ObjectList<TestObject> obj_list;

    std::cout << "[12.1] Mix of Add and Create:" << std::endl;
    obj_list.Add(new TestObject(1, "Manual"));
    obj_list.Create(2, "InPlace");
    obj_list.Add(new TestObject(3, "Manual2"));
    TEST_ASSERT(obj_list.GetCount() == 3, "Count is 3");
    TEST_ASSERT(TestObject::instance_count == 3, "3 instances");

    std::cout << "\n[12.2] Mix of Unlink and DeleteAtOwn:" << std::endl;
    TestObject* keep_ref = obj_list[0];
    obj_list.Unlink(0);  // Remove first without delete
    TEST_ASSERT(TestObject::instance_count == 3, "Still 3 instances");

    obj_list.DeleteAtOwn(0);  // Delete second (now at index 0)
    TEST_ASSERT(TestObject::instance_count == 2, "Down to 2 instances");

    std::cout << "\n[12.3] Cleanup:" << std::endl;
    delete keep_ref;
    TEST_ASSERT(TestObject::instance_count == 1, "Manual delete worked");
}

void test_list_stress()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 13: List - Stress Test" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[13.1] Add 10000 elements:" << std::endl;
    List<int> list;
    for (int i = 0; i < 10000; i++) {
        list.Add(i);
    }
    TEST_ASSERT(list.GetCount() == 10000, "Count is 10000");

    std::cout << "\n[13.2] Random access:" << std::endl;
    bool access_ok = true;
    for (int i = 0; i < 1000; i++) {
        int idx = (i * 7) % 10000;
        if (*list.At(idx) != idx) {
            access_ok = false;
            break;
        }
    }
    TEST_ASSERT(access_ok, "Random access correct");

    std::cout << "\n[13.3] Delete half:" << std::endl;
    for (int i = 0; i < 5000; i++) {
        list.Delete(0);
    }
    TEST_ASSERT(list.GetCount() == 5000, "Count is 5000");

    std::cout << "\n[13.4] Cleanup:" << std::endl;
    list.Free();
    TEST_ASSERT(list.GetCount() == 0, "Freed successfully");
}

void test_objectlist_stress()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 14: ObjectList - Stress Test" << std::endl;
    std::cout << "========================================\n" << std::endl;

    TestObject::instance_count = 0;

    std::cout << "[14.1] Create 1000 objects:" << std::endl;
    {
        ObjectList<TestObject> obj_list;
        for (int i = 0; i < 1000; i++) {
            obj_list.Create(i, "Stress");
        }
        TEST_ASSERT(obj_list.GetCount() == 1000, "Count is 1000");
        TEST_ASSERT(TestObject::instance_count == 1000, "1000 instances");

        std::cout << "\n[14.2] Delete every other object:" << std::endl;
        for (int i = 999; i >= 0; i -= 2) {
            obj_list.DeleteAtOwn(i);
        }
        TEST_ASSERT(obj_list.GetCount() == 500, "Count is 500");
        TEST_ASSERT(TestObject::instance_count == 500, "500 instances remain");

        std::cout << "\n[14.3] Scope exit (auto cleanup):" << std::endl;
    }
    TEST_ASSERT(TestObject::instance_count == 0, "All cleaned up");
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "List and ObjectList Comprehensive Test Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ValueArray/List tests
    test_list_basic_operations();
    test_list_delete_operations();
    test_list_batch_operations();
    test_list_iterator();
    test_list_sorting();
    test_list_complex_type();
    test_list_edge_cases();

    // ObjectList tests
    test_objectlist_basic_operations();
    test_objectlist_ownership();
    test_objectlist_iterator();
    test_objectlist_edge_cases();
    test_objectlist_mixed_operations();

    // Stress tests
    test_list_stress();
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
