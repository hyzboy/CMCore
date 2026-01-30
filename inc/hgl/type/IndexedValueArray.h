#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/type/CompareUtil.h>
#include<hgl/type/Stack.h>
#include<initializer_list>
#include<algorithm>
#include<type_traits>

namespace hgl
{
    /**
    * 索引数据列表<br>
    * IndexedValueArray与ValueArray功能类似，但它的区别是它使用索引来访问数据。
    * 当数据被移动、删除、排序时，数据本身的内存并不会变动，只会调整索引。
    */
    template<typename T,typename I=int32> class IndexedValueArray
    {
        static_assert(std::is_trivially_copyable_v<T>, "IndexedValueArray only supports trivially copyable types; use IndexedManagedArray for non-trivial types.");
    protected:

        std::vector<T> data_array;
        std::vector<I> data_index;
        Stack<I> free_index;

    public: //属性

        const int32     GetAllocCount   ()const{return (int32)data_array.capacity();}
        const int32     GetCount        ()const{return (int32)data_index.size();}
        const int32     GetFreeCount    ()const{return free_index.GetCount();}

        const size_t    GetTotalBytes   ()const{return data_index.size()*sizeof(T);}

        const bool      IsEmpty         ()const{return data_index.empty();}

        bool Reserve(int32 count)
        {
            if(count<=0)return(false);

            data_array.reserve(count);
            data_index.reserve(count);
            free_index.Reserve(count);

            return(true);
        }

        const std::vector<T> &GetRawData()const{return data_array;}
        const std::vector<I> &GetRawIndex()const{return data_index;}

        T &operator[](int32 index)
        {
            if ( index<0||index>=(int32)data_index.size() )
                return data_array[0];
            return data_array[data_index[index]];
        }

        const T &operator[](int32 index)const
        {
            if ( index<0||index>=(int32)data_index.size() )
                return data_array[0];
            return data_array[data_index[index]];
        }

    public: // 迭代器

        class Iterator
        {
        private:

            IndexedValueArray<T,I> *list;
            int32 current_index;

        public:

            using pointer = T*;
            using reference = T&;

        public:

            Iterator()
            {
                list=nullptr;
                current_index=-1;
            }

            Iterator(IndexedValueArray<T, I>* lst, int32 idx):list(lst),current_index(idx){}

            T& operator*()
            {
                return (*list)[current_index];
            }

            Iterator &operator++(){current_index++;return *this;}
            Iterator operator++(int){Iterator tmp=*this;++(*this);return tmp;}

            Iterator &operator+=(int32 v) { current_index+=v;return *this; }
            Iterator &operator-=(int32 v) { current_index-=v;return *this; }

            Iterator &operator--(){current_index--;return *this;}
            Iterator operator--(int){Iterator tmp=*this;--(*this);return tmp;}

            bool operator==(const Iterator &other) const
            {
                return current_index==other.current_index;
            }

            bool operator!=(const Iterator &other) const
            {
                return current_index!=other.current_index;
            }
        };//class Iterator

        Iterator begin  (){return Iterator(this,0);}
        Iterator end    (){return Iterator(this,(int32)data_index.size());}
        Iterator last   (){return Iterator(this,(int32)data_index.size()-1);}

    public: // 只读迭代器

        class ConstIterator
        {
        private:

            const IndexedValueArray<T,I> *list;
            int32 current_index;

        public:

            using pointer = T*;
            using reference = T&;

        public:

            ConstIterator(const IndexedValueArray<T, I>* lst, int32 idx):list(lst),current_index(idx){}

            const T& operator*()const
            {
                return (*list)[current_index];
            }

            ConstIterator &operator++(){current_index++;return *this;}
            ConstIterator operator++(int){ConstIterator tmp=*this;++(*this);return tmp;}

            ConstIterator &operator--(){current_index--;return *this;}
            ConstIterator operator--(int){ConstIterator tmp=*this;--(*this);return tmp;}

            bool operator==(const ConstIterator &other) const
            {
                return current_index==other.current_index;
            }

            bool operator!=(const ConstIterator &other) const
            {
                return current_index!=other.current_index;
            }
        };//class ConstIterator

        ConstIterator begin ()const{return ConstIterator(this,0);}
        ConstIterator end   ()const{return ConstIterator(this,(int32)data_index.size());}
        ConstIterator last  ()const{return ConstIterator(this,(int32)data_index.size()-1);}

    public:

        IndexedValueArray()=default;
        IndexedValueArray(const T *lt,const int n){Add(lt,n);}
        IndexedValueArray(const IndexedValueArray<T,I> &lt){operator=(lt);}
        IndexedValueArray(const std::initializer_list<T> &lt){operator=(lt);}
        virtual ~IndexedValueArray(){Free();}

        /**
         * 向列表中添加一个空数据
         * @return 这个数据的指针
         */
        virtual T *Add()
        {
            if(free_index.IsEmpty())
            {
                int32 index=(int32)data_array.size();
                data_array.resize(index + 1);
                data_index.push_back(index);
                return &data_array.back();
            }
            else
            {
                int32 index;

                free_index.Pop(index);
                data_index.push_back(index);

                return &data_array[index];
            }
        }

        /**
         * 向列表中添加一个数据对象
         * @param data 要添加的数据对象
         * @return 这个数据的索引号
         */
        virtual int32 Add(const T &data)
        {
            int32 index;

            if(free_index.IsEmpty())
            {
                index=(int32)data_array.size();

                data_array.resize(index + 1);
                data_index.push_back(index);
            }
            else
            {
                free_index.Pop(index);
                data_index.push_back(index);
            }

            memcpy(&data_array[index], &data, sizeof(T));
            return index;
        }

        /**
         * 向列表中添加一批数据对象
         * @param data 要添加的数据对象
         * @param n 要添加的数据数量
         * @return 添加了几个
         */
        virtual int32 Add(const T *data,int n)
        {
            if(!data||n<=0)
                return(-1);

            const int32 fc=free_index.GetCount();

            int32 result=0;

            if(fc>0)        //如果有剩余的空间
            {
                const int32 mc=hgl_min(fc,n);

                int32 index;

                for(int32 i=0;i<mc;i++)
                {
                    free_index.Pop(index);
                    data_index.push_back(index);

                    memcpy(&data_array[index], &data[i], sizeof(T));
                }

                n-=mc;
                data+=mc;

                result=mc;
            }

            //剩余空间没了

            if(n>0) //如果还有，那就整段添加吧
            {
                int32 index=(int32)data_array.size();

                data_array.resize(index + n);

                for(int32 i=0;i<n;i++)
                {
                    data_index.push_back(index+i);
                    memcpy(&data_array[index+i], &data[i], sizeof(T));
                }

                result+=n;
            }

            return result;
        }

        virtual void Clear()
        {
            data_array.clear();
            data_index.clear();
            free_index.Clear();
        }

        virtual void Free()
        {
            data_array.clear();
            if constexpr (!std::is_array_v<T>)
            {
                data_array.shrink_to_fit();
            }
            data_index.clear();
            data_index.shrink_to_fit();
            free_index.Free();
        }

        const bool IsValidIndex(const int32 index)const
        {
            return !(index<0||index>=(int32)data_index.size());
        }

        virtual bool Insert(int32 pos,const T &value)
        {
            if(pos<0)return(false);
            if(pos>(int32)data_index.size())return(false);
            if(pos==(int32)data_index.size())
            {
                return Add(value)>=0;
            }

            int32 index;

            if(free_index.IsEmpty())
            {
                index=(int32)data_array.size();
                data_array.resize(index + 1);
                data_index.insert(data_index.begin() + pos, index);
            }
            else
            {
                free_index.Pop(index);
                data_index.insert(data_index.begin() + pos, index);
            }

            memcpy(&data_array[index], &value, sizeof(T));
            return true;
        }

        /**
        * 删除数据
        * @param start 起始删除的数据索引
        * @param count 删除个数
        * @return 删除成功的数据个数
        */
        virtual int32 Delete(int32 start,int32 count=1)
        {
            if(!IsValidIndex(start))return(-1);
            if(count<=0)return(count);

            if(start+count>(int32)data_index.size())
                count=(int32)data_index.size()-start;

            if(count<=0)return(0);

            for(int32 i=start;i<start+count;i++)
                free_index.Push(data_index[i]);

            data_index.erase(data_index.begin() + start, data_index.begin() + start + count);
            return count;
        }

        virtual bool Exchange(int32 a,int32 b)
        {
            if(!IsValidIndex(a))return(false);
            if(!IsValidIndex(b))return(false);

            hgl_swap(data_index[a],data_index[b]);

            return true;
        }

        /**
        * 收缩数据
        * 将所有数据收缩到前面
        */
        virtual bool Shrink()
        {
            if(data_index.empty())
                return(false);

            const int32 count=GetCount();

            std::vector<int32> sorted_index(count);
            Stack<int32> overflow_index;
            Stack<int32> space_location;

            overflow_index.Reserve(count);
            space_location.Reserve(count);

            memcpy(sorted_index.data(),data_index.data(),count * sizeof(int32));

            std::sort(sorted_index.begin(),sorted_index.end(),std::less<int>());

            //查找空的位置
            {
                int32 *p=sorted_index.data();

                for(int i=0;i<count;i++)
                {
                    if(i<*p)
                    {
                        space_location.Push(i);
                    }
                    else
                    {
                        ++p;
                    }
                }
            }

            //查找超出边界的索引
            {
                int32 *p=data_index.data();

                for(int i=0;i<count;i++)
                {
                    if(*p>=count)
                    {
                        overflow_index.Push(i);
                    }

                    ++p;
                }
            }

            if(overflow_index.IsEmpty())
                return(true);

            if(overflow_index.GetCount()!=space_location.GetCount())
                return(false);

            //直接将超出边界的数据移到空的位置上，并更新索引
            {
                int32 new_location;
                int32 index;

                while(space_location.Pop(new_location))
                {
                    overflow_index.Pop(index);

                    memcpy(&data_array[new_location], &data_array[data_index[index]], sizeof(T));

                    data_index[index]=new_location;
                }
            }

            free_index.Clear();
            return(true);
        }

        /**
        * 当前数据是否顺序排好的
        */
        const bool IsOrdered()const
        {
            const int count=(int)data_index.size();
            const int32 *p=data_index.data();

            for(int i=0;i<count;i++)
            {
                if(i!=*p)
                {
                    return(false);
                }

                ++p;
            }

            return(true);
        }

        virtual void Reorder()
        {
            const int count=(int)data_index.size();

            if(count<=0)return;

            if(IsOrdered())         //顺序没问题
                return;

            std::vector<T> temp_array(count);

            int i = 0;
            while (i < count)
            {
                int start = i; // 当前连续块的起始位置
                int end = i;   // 当前连续块的结束位置

                // 检查后续索引是否是连续的正确顺序
                while (end + 1 < count && data_index[end + 1] == data_index[end] + 1)
                {
                    ++end;
                }

                // 批量复制连续块的数据
                int length = end - start + 1;
                memcpy(temp_array.data()+start, data_array.data() + data_index[start], length * sizeof(T));

                // 更新索引
                i = end + 1;
            }

            // 将临时数组的数据复制回 data_array
            memcpy(data_array.data(), temp_array.data(), count * sizeof(T));

            // 更新 data_index，使其与 data_array 的顺序一致
            for (int i = 0; i < count; ++i)
            {
                data_index[i] = i;
            }
        }
    };//template<typename T> class IndexedValueArray

