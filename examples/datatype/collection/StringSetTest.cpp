#include<hgl/type/ConstStringSet.h>
#include<hgl/type/IDName.h>
#include<hgl/type/SortedSet.h>
#include<hgl/type/String.h>
#include<iostream>
#include<cstring>

using namespace hgl;
using namespace std;

//#define SCL std::source_location::current()

// 非平凡类型用于 SortedObjectSet 测试
struct ComplexData
{
    int id;
    std::string str_name;  // 使用 std::string 而不是 U8String
    int* dynamic_data;

    static int construct_count;
    static int destruct_count;
    static int copy_count;
    static int move_count;

    ComplexData() : id(0), dynamic_data(nullptr)
    {
        construct_count++;
        std::cout << "    [ComplexData] Default construct #" << construct_count << std::endl;
    }

    ComplexData(int i, const char* n) : id(i), str_name(n), dynamic_data(new int(i * 100))
    {
        construct_count++;
        std::cout << "    [ComplexData] Construct #" << construct_count << " id=" << id << " name=" << n << std::endl;
    }

    ComplexData(const ComplexData& other) : id(other.id), str_name(other.str_name)
    {
        construct_count++;
        copy_count++;
        dynamic_data = other.dynamic_data ? new int(*other.dynamic_data) : nullptr;
        std::cout << "    [ComplexData] Copy construct #" << construct_count << " from id=" << other.id << std::endl;
    }

    ComplexData(ComplexData&& other) noexcept : id(other.id), str_name(std::move(other.str_name)), dynamic_data(other.dynamic_data)
    {
        construct_count++;
        move_count++;
        other.dynamic_data = nullptr;
        std::cout << "    [ComplexData] Move construct #" << construct_count << " from id=" << other.id << std::endl;
    }

    ComplexData& operator=(const ComplexData& other)
    {
        if (this != &other) {
            copy_count++;
            id = other.id;
            str_name = other.str_name;
            delete dynamic_data;
            dynamic_data = other.dynamic_data ? new int(*other.dynamic_data) : nullptr;
            std::cout << "    [ComplexData] Copy assign id=" << id << std::endl;
        }
        return *this;
    }

    ComplexData& operator=(ComplexData&& other) noexcept
    {
        if (this != &other) {
            move_count++;
            id = other.id;
            str_name = std::move(other.str_name);
            delete dynamic_data;
            dynamic_data = other.dynamic_data;
            other.dynamic_data = nullptr;
            std::cout << "    [ComplexData] Move assign id=" << id << std::endl;
        }
        return *this;
    }

    ~ComplexData()
    {
        destruct_count++;
        std::cout << "    [ComplexData] Destruct #" << destruct_count << " id=" << id
                  << " ptr=" << (void*)dynamic_data << std::endl;
        delete dynamic_data;
    }

    bool operator<(const ComplexData& other) const { return id < other.id; }
    bool operator>(const ComplexData& other) const { return id > other.id; }
    bool operator==(const ComplexData& other) const { return id == other.id; }
};

int ComplexData::construct_count = 0;
int ComplexData::destruct_count = 0;
int ComplexData::copy_count = 0;
int ComplexData::move_count = 0;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "  ✓ PASS: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  ✗ FAIL: " << message << std::endl; \
        } \
    } while(0)

