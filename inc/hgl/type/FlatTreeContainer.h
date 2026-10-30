#pragma once

#include <hgl/type/DataType.h>
#include <hgl/type/ValueArray.h>
#include <hgl/type/Queue.h>
#include <hgl/type/Stack.h>
#include <hgl/io/InputStream.h>
#include <hgl/io/OutputStream.h>
#include <type_traits>
#include <cstring>
#include <utility>

namespace hgl
{
    /**
     * @brief 扁平化数据与拓扑解耦树形容器 (FlatTreeContainer)
     *
     * 设计特性：
     * 1. 结构与数据彻底解耦：底部分离维护紧凑连续的数据数组与拓扑索引数组。
     * 2. 零指针引用：节点间完全由整型下标关联，内存紧凑，天然支持 memcpy。
     * 3. 双重视角 (Dual-View)：
     *    - 原始数组视角：直接提供连续物理内存指针，零拷贝供批量批处理及 GPU VRAM 上传；
     *    - 树形结构视角：通过轻量代理 NodeView / ConstNodeView 进行面向对象的父子兄弟树形导航。
     * 4. 双拓扑架构 (Dual-Topology)：
     *    - CPU 富拓扑：左孩子右兄弟 + 前驱兄弟 + 末尾孩子 (LCRS)，支持 O(1) 添加子节点、摘除与重置父节点 (Reparenting)；
     *    - GPU 极简拓扑：自动烘焙衍生纯 Parent 索引、Depth 与深度分层偏移表 (Depth Offsets)，直接匹配 Compute Shader (SSBO)。
     * 5. 线性零拷贝序列化：无需递归，无指针修正，整块流式 I/O。
     */
    template<typename T>
    class FlatTreeContainer
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "FlatTreeContainer<T> requires trivially copyable types (PODs, transforms, matrices, etc.) "
                      "to ensure zero-copy GPU VRAM upload, linear serialization, and memcpy-safe flat memory layout.");

    public:
        static constexpr int32 INVALID_INDEX = -1;

        /**
         * @brief CPU 端富拓扑节点（面向编辑、UI层级树、双向链表导航）
         */
        struct CPUTopologyNode
        {
            int32  parent       = INVALID_INDEX;    ///< 父节点下标，INVALID_INDEX 表示根节点
            int32  first_child  = INVALID_INDEX;    ///< 首个子节点下标
            int32  last_child   = INVALID_INDEX;    ///< 末尾子节点下标（用于 O(1) 追加子节点）
            int32  prev_sibling = INVALID_INDEX;    ///< 上一个兄弟节点下标（用于 O(1) 摘除节点）
            int32  next_sibling = INVALID_INDEX;    ///< 下一个兄弟节点下标
            uint16 depth        = 0;                ///< 树层级深度（根节点为 0）
            uint16 flags        = 0;                ///< 用户自定义标志位
        };

        /**
         * @brief GPU 端衍生精简拓扑节点（面向 Compute Shader、SSBO 对齐）
         */
        struct alignas(8) GPUTopologyNode
        {
            int32  parent       = INVALID_INDEX;    ///< 父节点下标（供 GPU 并行读取父节点变换）
            uint16 depth        = 0;                ///< 层级深度
            uint16 flags        = 0;                ///< 标志位
        };

#pragma pack(push, 1)
        /**
         * @brief 线性二进制序列化文件头
         */
        struct FileHeader
        {
            char   magic[8];            // "FLATTREE"
            uint32 version;             // 1
            uint32 node_count;          // 节点总数
            uint32 element_size;        // sizeof(T)
            uint32 cpu_topology_size;   // sizeof(CPUTopologyNode)
            uint32 gpu_topology_size;   // sizeof(GPUTopologyNode)
            uint32 flags;               // 标志位
            uint32 reserved;            // 预留对齐
        };
