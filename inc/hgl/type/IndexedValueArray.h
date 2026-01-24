#pragma once

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

        ValueBuffer<T> data_array;
        ValueBuffer<I> data_index;
        ValueStack<I> free_index;

    public: //属性

        const int32     GetAllocCount   ()const{return data_array.GetAllocCount();}
        const int32     GetCount        ()const{return data_index.GetCount();}
        const int32     GetFreeCount    ()const{return free_index.GetCount();}

        const size_t    GetTotalBytes   ()const{return data_index.GetCount()*sizeof(T);}

        const bool      IsEmpty         ()const{return data_index.IsEmpty();}

        bool Reserve(int32 count)
        {
            if(count<=0)return(false);

            data_array.Reserve(count);
            data_index.Reserve(count);
            free_index.Reserve(count);

            return(true);
        }

        const ValueBuffer<T> &GetRawData()const{return data_array;}
        const ValueBuffer<I> &GetRawIndex()const{return data_index;}

        T &operator[](int32 index)
        {
            if ( index<0||index>=data_index.GetCount() )
                return data_array[0];
            return data_array[data_index[index]];
        }

        const T &operator[](int32 index)const
        {
            if ( index<0||index>=data_index.GetCount() )
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
        Iterator end    (){return Iterator(this,data_index.GetCount());}
        Iterator last   (){return Iterator(this,data_index.GetCount()-1);}

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
        ConstIterator end   ()const{return ConstIterator(this,data_index.GetCount());}
        ConstIterator last  ()const{return ConstIterator(this,data_index.GetCount()-1);}

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
                int32 index=data_array.GetCount();
                data_array.Expand(1);
                data_index.Append(index);
                return data_array.last();
            }
            else
            {
                int32 index;

                free_index.Pop(index);
                data_index.Append(index);

                return data_array.At(index);
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
                index=data_array.GetCount();

                data_array.Expand(1);
                data_index.Append(index);
            }
            else
            {
                free_index.Pop(index);
                data_index.Append(index);
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
                    data_index.Append(index);

                    memcpy(&data_array[index], &data[i], sizeof(T));
                }

                n-=mc;
                data+=mc;

                result=mc;
            }

            //剩余空间没了

            if(n>0) //如果还有，那就整段添加吧
            {
                int32 index=data_array.GetCount();

                data_array.Expand(n);

                for(int32 i=0;i<n;i++)
                {
                    data_index.Append(index+i);
                    memcpy(&data_array[index+i], &data[i], sizeof(T));
                }

                result+=n;
            }

            return result;
        }

        virtual void Clear()
        {
            data_array.Clear();
            data_index.Clear();
            free_index.Clear();
        }

        virtual void Free()
        {
            data_array.Free();
            data_index.Free();
            free_index.Free();
        }

        const bool IsValidIndex(const int32 index)const
        {
            return !(index<0||index>=data_index.GetCount());
        }

        virtual bool Insert(int32 pos,const T &value)
        {
            if(pos<0)return(false);
            if(pos>data_index.GetCount())return(false);
            if(pos==data_index.GetCount())
            {
                return Add(value)>=0;
            }

            int32 index;

            if(free_index.IsEmpty())
            {
                index=data_array.GetCount();
                data_array.Expand(1);
                data_index.Insert(pos,&index,1);
            }
            else
            {
                free_index.Pop(index);
                data_index.Insert(pos,&index,1);
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

            if(start+count>data_index.GetCount())
                count=data_index.GetCount()-start;

            if(count<=0)return(0);

            for(int32 i=start;i<start+count;i++)
                free_index.Push(data_index[i]);

            data_index.Delete(start,count);
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
            if(data_index.IsEmpty())
                return(false);

            const int32 count=GetCount();

            ValueBuffer<int32> sorted_index(count);
            ValueStack<int32> overflow_index;
            ValueStack<int32> space_location;

            overflow_index.Reserve(count);
            space_location.Reserve(count);

            mem_copy(sorted_index.GetData(),data_index.GetData(),count);

            std::sort(sorted_index.begin(),sorted_index.end(),std::less<int>());

            //查找空的位置
            {
                int32 *p=sorted_index.GetData();

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
                int32 *p=data_index.GetData();

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
            const int count=data_index.GetCount();
            const int32 *p=data_index.GetData();

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
            const int count=data_index.GetCount();

            if(count<=0)return;

            if(IsOrdered())         //顺序没问题
                return;

            T *temp_array=hgl_align_malloc<T>(count);

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
                memcpy(temp_array+start, data_array.GetData() + data_index[start], length * sizeof(T));

                // 更新索引
                i = end + 1;
            }

            // 将临时数组的数据复制回 data_array
            data_array.SetData(temp_array,count);

            // 更新 data_index，使其与 data_array 的顺序一致
            for (int i = 0; i < count; ++i)
            {
                data_index[i] = i;
            }
        }
    };//template<typename T> class IndexedValueArray
}//namespace hgl
