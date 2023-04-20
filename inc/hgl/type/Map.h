#ifndef HGL_MAP_INCLUDE
#define HGL_MAP_INCLUDE

#include<hgl/type/List.h>
#include<hgl/type/Pair.h>
#include<hgl/type/Pool.h>
#include<hgl/thread/RWLock.h>
namespace hgl
{
    template<typename K,typename V> struct KeyValue
    {
        K key;
        V value;
    };

    /**
    * 索引数据模板
    */
    template<typename K,typename V,typename KVData> class _Map
    {
    protected:
    
        using ThisClass=_Map<K,V,KVData>;

        using KVDataPool=ObjectPool<KVData>;
        using KVDataList=List<KVData *>;

        KVDataPool data_pool;
        KVDataList data_list;

    public:

//        KVData **begin()const{return data_list.begin();}
//        KVData **end()const{return data_list.end();}

    public: //方法

        _Map()=default;
        virtual ~_Map()=default;

        const   int     GetCount()const{return data_list.GetCount();}                               ///<取得数据总量
        const   bool    IsEmpty()const{return data_list.IsEmpty();}                                 ///<是否为空

                KVData *Add(const K &,const V &);                                                   ///<添加一个数据，数果索引已存在，返回nullptr
                void    Add(KVData *);                                                              ///<添加一个数据
                bool    FindPos(const K &,int &)const;                                              ///<查找数据如果插入后，会所在的位置，返回是否存在这个数据
                int     FindPos(const K &flag)const{int pos;FindPos(flag,pos);return(pos);}         ///<查找数据如果插入后，会所在的位置
                int     Find(const K &)const;                                                       ///<查找数据是否存在，返回-1表示数据不存在
                int     FindByValue(const V &)const;                                                ///<查找数据是否存在，返回-1表示数据不存在
                bool    KeyExist(const K &key)const{return(Find(key)!=-1);}                         ///<确认这个数据是否存在
                bool    ValueExist(const V &value)const{return(FindByValue(value)!=-1);}            ///<确认这个数据是否存在
                bool    Check(const K &key,const V &value)const;                                    ///<确认数据是否是这个
        virtual int     GetValueAndSerial(const K &,V &) const;                                     ///<取得数据与索引
                bool    Get(const K &key,V &value) const {return(GetValueAndSerial(key,value)>=0);} ///<取得数据
        virtual bool    Delete(const K &,V &);                                                      ///<将指定数据从列表中移除，并获得这个数据
        virtual bool    DeleteByKey(const K &);                                                     ///<根据索引将指定数据从列表中移除
        virtual int     DeleteByKey(const K *,const int);                                           ///<根据索引将指定数据从列表中批量移除
        virtual bool    DeleteByValue(const V &);                                                   ///<根据数据将指定数据从列表中移除
        virtual bool    DeleteBySerial(int);                                                        ///<根据序号将指定数据从列表中移除
        virtual bool    DeleteBySerial(int,int);                                                    ///<根据序号将指定数据从列表中移除
        virtual bool    ChangeOrAdd(const K &,const V &);                                           ///<更改一个数据的内容(如不存在则添加)
        virtual bool    Change(const K &,const V &);                                                ///<更改一个数据的内容(如不存在则更改失效)
        virtual void    Clear();                                                                    ///<清除所有数据
        virtual void    ClearData();                                                                ///<清除所有数据，但不释放内存

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


                KVData *GetItem(int n){return GetListObject(data_list,n);}                          ///<取指定序号的数据
                bool    GetBySerial(int,K &,V &) const;                                             ///<取指定序号的数据
                bool    GetKey(int,K &);                                                            ///<取指定序号的索引
                bool    GetValue(int,V &);                                                          ///<取指定序号的数据

                bool    SetValueBySerial(int,V &);                                                  ///<根据序号设置数据

                void    operator=(const ThisClass &);                                               ///<操作符重载，复制一个列表

                void    Enum(void (*enum_func)(const K &,V));                                       ///<枚举所有数据项
                void    EnumKey(void (*enum_func)(const K &));                                      ///<枚举所有索引
                void    EnumAllValue(void (*enum_func)(V));                                         ///<枚举所有数值
                void    EnumValue(bool (*enum_func)(V));                                            ///<枚举所有数值(返回true/false表示是否继续)

                void    WithList(KVDataList &with_list,const List<K> &in_list);                     ///<统计出所有在in_list中出现的数据，产生的结果写入with_list
                void    WithoutList(KVDataList &without_list,const List<K> &in_list);               ///<统计出所有没有出现在in_list中的数据，产生的结果写入without_list
    };//class _Map

    template<typename K,typename V> class Map:public _Map<K,V,KeyValue<K,V> >
    {
    public:

        Map()=default;
        virtual ~Map()=default;
    };//class Map

    template<typename T_ID,typename T_U> T_U *GetListObject(Map<T_ID,T_U *> &list,const T_ID &id)
    {
        T_U *result;

        if(!list.Get(id,result))
            return(nullptr);

        return result;
    }

