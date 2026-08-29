/**
 * test_common.h
 *
 * 共享测试框架头文件 (datatype 测试专用)
 *
 * 统一断言/统计/退出码约定：
 *   - TEST_ASSERT(condition, message)  记录通过/失败并打印
 *   - TEST_ASSERT_EQ(a, b, message)    比较两个值是否相等
 *   - TEST_SUMMARY()                   打印统计并返回退出码(0=全过, 1=有失败)
 *   - main 末尾直接 `return TEST_SUMMARY();`
 *
 * 相比各文件自带的私有 TEST_ASSERT 宏：
 *   - 全局唯一实现，避免 40+ 份重复定义
 *   - 退出码可被 CTest 感知（add_test 后失败即 CI 失败）
 */
#pragma once

#include <cstdio>
#include <cstring>

namespace hgltest
{
    struct TestStats
    {
        int passed = 0;
        int failed = 0;

        void report(bool ok, const char *msg, int line)
        {
            if (ok)
            {
                ++passed;
                std::printf("  [PASS] %s\n", msg);
            }
            else
            {
                ++failed;
                std::printf("  [FAIL] %s  (line %d)\n", msg, line);
            }
        }
    };

    inline TestStats &stats()
    {
        static TestStats s;
        return s;
    }

    inline void reset()
    {
        stats().passed = 0;
        stats().failed = 0;
    }

    inline int summary(const char *name)
    {
        TestStats &s = stats();
        std::printf("\n========================================\n");
        std::printf("Results: %d passed, %d failed\n", s.passed, s.failed);
        std::printf("========================================\n");
        (void)name;
        return s.failed == 0 ? 0 : 1;
    }
}//namespace hgltest

#define TEST_ASSERT(condition, message)                                      \
    do                                                                       \
    {                                                                        \
        hgltest::stats().report((condition) ? true : false, (message), __LINE__); \
    } while (0)

#define TEST_ASSERT_EQ(a, b, message)                                        \
    do                                                                       \
    {                                                                        \
        const auto _va = (a);                                                \
        const auto _vb = (b);                                                \
        hgltest::stats().report((_va == _vb), (message), __LINE__);          \
    } while (0)

#define TEST_SUMMARY() hgltest::summary(__FILE__)
