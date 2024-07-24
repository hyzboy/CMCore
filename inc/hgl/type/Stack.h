#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/LifetimeCallback.h>
namespace hgl
{
    /**
    * Stack模板类用于保存一个先进后出、后进先出的数据堆栈
    */
    template<typename T> class Stack                                                                ///堆栈顺序数据访问类
    {
    protected:

        DataArray<T> data_array;

    public: //属性

                const   int     GetAllocCount   ()const{return data_array.GetAllocCount();}         ///<取得已分配容量
                const   int     GetCount        ()const{return data_array.GetCount();}              ///<取得列表内数据数量
        virtual         bool    SetCount        (int count){return data_array.SetCount(count);}     ///<设置列表内数据数量
        virtual         bool    PreAlloc        (int count){return data_array.Alloc(count);}        ///<预分配指定数量的数据空间

                const   bool    IsEmpty         ()const{return data_array.IsEmpty();}               ///<确认列表是否为空

                        T *     GetData         ()const{return data_array.GetData();}               ///<提供原始数据项
                        int     GetTotalBytes   ()const{return data_array.GetTotalBytes();}         ///<取得原始数据总字节数

                        T *     begin           ()const{return data_array.begin();}
                        T *     end             ()const{return data_array.end();}

    public:

            DataArray<T> &      GetArray()          {return data_array;}
            const DataArray<T> &GetArray()const     {return data_array;}

            operator        DataArray<T> & ()       {return data_array;}
            operator const  DataArray<T> & ()const  {return data_array;}

    public: //方法

        Stack()=default;
        virtual ~Stack()=default;

        virtual bool Push(T *data,int count)                                                        ///<压入多个数据
                {
                    if(!data||count<=0)return(false);

                    int offset=data_array.GetCount();

                    data_array.AddCount(count);

                    data_array.WriteAt(data,offset,count);

                    return(true);
                }

        virtual bool Push(T &data){return Push(&data,1)==1;}                                        ///<压入一个数据

        virtual bool Peek(T &data)                                                                  ///<尝试访问一个数据
                {
                    if(data_array.GetCount()<=0)return(false);

                    return data_array.ReadAt(data,data_array.GetCount()-1);
                }

        virtual bool Pop(T &data)                                                                   ///<弹出一个数据
                {
                    if(data_array.GetCount()<=0)return(false);

                    if(!data_array.ReadAt(data,data_array.GetCount()-1))
                        return(false);

                    data_array.AddCount(-1);
                    return(true);
                }

        virtual bool Pop(T *data,int count)
                {
                    if(data_array.GetCount()<count)return(false);

                    if(!data_array.ReadAt(data,data_array.GetCount()-count,count))
                        return(false);

                    data_array.AddCount(-count);
                    return(true);
                }

    public:

        virtual void Clear  (DataLifetimeCallback<T> *dlc=nullptr)                                  ///<清除所有数据
        {
            if(dlc)
                dlc->Clear(data_array.GetData(),data_array.GetCount());

            data_array.Clear();
        }

        virtual void Free   (DataLifetimeCallback<T> *dlc=nullptr)                                  ///<清除所有数据并释放内存
        {
            Clear(dlc);
            data_array.Free();
        }

        virtual void operator =(const DataArray<T> &da)                                             ///<复制一个堆栈
                {
                    data_array.SetCount(da.GetCount());

                    data_array.WriteAt(da.GetData(),0,da.GetCount());
                }

        virtual void operator =(const Stack<T> &s){this->operator=(s.data_array);}
    };//template<typename T> class Stack

    template<typename T> class ObjectStack:public Stack<T *>                                        ///堆栈对象
    {
        DefaultObjectLifetimeCallback<T> default_olc;

    public:

        using Stack<T *>::Stack;
        virtual ~ObjectStack() override {Free();}

        virtual bool Push(T *obj)
        {
            if(!obj)return(false);

            return Stack<T *>::Push(obj);
        }

        virtual T *Pop()
        {
            T *obj;

            if(!Stack<T *>::Pop(obj))
                return(nullptr);

            return obj;
        }

        void Clear(ObjectLifetimeCallback<T> *olc=nullptr)
        {
            if(!olc)
                olc=&default_olc;

            Stack<T *>::Clear(olc);
        }

        void Free(ObjectLifetimeCallback<T> *olc=nullptr)
        {
            ObjectStack<T>::Clear(olc);

            this->data_array.Free();
        }
    };//template<typename T> class ObjectStack
}//namespace hgl