    template<typename K,typename V,typename KVData> class _ObjectMap:public _Map<K,V *,KVData>
    {
    protected:

        typedef _Map<K,V *,KVData> SuperClass;

        virtual void    DeleteObject(const K &,V *)=0;                                              ///<删除一个数据
                void    DeleteObject(KVData *ds)
                {
                    if(!ds)return;

                    if(ds->value)                                                                   ///<存在数据就是nullptr的可能
                        DeleteObject(ds->key,ds->value);
                }

                void    DeleteObject(int index)
                {
                    DeleteObject(GetListObject(this->data_list,index));
                }

    public:

        _ObjectMap()=default;
        virtual ~_ObjectMap()
        {
            if(SuperClass::GetCount()>0)
            {
                //LOG_ERROR(u"这是一个严重的程序设计错误，会产生纯虚函数调用，请在派生类的析构函数中调用Clear函数以清除数据。");
                //LOG_ERROR(OS_TEXT("This is a serious design errors, will produce the pure virtual function call, please call in the destructor of the derived class the <Clear> function to clear the data."));
            }
        }

        /**
        * 断开一个数据
        * @param flag 要断开的数据标识
        * @return 是否断开成功
        */
        bool UnlinkByKey(const K &flag)
        {
            return UnlinkBySerial(SuperClass::Find(flag));
        }

        /**
        * 断开一个数据
        * @param tp 要断开的数据
        * @return 是否断开成功
        */
        bool UnlinkByValue(V *tp)
        {
            return UnlinkBySerial(this->FindByValue(tp));
        }

        /**
        * 断开一个数据关联
        * @param index 要断开的数据的序列号
        * @return 是否断开成功
        */
        bool UnlinkBySerial(int index)
        {
            if(index<0||index>=this->data_list.GetCount())return(false);

            SuperClass::DeleteBySerial(index);

            return(true);
        }

        /**
        * 断开所有数据关联
        */
        void UnlinkAll()
        {
            SuperClass::Clear();
        }

        /**
        * 删除一个数据
        * @param flag 要删除的数据标识
        * @return 是否删除成功
        */
        bool DeleteByKey(const K &flag)
        {
            return DeleteBySerial(SuperClass::Find(flag));
        }

        /**
        * 删除一个数据
        * @param tp 要删除的数据
        * @return 是否删除成功
        */
        bool DeleteByValue(V *tp)
        {
            return DeleteBySerial(this->FindByValue(tp));
        }

        /**
        * 删除一个数据
        * @param index 要删除的数据的序列号
        * @return 是否删除成功
        */
        bool DeleteBySerial(int index)
        {
            if(index<0||index>=this->data_list.GetCount())return(false);

            DeleteObject(index);
            SuperClass::DeleteBySerial(index);

            return(true);
        }

        /**
        * 清除所有数据
        */
        void DeleteAll()
        {
            int n=this->data_list.GetCount();

            while(n--)
                DeleteObject(n);

            _Map<K,V *,KVData>::Clear();
        }

        /**
        * 更新数据,如果数据不存在就添加一个新的
        * @param flag 数据标识
        * @param data 新的数据内容
        */
        void Update(const K &flag,V *data)
        {
            int index=this->Find(flag);

            if(index!=-1)
            {
                DeleteObject(index);

                KVData *dp=GetListObject(this->data_list,index);

                if(dp)
                    dp->value=data;
            }
            else
            {
                this->Add(flag,data);
            }
        }

        /**
        * 更改数据,这个更改和Set不同,它要求指定标识的数据必须存在，则否就会更改失败
        * @param flag 数据标识
        * @param data 新的数据内容
        * @param return 是否更改成功
        */
        bool Change(const K &flag,V *data)
        {
            int index=this->Find(flag);

            if(index!=-1)
            {
                DeleteObject(index);

                KVData *dp=GetListObject(this->data_list,index);

                if(!dp)
                    return(false);

                dp->value=data;
                return(true);
            }
            else
                return(false);
        }

        void Clear(){DeleteAll();}
    };//class _ObjectMap

    template<typename K,typename V,typename KVData> class CustomObjectMap:public _ObjectMap<K,V,KVData>
    {
    protected:

        //virtual T *       CreateObject(const F &){return(new T);}                                     ///<创建一个数据
        virtual void    DeleteObject(const K &,V *obj){delete obj;}                                 ///<删除一个数据

    public:

        CustomObjectMap()=default;
        virtual ~CustomObjectMap()
        {
            _ObjectMap<K,V,KVData>::Clear();
        }
    };//class CustomObjectMap

    template<typename K,typename V> class ObjectMap:public CustomObjectMap<K,V,KeyValue<K,V *> >
    {
    public:

        ObjectMap()=default;
        virtual ~ObjectMap()
        {
            CustomObjectMap<K,V,KeyValue<K,V *> >::Clear();
        }

        V *operator[](const K &index)const
        {
            auto *obj=GetListObject(this->data_list,this->Find(index));

            if(obj)
                return obj->value;
            else
                return nullptr;
        };
    };//class ObjectMap
}//namespace hgl
#include<hgl/type/Map.cpp>
#endif//HGL_MAP_INCLUDE
