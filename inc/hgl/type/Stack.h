#pragma once

#include<hgl/type/DataArray.h>
namespace hgl
{
    /**
    * Stack模板类用于保存一个先进后出、后进先出的数据堆栈
    *
    * 注：这个类还在测试中，请不要修改这个类的源代码，如有修改意见，请致电作者。
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
                        int     GetBytes        ()const{return data_array.GetBytes();}              ///<取得原始数据总字节数

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

        virtual void Clear(){data_array.Clear();}                                                   ///<清除所有数据
        virtual void Free(){data_array.Free();}                                                     ///<清除所有数据并释放内存

        virtual void operator =(const DataArray<T> &da)                                             ///<复制一个堆栈
                {
                    data_array.SetCount(da.GetCount());

                    data_array.WriteAt(da.GetData(),0,da.GetCount());
                }

        virtual void operator =(const Stack<T> &s){this->operator=(s.data_array);}
    };//template<typename T> class Stack

    template<typename T> class ObjectStack:public Stack<T *>                                        ///堆栈对象
    {
    protected:

        virtual void DeleteObject(T *obj){if(obj)delete obj;}

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

        void Clear()
        {
            for(T *obj:data_array)
                DeleteObject(obj);

            data_array.Clear();
        }

        void Free()
        {
            ObjectStack<T>::Clear();

            data_array.Free();
        }
    };//template<typename T> class ObjectStack
}//namespace hgl
