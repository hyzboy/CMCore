/**
 * BitsArrayTest.cpp
 *
 * BitsArray 位数据阵列测试。
 *
 * 注意：测试只覆盖 Create(n) 容量内的安全路径。
 * 已发现的实现缺陷（不在本测试中触发，避免 UB）：
 *   1. WriteBit/WriteBytes 超过 Create(n) 容量时只增 count 不扩 byte_size，
 *      会越界写（WriteOne/WriteZero 中 byte_offset 可能 >= byte_size）。
 *   2. FlipBit 直接用 pos 同时作为字节偏移和位偏移（pos>=8 时越界）。
 *   3. ReadBit 不检查 pos 是否越界，读到末尾仍返回 true 且可能越界读。
 */
#include<hgl/type/BitsArray.h>
#include"test_common.h"

using namespace hgl;

static void TestBasic()
{
    std::printf("\n=== BitsArray: Basic Read/Write ===\n");

    BitsArray ba;

    ba.Create(16);
    TEST_ASSERT(ba.GetCount() == 16, "Bit count 16 after Create(16)");
    TEST_ASSERT(ba.GetByteSize() == 2, "16 bits take 2 bytes");

    // 顺序写 16 位：1010 1010 1100 1100
    const bool pattern[16] = {1,0,1,0,1,0,1,0, 1,1,0,0,1,1,0,0};
    for(int i = 0; i < 16; i++)
        TEST_ASSERT(ba.WriteBit(pattern[i]), "WriteBit succeeded");

    TEST_ASSERT(ba.GetCount() == 16, "Writes within capacity do not change count");

    // 复位读取位置后顺序读回
    ba.SetPos(0);
    bool b = false;
    bool all_ok = true;
    for(int i = 0; i < 16; i++)
    {
        if(!ba.ReadBit(b) || b != pattern[i])
            all_ok = false;
    }
    TEST_ASSERT(all_ok, "Sequential read-back matches");
}

static void TestRandomAccess()
{
    std::printf("\n=== BitsArray: Random Access ===\n");

    BitsArray ba(16);

    ba.WriteAt(0, true);
    ba.WriteAt(7, true);
    ba.WriteAt(8, true);
    ba.WriteAt(15, true);

    bool b = false;
    TEST_ASSERT(ba.ReadAt(0, b) && b, "ReadAt(0) == 1");
    TEST_ASSERT(ba.ReadAt(1, b) && !b, "ReadAt(1) == 0");
    TEST_ASSERT(ba.ReadAt(7, b) && b, "ReadAt(7) == 1 (cross byte boundary)");
    TEST_ASSERT(ba.ReadAt(8, b) && b, "ReadAt(8) == 1 (cross byte boundary)");
    TEST_ASSERT(ba.ReadAt(15, b) && b, "ReadAt(15) == 1");

    // FlipBitAt 返回翻转后该位的值（0 号位原为 1：翻转为 0 返回 false，再翻回 1 返回 true）
    TEST_ASSERT(!ba.FlipBitAt(0), "FlipBitAt(0) flips 1->0");
    TEST_ASSERT(ba.ReadAt(0, b) && !b, "Bit 0 is 0 after flip");
    TEST_ASSERT(ba.FlipBitAt(0), "FlipBitAt(0) flips back 0->1");
    TEST_ASSERT(ba.ReadAt(0, b) && b, "Flipped back to 1");
}

static void TestCompare()
{
    std::printf("\n=== BitsArray: Compare and Copy ===\n");

    BitsArray a(16);
    BitsArray b(16);

    a.WriteBit(true);
    a.WriteBit(false);
    a.WriteBit(true);

    b.WriteBit(true);
    b.WriteBit(false);
    b.WriteBit(true);

    TEST_ASSERT(a == b, "Same content operator==");
    TEST_ASSERT(!(a != b), "Same content operator!= is false");

    b.WriteBit(true);       // 第 4 位不同
    TEST_ASSERT(a != b, "Different content operator!=");
    TEST_ASSERT(!(a == b), "Different content operator== is false");

    BitsArray c;
    c = a;                  // 拷贝赋值
    TEST_ASSERT(c == a, "Equal after copy assignment");

    // Fill
    BitsArray d(8);
    d.Fill(true);
    bool bit = false;
    bool all_one = true;
    for(int i = 0; i < 8; i++)
    {
        d.ReadAt(i, bit);
        if(!bit)
            all_one = false;
    }
    TEST_ASSERT(all_one, "All 1 after Fill(true)");

    // Clear
    d.Clear();
    TEST_ASSERT(d.GetCount() == 0, "Bit count 0 after Clear");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestBasic();
    TestRandomAccess();
    TestCompare();

    return TEST_SUMMARY();
}