    /**
     * @brief Specialization of IndexedValueArray for array types (e.g., uint64[256])
     * Uses byte-level storage to avoid std::vector's array type construction issues
     */
    template<typename ElementType, size_t ArraySize, typename I>
    class IndexedValueArray<ElementType[ArraySize], I>
    {
    protected:
        using ArrayType = ElementType[ArraySize];
        std::vector<uint8_t> byte_storage;
        std::vector<I> data_index;
        Stack<I> free_index;

        int GetElementCount() const { return (int)(byte_storage.size() / sizeof(ArrayType)); }
        ArrayType* GetElementPtr(int idx) { return reinterpret_cast<ArrayType*>(byte_storage.data() + (size_t)idx * sizeof(ArrayType)); }
        const ArrayType* GetElementPtr(int idx) const { return reinterpret_cast<const ArrayType*>(byte_storage.data() + (size_t)idx * sizeof(ArrayType)); }

    public:
        int32 GetAllocCount() const { return (int32)(byte_storage.capacity() / sizeof(ArrayType)); }
        int32 GetCount() const { return (int32)data_index.size(); }
        int32 GetFreeCount() const { return free_index.GetCount(); }
        size_t GetTotalBytes() const { return data_index.size() * sizeof(ArrayType); }
        bool IsEmpty() const { return data_index.empty(); }

