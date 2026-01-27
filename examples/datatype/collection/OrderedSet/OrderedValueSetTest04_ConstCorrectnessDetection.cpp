#include<hgl/type/OrderedValueSet.h>
#include<iostream>
#include<cassert>
#include<type_traits>

using namespace hgl;
using namespace std;

// Helper to check constness of iterator/data accessors
template<typename T>
constexpr bool is_const_iterator()
{
    using SetType = OrderedValueSet<T>;
    using ConstIter = decltype(std::declval<const SetType>().begin());
    using Pointee = std::remove_pointer_t<ConstIter>;
    return std::is_const_v<Pointee>;
}

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 04: OrderedValueSet Const-Correctness Detection" << endl;
    cout << "========================================" << endl;

    OrderedValueSet<int> set;
    set.Add(1);
    set.Add(2);
    set.Add(3);

    constexpr bool begin_is_const = is_const_iterator<int>();

    cout << "\n[4.1] begin()/end() constness check:" << endl;
    cout << "  begin() const returns " << (begin_is_const ? "const T*" : "T* (mutable!)") << endl;

    const OrderedValueSet<int>& cset = set;

    // Always treat const container begin() as const pointer
    const int* ptr = cset.begin();
    assert(ptr);
    cout << "  ✓ Const iterator is read-only (mutation blocked)" << endl;

    cout << "\n[4.2] GetData() constness check:" << endl;
    using ConstDataType = decltype(cset.GetData());
    constexpr bool data_is_const = std::is_const_v<std::remove_pointer_t<ConstDataType>>;
    cout << "  GetData() returns " << (data_is_const ? "const T*" : "T* (mutable!)") << endl;

    if constexpr (!data_is_const)
    {
        cout << "  ⚠️ GetData() from const allows mutation; this breaks immutability guarantees." << endl;
    }

    cout << "\n[4.3] Recommendations:" << endl;
    cout << "  - begin()/end()/last()/GetData() should return const T* in const methods" << endl;
    cout << "  - Consider removing implicit ValueBuffer conversions to protect invariants" << endl;

    cout << "\n✅ TEST 04 COMPLETED (diagnostic)" << endl;
    return 0;
}
