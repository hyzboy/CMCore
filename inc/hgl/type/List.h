#ifndef HGL_LIST_INCLUDE
#define HGL_LIST_INCLUDE

#include<stdlib.h>
#include<initializer_list>
#include<hgl/type/DataType.h>

namespace hgl
{
    /**
    * List类用于保存数据列表。可以在列表中添加、删除、查找、访问和排序数据。
    */
    template <typename T> class List                                                                ///列表处理类
    {
    protected:

        int count=0;
        int max_count=0;
        T *items=nullptr;

    public: //属性

                        int     GetCount    ()const{return count;}                                  ///<取得列表内数据数量
        virtual         void    SetCount    (int);                                                  ///<设置列表内数据数量
        virtual         void    PreMalloc   (int);                                                  ///<预分配指定数量的数据空间
                        T *     GetData     ()const{return items;}                                  ///<提供原始数据项
                        int     GetBytes    ()const{return count*sizeof(T);}                        ///<取得原始数据总字节数

                        T *     GetBegin    (){return (items&&count>0)?items:nullptr;}              ///<取得第一个数据项指针
                        T *     GetEnd      (){return (items&&count>0)?items+count-1:nullptr;}      ///<取得最后一个数据项指针

                        T *     begin()const{return items;}
                        T *     end()const{return items+count;}

    public: //方法

        List(){};                                                                                   ///<本类构造函数
        List(const List<T> &lt){operator=(lt);}                                                     ///<本类构造函数
        List(const std::initializer_list<T> &lt){operator=(lt);}

        virtual ~List(){Clear();}                                                                   ///<本类析构函数

        virtual T *  Add();                                                                         ///<添加一个空数据
        virtual int  Add(const T &);                                                                ///<增加一个数据
        virtual int  Add(const T &,int n);                                                          ///<重复增加一个数据
        virtual int  Add(const T *,int n);                                                          ///<增加一批数据
                int  Add(const List<T> &l){return Add(l.items,l.count);}                            ///<增加一批数据

        virtual void Clear();                                                                       ///<清除所有数据
        virtual void ClearData();                                                                   ///<清除所有数据，但不清空缓冲区
        virtual int  Find(const T &)const;                                                          ///<查找指定数据的索引
        virtual bool IsExist(const T &flag)const{return Find(flag)!=-1;}                            ///<确认数据项是否存在
        virtual bool Delete(int);                                                                   ///<删除指定索引的数据
        virtual bool Delete(int,int);                                                               ///<删除指定索引的数据
        virtual bool DeleteMove(int);                                                               ///<删除指定索引的数据,将后面紧邻的数据前移
        virtual bool DeleteByValue(const T &);                                                      ///<删除一个指定数据
        virtual void DeleteByValue(const T *,int);                                                  ///<删除一批指定的数据
        virtual void Exchange(int,int);                                                             ///<根据索引交换两个数据
        virtual void Insert(int,const T &);                                                         ///<在指定索引处插入一个数据
        virtual void Move(int,int);                                                                 ///<移动一个数据到移指索引处

                void DeleteClear();                                                                 ///<清除所有数据并全部调用delete

        virtual void operator = (const List<T> &);                                                  ///<操作符重载复制一个列表
        virtual void operator = (const std::initializer_list<T> &l);

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

                bool Get(int,T &)const;                                                             ///<取得指定索引处的数据
                void Set(int,const T &);                                                            ///<设置指定索引处的数据
                bool Rand(T &)const;                                                                ///<随机取得一个数据

        virtual bool Begin(T &)const;                                                               ///<取第一个数据
        virtual bool End(T &)const;                                                                 ///<取最后一个数据

        virtual void Enum(void (*enum_func)(T &))                                                   ///<枚举所有数据成员
        {
            T *obj=items;

            for(int i=0;i<count;i++)
            {
                enum_func(*obj);
                ++obj;
            }
        }

                /**
                 * 统计出不在in_list中的数据，产生的结果写入without_list
                 */
                void WithoutList(List<T> &without_list,const List<T> &in_list)
                {
                    without_list.ClearData();
                    const int count=this->GetCount();

                    if(count<=0)return;

                    without_list.PreMalloc(count);

                    const T *sp=this->GetData();

                    for(int i=0;i<count;i++)
                    {
                        if(!in_list.IsExist(*sp))
                            without_list.Add(*sp);

                        ++sp;
                    }
                }
    };//template <typename T> class List

