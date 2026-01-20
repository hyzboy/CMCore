#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/ObjectArray.h>
#include<type_traits>
namespace hgl
{
    /**
    * Queue模板类用于保存一个先进先出、后进后出的数据队列
    * 支持不同的底层数组类型
    */
    template<typename T, typename ArrayT = DataArray<T>> class QueueTemplate           ///队列顺序数据访问类
    {
    protected:

        ArrayT data_array[2];

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
        ArrayT &GetArray(){ return data_array[write_index]; }
        const ArrayT &GetArray() const { return data_array[write_index]; }

    public: //属性

        const int GetAllocCount() const
        {
            return data_array[0].GetAllocCount()+data_array[1].GetAllocCount();
        }

        const int GetCount() const
        {
            return data_array[0].GetCount()+data_array[1].GetCount()-read_offset;
        }

        virtual bool Reserve(int count)                                          ///<预分配数据空间
        {
            if(!data_array[0].Reserve(count))return(false);
            if(!data_array[1].Reserve(count))return(false);
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

            if(!data_array[write_index].Expand(count))
                return false;

            data_array[write_index].WriteAt(data,offset,count);

            return true;
        }

        virtual bool Push(const T &data){return Push((T*)&data,1);}               ///<压入一个数据

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
    };//template<typename T, typename ArrayT> class QueueTemplate

    /**
     * 用于平凡类型的队列 - 使用 DataArray
     */
    template<typename T> class Queue:public QueueTemplate<T, DataArray<T>>
    {
    public:
        static_assert(std::is_trivially_copyable_v<T>,
            "Queue<T> requires trivially copyable types (int, float, POD structs, etc). "
            "For non-trivial types (std::string, custom classes with dynamic memory), use ObjectQueue<T> instead.");

        Queue():QueueTemplate<T, DataArray<T>>(){}
        virtual ~Queue()=default;
    };//template<typename T> class Queue

    /**
     * 用于非平凡类型的队列 - 使用 ObjectArray
     * ObjectArray 自动管理对象的构造和析构
     */
    template<typename T> class ObjectQueue:public QueueTemplate<T, ObjectArray<T>>
    {
    public:
        static_assert(!std::is_trivially_copyable_v<T>,
            "ObjectQueue<T> requires non-trivial types (std::string, custom classes with dynamic memory, etc). "
            "For trivially copyable types (int, float, POD structs), use Queue<T> instead for better performance.");

        ObjectQueue():QueueTemplate<T, ObjectArray<T>>(){}
        virtual ~ObjectQueue() override { this->Free(); }

        /**
         * 压入一个指针（用于对象指针管理）
         * 队列接管对象所有权，析构时自动 delete
         */
        virtual bool Push(T *ptr)
        {
            if(!ptr) return false;
            // 对于 ObjectArray，我们需要存储对象副本
            // 这里传递对象引用
            return QueueTemplate<T, ObjectArray<T>>::Push(*ptr);
        }

        /**
         * 弹出一个对象（返回指针版本）
         */
        virtual bool Pop(T *&ptr)
        {
            // 从数组中读取对象
            T obj;
            if(!QueueTemplate<T, ObjectArray<T>>::Pop(obj))
            {
                ptr = nullptr;
                return false;
            }

            // 创建堆副本并返回指针
            ptr = new T(obj);
            return ptr != nullptr;
        }

    };//template<typename T> class ObjectQueue
}//namespace hgl
