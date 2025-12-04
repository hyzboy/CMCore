/**
 * TypeLimits.h - 类型极值定义
 * Type limits definitions
 * 
 * This file provides type limit constants and utilities
 */

#pragma once

#include <hgl/platform/Platform.h>
#include <limits>

namespace hgl
{
    // Integer limits
    constexpr int8   INT8_MIN_VALUE   = std::numeric_limits<int8>::min();
    constexpr int8   INT8_MAX_VALUE   = std::numeric_limits<int8>::max();
    constexpr int16  INT16_MIN_VALUE  = std::numeric_limits<int16>::min();
    constexpr int16  INT16_MAX_VALUE  = std::numeric_limits<int16>::max();
    constexpr int32  INT32_MIN_VALUE  = std::numeric_limits<int32>::min();
    constexpr int32  INT32_MAX_VALUE  = std::numeric_limits<int32>::max();
    constexpr int64  INT64_MIN_VALUE  = std::numeric_limits<int64>::min();
    constexpr int64  INT64_MAX_VALUE  = std::numeric_limits<int64>::max();

    constexpr uint8  UINT8_MAX_VALUE  = std::numeric_limits<uint8>::max();
    constexpr uint16 UINT16_MAX_VALUE = std::numeric_limits<uint16>::max();
    constexpr uint32 UINT32_MAX_VALUE = std::numeric_limits<uint32>::max();
    constexpr uint64 UINT64_MAX_VALUE = std::numeric_limits<uint64>::max();

    // Floating point limits
    constexpr float  FLOAT_MIN_VALUE  = std::numeric_limits<float>::min();
    constexpr float  FLOAT_MAX_VALUE  = std::numeric_limits<float>::max();
    constexpr double DOUBLE_MIN_VALUE = std::numeric_limits<double>::min();
    constexpr double DOUBLE_MAX_VALUE = std::numeric_limits<double>::max();

    // Float epsilon
    constexpr float  FLOAT_EPSILON    = std::numeric_limits<float>::epsilon();
    constexpr double DOUBLE_EPSILON   = std::numeric_limits<double>::epsilon();
}
