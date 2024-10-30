#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/LifecycleManager.h>
namespace hgl
{
    /**
    * Queue模板类用于保存一个先进先出、后进后出的数据队列
    */
    template<typename T> class QueueTemplate                                                        ///队列顺序数据访问类
    {
    protected:

        DataLifecycleManager<T> *dlm;

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

                const   bool    Contains         (const T &data)const
                        {
                            if(data_array[read_index].Find(data,read_offset)!=-1)
                                return(true);

                            return data_array[write_index].Find(data)!=-1;
                        }

    public: //方法

        QueueTemplate(DataLifecycleManager<T> *_dlm)
        {
            dlm=_dlm;

            write_index=0;
            read_index=1;
            read_offset=0;
        }

        virtual ~QueueTemplate()=default;

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

        virtual void Clear  ()                                                                      ///<清除所有数据
        {
            if(dlm)
            {
                if(data_array[read_index].GetCount()>read_offset)       //还有没读完的，需要清掉
                    dlm->Clear(data_array[read_index].GetData()+read_offset,
                               data_array[read_index].GetCount()-read_offset);

                dlm->Clear(data_array[write_index].GetData(),
                           data_array[write_index].GetCount());
            }

            data_array[0].Clear();
            data_array[1].Clear();
        }

        virtual void Free   ()                                                                      ///<清除所有数据并释放内存
        {
            Clear(dlm);

            data_array[0].Free();
            data_array[1].Free();
        }
    };//template<typename T> class QueueTemplate

    template<typename T> class Queue:public QueueTemplate<T>
    {
    protected:

        DataLifecycleManager<T> DefaultDLM;

    public:

        Queue():QueueTemplate(&DefaultDLM){};
        virtual ~Queue()=default;
    };//template<typename T> class Queue:public QueueTemplate<T>

    template<typename T> class ObjectQueue:public QueueTemplate<T *>                                ///对象队列
    {
        ObjectLifecycleManager<T> DefaultOLM;

    public:

        ObjectQueue():QueueTemplate(&DefaultOLM){}
        virtual ~ObjectQueue() override { Free(); }

        virtual bool Push(T *obj)
        {
            if(!obj)return(false);

            return Queue<T *>::Push(obj);
        }

        virtual bool Push(T **obj_list,int count)
        {
            if(!obj_list)return(false);

            return Queue<T *>::Push(obj_list,count);
        }

        T *Pop()
        {
            T *obj;

            if(!Queue<T *>::Pop(obj))
                return(nullptr);

            return obj;
        }

        void Free()
        {
            ObjectQueue<T>::Clear();

            this->data_array[0].Free();
            this->data_array[1].Free();
        }
    };//template<typename T> class ObjectQueue
}//namespace hgl
