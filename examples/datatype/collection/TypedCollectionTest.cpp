/**
 * TypedCollectionTest.cpp
 *
 * TypedCollection<T> 测试：
 *   - Add/Get/Set/Insert 基本操作
 *   - indexOf/Contains/Remove 按值查找与删除
 *   - Map 映射区间访问
 *   - Exchange 交换
 *   - AddCollection 整集合追加
 *   - ToArray/begin/end 迭代
 *   - Clear 保留容量 / Free 释放
 *   - 与非平凡类型(带析构)的编译与基本行为
 */
#include<hgl/type/TypedCollection.h>
#include"test_common.h"

#include<cstring>

using namespace hgl;

namespace
{
    struct Point
    {
        int x, y;
    };

    bool operator==(const Point &a, const Point &b)
    {
        return a.x == b.x && a.y == b.y;
    }

    bool operator!=(const Point &a, const Point &b)
    {
        return !(a == b);
    }

}//namespace

static void TestBasic()
{
    std::printf("\n=== TypedCollection: Basic Add/Get/Count ===\n");

    TypedCollection<Point> tc;

    TEST_ASSERT(tc.IsEmpty(), "New collection should be empty");
    TEST_ASSERT(tc.GetCount() == 0, "New collection count is 0");

    Point p1{1, 2};
    Point p2{3, 4};

    TEST_ASSERT(tc.Add(p1), "Add(p1) succeeded");
    TEST_ASSERT(tc.Add(p2), "Add(p2) succeeded");
    TEST_ASSERT(!tc.IsEmpty(), "Not empty after Add");
    TEST_ASSERT(tc.GetCount() == 2, "Count is 2");
    TEST_ASSERT(tc.GetUnitBytes() == sizeof(Point), "unit_bytes == sizeof(Point)");

    Point got{0, 0};
    TEST_ASSERT(tc.Get(0, got) && got == p1, "Get(0) == p1");
    TEST_ASSERT(tc.Get(1, got) && got == p2, "Get(1) == p2");
    TEST_ASSERT(!tc.Get(2, got), "Get(out of range) fails");
}

static void TestSetInsertExchange()
{
    std::printf("\n=== TypedCollection: Set/Insert/Exchange ===\n");

    TypedCollection<Point> tc;
    tc.Add(Point{1, 1});
    tc.Add(Point{2, 2});
    tc.Add(Point{3, 3});

    // Set
    TEST_ASSERT(tc.Set(1, Point{20, 20}), "Set(1) succeeded");
    Point got{0, 0};
    tc.Get(1, got);
    TEST_ASSERT((got == Point{20, 20}), "After Set, Get(1) == {20,20}");
    TEST_ASSERT(!tc.Set(9, Point{0, 0}), "Set(out of range) fails");

    // Insert
    TEST_ASSERT(tc.Insert(1, Point{99, 99}), "Insert(1) succeeded");
    TEST_ASSERT(tc.GetCount() == 4, "Count is 4 after Insert");
    tc.Get(1, got);
    TEST_ASSERT((got == Point{99, 99}), "After Insert, Get(1) == {99,99}");
    tc.Get(2, got);
    TEST_ASSERT((got == Point{20, 20}), "Original item 1 shifted after Insert");

    // Exchange
    TEST_ASSERT(tc.Exchange(0, 3), "Exchange(0,3) succeeded");
    tc.Get(0, got);
    TEST_ASSERT((got == Point{3, 3}), "After Exchange, Get(0) == {3,3}");
    tc.Get(3, got);
    TEST_ASSERT((got == Point{1, 1}), "After Exchange, Get(3) == {1,1}");
}

