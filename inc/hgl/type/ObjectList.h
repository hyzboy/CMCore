#pragma once

#include<hgl/type/ArrayList.h>
#include<hgl/type/LifecycleManager.h>

namespace hgl
{
    /**
     * 对象列表处理类与标准列表处理类的区别在于它对数据清除时会调用delete
     */
    template<typename T> class ObjectListTemplate:public ArrayList<T *>                                  ///对象列表处理类
    {
    protected:

        ObjectLifecycleManager<T> *dlm;                                                               ///<数据生命周期回调函数

    public:

        using ItemPointer=T *;

    public: //方法

        ObjectListTemplate(ObjectLifecycleManager<T> *_dlm):ArrayList<T *>(){dlm=_dlm;}
        virtual ~ObjectListTemplate(){Free();}

    public:

                /**
                * 在指定位置插入一个对象
                */
                bool Insert(int index,const ItemPointer &obj) override
                {
                    return ArrayList<T *>::Insert(index,obj);
                }

        virtual void    Free() override                                                             ///<清除所有数据并释放缓冲区
        {
            Clear();
            ArrayList<T *>::Free();
        }

        virtual void    Clear() override                                                            ///<清除所有数据，但不清空缓冲区
        {
            dlm->Clear(this->data_array.GetData(),this->data_array.GetCount());

            ArrayList<T *>::Clear();
        }

        virtual bool    Contains(const ItemPointer &flag)const override                              ///<确认数据项是否存在
        {
            return ArrayList<T *>::Find((T *)flag)!=-1;
        }

        virtual bool    Unlink(int index){return ArrayList<T *>::Delete(index);}                         ///<将指定索引处的数据与列表断开
        virtual bool    UnlinkMove(int index){return ArrayList<T *>::DeleteShift(index);}                 ///<将指定索引处的数据与列表断开,将前移后面的数据
        virtual bool    Unlink(int start,int number){return ArrayList<T *>::Delete(start,number);}       ///<将指定索引处的数据与列表断开
        virtual bool    UnlinkByValue(ItemPointer &ip){return ArrayList<T *>::DeleteByValue(ip);}        ///<将一个指定数据与列表断开
        virtual void    UnlinkByValue(ItemPointer *ip,int n){ArrayList<T *>::DeleteByValue(ip,n);}       ///<将一批指定数据与列表断开
        virtual void    UnlinkAll(){ArrayList<T *>::Clear();}                                            ///<断开所有数据

    private:

        bool    _Delete(int index,int num)
        {
            if(index<0||num<=0||index+num>=ArrayList<T *>::GetCount())
                return(false);

            dlm->Clear(this->data_array.GetData()+index,num);

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

            return ArrayList<T *>::Delete(index,num);
        }

        /**
        * 删除列表中的指定项
        *
        * 这个函数在删除指定对象时，附加使用delete方法
        * @param index 要删除的对象的索引值
        * @param num 要删除的对象数量
        * @return 是否成功
        */
        virtual bool    DeleteShift(int index,int num=1) override
        {
            if(!_Delete(index,num))
                return(false);

            return ArrayList<T *>::DeleteShift(index,num);
        }

        virtual bool    DeleteByValue(ItemPointer &obj)  override                              ///<删除指定的一个数据
        {
            if(!obj)return(false);

            delete obj;

            return ArrayList<T *>::DeleteByValue(obj);
        }

        virtual int     DeleteByValue(ItemPointer *obj_list,int num) override                  ///<删除指定的一批数据
        {
            if(!obj_list||num<=0)return(-1);

            dlm->Clear(obj_list,num);

            return ArrayList<T *>::DeleteByValue(obj_list,num);
        }

        virtual T *operator[](int n)const                                                           ///<操作符重载取得指定索引处的数据
        {
            T **obj=this->data_array.At(n);

            if(!obj)return(nullptr);

            return(*obj);
        }

        virtual void Enum(void (*enum_func)(T *))                                                   ///<枚举所有数据成员
        {
            if(!enum_func)return;

            for(auto *obj:this->data_array)
                enum_func(obj);
        }

        virtual bool SetCount(const int new_count) override
        {
            const int cur_count=this->GetCount();

            if(new_count==cur_count)return(true);

            if(new_count<=0)
            {
                Clear();
                return(true);
            }

            if(new_count>cur_count)
            {
                if(!this->data_array.SetCount(new_count))
                    return(false);

                ItemPointer *p=this->data_array.At(cur_count);

                hgl_zero<ItemPointer>(p,new_count-cur_count);

                return(true);
            }
            else
            {
                const int del_count=cur_count-new_count;

                Delete(this->GetCount()-del_count,del_count);
                return(true);
            }
        }

        virtual bool Set(int index,const ItemPointer &data) override
        {
            ItemPointer *p=this->data_array.At(index);

            if(!p)return(false);

            dlm->Clear(p);
            *p=data;

            return(true);
        }
    };//template <typename T> class ObjectList

    template<typename T> class ObjectList:public ObjectListTemplate<T>
    {
        ObjectLifecycleManager<T> DefaultOLM;

    public:

        ObjectList():ObjectListTemplate<T>(&DefaultOLM){}
        virtual ~ObjectList()=default;
    };//template<typename T> class ObjectList:public ObjectListTemplate<T>

    #define OBJECT_LIST_ARRAY_FREE(object_list)     for(auto &obj:object_list)obj.Free();
    #define OBJECT_LIST_ARRAY_CLEAR(object_list)    for(auto &obj:object_list)obj.Clear();
}//namespace hgl
