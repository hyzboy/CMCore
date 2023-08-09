#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/LifetimeCallback.h>
namespace hgl
{
    /**
    * Queue模板类用于保存一个先进先出、后进后出的数据队列
    */
    template<typename T> class Queue                                                                ///队列顺序数据访问类
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

    public: //属性

                const   int     GetAllocCount   ()const{return  data_array[0].GetAllocCount()+
                                                                data_array[1].GetAllocCount();}     ///<取得已分配的数据数量

                const   int     GetCount        ()const{return  data_array[0].GetCount()+
                                                                data_array[1].GetCount()-read_offset;}///<取得列表内数据数量

        virtual         bool    PreAlloc        (int count)                                         ///<预分配数据空间
                        {
                            if(!data_array[0].Alloc(count))return(false);
                            if(!data_array[1].Alloc(count))return(false);

                            return(true);   
                        }

                const   bool    IsEmpty         ()const{return GetCount()==0;}                      ///<确认列表是否为空

                const   bool    IsExist         (const T &data)const
                        {
                            if(data_array[read_index].Find(data,read_offset)!=-1)
                                return(true);

                            return data_array[write_index].Find(data)!=-1;
                        }

    public: //方法

        Queue()
        {
            write_index=0;
            read_index=1;
            read_offset=0;
        }

        virtual ~Queue()=default;

        virtual bool Push   (T *data,int count)                                                     ///<压入一批数据
        {
            if(!data||count<=0)return(false);

            int offset=data_array[write_index].GetCount();

            if(!data_array[write_index].AddCount(count))
                return(false);

            data_array[write_index].WriteAt(data,offset,count);

            return(true);
        }

        virtual bool Push   (T &data){return Push(&data,1);}                                        ///<压入一个数据

        virtual bool Peek   (T &data)                                                               ///<尝试访问一个数据
        {
            if(data_array[read_index].GetCount()>=read_offset)
            {
                if(data_array[write_index].GetCount()<=0)
                   return(false);

                data_array[write_index].ReadAt(data,0);

            }
            else
            {
                data_array[read_index].ReadAt(data,read_offset);
            }

            return(true);
        }

        virtual bool Pop    (T &data)                                                               ///<弹出一个数据
        {
            if(data_array[read_index].GetCount()<=read_offset)
            {
                if(data_array[write_index].GetCount()<=0)
                   return(false);

                data_array[read_index].Clear();

                SwapIndex();
            }

            data_array[read_index].ReadAt(data,read_offset);

            ++read_offset;

            return(true);
        }

    public:

        virtual void Clear  (DataLifetimeCallback<T> *dlc=nullptr)                                  ///<清除所有数据
        {
            if(dlc)
            {
                if(data_array[read_index].GetCount()>read_offset)       //还有没读完的，需要清掉

                dlc->Clear(data_array[read_index].GetData()+read_offset,
                           data_array[read_index].GetCount()-read_offset);
            }

            data_array[0].Clear();
            data_array[1].Clear();
        }

        virtual void Free   (DataLifetimeCallback<T> *dlc=nullptr)                                  ///<清除所有数据并释放内存
        {
            Clear(dlc);

            data_array[0].Free();
            data_array[1].Free();
        }
    };//template<typename T> class Queue

    template<typename T> class ObjectQueue:public Queue<T *>                                        ///对象队列
    {
        DefaultObjectLifetimeCallback<T> default_olc;

    public:

        using Queue<T *>::Queue;
        virtual ~ObjectQueue() override { Free(); }

        virtual bool Push(T *obj)
        {
            if(!obj)return(false);

            return Queue<T *>::Push(obj);
        }

        virtual T *Pop()
        {
            T *obj;

            if(!Queue<T *>::Pop(obj))
                return(nullptr);

            return obj;
        }

        void Clear(ObjectLifetimeCallback<T> *olc=nullptr)
        {
            if(!olc)
                olc=&default_olc;
            
            Queue<T *>::Clear(olc);
        }

        void Free(ObjectLifetimeCallback<T> *olc=nullptr)
        {
            ObjectQueue<T>::Clear(olc);

            data_array[0].Free();
            data_array[1].Free();
        }
    };//template<typename T> class ObjectQueue
}//namespace hgl
