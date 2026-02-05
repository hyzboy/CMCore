# 静态完美哈希设计方案

## 需求分析

### 用户需求（游戏场景）

```
编辑器阶段（离线）：
  静态数据 → 预计算排列 → 构建完美哈希 → 保存FLAT数据文件

游戏运行时（在线）：
  加载FLAT数据文件 → 直接使用（无需重建） → 最快查找性能
```

### 核心特点
1. **纯静态**：数据在编辑器确定，运行时不变
2. **预计算**：在离线阶段完成所有计算
3. **零碰撞**：真正的完美哈希，无冲突
4. **极速查找**：O(1) 最坏情况，无探测循环
5. **即加载即用**：无需运行时重建哈希表

---

## 这正是完美哈希的理想场景！

### 为什么适合？
- ✅ 静态键集（游戏配置、资源ID、技能表等）
- ✅ 离线构建时间不受限（可以花几秒甚至几分钟）
- ✅ 运行时性能至关重要（游戏帧率）
- ✅ 内存映射友好（快速加载大数据）
- ✅ 零拷贝加载（无需解析）

---

## 设计方案

### 架构：两阶段设计

```
┌─────────────────────────────────────────────────────────┐
│ 阶段1：编辑器/构建工具（离线，C++）                        │
│                                                           │
│  输入：                                                   │
│    - 键值对列表（K, V）                                   │
│    - 配置（算法选项、优化级别）                            │
│                                                           │
│  处理：                                                   │
│    1. 键集分析（数量、类型、分布）                         │
│    2. 选择MPH算法（CHD/BDZ/PTHash）                       │
│    3. 构建完美哈希函数                                     │
│    4. 生成哈希参数（g-values）                            │
│    5. 验证零碰撞                                          │
│                                                           │
│  输出：                                                   │
│    - .mph 文件（二进制格式）                              │
│      [Header | Hash Params | Keys | Values]             │
└─────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────┐
│ 阶段2：游戏运行时（在线，C++）                             │
│                                                           │
│  加载：                                                   │
│    - mmap 或直接读取 .mph 文件                           │
│    - 解析文件头                                          │
│    - 映射数据指针（零拷贝）                               │
│                                                           │
│  查找：                                                   │
│    1. 计算完美哈希：h = MPH(key, params)                 │
│    2. 直接访问：value = values[h]                        │
│    3. （可选）验证键匹配：keys[h] == key                  │
│                                                           │
│  特点：                                                   │
│    - 无循环、无探测、无比较（理想情况）                    │
│    - O(1) 最坏情况                                       │
│    - 缓存友好                                            │
└─────────────────────────────────────────────────────────┘
```

---

## 算法选择

### 候选算法对比

| 算法 | 空间 | 构建时间 | 查找速度 | 复杂度 | 推荐场景 |
|------|------|---------|---------|--------|---------|
| **CHD** | 1.23N | 快 | 快 | 中等 | **推荐**：平衡 |
| **BDZ** | 1.23N | 快 | 较快 | 简单 | 小规模（<100万） |
| **PTHash** | 1.02N | 慢 | 最快 | 复杂 | 大规模（>百万） |
| **RecSplit** | 1.01N | 很慢 | 快 | 很复杂 | 空间极限场景 |

### 推荐：CHD 算法

**原因**：
1. **空间合理**：仅需 1.23N（23%开销）
2. **构建快**：线性时间 O(N)
3. **查找快**：2-3次哈希计算
4. **实现适中**：不太复杂，可自己实现
5. **稳定可靠**：成熟算法，广泛使用

**CHD 原理简述**：
```
1. 将键分配到桶中（第一级哈希）
2. 对每个桶独立计算位移值（displacement）
3. 最终位置 = (bucket_base + displacement[h1(key)] + h2(key)) % N
```

---

## 数据结构设计

### 文件格式（.mph 格式）

```cpp
// 文件头（固定大小，方便解析）
struct MPHFileHeader {
    char magic[4];           // "MPH\0"
    uint32_t version;        // 文件格式版本
    uint32_t algorithm;      // 算法类型（CHD=1, BDZ=2等）
    uint64_t num_keys;       // 键数量
    uint32_t key_size;       // sizeof(K)
    uint32_t value_size;     // sizeof(V)
    
    // CHD 特定参数
    uint32_t num_buckets;    // 桶数量
    uint32_t disp_table_size;// 位移表大小
    
    // 数据偏移
    uint64_t disp_table_offset;  // 位移表在文件中的偏移
    uint64_t keys_offset;        // 键数组偏移
    uint64_t values_offset;      // 值数组偏移
    
    uint64_t file_size;      // 文件总大小（校验用）
    uint32_t checksum;       // CRC32校验和
};

// 文件布局
[MPHFileHeader]              // 128字节（对齐）
[Displacement Table]         // disp_table_size * sizeof(uint16_t)
[Padding to 64-byte align]
[Keys Array]                 // num_keys * sizeof(K)
[Padding to 64-byte align]
[Values Array]               // num_keys * sizeof(V)
```

### 运行时数据结构

