#pragma once

#include<stdlib.h>
#include<initializer_list>
#include<hgl/type/DataArray.h>

namespace hgl
{
    /**
    * List类用于保存数据列表。可以在列表中添加、删除、查找、访问和排序数据。
    */
    template<typename T> class List                                                                 ///列表处理类
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

            operator        DataArray<T> & ()       {return data_array;}
            operator const  DataArray<T> & ()const  {return data_array;}

    public: //方法

        List()=default;                                                                             ///<本类构造函数
        List(const T *lt,const int n){Add(lt,n);}                                                   ///<本类构造函数
        List(const List<T> &lt){operator=(lt);}                                                     ///<本类构造函数
        List(const std::initializer_list<T> &lt){operator=(lt);}

        virtual ~List(){Clear();}                                                                   ///<本类析构函数

        /**
         * 向列表中添加一个空数据
         * @return 这个数据的指针
         */
        virtual T *  Add()
        {
            data_array.AddCount(1);
            return data_array.last();
        }

        /**
        * 向列表中添加一个数据对象
        * @param data 要添加的数据对象
        * @return 这个数据的索引号
        */
        virtual int  Add(const T &data)
        {
            const int index=GetCount();

            data_array.AddCount(1);

            data_array.WriteAt(data,index);
            
            return index;
        }

        /**
        * 重复向列表中添加一个数据对象
        * @param data 要添加的数据对象
        * @param n 要添加的数据个数
        * @return 这个数据的索引号
        * @return -1 出错
        */
        virtual int  RepeatAdd(const T &data,int n)
        {
            if(n<=0)return(-1);

            const int ec=data_array.GetCount();

            data_array.AddCount(n);

            hgl_set(data_array.data()+ec,data,n);

            return(ec);
        }

        /**
        * 向列表中添加一批数据对象
        * @param data 要添加的数据对象
        * @param n 要添加的数据数量
        * @return 起始数据的索引号
        */
        virtual int  Add(const T *data,int n)
        {
            if(!data||n<=0)
                return(-1);

            const int ec=data_array.GetCount();

            data_array.AddCount(n);

            data_array.WriteAt(data,ec,n);

            return(ec);
        }

                int  Add(const List<T> &l){return Add(l.items,l.count);}                            ///<增加一批数据

        virtual void Clear(){data_array.Clear();}                                                   ///<清除所有数据
        virtual void ClearData(){data_array.ClearData();}                                           ///<清除所有数据，但不清空缓冲区

        virtual int  Find(const T &data)const{return data_array.Find(data);}                        ///<查找指定数据的索引
        virtual bool IsExist(const T &flag)const{return Find(flag)!=-1;}                            ///<确认数据项是否存在

        virtual bool Delete(int start,int num=1){return data_array.Delete(start,num);}              ///<删除指定索引的数据
        virtual bool DeleteMove(int start,int num=1){return data_array.DeleteMove(start,num);}      ///<删除指定索引的数据,将后面紧邻的数据前移

        /**
        * 删除列表中的指定项
        * @param data 要删除的数据项
        * @return 是否成功
        */
        virtual bool DeleteByValue(const T &data)
        {
            const int pos=Find(data);

            return(pos>=0?Delete(pos,1):pos);
        }

        /**
        * 删除列表中的指定项
        * @param data 要删除的数据项
        * @param n 要删除的数据个数
        * @return 成功删除的数据个数
        */
        virtual int  DeleteByValue(const T *data,int n)
        {
            int result=0;

            while(n--)
            {
                int index=Find(*data);

                ++data;

                if(index!=-1)
                    if(Delete(index))
                        ++result;
            }

            return result;
        }

        virtual void Exchange(int a,int b){data_array.Exchange(a,b);}                               ///<根据索引交换两个数据
        virtual bool Insert(int pos,const T &data){return data_array.Insert(pos,&data,1);}          ///<在指定索引处插入一个数据

        /**
        * 在指定索引处插入一批数据
        * @param pos 插入的位置
        * @param data 要插入的数据
        * @param number 要插入的数据个数
        */
        virtual bool Insert(int pos,const T *data,const int number)
        {
            return data_array.Insert(pos,data,number);
        }

        /**
        * 移动一批数据到新的位置 
        * @param new_pos 新的位置
        * @param old_pos 原来的位置
        * @param move_count 要移动的数据个数
        */
        virtual bool Move(const int new_pos,const int old_pos,const int move_count)
        {
            return data_array.Move(new_pos,old_pos,move_count);
        }

        virtual void operator = (const DataArray<T> &da){data_array=da;}                            ///<操作符重载复制一个列表
        virtual void operator = (const std::initializer_list<T> &l){data_array=l;}                  ///<操作符重载复制一个列表

        virtual void operator += (const T &obj){Add(obj);}                                          ///<操作符重载添加一个数据
        virtual void operator << (const T &obj){Add(obj);}                                          ///<操作符重载添加一个数据
        virtual void operator -= (const T &obj){DeleteByValue(obj);}                                ///<操作符重载删除一个数据

                T *  GetPointer(const int index)                                                    ///<取得指定序列号数据的索引
                {
                    return(index>=count?nullptr:items+index);
                }

        const   T *  GetPointer(const int index) const                                              ///<取得指定序列号数据的索引
                {
                    return(index>=count?nullptr:items+index);
                }

                bool Get(int index,      T &data)const  {return data_array.ReadAt (data,index);}    ///<取得指定索引处的数据
        virtual bool Set(int index,const T &data)       {return data_array.WriteAt(data,index);}    ///<设置指定索引处的数据

        virtual bool GetFirst   (T &data)const{return data_array.ReadAt(data,0);}                   ///<取第一个数据
        virtual bool GetLast    (T &data)const{return data_array.ReadAt(data,GetCount()-1);}        ///<取最后一个数据
    };//template <typename T> class List

    template<typename T> T *GetListObject(const List<T *> &list,const int index)
    {
        T *obj;

        if(list.Get(index,obj))
            return(obj);

        return(nullptr);
    }
}//namespace hgl
