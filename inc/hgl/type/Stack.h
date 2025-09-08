#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/LifecycleTraits.h>
namespace hgl
{
    /**
    * Stack模板类用于保存一个先进后出、后进先出的数据堆栈
    */
    template<typename T> class StackTemplate                                                        ///堆栈顺序数据访问类
    {
    protected:
        DataArray<T> data_array;

    public: //属性

                const   int     GetAllocCount   ()const{return data_array.GetAllocCount();}         ///<取得已分配容量
                const   int     GetCount        ()const{return data_array.GetCount();}              ///<取得列表内数据数量
        virtual         bool    Resize          (int count){return data_array.Resize(count);}       ///<设置列表内数据数量
        virtual         bool    Reserve         (int count){return data_array.Reserve(count);}      ///<预分配指定数量的数据空间

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

        StackTemplate()=default; virtual ~StackTemplate()=default;

        virtual bool Push(const T *data,int count)                                                  ///<压入多个数据
                {
                    if(!data||count<=0)return(false);

                    int offset=data_array.GetCount();

                    data_array.Expand(count);

                    data_array.WriteAt(data,offset,count);

                    return(true);
                }

        virtual bool Push(const T &data){return Push(&data,1)==1;}                                  ///<压入一个数据

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

                    data_array.Expand(-1);
                    return(true);
                }

        virtual bool Pop(T *data,int count)
                {
                    if(data_array.GetCount()<count)return(false);

                    if(!data_array.ReadAt(data,data_array.GetCount()-count,count))
                        return(false);

                    data_array.Expand(-count);
                    return(true);
                }

    public:

        virtual void Clear  ()                                                                      ///<清除所有数据
        {
            LifecycleTraits<T>::destroy(data_array.GetData(),data_array.GetCount());
            data_array.Clear();
        }

        virtual void Free   ()                                                                      ///<清除所有数据并释放内存
        {
            Clear();
            data_array.Free();
        }

        virtual void operator =(const DataArray<T> &da)                                             ///<复制一个堆栈
                {
                    data_array.Resize(da.GetCount());

                    data_array.WriteAt(da.GetData(),0,da.GetCount());
                }

        virtual void operator =(const StackTemplate<T> &s){this->operator=(s.data_array);}
    };//template<typename T> class StackTemplate

    template<typename T> class Stack:public StackTemplate<T>
    {
    public:

        Stack()=default; virtual ~Stack()=default;
    };//template<typename T> class Stack:public StackTemplate<T>

    template<typename T> class ObjectStack:public StackTemplate<T *>                                ///堆栈对象
    {
    public:
        ObjectStack()=default; virtual ~ObjectStack() override {Free();}

        virtual bool Push(T *obj)
        {
            if(!obj)return(false);
            return StackTemplate<T *>::Push(obj,1);
        }

        T *Pop()
        {
            T *obj=nullptr;
            if(!StackTemplate<T *>::Pop(obj))
                return(nullptr);
            return obj;
        }

        virtual void Clear() override
        {
            // 拥有指针：删除
            T **ptr=this->data_array.GetData();
            int n=this->data_array.GetCount();
            LifecycleTraitsOwningPtr<T>::destroy(ptr,n);
            this->data_array.Clear();
        }

        void Free()
        {
            Clear();
            this->data_array.Free();
        }
    };//template<typename T> class ObjectStack
}//namespace hgl