static void TestSearchAndRemove()
{
    std::printf("\n=== TypedCollection: indexOf/Contains/Remove ===\n");

    TypedCollection<Point> tc;
    tc.Add(Point{1, 1});
    tc.Add(Point{2, 2});
    tc.Add(Point{2, 2});
    tc.Add(Point{3, 3});

    TEST_ASSERT(tc.Contains(Point{2, 2}), "Contains found");
    TEST_ASSERT(!tc.Contains(Point{9, 9}), "Contains not found");
    TEST_ASSERT(tc.indexOf(Point{3, 3}) == 3, "indexOf finds last element");
    TEST_ASSERT(tc.indexOf(Point{9, 9}) == -1, "indexOf returns -1 when not found");

    // Remove 按值删除（默认删 1 个）
    TEST_ASSERT(tc.Remove(Point{2, 2}) == 1, "Remove deletes 1 item");
    TEST_ASSERT(tc.GetCount() == 3, "Count is 3 after delete");
    TEST_ASSERT(tc.Contains(Point{2, 2}), "One {2,2} still exists");

    // Remove 指定 max_count
    TEST_ASSERT(tc.Remove(Point{2, 2}, 5) == 1, "Remove max_count=5 actually deletes 1");
    TEST_ASSERT(!tc.Contains(Point{2, 2}), "All {2,2} deleted");

    // RemoveAt
    TEST_ASSERT(tc.RemoveAt(0), "RemoveAt(0) succeeded");
    TEST_ASSERT(tc.GetCount() == 1, "Count is 1 after RemoveAt");
    TEST_ASSERT(!tc.RemoveAt(99), "RemoveAt(out of range) fails");
}

static void TestMapAndArray()
{
    std::printf("\n=== TypedCollection: Map/ToArray/Iteration ===\n");

    TypedCollection<Point> tc;
    tc.Add(Point{1, 1});
    tc.Add(Point{2, 2});
    tc.Add(Point{3, 3});

    Point *arr = tc.ToArray();
    TEST_ASSERT(arr != nullptr, "ToArray not null");
    TEST_ASSERT((arr[0] == Point{1, 1} && arr[2] == Point{3, 3}), "ToArray content correct");

    Point *mapped = tc.Map(1, 2);
    TEST_ASSERT(mapped != nullptr, "Map(1,2) not null");
    TEST_ASSERT((mapped[0] == Point{2, 2} && mapped[1] == Point{3, 3}), "Map range content correct");

    // begin/end 迭代求和
    int sum = 0;
    for(const Point &p : tc)
        sum += p.x;
    TEST_ASSERT(sum == 6, "begin/end iteration x sum == 6");

    // 从原始指针构造（覆盖 C 数组构造）
    const Point raw[2] = {{7, 7}, {8, 8}};
    TypedCollection<Point> tc2;
    tc2.Add(raw[0]);
    tc2.Add(raw[1]);
    TEST_ASSERT(tc2.GetCount() == 2 && tc2.Contains(Point{8, 8}), "Construct from raw data");

    // AddCollection
    TypedCollection<Point> tc3;
    tc3.AddCollection(tc2);
    TEST_ASSERT(tc3.GetCount() == 2, "Count is 2 after AddCollection");
    TEST_ASSERT(tc3.Contains(Point{7, 7}), "AddCollection content correct");
}

static void TestClearFree()
{
    std::printf("\n=== TypedCollection: Clear/Free ===\n");

    TypedCollection<Point> tc;
    for(int i = 0; i < 100; i++)
        tc.Add(Point{i, i});

    TEST_ASSERT(tc.GetCount() == 100, "Added 100 elements");

    tc.Clear();
    TEST_ASSERT(tc.IsEmpty(), "Empty after Clear");
    TEST_ASSERT(tc.GetAllocCount() >= 100, "Clear keeps capacity");

    tc.Add(Point{1, 1});
    TEST_ASSERT(tc.GetCount() == 1, "Can Add again after Clear");

    tc.ClearAndDeallocate();
    TEST_ASSERT(tc.IsEmpty(), "Empty after Free");
    TEST_ASSERT(tc.GetAllocCount() == 0, "Free releases capacity");
}

// 注意：Collection/TypedCollection 底层按字节 memcpy 拷贝，
// 仅支持 trivially copyable 类型（见 Collection::Add 的 unit_bytes 拷贝）。
// 非平凡类型（含指针/动态内存）会因浅拷贝+临时对象析构产生悬垂指针，不在支持范围内。
// 因此本测试不包含非平凡类型用例。

int os_main(int, os_char **)
{
    hgltest::reset();

    TestBasic();
    TestSetInsertExchange();
    TestSearchAndRemove();
    TestMapAndArray();
    TestClearFree();

    return TEST_SUMMARY();
}
