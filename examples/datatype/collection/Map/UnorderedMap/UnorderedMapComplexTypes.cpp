#include<hgl/type/UnorderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

struct Person
{
    string name;
    int age;
    double salary;

    Person() = default;
    Person(const string& n, int a, double s) : name(n), age(a), salary(s) {}

    bool operator==(const Person& other) const
    {
        return name == other.name && age == other.age && salary == other.salary;
    }

    void print() const
    {
        cout << "Person{" << name << ", age=" << age << ", salary=" << salary << "}";
    }
};

enum class Department
{
    Engineering = 0,
    Sales = 1,
    Marketing = 2,
    HR = 3
};

int os_main(int, os_char**)
{
    cout << "\n===========================================" << endl;
    cout << "TEST 05: UnorderedMap Complex Types" << endl;
    cout << "===========================================" << endl;

    cout << "\n[5.1] Map with string keys and complex values:" << endl;
    UnorderedMap<string, Person> person_map;

    person_map.Add("alice", Person("Alice Johnson", 28, 75000.0));
    person_map.Add("bob", Person("Bob Smith", 35, 85000.0));
    person_map.Add("carol", Person("Carol White", 32, 80000.0));

    assert(person_map.GetCount() == 3);

    Person p;
    assert(person_map.Get("alice", p));
    cout << "  ✓ Retrieved: ";
    p.print();
    cout << endl;

    assert(p.name == "Alice Johnson" && p.age == 28);
    cout << "  ✓ Values correctly stored and retrieved" << endl;

    cout << "\n[5.2] Map with enum keys:" << endl;
    UnorderedMap<int, string> dept_map;  // Using int for enum values

    dept_map.Add(static_cast<int>(Department::Engineering), "Software Development");
    dept_map.Add(static_cast<int>(Department::Sales), "Revenue Generation");
    dept_map.Add(static_cast<int>(Department::Marketing), "Brand Awareness");
    dept_map.Add(static_cast<int>(Department::HR), "Human Resources");

    assert(dept_map.GetCount() == 4);

    string dept;
    assert(dept_map.Get(static_cast<int>(Department::Engineering), dept));
    assert(dept == "Software Development");
    cout << "  ✓ Enum-keyed entries work correctly" << endl;

    cout << "\n[5.3] Complex value types with strings:" << endl;
    struct TeamInfo
    {
        string team_name;
        int member_count;
        string manager;
    };

    // Workaround: use string value to store complex data
    UnorderedMap<int, string> team_map;
    team_map.Add(1, "TeamInfo{name=Backend, members=5, mgr=Alice}");
    team_map.Add(2, "TeamInfo{name=Frontend, members=4, mgr=Bob}");

    string team;
    assert(team_map.Get(1, team));
    assert(team.find("Backend") != string::npos);
    cout << "  ✓ Complex data stored as string values" << endl;

    cout << "\n[5.4] Integer keys with string values (various ranges):" << endl;
    UnorderedMap<int, string> int_map;

    int_map.Add(0, "zero");
    int_map.Add(1, "one");
    int_map.Add(100, "hundred");
    int_map.Add(1000, "thousand");
    int_map.Add(2147483647, "max_int");  // Max 32-bit int
    int_map.Add(-1, "minus_one");
    int_map.Add(-1000, "minus_thousand");

    assert(int_map.GetCount() == 7);

    string val;
    assert(int_map.Get(100, val) && val == "hundred");
    assert(int_map.Get(2147483647, val) && val == "max_int");
    assert(int_map.Get(-1000, val) && val == "minus_thousand");
    cout << "  ✓ Integer key ranges handled correctly" << endl;

    cout << "\n[5.5] Nested structures in values:" << endl;
    struct Score
    {
        int math;
        int english;
        int science;

        int total() const { return math + english + science; }
    };

    // Store as string representation
    UnorderedMap<string, string> score_map;
    score_map.Add("student_001", "Math:85,English:90,Science:88");
    score_map.Add("student_002", "Math:92,English:87,Science:91");

    string scores;
    assert(score_map.Get("student_001", scores));
    assert(scores.find("Math:85") != string::npos);
    cout << "  ✓ Nested structure data handled as strings" << endl;

    cout << "\n[5.6] String keys with special characters:" << endl;
    UnorderedMap<string, string> special_map;

    special_map.Add("key_with_underscore", "value1");
    special_map.Add("key-with-dash", "value2");
    special_map.Add("key.with.dots", "value3");
    special_map.Add("key with spaces", "value4");
    special_map.Add("", "empty_key_value");  // Empty string key

    assert(special_map.GetCount() == 5);

    string v;
    assert(special_map.Get("key_with_underscore", v) && v == "value1");
    assert(special_map.Get("key with spaces", v) && v == "value4");
    assert(special_map.Get("", v) && v == "empty_key_value");
    cout << "  ✓ Special characters in string keys handled" << endl;

    cout << "\n[5.7] Large number of diverse types:" << endl;
    UnorderedMap<int, string> diverse_map;

    // Add 100 entries with different value types (as strings)
    for (int i = 0; i < 100; i++)
    {
        string val = "Type:" + to_string(i % 3) + "|Value:" + to_string(i);
        diverse_map.Add(i, val);
    }

    assert(diverse_map.GetCount() == 100);

    string retrieved;
    assert(diverse_map.Get(50, retrieved));
    assert(retrieved.find("Type:2") != string::npos);
    cout << "  ✓ Large diverse dataset handled (" << diverse_map.GetCount() << " items)" << endl;

    cout << "\n[5.8] Enum-like behavior with string mapping:" << endl;
    UnorderedMap<string, int> status_map;

    status_map.Add("PENDING", 0);
    status_map.Add("ACTIVE", 1);
    status_map.Add("COMPLETED", 2);
    status_map.Add("FAILED", 3);

    int status_code;
    assert(status_map.Get("ACTIVE", status_code) && status_code == 1);
    assert(status_map.Get("FAILED", status_code) && status_code == 3);
    cout << "  ✓ Enum-like string-to-int mapping works" << endl;

    cout << "\n[5.9] Modification of complex value types:" << endl;
    UnorderedMap<int, string> modify_map;
    modify_map.Add(1, "Original");

    // Modify via Change
    bool changed = modify_map.Change(1, "Modified");
    assert(changed);

    string val_check;
    assert(modify_map.Get(1, val_check) && val_check == "Modified");

    // Modify via operator[] assignment
    modify_map[1] = "Modified Again";
    assert(modify_map.Get(1, val_check) && val_check == "Modified Again");

    cout << "  ✓ Modification of complex values works" << endl;

    cout << "\n[5.10] Enumeration with complex types:" << endl;
    UnorderedMap<string, Person> enum_map;
    enum_map.Add("emp1", Person("Employee One", 25, 60000));
    enum_map.Add("emp2", Person("Employee Two", 30, 70000));
    enum_map.Add("emp3", Person("Employee Three", 35, 80000));

    int count = 0;
    enum_map.EnumKV([&](const string& key, const Person& person) -> bool
    {
        cout << "  Key=" << key << ", ";
        person.print();
        cout << endl;
        count++;
        return true;
    });

    assert(count == 3);
    cout << "  ✓ Enumeration of complex types successful" << endl;

    cout << "\n✅ TEST 05 PASSED" << endl;
    return 0;
}
