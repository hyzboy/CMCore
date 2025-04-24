#pragma once

#include<hgl/type/Stack.h>
#include<initializer_list>
#include<algorithm>

namespace hgl
{
    /**
    * 索引数据列表<br>
    * IndexedList与ArrayList功能类似，但它的区别是它使用索引来访问数据。
    * 当数据被移动、删除、排序时，数据本身的内存并不会变动，只会调整索引。
    */
    template<typename T,typename I=int32> class IndexedList
    {
    protected:

        DataArray<T> data_array;
        DataArray<I> data_index;
        Stack<I> free_index;

    public: //属性

        const int32     GetAllocCount   ()const{return data_array.GetAllocCount();}
        const int32     GetCount        ()const{return data_index.GetCount();}
        const int32     GetFreeCount    ()const{return free_index.GetCount();}

        const size_t    GetTotalBytes   ()const{return data_index.GetCount()*sizeof(T);}

        const bool      IsEmpty         ()const{return data_index.IsEmpty();}

        bool PreAlloc(int32 count)
        {
            if(count<=0)return(false);

            data_array.Alloc(count);
            data_index.Alloc(count);
            free_index.PreAlloc(count);

            return(true);
        }

    public: // 迭代器

        class Iterator
        {
        private:

            IndexedList<T,I> *list;
            int32 current_index;

        public:

            using pointer = T*;
            using reference = T&;

        public:

            Iterator(const IndexedList<T, I>* lst, int32 idx):list(lst),current_index(idx){}

            T& operator*() const
            {
                return list->data_array[list->data_index[current_index]];
            }

            Iterator &operator++(){current_index++;return *this;}
            Iterator operator++(int){Iterator tmp=*this;++(*this);return tmp;}

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

        Iterator begin() const
        {
            return Iterator(this,0);
        }

        Iterator end() const
        {
            return Iterator(this,data_index.GetCount());
        }

        Iterator last() const
        {
            return Iterator(this,data_index.GetCount()-1);
        }

    public:

        IndexedList()=default;
        IndexedList(const T *lt,const int n){Add(lt,n);}
        IndexedList(const IndexedList<T,I> &lt){operator=(lt);}
        IndexedList(const std::initializer_list<T> &lt){operator=(lt);}
        virtual ~IndexedList(){Free();}

        /**
         * 向列表中添加一个空数据
         * @return 这个数据的指针
         */
        virtual T *Add()
        {
            if(free_index.IsEmpty())
            {
                data_array.AddCount(1);
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

                data_array.AddCount(1);
                data_index.Append(index);
            }
            else
            {
                free_index.Pop(index);
                data_index.Append(index);
            }

            hgl_cpy<T>(data_array[index],data);
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

                int32 index=data_array.GetCount();

                data_array.AddCount(n);
                data_index.AddCount(n);

                for(int32 i=0;i<mc;i++)
                {
                    free_index.Pop(index);
                    data_index.Append(index);

                    hgl_cpy<T>(data_array[index],data[i]);
                }

                n-=mc;
                data+=mc;

                result=mc;
            }

            //剩余空间没了

            if(n>0) //如果还有，那就整段添加吧
            {
                int32 index=data_array.GetCount();

                data_array.AddCount(n);
                data_index.AddCount(n);

                for(int32 i=0;i<n;i++)
                    data_index[index+i]=index+i;

                hgl_cpy<T>(data_array.At(index),data,n);

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

            DataArray<int32> sorted_index(count);
            Stack<int32> overflow_index;
            Stack<int32> space_location;

            overflow_index.PreAlloc(count);
            space_location.PreAlloc(count);

            hgl_cpy(sorted_index.GetData(),data_index.GetData(),count);

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

                    hgl_cpy<T>(data_array[new_location],data_array[data_index[index]]);

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
                hgl_cpy<T>(temp_array+start, data_array.GetData() + data_index[start], length);

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
    };//template<typename T> class IndexedList
}//namespace hgl
