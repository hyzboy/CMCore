/**
 * PtrArrayTest.cpp
 *
 * PtrArray<T> 非平凡类型阵列容器测试：
 *   - 构造（默认/容量/初始化列表/原始数组/移动）
 *   - 增删（Add/Append/Insert/Delete/DeleteShift/Move/Exchange）
 *   - 访问（operator[]/At/ReadAt/WriteAt）
 *   - 容量（Reserve/Resize/Expand）
 *   - 查找（Find/Contains）
 *   - 生命周期：构造/析构配对，无泄漏无双重释放
 *   - 拷贝被禁止（编译期保证）
 */
#include<hgl/platform/Platform.h>
#include<hgl/type/PtrArray.h>
#include"test_common.h"

#include<cstring>
#include<stdexcept>
#include<utility>

using namespace hgl;

namespace
{
    struct Tracked
    {
        static int alive;

        int value;

        explicit Tracked(int v = 0):value(v) { ++alive; }
        Tracked(const Tracked &o):value(o.value) { ++alive; }
        Tracked(Tracked &&o) noexcept:value(o.value) { ++alive; }
        Tracked &operator=(const Tracked &o)
        {
            value = o.value;
            return *this;
        }
        Tracked &operator=(Tracked &&o) noexcept
        {
            value = o.value;
            return *this;
        }
        ~Tracked() { --alive; }

        bool operator==(const Tracked &o)const{return value == o.value;}
        bool operator!=(const Tracked &o)const{return value != o.value;}
    };

    int Tracked::alive = 0;

    // 用于验证 reallocate_and_move 的异常安全：
    // 拷贝构造可能抛异常，移动构造不是 noexcept（因此走拷贝分支）
    struct ThrowingCopy
    {
        static int alive;
        static int copy_budget;     // 剩余允许的成功拷贝次数，<0 表示不抛异常

        int value;

        explicit ThrowingCopy(int v = 0):value(v) { ++alive; }

        ThrowingCopy(const ThrowingCopy &o):value(o.value)
        {
            if(copy_budget == 0)
                throw std::runtime_error("ThrowingCopy: copy budget exhausted");

            if(copy_budget > 0)
                --copy_budget;

            ++alive;
        }

        ThrowingCopy(ThrowingCopy &&o) noexcept(false):value(o.value) { ++alive; }

        ThrowingCopy &operator=(const ThrowingCopy &o) { value = o.value; return *this; }
        ThrowingCopy &operator=(ThrowingCopy &&o) noexcept { value = o.value; return *this; }

        ~ThrowingCopy() { --alive; }

        bool operator==(const ThrowingCopy &o)const{return value == o.value;}
        bool operator!=(const ThrowingCopy &o)const{return value != o.value;}
    };

    int ThrowingCopy::alive = 0;
    int ThrowingCopy::copy_budget = -1;

    // 非平凡拷贝、但可平凡重定位（平凡移动构造 + 平凡析构）→ 走 memmove 快路径
    struct Reloc
    {
        int value;

        explicit Reloc(int v = 0):value(v) {}
        Reloc(const Reloc &o):value(o.value) {}          // 用户提供拷贝构造 ⇒ 非 trivially copyable
        Reloc(Reloc &&) = default;                       // 平凡移动构造
        Reloc &operator=(const Reloc &) = default;
        Reloc &operator=(Reloc &&) = default;
        ~Reloc() = default;                              // 平凡析构

        bool operator==(const Reloc &o)const{return value == o.value;}
        bool operator!=(const Reloc &o)const{return value != o.value;}
    };
}//namespace

static void TestConstruction()
{
    std::printf("\n=== PtrArray: Construction ===\n");

    TEST_ASSERT(Tracked::alive == 0, "No objects alive initially");

    {
        PtrArray<Tracked> arr;
        TEST_ASSERT(arr.IsEmpty() && arr.GetCount() == 0, "Default construction is empty");

        PtrArray<Tracked> sized(3);
        TEST_ASSERT(sized.GetCount() == 3, "Size constructor creates 3 default elements");

        PtrArray<Tracked> init_list{Tracked(1), Tracked(2), Tracked(3)};
        TEST_ASSERT(init_list.GetCount() == 3, "Initializer list construction");
        TEST_ASSERT(init_list[0] == Tracked(1) && init_list[2] == Tracked(3), "Content correct");

        const Tracked raw[2] = {Tracked(7), Tracked(8)};
        PtrArray<Tracked> from_raw(raw, 2);
        TEST_ASSERT(from_raw.GetCount() == 2 && from_raw[1] == Tracked(8), "Construct from raw array");

        // 移动构造
        PtrArray<Tracked> moved(std::move(from_raw));
        TEST_ASSERT(moved.GetCount() == 2, "Count correct after move construction");
        TEST_ASSERT(from_raw.GetCount() == 0, "Source empty after move");
    }

    TEST_ASSERT(Tracked::alive == 0, "All destroyed, no leak");
}

