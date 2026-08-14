# BlockPool / TypedBlockAllocator 块内存分配体系

> 面向 GPU（SSBO / 显存映射）的固定块号内存分配栈。所有分配器对外只暴露 `uint32` 块号，
> shader 中用 `block_index * block_size` 直接寻址；块号 0 作为"空 / 错误"哨兵，永远不被使用。

## 相关文件

| 文件 | 作用 |
| --- | --- |
| `inc/hgl/type/MemoryAllocator.h` | 内存分配抽象接口（纯虚注入缝） |
| `inc/hgl/type/BlockAllocator.h` | 块号链管理器（引擎已有，本体系复用它） |
| `inc/hgl/type/BlockPool.h` | 顶级块池：块号 ↔ 字节地址的桥梁 |
| `inc/hgl/type/TypedBlockAllocator.h` | 绑定 C++ 结构的类型化子分配器 |
| `examples/datatype/TypedBlockAllocatorTest.cpp` | 随机多材质测试示例 |

---

## 1. 设计目标

这套分配栈要解决 GPU 数据区的几个具体问题：

- **块号即句柄**：分配结果是一个 `uint32` 块索引，不是裸指针。没有野指针问题，
  shader 端用 `block_index * block_size` 即可算出偏移；块号 0 表示"空 / 出错"。
- **内存来源可替换**：字节内存既可以是 CPU 堆（`hgl_align_realloc`），也可以是
  `vkMapMemory` 映射回来的显存——通过 `AbstractMemoryAllocator` 纯虚接口注入。
- **预算式分配**：材质数据区规模可控（远小于 1GB），一次性预留固定容量，块号稳定不变。
- **缓存友好**：类型化子分配器按"批"向上级申请连续块，同类型对象在块池中局部连续。

---

## 2. 架构分层

```
┌──────────────────────────────────────────────────────────────┐
│  GPU Shader                                                    │
│  block_index * block_size  →  直接寻址                          │
└──────────────────────────────────────────────────────────────┘
                        ▲  只与 uint32 块号打交道
┌──────────────────────────────────────────────────────────────┐
│  TypedBlockAllocator<T>        类型化子分配器（按对象大小切块）   │
│    slot_blocks = ceil(sizeof(T) / block_size)                 │
│    Acquire()/Release() 返回/回收块号；按批向上级申请            │
└──────────────────────────────────────────────────────────────┘
                        ▲  pool->Acquire(count) / Release(start)
┌──────────────────────────────────────────────────────────────┐
│  BlockPool                    顶级块池                          │
│    组合 BlockAllocator(块号链) + AbstractMemoryAllocator(字节)  │
│    块号 → 字节地址：base + index * block_size                  │
└──────────────────────────────────────────────────────────────┘
                        ▲  /  ▲
┌──────────────────────────┐  ┌────────────────────────────────┐
│  BlockAllocator            │  │  AbstractMemoryAllocator         │
│  （只管理块号链，合并空闲）  │  │  （纯虚：AllocMemory/Free/Write） │
│  节点: UserNode/ChainNode  │  │  实现: MemoryAllocator(堆)       │
│                            │  │        或 vkMemory 映射实现      │
└──────────────────────────┘  └────────────────────────────────┘
```

---

## 3. AbstractMemoryAllocator / MemoryAllocator

内存来源的**注入缝**。`BlockPool` 只依赖这个抽象接口，不关心底层是堆还是显存。

```cpp
class AbstractMemoryAllocator
{
protected:
    void  *memory_block;      // 内存地址
    uint64 data_size;         // 数据长度
    uint64 alloc_unit_size;   // 分配单位长度（分配长度必须为其整倍数）
    uint64 alloc_size;        // 实际分配长度

    virtual bool AllocMemory()=0;   // 分配指定空间

public:
    virtual const uint64 GetSize         ()const;  // 所需内存大小
    virtual const uint64 GetAllocUnitSize()const;  // 分配单元大小
    virtual const uint64 GetAllocSize    ()const;  // 实际分配大小

    virtual const bool CanRealloc()const=0;       // 是否可重新分配

    virtual void *Get(){return memory_block;}     // 取得内存块地址
    virtual void *Get(const uint64 offset);       // 取得偏移地址

    virtual void SetAllocUnitSize(const uint64 size);  // 设置分配单元长度
    virtual uint64 ComputeAllocSize(const uint64 size); // 计算实际需分配容量

    bool Reserve(const uint64);       // 分配指定长度的空间
    virtual void Free()=0;            // 释放
    virtual void Clear(){data_size=0;}
    virtual bool Write(const void *source,const uint64 offset,const uint64 size)=0;
};
```

- 纯虚接口：`AllocMemory()`、`CanRealloc()`、`Free()`、`Write()`。
- 非虚工具方法：`Reserve()`、`SetAllocUnitSize()`、`ComputeAllocSize()`、`Get()` 等。
- `MemoryAllocator` 是其堆实现，内部用 `hgl_align_realloc` 完成对齐分配，`CanRealloc()==true`。
- 若要接显存，只需实现一个新的子类（如 `VkMemoryAllocator`），把 `vkMapMemory` 的映射指针
  作为 `memory_block` 返回即可，`BlockPool` 无需任何改动。

