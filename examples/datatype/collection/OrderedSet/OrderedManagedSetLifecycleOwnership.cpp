#include<hgl/type/OrderedManagedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

struct Tracked
{
    static int alive;
    int value;

    Tracked(int v = 0) : value(v) { ++alive; }
    Tracked(const Tracked& other) : value(other.value) { ++alive; }
    Tracked(Tracked&& other) noexcept : value(other.value) { ++alive; other.value = -1; }
    Tracked& operator=(const Tracked& other) { value = other.value; return *this; }
    Tracked& operator=(Tracked&& other) noexcept { value = other.value; other.value = -1; return *this; }
    ~Tracked() { --alive; }

    bool operator<(const Tracked& rhs) const { return value < rhs.value; }
    bool operator==(const Tracked& rhs) const { return value == rhs.value; }
};

int Tracked::alive = 0;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 05: OrderedSet Lifecycle & Ownership" << endl;
    cout << "========================================" << endl;

    Tracked::alive = 0;
    {
        OrderedManagedSet<Tracked> set;

        cout << "\n[5.1] Add objects (auto-sorted, unique):" << endl;
        Tracked a(10), b(5), c(15);
        set.Add(a);
        set.Add(b);
        set.Add(c);
        assert(set.GetCount() == 3);
        cout << "  Alive after add: " << Tracked::alive << endl;

        cout << "\n[5.2] Duplicate add should be rejected:" << endl;
        int64 dup = set.Add(a);
        assert(dup == -1);
        assert(set.GetCount() == 3);
        cout << "  ✓ Duplicate not inserted" << endl;

        cout << "\n[5.3] Delete by value:" << endl;
        int alive_before = Tracked::alive;
        assert(set.Delete(b));
        assert(set.GetCount() == 2);
        cout << "  Alive delta after delete: " << (Tracked::alive - alive_before) << endl;

        cout << "\n[5.4] Clear releases contained objects but keeps capacity:" << endl;
        alive_before = Tracked::alive;
        set.Clear();
        assert(set.GetCount() == 0);
        cout << "  Alive delta after Clear(): " << (Tracked::alive - alive_before) << endl;
        int64 alloc_after_clear = set.GetAllocCount();
        cout << "  Capacity after Clear(): " << alloc_after_clear << endl;

        cout << "\n[5.5] Free releases capacity as well:" << endl;
        set.Free();
        cout << "  Capacity after Free(): " << set.GetAllocCount() << endl;
    }

    cout << "\n[5.6] After scope exit, all objects destroyed:" << endl;
    assert(Tracked::alive == 0);
    cout << "  ✓ No leaks or double frees detected" << endl;

    cout << "\n✅ TEST 05 PASSED" << endl;
    return 0;
}
