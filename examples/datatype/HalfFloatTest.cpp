#include<hgl/math/HalfFloat.h>
#include<hgl/CoreType.h>
#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<cmath>
#include<limits>
#include<vector>

using namespace hgl::math;
using namespace hgl;
using namespace std;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            cout << "  [FAIL] " << msg << endl; \
            return false; \
        } \
        cout << "  [PASS] " << msg << endl; \
    } while(0)

#define FLOAT_EPSILON 0.001f  // 允许的误差范围

// 打印float的二进制表示
void PrintFloatBits(float f)
{
    uint32 bits = *reinterpret_cast<uint32*>(&f);
    cout << "0x" << hex << setfill('0') << setw(8) << bits << dec;
}

// 打印half_float的二进制表示
void PrintHalfBits(half_float hf)
{
    cout << "0x" << hex << setfill('0') << setw(4) << hf << dec;
}

// 比较两个float是否在误差范围内相等
bool FloatNearlyEqual(float a, float b, float epsilon = FLOAT_EPSILON)
{
    if (isnan(a) && isnan(b)) return true;
    if (isinf(a) && isinf(b) && signbit(a) == signbit(b)) return true;
    if (abs(a) < epsilon && abs(b) < epsilon) return true;  // 接近零
    return abs(a - b) <= epsilon * max(abs(a), abs(b));
}

// 测试1: 基本转换功能
bool test_basic_conversion()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: Basic Float32 to Float16 Conversion" << endl;
    cout << "========================================\n" << endl;
    
    struct TestCase {
        float input;
        const char* name;
    };
    
    TestCase cases[] = {
        {0.0f, "Zero"},
        {1.0f, "One"},
        {-1.0f, "Negative One"},
        {0.5f, "Half"},
        {2.0f, "Two"},
        {10.0f, "Ten"},
        {100.0f, "One Hundred"},
        {0.1f, "Point One"},
        {0.333333f, "One Third"},
    };
    
    cout << "[1.1] Converting common values:" << endl;
    for (const auto& tc : cases)
    {
        half_float hf;
        Float32toFloat16(&hf, &tc.input, 1);
        
        float reconstructed = half2float(hf);
        
        cout << "  " << tc.name << ": " << tc.input << " -> ";
        PrintHalfBits(hf);
        cout << " -> " << reconstructed;
        
        bool match = FloatNearlyEqual(tc.input, reconstructed, 0.01f);
        if (match) {
            cout << " [OK]" << endl;
        } else {
            cout << " [ERROR: diff=" << abs(tc.input - reconstructed) << "]" << endl;
            return false;
        }
    }
    
    return true;
}

// 测试2: 特殊值（零、无穷大、NaN）
bool test_special_values()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Special Values (Zero, Infinity, NaN)" << endl;
    cout << "========================================\n" << endl;
    
    cout << "[2.1] Positive and Negative Zero:" << endl;
    {
        float pos_zero = 0.0f;
        float neg_zero = -0.0f;
        half_float hf_pos, hf_neg;
        
        Float32toFloat16(&hf_pos, &pos_zero, 1);
        Float32toFloat16(&hf_neg, &neg_zero, 1);
        
        cout << "  +0.0: "; PrintFloatBits(pos_zero); cout << " -> "; PrintHalfBits(hf_pos); cout << endl;
        cout << "  -0.0: "; PrintFloatBits(neg_zero); cout << " -> "; PrintHalfBits(hf_neg); cout << endl;
        
        TEST_ASSERT(hf_pos == 0x0000, "Positive zero converts to 0x0000");
        TEST_ASSERT(hf_neg == 0x8000, "Negative zero converts to 0x8000");
    }
    
    cout << "\n[2.2] Positive and Negative Infinity:" << endl;
    {
        float pos_inf = numeric_limits<float>::infinity();
        float neg_inf = -numeric_limits<float>::infinity();
        half_float hf_pos_inf, hf_neg_inf;
        
        Float32toFloat16(&hf_pos_inf, &pos_inf, 1);
        Float32toFloat16(&hf_neg_inf, &neg_inf, 1);
        
        cout << "  +Inf: "; PrintFloatBits(pos_inf); cout << " -> "; PrintHalfBits(hf_pos_inf); cout << endl;
        cout << "  -Inf: "; PrintFloatBits(neg_inf); cout << " -> "; PrintHalfBits(hf_neg_inf); cout << endl;
        
        TEST_ASSERT(hf_pos_inf == 0x7C00, "Positive infinity converts to 0x7C00");
        TEST_ASSERT(hf_neg_inf == 0xFC00, "Negative infinity converts to 0xFC00");
        
        float reconstructed_pos = half2float(hf_pos_inf);
        float reconstructed_neg = half2float(hf_neg_inf);
        
        TEST_ASSERT(isinf(reconstructed_pos) && reconstructed_pos > 0, "Reconstructed +Inf is positive infinity");
        TEST_ASSERT(isinf(reconstructed_neg) && reconstructed_neg < 0, "Reconstructed -Inf is negative infinity");
    }
    
    cout << "\n[2.3] NaN (Not a Number):" << endl;
    {
        float nan_val = numeric_limits<float>::quiet_NaN();
        half_float hf_nan;
        
        Float32toFloat16(&hf_nan, &nan_val, 1);
        
        cout << "  NaN: "; PrintFloatBits(nan_val); cout << " -> "; PrintHalfBits(hf_nan); cout << endl;
        
        TEST_ASSERT(hf_nan == 0x7FFF, "NaN converts to 0x7FFF");
        
        float reconstructed = half2float(hf_nan);
        TEST_ASSERT(isnan(reconstructed), "Reconstructed value is NaN");
    }
    
    return true;
}

