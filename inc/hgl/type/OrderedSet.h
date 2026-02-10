#pragma once

#include<absl/container/btree_set.h>
#include<hgl/type/DataType.h>

namespace hgl
{
    // AI NOTE: Ordered set backed by absl::btree_set.
    // Keeps elements sorted by operator< with O(log n) ops.
    // Iteration yields sorted order; erase invalidates only erased iterator.
    /**
     * 有序合集</br>
     * 集合数据列表中不允许数据出现重复性，同时它会将数据排序</br>
     * 使用 absl::btree_set 实现，提供 O(log n) 插入/删除/查找性能</br>
     *
     * <b>设计目标：</b>
     * - 频繁插入和删除操作的场景
     * - 查找 O(log n)，插入/删除 O(log n)
     * - 如需连续内存序列化支持，请使用 FlatOrderedSet
     *
     * @tparam T 必须支持 operator< 用于排序
     * @see OrderedManagedSet 对象管理版本
     * @see FlatOrderedSet 序列化友好的连续内存版本
     */
    template<typename T>
    class OrderedSet
    {
    protected:
        using ThisClass = OrderedSet<T>;

        /**
         * @brief CN:有序集合（B树实现）\nEN:Ordered set (B-tree implementation)
         */
        absl::btree_set<T> data;

    public:
        using iterator = typename absl::btree_set<T>::iterator;
        using const_iterator = typename absl::btree_set<T>::const_iterator;

        /**
         * @brief CN:获取开始迭代器\nEN:Get begin iterator
         */
        iterator begin() { return data.begin(); }
        const_iterator begin() const { return data.begin(); }
        const_iterator cbegin() const { return data.cbegin(); }

        /**
         * @brief CN:获取结束迭代器\nEN:Get end iterator
         */
        iterator end() { return data.end(); }
        const_iterator end() const { return data.end(); }
        const_iterator cend() const { return data.cend(); }

        /**
         * @brief CN:获取反向开始迭代器\nEN:Get reverse begin iterator
         */
        auto rbegin() { return data.rbegin(); }
        auto rbegin() const { return data.rbegin(); }
        auto crbegin() const { return data.crbegin(); }

        /**
         * @brief CN:获取反向结束迭代器\nEN:Get reverse end iterator
         */
        auto rend() { return data.rend(); }
        auto rend() const { return data.rend(); }
        auto crend() const { return data.crend(); }

    public:
        OrderedSet() = default;
        virtual ~OrderedSet() = default;

        // ==================== 容量管理 ====================

        /**
         * @brief CN:获取元素数量\nEN:Get element count
         */
        int64 GetCount() const { return static_cast<int64>(data.size()); }

        /**
         * @brief CN:是否为空\nEN:Check if empty
         */
        bool IsEmpty() const { return data.empty(); }

        // ==================== 查找 ====================

        /**
         * @brief CN:查找元素（返回迭代器）\nEN:Find element (return iterator)
         * @param flag 要查找的值
         * @return 迭代器，未找到返回 end()
         */
        const_iterator Find(const T& flag) const
        {
            return data.find(flag);
        }

        iterator Find(const T& flag)
        {
            return data.find(flag);
        }

        /**
         * @brief CN:查找元素索引\nEN:Find element index
         * @param flag 要查找的值
         * @return 索引位置，未找到返回 -1
         */
        int64 FindIndex(const T& flag) const
        {
            auto it = data.find(flag);
            if (it == data.end())
                return -1;
            return std::distance(data.begin(), it);
        }

        /**
         * @brief CN:检查是否包含元素\nEN:Check if contains element
         */
        bool Contains(const T& v) const
        {
            return data.contains(v);
        }

        // ==================== 添加 ====================

        /**
         * @brief CN:添加一个元素\nEN:Add an element
         * @param value 要添加的值
         * @return 插入位置的索引，如果已存在返回 -1
         */
        int64 Add(const T& value)
        {
            auto result = data.insert(value);
            if (!result.second)
                return -1;  // 已存在

            return std::distance(data.begin(), result.first);
        }

