# DataArray Non-Trivial Type 崩溃问题修复说明

## 问题描述

程序在使用 `ArrayList<NonTrivialClass>` 时崩溃，崩溃堆栈显示在 `NonTrivialClass::operator=` 中访问未初始化的 `std::string` 导致崩溃。

## 根本原因

`DataArray` 使用 `hgl_malloc` 分配原始内存，但对于 **non-trivial 类型**（包含析构函数、`std::string` 等），这些内存上的对象没有被构造。后续使用 `mem_copy` 时，它会调用赋值操作符，但赋值操作符假设目标对象已经构造，导致崩溃。

## 修复方案

### 1. 修改 `DataArray::Resize()` (CMCore/inc/hgl/type/DataArray.h)

**问题**: 增加大小时，新分配的元素没有被构造。

**修复**: 对于 non-trivial 类型，使用 placement new 构造新元素：

```cpp
int64 Resize(int64 size)
{
    const int64 old_count = count;
    
    Reserve(size);

    // 对于 non-trivial 类型，需要构造新增的对象
    if constexpr(!std::is_trivially_constructible_v<T>)
    {
        if(size > old_count)
        {
            // 构造新增的元素
            for(int64 i = old_count; i < size; i++)
            {
                new (items + i) T();  // placement new
            }
        }
        else if(size < old_count)
        {
            // 析构移除的元素
            for(int64 i = size; i < old_count; i++)
            {
                items[i].~T();
            }
        }
    }

    count=size;
    return count;
}
```

### 2. 修改 `DataArray::Reserve()` (CMCore/inc/hgl/type/DataArray.h)

**问题**: 对 non-trivial 类型使用 `realloc` 会导致浅拷贝。

**修复**: 对于 non-trivial 类型，手动分配新内存并移动对象：

```cpp
bool Reserve(int64 size)
{
    if(size<=alloc_count)
        return(true);

    const int64 new_alloc_count = power_to_2(size);

    if(!items)
    {
        items=hgl_align_malloc<T>(new_alloc_count);
        alloc_count = new_alloc_count;
    }
    else
    {
        // 对于 non-trivial 类型，不能直接使用 realloc
        if constexpr(!std::is_trivially_copyable_v<T>)
        {
            T* new_items = hgl_align_malloc<T>(new_alloc_count);
            
            if(new_items)
            {
                // 移动已构造的对象到新位置
                for(int64 i = 0; i < count; i++)
                {
                    new (new_items + i) T(std::move(items[i]));
                    items[i].~T();
                }
                
                hgl_free(items);
                items = new_items;
                alloc_count = new_alloc_count;
            }
            else
            {
                return false;
            }
        }
        else
        {
            items=hgl_align_realloc<T>(items,new_alloc_count);
            alloc_count = new_alloc_count;
        }
    }

    return(items!=nullptr);
}
```

### 3. 修改 `DataArray::Free()` (CMCore/inc/hgl/type/DataArray.h)

**问题**: 释放内存前没有析构对象。

**修复**: 对于 non-trivial 类型，先析构所有对象：

```cpp
void Free()
{
    // 对于 non-trivial 类型，需要先析构所有对象
    if constexpr(!std::is_trivially_destructible_v<T>)
    {
        for(int64 i = 0; i < count; i++)
        {
            items[i].~T();
        }
    }
    
    SAFE_FREE(items);

    count=0;
    alloc_count=0;
}
```

## 测试结果

运行简化版测试（`ArrayListTest.exe`）：
- ✅ 添加 3 个 NonTrivialClass 对象成功
- ✅ 对象构造和析构次数匹配
- ✅ 没有内存泄漏
- ✅ 没有崩溃

## 关键点

1. **Trivially Copyable vs Non-Trivial**: 
   - Trivially copyable 类型（如 `int`, POD struct）可以安全使用 `memcpy/realloc`
   - Non-trivial 类型（含 `std::string`, 虚函数, 析构函数）必须调用构造/析构/移动

2. **使用 `constexpr if`**: 
   - 编译时根据类型特征选择不同的代码路径
   - 零运行时开销

3. **Placement New**: 
   - 在已分配的内存上构造对象：`new (ptr) T()`
   - 必须手动调用析构：`ptr->~T()`

## 下一步

建议：
1. 运行完整的测试套件（`ArrayListTest` 和 `DataArrayTestEnhanced`）
2. 在实际项目中测试修复是否有效
3. 考虑添加编译时断言来检测类型特征
