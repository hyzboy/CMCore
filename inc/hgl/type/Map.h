#pragma once

#include<hgl/type/KeyValue.h>
#include<hgl/type/ValueArray.h>
#include<hgl/type/Pair.h>
#include<hgl/type/Pool.h>

namespace hgl
{
    /**
    * 索引数据模板
    */
    template<typename K,typename V,typename KVData> class MapTemplate
    {
    protected:

        using ThisClass=MapTemplate<K,V,KVData>;

        using KVDataPool=ObjectPool<KVData>;
        using KVDataList=ValueArray<KVData *>;

        KVDataPool data_pool;
        KVDataList data_list;

    public:

        KVData **begin()const{return data_list.begin();}
        KVData **end()const{return data_list.end();}

    public: //方法

        MapTemplate()=default;
        virtual ~MapTemplate()=default;

        const   int     GetCount()const{return data_list.GetCount();}                               ///<取得数据总量
        const   bool    IsEmpty()const{return data_list.IsEmpty();}                                 ///<是否为空

                KVData *Add(const K &,const V &);                                                   ///<添加一个数据，数果索引已存在，返回nullptr
                void    Add(KVData *);                                                              ///<添加一个数据
                bool    FindPos(const K &,int &)const;                                              ///<查找数据如果插入后，会所在的位置，返回是否存在这个数据
                int     FindPos(const K &flag)const{int pos;FindPos(flag,pos);return(pos);}         ///<查找数据如果插入后，会所在的位置
                int     Find(const K &)const;                                                       ///<查找数据是否存在，返回-1表示数据不存在
                int     FindByValue(const V &)const;                                                ///<查找数据是否存在，返回-1表示数据不存在
                bool    ContainsKey(const K &key)const{return(Find(key)!=-1);}                      ///<确认这个数据是否存在
                bool    ContainsValue(const V &value)const{return(FindByValue(value)!=-1);}         ///<确认这个数据是否存在
                bool    Check(const K &key,const V &value)const;                                    ///<确认数据是否是这个
        virtual V *     GetValuePointer(const K &key)const;                                         ///<取得数据指针
        virtual int     GetValueAndSerial(const K &,V &) const;                                     ///<取得数据与索引
                bool    Get(const K &key,V &value)const{return(GetValueAndSerial(key,value)>=0);}   ///<取得数据
        virtual bool    GetAndDelete(const K &,V &);                                                ///<将指定数据从列表中移除，并获得这个数据
        virtual bool    DeleteByKey(const K &);                                                     ///<根据索引将指定数据从列表中移除
        virtual int     DeleteByKey(const K *,const int);                                           ///<根据索引将指定数据从列表中批量移除
        virtual bool    DeleteByValue(const V &);                                                   ///<根据数据将指定数据从列表中移除
        virtual bool    DeleteAt(int);                                                              ///<根据序号将指定数据从列表中移除
        virtual bool    DeleteAt(int,int);                                                          ///<根据序号将指定数据从列表中移除
        virtual bool    ChangeOrAdd(const K &,const V &);                                           ///<更改一个数据的内容(如不存在则添加)
        virtual bool    Change(const K &,const V &);                                                ///<更改一个数据的内容(如不存在则更改失效)
        virtual void    Free();                                                                     ///<清除所有数据，并释放内存
        virtual void    Clear();                                                                    ///<清除所有数据，但不释放内存

        KVDataList &    GetList(){return data_list;}                                                ///<取得线性列表
        KVData **       GetDataList()const{return data_list.GetData();}                             ///<取得纯数据线性列表

                template<typename IT>
                int     GetKeyList(IT &il_list)                                                     ///<取得所有索引合集
                {
                    const int count=data_list.GetCount();

                    if(count<=0)
                        return count;

                    KVData **idp=data_list.GetData();

                    for(int i=0;i<count;i++)
                    {
                        il_list.Add((*idp)->key);
                        ++idp;
                    }

                    return count;
                }

                template<typename IT>
                int     GetValueList(IT &il_list)                                                  ///<取得所有数值合集
                {
                    const int count=data_list.GetCount();

                    if(count<=0)
                        return count;

                    KVData **idp=data_list.GetData();

                    for(int i=0;i<count;i++)
                    {
                        il_list.Add((*idp)->value);
                        ++idp;
                    }

                    return count;
                }

                template<typename ITK,typename ITV>
                int     GetList(ITK &key_list,ITV &value_list)                                      ///<取得所有索引合集
                {
                    const int count=data_list.GetCount();

                    if(count<=0)
                        return count;

                    KVData **idp=data_list.GetData();

                    for(int i=0;i<count;i++)
                    {
                        key_list.Add((*idp)->key);
                        value_list.Add((*idp)->value);
                        ++idp;
                    }

                    return count;
                }

                KVData *GetItem(int n)const{return GetObjectFromMap(data_list,n);}                  ///<取指定序号的数据
                bool    GetBySerial(int,K &,V &) const;                                             ///<取指定序号的数据
                bool    GetKey(int,K &);                                                            ///<取指定序号的索引
                bool    GetValue(int,V &);                                                          ///<取指定序号的数据

                bool    SetValueBySerial(int,V &);                                                  ///<根据序号设置数据

                void    operator=(const ThisClass &);                                               ///<操作符重载，复制一个列表

                // Template variants to support lambda and other callable types (C++20 compatible)
                template<typename F>
                void    EnumKV(F &&func)                                                            ///<枚举所有键值对(支持lambda)
                {
                    const int count=data_list.GetCount();
                    if(count<=0) return;

                    KVData **idp=data_list.GetData();
                    for(int i=0;i<count;i++)
                    {
                        func((*idp)->key, (*idp)->value);
                        ++idp;
                    }
                }

                template<typename F>
                void    EnumKeys(F &&func)                                                          ///<枚举所有键(支持lambda)
                {
                    const int count=data_list.GetCount();
                    if(count<=0) return;

                    KVData **idp=data_list.GetData();
                    for(int i=0;i<count;i++)
                    {
                        func((*idp)->key);
                        ++idp;
                    }
                }

                template<typename F>
                void    EnumValues(F &&func)                                                        ///<枚举所有值(支持lambda)
                {
                    const int count=data_list.GetCount();
                    if(count<=0) return;

                    KVData **idp=data_list.GetData();
                    for(int i=0;i<count;i++)
                    {
                        func((*idp)->value);
                        ++idp;
                    }
                }

                void    WithList(KVDataList &with_list,const ValueArray<K> &in_list);                     ///<统计出所有在in_list中出现的数据，产生的结果写入with_list
                void    WithoutList(KVDataList &without_list,const ValueArray<K> &in_list);               ///<统计出所有没有出现在in_list中的数据，产生的结果写入without_list
    };//class MapTemplate

