#pragma once

#include<hgl/type/ArrayList.h>
#include<hgl/type/MemoryUtil.h>

namespace hgl
{
    /**
     * 对象列表：
     * 本容器保存指针类型并对这些指针拥有所有权（Owning Ptr）。
     * 当调用 Clear()/Free()/Delete 系列函数时，会对所包含的元素执行 delete 操作
     *
     * 设计要点：
     * - 继承自 ArrayList<T *>，保留其大部分接口；
     * - 新增以 Own 命名的方法（如 DeleteAtOwn），用于先释放对象再从列表中移除；
     * - 提供 Unlink 系列方法仅断开关联（不 delete），以支持不同的内存管理策略；
     */
    template<typename T> class ObjectList
    {
    private:

        static_assert(!std::is_trivially_copyable_v<T>, 
                      "ObjectList<T> requires non-trivial types (std::string, custom classes with dynamic memory, etc). "
                      "For trivially copyable types (int, float, POD structs), use ArrayList<T> instead for better performance.");

        ArrayList<T *> items;   // composition: store pointers, manage ownership here

    public:

        using ItemPointer = T *;

                ItemPointer operator[](int idx)const{return items.GetData()[idx];}

    public: //属性

                const   int     GetAllocCount   ()const{return items.GetAllocCount();}         ///<取得已分配容量
                const   int     GetCount        ()const{return items.GetCount();}              ///<取得列表内数据数量
        virtual         bool    Resize          (int count){return items.Resize(count);}       ///<设置列表内数据数量
        virtual         bool    Reserve         (int count){return items.Reserve(count);}      ///<预分配指定数量的数据空间

                const   bool    IsEmpty         ()const{return items.IsEmpty();}               ///<确认列表是否为空

                        ItemPointer *GetData    ()const{return items.GetData();}               ///<提供原始数据项
                        int     GetTotalBytes   ()const{return items.GetTotalBytes();}         ///<取得原始数据总字节数

                        ItemPointer *DataBegin  ()const{return items.begin();}
                        ItemPointer *DataEnd    ()const{return items.end();}
                        ItemPointer *DataLast   ()const{return items.last();}

    public:

                    ValueBuffer<ItemPointer> &GetArray ()     {return items.GetArray();}
            const   ValueBuffer<ItemPointer> &GetArray ()const{return items.GetArray();}

            operator        ValueBuffer<ItemPointer> & ()     {return items.GetArray();}
            operator const  ValueBuffer<ItemPointer> & ()const{return items.GetArray();}

    public: //方法

        ObjectList()=default;                                                                          ///<本类构造函数
        virtual ~ObjectList(){Free();}                                                                 ///<本类析构函数

        template<typename ...ARGS>
        T* Create(ARGS...args)
        {
            T* obj = new T(args...);

            if (!obj) return nullptr;

            if (items.Add(obj) < 0)  // Add returns index, -1 on failure
            {
                delete obj;
                return nullptr;
            }

            return obj;
        }

        // ============ Add 系列方法 ============
        virtual T *Add()
        {
            items.Add();
            return *items.last();
        }

        virtual int Add(const ItemPointer &obj)
        {
            return items.Add(obj);
        }

        virtual int Add(const ItemPointer *data, int n)
        {
            return items.Add(data, n);
        }

        virtual int Add(const ObjectList<T> &l)
        {
            return items.Add(l.GetData(), l.GetCount());
        }
 
        // ============ 查找 ============
        virtual int Find(const ItemPointer &data) const { return items.Find(data); }

        // ============ 判断是否包含 ============
        virtual bool Contains(const ItemPointer &flag) const { return items.Find((T *)flag) != -1; }

        // ============ 插入元素 ============
        virtual bool Insert(int index, const ItemPointer &obj) { return items.Insert(index, obj); }

        virtual bool Insert(int index, const ItemPointer *data, int number)
        {
            return items.Insert(index, data, number);
        }

        // ============ 交换与移动 ============
        virtual void Exchange(int a, int b) { items.Exchange(a, b); }

        virtual void Move(int new_pos, int old_pos, int move_count)
        {
            items.Move(new_pos, old_pos, move_count);
        }

        // ============ 数据读写 ============
        ItemPointer *At(int index) { return items.At(index); }
        const ItemPointer *At(int index) const { return items.At(index); }

        virtual bool Get(int index, ItemPointer &data) const
        {
            return items.Get(index, data);
        }

        virtual bool Set(int index, const ItemPointer &data)
        {
            return items.Set(index, data);
        }

        virtual bool GetFirst(ItemPointer &data) const
        {
            return items.GetFirst(data);
        }

        virtual bool GetLast(ItemPointer &data) const
        {
            return items.GetLast(data);
        }

        // ============ 赋值操作符 ============
        virtual void operator=(const ValueBuffer<ItemPointer> &da)
        {
            items = da;
        }

        virtual void operator=(const ObjectList<T> &ol)
        {
            if (this != &ol)
            {
                Clear();
                Add(ol.GetData(), ol.GetCount());
            }
        }

        virtual void operator+=(ItemPointer &obj) { Add(obj); }
        virtual void operator<<(ItemPointer &obj) { Add(obj); }

        virtual void operator-=(ItemPointer &obj) { DeleteByValueOwn(obj); }

        // ============ 释放与清理 ============
        // Free: 先 Clear（对元素 delete），再调用基类 Free
        virtual void Free() { Clear(); items.Free(); }

        // Clear: 对内部所有指针执行销毁（delete），然后清空数组
        virtual void Clear()
        {
            T **ptr = items.GetData();
            int n = items.GetCount();
            for(int i = 0; i < n; ++i)
                delete ptr[i];
            items.Clear();
        }

        // ============ Unlink 系列：仅断开关联（不 delete） ============
        virtual bool Unlink(int index)                    { return items.Delete(index); }          ///< 断开指定索引
        virtual bool UnlinkMove(int index)                { return items.DeleteShift(index); }     ///< 断开并前移其后的元素
        virtual bool Unlink(int start, int number)        { return items.Delete(start, number); }  ///< 断开一段范围
        virtual bool UnlinkByValue(ItemPointer &ip)       { return items.DeleteByValue(ip); }      ///< 通过值断开（单个）
        virtual void UnlinkByValue(ItemPointer *ip, int n){ items.DeleteByValue(ip, n); }          ///< 通过数组断开
        virtual void UnlinkAll()                          { items.Clear(); }                       ///< 断开所有（不 delete ）

    private:
        // 内部辅助：删除范围的元素并先行销毁这些指针
        bool _DeleteRange(int index, int num)
        {
            if (index < 0 || num <= 0 || index + num > items.GetCount())
                return false;

            T **ptr = items.GetData() + index;
            for(int i = 0; i < num; ++i)
                delete ptr[i];

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

            return items.Delete(index);
        }

        /**
         * 删除指定索引的元素并保持顺序（先销毁对象，再将后续元素向前移动）
         * @param index 要删除的索引位置
         * @return 删除成功返回 true，失败返回 false
         * 
         * 注意：此函数会先 delete 对应的对象，然后将后面的元素依次向前移动，
         *       保持列表的顺序。性能开销比 DeleteAtOwn 略高，但可以保持元素顺序。
         */
        bool DeleteShift(int index)
        {
            if (!_DeleteRange(index, 1))
                return false;

            return items.DeleteShift(index);
        }

        // 删除一段范围并销毁这些对象
        bool DeleteRangeOwn(int index, int number)
        {
            if (!_DeleteRange(index, number))
                return false;

            return items.Delete(index, number);
        }

        // 通过值查找并销毁，然后从列表移除（单个）
        bool DeleteByValueOwn(ItemPointer &ip)
        {
            int idx = items.Find(ip);
            if (idx == -1) return false;

            _DeleteRange(idx, 1);
            return items.Delete(idx);
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

        Iterator begin  () { return Iterator(items.GetData(), 0); }
        Iterator end    () { return Iterator(items.GetData(), items.GetCount()); }
        Iterator last   () { int c = items.GetCount(); return (c == 0) ? end() : Iterator(items.GetData(), c - 1); }

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

        ConstIterator begin ()const { return ConstIterator(items.GetData(), 0); }
        ConstIterator end   ()const { return ConstIterator(items.GetData(), items.GetCount()); }
        ConstIterator last  ()const { int c = items.GetCount(); return (c == 0) ? end() : ConstIterator(items.GetData(), c - 1); }
    };// class ObjectList
}// namespace hgl