static void TestAddAppend()
{
    std::printf("\n=== PtrArray: Add/Append ===\n");

    PtrArray<Tracked> arr;

    TEST_ASSERT(arr.Add(Tracked(10)) == 0, "Add returns index 0");
    TEST_ASSERT(arr.Add(Tracked(20)) == 1, "Add returns index 1");
    TEST_ASSERT(arr.GetCount() == 2, "Count is 2");

    arr.Append(Tracked(30));
    TEST_ASSERT(arr.GetCount() == 3 && arr[2] == Tracked(30), "Append adds item");

    // 批量添加
    const Tracked batch[2] = {Tracked(40), Tracked(50)};
    TEST_ASSERT(arr.Add(batch, 2) == 3, "Bulk Add returns start index");
    TEST_ASSERT(arr.GetCount() == 5, "Count is 5 after bulk add");
    TEST_ASSERT(arr[3] == Tracked(40) && arr[4] == Tracked(50), "Bulk content correct");

    // Add 空指针
    TEST_ASSERT(arr.Add((const Tracked *)nullptr, 2) == -1, "Bulk Add(nullptr) fails");

    TEST_ASSERT(arr.GetAllocCount() >= arr.GetCount(), "Capacity sufficient");
}

static void TestAccess()
{
    std::printf("\n=== PtrArray: Access ===\n");

    PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3)};

    TEST_ASSERT(arr[1] == Tracked(2), "operator[]");
    arr[1] = Tracked(22);
    TEST_ASSERT(arr[1] == Tracked(22), "operator[] write");

    Tracked *p = arr.At(2);
    TEST_ASSERT(p != nullptr && *p == Tracked(3), "At returns pointer");
    TEST_ASSERT(arr.At(9) == nullptr, "At out of range returns null");

    Tracked out(0);
    TEST_ASSERT(arr.ReadAt(out, 0) && out == Tracked(1), "ReadAt");
    TEST_ASSERT(!arr.ReadAt(out, 9), "ReadAt out of range fails");
    TEST_ASSERT(arr.WriteAt(Tracked(11), 0), "WriteAt");
    TEST_ASSERT(arr[0] == Tracked(11), "WriteAt effective");

    // begin/end
    int sum = 0;
    for(const Tracked &t : arr)
        sum += t.value;
    TEST_ASSERT(sum == 11 + 22 + 3, "Range iteration");
}

static void TestInsertDelete()
{
    std::printf("\n=== PtrArray: Insert/Delete ===\n");

    PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3)};

    // Insert
    TEST_ASSERT(arr.Insert(1, Tracked(9)), "Insert succeeded");
    TEST_ASSERT(arr.GetCount() == 4, "Count is 4 after Insert");
    TEST_ASSERT(arr[0] == Tracked(1) && arr[1] == Tracked(9) && arr[2] == Tracked(2), "Order correct after Insert");
    // 越界 pos 被钳制为追加语义
    TEST_ASSERT(arr.Insert(99, Tracked(0)), "Out-of-range Insert clamps to append");
    TEST_ASSERT(arr.GetCount() == 5 && arr[4] == Tracked(0), "Out-of-range Insert appends at end");

    // Delete（当前数组 {1,9,2,3,0}）
    TEST_ASSERT(arr.Delete(1), "Delete succeeded");
    TEST_ASSERT(arr.GetCount() == 4, "Count is 4 after Delete");
    TEST_ASSERT(arr[1] == Tracked(2), "Elements shift forward after Delete");

    // DeleteShift（当前数组 {1,2,3,0}）
    TEST_ASSERT(arr.DeleteShift(1), "DeleteShift succeeded");
    TEST_ASSERT(arr.GetCount() == 3, "Count is 3 after DeleteShift");
    TEST_ASSERT(arr[1] == Tracked(3), "Elements shift forward after DeleteShift");

    // 批量 Delete
    PtrArray<Tracked> arr2{Tracked(1), Tracked(2), Tracked(3), Tracked(4), Tracked(5)};
    TEST_ASSERT(arr2.Delete(1, 3), "Bulk Delete(1,3)");
    TEST_ASSERT(arr2.GetCount() == 2, "Count is 2 after bulk delete");
    TEST_ASSERT(arr2[0] == Tracked(1) && arr2[1] == Tracked(5), "Bulk delete keeps first and last");
}

