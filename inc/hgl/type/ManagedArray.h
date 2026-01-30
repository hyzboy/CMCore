#pragma once

#include<vector>
#include<algorithm>
#include<hgl/type/MemoryUtil.h>

namespace hgl
{
    /**
     * 对象列表：
     * 本容器保存指针类型并对这些指针拥有所有权（Owning Ptr）。
     * 当调用 Clear()/Free()/Delete 系列函数时，会对所包含的元素执行 delete 操作
     *
     * 设计要点：
     * - 内部使用 std::vector<T *> 存储指针；
     * - 新增以 Own 命名的方法（如 DeleteAt），用于先释放对象再从列表中移除；
     * - 提供 Unlink 系列方法仅断开关联（不 delete），以支持不同的内存管理策略；
     */
    template<typename T> class ManagedArray
    {
    private:

        static_assert(!std::is_trivially_copyable_v<T>,
                      "ManagedArray<T> requires non-trivial types (std::string, custom classes with dynamic memory, etc). "
                      "For trivially copyable types (int, float, POD structs), use ValueArray<T> instead for better performance.");

        std::vector<T *> items;   // composition: store pointers, manage ownership here

    public:

        using ItemPointer = T *;

                ItemPointer operator[](int idx)const{return items.data()[idx];}

    public: //属性

                const   int     GetAllocCount   ()const{return (int)items.capacity();}         ///<取得已分配容量
                const   int     GetCount        ()const{return (int)items.size();}              ///<取得列表内数据数量
        virtual         bool    Resize          (int count){items.resize(count); return true;}       ///<设置列表内数据数量
        virtual         bool    Reserve         (int count){items.reserve(count); return true;}      ///<预分配指定数量的数据空间

                const   bool    IsEmpty         ()const{return items.empty();}               ///<确认列表是否为空

                        ItemPointer *GetData    (){return items.empty()?nullptr:items.data();}               ///<提供原始数据项
                const   ItemPointer *GetData    ()const{return items.empty()?nullptr:items.data();}               ///<提供原始数据项（const 版本）
                        int     GetTotalBytes   ()const{return (int)(items.size()*sizeof(ItemPointer));}         ///<取得原始数据总字节数

                        ItemPointer *DataBegin  (){return items.empty()?nullptr:items.data();}
                const   ItemPointer *DataBegin  ()const{return items.empty()?nullptr:items.data();}
                        ItemPointer *DataEnd    (){return items.empty()?nullptr:items.data()+items.size();}
                const   ItemPointer *DataEnd    ()const{return items.empty()?nullptr:items.data()+items.size();}
                        ItemPointer *DataLast   (){return items.empty()?nullptr:items.data()+items.size()-1;}
                const   ItemPointer *DataLast   ()const{return items.empty()?nullptr:items.data()+items.size()-1;}

    public:

                    std::vector<ItemPointer> &GetArray ()     {return items;}
            const   std::vector<ItemPointer> &GetArray ()const{return items;}

            operator        std::vector<ItemPointer> & ()     {return items;}
            operator const  std::vector<ItemPointer> & ()const{return items;}

    public: //方法

        ManagedArray()=default;                                                                          ///<本类构造函数
        virtual ~ManagedArray(){Free();}                                                                 ///<本类析构函数

        template<typename ...ARGS>
        T* Create(ARGS...args)
        {
            T* obj = new T(args...);

            if (!obj) return nullptr;

            items.push_back(obj);
            return obj;
        }

        // ============ Add 系列方法 ============
        virtual T *Add()
        {
            items.push_back(nullptr);
            return items.back();
        }

        virtual int Add(const ItemPointer &obj)
        {
            const int index=(int)items.size();
            items.push_back(obj);
            return index;
        }

        virtual int Add(const ItemPointer *data, int n)
        {
            if(!data||n<=0)
                return(-1);

            const int ec=(int)items.size();
            items.insert(items.end(), data, data+n);
            return(ec);
        }

        virtual int Add(const ManagedArray<T> &l)
        {
            return Add(l.GetData(), l.GetCount());
        }

        // ============ 查找 ============
        virtual int Find(const ItemPointer &data) const 
        { 
            auto it = std::find(items.begin(), items.end(), data);
            return (it == items.end()) ? -1 : (int)std::distance(items.begin(), it);
        }

        // ============ 判断是否包含 ============
        virtual bool Contains(const ItemPointer &flag) const { return Find(flag) >= 0; }

        // ============ 插入元素 ============
        virtual bool Insert(int index, const ItemPointer &obj) 
        { 
            if(index<0 || index>(int)items.size()) return false;
            items.insert(items.begin()+index, obj);
            return true;
        }

        virtual bool Insert(int index, const ItemPointer *data, int number)
        {
            if(!data || number<=0 || index<0 || index>(int)items.size()) return false;
            items.insert(items.begin()+index, data, data+number);
            return true;
        }

        // ============ 交换与移动 ============
        virtual void Exchange(int a, int b) 
        { 
            if(a>=0 && a<(int)items.size() && b>=0 && b<(int)items.size())
                std::swap(items[a], items[b]);
        }

        virtual void Move(int new_pos, int old_pos, int move_count)
        {
            if(old_pos<0 || old_pos+move_count>(int)items.size()) return;
            if(new_pos<0 || new_pos>(int)items.size()) return;
            if(move_count<=0) return;

            if(new_pos>=old_pos && new_pos<=old_pos+move_count) return;

            std::vector<ItemPointer> temp(items.begin()+old_pos, items.begin()+old_pos+move_count);
            items.erase(items.begin()+old_pos, items.begin()+old_pos+move_count);

            int actual_new_pos = new_pos;
            if(new_pos > old_pos)
                actual_new_pos -= move_count;

            items.insert(items.begin()+actual_new_pos, temp.begin(), temp.end());
        }

        // ============ 数据读写 ============
        ItemPointer At(int index) 
        { 
            if(index<0 || index>=(int)items.size()) return nullptr;
            return items[index];
        }
        const ItemPointer At(int index) const 
        { 
            if(index<0 || index>=(int)items.size()) return nullptr;
            return items[index];
        }

        virtual bool Get(int index, ItemPointer &data) const
        {
            if(index<0 || index>=(int)items.size()) return false;
            data = items[index];
            return true;
        }

        virtual bool Set(int index, const ItemPointer &data)
        {
            if(index<0 || index>=(int)items.size()) return false;
            items[index] = data;
            return true;
        }

        virtual bool GetFirst(ItemPointer &data) const
        {
            if(items.empty()) return false;
            data = items.front();
            return true;
        }

        virtual bool GetLast(ItemPointer &data) const
        {
            if(items.empty()) return false;
            data = items.back();
            return true;
        }

        // ============ 赋值操作符 ============
        virtual void operator=(const std::vector<ItemPointer> &da)
        {
            items = da;
        }

        virtual void operator=(const ManagedArray<T> &ol)
        {
            if (this != &ol)
            {
                items = ol.items;
            }
        }

        virtual void operator+=(ItemPointer &obj) { Add(obj); }
        virtual void operator<<(ItemPointer &obj) { Add(obj); }

        virtual void operator-=(ItemPointer &obj) { DeleteByValue(obj); }

        // ============ 释放与清理 ============
        // Free: 先 Clear（对元素 delete），再调用 shrink_to_fit
        virtual void Free() 
        { 
            Clear(); 
            items.shrink_to_fit();
        }

        // Clear: 对内部所有指针执行销毁（delete），然后清空数组
        virtual void Clear()
        {
            T **ptr = items.data();
            int n = (int)items.size();
            for(int i = 0; i < n; ++i)
                if(ptr[i]) delete ptr[i];
            items.clear();
        }

        // ============ Unlink 系列：仅断开关联（不 delete） ============
        virtual bool Unlink(int index)                    
        { 
            if(index<0 || index>=(int)items.size()) return false;
            items.erase(items.begin()+index);
            return true;
        }          ///< 断开指定索引
        virtual bool UnlinkMove(int index)                
        { 
            if(index<0 || index>=(int)items.size()) return false;
            items.erase(items.begin()+index);
            return true;
        }     ///< 断开并前移其后的元素
        virtual bool Unlink(int start, int number)        
        { 
            if(start<0 || number<=0 || start+number>(int)items.size()) return false;
            items.erase(items.begin()+start, items.begin()+start+number);
            return true;
        }  ///< 断开一段范围
        virtual bool UnlinkByValue(ItemPointer &ip)       
        { 
            auto it = std::find(items.begin(), items.end(), ip);
            if(it == items.end()) return false;
            items.erase(it);
            return true;
        }      ///< 通过值断开（单个）
        virtual void UnlinkByValue(ItemPointer *ip, int n)
        { 
            if(!ip || n<=0) return;
            for(int i = 0; i < n; ++i)
            {
                auto it = std::find(items.begin(), items.end(), ip[i]);
                if(it != items.end()) items.erase(it);
            }
        }          ///< 通过数组断开
        virtual void UnlinkAll()                          { items.clear(); }                       ///< 断开所有（不 delete ）

    private:
        // 内部辅助：删除范围的元素并先行销毁这些指针
        bool _DeleteRange(int index, int num)
        {
            if (index < 0 || num <= 0 || index + num > (int)items.size())
                return false;

            T **ptr = items.data() + index;
            for(int i = 0; i < num; ++i)
                if(ptr[i]) delete ptr[i];

            return true;
        }

    public:
        // 以下以 Own 结尾的方法会先 delete 对象（如果存在），再从列表中移除对应条目。
        // 不使用 override 是为避免与 ValueArray 中 Delete/DeleteByValue 的签名冲突。

        // 删除指定索引并销毁该对象
        bool DeleteAt(int index)
        {
            if (!_DeleteRange(index, 1))
                return false;

            items.erase(items.begin() + index);
            return true;
        }

        /**
         * 删除指定索引的元素并保持顺序（先销毁对象，再将后续元素向前移动）
         * @param index 要删除的索引位置
         * @return 删除成功返回 true，失败返回 false
         *
         * 注意：此函数会先 delete 对应的对象，然后将后面的元素依次向前移动，
         *       保持列表的顺序。性能开销比 DeleteAt 略高，但可以保持元素顺序。
         */
        bool DeleteShift(int index)
        {
            if (!_DeleteRange(index, 1))
                return false;

            items.erase(items.begin() + index);
            return true;
        }

        // 删除一段范围并销毁这些对象
        bool DeleteRange(int index, int number)
        {
            if (!_DeleteRange(index, number))
                return false;

            items.erase(items.begin() + index, items.begin() + index + number);
            return true;
        }

        // 通过值查找并销毁，然后从列表移除（单个）
        bool DeleteByValue(ItemPointer &ip)
        {
            auto it = std::find(items.begin(), items.end(), ip);
            if (it == items.end()) return false;
            
            int idx = std::distance(items.begin(), it);
            _DeleteRange(idx, 1);
            items.erase(items.begin() + idx);
            return true;
        }

        // 通过数组的值逐个销毁并移除
        void DeleteByValue(ItemPointer *ip, int n)
        {
            if (!ip || n <= 0) return;

            for (int i = 0; i < n; ++i)
                DeleteByValue(ip[i]);
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
            // for(ManagedArray<T>::Iterator it : objlist) { ... }
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

        Iterator begin  () { return Iterator(items.data(), 0); }
        Iterator end    () { return Iterator(items.data(), (int)items.size()); }
        Iterator last   () { int c = (int)items.size(); return (c == 0) ? end() : Iterator(items.data(), c - 1); }

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

        ConstIterator begin ()const { return ConstIterator(items.empty()?nullptr:items.data(), 0); }
        ConstIterator end   ()const { return ConstIterator(items.empty()?nullptr:items.data(), (int)items.size()); }
        ConstIterator last  ()const { int c = (int)items.size(); return (c == 0) ? end() : ConstIterator(items.empty()?nullptr:items.data(), c - 1); }
    };// class ManagedArray
}// namespace hgl
