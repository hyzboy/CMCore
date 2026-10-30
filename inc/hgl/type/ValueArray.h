#pragma once

#include<stdlib.h>
#include<initializer_list>
#include<type_traits>
#include<vector>
#include<algorithm>
#include<cstring>
#include<hgl/type/MemoryUtil.h>

namespace hgl
{
    // AI NOTE: Simple contiguous array wrapper for trivially copyable types.
    // No indirection or stable indices; inserts/deletes can move elements.
    // Prefer ManagedArray or PtrArray for non-trivial types.
    /**
    * ValueArray类用于保存数据列表。可以在列表中添加、删除、查找、访问和排序数据。<br>
    * ValueArray使用真实的数组保存数据，没有独立的索引，所以仅适用于不会变动的小数据的保存与访问。<br>
    * 如果使用大块的数据，仅需要频繁的增删排序，建议使用IndexedList.
    */
    template<typename T> class ValueArray                                                            ///阵列列表处理类
    {
    protected:

        std::vector<T> data_array;

    public: //属性

        static_assert(std::is_trivially_copyable_v<T>,
                      "ValueArray<T> requires trivially copyable types (int, float, POD structs, etc). "
                      "For non-trivial types (std::string, custom classes with dynamic memory), use ManagedArray<T> instead.");

                int     GetAllocCount   ()const{return (int)data_array.capacity();}         ///<取得已分配容量
                int     GetCount        ()const{return (int)data_array.size();}              ///<取得列表内数据数量
        bool    Resize          (int count){data_array.resize(count); return true;}       ///<设置列表内数据数量
        bool    Reserve         (int count){data_array.reserve(count); return true;}      ///<预分配指定数量的数据空间

                bool    IsEmpty         ()const{return data_array.empty();}               ///<确认列表是否为空

                        T *     GetData         (){return data_array.empty()?nullptr:data_array.data();}               ///<提供原始数据项
                const   T *     GetData         ()const{return data_array.empty()?nullptr:data_array.data();}         ///<提供原始数据项（只读）
                        size_t  GetTotalBytes   ()const{return data_array.size()*sizeof(T);}         ///<取得原始数据总字节数（size_t 防止大数组溢出）

                size_t  size            ()const{return data_array.size();}                  ///<元素数量（STL 风格别名）
                bool    empty           ()const{return data_array.empty();}                 ///<是否为空（STL 风格别名）

                        T *     begin           (){return data_array.empty()?nullptr:data_array.data();}
                const   T *     begin           ()const{return data_array.empty()?nullptr:data_array.data();}
                        T *     end             (){return data_array.empty()?nullptr:data_array.data()+data_array.size();}
                const   T *     end             ()const{return data_array.empty()?nullptr:data_array.data()+data_array.size();}
                        T *     last            (){return data_array.empty()?nullptr:data_array.data()+data_array.size()-1;}
                const   T *     last            ()const{return data_array.empty()?nullptr:data_array.data()+data_array.size()-1;}

    public:

            /// 只读访问底层 vector（保持原有接口）
            const std::vector<T> &GetArray()const     {return data_array;}

            /**
             * 可写访问底层 vector（绕过容器不变量，慎用）。
             * 直接修改可能破坏容量/数量一致性。
             */
            std::vector<T> &      GetArrayUnsafe()    {return data_array;}

            explicit operator        std::vector<T> & ()       {return data_array;}
            explicit operator const  std::vector<T> & ()const  {return data_array;}
                    T &     operator[](int index)             {return data_array[index];}
            const   T &     operator[](int index)const        {return data_array[index];}

    public: //方法

        ValueArray()=default;                                                                             ///<本类构造函数
        ValueArray(const T *lt,const int n){Add(lt,n);}                                                   ///<本类构造函数
        ValueArray(const ValueArray<T> &lt){operator=(lt);}                                                     ///<本类构造函数
        ValueArray(const std::initializer_list<T> &lt){operator=(lt);}

        ~ValueArray(){ClearAndDeallocate();}                                                                    ///<本类析构函数

        /**
         * 向列表中添加一个空数据
         * @return 这个数据的指针
         */
        T *  Add()
        {
            const int index=(int)data_array.size();
            data_array.resize(index+1);
            return &data_array.back();
        }

        /**
        * 向列表中添加一个数据对象
        * @param data 要添加的数据对象
        * @return 这个数据的索引号
        */
        int  Add(const T &data)
        {
            const int index=(int)data_array.size();
            data_array.push_back(data);
            return index;
        }

        /**
        * 重复向列表中添加一个数据对象
        * @param data 要添加的数据对象
        * @param n 要添加的数据个数
        * @return 这个数据的索引号
        * @return >0 出错
        */
        int  RepeatAdd(const T &data,int n)
        {
            if(n<=0)return(-1);

            const int ec=(int)data_array.size();
            data_array.insert(data_array.end(), n, data);
            return(ec);
        }

        /**
        * 向列表中添加一批数据对象
        * @param data 要添加的数据对象
        * @param n 要添加的数据数量
        * @return 起始数据的索引号
        */
        int  Add(const T *data,int n)
        {
            if(!data||n<=0)
                return(-1);

            const int ec=(int)data_array.size();
            data_array.insert(data_array.end(), data, data+n);
            return(ec);
        }

                int  Add(const ValueArray<T> &l){return Add(l.GetData(),l.GetCount());}                            ///<增加一批数据

        void ClearAndDeallocate()
        {
            data_array.clear();
            data_array.shrink_to_fit();
        }                                                     ///<清除所有数据，并释放内存
        void Clear(){data_array.clear();}                                                   ///<清除所有数据，但不清空缓冲区

        int  Find(const T &data)const
        {
            auto it = std::find(data_array.begin(), data_array.end(), data);
            return (it == data_array.end()) ? -1 : (int)std::distance(data_array.begin(), it);
        }                        ///<查找指定数据的索引

        bool Contains(const T &flag)const{return Find(flag)>=0;}                            ///<确认数据项是否存在

        /**
         * 删除指定索引的数据，保持后续元素顺序（O(n)）。
         * @param start 起始索引
         * @param num 删除数量
         */
        bool Delete(int start,int num=1)
        {
            if(start<0 || start+num>(int)data_array.size()) return false;

            data_array.erase(data_array.begin()+start, data_array.begin()+start+num);
            return true;
        }              ///<删除指定索引的数据

        /**
         * 删除指定索引的数据（O(1)，不保序）。
         * 用最后一个元素填补被删位置，因此元素顺序可能改变。
         * 需要保持顺序时请使用 Delete()。
         */
        bool DeleteSwap(int index)
        {
            const int count=(int)data_array.size();
            if(index<0 || index>=count) return false;

            if(index!=count-1)
                data_array[index]=data_array[count-1];

            data_array.pop_back();
            return true;
        }

        /**
         * Delete() 的历史别名：两者都保序，行为完全一致。
         */
        bool DeleteShift(int start,int num=1)
        {
            return Delete(start, num);
        }      ///<删除指定索引的数据,将后面紧邻的数据前移（等价于 Delete）

        /**
        * 删除列表中的指定项
        * @param data 要删除的数据项
        * @return 是否成功
        */
        bool DeleteByValue(const T &data)
        {
            const int pos=Find(data);

            return(pos>=0?Delete(pos,1):false);
        }

        /**
        * 删除列表中的指定项
        * @param data 要删除的数据项
        * @param n 要删除的数据个数
        * @return 成功删除的数据个数
        */
        int  DeleteByValue(const T *data,int n)
        {
            int result=0;

            if(!data || n<=0)
                return 0;

            while(n--)
            {
                int index=Find(*data);

                ++data;

                if(index>=0)
                    if(Delete(index))
                        ++result;
            }

            return result;
        }

        void Exchange(int a,int b)
        {
            if(a>=0 && a<(int)data_array.size() && b>=0 && b<(int)data_array.size())
                std::swap(data_array[a], data_array[b]);
        }                               ///<根据索引交换两个数据

        bool Insert(int pos,const T &data)
        {
            if(pos<0 || pos>(int)data_array.size()) return false;

            data_array.insert(data_array.begin()+pos, data);
            return true;
        }          ///<在指定索引处插入一个数据

        /**
        * 在指定索引处插入一批数据
        * @param pos 插入的位置
        * @param data 要插入的数据
        * @param number 要插入的数据个数
        */
        bool Insert(int pos,const T *data,const int number)
        {
            if(!data || number<=0 || pos<0 || pos>(int)data_array.size()) return false;

            data_array.insert(data_array.begin()+pos, data, data+number);
            return true;
        }

        /**
        * 移动一批数据到新的位置
        * @param new_pos 新的位置
        * @param old_pos 原来的位置
        * @param move_count 要移动的数据个数
        */
        void Move(const int new_pos,const int old_pos,const int move_count)
        {
            if(old_pos<0 || old_pos+move_count>(int)data_array.size()) return;
            if(new_pos<0 || new_pos>(int)data_array.size()) return;
            if(move_count<=0) return;

            // 如果目标位置在源范围内，不处理
            if(new_pos>=old_pos && new_pos<=old_pos+move_count) return;

            std::vector<T> temp(data_array.begin()+old_pos, data_array.begin()+old_pos+move_count);
            data_array.erase(data_array.begin()+old_pos, data_array.begin()+old_pos+move_count);

            int actual_new_pos = new_pos;
            if(new_pos > old_pos)
                actual_new_pos -= move_count;

            data_array.insert(data_array.begin()+actual_new_pos, temp.begin(), temp.end());
        }

        ValueArray<T>& operator = (const ValueArray<T> &da)
        {
            if(&da != this)
                data_array = da.data_array;
            return *this;
        }

        ValueArray<T>& operator = (const std::initializer_list<T> &l)
        {
            data_array.assign(l.begin(), l.end());
            return *this;
        }                  ///<操作符重载复制一个列表

        void operator += (const T &obj){Add(obj);}                                                ///<操作符重载添加一个数据
        void operator << (const T &obj){Add(obj);}                                                ///<操作符重载添加一个数据
        void operator -= (const T &obj){DeleteByValue(obj);}                                      ///<操作符重载删除一个数据

                T *  At(const int index)
        {
            if(index<0 || index>=(int)data_array.size()) return nullptr;
            return &data_array[index];
        }                         ///<取得指定序列号数据的索引

        const   T *  At(const int index)const
        {
            if(index<0 || index>=(int)data_array.size()) return nullptr;
            return &data_array[index];
        }                         ///<取得指定序列号数据的索引

                bool Get(int index,      T &data)const
                {
                    if(index<0 || index>=(int)data_array.size()) return false;
                    data = data_array[index];
                    return true;
                }    ///<取得指定索引处的数据

                bool Set(int index,const T &data)
                {
                    if(index<0 || index>=(int)data_array.size()) return false;
                    data_array[index] = data;
                    return true;
                }    ///<设置指定索引处的数据

        bool GetFirst   (T &data)const{return Get(0, data);}                   ///<取第一个数据
        bool GetLast    (T &data)const{return Get((int)data_array.size()-1, data);}        ///<取最后一个数据
    };//template <typename T> class ValueArray

