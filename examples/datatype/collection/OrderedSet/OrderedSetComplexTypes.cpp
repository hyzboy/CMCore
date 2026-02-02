#include<hgl/type/OrderedSet.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

struct Person
{
    string name;
    int age;

    Person() : age(0) {}
    Person(const string& n, int a) : name(n), age(a) {}

    bool operator<(const Person& other) const
    {
        return age < other.age;
    }

    bool operator==(const Person& other) const
    {
        return age == other.age && name == other.name;
    }
};

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 07: OrderedSet<T> Complex Types" << endl;
    cout << "========================================" << endl;

    OrderedSet<string> str_set;

    cout << "\n[7.1] OrderedSet with strings:" << endl;
    str_set.Add("zebra");
    str_set.Add("apple");
    str_set.Add("banana");
    assert(str_set.GetCount() == 3);

    string first;
    assert(str_set.GetFirst(first));
    assert(first == "apple");
    cout << "  ✓ Strings are sorted: " << first << " (first)" << endl;

    string last;
    assert(str_set.GetLast(last));
    assert(last == "zebra");
    cout << "  ✓ Last element: " << last << endl;

    assert(str_set.Contains("banana"));
    assert(!str_set.Contains("cherry"));
    cout << "  ✓ String contains/find works" << endl;

    cout << "\n[7.2] OrderedSet with struct (Person):" << endl;
    OrderedSet<Person> person_set;

    person_set.Add(Person("Charlie", 30));
    person_set.Add(Person("Alice", 25));
    person_set.Add(Person("Bob", 28));
    assert(person_set.GetCount() == 3);
    cout << "  ✓ Added 3 persons" << endl;

    cout << "\n[7.3] Person set ordering (by age):" << endl;
    Person p;
    assert(person_set.Get(0, p));
    assert(p.age == 25);
    cout << "  First (youngest): " << p.name << " (age " << p.age << ")" << endl;

    assert(person_set.Get(1, p));
    assert(p.age == 28);
    cout << "  Second: " << p.name << " (age " << p.age << ")" << endl;

    assert(person_set.Get(2, p));
    assert(p.age == 30);
    cout << "  Third (oldest): " << p.name << " (age " << p.age << ")" << endl;
    cout << "  ✓ Persons correctly ordered by age" << endl;

    cout << "\n[7.4] Find in person set:" << endl;
    Person target("Alice", 25);
    auto it = person_set.Find(target);
    assert(it != person_set.end());
    cout << "  ✓ Found person: " << it->name << endl;

    cout << "\n[7.5] Duplicate person (same age):" << endl;
    Person alice_dup("Alice2", 25);  // Same age, different name
    int64 dup_result = person_set.Add(alice_dup);
    assert(dup_result == -1);  // Should be rejected as duplicate by age
    assert(person_set.GetCount() == 3);
    cout << "  ✓ Duplicate person (by age) correctly rejected" << endl;

    cout << "\n[7.6] Delete person and verify:" << endl;
    person_set.Delete(Person("Bob", 28));
    assert(person_set.GetCount() == 2);
    assert(!person_set.Contains(Person("Bob", 28)));
    cout << "  ✓ Person successfully deleted" << endl;

    cout << "\n[7.7] OrderedSet with double:" << endl;
    OrderedSet<double> double_set;
    double_set.Add(3.14);
    double_set.Add(1.41);
    double_set.Add(2.71);

    double d;
    assert(double_set.GetFirst(d));
    assert(d > 1.4 && d < 1.5);
    cout << "  ✓ Double values work: first = " << d << endl;

    cout << "\n✅ TEST 07 PASSED" << endl;
    return 0;
}
