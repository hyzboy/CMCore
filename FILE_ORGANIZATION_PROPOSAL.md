# CMCore 文件分类与命名建议方案
# CMCore File Classification and Naming Proposal

> **注意**: 本文档仅提供建议，不包含实际操作。  
> **Note**: This document only provides suggestions, no actual operations included.

---

## 📋 目录 / Table of Contents

1. [问题概述](#问题概述--problem-overview)
2. [当前状态分析](#当前状态分析--current-state-analysis)
3. [命名不一致问题](#命名不一致问题--naming-inconsistencies)
4. [建议的改进方案](#建议的改进方案--proposed-improvements)
5. [详细分类建议](#详细分类建议--detailed-classification-suggestions)
6. [实施路线图](#实施路线图--implementation-roadmap)
7. [风险评估](#风险评估--risk-assessment)

---

## 问题概述 / Problem Overview

### 主要问题 / Main Issues

1. **目录命名不统一**: 头文件目录使用小写 (inc/hgl/), 源文件目录使用 PascalCase (src/)
2. **模块名称不一致**: plugin/PlugIn, proc (无对应src), event (分散在多处)
3. **文件组织分散**: 部分相关功能分散在不同目录
4. **缺少清晰的分层**: 核心功能、工具类、平台特定代码混在一起

### 影响 / Impact

- 降低代码可读性和可维护性
- 增加新开发者学习成本
- 可能导致文件查找困难
- 不利于模块化和重用

---

## 当前状态分析 / Current State Analysis

### 目录结构对比 / Directory Structure Comparison

| 头文件目录 (inc/hgl/) | 源文件目录 (src/) | 一致性 | 备注 |
|---------------------|------------------|--------|------|
| color               | Color            | ❌ 大小写不同 | |
| event               | ❌ 无对应         | ❌ 分散 | 部分在 IO/Event |
| filesystem          | FileSystem       | ❌ 大小写不同 | |
| io                  | IO               | ❌ 大小写不同 | |
| log                 | Log              | ❌ 大小写不同 | |
| math                | Math             | ❌ 大小写不同 | |
| platform            | ❌ 无对应         | ⚠️ 纯头文件模块 | SystemInfo.cpp在根目录 |
| plugin              | PlugIn           | ❌ 名称不同 | |
| proc                | ❌ 无对应         | ⚠️ 纯头文件模块 | |
| text                | Text             | ❌ 大小写不同 | |
| thread              | ❌ 无对应         | ⚠️ 纯头文件模块 | ThreadFunc.cpp在Other/ |
| time                | Time             | ❌ 大小写不同 | |
| type                | Type             | ❌ 大小写不同 | |
| ❌ 无对应            | Object           | ⚠️ type/object/ | 应该统一 |
| ❌ 无对应            | Other            | ❌ 杂项 | 应该重新分类 |

### 文件统计 / File Statistics

```
总文件数: 278 个源文件
  - 头文件 (.h): ~210 个
  - 源文件 (.cpp): ~68 个
  
模块分布:
  - type/: 最多 (~80 个头文件)
  - io/: 约 50 个文件
  - math/: 约 25 个文件
  - platform/: 约 20 个文件
  - 其他模块: 各约 5-15 个文件
```

---

## 命名不一致问题 / Naming Inconsistencies

### 1. 目录命名风格不统一

#### 问题描述 / Problem Description

**inc/hgl/** 使用小写下划线风格:
```
inc/hgl/color/
inc/hgl/filesystem/
inc/hgl/io/
```

**src/** 使用 PascalCase 风格:
```
src/Color/
src/FileSystem/
src/IO/
```

#### 影响 / Impact
- 开发者在头文件和源文件之间切换时产生认知负担
- 工具脚本需要处理大小写映射
- 跨平台构建可能遇到大小写敏感问题

### 2. 模块名称不一致

| 模块 | inc/hgl/ | src/ | 建议 |
|------|---------|------|------|
| 插件系统 | plugin | PlugIn | 统一为 plugin |
| 进程通信 | proc | ❌ 无 | 保持 proc |
| 事件系统 | event | IO/Event | 统一为 event |
| 对象系统 | type/object | Object | 统一为 type/object |

### 3. 功能分散问题

#### Event 事件系统分散
- `inc/hgl/event/` - 异步事件
- `inc/hgl/io/event/` - 输入事件
- 建议: 统一到 `event/` 下，分为 `async/` 和 `input/` 子目录

#### Thread 相关代码分散
- `inc/hgl/thread/` - 线程接口
- `src/Other/ThreadFunc.cpp` - 线程实现
- 建议: 创建 `src/Thread/` 目录

#### Platform 代码分散
- `inc/hgl/platform/` - 平台定义
- `src/SystemInfo.cpp` - 系统信息（在根目录）
- 建议: 创建 `src/Platform/` 目录

### 4. Other 目录问题

`src/Other/` 包含:
```
ExpendString.cpp
MemoryAllocator.cpp
MemoryBlock.cpp
ThreadFunc.cpp
```

**问题**: "Other" 是一个模糊的分类，不利于查找
**建议**: 
- `ThreadFunc.cpp` → `src/Thread/`
- `MemoryAllocator.cpp`, `MemoryBlock.cpp` → `src/Type/` 或 `src/Memory/`
- `ExpendString.cpp` → `src/Text/`

---

## 建议的改进方案 / Proposed Improvements

### 方案 A: 最小改动方案（推荐）

**原则**: 
- 只统一命名风格，不重组目录结构
- 修改成本低，兼容性好
- 适合短期实施

**具体建议**:

#### 1. 统一使用小写下划线风格 (snake_case)

**理由**:
- ✅ 跨平台兼容性更好（Linux/Unix区分大小写）
- ✅ 现代C++项目常用风格（如Boost、STL）
- ✅ 头文件目录已经使用此风格
- ✅ 避免Windows/Linux大小写冲突

**变更映射表**:

```
src/Color/      → src/color/
src/FileSystem/ → src/filesystem/
src/IO/         → src/io/
src/Log/        → src/log/
src/Math/       → src/math/
src/Object/     → src/object/
src/Other/      → src/other/ (临时，后续需要拆分)
src/PlugIn/     → src/plugin/
src/Text/       → src/text/
src/Time/       → src/time/
src/Type/       → src/type/
```

#### 2. 解决模块名称不一致

```
inc/hgl/plugin/ ✓ 保持
src/PlugIn/     → src/plugin/
```

#### 3. 创建缺失的源文件目录

```
新建目录:
src/thread/    - 存放 ThreadFunc.cpp
src/platform/  - 存放 SystemInfo.cpp
src/event/     - 存放事件相关实现
src/proc/      - 如果将来有进程通信实现
```

#### 4. 重新分类 Other 目录

```
src/Other/ 应该废弃，文件重新分配:

ExpendString.cpp    → src/text/
MemoryAllocator.cpp → src/type/ 或新建 src/memory/
MemoryBlock.cpp     → src/type/ 或新建 src/memory/
ThreadFunc.cpp      → src/thread/
```

### 方案 B: 深度重组方案（可选）

**原则**: 
- 按功能层次重新组织
- 建立清晰的模块边界
- 适合长期规划

**目录结构**:

```
inc/hgl/
├── core/              # 核心基础设施
│   ├── platform/      # 平台抽象层
│   ├── type/          # 类型系统
│   └── memory/        # 内存管理
├── container/         # 容器和数据结构
│   ├── list/
│   ├── map/
│   └── pool/
├── system/            # 系统交互
│   ├── filesystem/
│   ├── io/
│   ├── thread/
│   └── process/       # 进程通信 (原proc)
├── utility/           # 工具库
│   ├── text/
│   ├── time/
│   ├── math/
│   └── color/
├── framework/         # 框架功能
│   ├── event/
│   ├── log/
│   └── plugin/
└── compat/            # 兼容性层
    └── legacy/
```

**注意**: 此方案需要大量重构，建议分阶段实施。

---

## 详细分类建议 / Detailed Classification Suggestions

### 1. Core 核心模块

#### platform/ - 平台抽象层
```
当前位置: inc/hgl/platform/
功能: 平台检测、编译器适配、操作系统抽象
文件数: ~20
建议: 
  ✓ 保持当前结构
  + 添加 src/platform/ 存放 SystemInfo.cpp
  + 考虑添加平台特定实现
```

#### type/ - 类型系统
```
当前位置: inc/hgl/type/
功能: 基础类型、容器、智能指针、对象管理
文件数: ~80
建议:
  ✓ 保持核心类型在 type/
  ? 考虑将容器单独分类到 container/
  + 统一 type/object/ 和 src/Object/
```

**详细子分类建议**:

```
type/
├── basic/           # 基础类型
│   ├── DataType.h
│   ├── CharType.h
│   └── TypeInfo.h
├── container/       # 容器类
│   ├── array/
│   │   ├── ArrayList.h
│   │   ├── DataArray.h
│   │   └── BitsArray.h
│   ├── list/
│   │   ├── DataChain.h
│   │   └── ObjectList.h
│   ├── map/
│   │   ├── Map.h
│   │   ├── SmallMap.h
│   │   └── LRUCache.h
│   ├── set/
│   │   ├── SortedSet.h
│   │   └── ConstStringSet.h
│   └── pool/
│       ├── Pool.h
│       ├── RefObjectPool.h
│       └── DataStackPool.h
├── string/          # 字符串相关
│   ├── String.h
│   ├── StringView.h
│   ├── StringList.h
│   ├── Str.*.h (所有字符串函数)
│   └── StrChar.h
├── smart_ptr/       # 智能指针
│   └── Smart.h
├── memory/          # 内存管理
│   ├── MemoryBlock.h
│   ├── MemoryAllocator.h
│   ├── AccumMemoryManager.h
│   └── Active*Manager.h
└── object/          # 对象系统
    ├── Object.h
    ├── ObjectBaseInfo.h
    └── ObjectManage.h
```

### 2. System 系统模块

#### io/ - 输入输出
```
当前位置: inc/hgl/io/
功能: 流、文件访问、序列化
文件数: ~50
建议:
  ✓ 保持核心流接口
  ? 考虑将 io/event/ 移到顶层 event/
  + 整理 Java*Stream（考虑兼容性）
```

**详细子分类建议**:

```
io/
├── stream/          # 流基类
│   ├── InputStream.h
│   ├── OutputStream.h
│   ├── SeekAccess.h
│   └── IOType.h
├── data_stream/     # 数据流
│   ├── DataInputStream.h
│   ├── DataOutputStream.h
│   ├── EndianDataInputStream.h
│   └── EndianDataOutputStream.h
├── memory_stream/   # 内存流
│   ├── MemoryInputStream.h
│   ├── MemoryOutputStream.h
│   └── ConstBufferReader.h
├── file_stream/     # 文件流
│   ├── FileInputStream.h
│   ├── FileOutputStream.h
│   ├── FileAccess.h
│   ├── RandomAccessFile.h
│   └── MMapFile.h
├── text_stream/     # 文本流
│   ├── TextInputStream.h
│   └── TextOutputStream.h
├── pipe_stream/     # 管道流
│   ├── PipeStream.h
│   └── FifoStream.h
├── specialized/     # 特殊流
│   ├── JavaInputStream.h (兼容层)
│   ├── JavaOutputStream.h (兼容层)
│   └── MiniPack.h (打包格式)
└── array_io/        # 数组IO
    ├── LoadDataArray.h
    ├── SaveDataArray.h
    ├── LoadStringList.h
    └── SaveStringList.h
```

#### filesystem/ - 文件系统
```
当前位置: inc/hgl/filesystem/
功能: 文件枚举、路径操作
文件数: ~7
建议:
  ✓ 保持当前结构
  + 考虑添加更多文件操作功能
```

#### thread/ - 线程管理
```
当前位置: inc/hgl/thread/
功能: 线程、锁、同步原语
文件数: ~15
建议:
  ✓ 保持当前结构
  + 创建 src/thread/ 目录
  + 移动 src/Other/ThreadFunc.cpp
```

**详细子分类建议**:

```
thread/
├── sync/            # 同步原语
│   ├── ThreadMutex.h
│   ├── RWLock.h
│   ├── Semaphore.h
│   ├── SemLock.h
│   └── CondVar.h
├── atomic/          # 原子操作
│   └── Atomic.h
├── thread/          # 线程管理
│   ├── Thread.h
│   └── Workflow.h
├── communication/   # 线程通信
│   ├── DataPost.h
│   ├── SwapData.h
│   └── SwapColl.h
└── container/       # 线程安全容器
    ├── RingBuffer.h
    └── Loader.h
```

#### proc/ - 进程通信
```
当前位置: inc/hgl/proc/
功能: 进程管理、IPC
文件数: ~4
建议:
  ✓ 保持当前结构
  + 如果有实现，创建 src/proc/
  ? 考虑改名为 process/ 或 ipc/
```

### 3. Utility 工具模块

#### math/ - 数学库
```
当前位置: inc/hgl/math/
功能: 向量、矩阵、数学函数
文件数: ~25
建议:
  ✓ 保持当前结构
  + 考虑细分为 vector/, matrix/, geometry/
```

**详细子分类建议**:

```
math/
├── basic/           # 基础数学
│   ├── Math.h
│   ├── MathConst.h
│   ├── PhysicsConst.h
│   ├── PrimaryMathematics.h
│   └── Binary.h
├── float/           # 浮点数
│   ├── HalfFloat.h
│   ├── FloatControl.h
│   └── FloatValidation.h
├── vector/          # 向量
│   ├── Vector.h
│   └── bvec.h (布尔向量)
├── matrix/          # 矩阵
│   ├── Matrix.h
│   └── Transform.h
├── geometry/        # 几何
│   ├── Area.h
│   ├── FastTriangle.h
│   └── Bezier.h
└── interpolation/   # 插值
    ├── LerpType.h
    ├── Lerp1D.h
    ├── Lerp2D.h
    └── Lerp3D.h
```

#### text/ - 文本处理
```
当前位置: inc/hgl/text/, inc/hgl/type/Str*.h
功能: 字符串操作、编码转换
文件数: ~15 (分散)
问题: 字符串功能分散在 type/ 和 text/
建议:
  Option 1: 统一到 text/
    - 将 type/Str*.h 移到 text/string/
    - 将 type/String.h 移到 text/
  Option 2: 保持分离
    - type/ 保留 String 类和容器
    - text/ 保留编码和格式化
```

#### color/ - 颜色处理
```
当前位置: inc/hgl/color/
功能: 颜色空间转换、混合
文件数: ~12
建议:
  ✓ 保持当前结构
  + 考虑添加更多颜色空间支持
```

#### time/ - 时间处理
```
当前位置: inc/hgl/time/
功能: 日期时间、时间戳
文件数: ~6
建议:
  ✓ 保持当前结构
```

### 4. Framework 框架模块

#### event/ - 事件系统
```
当前位置: inc/hgl/event/, inc/hgl/io/event/
功能: 事件分发、输入处理
文件数: ~10 (分散)
问题: 分散在两个目录
建议:
  方案1: 统一到 event/
    - event/async/      (原 hgl/event/)
    - event/input/      (原 io/event/)
  方案2: 保持分离但重命名
    - event/            (异步事件)
    - input/            (输入事件)
```

**详细子分类建议**:

```
event/
├── core/            # 核心事件系统
│   ├── ASyncEvent.h
│   └── EventProc.h
└── input/           # 输入事件
    ├── InputEventSource.h
    ├── EventDispatcher.h
    ├── InputMapping.h
    ├── KeyboardEvent.h
    ├── MouseEvent.h
    ├── JoystickEvent.h
    └── WindowEvent.h
```

#### log/ - 日志系统
```
当前位置: inc/hgl/log/
功能: 日志记录、日志级别
文件数: ~5
建议:
  ✓ 保持当前结构
```

#### plugin/ - 插件系统
```
当前位置: inc/hgl/plugin/
功能: 插件加载、管理
文件数: ~5
建议:
  ✓ 统一命名为 plugin (不是 PlugIn)
```

### 5. Compatibility 兼容性

#### 根目录文件
```
当前位置: inc/hgl/*.h
文件:
  - Charset.h       → text/ 或保持
  - Comparator.h    → type/utility/ 或保持
  - Endian.h        → platform/ 或保持
  - Macro.h         → platform/ 或保持
  - TypeFunc.h      → type/ 或保持
  - utf.h           → text/ 或保持

建议:
  Option 1: 保持在根目录（向后兼容）
  Option 2: 移到相应模块，提供兼容性头文件
  
  推荐: 保持在根目录，作为常用快捷访问
```

---

## 实施路线图 / Implementation Roadmap

### 阶段 0: 准备阶段（1-2天）

**目标**: 做好重构准备，最小化风险

- [ ] 创建完整的文件清单
- [ ] 统计所有 `#include` 依赖
- [ ] 备份当前代码
- [ ] 创建重构脚本
- [ ] 准备测试环境

### 阶段 1: 统一命名风格（1周）

**目标**: 解决大小写不一致问题

#### 步骤 1.1: 重命名 src 目录
```bash
# 重命名 src 目录（由大写改为小写）
src/Color/      → src/color/
src/FileSystem/ → src/filesystem/
src/IO/         → src/io/
src/Log/        → src/log/
src/Math/       → src/math/
src/Object/     → src/object/
src/PlugIn/     → src/plugin/
src/Text/       → src/text/
src/Time/       → src/time/
src/Type/       → src/type/
```

**影响文件**: 
- `src/CMakeLists.txt` - 更新所有路径引用
- 所有使用相对路径的源文件

#### 步骤 1.2: 更新 CMakeLists.txt
```cmake
# 更新所有目录引用
SET(COLOR_SOURCE_FILES Color/*.cpp)
# 改为
SET(COLOR_SOURCE_FILES color/*.cpp)

# 更新所有 SOURCE_GROUP
SOURCE_GROUP("Color\\Header Files" ...)
# 改为
SOURCE_GROUP("color\\Header Files" ...)
```

**验证**: 
- [ ] CMake 配置成功
- [ ] 编译通过
- [ ] 所有测试通过（如果有）

### 阶段 2: 创建缺失目录（3-5天）

**目标**: 为纯头文件模块创建对应的源文件目录

#### 步骤 2.1: 创建新目录
```bash
mkdir -p src/thread
mkdir -p src/platform  
mkdir -p src/event
mkdir -p src/proc      # 预留
```

#### 步骤 2.2: 移动分散的文件
```bash
# 移动线程相关
mv src/other/ThreadFunc.cpp → src/thread/

# 移动平台相关
mv src/SystemInfo.cpp → src/platform/

# 如果有事件实现，移动到 src/event/
# (当前可能没有，预留目录)
```

#### 步骤 2.3: 更新 CMakeLists.txt
```cmake
# 添加新的源文件组
SET(BASE_THREAD_SOURCE_FILES thread/ThreadFunc.cpp)
SET(PLATFORM_SOURCE_FILES platform/SystemInfo.cpp)
```

**验证**:
- [ ] 所有文件正确移动
- [ ] 编译通过
- [ ] 功能测试通过

### 阶段 3: 清理 Other 目录（3-5天）

**目标**: 将 src/Other/ 中的文件正确分类

#### 步骤 3.1: 分析 Other 中的文件
```
ExpendString.cpp     - 字符串扩展 → text/
MemoryAllocator.cpp  - 内存分配 → type/ 或 memory/
MemoryBlock.cpp      - 内存块 → type/ 或 memory/
ThreadFunc.cpp       - 已移到 thread/
```

#### 步骤 3.2: 决定内存管理文件位置
```
Option 1: 移到 src/type/
  理由: 头文件在 inc/hgl/type/
  
Option 2: 创建 src/memory/
  理由: 独立的内存管理模块
  
推荐: Option 1 (保持一致性)
```

#### 步骤 3.3: 执行移动
```bash
mv src/other/ExpendString.cpp → src/text/
mv src/other/MemoryAllocator.cpp → src/type/
mv src/other/MemoryBlock.cpp → src/type/
rmdir src/other/  # 删除空目录
```

#### 步骤 3.4: 更新 CMakeLists.txt
```cmake
# 删除 Other 相关配置
# SET(BASE_OTHER_SOURCE ...) 删除

# 添加到对应模块
SET(TEXT_SOURCE_FILES ... text/ExpendString.cpp)
SET(TYPE_MEMORY_FILES ... type/MemoryAllocator.cpp type/MemoryBlock.cpp)
```

**验证**:
- [ ] 所有文件正确移动
- [ ] 编译通过
- [ ] 链接成功

### 阶段 4: 事件系统统一（可选，1周）

**目标**: 统一分散的事件系统

#### 步骤 4.1: 评估影响
```
影响范围:
- inc/hgl/event/         → 保持
- inc/hgl/io/event/      → 考虑移到 inc/hgl/event/input/
- src/IO/Event/          → 移到 src/event/input/
```

#### 步骤 4.2: 执行重组（如果采纳）
```bash
# 方案1: 深度重组
mkdir -p inc/hgl/event/input
mkdir -p src/event/input
mv inc/hgl/io/event/* → inc/hgl/event/input/
mv src/IO/Event/* → src/event/input/

# 方案2: 保持现状，只统一命名
# (推荐: 先保持现状)
```

**验证**:
- [ ] 更新所有 `#include` 路径
- [ ] 编译通过
- [ ] API 向后兼容

### 阶段 5: 文档更新（2-3天）

**目标**: 更新文档反映新的目录结构

- [ ] 更新 README.md - 添加目录结构说明
- [ ] 创建 DIRECTORY_STRUCTURE.md - 详细说明各目录用途
- [ ] 更新 ANALYSIS.md - 反映改进成果
- [ ] 添加迁移指南 - 帮助现有用户适应

### 阶段 6: 验证和发布（1周）

**目标**: 确保所有改动正确

- [ ] 完整编译测试（所有平台）
- [ ] 性能回归测试
- [ ] API 兼容性测试
- [ ] 文档审查
- [ ] 创建迁移工具（如果需要）
- [ ] 发布版本说明

---

## 风险评估 / Risk Assessment

### 高风险项 ⚠️

#### 1. 大小写敏感问题
**风险**: Windows 不区分大小写，Linux 区分
**影响**: 可能导致跨平台编译失败
**缓解措施**:
- 使用版本控制强制大小写
- 在 Linux 环境测试
- 使用 Git 的 `core.ignorecase=false`

#### 2. 构建系统兼容性
**风险**: CMake 配置可能出错
**影响**: 编译失败
**缓解措施**:
- 先在分支测试
- 逐步更新，每次更新后测试
- 保留回退方案

#### 3. 现有用户代码破坏
**风险**: 如果移动头文件，会破坏现有代码
**影响**: 现有项目无法编译
**缓解措施**:
- **本方案不移动头文件**，只整理源文件
- 如果将来需要移动头文件，提供兼容性头文件
- 提前通知，提供迁移期

### 中风险项 ⚠️

#### 4. IDE 项目文件
**风险**: Visual Studio, CLion 等 IDE 的项目文件可能需要重新生成
**影响**: 开发体验中断
**缓解措施**:
- 提醒开发者重新生成项目文件
- 更新 .gitignore 排除 IDE 文件

#### 5. 文档滞后
**风险**: 文档更新不及时
**影响**: 新开发者困惑
**缓解措施**:
- 将文档更新列为必须步骤
- 使用自动化工具生成部分文档

### 低风险项 ✅

#### 6. 性能影响
**风险**: 目录结构变化不会影响性能
**影响**: 无
**缓解措施**: 无需特别措施

#### 7. 版本控制历史
**风险**: Git 历史可能显示为删除+新增而非移动
**影响**: 追溯文件历史稍微困难
**缓解措施**: 使用 `git mv` 命令保留历史

---

## 附录 / Appendix

### A. 命名规范参考 / Naming Convention Reference

#### C++ 项目常用命名风格

| 风格 | 示例 | 适用 | 项目案例 |
|------|------|------|----------|
| snake_case | `file_system/` | 目录、文件 | Boost, LLVM |
| PascalCase | `FileSystem/` | 类名 | Qt, Windows |
| camelCase | `fileSystem` | 变量、函数 | JavaScript |
| kebab-case | `file-system/` | URL、包名 | npm |

**CMCore 当前混用**: PascalCase (src/) + snake_case (inc/)  
**建议统一**: snake_case (更通用，更安全)

#### 目录命名最佳实践

1. **使用小写** - 避免大小写问题
2. **使用下划线或无分隔符** - 不用连字符
3. **简短有意义** - `io` 比 `input_output` 好
4. **避免缩写除非很常见** - `filesystem` 比 `fs` 好（但 `io` 可接受）
5. **保持一致性** - 选择一种风格坚持使用

### B. 重构工具推荐 / Refactoring Tools

#### 自动化重命名脚本
```bash
#!/bin/bash
# rename_directories.sh

# 定义映射关系
declare -A dir_map=(
    ["Color"]="color"
    ["FileSystem"]="filesystem"
    ["IO"]="io"
    ["Log"]="log"
    ["Math"]="math"
    ["Object"]="object"
    ["PlugIn"]="plugin"
    ["Text"]="text"
    ["Time"]="time"
    ["Type"]="type"
)

# 执行重命名
for old in "${!dir_map[@]}"; do
    new="${dir_map[$old]}"
    if [ -d "src/$old" ]; then
        git mv "src/$old" "src/$new"
        echo "Renamed: src/$old → src/$new"
    fi
done

# 更新 CMakeLists.txt
sed -i 's/Color\//color\//g' src/CMakeLists.txt
sed -i 's/FileSystem\//filesystem\//g' src/CMakeLists.txt
# ... 更多替换
```

#### CMake 路径更新脚本
```python
# update_cmake.py
import re
import sys

def update_cmake_paths(filepath):
    """更新 CMakeLists.txt 中的路径"""
    replacements = {
        'Color/': 'color/',
        'FileSystem/': 'filesystem/',
        'IO/': 'io/',
        'Log/': 'log/',
        'Math/': 'math/',
        'Object/': 'object/',
        'PlugIn/': 'plugin/',
        'Text/': 'text/',
        'Time/': 'time/',
        'Type/': 'type/',
    }
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    for old, new in replacements.items():
        content = content.replace(old, new)
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"Updated: {filepath}")

if __name__ == '__main__':
    update_cmake_paths('src/CMakeLists.txt')
```

### C. 兼容性头文件示例 / Compatibility Header Example

如果将来需要移动头文件，可以提供兼容性头文件:

```cpp
// inc/hgl/OldLocation.h (已弃用)
#pragma once

#warning "This header is deprecated. Please use <hgl/new/location.h> instead."

// 转发到新位置
#include <hgl/new/location.h>

// 如果有命名空间变化，提供别名
namespace old_namespace = new_namespace;
```

### D. 检查清单 / Checklist

#### 重构前检查
- [ ] 已备份代码
- [ ] 已记录当前目录结构
- [ ] 已统计所有 include 依赖
- [ ] 已创建新分支
- [ ] 已通知团队成员

#### 重构中检查
- [ ] 每次重命名后立即提交
- [ ] 使用 `git mv` 而非 `mv`
- [ ] 更新 CMakeLists.txt
- [ ] 更新相关文档
- [ ] 运行编译测试

#### 重构后检查
- [ ] 所有平台编译通过
- [ ] 所有测试通过
- [ ] 文档已更新
- [ ] API 兼容性验证
- [ ] 性能测试通过
- [ ] 代码审查完成

### E. 参考资料 / References

1. **C++ 核心指南** - https://isocpp.github.io/CppCoreGuidelines/
2. **Google C++ 风格指南** - https://google.github.io/styleguide/cppguide.html
3. **LLVM 编码规范** - https://llvm.org/docs/CodingStandards.html
4. **Boost 库结构** - https://www.boost.org/
5. **CMake 最佳实践** - https://cliutils.gitlab.io/modern-cmake/

---

## 总结与建议 / Summary and Recommendations

### 核心建议 / Core Recommendations

1. **采用方案 A（最小改动）** - 短期内实施，风险低
   - ✅ 统一目录命名为小写
   - ✅ 创建缺失的源文件目录
   - ✅ 清理 Other 目录
   - ⚠️ 暂不移动头文件（保持向后兼容）

2. **保持头文件目录结构不变** - 避免破坏现有用户代码
   - `inc/hgl/` 结构已经合理
   - 用户代码的 `#include` 路径不受影响

3. **渐进式改进** - 分阶段实施，每阶段充分测试
   - 阶段1: 命名统一（1周）
   - 阶段2-3: 目录整理（1-2周）
   - 阶段4: 可选的深度重组（1周）

4. **优先修复的问题**:
   - 🔴 **必须**: 统一 src/ 目录大小写（避免跨平台问题）
   - 🟡 **重要**: 清理 Other/ 目录（提高可维护性）
   - 🟢 **可选**: 事件系统统一（长期规划）

### 不建议的改动 / Not Recommended

1. ❌ **不要大规模移动头文件** - 会破坏现有用户代码
2. ❌ **不要一次性重构所有模块** - 风险太大
3. ❌ **不要改变命名空间结构** - 影响 API 兼容性
4. ❌ **不要删除现有功能** - 即使看起来过时

### 后续行动 / Next Actions

如果同意此建议方案，下一步是:

1. 📋 **审查此文档** - 确认方案细节
2. ✅ **批准实施** - 选择要执行的阶段
3. 🔀 **创建重构分支** - 在分支中进行改动
4. 🧪 **充分测试** - 确保所有改动正确
5. 📝 **更新文档** - 反映新结构
6. 🚀 **合并发布** - 发布新版本

---

**文档生成时间**: 2025-12-02  
**文档版本**: 1.0  
**建议有效期**: 6个月（需根据项目发展调整）

**注意**: 此文档仅提供建议，不包含实际操作。需要项目维护者审查批准后才能实施。
