/**
 * Vector.h - 向量类型和操作的兼容性包装器
 * 
 * 本文件为向量功能的统一入口，包含以下分类模块：
 * - VectorTypes.h: 向量类型定义 (AXIS枚举、Vector类型、轴向量常量)
 * - VectorOperations.h: 基础向量运算 (比较、归一化、叉积、点积、长度、插值、钳制)
 * - VectorUtils.h: 向量工具函数 (转换、极值、方向插值、角度计算、2D旋转、极坐标)
 * 
 * 保持向后兼容性，原有代码无需修改
 */

#pragma once

#ifdef _MSC_VER
#pragma warning(disable:4244)           // double -> int 精度丢失警告
#endif//_MSC_VER

// 包含所有向量相关的分类头文件
#include<hgl/math/VectorTypes.h>
#include<hgl/math/VectorOperations.h>
#include<hgl/math/VectorUtils.h>