```cpp
template<typename K, typename V>
class StaticPerfectHashMap {
private:
    // 文件映射（mmap或内存）
    void* file_data;
    size_t file_size;
    
    // 解析后的指针（零拷贝）
    MPHFileHeader* header;
    uint16_t* disp_table;    // 位移表
    K* keys;                 // 键数组
    V* values;               // 值数组
    
    // MPH 参数（从header复制，加速访问）
    uint32_t num_buckets;
    uint64_t num_keys;
    
public:
    // 加载（mmap版本）
    bool LoadFromFile(const char* path);
    
    // 加载（内存拷贝版本）
    bool LoadFromMemory(const void* data, size_t size);
    
    // 查找（O(1) 最坏情况）
    const V* Get(const K& key) const;
    
    // 检查键是否存在
    bool Contains(const K& key) const;
    
    // 获取值（带默认值）
    V GetOrDefault(const K& key, const V& default_val) const;
};
```

---

## 查找算法（运行时）

### CHD 查找伪代码

```cpp
template<typename K, typename V>
const V* StaticPerfectHashMap<K, V>::Get(const K& key) const {
    // 第一级哈希：确定桶
    uint64_t h1 = Hash1(key);
    uint32_t bucket = h1 % num_buckets;
    
    // 第二级哈希：桶内位移
    uint64_t h2 = Hash2(key);
    uint16_t displacement = disp_table[bucket];
    
    // 计算最终位置（完美哈希，零碰撞）
    uint64_t index = (h1 + displacement * h2) % num_keys;
    
    // 验证键（防止不在集合中的键）
    if (keys[index] != key)
        return nullptr;  // 键不存在
    
    return &values[index];
}
```

### 性能分析
- **哈希计算**：2次（Hash1 + Hash2）
- **查表**：2次（disp_table + keys/values）
- **比较**：1次（keys[index] == key）
- **总开销**：~10-20ns（现代CPU）

---

## 构建工具设计

### Builder API

```cpp
template<typename K, typename V>
class PerfectHashMapBuilder {
public:
    // 添加键值对
    void Add(const K& key, const V& value);
    
    // 批量添加
    void AddBatch(const K* keys, const V* values, size_t count);
    
    // 构建完美哈希（主要计算在这里）
    bool Build();
    
    // 保存到文件
    bool SaveToFile(const char* path);
    
    // 获取统计信息
    struct Stats {
        size_t num_keys;
        size_t hash_table_size;
        size_t file_size;
        double space_overhead;
        double build_time_ms;
    };
    Stats GetStats() const;

private:
    std::vector<std::pair<K, V>> data;
    
    // CHD 内部结构
    std::vector<uint16_t> disp_table;
    std::vector<K> sorted_keys;
    std::vector<V> sorted_values;
    uint32_t num_buckets;
    
    // 构建算法（CHD）
    bool BuildCHD();
    bool AssignBuckets();
    bool ComputeDisplacements();
    bool VerifyPerfect();
};
```

### 构建流程（CHD 算法）

```cpp
bool PerfectHashMapBuilder::BuildCHD() {
    // 1. 确定桶数量（通常 = sqrt(N)）
    num_buckets = (uint32_t)ceil(sqrt(data.size()));
    
    // 2. 第一级哈希：分配键到桶
    std::vector<std::vector<K>> buckets(num_buckets);
    for (auto& [key, val] : data) {
        uint32_t bucket = Hash1(key) % num_buckets;
        buckets[bucket].push_back(key);
    }
    
    // 3. 按桶大小排序（大桶优先处理）
    std::sort(buckets, [](auto& a, auto& b) { 
        return a.size() > b.size(); 
    });
    
    // 4. 为每个桶计算位移值
    disp_table.resize(num_buckets, 0);
    std::vector<bool> occupied(data.size(), false);
    
    for (uint32_t b = 0; b < num_buckets; ++b) {
        // 尝试不同的位移值
        for (uint16_t disp = 0; disp < 65535; ++disp) {
            bool collision = false;
            std::vector<size_t> positions;
            
            // 检查这个位移是否导致碰撞
            for (const K& key : buckets[b]) {
                uint64_t h1 = Hash1(key);
                uint64_t h2 = Hash2(key);
                size_t pos = (h1 + disp * h2) % data.size();
                
                if (occupied[pos]) {
                    collision = true;
                    break;
                }
                positions.push_back(pos);
            }
            
            // 找到无碰撞的位移值
            if (!collision) {
                disp_table[b] = disp;
                for (size_t pos : positions) {
                    occupied[pos] = true;
                }
                break;
            }
        }
    }
    
    // 5. 验证完美性（所有位置都已占用）
    return std::all_of(occupied.begin(), occupied.end(), 
                      [](bool b) { return b; });
}
```

---

## 实现计划

### 阶段1：基础实现（1-2天）
- [ ] 实现 `StaticPerfectHashMap<K,V>` 运行时容器
- [ ] 实现基本的文件格式读写
- [ ] 实现简单的 CHD 算法
- [ ] 单元测试（小规模数据）