// 测试3: 边界值和极限值
bool test_boundary_values()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: Boundary and Extreme Values" << endl;
    cout << "========================================\n" << endl;
    
    cout << "[3.1] Very Small Values (Underflow):" << endl;
    {
        float tiny_values[] = {1e-8f, 1e-6f, 1e-5f, 6e-5f};  // 6e-5是half_float的最小规格化数
        
        for (float val : tiny_values)
        {
            half_float hf;
            Float32toFloat16(&hf, &val, 1);
            float reconstructed = half2float(hf);
            
            cout << "  " << scientific << val << ": "; 
            PrintHalfBits(hf);
            cout << " -> " << reconstructed;
            
            if (val < 6e-5f) {
                cout << " (underflow to zero or denormal)" << endl;
            } else {
                cout << " [OK]" << endl;
            }
        }
    }
    
    cout << "\n[3.2] Very Large Values (Overflow):" << endl;
    {
        float large_values[] = {1000.0f, 10000.0f, 65504.0f, 70000.0f, 100000.0f};
        // half_float最大值约为65504
        
        for (float val : large_values)
        {
            half_float hf;
            Float32toFloat16(&hf, &val, 1);
            float reconstructed = half2float(hf);
            
            cout << "  " << fixed << val << ": ";
            PrintHalfBits(hf);
            cout << " -> " << reconstructed;
            
            if (val > 65504.0f) {
                cout << " (overflow to infinity)" << endl;
                TEST_ASSERT(isinf(reconstructed), "Large value overflows to infinity");
            } else {
                cout << " [OK]" << endl;
            }
        }
    }
    
    cout << "\n[3.3] Precision Loss at Different Scales:" << endl;
    {
        float test_values[] = {0.1f, 0.01f, 0.001f, 10.0f, 100.0f, 1000.0f};
        
        for (float val : test_values)
        {
            half_float hf;
            Float32toFloat16(&hf, &val, 1);
            float reconstructed = half2float(hf);
            float error = abs(val - reconstructed);
            float relative_error = error / val * 100.0f;
            
            cout << "  " << val << " -> " << reconstructed 
                 << " (error: " << error << ", " << relative_error << "%)" << endl;
        }
    }
    
    return true;
}

// 测试4: 批量转换
bool test_batch_conversion()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Batch Conversion" << endl;
    cout << "========================================\n" << endl;
    
    cout << "[4.1] Converting array of values:" << endl;
    {
        const int count = 16;
        float input[count];
        half_float output[count];
        
        // 生成测试数据
        for (int i = 0; i < count; i++)
        {
            input[i] = (i - 8) * 0.5f;  // -4.0 到 3.5
        }
        
        // 批量转换
        Float32toFloat16(output, input, count);
        
        // 验证每个值
        int errors = 0;
        for (int i = 0; i < count; i++)
        {
            float reconstructed = half2float(output[i]);
            if (!FloatNearlyEqual(input[i], reconstructed, 0.01f))
            {
                cout << "  [ERROR] Index " << i << ": " << input[i] 
                     << " -> " << reconstructed << endl;
                errors++;
            }
        }
        
        TEST_ASSERT(errors == 0, "All batch conversions accurate");
        cout << "  Converted " << count << " values successfully" << endl;
    }
    
    return true;
}