> 注意：`BlockPool` **不拥有**注入的分配器，调用方需保证其生命周期不短于块池。

---

## 4. BlockAllocator（块号链）

引擎已有的底层结构，`BlockPool` 直接复用。它**只管理块号链，不管理实际数据**：

- 把空间切成固定长度的单位，用链表管理空闲与占用情况。
- 释放时自动**合并相邻**空闲节点；没有相邻节点则新建节点。
- 申请时查找**最合适**的节点（`Acquire(count) -> UserNode*`）。

```cpp
struct UserNode { int start, count; ... };   // 用户占用节点
struct ChainNode { ChainNode *prev,*next; int start,count; ... }; // 空闲链节点

bool Init(const int mc);
UserNode *Acquire(const int acquire_count);  // 请求一个数据区块
bool Release(UserNode *ud);                  // 释放一个数据区块
```

`BlockPool` 通过 `UserNode` 拿到起始块号，并用一张 `UnorderedMap<start, UserNode*>` 做反查，
从而让 `Release(start)` 只需起始块号即可 O(1) 找到占用节点、得知连续长度。

---

## 5. BlockPool（顶级块池）

把"块号管理"和"字节内存"两件事粘合起来，对外统一暴露 `uint32` 块号接口。

```cpp
class BlockPool
{
    BlockAllocator           block_alloc;       // 块号分配器
    AbstractMemoryAllocator *memory_allocator;  // 底层字节内存（注入，非拥有）

    uint32 block_size;   // 每块字节数（默认 16）
    uint32 max_count;    // 总块数

    UnorderedMap<uint32, BlockAllocator::UserNode*> node_by_start; // 起始块号反查
};
```

### 主要接口

| 方法 | 说明 |
| --- | --- |
| `bool Init(ma, block_count, block_size=16)` | 初始化；`block_count>=2`，内部 `Acquire(1)` 永久占用 0 号块 |
| `uint32 Acquire(count)` | 申请 `count` 个**连续**块，返回起始块号；失败返回 0 |
| `bool Release(start)` | 释放起始块号为 `start` 的连续块；`start==0` 或非法返回 `false` |
| `void *GetData(index)` / `T *GetData<T>(index)` | 块号 → 字节地址 / 类型指针（CPU 侧直读直写） |
| `void *GetBase()` | 底层内存基址 |
| `GetBlockSize() / GetMaxCount() / GetFreeCount()` | 块大小 / 总块数 / 空闲块数 |

### 关键行为

- **0 号块保留**：`Init` 末尾 `Acquire(1)` 永久占用块 0，此后 `Acquire` 返回的块号恒 `>=1`；
  `Acquire(0)` 返回 0，`Release(0)` 恒失败。因此池必须至少 2 块。
- **字节地址公式**：`GetData(index) == GetBase() + index * block_size`。
- **反查释放**：`Release(start)` 通过 `node_by_start` 反查 `UserNode`，无需调用方记住 `count`。

---

## 6. TypedBlockAllocator\<T\>（类型化子分配器）

绑定一个 C++ 结构 `T` 的子分配器。它是面向使用者最常用的一层：

```cpp
template<typename T>
class TypedBlockAllocator
{
    static_assert(std::is_trivially_copyable_v<T>,
        "TypedBlockAllocator<T> requires T to be trivially copyable (POD).");

    BlockPool *pool;         // 上级块池（非拥有）
    uint32 slot_blocks;      // 每对象块数 = ceil(sizeof(T)/block_size)
    uint32 batch_slots;      // 每批向上级申请的槽数

    Stack<uint32> free_slots;   // 空闲槽起始块号（LIFO，缓存友好）
    Stack<uint32> batch_starts; // 每次向上级申请的连续段起始块号（供整体回收）
};
```

### 主要接口

| 方法 | 说明 |
| --- | --- |
| `bool Init(BlockPool*, batch_slots=16)` | 绑定上级块池；`slot_blocks = ceil(sizeof(T)/block_size)` |
| `uint32 Acquire()` | 申请一个对象，返回起始块号；失败返回 0 |
| `bool Release(idx)` | 释放对象（块号回本地空闲列表，**不归还上级**） |
| `void ReleaseAll()` | 把所有连续段一次性归还 `BlockPool`，清空空闲列表 |
| `void Detach()` | 放弃所有权，清空内部状态但不归还上级 |
| `T *GetData(idx)` | 块号 → `T*`（CPU 侧直读直写） |
| `GetSlotBlocks() / GetBatchSlots() / GetFreeCount() / GetBatchCount()` | 查询统计 |

### 关键行为

- **批式申请**：空闲列表为空时，向上级 `pool->Acquire(batch_slots * slot_blocks)` 一次性拿
  一大段连续块；第 0 个槽直接返回，其余 `batch_slots-1` 个槽压入 `free_slots`（LIFO）。
  这样同类型对象在块池中局部连续，缓存命中更好。
