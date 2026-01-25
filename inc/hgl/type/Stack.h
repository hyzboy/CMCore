#pragma once

#include<hgl/type/ValueBuffer.h>
#include<cassert>
namespace hgl
{
    /**
     * @brief 标准栈容器（无虚函数）
     * CN: 用于大多数场景，高效无多态开销。支持任意平凡可复制类型，调用方管理生命周期。
     *     对于复杂对象，使用指针形式：Stack<MyObject*>，由调用方管理new/delete
     * EN: Standard stack container without virtual functions. Supports trivially copyable types only,
     *     caller manages lifetime. For complex objects, use pointer form: Stack<MyObject*> with manual new/delete
     */
    template<typename T>
    class Stack
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "Stack<T> requires T to be trivially copyable. For complex objects, use Stack<T*> instead.");

    protected:

        ValueBuffer<T> data_array;

    public:
        // ===== 属性 / Properties =====

        int GetAllocCount() const { return data_array.GetAllocCount(); }      ///< 已分配容量 / Allocated capacity
        int GetCount() const { return data_array.GetCount(); }                ///< 当前元素数量 / Current element count
        bool IsEmpty() const { return data_array.IsEmpty(); }                 ///< 是否\为空 / Is empty

        int GetTotalBytes() const { return data_array.GetTotalBytes(); }      ///< 总字节数 / Total bytes

        /**
         * @brief 获取常数据指针（Const 正确）
         * CN: 只读访问内部数据
         * EN: Read-only access to internal data
         */
        const T* GetData() const { return data_array.GetData(); }

        /**
         * @brief 获取迭代器（开始）
         * CN: 用于范围遍历，提供const和非const两个版本
         * EN: Iterator begin, provides both const and non-const versions
         */
        const T* begin() const { return data_array.begin(); }
        T* begin() { return data_array.begin(); }

        /**
         * @brief 获取迭代器（结束）
         * CN: 用于范围遍历，提供const和非const两个版本
         * EN: Iterator end, provides both const and non-const versions
         */
        const T* end() const { return data_array.end(); }
        T* end() { return data_array.end(); }

    public:
        // ===== 构造/析构 / Constructor/Destructor =====

        Stack() = default;
        virtual ~Stack() = default;

        // ===== 隐式转换 / Implicit Conversion =====

        /**
         * @brief 隐式转换为 const ValueBuffer<T>&
         * CN: 方便与需要 ValueBuffer 的代码兼容
         * EN: Implicit conversion to const ValueBuffer for compatibility
         */
        operator const ValueBuffer<T>&() const { return data_array; }

        /**
         * @brief 隐式转换为 ValueBuffer<T>&
         * CN: 用于获取可写的底层数组访问
         * EN: Get mutable ValueBuffer for direct access
         */
        operator ValueBuffer<T>&() { return data_array; }

        // ===== 拷贝语义 / Copy Semantics =====

        Stack(const Stack& rhs) : data_array(rhs.data_array) {}

        // ===== 移动语义 / Move Semantics =====

        Stack(Stack&& rhs) noexcept : data_array(std::move(rhs.data_array)) {}

        Stack& operator=(Stack&& rhs) noexcept
        {
            if (this != &rhs)
                data_array = std::move(rhs.data_array);
            return *this;
        }

    public:
        // ===== 容量管理 / Capacity Management =====

        /**
         * @brief 预分配容量
         * CN: 为栈预先分配足够的内存以减少重新分配
         * EN: Reserve capacity to reduce reallocations
         */
        bool Reserve(int count) { return data_array.Reserve(count); }

        /**
         * @brief 调整大小
         * CN: 改变栈中元素的数量
         * EN: Resize the stack
         */
        bool Resize(int count) { return data_array.Resize(count); }

    public:
        // ===== 栈操作 / Stack Operations =====

        /**
         * @brief 压入多个数据
         * CN: 一次性压入指定数量的元素
         * EN: Push multiple elements at once
         */
        bool Push(const T* data, int count)
        {
            if (!data || count <= 0)
                return false;

            int offset = data_array.GetCount();
            data_array.Expand(count);
            data_array.WriteAt(data, offset, count);
            return true;
        }

        /**
         * @brief 压入单个数据
         * CN: 压入一个元素
         * EN: Push one element
         */
        bool Push(const T& data) { return Push(&data, 1); }

        /**
         * @brief 获取栈顶元素（不移除）
         * CN: 查看栈顶但不弹出
         * EN: Peek at top element without removing
         */
        bool Peek(const T*& out_data) const
        {
            if (data_array.GetCount() <= 0)
                return false;

            out_data = &data_array[data_array.GetCount() - 1];
            return true;
        }

        /**
         * @brief 弹出单个数据
         * CN: 移除并返回栈顶元素
         * EN: Pop and return top element
         */
        bool Pop(T& out_data)
        {
            if (data_array.GetCount() <= 0)
                return false;

            if (!data_array.ReadAt(out_data, data_array.GetCount() - 1))
                return false;

            data_array.Expand(-1);
            return true;
        }

        /**
         * @brief 弹出多个数据
         * CN: 一次性弹出多个元素
         * EN: Pop multiple elements
         */
        bool Pop(T* out_data, int count)
        {
            if (data_array.GetCount() < count)
                return false;

            if (!data_array.ReadAt(out_data, data_array.GetCount() - count, count))
                return false;

            data_array.Expand(-count);
            return true;
        }

    public:
        // ===== 清理 / Cleanup =====

        /**
         * @brief 清空所有数据
         * CN: 移除所有元素但保留容量
         * EN: Clear all elements but keep capacity
         */
        void Clear()
        {
            data_array.Clear();
        }

        /**
         * @brief 清空并释放内存
         * CN: 彻底清除所有数据和容量
         * EN: Clear all data and release memory
         */
        void Free()
        {
            Clear();
            data_array.Free();
        }

    public:
        // ===== 赋值 / Assignment =====

        Stack& operator=(const Stack& rhs)
        {
            if (this != &rhs)
            {
                Clear();
                data_array.Resize(rhs.data_array.GetCount());
                data_array.WriteAt(rhs.data_array.GetData(), 0, rhs.data_array.GetCount());
            }
            return *this;
        }

    public:
        // ===== 栈特殊操作 / Stack Special Operations =====

        /**
         * @brief 安全的随机访问
         * CN: 用于遍历栈，从栈底索引 (0=bottom, Count-1=top)。越界返回 false
         * EN: Safe random access; returns false if out of bounds
         */
        bool GetAt(int index, T& out_data) const
        {
            if (index < 0 || index >= this->data_array.GetCount())
                return false;
            out_data = this->data_array[index];
            return true;
        }

        /**
         * @brief 获取栈顶元素
         * CN: 查看栈顶但不弹出，失败返回 false
         * EN: Peek top element, return false if empty
         */
        bool Top(T& out_data) const
        {
            if (this->data_array.GetCount() <= 0)
                return false;
            out_data = this->data_array[this->data_array.GetCount() - 1];
            return true;
        }

        /**
         * @brief 获取栈底元素
         * CN: 查看栈底但不弹出，失败返回 false
         * EN: Peek bottom element, return false if empty
         */
        bool Bottom(T& out_data) const
        {
            if (this->data_array.GetCount() <= 0)
                return false;
            out_data = this->data_array[0];
            return true;
        }

        /**
         * @brief 从栈顶向下遍历
         * CN: 逆向遍历，从栈顶(Count-1)到栈底(0)
         * EN: Iterate from top to bottom (Count-1 to 0)
         */
        template<typename Func>
        void ForEachFromTop(Func callback) const
        {
            int count = this->data_array.GetCount();
            for (int i = count - 1; i >= 0; --i)
                callback(i,this->data_array[i]);
        }

        /**
         * @brief 从栈底向上遍历
         * CN: 正向遍历，从栈底(0)到栈顶(Count-1)
         * EN: Iterate from bottom to top (0 to Count-1)
         */
        template<typename Func>
        void ForEachFromBottom(Func callback) const
        {
            int count = this->data_array.GetCount();
            for (int i = 0; i < count; ++i)
                callback(this->data_array[i], i);
        }

        /**
         * @brief 检查栈中是否包含指定元素
         * CN: 线性搜索栈中的元素，返回是否找到
         * EN: Check if stack contains the specified element (linear search)
         *
         * @param value 要查找的值
         * @return 如果找到返回 true，否则返回 false
         */
        bool Contains(const T& value) const
        {
            const T* data = this->data_array.GetData();
            int count = this->data_array.GetCount();

            for (int i = 0; i < count; ++i)
            {
                if (data[i] == value)
                    return true;
            }

            return false;
        }

        /**
         * @brief 查找栈中元素的索引
         * CN: 线性搜索栈中的元素，返回索引或-1
         * EN: Find index of element in stack (linear search)
         *
         * @param value 要查找的值
         * @return 找到的索引（0=栈底, Count-1=栈顶），未找到返回 -1
         */
        int Find(const T& value) const
        {
            const T* data = this->data_array.GetData();
            int count = this->data_array.GetCount();

            for (int i = 0; i < count; ++i)
            {
                if (data[i] == value)
                    return i;
            }

            return -1;
        }

        /**
         * @brief 相等比较运算符
         * CN: 比较两个栈中的所有元素是否完全相同
         * EN: Check if two stacks have identical elements
         *
         * @param other 要比较的另一个栈
         * @return 如果两个栈的元素相同，返回 true；否则返回 false
         */
        bool operator==(const Stack<T>& other) const
        {
            if (this->data_array.GetCount() != other.data_array.GetCount())
                return false;

            return hgl::mem_compare(this->data_array.GetData(), other.data_array.GetData(), this->data_array.GetCount()) == 0;
        }

        /**
         * @brief 不相等比较运算符
         * CN: 检查两个栈中的元素是否不同
         * EN: Check if two stacks have different elements
         *
         * @param other 要比较的另一个栈
         * @return 如果两个栈的元素不同，返回 true；否则返回 false
         */
        bool operator!=(const Stack<T>& other) const
        {
            return !(*this == other);
        }
    }; // class Stack

    // 向后兼容别名
    template<typename T> using ValueStack = Stack<T>;
    template<typename T> using ManagedStack = Stack<T*>;
} // namespace hgl
