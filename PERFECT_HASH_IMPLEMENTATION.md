# Perfect Hash 容器实现文档

## 概述

本实现提供了四个基于完美哈希技术的容器，支持O(1)查找和零拷贝序列化：

1. **FlatPerfectHashOrderedSet** - 有序集合，支持O(1)查找和有序遍历
2. **FlatPerfectHashOrderedMap** - 有序映射，支持O(1)查找和按Key有序遍历
3. **FlatPerfectHashUnorderedSet** - 无序集合，支持O(1)查找和插入顺序遍历
4. **FlatPerfectHashUnorderedMap** - 无序映射，支持O(1)查找和插入顺序遍历

## 设计特点

### 1. 完美哈希技术

- **哈希表索引**：使用线性探测哈希表实现O(1)查找
- **负载因子**：有序版本使用1.5x，无序版本使用2x，保证良好性能
- **碰撞处理**：线性探测法，简单高效
- **零开销哈希**：QuickHash.h提供针对平凡类型的优化哈希函数

### 2. 零拷贝序列化

所有容器支持直接内存块序列化：

```cpp
// 序列化（保存到文件）
auto data_ptr = container.GetData();
auto hash_ptr = container.GetHashTable();
int64 count = container.GetCount();
int32_t hash_size = container.GetHashTableSize();
// 写入文件...

// 反序列化（从文件加载）
container.LoadFromBuffers(data_buffer, hash_buffer, count, hash_size);
// 立即可用，无需重建
```

### 3. 平凡类型要求

所有容器仅支持trivially copyable类型：
- 基本类型：int, float, double等
- POD结构体
- 指针类型
- 不支持：std::string, std::vector, 带虚函数的类等

### 4. 内存布局

#### 有序版本（Ordered）
```
[数据数组（排序）] + [哈希表]
   keys[N]              hash_table[M]
   values[N]           (M = N * 1.5)
```

#### 无序版本（Unordered）
```
[数据数组（插入顺序）] + [哈希表]
   keys[N]                hash_table[M]
   values[N]             (M = N * 2)
```

## API 文档

### FlatPerfectHashOrderedSet<T>

**主要方法：**

```cpp
// 添加元素（O(n)，需要维护有序）
int64 Add(const T& value);

// O(1) 查找
bool Contains(const T& value) const;
int64 Find(const T& value) const;

// 删除（O(n)，需要移动元素）
bool Delete(const T& value);

// 有序遍历
for (auto& val : set) { /* ... */ }

// 范围查询
auto it = set.lower_bound(value);
auto it = set.upper_bound(value);

// 序列化
T* GetData();
int32_t* GetHashTable();
int32_t GetHashTableSize();
void LoadFromBuffers(const T* data, const int32_t* hash, 
                     int64 count, int32_t hash_size, bool is_sorted=true);
```

### FlatPerfectHashOrderedMap<K,V>

**主要方法：**

```cpp
// 添加键值对（O(n)）
bool Add(const K& key, const V& value);

// O(1) 查找
bool Get(const K& key, V& value) const;
bool ContainsKey(const K& key) const;
V* GetValuePointer(const K& key);

// 修改
bool Change(const K& key, const V& value);
bool AddOrUpdate(const K& key, const V& value);

// 删除（O(n)）
bool DeleteByKey(const K& key);

// 有序遍历
map.EnumKV([](const K& key, const V& value) { /* ... */ });

// 序列化
K* GetKeyData();
V* GetValueData();
int32_t* GetHashTable();
void LoadFromBuffers(const K* keys, const V* values, 
                     const int32_t* hash, int64 count, 
                     int32_t hash_size, bool is_sorted=true);
```

### FlatPerfectHashUnorderedSet<T>

**主要方法：**

```cpp
// 添加元素（O(1)平均）
int64 Add(const T& value);

// O(1) 查找
bool Contains(const T& value) const;
int64 Find(const T& value) const;

// 删除（O(1)平均，swap-and-pop）
bool Delete(const T& value);

// 插入顺序遍历
for (auto& val : set) { /* ... */ }

// 序列化
T* GetData();
int32_t* GetHashTable();
void LoadFromBuffers(const T* data, const int32_t* hash,
                     int64 count, int32_t hash_size);
```

### FlatPerfectHashUnorderedMap<K,V>

**主要方法：**

```cpp
// 添加（O(1)平均）
bool Add(const K& key, const V& value);

// O(1) 查找
bool Get(const K& key, V& value) const;
V* GetValuePointer(const K& key);

// 修改
bool Change(const K& key, const V& value);
bool AddOrUpdate(const K& key, const V& value);

// 删除（O(1)平均）
bool DeleteByKey(const K& key);

// 插入顺序遍历
map.EnumKV([](const K& key, const V& value) { /* ... */ });

// 序列化
K* GetKeyData();
V* GetValueData();
int32_t* GetHashTable();
void LoadFromBuffers(const K* keys, const V* values,
                     const int32_t* hash, int64 count,
                     int32_t hash_size);
```