    template<typename T> T *GetListObject(const List<T *> &list,const int index)
    {
        T *obj;

        if(list.Get(index,obj))
            return(obj);

        return(nullptr);
    }
}//namespace hgl

#include<hgl/type/List.cpp>
//--------------------------------------------------------------------------------------------------
namespace hgl
{
    /**
    * 自定义对象列表处理类与标准列表处理类的区别在于它对数据生成/清除时会多调用虚拟函数Create/Delte
    */
    template<typename T> class CusObjectList:public List<T *>                                       ///对象列表处理类
    {
    public:

        virtual void    SetCount(int);

//      virtual T *     CreateObject()=0;                                                           ///<创建一个数据，但不加入列表
        virtual void    DeleteObject(T *)=0;                                                        ///<删除一个数据

    public:

        typedef T *ItemPointer;

    public: //方法

        CusObjectList(){}
        virtual ~CusObjectList();

    public:

//      virtual T *     Append();                                                                   ///<追加一个数据
//      virtual T *     Insert(int);                                                                ///<在指定索引处创建一个数据
                void    Insert(int,const ItemPointer &);                                            ///<在指定索引处插入一个数据

        virtual void    Clear();                                                                    ///<清除所有数据
        virtual void    ClearData();                                                                ///<清除所有数据，但不清空缓冲区

        virtual bool    IsExist(const T *flag)const{return this->Find((T *)flag)!=-1;}              ///<确认数据项是否存在

        virtual bool    Unlink(int index){return List<T *>::Delete(index);}                         ///<将指定索引处的数据与列表断开
        virtual bool    UnlinkMove(int index){return List<T *>::DeleteMove(index);}                 ///<将指定索引处的数据与列表断开,将前移后面的数据
        virtual bool    Unlink(int start,int number){return List<T *>::Delete(start,number);}       ///<将指定索引处的数据与列表断开
        virtual bool    UnlinkByValue(const ItemPointer &ip){return List<T *>::DeleteByValue(ip);}  ///<将一个指定数据与列表断开
        virtual void    UnlinkByValue(const ItemPointer *ip,int n){List<T *>::DeleteByValue(ip,n);} ///<将一批指定数据与列表断开
        virtual void    UnlinkAll(){List<T *>::ClearData();}                                        ///<断开所有数据

        virtual bool    Delete(int);                                                                ///<删除指定索引处的数据
        virtual bool    DeleteMove(int);                                                            ///<删除指定索引处的数据
        virtual bool    DeleteByValue(const ItemPointer &);                                         ///<删除指定的一个数据
        virtual void    DeleteByValue(const ItemPointer *,int);                                     ///<删除指定的一批数据
        virtual void    DeleteAll();                                                                ///<删除所有数据

        virtual ItemPointer &operator[](int n)const                                                 ///<操作符重载取得指定索引处的数据
        {
            static T *null_pointer=nullptr;

            if(n<0||n>=this->count)
                return(null_pointer);

            return this->items[n];
        }

        virtual void Enum(void (*enum_func)(T *))                                                     ///<枚举所有数据成员
        {
            T **obj=this->items;

            for(int i=0;i<this->count;i++)
            {
                enum_func(*obj);

                ++obj;
            }
        }
    };//template <typename T> class CusObjectList

    /**
    * 对象列表处理类<br>
    * 将自定义对象列表中的Create重载为new,Delete重载为delete
    */
    template<typename T> class ObjectList:public CusObjectList<T>
    {
    private:

//      virtual T *     CreateObject(){return(new T);}                                              ///<创建一个数据
        virtual void    DeleteObject(T *obj){if(obj)delete obj;}                                    ///<删除一个数据

    public:

        T **begin()const{return this->items;}
        T **end()const{return this->items+this->count;}

    public:

        virtual ~ObjectList()
        {
            CusObjectList<T>::Clear();
        }
    };//class ObjectList
}//namespace hgl
#include<hgl/type/ObjectList.cpp>
#endif//HGL_LIST_INCLUDE
