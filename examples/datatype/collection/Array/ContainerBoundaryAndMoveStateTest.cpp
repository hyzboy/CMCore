/**
 * ContainerBoundaryAndMoveStateTest.cpp
 *
 * 覆盖容器层级的边界与后置状态（对应 hgl_type_fix_suggestions.md F25）：
 *   - 空容器上的 Pop/Peek/Delete/Insert/Move/Exchange
 *   - 越界下标（-1 / count / count+1）
 *   - Delete/Insert/Move 的单元素、整段、前移、后移
 *   - 拷贝/移动之后源对象的状态
 *   - size_t 字节数与 STL 风格 size()/empty()
 */
#include<hgl/platform/Platform.h>
#include<hgl/type/ValueArray.h>
#include<hgl/type/PtrArray.h>
#include<hgl/type/Queue.h>
#include<hgl/type/Stack.h>
#include"test_common.h"

#include<string>
#include<utility>

using namespace hgl;

namespace
{
    struct Item
    {
        static int alive;

        std::string text;

        explicit Item(const char *t = ""):text(t) { ++alive; }
        Item(const Item &o):text(o.text) { ++alive; }
        Item(Item &&o) noexcept:text(std::move(o.text)) { ++alive; }
        Item &operator=(const Item &o) { text = o.text; return *this; }
        Item &operator=(Item &&o) noexcept { text = std::move(o.text); return *this; }
        ~Item() { --alive; }

        bool operator==(const Item &o)const{return text == o.text;}
    };

    int Item::alive = 0;
}

static void TestValueArrayBoundaries()
{
    std::printf("\n=== ValueArray: boundaries and aliases ===\n");

    ValueArray<int> v;

    TEST_ASSERT(v.size() == 0 && v.empty(), "New array is empty");
    TEST_ASSERT(v.GetTotalBytes() == 0, "Empty array has zero bytes");
    TEST_ASSERT(v.Delete(0) == false, "Delete on empty fails");
    TEST_ASSERT(v.DeleteSwap(0) == false, "DeleteSwap on empty fails");
    TEST_ASSERT(v.Insert(0, 1) == true, "Insert at 0 works on empty");
    TEST_ASSERT(v.GetCount() == 1 && v[0] == 1, "Inserted single element");

    v.Clear();

    for(int i = 0; i < 5; ++i)
        v.Add(i * 10);

    TEST_ASSERT(v.GetCount() == 5, "5 elements");
    TEST_ASSERT(v.GetTotalBytes() == 5 * sizeof(int), "GetTotalBytes counts all elements");
    TEST_ASSERT(v.size() == 5 && !v.empty(), "size()/empty() agree with GetCount()");

    TEST_ASSERT(v.Delete(-1) == false, "Negative index rejected");
    TEST_ASSERT(v.Delete(5) == false, "count index rejected (nothing to delete)");
    TEST_ASSERT(v.Delete(4) == true, "Last element deletable");
    TEST_ASSERT(v.GetCount() == 4, "Count decremented");

    TEST_ASSERT(v.Insert(-1, 1) == false, "Insert negative index rejected");
    TEST_ASSERT(v.Insert(99, 1) == false, "Insert past end rejected");
    TEST_ASSERT(v.Insert(4, 40) == true, "Insert at end allowed");

    TEST_ASSERT(v.At(-1) == nullptr && v.At(v.GetCount()) == nullptr, "At out of range returns null");

    // Move：空容器与非法参数不应改变内容
    ValueArray<int> empty_array;
    empty_array.Move(0, 0, 1);
    TEST_ASSERT(empty_array.GetCount() == 0, "Move on empty array is a no-op");

    // Exchange 越界不改变内容
    int before = v[0];
    v.Swap(-1, 100);
    TEST_ASSERT(v[0] == before, "Exchange with invalid indices is a no-op");

    TEST_ASSERT(Item::alive == 0, "No Item leaks in ValueArray test");
}