        /**
         * @brief CN:添加一个元素（移动语义版本）\nEN:Add an element (move semantics version)
         * @param value 要添加的值（右值引用）
         * @return 插入位置的索引，如果已存在返回 -1
         */
        int64 Add(T&& value)
        {
            auto result = data.insert(std::move(value));
            if (!result.second)
                return -1;  // 已存在

            return std::distance(data.begin(), result.first);
        }

        /**
         * @brief CN:批量添加元素\nEN:Add multiple elements
         * @param dl 数据指针
         * @param count 数据个数
         * @return 成功添加的元素个数
         */
        int64 Add(const T* dl, int64 count)
        {
            if (!dl || count <= 0)
                return 0;

            int64 added = 0;
            for (int64 i = 0; i < count; i++)
            {
                if (data.insert(dl[i]).second)
                    ++added;
            }
            return added;
        }

        // ==================== 删除 ====================

        /**
         * @brief CN:根据索引删除元素\nEN:Delete element at index
         * @param pos 索引位置
         * @return 成功返回 true
         */
        bool DeleteAt(int64 pos)
        {
            if (pos < 0 || pos >= static_cast<int64>(data.size()))
                return false;

            auto it = data.begin();
            std::advance(it, pos);
            data.erase(it);
            return true;
        }

        /**
         * @brief CN:删除指定元素\nEN:Delete specified element
         * @param value 要删除的值
         * @return 成功返回 true
         */
        bool Delete(const T& value)
        {
            return data.erase(value) > 0;
        }

        /**
         * @brief CN:批量删除元素\nEN:Delete multiple elements
         * @param dp 数据指针
         * @param count 数据个数
         * @return 成功删除的元素个数
         */
        int64 Delete(const T* dp, int64 count)
        {
            if (!dp || count <= 0)
                return 0;

            int64 deleted = 0;
            for (int64 i = 0; i < count; i++)
            {
                if (data.erase(dp[i]) > 0)
                    ++deleted;
            }
            return deleted;
        }

        /**
         * @brief CN:清空所有元素并释放内存\nEN:Clear all elements and free memory
         */
        void Free() { data.clear(); }

        /**
         * @brief CN:清空所有元素\nEN:Clear all elements
         */
        void Clear() { data.clear(); }

        // ==================== 获取数据 ====================

        /**
         * @brief CN:根据索引获取元素\nEN:Get element by index
         * @param index 索引位置
         * @param value 输出参数，存储获取的值
         * @return 成功返回 true
         */
        bool Get(int64 index, T& value) const
        {
            if (index < 0 || index >= static_cast<int64>(data.size()))
                return false;

            auto it = data.begin();
            std::advance(it, index);
            value = *it;
            return true;
        }

        /**
         * @brief CN:获取第一个元素\nEN:Get first element
         */
        bool GetFirst(T& value) const
        {
            if (data.empty())
                return false;
            value = *data.begin();
            return true;
        }

        /**
         * @brief CN:获取最后一个元素\nEN:Get last element
         */
        bool GetLast(T& value) const
        {
            if (data.empty())
                return false;
            value = *data.rbegin();
            return true;
        }

        // ==================== 范围查询 ====================

        /**
         * @brief CN:查找第一个不小于给定值的元素\nEN:Find first element not less than given value
         */
        const_iterator lower_bound(const T& value) const
        {
            return data.lower_bound(value);
        }

        /**
         * @brief CN:查找第一个大于给定值的元素\nEN:Find first element greater than given value
         */
        const_iterator upper_bound(const T& value) const
        {
            return data.upper_bound(value);
        }

        // ==================== 运算符 ====================

        /**
         * @brief CN:赋值运算符\nEN:Assignment operator
         */
        void operator=(const OrderedSet<T>& other)
        {
            data = other.data;
        }

        /**
         * @brief CN:相等比较运算符\nEN:Equality comparison operator
         */
        bool operator==(const OrderedSet<T>& other) const
        {
            return data == other.data;
        }

        /**
         * @brief CN:不等比较运算符\nEN:Inequality comparison operator
         */
        bool operator!=(const OrderedSet<T>& other) const
        {
            return data != other.data;
        }
    };//template<typename T> class OrderedSet
}//namespace hgl