static void TestMoveExchange()
{
    std::printf("\n=== PtrArray: Move/Exchange ===\n");

    PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4), Tracked(5)};

    // Exchange
    arr.Exchange(0, 4);
    TEST_ASSERT(arr[0] == Tracked(5) && arr[4] == Tracked(1), "Exchange swaps");

    // Move：把末尾元素(1)移到最前（np < old_pos 分支）
    // Exchange(0,4) 后 {5,2,3,4,1}；Move(0,4,1) 后 {1,5,2,3,4}
    arr.Move(0, 4, 1);
    TEST_ASSERT(arr[0] == Tracked(1) && arr[1] == Tracked(5) && arr[4] == Tracked(4), "Order correct after Move");
    TEST_ASSERT(arr.GetCount() == 5, "Move does not change count");
}

static void TestMoveSemantics()
{    std::printf("\n=== PtrArray: Find and Capacity ===\n");
    std::printf("\n=== PtrArray: Move semantics (forward/backward/boundary) ===\n");

    // 前移单元素（np > old_pos）：{1,2,3,4,5} Move(4,1,1) => {1,3,4,2,5}
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4), Tracked(5)};
        arr.Move(4, 1, 1);
        TEST_ASSERT(arr.GetCount() == 5, "Forward move keeps count");
        TEST_ASSERT(arr[0] == Tracked(1) && arr[1] == Tracked(3) && arr[2] == Tracked(4)
                    && arr[3] == Tracked(2) && arr[4] == Tracked(5), "Forward single move");
    }

    // 后移整段（np < old_pos）：{1,2,3,4,5} Move(0,3,2) => {4,5,1,2,3}
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4), Tracked(5)};
        arr.Move(0, 3, 2);
        TEST_ASSERT(arr[0] == Tracked(4) && arr[1] == Tracked(5) && arr[2] == Tracked(1)
                    && arr[3] == Tracked(2) && arr[4] == Tracked(3), "Backward block move");
    }

    // 移动到末尾：{1,2,3,4,5} Move(5,0,2) => {3,4,5,1,2}
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4), Tracked(5)};
        arr.Move(5, 0, 2);
        TEST_ASSERT(arr[0] == Tracked(3) && arr[1] == Tracked(4) && arr[2] == Tracked(5)
                    && arr[3] == Tracked(1) && arr[4] == Tracked(2), "Move block to end");
    }

    // 目标落在源区间内：不移动
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4), Tracked(5)};
        arr.Move(2, 1, 3);
        TEST_ASSERT(arr[0] == Tracked(1) && arr[1] == Tracked(2) && arr[2] == Tracked(3)
                    && arr[3] == Tracked(4) && arr[4] == Tracked(5), "Target inside source range is a no-op");
    }

    // 越界参数：move_count 超出尾部时被裁剪
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4), Tracked(5)};
        arr.Move(0, 4, 10);
        TEST_ASSERT(arr[0] == Tracked(5) && arr[1] == Tracked(1) && arr[4] == Tracked(4),
                    "Oversized move_count is clamped");
    }

    // 空数组与非法位置
    {
        PtrArray<Tracked> arr;
        arr.Move(0, 0, 1);
        TEST_ASSERT(arr.IsEmpty(), "Move on empty array is a no-op");

        PtrArray<Tracked> arr2{Tracked(1), Tracked(2)};
        arr2.Move(-5, 1, 1);                     // np 被裁剪到 0
        TEST_ASSERT(arr2[0] == Tracked(2) && arr2[1] == Tracked(1), "Negative new_pos is clamped to 0");
    }

    TEST_ASSERT(Tracked::alive == 0, "Move semantics leave no leaks");
}

