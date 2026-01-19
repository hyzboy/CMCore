# DataArray 和 ArrayList 测试用例说明

## 概述

这些测试用例专门用于测试从 `memcpy` 迁移到 C++20 封装后的内存操作函数的正确性。

## 测试文件

### 1. ArrayListTest.cpp
全面测试 `ArrayList<T>` 模板类的各种操作。

**测试类型：**
- `int` - trivially copyable 类型
- `SimplePOD` - POD 结构体
- `NonTrivialClass` - 包含动态内存和析构函数的非平凡类

**测试操作：**
- Add/RepeatAdd - 测试 `mem_fill_pattern`
- Delete/DeleteShift - 测试 `mem_copy` 和 `mem_move`
- Insert - 测试内存移动
- Move - 测试重叠区域的内存操作
- Exchange - 测试数据交换
- Copy assignment - 测试 `operator=` 和 `mem_copy`

**关键测试点：**
- NonTrivialClass 包含内存泄漏检测
- 统计构造、析构、拷贝、移动次数
- 验证动态内存是否正确释放

### 2. DataArrayTestEnhanced.cpp
深度测试 `DataArray<T>` 的底层内存操作。

**测试类型：**
- `int` - 基本类型
- `Point2D` - POD 结构体
- `TrackedObject` - 带有生命周期追踪的非平凡类

**测试操作：**
- Delete - 测试不同位置的删除（头部、中间、尾部）
- DeleteShift - 重点测试 `mem_move` 的正确性
- Move - 测试重叠区域的 `mem_move`
- Insert - 测试插入时的内存移动
- Copy assignment - 测试 `mem_copy` 的深拷贝

**关键测试点：**
- TrackedObject 追踪所有对象的创建和销毁
- 检测内存泄漏（aliveCount 应该为 0）
- 验证 mem_move 在重叠区域的正确性
- 验证 non-trivial 类型的析构函数是否正确调用

## MemoryUtil.h 中的关键函数

### mem_copy
```cpp
template<typename T>
inline void mem_copy(T *dst, const T *src, const size_t count)
```
- **trivially_copyable**: 使用 `std::memcpy` (快速)
- **non-trivial**: 使用 `std::copy` (正确调用拷贝构造/赋值)

### mem_move
```cpp
template<typename T>
inline void mem_move(T *dst, const T *src, const size_t count)
```
- **trivially_copyable**: 使用 `std::memmove` (处理重叠)
- **non-trivial**: 使用 `std::move` / `std::move_backward` (处理重叠)

### mem_fill_pattern
```cpp
template<typename T>
inline void mem_fill_pattern(T *data, const T *pattern, const size_t count)
```
- **trivially_copyable**: 使用 `std::memcpy` 循环填充
- **non-trivial**: 使用赋值操作符填充

## 可能出现的问题

### 1. mem_move 重叠区域处理不当
**症状**: DeleteShift 或 Move 操作后数据错乱或崩溃

**原因**: 
- 对于 non-trivial 类型，`std::move` 和 `std::move_backward` 的方向选择错误
- 在重叠区域没有正确处理移动方向

**测试**: 
- `TestNonTrivialDeleteShift` (DataArrayTestEnhanced.cpp)
- `TestNonTrivialMove` (DataArrayTestEnhanced.cpp)

### 2. mem_fill_pattern 没有正确调用拷贝构造
**症状**: RepeatAdd 后对象状态不正确，可能有内存泄漏

**原因**:
- non-trivial 类型使用 memcpy 而不是赋值操作符
- 导致浅拷贝，多个对象共享同一块动态内存

**测试**:
- TEST 3.3 in ArrayListTest.cpp (RepeatAdd non-trivial object)

### 3. Delete 操作后没有正确调用析构函数
**症状**: 内存泄漏，aliveCount != 0

**原因**:
- 删除元素时只是移动数据，没有调用被删除元素的析构函数
- 对于 non-trivial 类型需要显式调用析构

**测试**:
- TEST 6.2 in DataArrayTestEnhanced.cpp

### 4. 拷贝赋值没有正确深拷贝
**症状**: 修改一个数组影响到另一个数组

**原因**:
- operator= 中使用 memcpy 导致浅拷贝
- non-trivial 类型应该调用拷贝赋值操作符

**测试**:
- TEST 9 in DataArrayTestEnhanced.cpp (Copy Assignment)

## 运行测试

### 编译
```bash
cd build
cmake --build . --target ArrayListTest
cmake --build . --target DataArrayTestEnhanced
```

### 运行
```bash
# Windows
.\out\Windows_64\ArrayListTest.exe
.\out\Windows_64\DataArrayTestEnhanced.exe

# Linux
./out/Linux_64/ArrayListTest
./out/Linux_64/DataArrayTestEnhanced
```

### 预期输出
- 所有操作应该正常完成，没有崩溃
- NonTrivialClass/TrackedObject 的统计信息应该显示：
  - `Construct count - Destruct count = 0` (无内存泄漏)
  - `aliveCount = 0` (所有对象都已正确销毁)

## 调试建议

1. **使用调试器**: 在崩溃点设置断点，查看调用栈
2. **启用 Address Sanitizer** (如果支持):
   ```cmake
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
   ```
3. **检查输出**: 查看对象创建/销毁日志，确认生命周期正确
4. **逐步测试**: 先测试 trivially copyable 类型 (int)，再测试 non-trivial 类型

## 常见问题排查

### 如果 ArrayListTest 崩溃
1. 检查 `mem_fill_pattern` 实现
2. 检查 `ArrayList::RepeatAdd` 是否正确使用 mem_fill_pattern
3. 检查 non-trivial 类型的拷贝赋值操作符

### 如果 DataArrayTestEnhanced 崩溃
1. 检查 `mem_move` 的重叠处理逻辑
2. 检查 `DataArray::Delete` 和 `DeleteShift` 实现
3. 检查 `DataArray::Move` 实现
4. 使用 TEST 2, 3, 7, 8 来定位具体问题

### 如果有内存泄漏
1. 查看 aliveCount 何时变为非零
2. 检查是否在 Delete/Free 时调用了析构函数
3. 检查 operator= 是否正确处理旧数据

## 类型特征检查

可以添加以下代码来验证类型特征：
```cpp
#include <type_traits>

std::cout << "int is trivially copyable: " 
          << std::is_trivially_copyable_v<int> << std::endl;
std::cout << "SimplePOD is trivially copyable: " 
          << std::is_trivially_copyable_v<SimplePOD> << std::endl;
std::cout << "NonTrivialClass is trivially copyable: " 
          << std::is_trivially_copyable_v<NonTrivialClass> << std::endl;
```

预期输出：
- int: true
- SimplePOD: true
- NonTrivialClass: false