- **释放只回本地**：`Release(idx)` 只把块号压回 `free_slots`，下次 `Acquire` 会复用，
  避免频繁与上级 `BlockPool` 交互。
- **整体回收**：`ReleaseAll()`（或析构）把 `batch_starts` 中记录的每一段起始块号调用
  `pool->Release(start)` 批量归还；之后本分配器仍可继续 `Acquire`（会重新向上级批量申请）。
- **`Detach()`**：当 `BlockPool` 会先于本类型销毁时，先调用它置空 `pool` 并清空状态，
  避免析构时访问悬空指针。
- 要求 `T` 为 trivially copyable（无构造/析构逻辑，按原始字节读写）。

---

## 7. 关键设计决策

| 决策 | 理由 |
| --- | --- |
| 块大小固定 **16 字节** | GPU 中不允许小于 16 字节的数据结构；16 即最小颗粒度 |
| 块号 0 为哨兵 | 无野指针；shader 与 CPU 统一用 `0` 表示空/错误 |
| `slot_blocks = ceil(sizeof(T)/16)` | 对象可能跨多块，但块号仍只有一个起始值 |
| 预算式固定容量 | 材质区远小于 1GB，一次性预留，块号稳定；不做动态增长 |
| 批式申请 + 本地复用 | 缓存连续性；单对象释放不打扰上级池 |
| 连续申请失败仍可继续 | 连续不了也只是缓存 miss 率升高，渲染仍正确 |

---

## 8. 生命周期与所有权规则

- `AbstractMemoryAllocator` 生命周期 ≥ `BlockPool`；`BlockPool` 生命周期 ≥ 所有
  `TypedBlockAllocator`。
- `BlockPool` / `TypedBlockAllocator` 均为**非拥有**上层对象，禁止拷贝。
- `TypedBlockAllocator` 析构自动 `ReleaseAll()`；若上层池已先销毁，必须先 `Detach()`。

---

## 9. 示例：TypedBlockAllocatorTest

`examples/datatype/TypedBlockAllocatorTest.cpp` 定义 4 种"材质"结构（16/32/48/64 字节，
均 16 对齐），各自绑定一个 `TypedBlockAllocator<T>` 共享同一 `BlockPool`，用确定性
`mulberry32` 随机数驱动，共 5 组测试：

| 测试 | 内容 |
| --- | --- |
| TEST 1 | BlockPool 基本：哨兵块 0、连续申请/释放、重复释放、偏移公式、读写一致 |
| TEST 2 | 多材质交错随机压力：40000 次随机申请/释放，校验跨类型块号全局唯一、16 对齐、数据完整性 |
| TEST 3 | ReleaseAll 批量回收：一整批（128 块）归还后计数正确，且可被其它类型复用 |
| TEST 4 | 池耗尽哨兵：池不足时 `Acquire()` 返回 0 |
| TEST 5 | Detach 放弃所有权：Detach 后 `GetData` 返回 `nullptr`，析构安全 |

热循环内的断言静默计数，结束时统一汇总，避免刷屏输出。

---

## 10. 最小使用示例

```cpp
#include<hgl/type/TypedBlockAllocator.h>

// 1) 准备底层内存（这里用堆实现；显存场景换成 vkMemory 映射实现）
MemoryAllocator ma;

// 2) 建顶级块池：16 字节/块，共 65536 块
BlockPool pool;
pool.Init(&ma, 65536, 16);

// 3) 定义材质结构（trivially copyable）
struct Material { uint32 color; float data[7]; };  // 32 字节 → 2 块

// 4) 绑定类型化子分配器，每批 8 槽
TypedBlockAllocator<Material> mat_alloc;
mat_alloc.Init(&pool, 8);

// 5) 申请/读写/释放
uint32 idx = mat_alloc.Acquire();      // 拿到块号
if(idx != 0)
{
    Material *m = mat_alloc.GetData(idx);   // CPU 侧直读直写
    m->color = 0xDEADBEEFu;

    // 把 idx 传给 shader：shader 用 idx * 16 寻址该对象
    mat_alloc.Release(idx);            // 归还本地空闲列表
}

// 6) 需要时整体归还上级池（析构会自动调用）
mat_alloc.ReleaseAll();
```

---

## 11. API 速查

**BlockPool**

| 方法 | 签名 |
| --- | --- |
| Init | `bool Init(AbstractMemoryAllocator*, uint32 block_count, uint32 block_size=16)` |
| Acquire | `uint32 Acquire(uint32 count)` |
| Release | `bool Release(uint32 start)` |
| GetData | `void *GetData(uint32)`, `T *GetData<T>(uint32)` |
| GetBase | `void *GetBase()` |

**TypedBlockAllocator\<T\>**

| 方法 | 签名 |
| --- | --- |
| Init | `bool Init(BlockPool*, uint32 batch_slots=16)` |
| Acquire | `uint32 Acquire()` |
| Release | `bool Release(uint32 idx)` |
| ReleaseAll | `void ReleaseAll()` |
| Detach | `void Detach()` |
| GetData | `T *GetData(uint32 idx)` |