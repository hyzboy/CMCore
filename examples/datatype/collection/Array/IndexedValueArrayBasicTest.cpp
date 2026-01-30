#include<hgl/type/IndexedValueArray.h>
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

    ~ComplexItem()
    {
        destruct_count++;
        delete data;
    }
};

int ComplexItem::construct_count = 0;
int ComplexItem::destruct_count = 0;

// 辅助函数：打印列表内容
template<typename T>
void PrintList(const char* label, const IndexedValueArray<T>& list)
{
    std::cout << "    " << label << " [count=" << list.GetCount()
              << ", alloc=" << list.GetAllocCount()
              << ", free=" << list.GetFreeCount() << "]: ";

    if (list.IsEmpty()) {
        std::cout << "(empty)";
    } else {
        for (int i = 0; i < list.GetCount(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << list[i];
        }
    }
    std::cout << std::endl;
}

void test_basic_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: Basic Operations (int)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[1.1] Create empty list:" << std::endl;
    IndexedValueArray<int> list;
    TEST_ASSERT(list.IsEmpty(), "New list is empty");
    TEST_ASSERT(list.GetCount() == 0, "Count is 0");
    TEST_ASSERT(list.GetFreeCount() == 0, "Free count is 0");

    std::cout << "\n[1.2] Add single elements:" << std::endl;
    list.Add(10);
    list.Add(20);
    list.Add(30);
    TEST_ASSERT(list.GetCount() == 3, "Count is 3");
    TEST_ASSERT(list[0] == 10, "First element is 10");
    TEST_ASSERT(list[1] == 20, "Second element is 20");
    TEST_ASSERT(list[2] == 30, "Third element is 30");
    PrintList("After add", list);

    std::cout << "\n[1.3] Add batch elements:" << std::endl;
    int batch[] = {40, 50, 60};
    int added = list.Add(batch, 3);
    TEST_ASSERT(added == 3, "Added 3 elements");
    TEST_ASSERT(list.GetCount() == 6, "Count is 6");
    TEST_ASSERT(list[5] == 60, "Last element is 60");
    PrintList("After batch add", list);

    std::cout << "\n[1.4] Reserve space:" << std::endl;
    TEST_ASSERT(list.Reserve(20), "Reserve succeeded");
    TEST_ASSERT(list.GetAllocCount() >= 20, "Allocated at least 20");
    TEST_ASSERT(list.GetCount() == 6, "Count unchanged");
}

void test_delete_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: Delete Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    IndexedValueArray<int> list;
    for (int i = 0; i < 10; i++) {
        list.Add(i * 10);
    }
    PrintList("Initial", list);

    std::cout << "\n[2.1] Delete single element:" << std::endl;
    int deleted = list.Delete(5, 1);
    TEST_ASSERT(deleted == 1, "Deleted 1 element");
    TEST_ASSERT(list.GetCount() == 9, "Count is 9");
    TEST_ASSERT(list.GetFreeCount() == 1, "Free count is 1");
    TEST_ASSERT(list[5] == 60, "Element at position 5 is 60");
    PrintList("After delete pos 5", list);

    std::cout << "\n[2.2] Delete multiple elements:" << std::endl;
    deleted = list.Delete(2, 3);
    TEST_ASSERT(deleted == 3, "Deleted 3 elements");
    TEST_ASSERT(list.GetCount() == 6, "Count is 6");
    TEST_ASSERT(list.GetFreeCount() == 4, "Free count is 4");
    PrintList("After delete pos 2-4", list);

    std::cout << "\n[2.3] Reuse freed space:" << std::endl;
    list.Add(999);
    TEST_ASSERT(list.GetCount() == 7, "Count is 7");
    TEST_ASSERT(list.GetFreeCount() == 3, "Free count decreased to 3");
    TEST_ASSERT(list[6] == 999, "New element at end");
    PrintList("After add (reuse)", list);

    std::cout << "\n[2.4] Delete at boundaries:" << std::endl;
    list.Delete(0, 1);  // Delete first
    TEST_ASSERT(list.GetCount() == 6, "Deleted first");
    list.Delete(list.GetCount()-1, 1);  // Delete last
    TEST_ASSERT(list.GetCount() == 5, "Deleted last");
    PrintList("After boundary delete", list);
}