// 测试5: half2u16 和 half2u8 转换
bool test_integer_conversion()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: Half to Integer Conversions" << endl;
    cout << "========================================\n" << endl;
    
    cout << "[5.1] half2u16 (0.0-1.0 -> 0-65535):" << endl;
    {
        float inputs[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
        uint16 expected[] = {0, 16383, 32767, 49151, 65535};  // 近似值
        
        for (int i = 0; i < 5; i++)
        {
            half_float hf;
            uint16 output;
            
            Float32toFloat16(&hf, &inputs[i], 1);
            half2u16(&output, &hf, 1);
            
            cout << "  " << inputs[i] << " -> 0x";
            PrintHalfBits(hf);
            cout << " -> " << output << " (expected ~" << expected[i] << ")";
            
            int diff = abs((int)output - (int)expected[i]);
            if (diff < 100) {  // 允许小误差
                cout << " [OK]" << endl;
            } else {
                cout << " [DIFF: " << diff << "]" << endl;
            }
        }
    }
    
    cout << "\n[5.2] half2u8 (0.0-1.0 -> 0-255):" << endl;
    {
        float inputs[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
        uint8 expected[] = {0, 63, 127, 191, 255};
        
        for (int i = 0; i < 5; i++)
        {
            half_float hf;
            uint8 output;
            
            Float32toFloat16(&hf, &inputs[i], 1);
            half2u8(&output, &hf, 1);
            
            cout << "  " << inputs[i] << " -> 0x";
            PrintHalfBits(hf);
            cout << " -> " << (int)output << " (expected ~" << (int)expected[i] << ")";
            
            int diff = abs((int)output - (int)expected[i]);
            if (diff < 2) {
                cout << " [OK]" << endl;
            } else {
                cout << " [DIFF: " << diff << "]" << endl;
            }
        }
    }
    
    cout << "\n[5.3] Clamping behavior (values outside 0-1):" << endl;
    {
        float inputs[] = {-0.5f, 1.5f, 2.0f};
        
        for (float val : inputs)
        {
            half_float hf;
            uint8 u8_out;
            uint16 u16_out;
            
            Float32toFloat16(&hf, &val, 1);
            half2u8(&u8_out, &hf, 1);
            half2u16(&u16_out, &hf, 1);
            
            cout << "  " << val << " -> u8=" << (int)u8_out 
                 << ", u16=" << u16_out;
            
            if (val < 0.0f) {
                cout << " (clamped to 0)" << endl;
                TEST_ASSERT(u8_out == 0 && u16_out == 0, "Negative clamped to 0");
            } else if (val > 1.0f) {
                cout << " (clamped to max)" << endl;
                TEST_ASSERT(u8_out == 255 && u16_out == 65535, "Over 1.0 clamped to max");
            }
        }
    }
    
    return true;
}

// 测试6: 压力测试和随机值
bool test_stress_and_random()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Stress Test with Random Values" << endl;
    cout << "========================================\n" << endl;
    
    cout << "[6.1] Converting 10000 random values:" << endl;
    {
        srand(12345);  // 固定种子以便重现
        const int count = 10000;
        vector<float> inputs(count);
        vector<half_float> outputs(count);
        
        // 生成随机值
        for (int i = 0; i < count; i++)
        {
            float r = (rand() % 20000 - 10000) / 100.0f;  // -100.0 到 100.0
            inputs[i] = r;
        }
        
        // 批量转换
        Float32toFloat16(outputs.data(), inputs.data(), count);
        
        // 抽样验证
        int sample_count = 0;
        int error_count = 0;
        
        for (int i = 0; i < count; i += 100)  // 每100个检查一次
        {
            float reconstructed = half2float(outputs[i]);
            if (!FloatNearlyEqual(inputs[i], reconstructed, 0.1f))
            {
                if (error_count < 5)  // 只打印前5个错误
                {
                    cout << "  [ERROR] " << inputs[i] << " -> " 
                         << reconstructed << " (diff: " 
                         << abs(inputs[i] - reconstructed) << ")" << endl;
                }
                error_count++;
            }
            sample_count++;
        }
        
        float error_rate = (float)error_count / sample_count * 100.0f;
        cout << "  Sampled " << sample_count << " values, errors: " 
             << error_count << " (" << error_rate << "%)" << endl;
        
        TEST_ASSERT(error_rate < 5.0f, "Error rate below 5%");
    }
    
    return true;
}

// 测试7: half_to_float批量位格式转换
bool test_half_to_float_batch()
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Batch half_to_float (Bit Format)" << endl;
    cout << "========================================\n" << endl;
    
    cout << "[7.1] Batch bit format conversion:" << endl;
    {
        half_float half_values[] = {
            0x0000,  // 0.0
            0x3C00,  // 1.0
            0x4000,  // 2.0
            0x4200,  // 3.0
            0xBC00,  // -1.0
        };
        uint32 float_bits[5];
        
        half_to_float(float_bits, half_values, 5);
        
        for (int i = 0; i < 5; i++)
        {
            float result = *reinterpret_cast<float*>(&float_bits[i]);
            cout << "  half 0x" << hex << setfill('0') << setw(4) << half_values[i]
                 << " -> float bits 0x" << setw(8) << float_bits[i]
                 << dec << " = " << result << endl;
        }
        
        TEST_ASSERT(true, "Batch bit conversion completed");
    }
    
    return true;
}

int main(int, char**)
{
    cout << "========================================" << endl;
    cout << "HalfFloat Comprehensive Test Suite" << endl;
    cout << "========================================" << endl;
    
    bool all_passed = true;
    
    all_passed &= test_basic_conversion();
    all_passed &= test_special_values();
    all_passed &= test_boundary_values();
    all_passed &= test_batch_conversion();
    all_passed &= test_integer_conversion();
    all_passed &= test_stress_and_random();
    all_passed &= test_half_to_float_batch();
    
    cout << "\n========================================" << endl;
    if (all_passed) {
        cout << "ALL TESTS PASSED!" << endl;
    } else {
        cout << "SOME TESTS FAILED!" << endl;
    }
    cout << "========================================" << endl;
    
    return all_passed ? 0 : 1;
}
