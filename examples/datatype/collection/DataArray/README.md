# DataArray 综合测试套件

本目录包含针对 `DataArray<T>` 模板类的全面测试用例。

## 概述

`DataArray` 是一个动态数组模板类，提供了灵活的内存管理和数据操作功能。本测试套件旨在验证其所有主要功能和边界情况。

## 测试文件

### 1. ComprehensiveTest.cpp - 综合功能测试

**目的**: 全面测试 DataArray 的所有主要功能

**测试覆盖**:

#### 基本功能 (Test 1-4)
- **Test 1**: 基本构造和初始化
  - 默认构造函数
  - 带大小的构造函数
  - 元素初始化验证

- **Test 2**: 内存管理 - Reserve
  - 预留指定空间
  - 验证分配计数
  - 重复预留行为

- **Test 3**: 内存管理 - Resize
  - 扩展数组大小
  - 缩小数组大小
  - 数据保留验证

- **Test 4**: 内存管理 - Expand
  - 增加数组长度
  - 多次扩展操作

#### 数据操作 (Test 5-12)
- **Test 5**: 数据添加 - Append
  - 顺序添加元素
  - 内存自动扩展

- **Test 6**: 数据访问 - At
  - 有效和无效索引
  - 越界检查

- **Test 7**: 数据访问 - ReadAt/WriteAt
  - 单个元素读写
  - 多个元素批量操作
  - 边界检查

- **Test 8**: 数据删除 - Delete (不关心顺序)
  - 删除单个元素
  - 删除多个元素
  - 删除全部元素

- **Test 9**: 数据删除 - DeleteShift (关心顺序)
  - 删除后后续元素前移
  - 顺序保留验证

- **Test 10**: 数据交换 - Exchange
  - 交换两个元素位置
  - 相邻和远距离交换

- **Test 11**: 数据插入 - Insert
  - 在不同位置插入
  - 插入单个和多个元素

- **Test 12**: 数据移动 - Move
  - 向前/向后移动
  - 内存重叠情况

#### 查询和比较 (Test 13-14)
- **Test 13**: 数据查找 - Find
  - 查找存在的元素
  - 查找不存在的元素
  - 从指定位置查找

- **Test 14**: 数组比较 - Compare
  - 相等的数组
  - 不同长度的数组
  - 内容比较

#### 高级功能 (Test 15-21)
- **Test 15**: 清空和释放
  - Clear() - 保留内存
  - Free() - 完全释放

- **Test 16**: 运算符重载
  - operator= (复制赋值)
  - operator[] (索引访问)
  - operator T* (指针转换)
  - operator-> (指针成员访问)

- **Test 17**: 迭代器接口
  - begin() / end()
  - last()
  - 循环遍历

- **Test 18**: 集合操作 - WithoutList
  - 计算差集

- **Test 19**: 字节大小查询
  - GetTotalBytes()
  - GetAllocBytes()

- **Test 20**: SetData 和 Unlink
  - 关联外部数据
  - 解除数据关联

- **Test 21**: Zero 函数
  - 清零所有元素

### 2. NonTrivialTypeTest.cpp - 非平凡类型测试

**目的**: 验证 DataArray 对非平凡类型（包含动态内存的类）的正确处理

**测试覆盖**:

#### 构造和析构 (Test 1-2)
- **Test 1**: 构造和析构
  - 对象创建时调用构造函数
  - 数组销毁时调用析构函数
  - 生命周期追踪

- **Test 2**: Resize 和内存管理
  - 扩展时新元素构造
  - 缩小时旧元素析构
  - 内存正确释放

#### 数据操作 (Test 3-7)
- **Test 3**: Append 操作
  - 复制构造调用
  - 临时对象生命周期

- **Test 4**: Delete 操作
  - 删除时析构调用
  - 后续元素正确管理

- **Test 5**: DeleteShift 操作
  - 保留顺序的删除
  - 正确的数据移动

- **Test 6**: Insert 操作
  - 插入时的复制
  - 现有元素的移动

- **Test 7**: Move 操作
  - 数据块移动
  - 内存重叠处理

#### 赋值和清理 (Test 8-10)
- **Test 8**: Copy 赋值
  - 整个数组的复制
  - 深复制验证

- **Test 9**: Clear 和 Free
  - 清空而保留内存
  - 完全释放

- **Test 10**: 内存泄漏检查
  - 综合操作后的对象追踪
  - 零泄漏验证

### 3. BoundaryAndPerformanceTest.cpp - 边界和性能测试

**目的**: 测试边界条件、特殊情况和性能特征

**测试覆盖**:

#### 边界案例 (Test 1-10)
- **Test 1**: 空数组边界情况
  - 空数组操作
  - 无崩溃验证

- **Test 2**: 单元素数组
  - 特殊的单元素情况
  - last() 和 At() 行为

- **Test 3**: 边界索引操作
  - 第一个和最后一个元素
  - 越界检查

- **Test 4**: 大规模数据操作 (10000 元素)
  - 添加大量元素
  - 删除大量元素
  - 数据完整性