void test_sorted_set_complex()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: SortedObjectSet<ComplexData> (Non-Trivial Type)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    ComplexData::construct_count = 0;
    ComplexData::destruct_count = 0;
    ComplexData::copy_count = 0;
    ComplexData::move_count = 0;

    {
        SortedObjectSet<ComplexData> set;

        std::cout << "\n[2.1] Add complex objects:" << std::endl;
        ComplexData obj1(10, "First");
        ComplexData obj2(5, "Second");
        ComplexData obj3(15, "Third");

        set.Add(obj1);
        set.Add(obj2);
        set.Add(obj3);

        TEST_ASSERT(set.GetCount() == 3, "Count is 3");

        std::cout << "\n[2.2] Find objects:" << std::endl;
        ComplexData search(5, "");
        TEST_ASSERT(set.Contains(search), "Contains object with id=5");

        std::cout << "\n[2.3] Delete object:" << std::endl;
        TEST_ASSERT(set.Delete(search), "Delete succeeded");
        TEST_ASSERT(set.GetCount() == 2, "Count after delete is 2");

        std::cout << "\n[2.4] Get first and last:" << std::endl;
        ComplexData first, last;
        TEST_ASSERT(set.GetFirst(first), "GetFirst succeeded");
        TEST_ASSERT(set.GetLast(last), "GetLast succeeded");
        TEST_ASSERT(first.id < last.id, "First < Last");

        std::cout << "\n[2.5] Cleanup..." << std::endl;
    }

    std::cout << "\n[2.6] Verify memory safety:" << std::endl;
    TEST_ASSERT(ComplexData::construct_count == ComplexData::destruct_count,
                "All constructed objects were destructed");
    std::cout << "    Constructs: " << ComplexData::construct_count
              << ", Destructs: " << ComplexData::destruct_count << std::endl;
}

void test_const_string_set()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: ConstStringSet<char>" << std::endl;
    std::cout << "========================================\n" << std::endl;

    ConstStringSet<char> css;

    std::cout << "[3.1] Add strings:" << std::endl;
    int id1 = css.Add("Hello", 5);
    int id2 = css.Add("World", 5);
    int id3 = css.Add("Test", 4);

    TEST_ASSERT(id1 >= 0, "First string added");
    TEST_ASSERT(id2 >= 0, "Second string added");
    TEST_ASSERT(id3 >= 0, "Third string added");
    TEST_ASSERT(css.GetCount() == 3, "Count is 3");

    std::cout << "\n[3.2] Add duplicate string:" << std::endl;
    int id_dup = css.Add("Hello", 5);
    TEST_ASSERT(id_dup == id1, "Duplicate returns same ID");
    TEST_ASSERT(css.GetCount() == 3, "Count still 3");

    std::cout << "\n[3.3] Contains check:" << std::endl;
    TEST_ASSERT(css.Contains("Hello", 5), "Contains 'Hello'");
    TEST_ASSERT(css.Contains("World", 5), "Contains 'World'");
    TEST_ASSERT(!css.Contains("NotThere", 8), "Does not contain 'NotThere'");

    std::cout << "\n[3.4] GetID:" << std::endl;
    int found_id = css.GetID("World", 5);
    TEST_ASSERT(found_id == id2, "GetID returns correct ID");

    std::cout << "\n[3.5] GetString by ID:" << std::endl;
    const char* str = css.GetString(id1);
    TEST_ASSERT(str != nullptr, "GetString returns non-null");
    TEST_ASSERT(strcmp(str, "Hello") == 0, "GetString returns correct string");

    std::cout << "\n[3.6] Iterate over strings:" << std::endl;
    int iter_count = 0;
    for (const auto* view = css.begin(); view != css.end(); ++view) {
        iter_count++;
        TEST_ASSERT(view->id >= 0, "String view has valid ID");
    }
    TEST_ASSERT(iter_count == 3, "Iterator covered all strings");

    std::cout << "\n[3.7] Range-based for loop:" << std::endl;
    int range_count = 0;
    for (const auto& view : css) {
        range_count++;
        TEST_ASSERT(view.id >= 0, "Range-for view has valid ID");
    }
    TEST_ASSERT(range_count == 3, "Range-for covered all strings");

    std::cout << "\n[3.8] Large batch test:" << std::endl;
    for (int i = 0; i < 100; i++) {
        char buf[32];
        sprintf(buf, "String_%d", i);
        css.Add(buf, strlen(buf));
    }
    TEST_ASSERT(css.GetCount() == 103, "Count after batch is 103");

    std::cout << "\n[3.9] Clear:" << std::endl;
    css.Clear();
    TEST_ASSERT(css.IsEmpty(), "Set is empty after clear");
}