    /**
     * @brief ValueArray<T[N]> 的存储单元包装
     * 用结构体包装数组，使 std::vector 依据 alignof(ElementType) 分配对齐内存，
     * 从而避免把字节缓冲区当作 T[N] 使用时产生的未对齐访问与严格别名问题。
     */
    template<typename ElementType, size_t ArraySize>
    struct ArrayElementHolder
    {
        ElementType value[ArraySize];
    };

    /**
     * @brief Specialization for array types (e.g., uint64[256])
     * 使用 ArrayElementHolder 作为存储单元，保证对齐且无需字节级 reinterpret_cast。
     */
    template<typename ElementType, size_t ArraySize>
    class ValueArray<ElementType[ArraySize]>
    {
    protected:
        using ArrayType = ElementType[ArraySize];
        using Holder    = ArrayElementHolder<ElementType, ArraySize>;

        std::vector<Holder> storage;

        int GetElementCount() const { return (int)storage.size(); }

    public:
        int GetAllocCount() const { return (int)storage.capacity(); }
        int GetCount() const { return GetElementCount(); }
        bool IsEmpty() const { return storage.empty(); }

        bool Resize(int count)
        {
            if (count < 0) return false;
            storage.resize((size_t)count);
            return true;
        }

        bool Reserve(int count)
        {
            if (count < 0) return false;
            storage.reserve((size_t)count);
            return true;
        }

