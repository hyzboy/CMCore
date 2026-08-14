/**
 * GradientTest.cpp
 *
 * Gradient<P,T> 渐变模板测试：
 *   - Add 渐变点 / 计数
 *   - 区间内线性插值
 *   - 边界（<= 最低点 / >= 最高点）
 *   - 单点 / 空渐变
 *   - GetLowest/GetHighest 系列
 */
#include<hgl/type/Gradient.h>
#include"test_common.h"

using namespace hgl;

static void TestBasic()
{
    std::printf("\n=== Gradient: Basic Interpolation ===\n");

    Gradient<float, float> g;
    float r = 0;

    TEST_ASSERT(!g.GetLowestPosition(r), "Initially empty (no lowest point)");

    g.Add(0.0f, 0.0f);
    g.Add(1.0f, 10.0f);
    g.Add(2.0f, 20.0f);

    float pos = 0;
    TEST_ASSERT(g.GetLowestPosition(pos) && pos == 0.0f, "3 stops (lowest point 0)");
    TEST_ASSERT(g.GetHighestPosition(pos) && pos == 2.0f, "Highest point 2");

    g.Get(r, 0.5f);
    TEST_ASSERT(r > 4.9f && r < 5.1f, "Midpoint interpolation 0.5 -> ~5.0");

    g.Get(r, 1.5f);
    TEST_ASSERT(r > 14.9f && r < 15.1f, "Second segment interpolation 1.5 -> ~15.0");

    // 边界
    g.Get(r, 0.0f);
    TEST_ASSERT(r == 0.0f, "pos == lowest point");

    g.Get(r, 2.0f);
    TEST_ASSERT(r == 20.0f, "pos == highest point");
}

static void TestBoundary()
{
    std::printf("\n=== Gradient: Boundary Behavior ===\n");

    Gradient<float, float> g;
    g.Add(1.0f, 100.0f);
    g.Add(3.0f, 300.0f);

    float r = -1;
    g.Get(r, 0.0f);                     // 低于最低点
    TEST_ASSERT(r == 100.0f, "Below lowest point returns lowest value");

    g.Get(r, 99.0f);                    // 高于最高点
    TEST_ASSERT(r == 300.0f, "Above highest point returns highest value");

    // 最低/最高位置与数据
    float pos = 0;
    TEST_ASSERT(g.GetLowestPosition(pos) && pos == 1.0f, "GetLowestPosition");
    TEST_ASSERT(g.GetHighestPosition(pos) && pos == 3.0f, "GetHighestPosition");

    float data = 0;
    TEST_ASSERT(g.GetLowestData(data) && data == 100.0f, "GetLowestData");
    TEST_ASSERT(g.GetHighestData(data) && data == 300.0f, "GetHighestData");
}

static void TestEdgeCases()
{
    std::printf("\n=== Gradient: Single Point and Empty ===\n");

    // 空渐变：Get 无操作，位置/数据获取失败
    Gradient<float, float> empty;
    float r = 0;
    empty.Get(r, 0.5f);
    TEST_ASSERT(r == 0.0f, "Empty gradient Get does not change result");
    TEST_ASSERT(!empty.GetLowestPosition(r), "Empty gradient GetLowestPosition fails");
    TEST_ASSERT(!empty.GetHighestData(r), "Empty gradient GetHighestData fails");

    // 单点渐变：任意 pos 都取该值
    Gradient<float, float> single;
    single.Add(0.5f, 42.0f);
    single.Get(r, 0.0f);
    TEST_ASSERT(r == 42.0f, "Single-point gradient returns its value");
    single.Get(r, 1.0f);
    TEST_ASSERT(r == 42.0f, "Single-point gradient returns value even beyond pos");
}

static void TestClear()
{
    std::printf("\n=== Gradient: Clear ===\n");

    Gradient<float, float> g;
    g.Add(0.0f, 1.0f);
    g.Add(1.0f, 2.0f);

    float pos = 0;
    TEST_ASSERT(g.GetLowestPosition(pos) && pos == 0.0f, "2 stops");

    g.Clear();
    TEST_ASSERT(!g.GetLowestPosition(pos), "Empty after Clear");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestBasic();
    TestBoundary();
    TestEdgeCases();
    TestClear();

    return TEST_SUMMARY();
}
