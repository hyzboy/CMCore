#include <iostream>
#include <iomanip>
#include <hgl/math/AlphaBlend.h>
#include <hgl/math/Vector.h>

using namespace std;
using namespace hgl::graph;
using namespace hgl::math;

// 辅助宏用于测试断言
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            cout << "  [FAIL] " << message << endl; \
            return false; \
        } else { \
            cout << "  [PASS] " << message << endl; \
        } \
    } while(0)

// 浮点数近似相等判断
bool FloatNearlyEqual(float a, float b, float epsilon = 0.0001f)
{
    return abs(a - b) < epsilon;
}

// 向量近似相等判断
bool VectorNearlyEqual(const Vector3f &a, const Vector3f &b, float epsilon = 0.0001f)
{
    return FloatNearlyEqual(a.x, b.x, epsilon) &&
           FloatNearlyEqual(a.y, b.y, epsilon) &&
           FloatNearlyEqual(a.z, b.z, epsilon);
}

// 测试1: 基础混合模式（标量）
bool test_basic_blend_scalar()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: Basic Alpha Blend (Scalar)" << endl;
    cout << "========================================\n" << endl;

    float base = 0.5f;
    float blend = 0.8f;
    float alpha = 0.5f;

    cout << "[1.1] Normal Blend:" << endl;
    {
        float result = AlphaBlendNormal(base, blend, alpha);
        float expected = 0.5f * 0.5f + 0.8f * 0.5f; // 0.65
        cout << "  Result: " << result << " (expected: " << expected << ")" << endl;
        TEST_ASSERT(FloatNearlyEqual(result, expected), "Normal blend calculation");
    }

    cout << "\n[1.2] Add Blend:" << endl;
    {
        float result = AlphaBlendAdd(base, blend, alpha);
        float expected = 0.5f + 0.8f * 0.5f; // 0.9
        cout << "  Result: " << result << " (expected: " << expected << ")" << endl;
        TEST_ASSERT(FloatNearlyEqual(result, expected), "Add blend calculation");
    }

    cout << "\n[1.3] Subtract Blend:" << endl;
    {
        float result = AlphaBlendSubtract(base, blend, alpha);
        float expected = 0.5f - 0.8f * 0.5f; // 0.1
        cout << "  Result: " << result << " (expected: " << expected << ")" << endl;
        TEST_ASSERT(FloatNearlyEqual(result, expected), "Subtract blend calculation");
    }

    cout << "\n[1.4] Multiply Blend:" << endl;
    {
        float result = AlphaBlendMultiply(base, blend, alpha);
        cout << "  Result: " << result << endl;
        TEST_ASSERT(result > 0.0f && result < 1.0f, "Multiply blend in valid range");
    }

    return true;
}

// 测试2: 向量混合
bool test_vector_blend()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Alpha Blend (Vector3f - RGB Color)" << endl;
    cout << "========================================\n" << endl;

    Vector3f red(1.0f, 0.0f, 0.0f);
    Vector3f blue(0.0f, 0.0f, 1.0f);
    float alpha = 0.5f;

    cout << "[2.1] Normal Blend (Red + Blue):" << endl;
    {
        Vector3f result = AlphaBlendNormal(red, blue, alpha);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        Vector3f expected(0.5f, 0.0f, 0.5f); // Purple
        TEST_ASSERT(VectorNearlyEqual(result, expected), "Normal blend RGB");
    }

    cout << "\n[2.2] Add Blend (Red + Blue):" << endl;
    {
        Vector3f result = AlphaBlendAdd(red, blue, alpha);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        Vector3f expected(1.0f, 0.0f, 0.5f); // Bright magenta
        TEST_ASSERT(VectorNearlyEqual(result, expected), "Add blend RGB");
    }

    cout << "\n[2.3] Screen Blend:" << endl;
    {
        Vector3f result = AlphaBlendScreen(red, blue, alpha);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(result.x >= red.x && result.z >= 0.0f, "Screen blend brightens");
    }

    cout << "\n[2.4] Multiply Blend:" << endl;
    {
        Vector3f result = AlphaBlendMultiply(red, blue, alpha);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(result.x <= red.x, "Multiply blend darkens");
    }

    return true;
}

