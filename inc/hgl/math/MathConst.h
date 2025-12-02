/**
 * MathConst.h - 数学常量与工具函数的兼容性包装器
 * 
 * 本文件为数学常量功能的统一入口，包含以下分类模块：
 * - FloatPrecision.h: 浮点数精度常量与比较函数
 * - MathConstants.h: 基础数学常量 (e, π, √2 等)
 * - TrigConstants.h: 三角函数常量与角度转换
 * - MathUtils.h: 数学工具函数 (浮点截取、体积计算)
 * 
 * 保持向后兼容性，原有代码无需修改
 */

#pragma once

// 包含所有数学常量相关的分类头文件
#include<hgl/math/FloatPrecision.h>
#include<hgl/math/MathConstants.h>
#include<hgl/math/TrigConstants.h>
#include<hgl/math/MathUtils.h>