void test_ordered_id_name()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: OrderedIDName" << std::endl;
    std::cout << "========================================\n" << std::endl;

    HGL_DEFINE_ANSI_IDNAME(TestName);

    std::cout << "[4.1] Create OrderedIDName objects:" << std::endl;
    TestName name1("Material1");
    TestName name2("Material2");
    TestName name3("Material1"); // Duplicate

    TEST_ASSERT(name1.GetID() >= 0, "name1 has valid ID");
    TEST_ASSERT(name2.GetID() >= 0, "name2 has valid ID");
    TEST_ASSERT(name1.GetID() == name3.GetID(), "Duplicate names have same ID");
    TEST_ASSERT(name1.GetID() != name2.GetID(), "Different names have different IDs");

    std::cout << "\n[4.2] GetName:" << std::endl;
    const char* str1 = name1.GetName();
    TEST_ASSERT(str1 != nullptr, "GetName returns non-null");
    TEST_ASSERT(strcmp(str1, "Material1") == 0, "GetName returns correct string");

    std::cout << "\n[4.3] Assignment:" << std::endl;
    TestName name4;
    name4 = "Material3";
    TEST_ASSERT(name4.GetID() >= 0, "Assignment works");
    TEST_ASSERT(strcmp(name4.GetName(), "Material3") == 0, "Assigned name is correct");

    std::cout << "\n[4.4] String assignment:" << std::endl;
    std::string str_obj("Material4");
    TestName name5;
    name5 = str_obj.c_str();
    TEST_ASSERT(name5.GetID() >= 0, "String assignment works");
    TEST_ASSERT(strcmp(name5.GetName(), "Material4") == 0, "String assigned name is correct");

    std::cout << "\n[4.5] Copy assignment:" << std::endl;
    TestName name6;
    name6 = name1;
    TEST_ASSERT(name6.GetID() == name1.GetID(), "Copy assignment preserves ID");

    std::cout << "\n[4.6] Compare using operators:" << std::endl;
    TEST_ASSERT(name1 != name2, "Different names are not equal");
    TEST_ASSERT(name1 == name3, "Same names are equal");
    TEST_ASSERT((name1 <=> name2) != std::strong_ordering::equal, "Three-way comparison works");
    
    std::cout << "\n[4.7] Ordering:" << std::endl;
    TEST_ASSERT(name1.GetID() < name2.GetID() ? (name1 < name2) : (name1 >= name2), "Operator< works");
}

void test_sorted_set_with_id_name()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: SortedSet<OrderedIDName>" << std::endl;
    std::cout << "========================================\n" << std::endl;

    HGL_DEFINE_ANSI_IDNAME(MaterialName);

    std::cout << "[5.1] Create SortedSet of OrderedIDName:" << std::endl;
    MaterialNameSet name_set;

    MaterialName mat1("Texture1");
    MaterialName mat2("Texture2");
    MaterialName mat3("Texture3");
    MaterialName mat4("Texture1"); // Duplicate name

    name_set.Add(mat1);
    name_set.Add(mat2);
    name_set.Add(mat3);

    TEST_ASSERT(name_set.GetCount() == 3, "Count is 3");

    std::cout << "\n[5.2] Add duplicate ID (should be rejected):" << std::endl;
    int64 pos = name_set.Add(mat4);
    TEST_ASSERT(pos == -1, "Duplicate ID rejected");
    TEST_ASSERT(name_set.GetCount() == 3, "Count still 3");

    std::cout << "\n[5.3] Find by ID:" << std::endl;
    MaterialName search = mat1;
    TEST_ASSERT(name_set.Contains(search), "Contains mat1");

    std::cout << "\n[5.4] Delete:" << std::endl;
    TEST_ASSERT(name_set.Delete(mat2), "Delete mat2 succeeded");
    TEST_ASSERT(name_set.GetCount() == 2, "Count after delete is 2");

    std::cout << "\n[5.5] Large batch test:" << std::endl;
    for (int i = 0; i < 50; i++) {
        char buf[32];
        sprintf(buf, "Material_%d", i);
        MaterialName mat(buf);
        name_set.Add(mat);
    }
    TEST_ASSERT(name_set.GetCount() == 52, "Count after batch is 52");
}

