# DataArray 测试套件 - 快速开始指南

## 文件结构

```
DataArray/
├── CMakeLists.txt                          # CMake 配置文件
├── README.md                               # 详细文档
├── QUICK_START.md                          # 本文件
├── ComprehensiveTest.cpp                   # 综合功能测试 (21 项)
├── NonTrivialTypeTest.cpp                  # 非平凡类型测试 (10 项)
└── BoundaryAndPerformanceTest.cpp          # 边界和性能测试 (11 + 4 基准)
```

## 快速开始

### 1. 编译测试

在项目根目录运行：

```bash
# 完整构建
cmake -B build
cmake --build build

# 或者只构建 DataArray 测试
cmake -B build
cmake --build build --target DataArrayComprehensiveTest
cmake --build build --target DataArrayNonTrivialTest
cmake --build build --target DataArrayBoundaryTest
```

### 2. 运行测试

```bash
# Windows
build\Examples\CMCore\DataType\Collection\DataArray\Debug\DataArrayComprehensiveTest.exe
build\Examples\CMCore\DataType\Collection\DataArray\Debug\DataArrayNonTrivialTest.exe
build\Examples\CMCore\DataType\Collection\DataArray\Debug\DataArrayBoundaryTest.exe

# Linux/Mac
./build/Examples/CMCore/DataType/Collection/DataArray/DataArrayComprehensiveTest
./build/Examples/CMCore/DataType/Collection/DataArray/DataArrayNonTrivialTest
./build/Examples/CMCore/DataType/Collection/DataArray/DataArrayBoundaryTest
```

### 3. 验证结果

每个测试都会输出：
- 各个测试项的 ✓ PASSED 或 ✗ FAILED
- 最后的统计汇总

示例：
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

## 三大测试类别

### 1. ComprehensiveTest.cpp - 综合功能测试

覆盖 DataArray 的所有主要功能：

| 测试 | 功能 | 项数 |
|-----|------|------|
| 1-4 | 基本功能（构造、Reserve、Resize、Expand） | 4 |
| 5-12 | 数据操作（Append、At、Read/Write、Delete、Exchange、Insert、Move） | 8 |
| 13-14 | 查询和比较（Find、Compare） | 2 |
| 15-21 | 高级功能（Clear/Free、运算符、迭代器、WithoutList、字节大小、SetData、Zero） | 7 |

**运行时间**: < 100ms

### 2. NonTrivialTypeTest.cpp - 非平凡类型测试

验证 DataArray 对包含动态内存的类的支持：

| 测试 | 内容 |
|-----|------|
| 1-2 | 构造/析构、Resize 生命周期 |
| 3-7 | Append、Delete、DeleteShift、Insert、Move |
| 8-10 | Copy赋值、Clear/Free、内存泄漏检查 |

**关键验证**: 构造数 = 析构数（零泄漏）

**运行时间**: < 200ms

### 3. BoundaryAndPerformanceTest.cpp - 边界和性能测试

包括边界案例和性能基准：

| 类别 | 内容 | 项数 |
|-----|------|------|
| 边界 | 空数组、单元素、索引边界、大规模(10k)、Reserve多次、连续Delete | 6 |
| 边界 | DeleteShift连续、随机访问、内存重叠(Move)、数组比较 | 5 |
| 性能 | Append 100k、Reserve+Append 100k、随机访问 1M、Delete 10k | 4 |

**运行时间**: 1-2 秒（因为有性能基准）

## 测试数据类型

- **ComprehensiveTest**: `int` (POD 类型)
- **NonTrivialTypeTest**: `TrackedObject` (带追踪的非平凡类)
- **BoundaryAndPerformanceTest**: `int` (适合大规模操作)

## 典型输出解读

### 成功的测试

```
【基本功能】
  Test 1: 基本构造和初始化
    ✓ PASSED
```

### 失败的测试

```
【基本功能】
  Test 1: 基本构造和初始化
    ✗ 失败: 默认构造后的count应为0
```

### 统计行

```
总测试数: 21    - 运行的总测试项数
通过: 21        - 通过的测试数
失败: 0         - 失败的测试数
成功率: 100%    - (通过数 / 总数) * 100
```

## 性能基准解读

基准测试会输出操作耗时和吞吐量：

```
基准 1: Append 100000个元素...
  耗时: 2 ms
  速率: 50000000 ops/sec
```

参考值（现代 CPU）：
- Append 100k: < 5ms（预分配更快 < 2ms）
- 随机访问 1M: < 20ms
- Delete 10k: < 10ms

如果明显更慢，检查：
1. 编译优化级别（应使用 -O2 或 -O3）
2. 系统负载
3. CPU 频率

## 编译选项建议

```bash
# Release 构建（推荐用于性能测试）
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 带调试符号的 Release
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo
```

## 常见问题

### Q: 测试编译失败
A: 检查：
- CMakeLists.txt 路径是否正确
- 是否有 `#include<hgl/type/DataArray.h>`
- 依赖的 CMCore 库是否已编译

### Q: 测试运行时崩溃
A: 可能原因：
- 内存泄漏（非平凡类型测试）
- 缓冲区溢出（边界测试）
- 无效指针访问

运行调试器查看崩溃堆栈：
```bash
gdb ./build/Examples/CMCore/DataType/Collection/DataArray/DataArrayComprehensiveTest
(gdb) run
```

### Q: 性能基准明显慢
A: 检查：
- 是否使用 Debug 构建（应使用 Release）
- 系统是否忙碌
- 编译器优化选项

## 扩展和修改

要添加新的测试：

1. 在对应的 .cpp 文件中添加函数：
```cpp
bool test_new_feature()
{
    DataArray<int> arr;
    // ... 测试代码
    TEST_ASSERT(condition, "错误消息");
    return true;
}
```

2. 在 main() 中注册测试：
```cpp
RUN_TEST(test_new_feature, "Test N: 新功能");
```

3. 重新编译运行

## 详细文档

完整的文档请参考 [README.md](README.md)，包含：
- 每项测试的详细说明
- 预期结果
- 代码检查点解释
- 故障排查指南

## 许可

这些测试用例是 HGL C++ 库的一部分。

---

**最后更新**: 2024-01-19
**测试版本**: 1.0
**总测试数**: 42 (不含性能基准)
