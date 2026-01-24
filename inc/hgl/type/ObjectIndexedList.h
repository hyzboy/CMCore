#pragma once

#include <hgl/type/ObjectArray.h>
#include <hgl/type/ValueBuffer.h>
#include <hgl/type/Stack.h>
#include <hgl/type/ObjectUtil.h>
#include <initializer_list>
#include <algorithm>
#include <type_traits>

namespace hgl
{
    /**
    * 索引数据列表（非平凡类型版本）
    * 对应 IndexedList，但面向非平凡对象，显式调用构造/析构。
    * 对于平凡类型请使用 IndexedList。
    */
    template<typename T, typename I = int32> class ObjectIndexedList
    {
        static_assert(!std::is_trivially_copyable_v<T>, "ObjectIndexedList is for non-trivial types; use IndexedList for trivially copyable types.");

    protected:

        ObjectArray<T>   data_array;
        ValueBuffer<I>     data_index;
        Stack<I>         free_index;

    public: //属性

        const int32     GetAllocCount   ()const{return data_array.GetAllocCount();}
        const int32     GetCount        ()const{return data_index.GetCount();}
        const int32     GetFreeCount    ()const{return free_index.GetCount();}

        const size_t    GetTotalBytes   ()const{return static_cast<size_t>(data_index.GetCount())*sizeof(T);}

        const bool      IsEmpty         ()const{return data_index.IsEmpty();}

        bool Reserve(int32 count)
        {
            if(count<=0)return(false);

            data_array.Reserve(count);
            data_index.Reserve(count);
            free_index.Reserve(count);

            return(true);
        }

        const ObjectArray<T> &GetRawData()const{return data_array;}
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

            ObjectIndexedList<T,I> *list;
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

            Iterator(ObjectIndexedList<T, I>* lst, int32 idx):list(lst),current_index(idx){}

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

            const ObjectIndexedList<T,I> *list;
            int32 current_index;

        public:

            using pointer = T*;
            using reference = T&;

        public:

            ConstIterator(const ObjectIndexedList<T, I>* lst, int32 idx):list(lst),current_index(idx){}

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

        ObjectIndexedList()=default;
        ObjectIndexedList(const T *lt,const int n){Add(lt,n);} // copy elements
        ObjectIndexedList(const std::initializer_list<T> &lt){Add(lt.begin(),static_cast<int>(lt.size()));}
        ObjectIndexedList(const ObjectIndexedList<T,I> &)=delete;
        ObjectIndexedList& operator=(const ObjectIndexedList<T,I> &)=delete;
        ObjectIndexedList(ObjectIndexedList<T,I> &&)=default;
        ObjectIndexedList& operator=(ObjectIndexedList<T,I> &&)=default;
        virtual ~ObjectIndexedList(){Free();}

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

                T *slot=data_array.At(index);
                destroy_at(slot);
                construct_at(slot);
                return slot;
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

            T *slot=data_array.At(index);
            destroy_at(slot);
            construct_at_copy(slot,data);
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

                    T *slot=data_array.At(index);
                    destroy_at(slot);
                    construct_at_copy(slot,data[i]);
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
                    T *slot=data_array.At(index+i);
                    destroy_at(slot);
                    construct_at_copy(slot,data[i]);
                }

                result+=n;
            }

            return result;
        }

        virtual void Clear()
        {
            // 调用析构以释放资源，然后重新默认构造占位，保持 slots 始终处于已构造状态
            const int32 count=data_index.GetCount();
            for(int32 i=0;i<count;i++)
            {
                const int32 idx=data_index[i];
                T *slot=data_array.At(idx);
                destroy_at(slot);
                construct_at(slot);
            }

            data_index.Clear();
            free_index.Clear();
        }

        virtual void Free()
        {
            Clear();
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

            T *slot=data_array.At(index);
            destroy_at(slot);
            construct_at_copy(slot,value);
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
            {
                const int32 idx=data_index[i];
                T *slot=data_array.At(idx);
                destroy_at(slot);
                construct_at(slot); // 保持已构造状态以便未来复用
                free_index.Push(idx);
            }

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

            ObjectArray<T> new_data;
            new_data.Resize(count);

            for(int32 i=0;i<count;i++)
            {
                const int32 src=data_index[i];
                T *dst=new_data.At(i);
                destroy_at(dst);
                construct_at_move(dst,std::move(data_array[src]));
                data_index[i]=i;
            }

            data_array = std::move(new_data);
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

            ObjectArray<T> new_data;
            new_data.Resize(count);

            for(int i=0;i<count;i++)
            {
                const int32 src=data_index[i];
                T *dst=new_data.At(i);
                destroy_at(dst);
                construct_at_move(dst,std::move(data_array[src]));
                data_index[i]=i;
            }

            data_array = std::move(new_data);
            free_index.Clear();
        }
    };//template<typename T> class ObjectIndexedList
}//namespace hgl
