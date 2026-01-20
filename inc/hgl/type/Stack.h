#pragma once

#include<hgl/type/DataArray.h>
namespace hgl
{
    /**
     * @brief 基础栈容器（无虚函数）
     * CN: 提供核心栈操作，无多态开销
     * EN: Basic stack operations without virtual function overhead
     */
    template<typename T>
    class StackBase
    {
    protected:
        DataArray<T> data_array;

    public:
        // ===== 属性 / Properties =====

        int GetAllocCount() const { return data_array.GetAllocCount(); }      ///< 已分配容量 / Allocated capacity
        int GetCount() const { return data_array.GetCount(); }                ///< 当前元素数量 / Current element count
        bool IsEmpty() const { return data_array.IsEmpty(); }                 ///< 是否为空 / Is empty

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

        StackBase() = default;
        virtual ~StackBase() = default;

        // ===== 隐式转换 / Implicit Conversion =====

        /**
         * @brief 隐式转换为 const DataArray<T>&
         * CN: 方便与需要 DataArray 的代码兼容
         * EN: Implicit conversion to const DataArray for compatibility
         */
        operator const DataArray<T>&() const { return data_array; }

        /**
         * @brief 隐式转换为 DataArray<T>&
         * CN: 用于获取可写的底层数组访问
         * EN: Get mutable DataArray for direct access
         */
        operator DataArray<T>&() { return data_array; }

        // ===== 拷贝语义 / Copy Semantics =====

        StackBase(const StackBase& rhs) : data_array(rhs.data_array) {}

        // ===== 移动语义 / Move Semantics =====

        StackBase(StackBase&& rhs) noexcept : data_array(std::move(rhs.data_array)) {}

        StackBase& operator=(StackBase&& rhs) noexcept
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
         * CN: 移除所有元素但保留容量。由 DataArray 处理对象生命周期
         * EN: Clear all elements but keep capacity. DataArray handles lifecycle
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

        StackBase& operator=(const StackBase& rhs)
        {
            if (this != &rhs)
            {
                Clear();
                data_array.Resize(rhs.data_array.GetCount());
                data_array.WriteAt(rhs.data_array.GetData(), 0, rhs.data_array.GetCount());
            }
            return *this;
        }

    }; // class StackBase

    /**
     * @brief 标准栈容器（无虚函数）
     * CN: 用于大多数场景，高效无多态开销
     * EN: Standard stack container without virtual functions
     */
    template<typename T>
    class Stack : public StackBase<T>
    {
    public:
        // C++20: Ensure T is trivially copyable, otherwise use ObjectStack
        static_assert(std::is_trivially_copyable_v<T>,
            "Stack only supports trivially copyable types (primitives, POD structures, etc.). "
            "For non-trivial types with custom constructors/destructors, use ObjectStack<T> instead.");

        Stack() = default;
        virtual ~Stack() = default;

        /**
         * @brief 获取栈顶元素（只读）
         * CN: 返回栈顶元素的常引用
         * EN: Get const reference to top element
         */
        const T& Top() const
        {
            static const T empty_value = T();
            if (this->data_array.GetCount() <= 0)
                return empty_value;
            return this->data_array[this->data_array.GetCount() - 1];
        }

        /**
         * @brief 获取栈顶元素（可写）
         * CN: 返回栈顶元素的可写引用
         * EN: Get mutable reference to top element
         */
        T& Top()
        {
            static T empty_value = T();
            if (this->data_array.GetCount() <= 0)
                return empty_value;
            return this->data_array[this->data_array.GetCount() - 1];
        }

        /**
         * @brief 安全的随机访问（只读）
         * CN: 用于遍历栈，从栈底索引 (0=bottom, Count-1=top)
         * EN: Safe random access from bottom (0=bottom, Count-1=top)
         */
        const T& GetAt(int index) const
        {
            static const T empty_value = T();
            if (index < 0 || index >= this->data_array.GetCount())
                return empty_value;
            return this->data_array[index];
        }

        /**
         * @brief 安全的随机访问（可写）
         * CN: 用于修改栈中的元素
         * EN: Safe random access (writable)
         */
        T& GetAt(int index)
        {
            static T empty_value = T();
            if (index < 0 || index >= this->data_array.GetCount())
                return empty_value;
            return this->data_array[index];
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
                callback(this->data_array[i], i);
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

    }; // class Stack

    /**
     * @brief 对象栈（管理指针所有权）
     * CN: 专用于管理动态分配的对象指针，自动删除
     * EN: Stack for managing pointer ownership with automatic cleanup
     */
    template<typename T>
    class ObjectStack : public StackBase<T*>
    {
    public:
        ObjectStack() = default;
        virtual ~ObjectStack() { Free(); }

        /**
         * @brief 压入对象指针
         * CN: 压入对象指针并取得所有权
         * EN: Push object pointer and take ownership
         */
        bool Push(T* obj)
        {
            if (!obj)
                return false;
            return StackBase<T*>::Push(obj);
        }

        /**
         * @brief 弹出对象指针
         * CN: 弹出对象指针并移交所有权
         * EN: Pop object pointer and transfer ownership
         */
        T* Pop()
        {
            T* obj = nullptr;
            if (!StackBase<T*>::Pop(obj))
                return nullptr;
            return obj;
        }

        /**
         * @brief 调整栈大小
         * CN: 改变栈中元素数量，新增位置初始化为nullptr
         * EN: Resize stack with new positions initialized to nullptr
         */
        bool Resize(int count)
        {
            int old_count = this->GetCount();
            if (!StackBase<T*>::Resize(count))
                return false;

            // 新增的位置必须初始化为 nullptr，否则 Clear() 时会删除垃圾指针
            if (count > old_count)
            {
                T** ptr = const_cast<T**>(this->GetData());
                for (int i = old_count; i < count; i++)
                    ptr[i] = nullptr;
            }
            return true;
        }

        /**
         * @brief 清空所有对象
         * CN: 删除所有指向的对象，然后清空数组
         * EN: Delete all pointed objects and clear array
         */
        void Clear()
        {
            T** ptr = const_cast<T**>(this->GetData());
            int n = this->GetCount();
            
            // 手动删除所有指向的对象
            for (int i = 0; i < n; ++i)
            {
                if (ptr[i])
                {
                    delete ptr[i];
                    ptr[i] = nullptr;
                }
            }
            
            // 清空数组
            this->data_array.Clear();
        }

        /**
         * @brief 清空并释放内存
         * CN: 删除所有对象并释放栈容量
         * EN: Delete all objects and release stack capacity
         */
        void Free()
        {
            Clear();  // Clear() 已经完整处理了对象删除和数组清空
            this->data_array.Free();
        }

    }; // class ObjectStack

} // namespace hgl