void test_edge_cases()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: Edge Cases and Stress Tests" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[6.1] Empty operations:" << std::endl;
    SortedSet<int> empty_set;
    TEST_ASSERT(empty_set.IsEmpty(), "Empty set is empty");
    TEST_ASSERT(!empty_set.Delete(5), "Delete from empty fails");
    int val;
    TEST_ASSERT(!empty_set.GetFirst(val), "GetFirst on empty fails");
    TEST_ASSERT(!empty_set.GetLast(val), "GetLast on empty fails");

    std::cout << "\n[6.2] Single element:" << std::endl;
    empty_set.Add(42);
    TEST_ASSERT(empty_set.GetFirst(val) && val == 42, "Single element GetFirst");
    TEST_ASSERT(empty_set.GetLast(val) && val == 42, "Single element GetLast");
    std::cout << "  [6.2] Completed" << std::endl;

    std::cout << "\n[6.3] ConstStringSet empty string:" << std::endl;
    ConstStringSet<char> css;
    std::cout << "  ConstStringSet created" << std::endl;
    
    int id = css.Add("", 0);
    std::cout << "  Add(\"\", 0) returned: " << id << std::endl;
    TEST_ASSERT(id == -1, "Empty string rejected");

    std::cout << "\n[6.4] ConstStringSet null pointer:" << std::endl;
    id = css.Add(nullptr, 5);
    TEST_ASSERT(id == -1, "Null pointer rejected");
    std::cout << "  [6.4] Completed" << std::endl;

    std::cout << "\n[6.5] Long strings:" << std::endl;
    char long_str[1024];
    memset(long_str, 'A', 1023);
    long_str[1023] = 0;
    id = css.Add(long_str, 1023);
    TEST_ASSERT(id >= 0, "Long string accepted");
    TEST_ASSERT(css.Contains(long_str, 1023), "Long string found");

    std::cout << "\n[6.6] Stress test with many strings:" << std::endl;
    ConstStringSet<char> stress_css;
    for (int i = 0; i < 1000; i++) {
        char buf[64];
        sprintf(buf, "StressTest_%d_%d", i, i * 7);
        stress_css.Add(buf, strlen(buf));
    }
    TEST_ASSERT(stress_css.GetCount() == 1000, "Stress test: 1000 strings added");

    std::cout << "\n[6.7] Memory reallocation test:" << std::endl;
    SortedSet<int> realloc_set;
    realloc_set.Reserve(10);
    for (int i = 0; i < 1000; i++) {
        realloc_set.Add(i * 3);
    }
    TEST_ASSERT(realloc_set.GetCount() == 1000, "Reallocation test: 1000 elements");
    TEST_ASSERT(realloc_set.Contains(999 * 3), "Last element exists");
    TEST_ASSERT(realloc_set.Contains(0), "First element exists");
}

int main(int,char **)
{
    std::cout << "========================================" << std::endl;
    std::cout << "String Set Comprehensive Test Suite" << std::endl;
    std::cout << "========================================\n" << std::endl;

    test_sorted_set_complex();
    test_const_string_set();
    test_ordered_id_name();
    test_sorted_set_with_id_name();
    test_edge_cases();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Tests Passed: " << tests_passed << std::endl;
    std::cout << "Tests Failed: " << tests_failed << std::endl;
    std::cout << "Total Tests: " << (tests_passed + tests_failed) << std::endl;

    if (tests_failed == 0) {
        std::cout << "\n🎉 ALL TESTS PASSED! 🎉" << std::endl;
    } else {
        std::cout << "\n❌ SOME TESTS FAILED" << std::endl;
    }

    return tests_failed > 0 ? 1 : 0;
}