## 性能特征

| 操作 | Ordered | Unordered |
|------|---------|-----------|
| 查找 | O(1) | O(1) |
| 插入 | O(n) | O(1)平均 |
| 删除 | O(n) | O(1)平均 |
| 遍历 | O(n)有序 | O(n)插入顺序 |
| 序列化 | O(n) | O(n) |
| 内存 | N * sizeof(T) + 1.5N * 4字节 | N * sizeof(T) + 2N * 4字节 |

## 使用场景

### FlatPerfectHashOrderedSet/Map

✅ **适合：**
- 配置文件：需要快速查找和有序遍历
- 资源索引：需要按名称/ID排序显示
- 静态数据表：数据预排序，需要范围查询
- 字典/目录：需要按字母顺序浏览

❌ **不适合：**
- 频繁插入删除的场景
- 不关心顺序的场景

### FlatPerfectHashUnorderedSet/Map

✅ **适合：**
- 查找表：快速成员测试
- 缓存：ID到对象的映射
- 去重：快速检查重复
- 配置项：键值对查找

❌ **不适合：**
- 需要有序遍历的场景
- 需要范围查询的场景

## 示例代码

### 示例1：配置文件映射

```cpp
// 加载配置文件到有序映射（支持有序遍历）
FlatPerfectHashOrderedMap<int, float> config;

// 从文件读取
config.LoadFromBuffers(key_buffer, value_buffer, 
                       hash_buffer, count, hash_size);

// O(1)查找配置项
float value;
if (config.Get(CONFIG_TIMEOUT, value)) {
    // 使用配置值...
}

// 有序遍历所有配置
config.EnumKV([](const int& key, const float& value) {
    std::cout << "Config " << key << " = " << value << std::endl;
});
```

### 示例2：资源ID映射

```cpp
// 资源ID到索引的映射（无需排序）
FlatPerfectHashUnorderedMap<uint32_t, ResourceInfo> resources;

// 从资源包直接加载
resources.LoadFromBuffers(id_buffer, info_buffer,
                          hash_buffer, count, hash_size);

// O(1)查找资源
ResourceInfo* info = resources.GetValuePointer(resource_id);
if (info) {
    // 使用资源...
}
```

### 示例3：内存映射文件

```cpp
// 映射文件到内存
void* mapped = mmap(nullptr, file_size, PROT_READ, 
                    MAP_PRIVATE, fd, 0);

// 解析文件头
FileHeader* header = (FileHeader*)mapped;
int* keys = (int*)((char*)mapped + header->keys_offset);
float* values = (float*)((char*)mapped + header->values_offset);
int32_t* hash = (int32_t*)((char*)mapped + header->hash_offset);

// 零拷贝加载
FlatPerfectHashOrderedMap<int, float> map;
map.LoadFromBuffers(keys, values, hash, 
                    header->count, header->hash_size);

// 立即可用，无需拷贝或重建
float val;
map.Get(key, val);
```

## 实现文件

- **inc/hgl/util/hash/QuickHash.h** - 优化的哈希函数
- **inc/hgl/type/FlatPerfectHashOrderedSet.h** - 有序集合
- **inc/hgl/type/FlatPerfectHashOrderedMap.h** - 有序映射
- **inc/hgl/type/FlatPerfectHashUnorderedSet.h** - 无序集合
- **inc/hgl/type/FlatPerfectHashUnorderedMap.h** - 无序映射

## 测试文件

- **examples/datatype/collection/Map/PerfectHashMap/PerfectHashOrderedMapBasic.cpp**
- **examples/datatype/collection/Map/PerfectHashMap/PerfectHashOrderedMapSerialization.cpp**
- **examples/datatype/collection/Map/PerfectHashMap/PerfectHashOrderedSetBasic.cpp**
- **examples/datatype/collection/Map/PerfectHashMap/PerfectHashUnorderedMapBasic.cpp**
- **examples/datatype/collection/Map/PerfectHashMap/PerfectHashUnorderedMapSerialization.cpp**
- **examples/datatype/collection/Map/PerfectHashMap/PerfectHashUnorderedSetBasic.cpp**
- **examples/datatype/collection/Map/PerfectHashMap/PerfectHashContainersComprehensive.cpp**

## 注意事项

1. **平凡类型限制**：只支持trivially copyable类型
2. **静态数据优化**：针对静态/只读数据集设计
3. **文件格式**：需要自行设计文件头格式保存元数据
4. **字节序**：跨平台使用需要处理字节序问题
5. **版本兼容**：修改结构时需要考虑版本兼容性

## 总结

完美哈希容器提供了：
- ✅ O(1)查找性能
- ✅ 零拷贝序列化
- ✅ 内存映射友好
- ✅ 有序/无序两种版本
- ✅ 简单高效的实现

适合用于配置文件、资源索引、静态数据表等场景。