#pragma pack(pop)

        // 前向声明代理类
        class NodeView;
        class ConstNodeView;

    protected:
        ValueArray<T>               m_data;             ///< 紧凑连续的数据数组
        ValueArray<CPUTopologyNode> m_cpu_topology;     ///< CPU 端富拓扑数组
        ValueArray<GPUTopologyNode> m_gpu_topology;     ///< GPU 端衍生极简拓扑数组（用于 SSBO）
        ValueArray<uint32>          m_depth_offsets;    ///< GPU 深度分层偏移表（用于逐层 Dispatch）
        ValueArray<uint32>          m_depth_counts;     ///< 各深度层包含的节点数
        uint32                      m_max_depth = 0;    ///< 树的最大深度
        bool                        m_gpu_dirty = true; ///< GPU 拓扑脏标记

    public:
        // ====================================================================
        // 轻量代理：只读节点视角 (ConstNodeView)
        // ====================================================================
        class ConstNodeView
        {
        protected:
            const FlatTreeContainer<T> *m_container = nullptr;
            int32                       m_index     = INVALID_INDEX;

        public:
            ConstNodeView() = default;
            ConstNodeView(const FlatTreeContainer<T> *container, int32 index)
                : m_container(container), m_index(index) {}

            bool IsValid() const
            {
                return m_container && m_index >= 0 && m_index < m_container->GetCount();
            }

            explicit operator bool() const { return IsValid(); }

            int32 GetIndex() const { return m_index; }

            const T &GetData() const
            {
                return (*m_container)[m_index];
            }

            int32 GetParent() const
            {
                return m_container->GetParent(m_index);
            }

            int32 GetFirstChild() const
            {
                return m_container->GetFirstChild(m_index);
            }

            int32 GetLastChild() const
            {
                return m_container->GetLastChild(m_index);
            }

            int32 GetNextSibling() const
            {
                return m_container->GetNextSibling(m_index);
            }

            int32 GetPrevSibling() const
            {
                return m_container->GetPrevSibling(m_index);
            }

            uint16 GetDepth() const
            {
                return m_container->GetDepth(m_index);
            }

            bool IsRoot() const
            {
                return m_container->IsRoot(m_index);
            }

            bool IsLeaf() const
            {
                return m_container->IsLeaf(m_index);
            }

            bool HasChildren() const
            {
                return m_container->HasChildren(m_index);
            }

            int32 GetChildCount() const
            {
                return m_container->GetChildCount(m_index);
            }

            ConstNodeView GetParentView() const
            {
                return ConstNodeView(m_container, GetParent());
            }

            ConstNodeView GetFirstChildView() const
            {
                return ConstNodeView(m_container, GetFirstChild());
            }

            ConstNodeView GetLastChildView() const
            {
                return ConstNodeView(m_container, GetLastChild());
            }

            ConstNodeView GetNextSiblingView() const
            {
                return ConstNodeView(m_container, GetNextSibling());
            }

            ConstNodeView GetPrevSiblingView() const
            {
                return ConstNodeView(m_container, GetPrevSibling());
            }
        };

        // ====================================================================
        // 轻量代理：可写节点视角 (NodeView)
        // ====================================================================
        class NodeView : public ConstNodeView
        {
        public:
            NodeView() = default;
            NodeView(FlatTreeContainer<T> *container, int32 index)
                : ConstNodeView(container, index) {}

            FlatTreeContainer<T> *GetContainer() const
            {
                return const_cast<FlatTreeContainer<T> *>(this->m_container);
            }

            T &GetData()
            {
                return (*GetContainer())[this->m_index];
            }

            bool SetParent(int32 new_parent)
            {
                if (!this->IsValid()) return false;
                return GetContainer()->SetParent(this->m_index, new_parent);
            }

            NodeView GetParentView()
            {
                return NodeView(GetContainer(), this->GetParent());
            }

            NodeView GetFirstChildView()
            {
                return NodeView(GetContainer(), this->GetFirstChild());
            }

            NodeView GetLastChildView()
            {
                return NodeView(GetContainer(), this->GetLastChild());
            }

            NodeView GetNextSiblingView()
            {
                return NodeView(GetContainer(), this->GetNextSibling());
            }

            NodeView GetPrevSiblingView()
            {
                return NodeView(GetContainer(), this->GetPrevSibling());
            }
        };

    public:
        // ====================================================================
        // 构造与析构
        // ====================================================================
        FlatTreeContainer() = default;
        ~FlatTreeContainer() { ClearAndDeallocate(); }

        FlatTreeContainer(const FlatTreeContainer &) = default;
        FlatTreeContainer(FlatTreeContainer &&) noexcept = default;
        FlatTreeContainer &operator=(const FlatTreeContainer &) = default;
        FlatTreeContainer &operator=(FlatTreeContainer &&) noexcept = default;

        // ====================================================================
        // 原始数组与线性视角 (Raw Array View / GPU View)
        // ====================================================================
        int32   GetCount()          const { return m_data.GetCount(); }
        int32   GetAllocCount()      const { return m_data.GetAllocCount(); }
        bool    IsEmpty()           const { return m_data.IsEmpty(); }
        size_t  GetTotalBytes()     const { return m_data.GetTotalBytes(); }

        size_t  size()              const { return (size_t)m_data.GetCount(); }
        bool    empty()             const { return m_data.IsEmpty(); }

        T *         GetData()             { return m_data.GetData(); }
        const T *   GetData()       const { return m_data.GetData(); }

        T &         operator[](int32 index)       { return m_data[index]; }
        const T &   operator[](int32 index) const { return m_data[index]; }

        T *At(int32 index)
        {
            if (index < 0 || index >= m_data.GetCount()) return nullptr;
            return &m_data[index];
        }

        const T *At(int32 index) const
        {
            if (index < 0 || index >= m_data.GetCount()) return nullptr;
            return &m_data[index];
        }

        // ====================================================================
        // CPU 拓扑原始数据访问
        // ====================================================================
        CPUTopologyNode *       GetCPUTopologyData()       { return m_cpu_topology.GetData(); }
        const CPUTopologyNode * GetCPUTopologyData() const { return m_cpu_topology.GetData(); }
        size_t                  GetCPUTopologyTotalBytes() const { return m_cpu_topology.GetTotalBytes(); }
        const ValueArray<CPUTopologyNode> &GetCPUTopology() const { return m_cpu_topology; }

        // ====================================================================
        // GPU 拓扑原始数据访问 (SSBO Upload)
        // ====================================================================
        const GPUTopologyNode * GetGPUTopologyData() const { return m_gpu_topology.GetData(); }
        size_t                  GetGPUTopologyTotalBytes() const { return m_gpu_topology.GetTotalBytes(); }
        int32                   GetGPUTopologyCount() const { return m_gpu_topology.GetCount(); }
        const ValueArray<GPUTopologyNode> &GetGPUTopology() const { return m_gpu_topology; }

        const ValueArray<uint32> &GetDepthOffsets() const { return m_depth_offsets; }
        const ValueArray<uint32> &GetDepthCounts() const { return m_depth_counts; }
        uint32                  GetMaxDepth() const { return m_max_depth; }
        bool                    IsGPUTopologyDirty() const { return m_gpu_dirty; }

        // ====================================================================
        // 树形代理视角获取
        // ====================================================================
        NodeView GetNode(int32 index)
        {
            return NodeView(this, index);
        }

        ConstNodeView GetNode(int32 index) const
        {
            return ConstNodeView(this, index);
        }

        NodeView operator()(int32 index)
        {
            return GetNode(index);
        }

        ConstNodeView operator()(int32 index) const
        {
            return GetNode(index);
        }

        // ====================================================================
        // 拓扑属性查询
        // ====================================================================
        bool IsValidIndex(int32 index) const
        {
            return index >= 0 && index < m_data.GetCount();
        }

        int32 GetParent(int32 index) const
        {
            if (!IsValidIndex(index)) return INVALID_INDEX;
            return m_cpu_topology[index].parent;
        }

        int32 GetFirstChild(int32 index) const
        {
            if (!IsValidIndex(index)) return INVALID_INDEX;
            return m_cpu_topology[index].first_child;
        }

        int32 GetLastChild(int32 index) const
        {
            if (!IsValidIndex(index)) return INVALID_INDEX;
            return m_cpu_topology[index].last_child;
        }

        int32 GetNextSibling(int32 index) const
        {
            if (!IsValidIndex(index)) return INVALID_INDEX;
            return m_cpu_topology[index].next_sibling;
        }

        int32 GetPrevSibling(int32 index) const
        {
            if (!IsValidIndex(index)) return INVALID_INDEX;
            return m_cpu_topology[index].prev_sibling;
        }

        uint16 GetDepth(int32 index) const
        {
            if (!IsValidIndex(index)) return 0;
            return m_cpu_topology[index].depth;
        }

        bool IsRoot(int32 index) const
        {
            if (!IsValidIndex(index)) return false;
            return m_cpu_topology[index].parent == INVALID_INDEX;
        }

        bool IsLeaf(int32 index) const
        {
            if (!IsValidIndex(index)) return false;
            return m_cpu_topology[index].first_child == INVALID_INDEX;
        }

        bool HasChildren(int32 index) const
        {
            if (!IsValidIndex(index)) return false;
            return m_cpu_topology[index].first_child != INVALID_INDEX;
        }

        int32 GetChildCount(int32 index) const
        {
            if (!IsValidIndex(index)) return 0;
            int32 count = 0;
            int32 c = m_cpu_topology[index].first_child;
            while (c != INVALID_INDEX)
            {
                ++count;
                c = m_cpu_topology[c].next_sibling;
            }
            return count;
        }

        /**
         * @brief 判定 ancestor 是否是 descendant 的祖先节点
         */
        bool IsAncestorOf(int32 ancestor, int32 descendant) const
        {
            if (!IsValidIndex(ancestor) || !IsValidIndex(descendant))
                return false;

            int32 curr = m_cpu_topology[descendant].parent;
            while (curr != INVALID_INDEX)
            {
                if (curr == ancestor)
                    return true;
                curr = m_cpu_topology[curr].parent;
            }
            return false;
        }

        // ====================================================================
        // 节点创建与树形构造
        // ====================================================================
        /**
         * @brief 创建一个新节点
         * @param value 节点业务数据
         * @param parent 父节点下标，INVALID_INDEX 表示创建根节点
         * @return 新节点在数组中的下标
         */
        int32 CreateNode(const T &value, int32 parent = INVALID_INDEX)
        {
            const int32 new_index = m_data.GetCount();
            m_data.Add(value);

            CPUTopologyNode node;
            node.parent = parent;
            node.first_child = INVALID_INDEX;
            node.last_child = INVALID_INDEX;
            node.prev_sibling = INVALID_INDEX;
            node.next_sibling = INVALID_INDEX;

            if (parent != INVALID_INDEX && IsValidIndex(parent))
            {
                node.depth = m_cpu_topology[parent].depth + 1;
                // 挂载到父节点末尾
                int32 last = m_cpu_topology[parent].last_child;
                if (last != INVALID_INDEX)
                {
                    m_cpu_topology[last].next_sibling = new_index;
                    node.prev_sibling = last;
                    m_cpu_topology[parent].last_child = new_index;
                }
                else
                {
                    m_cpu_topology[parent].first_child = new_index;
                    m_cpu_topology[parent].last_child = new_index;
                }
            }
            else
            {
                node.parent = INVALID_INDEX;
                node.depth = 0;
            }

            m_cpu_topology.Add(node);
            m_gpu_dirty = true;
            return new_index;
        }

        int32 CreateNode(T &&value, int32 parent = INVALID_INDEX)
        {
            return CreateNode(static_cast<const T &>(value), parent);
        }

        int32 AddChild(int32 parent, const T &value)
        {
            return CreateNode(value, parent);
        }

        int32 AddChild(int32 parent, T &&value)
        {
            return CreateNode(static_cast<const T &>(value), parent);
        }

        /**
         * @brief 为指定节点添加兄弟节点（紧随其后）
         */
        int32 AddSibling(int32 sibling, const T &value)
        {
            if (!IsValidIndex(sibling))
                return INVALID_INDEX;

            const int32 parent = m_cpu_topology[sibling].parent;
            const int32 new_index = m_data.GetCount();
            m_data.Add(value);

            CPUTopologyNode node;
            node.parent = parent;
            node.depth = m_cpu_topology[sibling].depth;
            node.first_child = INVALID_INDEX;
            node.last_child = INVALID_INDEX;

            // 插入到 sibling 之后
            const int32 old_next = m_cpu_topology[sibling].next_sibling;
            node.prev_sibling = sibling;
            node.next_sibling = old_next;

            m_cpu_topology[sibling].next_sibling = new_index;
            if (old_next != INVALID_INDEX)
            {
                m_cpu_topology[old_next].prev_sibling = new_index;
            }
            else if (parent != INVALID_INDEX)
            {
                // sibling 是原先的 last_child
                m_cpu_topology[parent].last_child = new_index;
            }

            m_cpu_topology.Add(node);
            m_gpu_dirty = true;
            return new_index;
        }

        // ====================================================================
        // 拓扑修改 (Reparenting / Unlink)
        // ====================================================================
        /**
         * @brief 重置父节点 (Reparenting)
         * @param node_index 需要更改父节点的节点下标
         * @param new_parent_index 新父节点下标 (INVALID_INDEX 表示成为根节点)
         * @return 成功返回 true；若存在环依赖或下标越界返回 false
         */
        bool SetParent(int32 node_index, int32 new_parent_index)
        {
            if (!IsValidIndex(node_index))
                return false;

            if (new_parent_index != INVALID_INDEX && !IsValidIndex(new_parent_index))
                return false;

            // 相同父节点，无操作
            if (m_cpu_topology[node_index].parent == new_parent_index)
                return true;

            // 禁止挂载到自身
            if (node_index == new_parent_index)
                return false;

            // 环检测：新父节点不能是 node_index 的子孙节点
            if (new_parent_index != INVALID_INDEX && IsAncestorOf(node_index, new_parent_index))
                return false;

            // 1. 从原父节点及兄弟链中摘除
            UnlinkFromParentAndSiblings(node_index);

            // 2. 接入新父节点
            m_cpu_topology[node_index].parent = new_parent_index;
            if (new_parent_index != INVALID_INDEX)
            {
                int32 last = m_cpu_topology[new_parent_index].last_child;
                if (last != INVALID_INDEX)
                {
                    m_cpu_topology[last].next_sibling = node_index;
                    m_cpu_topology[node_index].prev_sibling = last;
                    m_cpu_topology[new_parent_index].last_child = node_index;
                }
                else
                {
                    m_cpu_topology[new_parent_index].first_child = node_index;
                    m_cpu_topology[new_parent_index].last_child = node_index;
                }
                UpdateSubtreeDepth(node_index, m_cpu_topology[new_parent_index].depth + 1);
            }
            else
            {
                // 成为根节点
                m_cpu_topology[node_index].prev_sibling = INVALID_INDEX;
                m_cpu_topology[node_index].next_sibling = INVALID_INDEX;
                UpdateSubtreeDepth(node_index, 0);
            }

            m_gpu_dirty = true;
            return true;
        }

        /**
         * @brief 物理删除指定节点及其整个子树，并做数组压缩与下标重映射（保证零内存空洞）
         */
        bool RemoveSubtree(int32 root_index)
        {
            if (!IsValidIndex(root_index))
                return false;

            const int32 total = m_data.GetCount();

            // 标记待删除节点 (使用 uint8 避免 std::vector<bool> 引用特化问题)
            ValueArray<uint8> is_deleted;
            is_deleted.Resize(total);
            for (int32 i = 0; i < total; ++i)
                is_deleted[i] = 0;

            // 递归标记所有子孙
            MarkDeletedRecursive(root_index, is_deleted);

            // 从其父节点和兄弟链中断开
            UnlinkFromParentAndSiblings(root_index);

            // 计算保留节点的重映射下标
            ValueArray<int32> new_indices;
            new_indices.Resize(total);
            int32 new_count = 0;
            for (int32 i = 0; i < total; ++i)
            {
                if (is_deleted[i])
                    new_indices[i] = INVALID_INDEX;
                else
                    new_indices[i] = new_count++;
            }

            // 构建压缩后的新数组
            ValueArray<T> new_data;
            new_data.Reserve(new_count);
            ValueArray<CPUTopologyNode> new_topology;
            new_topology.Reserve(new_count);

            for (int32 i = 0; i < total; ++i)
            {
                if (!is_deleted[i])
                {
                    new_data.Add(m_data[i]);

                    CPUTopologyNode node = m_cpu_topology[i];
                    node.parent       = (node.parent != INVALID_INDEX)       ? new_indices[node.parent]       : INVALID_INDEX;
                    node.first_child  = (node.first_child != INVALID_INDEX)  ? new_indices[node.first_child]  : INVALID_INDEX;
                    node.last_child   = (node.last_child != INVALID_INDEX)   ? new_indices[node.last_child]   : INVALID_INDEX;
                    node.prev_sibling = (node.prev_sibling != INVALID_INDEX) ? new_indices[node.prev_sibling] : INVALID_INDEX;
                    node.next_sibling = (node.next_sibling != INVALID_INDEX) ? new_indices[node.next_sibling] : INVALID_INDEX;

                    new_topology.Add(node);
                }
            }

            m_data = std::move(new_data);
            m_cpu_topology = std::move(new_topology);
            m_gpu_dirty = true;
            return true;
        }

        // ====================================================================
        // 清理与容量
        // ====================================================================
        void Clear()
        {
            m_data.Clear();
            m_cpu_topology.Clear();
            m_gpu_topology.Clear();
            m_depth_offsets.Clear();
            m_depth_counts.Clear();
            m_max_depth = 0;
            m_gpu_dirty = false;
        }

        void ClearAndDeallocate()
        {
            m_data.ClearAndDeallocate();
            m_cpu_topology.ClearAndDeallocate();
            m_gpu_topology.ClearAndDeallocate();
            m_depth_offsets.ClearAndDeallocate();
            m_depth_counts.ClearAndDeallocate();
            m_max_depth = 0;
            m_gpu_dirty = false;
        }

        bool Reserve(int32 count)
        {
            if (count <= 0) return false;
            m_data.Reserve(count);
            m_cpu_topology.Reserve(count);
            return true;
        }

        // ====================================================================
        // 树形遍历 (Traversals)
        // ====================================================================
        /**
         * @brief 遍历直接子节点
         */
        template<typename Func>
        void ForEachChild(int32 parent, Func &&func) const
        {
            if (!IsValidIndex(parent)) return;
            int32 c = m_cpu_topology[parent].first_child;
            while (c != INVALID_INDEX)
            {
                func(c, m_data[c]);
                c = m_cpu_topology[c].next_sibling;
            }
        }

        template<typename Func>
        void ForEachChild(int32 parent, Func &&func)
        {
            if (!IsValidIndex(parent)) return;
            int32 c = m_cpu_topology[parent].first_child;
            while (c != INVALID_INDEX)
            {
                func(c, m_data[c]);
                c = m_cpu_topology[c].next_sibling;
            }
        }

        /**
         * @brief 深度优先先序遍历 (PreOrder DFS)
         */
        template<typename Func>
        void TraversePreOrder(int32 root_index, Func &&func) const
        {
            if (!IsValidIndex(root_index)) return;
            func(root_index, m_data[root_index]);

            int32 c = m_cpu_topology[root_index].first_child;
            while (c != INVALID_INDEX)
            {
                TraversePreOrder(c, func);
                c = m_cpu_topology[c].next_sibling;
            }
        }

        template<typename Func>
        void TraversePreOrder(int32 root_index, Func &&func)
        {
            if (!IsValidIndex(root_index)) return;
            func(root_index, m_data[root_index]);

            int32 c = m_cpu_topology[root_index].first_child;
            while (c != INVALID_INDEX)
            {
                TraversePreOrder(c, func);
                c = m_cpu_topology[c].next_sibling;
            }
        }

        /**
         * @brief 深度优先后序遍历 (PostOrder DFS)
         */
        template<typename Func>
        void TraversePostOrder(int32 root_index, Func &&func) const
        {
            if (!IsValidIndex(root_index)) return;

            int32 c = m_cpu_topology[root_index].first_child;
            while (c != INVALID_INDEX)
            {
                TraversePostOrder(c, func);
                c = m_cpu_topology[c].next_sibling;
            }

            func(root_index, m_data[root_index]);
        }

        template<typename Func>
        void TraversePostOrder(int32 root_index, Func &&func)
        {
            if (!IsValidIndex(root_index)) return;

            int32 c = m_cpu_topology[root_index].first_child;
            while (c != INVALID_INDEX)
            {
                TraversePostOrder(c, func);
                c = m_cpu_topology[c].next_sibling;
            }

            func(root_index, m_data[root_index]);
        }

        /**
         * @brief 广度优先/层序遍历 (BFS, 无递归，使用 HGL Queue)
         */
        template<typename Func>
        void TraverseBreadthFirst(int32 root_index, Func &&func) const
        {
            if (!IsValidIndex(root_index)) return;

            Queue<int32> queue;
            queue.Push(root_index);

            int32 curr = INVALID_INDEX;
            while (queue.Pop(curr))
            {
                func(curr, m_data[curr]);
                int32 c = m_cpu_topology[curr].first_child;
                while (c != INVALID_INDEX)
                {
                    queue.Push(c);
                    c = m_cpu_topology[c].next_sibling;
                }
            }
        }

        /**
         * @brief 先序遍历所有根节点构成的森林
         */
        template<typename Func>
        void TraverseAllRoots(Func &&func) const
        {
            const int32 count = m_data.GetCount();
            for (int32 i = 0; i < count; ++i)
            {
                if (m_cpu_topology[i].parent == INVALID_INDEX)
                {
                    TraversePreOrder(i, func);
                }
            }
        }

        // ====================================================================
        // GPU 拓扑烘焙 (Bake & Sync)
        // ====================================================================
        /**
         * @brief 将 CPU 拓扑烘焙为连续的 GPU 精简拓扑数组与深度分层表
         */
        bool BakeGPUTopology()
        {
            const int32 count = m_data.GetCount();
            m_gpu_topology.Resize(count);

            m_max_depth = 0;
            for (int32 i = 0; i < count; ++i)
            {
                const uint16 d = m_cpu_topology[i].depth;
                if (d > m_max_depth)
                    m_max_depth = d;

                m_gpu_topology[i].parent = m_cpu_topology[i].parent;
                m_gpu_topology[i].depth  = d;
                m_gpu_topology[i].flags  = m_cpu_topology[i].flags;
            }

            // 构建深度统计表
            const int32 depth_levels = (count > 0) ? (m_max_depth + 1) : 0;
            m_depth_counts.Resize(depth_levels);
            m_depth_offsets.Resize(depth_levels);

            for (int32 d = 0; d < depth_levels; ++d)
            {
                m_depth_counts[d] = 0;
                m_depth_offsets[d] = 0;
            }

            for (int32 i = 0; i < count; ++i)
            {
                const uint16 d = m_cpu_topology[i].depth;
                m_depth_counts[d]++;
            }

            // 计算前缀和作为偏移
            uint32 offset = 0;
            for (int32 d = 0; d < depth_levels; ++d)
            {
                m_depth_offsets[d] = offset;
                offset += m_depth_counts[d];
            }

            m_gpu_dirty = false;
            return true;
        }

        /**
         * @brief 若 GPU 拓扑为脏，则同步执行烘焙
         */
        bool SyncGPUTopology()
        {
            if (m_gpu_dirty)
                return BakeGPUTopology();
            return true;
        }

        // ====================================================================
        // 线性零拷贝序列化与反序列化 (Zero-Copy Linear Serialization)
        // ====================================================================
        /**
         * @brief 线性保存到输出流
         */
        bool Save(io::OutputStream *stream) const
        {
            if (!stream) return false;

            FileHeader header;
            memset(&header, 0, sizeof(header));
            memcpy(header.magic, "FLATTREE", 8);
            header.version            = 1;
            header.node_count         = (uint32)m_data.GetCount();
            header.element_size       = (uint32)sizeof(T);
            header.cpu_topology_size  = (uint32)sizeof(CPUTopologyNode);
            header.gpu_topology_size  = (uint32)sizeof(GPUTopologyNode);
            header.flags              = 0;
            header.reserved           = 0;

            if (stream->Write(&header, sizeof(header)) != sizeof(header))
                return false;

            if (header.node_count > 0)
            {
                const int64 data_bytes = (int64)m_data.GetTotalBytes();
                if (stream->Write(m_data.GetData(), data_bytes) != data_bytes)
                    return false;

                const int64 topo_bytes = (int64)m_cpu_topology.GetTotalBytes();
                if (stream->Write(m_cpu_topology.GetData(), topo_bytes) != topo_bytes)
                    return false;
            }

            return true;
        }

        /**
         * @brief 线性从输入流恢复
         */
        bool Load(io::InputStream *stream)
        {
            if (!stream) return false;

            FileHeader header;
            if (stream->Read(&header, sizeof(header)) != sizeof(header))
                return false;

            if (memcmp(header.magic, "FLATTREE", 8) != 0)
                return false;

            if (header.version != 1)
                return false;

            if (header.element_size != sizeof(T))
                return false;

            if (header.cpu_topology_size != sizeof(CPUTopologyNode))
                return false;

            Clear();

            if (header.node_count > 0)
            {
                m_data.Resize((int32)header.node_count);
                const int64 data_bytes = (int64)m_data.GetTotalBytes();
                if (stream->Read(m_data.GetData(), data_bytes) != data_bytes)
                {
                    Clear();
                    return false;
                }

                m_cpu_topology.Resize((int32)header.node_count);
                const int64 topo_bytes = (int64)m_cpu_topology.GetTotalBytes();
                if (stream->Read(m_cpu_topology.GetData(), topo_bytes) != topo_bytes)
                {
                    Clear();
                    return false;
                }
            }

            m_gpu_dirty = true;
            return true;
        }

    protected:
        // ====================================================================
        // 内部辅助方法
        // ====================================================================
        /**
         * @brief 从当前父节点和兄弟双向链表中解绑节点
         */
        void UnlinkFromParentAndSiblings(int32 node_index)
        {
            const int32 parent = m_cpu_topology[node_index].parent;
            const int32 prev   = m_cpu_topology[node_index].prev_sibling;
            const int32 next   = m_cpu_topology[node_index].next_sibling;

            if (prev != INVALID_INDEX)
            {
                m_cpu_topology[prev].next_sibling = next;
            }
            else if (parent != INVALID_INDEX)
            {
                // 该节点是首子节点
                m_cpu_topology[parent].first_child = next;
            }

            if (next != INVALID_INDEX)
            {
                m_cpu_topology[next].prev_sibling = prev;
            }
            else if (parent != INVALID_INDEX)
            {
                // 该节点是末尾子节点
                m_cpu_topology[parent].last_child = prev;
            }

            m_cpu_topology[node_index].prev_sibling = INVALID_INDEX;
            m_cpu_topology[node_index].next_sibling = INVALID_INDEX;
        }

        /**
         * @brief 递归更新子树深度
         */
        void UpdateSubtreeDepth(int32 node_index, uint16 new_depth)
        {
            m_cpu_topology[node_index].depth = new_depth;
            int32 c = m_cpu_topology[node_index].first_child;
            while (c != INVALID_INDEX)
            {
                UpdateSubtreeDepth(c, new_depth + 1);
                c = m_cpu_topology[c].next_sibling;
            }
        }

        /**
         * @brief 递归标记子树待删除
         */
        void MarkDeletedRecursive(int32 curr, ValueArray<uint8> &is_deleted)
        {
            is_deleted[curr] = 1;
            int32 c = m_cpu_topology[curr].first_child;
            while (c != INVALID_INDEX)
            {
                MarkDeletedRecursive(c, is_deleted);
                c = m_cpu_topology[c].next_sibling;
            }
        }
    };
} // namespace hgl