        size_t GetTotalBytes() const { return storage.size() * sizeof(Holder); }

        size_t size() const { return storage.size(); }
        bool empty() const { return storage.empty(); }

        ArrayType* GetData()
        {
            return storage.empty() ? nullptr : &storage.data()->value;
        }

        const ArrayType* GetData() const
        {
            return storage.empty() ? nullptr : &storage.data()->value;
        }

        ArrayType* begin() { return GetData(); }
        const ArrayType* begin() const { return GetData(); }
        ArrayType* end() { ArrayType *d = GetData(); return d ? d + GetElementCount() : nullptr; }
        const ArrayType* end() const { const ArrayType *d = GetData(); return d ? d + GetElementCount() : nullptr; }
        ArrayType* last() { const int n = GetElementCount(); return n == 0 ? nullptr : GetData() + n - 1; }
        const ArrayType* last() const { const int n = GetElementCount(); return n == 0 ? nullptr : GetData() + n - 1; }

        bool Clear() { storage.clear(); return true; }
        bool ClearAndDeallocate() { std::vector<Holder>().swap(storage); return true; }

        bool Add(const ArrayType& data)
        {
            Holder holder;
            std::memcpy(holder.value, data, sizeof(ArrayType));
            storage.push_back(holder);
            return true;
        }

