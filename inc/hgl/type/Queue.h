#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/LifecycleManager.h>
#include<type_traits>
namespace hgl
{
    /**
    * Queue模板类用于保存一个先进先出、后进后出的数据队列
    */
    template<typename T> class QueueTemplate                                                        ///队列顺序数据访问类
    {
    protected:

        DataArray<T> data_array[2];

        int read_index;
        int write_index;

        int read_offset;

    protected:

        void SwapIndex()
        {
            if(read_index==0)
            {
                read_index=1;
                write_index=0;
            }
            else
            {
                read_index=0;
                write_index=1;
            }

            read_offset=0;
        }

    public: //兼容 Active 接口的简易 GetArray()
        DataArray<T> &GetArray(){ return data_array[write_index]; }
        const DataArray<T> &GetArray() const { return data_array[write_index]; }

    public: //属性

        const int GetAllocCount() const
        {
            return data_array[0].GetAllocCount()+data_array[1].GetAllocCount();
        }

        const int GetCount() const
        {
            return data_array[0].GetCount()+data_array[1].GetCount()-read_offset;
        }

        virtual bool PreAlloc(int count)                                          ///<预分配数据空间
        {
            if(!data_array[0].Alloc(count))return(false);
            if(!data_array[1].Alloc(count))return(false);
            return true;
        }

        const bool IsEmpty() const{return GetCount()==0;}                         ///<确认列表是否为空

        const bool Contains(const T &data)const
        {
            if(data_array[read_index].Find(data,read_offset)!=-1)
                return true;
            return data_array[write_index].Find(data)!=-1;
        }

    public: //方法

        QueueTemplate()
        {
            write_index=0;
            read_index=1;
            read_offset=0;
        }

        virtual ~QueueTemplate()=default;

        virtual bool Push(T *data,int count)                                      ///<压入一批数据
        {
            if(!data||count<=0)return false;

            int offset=data_array[write_index].GetCount();

            if(!data_array[write_index].AddCount(count))
                return false;

            data_array[write_index].WriteAt(data,offset,count);

            return true;
        }

        virtual bool Push(T &data){return Push(&data,1);}                         ///<压入一个数据

        virtual bool Peek(T &data)                                                ///<尝试访问一个数据
        {
            if(read_offset >= data_array[read_index].GetCount())
            {
                if(data_array[write_index].GetCount()<=0)
                   return false;
                data_array[write_index].ReadAt(data,0);
            }
            else
            {
                data_array[read_index].ReadAt(data,read_offset);
            }
            return true;
        }

        virtual bool Pop(T &data)                                                 ///<弹出一个数据
        {
            if(data_array[read_index].GetCount()<=read_offset)
            {
                if(data_array[write_index].GetCount()<=0)
                   return false;
                data_array[read_index].Clear();
                SwapIndex();
            }

            data_array[read_index].ReadAt(data,read_offset);
            ++read_offset;
            return true;
        }

    public:

        virtual void Clear()                                                      ///<清除所有数据
        {
            if(data_array[read_index].GetCount()>read_offset)       //还有没读完的，需要清掉
            {
                if constexpr(std::is_pointer_v<T>)
                    LifecycleTraitsOwningPtr<typename std::remove_pointer<T>::type>::destroy( data_array[read_index].GetData()+read_offset,
                                                                                               data_array[read_index].GetCount()-read_offset);
                else
                    LifecycleTraits<T>::destroy( data_array[read_index].GetData()+read_offset,
                                                 data_array[read_index].GetCount()-read_offset);
            }

            if constexpr(std::is_pointer_v<T>)
                LifecycleTraitsOwningPtr<typename std::remove_pointer<T>::type>::destroy( data_array[write_index].GetData(),
                                                                                           data_array[write_index].GetCount());
            else
                LifecycleTraits<T>::destroy( data_array[write_index].GetData(),
                                             data_array[write_index].GetCount());

            data_array[0].Clear();
            data_array[1].Clear();
            read_offset=0;
        }

        virtual void Free()                                                       ///<清除所有数据并释放内存
        {
            Clear();
            data_array[0].Free();
            data_array[1].Free();
        }
    };//template<typename T> class QueueTemplate

    template<typename T> class Queue:public QueueTemplate<T>
    {
    public:
        Queue():QueueTemplate<T>(){};
        virtual ~Queue()=default;
    };//template<typename T> class Queue:public QueueTemplate<T>

    template<typename T> class ObjectQueue:public QueueTemplate<T *>             ///对象队列
    {
    public:
        ObjectQueue():QueueTemplate<T *>(){}
        virtual ~ObjectQueue() override { Free(); }

        virtual bool Push(T *obj)
        {
            if(!obj)return false;
            return QueueTemplate<T *>::Push(obj);
        }

        virtual bool Push(T **obj_list,int count)
        {
            if(!obj_list)return false;
            return QueueTemplate<T *>::Push(obj_list,count);
        }

        T *PopObject()
        {
            T *obj;
            if(!QueueTemplate<T *>::Pop(obj))
                return nullptr;
            return obj;
        }

        void Free()
        {
            QueueTemplate<T *>::Clear();
            this->data_array[0].Free();
            this->data_array[1].Free();
        }
    };//template<typename T> class ObjectQueue
}//namespace hgl
