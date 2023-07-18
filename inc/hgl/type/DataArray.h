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
        size_t count;                                                                               ///<当前数据数量
        size_t alloc_count;                                                                         ///<已分配的数据数量

    public:

                size_t  GetCount	    ()const{return count;}										///<取得数据数量(注：非字节数)
        const	size_t  GetAllocCount   ()const{return alloc_count;}								///<取得已分配的阵列大小(注：非字节数)
        const	size_t  GetBytes		()const{return count*sizeof(T);}							///<取得阵列已使用的字节数
        const	size_t  GetAllocBytes   ()const{return alloc_count*sizeof(T);}						///<取得阵列已分配空间字节数

        const   bool    IsEmpty         ()const{count==0;}                                          ///<是否为空

        const   T *     GetData()const{return items;}
                T *     data()        {return items;}

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
            items=nullptr;
            count=0;
            alloc_count=0;
        }

        DataArray(size_t size)
        {
            if(size<=0)
                items=nullptr;
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
            SAFE_FREE(items);

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
         * 解除数据关联
         */
        void Unlink()
        {
            items=nullptr;
            count=0;
            alloc_count=0;
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

        /**
        * 删除列表中的指定项(不关心顺序，如果删除中间的数据，可能会将最后面的数据拿过来填补)
        * @param start 要删除的数据项的索引起始值
        * @param number 要删除的数据项数量
        * @return 是否成功
        */
        bool Delete(int start,int number=1)
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

        /**
        * 删除列表中的指定项(关心顺序，删除中间数据后，会将后面的数据整体前移)
        * @param start 要删除的数据项的索引值
        * @param number 要删除的数据项数量
        * @return 是否成功
        */
        bool DeleteMove(int start,int number=1)
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
    };//template<typename T> class DataArray
}//namespace hgl