        bool Reserve(int32 count)
        {
            if (count <= 0) return false;
            byte_storage.reserve((size_t)count * sizeof(ArrayType));
            data_index.reserve(count);
            free_index.Reserve(count);
            return true;
        }

        const std::vector<uint8_t>& GetRawData() const { return byte_storage; }
        const std::vector<I>& GetRawIndex() const { return data_index; }

        ArrayType& operator[](int32 index)
        {
            if (index < 0 || index >= (int32)data_index.size())
                return *GetElementPtr(0);
            return *GetElementPtr(data_index[index]);
        }

        const ArrayType& operator[](int32 index) const
        {
            if (index < 0 || index >= (int32)data_index.size())
                return *GetElementPtr(0);
            return *GetElementPtr(data_index[index]);
        }

        bool IsValidIndex(int32 index) const
        {
            return !(index < 0 || index >= (int32)data_index.size());
        }

        ArrayType* Add()
        {
            I index;
            if (free_index.Pop(index))
            {
                data_index.push_back(index);
                return GetElementPtr(index);
            }
            else
            {
                index = GetElementCount();
                byte_storage.resize(byte_storage.size() + sizeof(ArrayType));
                data_index.push_back(index);
                return GetElementPtr(index);
            }
        }

        bool Add(const ArrayType& data)
        {
            I index;
            if (free_index.Pop(index))
            {
                std::memcpy(GetElementPtr(index), &data, sizeof(ArrayType));
                data_index.push_back(index);
            }
            else
            {
                index = GetElementCount();
                byte_storage.resize(byte_storage.size() + sizeof(ArrayType));
                std::memcpy(GetElementPtr(index), &data, sizeof(ArrayType));
                data_index.push_back(index);
            }
            return true;
        }