        bool Insert(int index, const ArrayType& data)
        {
            const int count = GetElementCount();
            if (index < 0 || index > count) return false;

            Holder holder;
            std::memcpy(holder.value, data, sizeof(ArrayType));
            storage.insert(storage.begin() + index, holder);
            return true;
        }

        bool Get(int index, ArrayType& data) const
        {
            const int count = GetElementCount();
            if (index < 0 || index >= count) return false;
            std::memcpy(data, storage[(size_t)index].value, sizeof(ArrayType));
            return true;
        }

        bool Set(int index, const ArrayType& data)
        {
            const int count = GetElementCount();
            if (index < 0 || index >= count) return false;
            std::memcpy(storage[(size_t)index].value, data, sizeof(ArrayType));
            return true;
        }

        /// 添加一个默认初始化的元素，返回其指针（与主模板 Add() 对齐）
        ArrayType* Add()
        {
            storage.emplace_back();
            return &storage.back().value;
        }

        /// 追加一批元素，返回起始索引（与主模板对齐）
        int Add(const ArrayType *data, int n)
        {
            if (!data || n <= 0) return -1;

            const int index = GetElementCount();

            for (int i = 0; i < n; ++i)
                Add(data[i]);

            return index;
        }

        int Add(const ValueArray<ArrayType>& other)
        {
            return Add(other.GetData(), other.GetCount());
        }

        /// 重复追加同一个元素，返回起始索引（与主模板对齐）
        int RepeatAdd(const ArrayType& data, int n)
        {
            if (n <= 0) return -1;

            const int index = GetElementCount();

            for (int i = 0; i < n; ++i)
                Add(data);

            return index;
        }