static void TestReallocationExceptionSafety()
{
    std::printf("\n=== PtrArray: Reallocation exception safety ===\n");

    ThrowingCopy::alive = 0;
    ThrowingCopy::copy_budget = -1;

    {
        PtrArray<ThrowingCopy> arr;
        for(int i = 0; i < 8; ++i)                // 首次分配容量为 8
            arr.Add(ThrowingCopy(i));

        TEST_ASSERT(arr.GetCount() == 8, "8 elements before reallocation");
        TEST_ASSERT(ThrowingCopy::alive == 8, "8 objects alive");

        // 下一次追加会触发扩容；令第一个拷贝抛出
        ThrowingCopy::copy_budget = 0;

        bool threw = false;
        try
        {
            arr.Add(ThrowingCopy(99));
        }
        catch(const std::runtime_error &)
        {
            threw = true;
        }

        ThrowingCopy::copy_budget = -1;

        TEST_ASSERT(threw, "Reallocation propagated the copy exception");
        TEST_ASSERT(arr.GetCount() == 8, "Count unchanged after failed reallocation");
        TEST_ASSERT(ThrowingCopy::alive == 8, "Only the original objects remain; no leaked new block");

        for(int i = 0; i < 8; ++i)
            TEST_ASSERT(arr[i].value == i, "Original elements intact after failed reallocation");

        arr.Clear();
    }

    TEST_ASSERT(ThrowingCopy::alive == 0, "No leak after exception test");
}

static void TestDeleteSwapSemantics()
{    std::printf("\n=== PtrArray: Find and Capacity ===\n");
    std::printf("\n=== PtrArray: Delete vs DeleteSwap ===\n");

    // Delete 保序
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4)};
        TEST_ASSERT(arr.Delete(1), "Delete succeeded");
        TEST_ASSERT(arr.GetCount() == 3, "Delete reduces count");
        TEST_ASSERT(arr[0] == Tracked(1) && arr[1] == Tracked(3) && arr[2] == Tracked(4),
                    "Delete preserves order");
    }

    // DeleteShift 与 Delete 等价
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4)};
        TEST_ASSERT(arr.DeleteShift(1), "DeleteShift succeeded");
        TEST_ASSERT(arr[0] == Tracked(1) && arr[1] == Tracked(3) && arr[2] == Tracked(4),
                    "DeleteShift equals Delete");
    }

    // DeleteSwap 用尾元素填补，不保序
    {
        PtrArray<Tracked> arr{Tracked(1), Tracked(2), Tracked(3), Tracked(4)};
        TEST_ASSERT(arr.DeleteSwap(1), "DeleteSwap succeeded");
        TEST_ASSERT(arr.GetCount() == 3, "DeleteSwap reduces count");
        TEST_ASSERT(arr[0] == Tracked(1) && arr[1] == Tracked(4) && arr[2] == Tracked(3),
                    "DeleteSwap fills the hole with the last element");

        TEST_ASSERT(arr.DeleteSwap(2), "DeleteSwap of last element succeeded");
        TEST_ASSERT(arr.GetCount() == 2 && arr[0] == Tracked(1) && arr[1] == Tracked(4),
                    "DeleteSwap of tail keeps the rest");

        TEST_ASSERT(!arr.DeleteSwap(5), "DeleteSwap out of range fails");
    }

    TEST_ASSERT(Tracked::alive == 0, "DeleteSwap leaves no leaks");
}

static void TestTriviallyRelocatablePath()
{    std::printf("\n=== PtrArray: Find and Capacity ===\n");
    std::printf("\n=== PtrArray: trivially relocatable bulk move ===\n");

    static_assert(!std::is_trivially_copyable_v<Reloc>, "Reloc must not be trivially copyable");
    static_assert(std::is_trivially_move_constructible_v<Reloc>, "Reloc must be trivially move constructible");
    static_assert(std::is_trivially_destructible_v<Reloc>, "Reloc must be trivially destructible");

    // Delete 走 memmove 分支，结果必须与逐个搬移一致
    {
        PtrArray<Reloc> arr;
        for(int i = 1; i <= 5; ++i)
            arr.Add(Reloc(i));

        TEST_ASSERT(arr.Delete(1, 2), "Bulk delete succeeded");
        TEST_ASSERT(arr.GetCount() == 3, "Bulk delete count");
        TEST_ASSERT(arr[0] == Reloc(1) && arr[1] == Reloc(4) && arr[2] == Reloc(5),
                    "Bulk delete preserves order (memmove path)");
    }

    // Insert 走 memmove 分支
    {
        PtrArray<Reloc> arr;
        for(int i = 1; i <= 4; ++i)
            arr.Add(Reloc(i));

        Reloc ins[2] = {Reloc(8), Reloc(9)};
        TEST_ASSERT(arr.Insert(2, ins, 2), "Bulk insert succeeded");
        TEST_ASSERT(arr.GetCount() == 6, "Bulk insert count");
        TEST_ASSERT(arr[0] == Reloc(1) && arr[1] == Reloc(2) && arr[2] == Reloc(8)
                    && arr[3] == Reloc(9) && arr[4] == Reloc(3) && arr[5] == Reloc(4),
                    "Bulk insert keeps tail order (memmove path)");
    }

    // DeleteSwap 在快路径类型上同样正确
    {
        PtrArray<Reloc> arr;
        for(int i = 1; i <= 4; ++i)
            arr.Add(Reloc(i));

        TEST_ASSERT(arr.DeleteSwap(0), "DeleteSwap on relocatable type");
        TEST_ASSERT(arr.GetCount() == 3 && arr[0] == Reloc(4) && arr[1] == Reloc(2) && arr[2] == Reloc(3),
                    "DeleteSwap fills with last element");
    }
}

