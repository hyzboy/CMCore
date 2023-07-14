#pragma once

#include<hgl/type/DataType.h>
namespace hgl
{
    /**
     * 数据阵列
     */
    template<typename T> class DataArray
    {
    protected:

        T *items;
        size_t count;
        size_t alloc_count;

    public:

                size_t GetCount	    ()const{return count;}										    ///<取得数据数量(注：非字节数)
        const	size_t GetAllocCount()const{return alloc_count;}									///<取得已分配的阵列大小(注：非字节数)
        const	size_t GetBytes		()const{return count*sizeof(T);}								///<取得阵列已使用的字节数
        const	size_t GetAllocBytes()const{return alloc_count*sizeof(T);}							///<取得阵列已分配空间字节数

                T *     GetData()const{return items;}

    public:

                T *     begin       (){return items;}                                               ///<取得阵列起始地址指针
                T *     end         (){return items+count;}                                         ///<取得阵列结束地址指针

        const   T *     begin       ()const{return items;}                                          ///<取得阵列起始地址指针
        const   T *     end         ()const{return items+count;}                                    ///<取得阵列结束地址指针

    public:

        /**
         * 分配指定空间出来，供未来使用
         */
        void Alloc(size_t size)
        {
            if(size<=alloc_count)
                return;

            alloc_count=power_to_2(size);

            if(!items)
                items=(T *)hgl_malloc(alloc_count*sizeof(T));
            else
                items=(T *)hgl_realloc(items,alloc_count*sizeof(T));
        }

        /**
         * 设置阵列长度
         */
        void SetCount(size_t size)																	///<设置阵列长度(注：非字节数)
        {
            Alloc(size);

            count=size;
        }

        /**
         * 增加阵列长度
         */
        void AddCount(size_t size)
        {
            SetCount(count+size);
        }

    public:

        DataArray()
        {
            items=0;
            count=0;
            alloc_count=0;
        }

        DataArray(size_t size)
        {
            if(size<=0)
                items=0;
            else
                items=(T *)hgl_malloc(size*sizeof(T));

            if(items)
            {
                count=size;
                alloc_count=size;
            }
            else
            {
                count=0;
                alloc_count=0;
            }
        }

        virtual ~DataArray()
        {
            Clear();
        }

        void Clear()
        {
            if(items)
                hgl_free(items);

            count=0;
            alloc_count=0;
        }

        void ClearData()
        {
            count=0;
        }

        void Zero()
        {
            if(items)
                memset(items,0,alloc_count);
        }

        /**
         * 设置数据,请保证数据使用hgl_malloc分配，否则会因为释放函数不配对出现错误
         */
        void SetData(T *d,int s)
        {
            Clear();

            items=d;
            alloc_count=s;
            count=s;
        }

        /**
         * 解除数据关联
         */
        void Unlink()
        {
            items=nullptr;
            count=0;
            alloc_count=0;
        }

        /**
         * 复制内存块中的数据
         * @param d 复制出来的数据指针
         * @param s 要复制出来的数据个数
         */
        void CopyData(const T *d,int s)
        {
            SetCount(s);
            memcpy(items,d,s*sizeof(T));
        }

        /**
        * 删除列表中的指定项,删除后将最后一个数据移到被删除的位置
        * @param index 要删除的数据项的索引值
        * @return 是否成功
        */
        bool Delete(int index)
        {
            if(count>0&&index>=0&&index<count)
            {
                --count;

                if(index<count)
                    memcpy(items+index,items+count,sizeof(T));      //将最后一个数据移到当前位置

                return(true);
            }
            else
                return(false);
        }

        /**
        * 删除列表中的指定项，删除后将后面的数据整体前移
        * @param index 要删除的数据项的索引值
        * @return 是否成功
        */
        bool DeleteMove(int index)
        {
            if(count>0&&index>=0&&index<count)
            {
                --count;

                if(index<count)
                    memmove(items+index,items+index+1,(count-index)*sizeof(T));

                return(true);
            }
            else
                return(false);
        }

        /**
        * 删除列表中的指定项
        * @param start 要删除的数据项的索引起始值
        * @param number 要删除的数据项数量
        * @return 是否成功
        */
        bool Delete(int start,int number)
        {
            if(start>=count)return(false);

            if(start<0)
            {
                number+=start;
                start=0;
            }

            if(start+number>count)
                number=count-start;

            if(number<=0)return(false);

            count-=number;

            if(start<count)
                memmove(items+start,items+start+number,(count-start)*sizeof(T));

            return(true);
        }

        operator T *()const
        {
            return items;
        }

        T *operator ->()const
        {
            return items;
        }

        T &operator[](int n)
        {
            return items[n];
        }

        const T &operator[](int n)const
        {
            return items[n];
        }
    };//template<typename T> class DataArray
}//namespace hgl
