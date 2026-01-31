#pragma once

#include <functional>
#include <vector>
#include <ankerl/unordered_dense.h>
#include <hgl/type/DataType.h>

namespace hgl
{
    /**
     * 无序集合</br>
     * 集合数据列表中不允许数据出现重复性</br>
     * 使用 ankerl::unordered_dense::set 实现，提供平均 O(1) 插入/删除/查找性能</br>
     *
     * <b>设计目标：</b>
     * - 高性能哈希集合
     * - 查找/插入/删除平均 O(1)
     * - 支持非平凡类型（需提供 Hash/Equal）
     *
     * @tparam T 元素类型
     * @tparam Hash 哈希函数（默认 std::hash<T>）
     * @tparam KeyEqual 相等比较（默认 std::equal_to<T>）
     */
    template<typename T, typename Hash = std::hash<T>, typename KeyEqual = std::equal_to<T>>
    class UnorderedSet
    {
    protected:
        using ThisClass = UnorderedSet<T, Hash, KeyEqual>;

        /**
         * @brief CN:无序集合实现\nEN:Unordered set implementation
         */
        ankerl::unordered_dense::set<T, Hash, KeyEqual> data;

    public:
        using iterator = typename ankerl::unordered_dense::set<T, Hash, KeyEqual>::iterator;
        using const_iterator = typename ankerl::unordered_dense::set<T, Hash, KeyEqual>::const_iterator;

        UnorderedSet() = default;
        virtual ~UnorderedSet() = default;

        // ==================== 迭代器支持 ====================

        iterator begin() { return data.begin(); }
        const_iterator begin() const { return data.begin(); }
        const_iterator cbegin() const { return data.cbegin(); }

        iterator end() { return data.end(); }
        const_iterator end() const { return data.end(); }
        const_iterator cend() const { return data.cend(); }

        // ==================== 容量管理 ====================

        /**
         * @brief CN:预分配容量\nEN:Reserve capacity
         */
        void Reserve(int64 capacity)
        {
            if (capacity > 0)
                data.reserve(static_cast<size_t>(capacity));
        }

        /**
         * @brief CN:获取元素数量\nEN:Get element count
         */
        int64 GetCount() const { return static_cast<int64>(data.size()); }

        /**
         * @brief CN:是否为空\nEN:Check if empty
         */
        bool IsEmpty() const { return data.empty(); }

        // ==================== 添加 ====================

        /**
         * @brief CN:添加一个元素\nEN:Add an element
         * @return CN:成功返回 true，已存在返回 false\nEN:true if added, false if already exists
         */
        bool Add(const T& value)
        {
            return data.insert(value).second;
        }

        /**
         * @brief CN:批量添加元素\nEN:Add multiple elements
         */
        int64 Add(const T* values, int64 count)
        {
            if (!values || count <= 0)
                return 0;

            int64 added = 0;
            for (int64 i = 0; i < count; ++i)
            {
                if (Add(values[i]))
                    ++added;
            }
            return added;
        }

        // ==================== 查找 ====================

        /**
         * @brief CN:查找元素（返回迭代器）\nEN:Find element (return iterator)
         */
        const_iterator Find(const T& value) const
        {
            return data.find(value);
        }

        iterator Find(const T& value)
        {
            return data.find(value);
        }

        /**
         * @brief CN:检查元素是否存在\nEN:Check if element exists
         */
        bool Contains(const T& value) const
        {
            return data.find(value) != data.end();
        }

        // ==================== 删除 ====================

        /**
         * @brief CN:删除指定元素\nEN:Delete specified element
         */
        bool Delete(const T& value)
        {
            return data.erase(value) > 0;
        }

        /**
         * @brief CN:批量删除元素\nEN:Delete multiple elements
         */
        int64 Delete(const T* values, int64 count)
        {
            if (!values || count <= 0)
                return 0;

            int64 deleted = 0;
            for (int64 i = 0; i < count; ++i)
            {
                if (Delete(values[i]))
                    ++deleted;
            }
            return deleted;
        }

        // ==================== 清理 ====================

        /**
         * @brief CN:清空所有元素\nEN:Clear all elements
         */
        void Clear() { data.clear(); }

        /**
         * @brief CN:清空并尝试释放内存\nEN:Clear and try to release memory
         */
        void Free()
        {
            data.clear();
            data.reserve(0);
        }

        // ==================== 遍历 ====================

        /**
         * @brief CN:枚举所有元素（支持 lambda）\nEN:Enumerate all elements (supports lambda)
         */
        template<typename F>
        void Enum(F&& func) const
        {
            for (const auto& value : data)
                func(value);
        }

        // ==================== 集合运算 ====================

        /**
         * @brief CN:求并集\nEN:Union
         */
        int64 Union(const ThisClass& other)
        {
            int64 added = 0;
            other.Enum([&](const T& value) {
                if (Add(value))
                    ++added;
            });
            return added;
        }

        /**
         * @brief CN:求交集\nEN:Intersection
         */
        int64 Intersect(const ThisClass& other)
        {
            std::vector<T> to_remove;
            to_remove.reserve(data.size());

            for (const auto& value : data)
            {
                if (!other.Contains(value))
                    to_remove.push_back(value);
            }

            int64 removed = 0;
            for (const auto& value : to_remove)
            {
                if (Delete(value))
                    ++removed;
            }

            return removed;
        }

        /**
         * @brief CN:求差集\nEN:Difference
         */
        int64 Difference(const ThisClass& other)
        {
            int64 removed = 0;
            other.Enum([&](const T& value) {
                if (Delete(value))
                    ++removed;
            });
            return removed;
        }

        // ==================== 比较运算 ====================

        /**
         * @brief CN:相等比较\nEN:Equality comparison
         */
        bool operator==(const ThisClass& other) const
        {
            if (GetCount() != other.GetCount())
                return false;

            bool all_match = true;
            Enum([&](const T& value) {
                if (!other.Contains(value))
                    all_match = false;
            });

            return all_match;
        }

        /**
         * @brief CN:不等比较\nEN:Inequality comparison
         */
        bool operator!=(const ThisClass& other) const
        {
            return !(*this == other);
        }
    };
} // namespace hgl