### 阶段2：优化和完善（2-3天）
- [ ] 实现 `PerfectHashMapBuilder<K,V>` 构建工具
- [ ] 优化 CHD 算法（并行化、内存优化）
- [ ] 添加文件校验和压缩
- [ ] 支持 mmap 加载
- [ ] 大规模测试（10万-100万键）

### 阶段3：易用性和工具（1-2天）
- [ ] 命令行工具：`mph-builder input.json output.mph`
- [ ] 统计和可视化工具
- [ ] 文档和示例
- [ ] 性能基准测试

---

## 使用示例

### 编辑器/构建脚本

```cpp
// 构建阶段（离线）
#include <hgl/type/StaticPerfectHashMap.h>

void BuildGameConfigHash() {
    PerfectHashMapBuilder<uint32_t, ConfigData> builder;
    
    // 从配置文件加载
    auto configs = LoadConfigsFromJSON("game_configs.json");
    
    for (auto& [id, config] : configs) {
        builder.Add(id, config);
    }
    
    // 构建完美哈希
    if (!builder.Build()) {
        std::cerr << "Failed to build perfect hash!" << std::endl;
        return;
    }
    
    // 保存到文件
    builder.SaveToFile("configs.mph");
    
    // 打印统计
    auto stats = builder.GetStats();
    std::cout << "Keys: " << stats.num_keys << std::endl;
    std::cout << "File size: " << stats.file_size << " bytes" << std::endl;
    std::cout << "Space overhead: " << stats.space_overhead << "%" << std::endl;
}
```

### 游戏运行时

```cpp
// 运行时（在线）
#include <hgl/type/StaticPerfectHashMap.h>

class GameConfigManager {
private:
    StaticPerfectHashMap<uint32_t, ConfigData> configs;
    
public:
    bool Init() {
        // 一次性加载（mmap，零拷贝）
        return configs.LoadFromFile("configs.mph");
    }
    
    const ConfigData* GetConfig(uint32_t id) {
        // O(1) 查找，零碰撞
        return configs.Get(id);
    }
};

// 使用
GameConfigManager mgr;
mgr.Init();  // 快速加载

// 游戏循环中
const ConfigData* cfg = mgr.GetConfig(SKILL_FIREBALL_ID);
if (cfg) {
    // 使用配置...
}
```

---

## 性能预估

### 构建性能（离线）
- **小规模** (1K keys): ~1ms
- **中规模** (10K keys): ~10ms
- **大规模** (100K keys): ~100ms
- **超大规模** (1M keys): ~1-2秒

### 运行时性能
- **加载时间** (mmap): ~1ms（任意大小，因为是虚拟内存映射）
- **查找时间**: ~10-20ns（单次查找）
- **内存开销**: 123%（相比原始数据）

### 对比当前实现

| 指标 | 当前实现 | 真正MPH | 提升 |
|------|---------|---------|------|
| 查找最坏 | O(1)平均 | O(1)最坏 | 稳定性↑ |
| 空间 | 150-200% | 123% | 节省20-40% |
| 碰撞 | 有（探测） | 无 | 性能↑ |
| 加载 | 需重建索引 | 零拷贝 | 速度↑↑ |

---

## 优势与限制

### 优势
1. ✅ **真正的O(1)**：最坏情况，无探测
2. ✅ **零碰撞**：无冲突处理代码
3. ✅ **空间最优**：仅23%开销
4. ✅ **极速加载**：mmap零拷贝
5. ✅ **缓存友好**：无间接跳转
6. ✅ **可预测性**：性能稳定

### 限制
1. ❌ **纯静态**：无法添加/删除
2. ❌ **构建开销**：需要离线构建工具
3. ❌ **键验证**：需要额外存储键（或接受假阳性）

---

## 文件组织

```
inc/hgl/type/
  StaticPerfectHashMap.h      // 运行时容器（游戏用）
  PerfectHashMapBuilder.h     // 构建工具（编辑器用）
  
src/Type/
  StaticPerfectHashMap.cpp
  PerfectHashMapBuilder.cpp
  
tools/mph-builder/
  main.cpp                     // 命令行工具
  
examples/
  static_mph_game_config.cpp   // 游戏配置示例
  static_mph_resource_id.cpp   // 资源ID映射示例
  
doc/
  STATIC_PERFECT_HASH.md       // 完整文档
```

---

## 总结

### 这个设计的核心价值

1. **真正的完美哈希**：零碰撞，O(1)最坏情况
2. **游戏优化友好**：
   - 编辑器预计算（时间不敏感）
   - 运行时零开销（性能关键）
   - mmap 快速加载
3. **工程实用性**：
   - 简单易用的API
   - 完整的工具链
   - 可验证的正确性

### 下一步行动

**建议优先级**：
1. **原型验证**（1天）：实现最小可用版本
2. **性能测试**（1天）：验证性能提升
3. **完整实现**（2-3天）：工具链+文档
4. **集成到项目**（1天）：示例和测试

**是否开始实现？**
如果您同意这个设计方案，我可以立即开始实现原型。