        /// 批量插入（与主模板对齐）
        bool Insert(int pos, const ArrayType *data, int number)
        {
            if (!data || number <= 0) return false;

            const int count = GetElementCount();
            if (pos < 0 || pos > count) return false;

            std::vector<Holder> batch((size_t)number);
            for (int i = 0; i < number; ++i)
                std::memcpy(batch[(size_t)i].value, data[i], sizeof(ArrayType));

            storage.insert(storage.begin() + pos, batch.begin(), batch.end());
            return true;
        }

        /// 交换两个元素（与主模板对齐）
        void Exchange(int a, int b)
        {
            const int count = GetElementCount();
            if (a < 0 || a >= count || b < 0 || b >= count) return;
            std::swap(storage[(size_t)a], storage[(size_t)b]);
        }

        /// 移动一段元素到新位置（与主模板对齐，语义一致）
        void Move(const int new_pos, const int old_pos, const int move_count)
        {
            if (old_pos < 0 || old_pos + move_count > GetElementCount()) return;
            if (new_pos < 0 || new_pos > GetElementCount()) return;
            if (move_count <= 0) return;

            if (new_pos >= old_pos && new_pos <= old_pos + move_count) return;

            if (new_pos < old_pos)
                std::rotate(storage.begin() + new_pos, storage.begin() + old_pos, storage.begin() + old_pos + move_count);
            else
                std::rotate(storage.begin() + old_pos, storage.begin() + old_pos + move_count, storage.begin() + new_pos);
        }

        bool GetFirst(ArrayType& data) const { return Get(0, data); }
        bool GetLast (ArrayType& data) const { return Get(GetElementCount() - 1, data); }

        int Find(const ArrayType& data) const
        {
            const int count = GetElementCount();
            const void* search_bytes = (const void*)data;

            for (int i = 0; i < count; ++i)
            {
                if (std::memcmp(storage[(size_t)i].value, search_bytes, sizeof(ArrayType)) == 0)
                    return i;
            }
            return -1;
        }

        /**
         * 删除指定索引的数据（O(1)，不保序）。
         * 用最后一个元素填补被删位置；需要保持顺序时请使用 Delete()。
         */
        bool DeleteSwap(int index)
        {
            const int count = GetElementCount();
            if (index < 0 || index >= count) return false;

            if (index != count - 1)
                std::memcpy(storage[(size_t)index].value, storage[(size_t)(count - 1)].value, sizeof(ArrayType));

            storage.pop_back();
            return true;
        }

        /**
         * 删除指定索引的数据，保持后续元素顺序（O(n)）。
         */
        bool Delete(int index)
        {
            const int count = GetElementCount();
            if (index < 0 || index >= count) return false;
            storage.erase(storage.begin() + index);
            return true;
        }

        bool Delete(const ArrayType& data)
        {
            int index = Find(data);
            return index >= 0 ? Delete(index) : false;
        }

        bool operator==(const ValueArray<ArrayType>& other) const
        {
            const size_t n = storage.size();
            if (n != other.storage.size()) return false;
            if (n == 0) return true;
            return std::memcmp(storage.data(), other.storage.data(), n * sizeof(Holder)) == 0;
        }

        bool operator!=(const ValueArray<ArrayType>& other) const
        {
            return !(*this == other);
        }

        ValueArray() = default;
        ~ValueArray() = default;
        ValueArray(const ValueArray& rhs) : storage(rhs.storage) {}
        ValueArray& operator=(const ValueArray& rhs)
        {
            if (this != &rhs)
                storage = rhs.storage;
            return *this;
        }
    };

    template<typename T> T *GetObjectFromMap(const ValueArray<T *> &list,const int index)
    {
        T *obj;

        if(list.Get(index,obj))
            return(obj);

        return(nullptr);
    }

    using CharPointerList=hgl::ValueArray<const char *>;
}//namespace hgl