    template<typename K,typename V> class Map:public MapTemplate<K,V,KeyValue<K,V> >
    {
    public:

        using Iterator=KeyValue<K,V>;

    public:

        Map()=default;
        virtual ~Map()=default;
    };//class Map

    template<typename T_ID,typename T_U> T_U *GetObjectFromMap(Map<T_ID,T_U *> &list,const T_ID &id)
    {
        T_U *result;

        if(!list.Get(id,result))
            return(nullptr);

        return result;
    }

    template<typename K,typename V,typename KVData> class ObjectMapTemplate:public MapTemplate<K,V *,KVData>
    {
    protected:
        typedef MapTemplate<K,V *,KVData> SuperClass;

        void DeleteObject(KVData *ds)
        {
            if(!ds)return;
            V *&p=ds->value;
            if(p){ delete p; }
        }

        void DeleteObject(int index)
        {
            DeleteObject(GetObjectFromMap(this->data_list,index));
        }

    public:
        ObjectMapTemplate()=default;
        virtual ~ObjectMapTemplate(){ Clear(); }

        bool UnlinkByKey(const K &flag){ return UnlinkBySerial(SuperClass::Find(flag)); }
        bool UnlinkByValue(V *tp){ return UnlinkBySerial(this->FindByValue(tp)); }

        bool UnlinkBySerial(int index)
        {
            if(index<0||index>=this->data_list.GetCount())return false;
            SuperClass::DeleteAt(index);
            return true;
        }

        void UnlinkAll(){ SuperClass::Free(); }

        bool DeleteByKey(const K &flag){ return DeleteAt(SuperClass::Find(flag)); }
        bool DeleteByValue(V *tp){ return DeleteAt(this->FindByValue(tp)); }

        bool DeleteAt(int index)
        {
            if(index<0||index>=this->data_list.GetCount())return false;
            DeleteObject(index);
            SuperClass::DeleteAt(index);
            return true;
        }

        void DeleteAll()
        {
            for(auto &it:this->data_list) DeleteObject(it);
            SuperClass::Free();
        }

        void Update(const K &flag,V *data)
        {
            int index=this->Find(flag);
            if(index!=-1)
            {
                DeleteObject(index);
                KVData *dp=GetObjectFromMap(this->data_list,index);
                if(dp) dp->value=data;
            }
            else
                this->Add(flag,data);
        }

        bool Change(const K &flag,V *data)
        {
            int index=this->Find(flag);
            if(index!=-1)
            {
                DeleteObject(index);
                KVData *dp=GetObjectFromMap(this->data_list,index);
                if(!dp) return false;
                dp->value=data; return true;
            }
            return false;
        }

        void Clear(){ DeleteAll(); }

        V *operator[](const K &index)const
        {
            auto *obj=GetObjectFromMap(this->data_list,this->Find(index));
            if(obj) return obj->value; else return nullptr;
        }
    };//ObjectMapTemplate

    template<typename K,typename V> class ObjectMap:public ObjectMapTemplate<K,V,KeyValue<K,V *>>
    {
    public:
        ObjectMap()=default;
        virtual ~ObjectMap()=default;
    };
}//namespace hgl

#include<hgl/type/Map.cpp>