- **Test 5**: 多次 Reserve 操作
  - 反复预留内存
  - 内存预留的单调性

- **Test 6**: 连续 Delete 操作
  - 反复删除同一位置
  - 数据有效性维护

- **Test 7**: 连续 DeleteShift 操作
  - 按顺序删除所有元素
  - 正确的内存释放

- **Test 8**: 随机访问模式
  - At() 和 operator[] 的一致性
  - 多点访问验证

- **Test 9**: 内存重叠情况 (Move)
  - 源和目标区间重叠
  - 向前和向后移动
  - 无数据损坏

- **Test 10**: 数组比较
  - 相同和不同内容
  - 大小差异比较

#### 性能基准
- **基准 1**: Append 性能
  - 添加 100,000 个元素的耗时
  - 吞吐量计算

- **基准 2**: Reserve + Append 性能
  - 预先分配的性能提升
  - 与基准 1 的比较

- **基准 3**: 随机访问性能
  - 1,000,000 次随机访问
  - 缓存行为观察

- **基准 4**: Delete 性能
  - 删除 10,000 个元素

## 编译和运行

### 编译

```bash
# 在项目根目录
cmake -B build
cmake --build build
```

或者单独编译 DataArray 测试：

```bash
cmake -B build
cmake --build build --target DataArrayComprehensiveTest
cmake --build build --target DataArrayNonTrivialTest
cmake --build build --target DataArrayBoundaryTest
```

### 运行

```bash
# 运行综合测试
./build/Examples/CMCore/DataType/Collection/DataArray/DataArrayComprehensiveTest

# 运行非平凡类型测试
./build/Examples/CMCore/DataType/Collection/DataArray/DataArrayNonTrivialTest

# 运行边界和性能测试
./build/Examples/CMCore/DataType/Collection/DataArray/DataArrayBoundaryTest
```

## 测试输出说明

每个测试文件会输出：
- **测试名称**: 当前运行的测试项
- **✓ PASSED**: 测试通过
- **✗ FAILED**: 测试失败（将显示失败原因）
- **统计汇总**: 总通过、失败和成功率

### 示例输出

```
========================================
  DataArray 综合测试套件
========================================

【基本功能】
  Test 1: 基本构造和初始化
    ✓ PASSED
  Test 2: 内存管理 - Reserve
    ✓ PASSED
  ...

========================================
  测试结果汇总
========================================
  总测试数: 21
  通过: 21
  失败: 0
  成功率: 100%
========================================
```

## 测试数据类型

### ComprehensiveTest.cpp
- **int** - 基本 POD 类型

### NonTrivialTypeTest.cpp
- **TrackedObject** - 带动态内存和生命周期追踪的非平凡类

### BoundaryAndPerformanceTest.cpp
- **int** - 适用于大规模操作和性能基准

## 代码检查点

每个测试都使用 `TEST_ASSERT` 宏进行验证：

```cpp
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "  ✗ 失败: " << message << std::endl; \
            return false; \
        } \
    } while(0)
```

## 预期结果

所有测试应该 100% 通过，没有内存泄漏或崩溃。

### 非平凡类型测试的特殊期望

- 每个构造应调用一次构造函数
- 每个销毁应调用一次析构函数
- 构造数 = 析构数（无泄漏）
- 对象生命周期正确管理

### 性能基准的参考值

这些是在现代 CPU 上的典型值（实际值取决于硬件）：

- **Append 100k 元素**: < 5ms
- **Reserve + Append 100k 元素**: < 2ms
- **随机访问 1M 次**: < 10ms
- **Delete 10k 元素**: < 10ms

## 故障排查

### 如果测试失败

1. **检查编译错误**: 确保所有包含的头文件路径正确
2. **检查测试消息**: 失败消息会指出具体的问题
3. **内存泄漏检查**: 在非平凡类型测试中，检查析构调用计数
4. **性能退化**: 如果基准测试明显慢，检查编译优化选项

## 添加新测试

要添加新的测试用例，遵循以下模式：

```cpp
bool test_new_feature()
{
    std::cout << "\n【测试 N: 功能描述】" << std::endl;
    
    DataArray<int> arr;
    // ... 设置测试数据
    
    TEST_ASSERT(condition1, "验证消息1");
    TEST_ASSERT(condition2, "验证消息2");
    
    return true;
}
```

然后在 `main()` 中调用：

```cpp
RUN_TEST(test_new_feature, "Test N: 功能描述");
```

## 相关文件

- `e:\cm\CMCore\inc\hgl\type\DataArray.h` - DataArray 模板定义
- `e:\cm\CMCore\examples\datatype\collection\DataArrayTest.cpp` - 原始测试
- `e:\cm\CMCore\examples\datatype\collection\DataArrayTestEnhanced.cpp` - 增强测试

## 许可和归属

这些测试用例是为 HGL C++ 库编写的综合测试套件的一部分。

## 更新历史

- **v1.0** (2024) - 初始完整测试套件
  - 21 项综合功能测试
  - 10 项非平凡类型测试
  - 11 项边界和性能测试
  - 4 项性能基准测试
