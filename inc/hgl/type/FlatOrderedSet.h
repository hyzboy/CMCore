#pragma once

#include<vector>
#include<algorithm>
#include<hgl/type/DataType.h>

namespace hgl
{
    /**
     * 有序集合</br>
     * 集合数据列表中不允许数据出现重复性，同时它会将数据排序</br>
     * 使用 std::vector 实现，支持连续内存布局</br>
     *
     * <b>特性：</b>
     * - 支持任意类型（需要支持 operator< 和 operator==）
     * - 自动去重和排序
     * - 查找 O(log n)，插入/删除 O(n)
     *
     * <b>重要提示：</b>
     * - 零拷贝序列化方法（GetData/LoadFromBuffer）仅适用于平凡类型（trivially copyable）
     * - 非平凡类型可以使用其他方法（Add/Delete/Find等）
     *
     * @tparam T 必须支持 operator< 用于排序，operator== 用于去重
     * @see OrderedManagedSet 提供对象管理的版本
     */
    template<typename T>
    class FlatOrderedSet
    {
    protected:
        using ThisClass = FlatOrderedSet<T>;

        /**
         * @brief CN:有序集合（连续数组实现）\nEN:Ordered set (contiguous array implementation)
         */
        std::vector<T> data;

    public:
        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;

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
        FlatOrderedSet() = default;
        virtual ~FlatOrderedSet() = default;

        // ==================== 序列化支持（仅限平凡类型）====================

        /**
         * @brief CN:获取原始数据指针（仅限平凡类型用于零拷贝序列化）\nEN:Get raw data pointer (for trivially copyable types only)
         * @warning 此方法仅适用于平凡可复制类型，非平凡类型需使用对象级序列化
         */
        T* GetData()
        {
            static_assert(std::is_trivially_copyable_v<T>,
                          "GetData() for binary serialization requires trivially copyable types. "
                          "For non-trivial types, iterate and serialize each element separately.");
            return data.empty() ? nullptr : data.data();
        }
        const T* GetData() const
        {
            static_assert(std::is_trivially_copyable_v<T>,
                          "GetData() for binary serialization requires trivially copyable types. "
                          "For non-trivial types, iterate and serialize each element separately.");
            return data.empty() ? nullptr : data.data();
        }

        /**
         * @brief CN:批量加载（从二进制序列化数据，仅限平凡类型）\nEN:Bulk load (from binary serialized data, trivially copyable types only)
         * @param buffer 数据缓冲区
         * @param count 元素个数
         * @param is_sorted 数据是否已排序（默认 true）
         * @warning 此方法假设buffer是二进制数据，仅适用于平凡类型。非平凡类型请逐个反序列化后Add
         */
        void LoadFromBuffer(const T* buffer, int64 count, bool is_sorted = true)
        {
            static_assert(std::is_trivially_copyable_v<T>,
                          "LoadFromBuffer() for binary deserialization requires trivially copyable types. "
                          "For non-trivial types, deserialize each element and use Add() instead.");

            if (!buffer || count <= 0)
                return;

            data.assign(buffer, buffer + count);

            if (!is_sorted)
            {
                std::sort(data.begin(), data.end());
                // 去重
                auto last = std::unique(data.begin(), data.end());
                data.erase(last, data.end());
            }
        }

        // ==================== 容量管理 ====================

        /**
         * @brief CN:获取元素数量\nEN:Get element count
         */
        int64 GetCount() const { return (int64)data.size(); }

        /**
         * @brief CN:获取已分配容量\nEN:Get allocated capacity
         */
        int64 GetAllocCount() const { return (int64)data.capacity(); }

        /**
         * @brief CN:是否为空\nEN:Check if empty
         */
        bool IsEmpty() const { return data.empty(); }

        /**
         * @brief CN:预留容量\nEN:Reserve capacity
         */
        void Reserve(int64 count) { if (count > 0) data.reserve(count); }

        // ==================== 查找 ====================

        /**
         * @brief CN:查找元素（返回迭代器）\nEN:Find element (return iterator)
         * @param flag 要查找的值
         * @return 迭代器，未找到返回 end()
         */
        const_iterator Find(const T& flag) const
        {
            auto it = std::lower_bound(data.begin(), data.end(), flag);
            return (it != data.end() && *it == flag) ? it : data.end();
        }