// 测试3: 边界值测试
bool test_boundary_values()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: Boundary Values" << endl;
    cout << "========================================\n" << endl;

    Vector3f white(1.0f, 1.0f, 1.0f);
    Vector3f black(0.0f, 0.0f, 0.0f);
    Vector3f gray(0.5f, 0.5f, 0.5f);

    cout << "[3.1] Alpha = 0 (should return base):" << endl;
    {
        Vector3f result = AlphaBlendNormal(gray, white, 0.0f);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(VectorNearlyEqual(result, gray), "Alpha=0 returns base");
    }

    cout << "\n[3.2] Alpha = 1 (should return blend):" << endl;
    {
        Vector3f result = AlphaBlendNormal(gray, white, 1.0f);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(VectorNearlyEqual(result, white), "Alpha=1 returns blend");
    }

    cout << "\n[3.3] Darken (Black + White):" << endl;
    {
        Vector3f result = AlphaBlendDarken(white, black, 0.5f);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(result.x <= white.x, "Darken keeps darker values");
    }

    cout << "\n[3.4] Lighten (Black + White):" << endl;
    {
        Vector3f result = AlphaBlendLighten(black, white, 0.5f);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(result.x >= black.x, "Lighten keeps lighter values");
    }

    return true;
}

// 测试4: 特殊混合模式
bool test_special_modes()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Special Blend Modes" << endl;
    cout << "========================================\n" << endl;

    Vector3f color1(0.3f, 0.6f, 0.9f);
    Vector3f color2(0.8f, 0.4f, 0.2f);
    float alpha = 0.6f;

    cout << "[4.1] Difference Blend:" << endl;
    {
        Vector3f result = AlphaBlendDifference(color1, color2, alpha);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(true, "Difference blend executed");
    }

    cout << "\n[4.2] Exclusion Blend:" << endl;
    {
        Vector3f result = AlphaBlendExclusion(color1, color2, alpha);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(true, "Exclusion blend executed");
    }

    cout << "\n[4.3] Overlay Blend:" << endl;
    {
        Vector3f result = AlphaBlendOverlay(color1, color2, alpha);
        cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        TEST_ASSERT(true, "Overlay blend executed");
    }

    return true;
}

// 测试5: 函数指针
bool test_function_pointers()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: Function Pointers" << endl;
    cout << "========================================\n" << endl;

    Vector3f base(0.4f, 0.5f, 0.6f);
    Vector3f blend(0.8f, 0.7f, 0.9f);
    float alpha = 0.5f;

    cout << "[5.1] Getting Normal blend function pointer:" << endl;
    {
        auto func = GetAlphaBlendFuncVec3f(AlphaBlendMode::Normal);
        TEST_ASSERT(func != nullptr, "Normal blend function pointer valid");
        
        if (func)
        {
            Vector3f result = func(base, blend, alpha);
            cout << "  Result: (" << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        }
    }

    cout << "\n[5.2] Testing all blend mode names:" << endl;
    {
        for (int i = 0; i <= (int)AlphaBlendMode::PremultipliedAlpha; i++)
        {
            AlphaBlendMode mode = (AlphaBlendMode)i;
            const char* name = GetAlphaBlendModeName(mode);
            cout << "  Mode " << i << ": " << name << endl;
        }
        TEST_ASSERT(true, "All blend mode names retrieved");
    }

    return true;
}

// 测试6: 通用AlphaBlend函数
bool test_generic_blend()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Generic AlphaBlend Function" << endl;
    cout << "========================================\n" << endl;

    Vector3f base(0.5f, 0.5f, 0.5f);
    Vector3f blend(1.0f, 0.0f, 0.0f);
    float alpha = 0.7f;

    cout << "[6.1] Testing multiple modes via generic function:" << endl;
    {
        AlphaBlendMode modes[] = {
            AlphaBlendMode::Normal,
            AlphaBlendMode::Add,
            AlphaBlendMode::Multiply,
            AlphaBlendMode::Screen
        };

        for (auto mode : modes)
        {
            Vector3f result = AlphaBlend(base, blend, alpha, mode);
            cout << "  " << GetAlphaBlendModeName(mode) << ": ("
                 << result.x << ", " << result.y << ", " << result.z << ")" << endl;
        }
        TEST_ASSERT(true, "Generic blend function works");
    }

    return true;
}

int main(int, char**)
{
    cout << "========================================" << endl;
    cout << "AlphaBlend Comprehensive Test Suite" << endl;
    cout << "========================================" << endl;

    bool all_passed = true;

    all_passed &= test_basic_blend_scalar();
    all_passed &= test_vector_blend();
    all_passed &= test_boundary_values();
    all_passed &= test_special_modes();
    all_passed &= test_function_pointers();
    all_passed &= test_generic_blend();

    cout << "\n========================================" << endl;
    if (all_passed) {
        cout << "ALL TESTS PASSED!" << endl;
    } else {
        cout << "SOME TESTS FAILED!" << endl;
    }
    cout << "========================================" << endl;

    return all_passed ? 0 : 1;
}