void test_insert_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: Insert Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    IndexedValueArray<int> list;
    list.Add(10);
    list.Add(30);
    list.Add(50);
    PrintList("Initial", list);

    std::cout << "\n[3.1] Insert at beginning:" << std::endl;
    TEST_ASSERT(list.Insert(0, 5), "Insert succeeded");
    TEST_ASSERT(list.GetCount() == 4, "Count is 4");
    TEST_ASSERT(list[0] == 5, "First element is 5");
    TEST_ASSERT(list[1] == 10, "Second element is 10");
    PrintList("After insert at 0", list);

    std::cout << "\n[3.2] Insert in middle:" << std::endl;
    TEST_ASSERT(list.Insert(2, 20), "Insert succeeded");
    TEST_ASSERT(list.GetCount() == 5, "Count is 5");
    TEST_ASSERT(list[2] == 20, "Element at pos 2 is 20");
    PrintList("After insert at 2", list);

    std::cout << "\n[3.3] Insert at end:" << std::endl;
    TEST_ASSERT(list.Insert(list.GetCount(), 60), "Insert succeeded");
    TEST_ASSERT(list.GetCount() == 6, "Count is 6");
    TEST_ASSERT(list[5] == 60, "Last element is 60");
    PrintList("After insert at end", list);

    std::cout << "\n[3.4] Insert after delete:" << std::endl;
    list.Delete(2, 2);  // Delete to create free slots
    TEST_ASSERT(list.Insert(1, 15), "Insert succeeded");
    TEST_ASSERT(list.GetFreeCount() == 1, "Reused one free slot");
    PrintList("After insert with reuse", list);
}

void test_exchange_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: Exchange Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    IndexedValueArray<int> list;
    for (int i = 0; i < 5; i++) {
        list.Add(i * 10);
    }
    PrintList("Initial", list);

    std::cout << "\n[4.1] Exchange two elements:" << std::endl;
    TEST_ASSERT(list.Exchange(1, 3), "Exchange succeeded");
    TEST_ASSERT(list[1] == 30, "Element 1 is now 30");
    TEST_ASSERT(list[3] == 10, "Element 3 is now 10");
    PrintList("After exchange 1,3", list);

    std::cout << "\n[4.2] Exchange first and last:" << std::endl;
    TEST_ASSERT(list.Exchange(0, 4), "Exchange succeeded");
    TEST_ASSERT(list[0] == 40, "First is now 40");
    TEST_ASSERT(list[4] == 0, "Last is now 0");
    PrintList("After exchange 0,4", list);

    std::cout << "\n[4.3] Invalid exchange:" << std::endl;
    TEST_ASSERT(!list.Exchange(-1, 2), "Invalid negative index");
    TEST_ASSERT(!list.Exchange(2, 10), "Invalid large index");
}

void test_iterator()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: Iterator Operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    IndexedValueArray<int> list;
    for (int i = 1; i <= 5; i++) {
        list.Add(i * 10);
    }

    std::cout << "[5.1] Forward iteration:" << std::endl;
    int sum = 0;
    int count = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        sum += *it;
        count++;
    }
    TEST_ASSERT(count == 5, "Iterated 5 elements");
    TEST_ASSERT(sum == 150, "Sum is correct");

    std::cout << "\n[5.2] Range-based for loop:" << std::endl;
    sum = 0;
    count = 0;
    for (int val : list) {
        sum += val;
        count++;
    }
    TEST_ASSERT(count == 5, "Range-for iterated 5 elements");
    TEST_ASSERT(sum == 150, "Range-for sum is correct");

    std::cout << "\n[5.3] Const iterator:" << std::endl;
    const IndexedValueArray<int>& const_list = list;
    sum = 0;
    for (auto it = const_list.begin(); it != const_list.end(); ++it) {
        sum += *it;
    }
    TEST_ASSERT(sum == 150, "Const iterator sum is correct");

    std::cout << "\n[5.4] Iterator modification:" << std::endl;
    for (auto it = list.begin(); it != list.end(); ++it) {
        *it += 1;
    }
    TEST_ASSERT(list[0] == 11, "Modified via iterator");
    TEST_ASSERT(list[4] == 51, "Last modified via iterator");
}