        iterator Find(const T& flag)
        {
            auto it = std::lower_bound(data.begin(), data.end(), flag);
            return (it != data.end() && *it == flag) ? it : data.end();
        }

        /**
         * @brief CN:查找元素索引\nEN:Find element index
         * @param flag 要查找的值
         * @return 索引位置，未找到返回 -1
         */
        int64 FindIndex(const T& flag) const
        {
            auto it = std::lower_bound(data.begin(), data.end(), flag);
            if (it == data.end() || *it != flag)
                return -1;
            return std::distance(data.begin(), it);
        }

        /**
         * @brief CN:检查是否包含元素\nEN:Check if contains element
         */
        bool Contains(const T& v) const
        {
            auto it = std::lower_bound(data.begin(), data.end(), v);
            return (it != data.end() && *it == v);
        }

        // ==================== 添加 ====================

        /**
         * @brief CN:添加一个元素\nEN:Add an element
         * @param value 要添加的值
         * @return 插入位置的索引，如果已存在返回 -1
         */
        int64 Add(const T& value)
        {
            auto it = std::lower_bound(data.begin(), data.end(), value);

            // 已存在
            if (it != data.end() && *it == value)
                return -1;

            auto result_it = data.insert(it, value);
            return std::distance(data.begin(), result_it);
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
                auto it = std::lower_bound(data.begin(), data.end(), dl[i]);
                if (it == data.end() || *it != dl[i])
                {
                    data.insert(it, dl[i]);
                    ++added;
                }
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
            if (pos < 0 || pos >= (int64)data.size())
                return false;

            data.erase(data.begin() + pos);
            return true;
        }

        /**
         * @brief CN:删除指定元素\nEN:Delete specified element
         * @param value 要删除的值
         * @return 成功返回 true
         */
        bool Delete(const T& value)
        {
            auto it = std::lower_bound(data.begin(), data.end(), value);
            if (it == data.end() || *it != value)
                return false;

            data.erase(it);
            return true;
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
                auto it = std::lower_bound(data.begin(), data.end(), dp[i]);
                if (it != data.end() && *it == dp[i])
                {
                    data.erase(it);
                    ++deleted;
                }
            }
            return deleted;
        }

        /**
         * @brief CN:清空所有元素并释放内存\nEN:Clear all elements and free memory
         */
        void Free()
        {
            data.clear();
            data.shrink_to_fit();
        }

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
            if (index < 0 || index >= (int64)data.size())
                return false;

            value = data[index];
            return true;
        }

        /**
         * @brief CN:根据索引直接访问元素\nEN:Direct element access by index
         * @param index 索引位置
         */
        const T& operator[](int64 index) const { return data[index]; }

        /**
         * @brief CN:获取第一个元素\nEN:Get first element
         */
        bool GetFirst(T& value) const
        {
            if (data.empty())
                return false;
            value = data.front();
            return true;
        }

        /**
         * @brief CN:获取最后一个元素\nEN:Get last element
         */
        bool GetLast(T& value) const
        {
            if (data.empty())
                return false;
            value = data.back();
            return true;
        }

        // ==================== 范围查询 ====================

        /**
         * @brief CN:查找第一个不小于给定值的元素\nEN:Find first element not less than given value
         */
        const_iterator lower_bound(const T& value) const
        {
            return std::lower_bound(data.begin(), data.end(), value);
        }

        /**
         * @brief CN:查找第一个大于给定值的元素\nEN:Find first element greater than given value
         */
        const_iterator upper_bound(const T& value) const
        {
            return std::upper_bound(data.begin(), data.end(), value);
        }

        // ==================== 运算符 ====Set<T>& other)
        {
            data = other.data;
        }

        /**
         * @brief CN:相等比较运算符\nEN:Equality comparison operator
         */
        bool operator==(const FlatOrderedSet<T>& other) const
        {
            return data == other.data;
        }

        /**
         * @brief CN:不等比较运算符\nEN:Inequality comparison operator
         */
        bool operator!=(const FlatOrderedSet<T>& other) const
        {
            return data != other.data;
        }
    };//template<typename T> class FlatOrderedSet
}//namespace hgl