        int32 Add(const ArrayType* data, int n)
        {
            if (!data || n <= 0) return 0;

            const int32 fc = free_index.GetCount();
            int32 result = 0;

            if (fc > 0)
            {
                const int32 add_count = (n < fc) ? n : fc;
                
                for (int32 i = 0; i < add_count; ++i)
                {
                    I index;
                    free_index.Pop(index);
                    std::memcpy(GetElementPtr(index), &data[i], sizeof(ArrayType));
                    data_index.push_back(index);
                    ++result;
                }
                
                n -= add_count;
            }

            if (n > 0)
            {
                const I start_index = GetElementCount();
                byte_storage.resize(byte_storage.size() + (size_t)n * sizeof(ArrayType));
                
                for (int32 i = 0; i < n; ++i)
                {
                    std::memcpy(GetElementPtr(start_index + i), &data[result + i], sizeof(ArrayType));
                    data_index.push_back(start_index + i);
                }
                
                result += n;
            }

            return result;
        }

        bool Insert(int32 pos, const ArrayType& data)
        {
            if (pos < 0 || pos > (int32)data_index.size()) return false;
            
            I index;
            if (free_index.Pop(index))
            {
                std::memcpy(GetElementPtr(index), &data, sizeof(ArrayType));
                data_index.insert(data_index.begin() + pos, index);
            }
            else
            {
                index = GetElementCount();
                byte_storage.resize(byte_storage.size() + sizeof(ArrayType));
                std::memcpy(GetElementPtr(index), &data, sizeof(ArrayType));
                data_index.insert(data_index.begin() + pos, index);
            }
            return true;
        }

