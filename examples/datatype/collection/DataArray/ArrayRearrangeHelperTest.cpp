#include <hgl/type/ArrayRearrangeHelper.h>
#include <hgl/type/MemoryUtil.h>
#include <array>
#include <iostream>
#include <new>
#include <stdexcept>

#define CHECK(cond, msg) do { if(!(cond)) { throw std::runtime_error(msg); } } while(0)

struct Tracker
{
    static int constructed;
    static int destroyed;
    static int moved;

    int value;

    explicit Tracker(int v = 0) : value(v) { ++constructed; }
    Tracker(Tracker&& other) noexcept : value(other.value) { other.value = -999; ++constructed; ++moved; }
    Tracker(const Tracker&) = delete;
    Tracker& operator=(const Tracker&) = delete;
    Tracker& operator=(Tracker&& other) noexcept { value = other.value; other.value = -999; ++moved; return *this; }
    ~Tracker() { ++destroyed; }

    static void Reset() { constructed = destroyed = moved = 0; }
    static int Alive() { return constructed - destroyed; }
};

int Tracker::constructed = 0;
int Tracker::destroyed = 0;
int Tracker::moved = 0;

void TestTrivialRearrange()
{
    constexpr int count = 5;
    const int src[count] = {1, 2, 3, 4, 5};
    int* dest = hgl::alloc_raw<int>(count);

    const bool ok = hgl::ArrayRearrange(dest, src, count, {2, 3}, {1, 0});
    CHECK(ok, "trivial rearrange failed");

    const int expected[count] = {3, 4, 5, 1, 2};
    for(int i = 0; i < count; ++i)
        CHECK(dest[i] == expected[i], "trivial rearrange result mismatch");

    hgl::free_raw(dest);
}

void TestNonTrivialRearrange()
{
    Tracker::Reset();
    constexpr int count = 5;
    Tracker src[count] = {Tracker{0}, Tracker{1}, Tracker{2}, Tracker{3}, Tracker{4}};

    Tracker* dest = hgl::alloc_raw<Tracker>(count);

    const bool ok = hgl::ArrayRearrange(dest, src, count, {1, 2, 2}, {0, 2, 1});
    CHECK(ok, "non-trivial rearrange failed");

    const int expected[count] = {0, 3, 4, 1, 2};
    for(int i = 0; i < count; ++i)
        CHECK(dest[i].value == expected[i], "non-trivial rearrange result mismatch");

    // After constructing dest, alive objects include both src and dest
    CHECK(Tracker::Alive() == 10, "alive count after rearrange unexpected");

    hgl::destroy_range(dest, count);
    hgl::free_raw(dest);
    CHECK(Tracker::Alive() == 5, "alive count after destroying dest unexpected");

    // src will be destroyed when leaving scope; final Alive should be zero
}

void TestInvalidIndexEarlyExit()
{
    Tracker::Reset();
    constexpr int count = 3;
    Tracker src[count] = {Tracker{10}, Tracker{11}, Tracker{12}};

    Tracker* dest = hgl::alloc_raw<Tracker>(count);
    const bool ok = hgl::ArrayRearrange(dest, src, count, {1, 1}, {-1});
    CHECK(!ok, "invalid index should fail");

    // No objects were constructed into dest, so only src are alive
    CHECK(Tracker::Alive() == 3, "alive count should reflect only source objects");

    hgl::free_raw(dest);
    // src will destroy on scope exit
}

int main()
{
    try
    {
        TestTrivialRearrange();
        TestNonTrivialRearrange();
        TestInvalidIndexEarlyExit();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "All ArrayRearrangeHelper tests passed" << std::endl;
    return 0;
}