static void TestFindAndCapacity()
{    std::printf("\n=== PtrArray: Find and Capacity ===\n");
    PtrArray<Tracked> arr{Tracked(10), Tracked(20), Tracked(30)};

    TEST_ASSERT(arr.Find(Tracked(20)) == 1, "Find found");
    TEST_ASSERT(arr.Find(Tracked(99)) == -1, "Find not found");
    TEST_ASSERT(arr.Contains(Tracked(30)), "Contains found");
    TEST_ASSERT(!arr.Contains(Tracked(40)), "Contains not found");

    TEST_ASSERT(arr.Reserve(100), "Reserve succeeded");
    TEST_ASSERT(arr.GetAllocCount() >= 100, "Capacity sufficient after Reserve");

    TEST_ASSERT(arr.Resize(5), "Resize expands");
    TEST_ASSERT(arr.GetCount() == 5, "Count is 5 after Resize");

    int added = arr.Expand(3);
    TEST_ASSERT(added == 8 && arr.GetCount() == 8, "Expand returns new count (5+3=8)");

    // Zero 清零
    arr.Zero();
    for(int i = 0; i < arr.GetCount(); i++)
        TEST_ASSERT(arr[i] == Tracked(0), "All zero after Zero");
}

static void TestLifetime()
{
    std::printf("\n=== PtrArray: Lifetime ===\n");

    {
        PtrArray<Tracked> arr;
        for(int i = 0; i < 100; i++)
            arr.Add(Tracked(i));

        TEST_ASSERT(Tracked::alive == 100, "100 objects alive");

        arr.Clear();                        // 清空（不释放容量）
        TEST_ASSERT(arr.GetCount() == 0, "Count is 0 after Clear");
        TEST_ASSERT(Tracked::alive == 0, "Clear destroys all objects");
        TEST_ASSERT(arr.GetAllocCount() >= 100, "Clear keeps capacity");

        arr.ClearAndDeallocate();                         // 释放
        TEST_ASSERT(arr.GetAllocCount() == 0, "Free releases capacity");
    }

    // 移动赋值
    {
        PtrArray<Tracked> a{Tracked(1), Tracked(2)};
        PtrArray<Tracked> b;
        b = std::move(a);
        TEST_ASSERT(b.GetCount() == 2 && a.GetCount() == 0, "Move assignment");
    }

    // AdoptRawBuffer 接管契约
    {
        PtrArray<Tracked> arr;
        arr.Add(Tracked(1));

        TEST_ASSERT(!arr.AdoptRawBuffer(nullptr, -1), "AdoptRawBuffer rejects negative count");
        TEST_ASSERT(arr.GetCount() == 1, "Failed AdoptRawBuffer leaves container untouched");

        Tracked *raw = allocate_raw_memory<Tracked>(2);
        hgl::construct_at_copy(raw + 0, Tracked(7));
        hgl::construct_at_copy(raw + 1, Tracked(8));

        TEST_ASSERT(arr.AdoptRawBuffer(raw, 2), "AdoptRawBuffer adopts raw memory");
        TEST_ASSERT(arr.GetCount() == 2 && arr[0] == Tracked(7) && arr[1] == Tracked(8),
                    "Adopted content is readable");

        arr.ClearAndDeallocate();
    }

    TEST_ASSERT(Tracked::alive == 0, "No leak, no double free at end");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestConstruction();
    TestAddAppend();
    TestAccess();
    TestInsertDelete();
    TestMoveExchange();
    TestMoveSemantics();
    TestReallocationExceptionSafety();
    TestDeleteSwapSemantics();
    TestTriviallyRelocatablePath();
    TestFindAndCapacity();
    TestLifetime();

    TEST_ASSERT(Tracked::alive == 0, "No global leak");

    return TEST_SUMMARY();
}
