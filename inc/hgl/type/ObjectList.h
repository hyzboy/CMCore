#pragma once

#include<hgl/type/ArrayList.h>
#include<hgl/type/LifecycleTraits.h>

namespace hgl
{
    /**
     * 对象列表：
     * 本容器保存指针类型并对这些指针拥有所有权（Owning Ptr）。
     * 当调用 Clear()/Free()/Delete 系列函数时，会对所包含的元素执行 delete 操作
     * （通过 LifecycleTraitsOwningPtr<T>::destroy 统一处理）。
     *
     * 设计要点：
     * - 继承自 ArrayList<T *>，保留其大部分接口；
     * - 新增以 Own 命名的方法（如 DeleteAtOwn），用于先释放对象再从列表中移除；
     * - 提供 Unlink 系列方法仅断开关联（不 delete），以支持不同的内存管理策略；
     */
    template<typename T> class ObjectList : public ArrayList<T *>
    {
    public:
        using ItemPointer = T *;

        ObjectList() = default;
        virtual ~ObjectList() { Free(); }

        // 访问运算符，返回指针
        ItemPointer operator[](int idx) const { return this->data_array.GetData()[idx]; }

        template<typename ...ARGS>
        T* Create(ARGS...args)
        {
            T* obj = new T(args...);

            if (!obj) return nullptr;

            if (ArrayList<T *>::Add(obj) < 0)  // Add returns index, -1 on failure
            {
                delete obj;
                return nullptr;
            }

            return obj;
        }

        // 插入元素（保留原 ArrayList 行为）
        bool Insert(int index, const ItemPointer &obj) { return ArrayList<T *>::Insert(index, obj); }

        // 释放与清理
        // Free: 先 Clear（对元素 delete），再调用基类 Free
        virtual void Free() { Clear(); ArrayList<T *>::Free(); }

        // Clear: 对内部所有指针执行销毁（delete），然后清空数组
        virtual void Clear()
        {
            T **ptr = this->data_array.GetData();
            int n = this->data_array.GetCount();
            LifecycleTraitsOwningPtr<T>::destroy(ptr, n);
            ArrayList<T *>::Clear();
        }

        // 判断是否包含指定指针（按值比较）
        virtual bool Contains(const ItemPointer &flag) const { return ArrayList<T *>::Find((T *)flag) != -1; }

        // Unlink 系列：仅断开关联（不 delete），等同于基类的 Delete/ Clear 行为
        virtual bool Unlink(int index)                    { return ArrayList<T *>::Delete(index); }          ///< 断开指定索引
        virtual bool UnlinkMove(int index)                { return ArrayList<T *>::DeleteShift(index); }     ///< 断开并前移其后的元素
        virtual bool Unlink(int start, int number)        { return ArrayList<T *>::Delete(start, number); }  ///< 断开一段范围
        virtual bool UnlinkByValue(ItemPointer &ip)       { return ArrayList<T *>::DeleteByValue(ip); }      ///< 通过值断开（单个）
        virtual void UnlinkByValue(ItemPointer *ip, int n){ ArrayList<T *>::DeleteByValue(ip, n); }          ///< 通过数组断开
        virtual void UnlinkAll()                          { ArrayList<T *>::Clear(); }                       ///< 断开所有（不 delete ）

    private:
        // 内部辅助：删除范围的元素并先行销毁这些指针
        bool _DeleteRange(int index, int num)
        {
            if (index < 0 || num <= 0 || index + num > ArrayList<T *>::GetCount())
                return false;

            T **ptr = this->data_array.GetData() + index;
            LifecycleTraitsOwningPtr<T>::destroy(ptr, num);

            return true;
        }

    public:
        // 以下以 Own 结尾的方法会先 delete 对象（如果存在），再从列表中移除对应条目。
        // 不使用 override 是为避免与 ArrayList 中 Delete/DeleteByValue 的签名冲突。

        // 删除指定索引并销毁该对象
        bool DeleteAtOwn(int index)
        {
            if (!_DeleteRange(index, 1))
                return false;

            return ArrayList<T *>::Delete(index);
        }

        // 删除一段范围并销毁这些对象
        bool DeleteRangeOwn(int index, int number)
        {
            if (!_DeleteRange(index, number))
                return false;

            return ArrayList<T *>::Delete(index, number);
        }

        // 通过值查找并销毁，然后从列表移除（单个）
        bool DeleteByValueOwn(ItemPointer &ip)
        {
            int idx = ArrayList<T *>::Find(ip);
            if (idx == -1) return false;

            _DeleteRange(idx, 1);
            return ArrayList<T *>::Delete(idx);
        }

        // 通过数组的值逐个销毁并移除
        void DeleteByValueOwn(ItemPointer *ip, int n)
        {
            if (!ip || n <= 0) return;

            for (int i = 0; i < n; ++i)
                DeleteByValueOwn(ip[i]);
        }

    public: // 迭代器

        class Iterator
        {
        private:

            T **list;
            T *single; // when constructed from a single element (copy-initialized in range-for)
            int32 current_index;

        public:

            using pointer = T*;    // pointer to element type
            using reference = T*;  // element type is pointer (T*)

        public:

            Iterator()
            {
                list = nullptr;
                single = nullptr;
                current_index = -1;
            }

            Iterator(T **lst, int32 idx) :list(lst), single(nullptr), current_index(idx) {}

            // Construct from a single element (pointer). This allows code like:
            // for(ObjectList<T>::Iterator it : objlist) { ... }
            Iterator(T *elem) : list(nullptr), single(elem), current_index(-1) {}

            // Return the element pointer
            pointer operator*() const
            {
                return (list ? list[current_index] : single);
            }

            // Pointer-like access to object's members
            pointer operator->() const
            {
                return (list ? list[current_index] : single);
            }

            Iterator& operator++() { if(list) ++current_index; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            Iterator& operator+=(int32 v) { if(list) current_index += v; return *this; }
            Iterator& operator-=(int32 v) { if(list) current_index -= v; return *this; }

            Iterator& operator--() { if(list) --current_index; return *this; }
            Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

            bool operator==(const Iterator& other) const
            {
                if(list || other.list)
                    return list == other.list && current_index == other.current_index;
                return single == other.single;
            }

            bool operator!=(const Iterator& other) const
            {
                if(list || other.list)
                    return list != other.list || current_index != other.current_index;
                return single != other.single;
            }
        };//class Iterator

        Iterator begin  () { return Iterator(this->data_array.GetData(), 0); }
        Iterator end    () { return Iterator(this->data_array.GetData(), this->data_array.GetCount()); }
        Iterator last   () { int c = this->data_array.GetCount(); return (c == 0) ? end() : Iterator(this->data_array.GetData(), c - 1); }

    public: // 迭代器

        class ConstIterator
        {
        private:

            // pointer to array of pointers; we don't modify pointers via const iterator
            T * const * list;
            T * single;
            int32 current_index;

        public:

            using pointer = T*;      // pointer to element (non-const pointee)
            using reference = T*;    // element type is pointer

        public:

            ConstIterator()
            {
                list = nullptr;
                single = nullptr;
                current_index = -1;
            }

            // Accept a pointer to array of pointers. We keep the internal pointer as
            // T * const * so that we don't allow modification of the pointer array via const iterator.
            ConstIterator(T * const * lst, int32 idx) :list(lst), single(nullptr), current_index(idx) {}

            // Construct from a single element pointer
            ConstIterator(T *elem) : list(nullptr), single(elem), current_index(-1) {}

            reference operator*() const
            {
                return (list ? list[current_index] : single);
            }

            pointer operator->() const
            {
                return (list ? list[current_index] : single);
            }

            ConstIterator& operator++() { if(list) ++current_index; return *this; }
            ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }

            ConstIterator& operator+=(int32 v) { if(list) current_index += v; return *this; }
            ConstIterator& operator-=(int32 v) { if(list) current_index -= v; return *this; }

            ConstIterator& operator--() { if(list) --current_index; return *this; }
            ConstIterator operator--(int) { ConstIterator tmp = *this; --(*this); return tmp; }

            bool operator==(const ConstIterator& other) const
            {
                if(list || other.list)
                    return list == other.list && current_index == other.current_index;
                return single == other.single;
            }

            bool operator!=(const ConstIterator& other) const
            {
                if(list || other.list)
                    return list != other.list || current_index != other.current_index;
                return single != other.single;
            }
        };//class ConstIterator

        ConstIterator begin ()const { return ConstIterator(this->data_array.GetData(), 0); }
        ConstIterator end   ()const { return ConstIterator(this->data_array.GetData(), this->data_array.GetCount()); }
        ConstIterator last  ()const { int c = this->data_array.GetCount(); return (c == 0) ? end() : ConstIterator(this->data_array.GetData(), c - 1); }
    };// class ObjectList
}// namespace hgl