        bool Get(int32 index, ArrayType& data) const
        {
            if (index < 0 || index >= (int32)data_index.size()) return false;
            std::memcpy(&data, GetElementPtr(data_index[index]), sizeof(ArrayType));
            return true;
        }

        bool Set(int32 index, const ArrayType& data)
        {
            if (index < 0 || index >= (int32)data_index.size()) return false;
            std::memcpy(GetElementPtr(data_index[index]), &data, sizeof(ArrayType));
            return true;
        }

        bool Delete(int32 index)
        {
            if (index < 0 || index >= (int32)data_index.size()) return false;
            free_index.Push(data_index[index]);
            data_index.erase(data_index.begin() + index);
            return true;
        }

        int32 Delete(int32 start, int32 count)
        {
            if (!IsValidIndex(start)) return 0;
            if (count <= 0) return 0;

            if (start + count > (int32)data_index.size())
                count = (int32)data_index.size() - start;

            if (count <= 0) return 0;

            for (int32 i = start; i < start + count; i++)
                free_index.Push(data_index[i]);

            data_index.erase(data_index.begin() + start, data_index.begin() + start + count);
            return count;
        }

        bool Exchange(int32 a, int32 b)
        {
            if (!IsValidIndex(a)) return false;
            if (!IsValidIndex(b)) return false;

            hgl_swap(data_index[a], data_index[b]);
            return true;
        }

        bool Shrink()
        {
            if (data_index.empty()) return true;

            const int32 count = GetCount();

            std::vector<int32> sorted_index(count);
            Stack<int32> overflow_index;
            Stack<int32> space_location;

            overflow_index.Reserve(count);
            space_location.Reserve(count);

            memcpy(sorted_index.data(), data_index.data(), count * sizeof(int32));
            std::sort(sorted_index.begin(), sorted_index.end(), std::less<int>());

            // 查找空的位置
            {
                int32* p = sorted_index.data();
                for (int i = 0; i < count; i++)
                {
                    if (i < *p)
                    {
                        space_location.Push(i);
                    }
                    else
                    {
                        ++p;
                    }
                }
            }

            // 查找超出边界的索引
            {
                int32* p = data_index.data();
                for (int i = 0; i < count; i++)
                {
                    if (*p >= count)
                    {
                        overflow_index.Push(i);
                    }
                    ++p;
                }
            }

            if (overflow_index.IsEmpty())
                return true;

            if (overflow_index.GetCount() != space_location.GetCount())
                return false;

            // 直接将超出边界的数据移到空的位置上，并更新索引
            {
                int32 new_location;
                int32 index;

                while (space_location.Pop(new_location))
                {
                    overflow_index.Pop(index);
                    std::memcpy(GetElementPtr(new_location), GetElementPtr(data_index[index]), sizeof(ArrayType));
                    data_index[index] = new_location;
                }
            }

            free_index.Clear();
            return true;
        }

        bool IsOrdered() const
        {
            const int count = (int)data_index.size();
            const I* p = data_index.data();

            for (int i = 0; i < count; i++)
            {
                if (i != *p)
                {
                    return false;
                }
                ++p;
            }

            return true;
        }

        void Reorder()
        {
            const int count = (int)data_index.size();

            if (count <= 0) return;
            if (IsOrdered()) return;

            std::vector<uint8_t> temp_storage(count * sizeof(ArrayType));

            int i = 0;
            while (i < count)
            {
                int start = i;
                int end = i;

                while (end + 1 < count && data_index[end + 1] == data_index[end] + 1)
                {
                    ++end;
                }

                int length = end - start + 1;
                std::memcpy(
                    temp_storage.data() + (size_t)start * sizeof(ArrayType),
                    GetElementPtr(data_index[start]),
                    (size_t)length * sizeof(ArrayType)
                );

                i = end + 1;
            }

            std::memcpy(byte_storage.data(), temp_storage.data(), count * sizeof(ArrayType));

            for (int i = 0; i < count; ++i)
            {
                data_index[i] = i;
            }
        }

        bool Clear()
        {
            data_index.clear();
            free_index.Clear();
            return true;
        }

