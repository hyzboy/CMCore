#pragma once

#include<hgl/type/List.h>

namespace hgl
{
    /**
     * 对象列表处理类与标准列表处理类的区别在于它对数据清除时会调用delete
     */
    template<typename T> class ObjectList:public List<T *>                                          ///对象列表处理类
    {
    public:

        using ItemPointer=T *;

        virtual void    DeleteObject(T *obj){if(obj)delete obj;}

    public: //方法

        using List<T *>::List;
        virtual ~ObjectList(){Free();}

    public:

                /**
                * 在指定位置插入一个对象
                */
                bool Insert(int index,const ItemPointer &obj) override
                {
                    return List<T *>::Insert(index,obj);
                }

        virtual void    Free() override                                                             ///<清除所有数据
        {
            Clear();
            List<T *>::Free();
        }

        virtual void    Clear() override                                                        ///<清除所有数据，但不清空缓冲区
        {
            for(auto *obj:data_array)
                DeleteObject(obj);

            List<T *>::Clear();
        }

        virtual bool    IsExist(const ItemPointer &flag)const override                              ///<确认数据项是否存在
        {
            return this->Find((T *)flag)!=-1;
        }

        virtual bool    Unlink(int index){return List<T *>::Delete(index);}                         ///<将指定索引处的数据与列表断开
        virtual bool    UnlinkMove(int index){return List<T *>::DeleteMove(index);}                 ///<将指定索引处的数据与列表断开,将前移后面的数据
        virtual bool    Unlink(int start,int number){return List<T *>::Delete(start,number);}       ///<将指定索引处的数据与列表断开
        virtual bool    UnlinkByValue(const ItemPointer &ip){return List<T *>::DeleteByValue(ip);}  ///<将一个指定数据与列表断开
        virtual void    UnlinkByValue(const ItemPointer *ip,int n){List<T *>::DeleteByValue(ip,n);} ///<将一批指定数据与列表断开
        virtual void    UnlinkAll(){List<T *>::Clear();}                                        ///<断开所有数据

    private:

        bool    _Delete(int index,int num)
        {
            if(index<0||num<=0||index+num>=GetCount())
                return(false);

            ItemPointer *p=data_array.data()+index;

            for(int i=0;i<num;i++)
            {
                DeleteObject(*p);
                ++p;
            }

            return true;
        }

    public:

        /**
        * 删除列表中的指定项
        *
        * 这个函数在删除指定对象时，附加使用delete方法
        * @param index 要删除的对象的索引值
        * @param num 要删除的对象数量
        * @return 是否成功
        */
        virtual bool    Delete(int index,int num=1) override
        {
            if(!_Delete(index,num))
                return(false);

            return List<T *>::Delete(index,num);
        }

        /**
        * 删除列表中的指定项
        *
        * 这个函数在删除指定对象时，附加使用delete方法
        * @param index 要删除的对象的索引值
        * @param num 要删除的对象数量
        * @return 是否成功
        */
        virtual bool    DeleteMove(int index,int num=1) override
        {
            if(!_Delete(index,num))
                return(false);

            return List<T *>::DeleteMove(index,num);
        }

        virtual bool    DeleteByValue(const ItemPointer &obj)  override                              ///<删除指定的一个数据
        {
            if(!obj)return(false);

            delete obj;

            return List<T *>::DeleteByValue(obj);
        }

        virtual int     DeleteByValue(const ItemPointer *obj_list,int num) override                  ///<删除指定的一批数据
        {
            if(!obj_list||num<=0)return(-1);

            ItemPointer *p=(ItemPointer *)obj_list;

            for(int i=0;i<num;i++)
            {
                DeleteObject(*p);
                ++p;
            }

            return List<T *>::DeleteByValue(obj_list,num);
        }

        virtual T *operator[](int n)const                                                           ///<操作符重载取得指定索引处的数据
        {
            T **obj=data_array.GetPointer(n);

            if(!obj)return(nullptr);

            return(*obj);
        }

        virtual void Enum(void (*enum_func)(T *))                                                   ///<枚举所有数据成员
        {
            if(!enum_func)return;

            for(auto *obj:data_array)
                enum_func(obj);
        }

        virtual bool SetCount(const int new_count) override
        {
            const int cur_count=GetCount();

            if(new_count==cur_count)return(true);

            if(new_count<=0)
            {
                Clear();
                return(true);
            }

            if(new_count>cur_count)
            {
                if(!data_array.SetCount(new_count))
                    return(false);

                ItemPointer *p=data_array.GetPointer(cur_count);

                hgl_zero<ItemPointer>(p,new_count-cur_count);

                return(true);
            }
            else
            {
                const int del_count=cur_count-new_count;

                Delete(GetCount()-del_count,del_count);
                return(true);
            }
        }

        virtual bool Set(int index,const ItemPointer &data) override
        {
            ItemPointer *p=data_array.GetPointer(index);

            if(!p)return(false);

            DeleteObject(*p);
            *p=data;

            return(true);
        }
    };//template <typename T> class ObjectList

    #define OBJECT_LIST_ARRAY_FREE(object_list)     for(auto &obj:object_list)obj.Free();
    #define OBJECT_LIST_ARRAY_CLEAR(object_list)    for(auto &obj:object_list)obj.Clear();
}//namespace hgl
