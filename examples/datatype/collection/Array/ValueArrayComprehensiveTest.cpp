#include<hgl/type/ValueArray.h>
#include<hgl/type/ManagedArray.h>
#include<iostream>

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

// 辅助函数：打印List
template<typename T>
void PrintList(const char* label, const ValueArray<T>& list)
{
    std::cout << "    " << label << " [count=" << list.GetCount() << "]: ";

    if (list.IsEmpty()) {
        std::cout << "(empty)";
    } else {
        for (int i = 0; i < list.GetCount() && i < 10; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << *list.At(i);
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
    ValueArray<int> list;
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

    ValueArray<int> list;
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

    ValueArray<int> list;

    std::cout << "[3.1] Add batch from array:" << std::endl;
    int batch[] = {10, 20, 30, 40, 50};
    int start_idx = list.Add(batch, 5);
    TEST_ASSERT(start_idx == 0, "Add returns starting index 0");
    TEST_ASSERT(list.GetCount() == 5, "Count is 5");
    PrintList("After batch add", list);

    std::cout << "\n[3.2] Add from another list:" << std::endl;
    ValueArray<int> list2;
    list2.Add(60);
    list2.Add(70);
    list.Add(list2);
    TEST_ASSERT(list.GetCount() == 7, "Count is 7");
    PrintList("After add list", list);

    std::cout << "\n[3.3] Initializer list:" << std::endl;
    ValueArray<int> list3 = {100, 200, 300};
    TEST_ASSERT(list3.GetCount() == 3, "Initializer list worked");
    PrintList("Initializer list", list3);

    std::cout << "\n[3.4] Copy assignment:" << std::endl;
    ValueArray<int> list4;
    list4 = list3;
    TEST_ASSERT(list4.GetCount() == 3, "Copy succeeded");
    TEST_ASSERT(*list4.At(0) == 100, "First element copied");
}

void test_list_iterator()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: List - Iterator Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    ValueArray<int> list = {10, 20, 30, 40, 50};

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

    ValueArray<int> list = {50, 20, 80, 10, 40};
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
        ManagedArray<ComplexItem> list;

        list.Add(new ComplexItem(1, "First"));
        list.Add(new ComplexItem(2, "Second"));
        list.Add(new ComplexItem(3, "Third"));

        TEST_ASSERT(list.GetCount() == 3, "Count is 3");
        TEST_ASSERT(list.At(0)->id == 1, "First item id is 1");        
        std::cout << "\n[6.2] Delete complex item:" << std::endl;
        list.DeleteAt(1);
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
    ValueArray<int> empty;
    TEST_ASSERT(!empty.Delete(0), "Delete from empty fails");
    TEST_ASSERT(empty.Find(42) == -1, "Find in empty returns -1");

    std::cout << "\n[7.2] Single element:" << std::endl;
    empty.Add(999);
    TEST_ASSERT(empty.GetCount() == 1, "Count is 1");
    TEST_ASSERT(*empty.At(0) == 999, "Single element correct");

    std::cout << "\n[7.3] Reserve and capacity:" << std::endl;
    ValueArray<int> list;
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

void test_list_stress()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 8: List - Stress Test" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[8.1] Add 10000 elements:" << std::endl;
    ValueArray<int> list;
    for (int i = 0; i < 10000; i++) {
        list.Add(i);
    }
    TEST_ASSERT(list.GetCount() == 10000, "Count is 10000");

    std::cout << "\n[8.2] Random access:" << std::endl;
    bool access_ok = true;
    for (int i = 0; i < 1000; i++) {
        int idx = (i * 7) % 10000;
        if (*list.At(idx) != idx) {
            access_ok = false;
            break;
        }
    }
    TEST_ASSERT(access_ok, "Random access correct");

    std::cout << "\n[8.3] Delete half:" << std::endl;
    for (int i = 0; i < 5000; i++) {
        list.Delete(0);
    }
    TEST_ASSERT(list.GetCount() == 5000, "Count is 5000");

    std::cout << "\n[8.4] Cleanup:" << std::endl;
    list.Free();
    TEST_ASSERT(list.GetCount() == 0, "Freed successfully");
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "ValueArray Comprehensive Test Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ValueArray/List tests
    test_list_basic_operations();
    test_list_delete_operations();
    test_list_batch_operations();
    test_list_iterator();
    test_list_sorting();
    test_list_complex_type();
    test_list_edge_cases();
    test_list_stress();

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