static void TestPtrArrayBoundariesAndMoveState()
{
    std::printf("\n=== PtrArray: boundaries and move state ===\n");

    {
        PtrArray<Item> arr;

        TEST_ASSERT(arr.size() == 0 && arr.empty(), "New PtrArray is empty");
        TEST_ASSERT(arr.GetTotalBytes() == 0 && arr.GetAllocBytes() == 0, "Empty byte counts are zero");
        TEST_ASSERT(!arr.Delete(0), "Delete on empty fails");
        TEST_ASSERT(!arr.DeleteSwap(0), "DeleteSwap on empty fails");
        TEST_ASSERT(arr.At(0) == nullptr, "At on empty is null");

        arr.Add(Item("a"));
        arr.Add(Item("b"));
        arr.Add(Item("c"));

        TEST_ASSERT(arr.size() == 3, "size() reports 3");
        TEST_ASSERT(arr.GetTotalBytes() == 3 * sizeof(Item), "GetTotalBytes matches count");

        TEST_ASSERT(!arr.Delete(-1), "Negative delete rejected");
        TEST_ASSERT(!arr.Delete(5), "Out-of-range delete rejected");

        // Insert 的位置参数会被裁剪到 [0, count]（与 ValueArray 的"拒绝"策略不同，保持既有行为）
        TEST_ASSERT(arr.Insert(-1, Item("x")), "Insert clamps a negative position to 0");
        TEST_ASSERT(arr.GetCount() == 4 && arr[0] == Item("x"), "Clamped insert went to the front");

        TEST_ASSERT(arr.Insert(3, Item("d")), "Insert in the middle allowed");
        TEST_ASSERT(arr.GetCount() == 5 && arr[3] == Item("d") && arr[4] == Item("c"),
                    "Middle insert keeps the tail order");

        arr.Move(0, 3, 1);
        TEST_ASSERT(arr[0] == Item("d"), "Move to front");

        // Resize 缩小后，被移除元素必须被析构
        const int alive_before = Item::alive;
        arr.Resize(2);
        TEST_ASSERT(arr.GetCount() == 2, "Resize shrinks");
        TEST_ASSERT(Item::alive == alive_before - 3, "Shrunk elements were destroyed");
    }

    TEST_ASSERT(Item::alive == 0, "No Item leaks in PtrArray test");

    // 移动之后源对象必须为空且可继续使用
    {
        PtrArray<Item> src;
        src.Add(Item("keep"));

        PtrArray<Item> dst(std::move(src));
        TEST_ASSERT(src.GetCount() == 0 && src.IsEmpty(), "Moved-from source is empty");
        TEST_ASSERT(src.GetData() == nullptr, "Moved-from source has no data");
        TEST_ASSERT(dst.GetCount() == 1 && dst[0] == Item("keep"), "Destination got the data");

        src.Add(Item("again"));
        TEST_ASSERT(src.GetCount() == 1, "Moved-from source is reusable");

        PtrArray<Item> assigned;
        assigned.Add(Item("old"));
        assigned = std::move(dst);
        TEST_ASSERT(assigned.GetCount() == 1 && assigned[0] == Item("keep"), "Move assignment replaces content");
        TEST_ASSERT(dst.GetCount() == 0, "Move-assigned source is empty");
    }

    TEST_ASSERT(Item::alive == 0, "No Item leaks after move tests");
}

static void TestQueueBoundaries()
{
    std::printf("\n=== Queue: boundaries ===\n");

    Queue<int> q;

    int value = -1;
    TEST_ASSERT(q.empty() && q.size() == 0, "New queue is empty");
    TEST_ASSERT(!q.Peek(value), "Peek on empty fails");
    TEST_ASSERT(!q.Pop(value), "Pop on empty fails");

    TEST_ASSERT(!q.Push((const int *)nullptr, 1), "Push null batch fails");
    TEST_ASSERT(!q.Push((const int *)nullptr, 0), "Push zero count fails");

    q.Push(1);
    TEST_ASSERT(q.size() == 1 && !q.empty(), "size()/empty() after push");

    TEST_ASSERT(q.Peek(value) && value == 1, "Peek returns front without popping");
    TEST_ASSERT(q.GetCount() == 1, "Peek does not consume");

    TEST_ASSERT(q.Pop(value) && value == 1, "Pop returns front");
    TEST_ASSERT(q.empty(), "Queue empty after pop");
    TEST_ASSERT(!q.Pop(value), "Second pop on empty fails");
}

static void TestStackBoundaries()
{
    std::printf("\n=== Stack: boundaries ===\n");

    Stack<int> s;

    int value = -1;
    TEST_ASSERT(s.empty() && s.size() == 0, "New stack is empty");
    TEST_ASSERT(!s.Pop(value), "Pop on empty fails");
    TEST_ASSERT(!s.Top(value), "Top on empty fails");
    TEST_ASSERT(!s.Bottom(value), "Bottom on empty fails");
    TEST_ASSERT(!s.GetAt(0, value), "GetAt(0) on empty fails");
    TEST_ASSERT(!s.GetAt(-1, value), "GetAt(-1) fails");

    TEST_ASSERT(s.Push(1) && s.Push(2) && s.Push(3), "Push three values");
    TEST_ASSERT(s.size() == 3, "size() is 3");

    TEST_ASSERT(s.GetAt(-1, value) == false, "GetAt(-1) still fails");
    TEST_ASSERT(s.GetAt(3, value) == false, "GetAt(count) fails");
    TEST_ASSERT(s.GetAt(0, value) && value == 1, "GetAt(0) returns bottom");

    TEST_ASSERT(!s.Push((const int *)nullptr, 1), "Push null batch fails");
    TEST_ASSERT(!s.Pop((int *)nullptr, 1), "Pop null batch fails");
    TEST_ASSERT(!s.Pop((int *)nullptr, 99), "Pop oversized batch fails");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestValueArrayBoundaries();
    TestPtrArrayBoundariesAndMoveState();
    TestQueueBoundaries();
    TestStackBoundaries();

    return TEST_SUMMARY();
}