        bool Free()
        {
            std::vector<uint8_t>().swap(byte_storage);
            std::vector<I>().swap(data_index);
            free_index.Free();
            return true;
        }

        int32 Find(const ArrayType& data) const
        {
            const uint8_t* search_bytes = reinterpret_cast<const uint8_t*>(&data);
            int element_count = GetElementCount();
            
            for (int32 i = 0; i < (int32)data_index.size(); ++i)
            {
                if (std::memcmp(byte_storage.data() + (size_t)data_index[i] * sizeof(ArrayType), search_bytes, sizeof(ArrayType)) == 0)
                    return i;
            }
            return -1;
        }

    public: // 迭代器支持

        class Iterator
        {
        private:
            IndexedValueArray<ArrayType, I>* list;
            int32 current_index;

        public:
            using pointer = ArrayType*;
            using reference = ArrayType&;

            Iterator() : list(nullptr), current_index(0) {}
            Iterator(IndexedValueArray<ArrayType, I>* lst, int32 idx) : list(lst), current_index(idx) {}

            ArrayType& operator*() { return (*list)[current_index]; }
            
            Iterator& operator++() { ++current_index; return *this; }
            Iterator operator++(int) { Iterator tmp = *this; ++current_index; return tmp; }
            
            Iterator& operator+=(int32 v) { current_index += v; return *this; }
            Iterator& operator-=(int32 v) { current_index -= v; return *this; }
            
            Iterator& operator--() { --current_index; return *this; }
            Iterator operator--(int) { Iterator tmp = *this; --current_index; return tmp; }
            
            bool operator==(const Iterator& other) const { return current_index == other.current_index && list == other.list; }
            bool operator!=(const Iterator& other) const { return !(*this == other); }
        };

        Iterator begin() { return Iterator(this, 0); }
        Iterator end() { return Iterator(this, (int32)data_index.size()); }
        Iterator last() { return Iterator(this, (int32)data_index.size() - 1); }

        class ConstIterator
        {
        private:
            const IndexedValueArray<ArrayType, I>* list;
            int32 current_index;

        public:
            using pointer = const ArrayType*;
            using reference = const ArrayType&;

            ConstIterator(const IndexedValueArray<ArrayType, I>* lst, int32 idx) : list(lst), current_index(idx) {}

            const ArrayType& operator*() const { return (*list)[current_index]; }
            
            ConstIterator& operator++() { ++current_index; return *this; }
            ConstIterator operator++(int) { ConstIterator tmp = *this; ++current_index; return tmp; }
            
            ConstIterator& operator--() { --current_index; return *this; }
            ConstIterator operator--(int) { ConstIterator tmp = *this; --current_index; return tmp; }
            
            bool operator==(const ConstIterator& other) const { return current_index == other.current_index && list == other.list; }
            bool operator!=(const ConstIterator& other) const { return !(*this == other); }
        };

        ConstIterator begin() const { return ConstIterator(this, 0); }
        ConstIterator end() const { return ConstIterator(this, (int32)data_index.size()); }
        ConstIterator last() const { return ConstIterator(this, (int32)data_index.size() - 1); }

        IndexedValueArray() = default;
        ~IndexedValueArray() = default;
        
        IndexedValueArray(const ArrayType* lt, const int n) { Add(lt, n); }
        
        IndexedValueArray(const IndexedValueArray& rhs) 
            : byte_storage(rhs.byte_storage), data_index(rhs.data_index), free_index(rhs.free_index) {}
        
        IndexedValueArray(const std::initializer_list<ArrayType>& lt) { operator=(lt); }
        
        IndexedValueArray& operator=(const IndexedValueArray& rhs)
        {
            if (this != &rhs)
            {
                byte_storage = rhs.byte_storage;
                data_index = rhs.data_index;
                free_index = rhs.free_index;
            }
            return *this;
        }

        IndexedValueArray& operator=(const std::initializer_list<ArrayType>& lt)
        {
            Clear();
            for (const ArrayType& item : lt)
                Add(item);
            return *this;
        }
    };//template<typename ElementType, size_t ArraySize, typename I>
}//namespace hgl