void test_shrink_reorder()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: Shrink and Reorder" << std::endl;
    std::cout << "========================================\n" << std::endl;

    IndexedValueArray<int> list;
    for (int i = 0; i < 10; i++) {
        list.Add(i * 10);
    }

    std::cout << "[6.1] Delete to create gaps:" << std::endl;
    list.Delete(2, 2);  // Delete positions 2,3
    list.Delete(5, 2);  // Delete positions 5,6 (now shifted)
    PrintList("After deletes", list);
    TEST_ASSERT(!list.IsOrdered(), "List is not ordered");

    std::cout << "\n[6.2] Shrink:" << std::endl;
    TEST_ASSERT(list.Shrink(), "Shrink succeeded");
    PrintList("After shrink", list);
    TEST_ASSERT(list.GetFreeCount() == 0, "No free slots after shrink");

    std::cout << "\n[6.3] Exchange to disorder:" << std::endl;
    list.Exchange(0, list.GetCount()-1);
    list.Exchange(1, list.GetCount()-2);
    PrintList("After exchanges", list);
    TEST_ASSERT(!list.IsOrdered(), "List is disordered");

    std::cout << "\n[6.4] Reorder:" << std::endl;
    list.Reorder();
    PrintList("After reorder", list);
    TEST_ASSERT(list.IsOrdered(), "List is ordered");
    // Verify order is maintained
    for (int i = 1; i < list.GetCount(); i++) {
        TEST_ASSERT(list.GetRawIndex()[i] == list.GetRawIndex()[i-1] + 1,
                    "Index is sequential");
    }
}

void test_edge_cases()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 8: Edge Cases" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[8.1] Empty list operations:" << std::endl;
    IndexedValueArray<int> empty;
    TEST_ASSERT(empty.IsEmpty(), "Empty list is empty");
    TEST_ASSERT(empty.GetCount() == 0, "Empty count is 0");
    TEST_ASSERT(empty.Delete(0, 1) == -1, "Delete from empty fails");
    TEST_ASSERT(!empty.Exchange(0, 1), "Exchange in empty fails");

    std::cout << "\n[8.2] Single element:" << std::endl;
    empty.Add(42);
    TEST_ASSERT(!empty.IsEmpty(), "Not empty after add");
    TEST_ASSERT(empty[0] == 42, "Single element correct");
    TEST_ASSERT(empty.IsOrdered(), "Single element is ordered");

    std::cout << "\n[8.3] Large batch add:" << std::endl;
    IndexedValueArray<int> large;
    int batch[100];
    for (int i = 0; i < 100; i++) batch[i] = i;
    large.Add(batch, 100);
    TEST_ASSERT(large.GetCount() == 100, "Added 100 elements");
    TEST_ASSERT(large[99] == 99, "Last element correct");

    std::cout << "\n[8.4] Delete all elements:" << std::endl;
    large.Delete(0, 100);
    TEST_ASSERT(large.IsEmpty(), "Empty after deleting all");
    TEST_ASSERT(large.GetFreeCount() == 100, "All slots freed");

    std::cout << "\n[8.5] Add after delete all:" << std::endl;
    large.Add(999);
    TEST_ASSERT(large.GetCount() == 1, "Count is 1");
    TEST_ASSERT(large.GetFreeCount() == 99, "Free count decreased");
    TEST_ASSERT(large[0] == 999, "Reused slot correctly");

    std::cout << "\n[8.6] Out of bounds access:" << std::endl;
    IndexedValueArray<int> list;
    list.Add(10);
    list.Add(20);
    // Note: operator[] returns first element for out of bounds
    int val = list[-1];  // Should return first element
    TEST_ASSERT(val == 10, "Negative index returns first");
    val = list[100];  // Should return first element
    TEST_ASSERT(val == 10, "Large index returns first");
}

void test_stress()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 9: Stress Test" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[9.1] Add/Delete cycle:" << std::endl;
    IndexedValueArray<int> list;

    for (int cycle = 0; cycle < 10; cycle++) {
        // Add 50 elements
        for (int i = 0; i < 50; i++) {
            list.Add(cycle * 100 + i);
        }

        // Delete 25 elements
        for (int i = 0; i < 25; i++) {
            if (list.GetCount() > 0) {
                list.Delete(list.GetCount() / 2, 1);
            }
        }
    }

    TEST_ASSERT(list.GetCount() == 250, "Count is 250 after cycles");
    TEST_ASSERT(list.GetFreeCount() > 0, "Has free slots");

    std::cout << "    Final state: count=" << list.GetCount()
              << ", free=" << list.GetFreeCount() << std::endl;

    std::cout << "\n[9.2] Shrink and verify:" << std::endl;
    list.Shrink();
    TEST_ASSERT(list.GetCount() == 250, "Count preserved after shrink");
    TEST_ASSERT(list.GetFreeCount() == 0, "No free slots after shrink");

    std::cout << "\n[9.3] Reorder and verify:" << std::endl;
    list.Reorder();
    TEST_ASSERT(list.IsOrdered(), "List is ordered");
    TEST_ASSERT(list.GetCount() == 250, "Count preserved after reorder");
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "IndexedValueArray Comprehensive Test Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;

    test_basic_operations();
    test_delete_operations();
    test_insert_operations();
    test_exchange_operations();
    test_iterator();
    test_shrink_reorder();
    test_edge_cases();
    test_stress();

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
