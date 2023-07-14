#pragma once

#include<hgl/type/Map.h>
namespace hgl
{
    template<typename K,typename V> struct RefKeyValue:public KeyValue<K,V *>       ///<带引用计数的Key/value对象结构
    {
        int ref_count;              ///<引用计数

    public:

        RefKeyValue():KeyValue<K,V *>()
        {
            ref_count=1;
        }
    };

    /**
    * 对象管理器,它没有缓冲管理，仅仅是统一管理对象，并保证不会被重复加载
    */
    template<typename K,typename V> class ObjectManage
    {
    protected:

        using ResItem=RefKeyValue<K,V>;

        _Map<K,V *,ResItem> items;

    protected:

        virtual void Clear(V *obj){delete obj;}                                 ///<对象释放虚拟函数(缺省为直接delete对象)

    public:

        virtual ~ObjectManage()
        {
            Clear();
        }

        virtual void        Clear()                                             ///<清除所有对象
        {
            int n=items.GetCount();

            while(n--)
            {
                ResItem *obj=items.GetItem(n);

                Clear(obj->value);
            }

            items.Clear();
        }

        virtual void        ClearFree()                                         ///<清除所有引用计数为0的对象
        {
            int n=items.GetCount();

            while(n--)
            {
                ResItem *obj=items.GetItem(n);

                if(obj->ref_count<=0)
                {
                    Clear(obj->value);
                    items.DeleteBySerial(n);
                }
            }
        }

                const int   GetCount()const{return items.GetCount();}           ///<取得对象数量

        virtual bool        Add(const K &key,V *obj)                            ///<添加一个对象
        {
            if(!obj)return(false);

            if(items.KeyExist(key))
                return(false);

            items.Add(key,obj);
            return(true);
        }

        virtual V *         Find(const K &key)                                  ///<查找一个对象(不增加引用计数)
        {
            int index=items.Find(key);

            if(index==-1)
                return(nullptr);

            ResItem *obj=items.GetItem(index);

            return obj->value;
        }

        virtual V *         Get(const K &key)                                   ///<取得一个对象(增加引用计数)
        {
            int index=items.Find(key);

            if(index!=-1)
            {
                ResItem *obj=items.GetItem(index);

                ++obj->ref_count;

                return obj->value;
            }

            return(nullptr);
        }

        virtual bool        ValueExist(V *value)                                ///<确认这个对象是否存在
        {
            return(items.FindByValue(value)!=-1);
        }
        
        /**
         * 获取指定对象的Key和引用计数
         * @param value 对象
         * @param key Key存放地址
         * @param ref_count 引用计数存放地址
         * @param 是否增加引用计数
         */
        virtual bool GetKeyByValue(V *value,K *key,uint *ref_count,bool inc_ref_count)
        {
            int index=items.FindByValue(value);

            if(index==-1)return(false);

            ResItem *obj=items.GetItem(index);

            if(inc_ref_count)
                ++obj->ref_count;

            if(key)
                *key=obj->key;

            if(ref_count)
                *ref_count=obj->ref_count;

            return(true);
        }

        /**
         * 根据序列号释放一个对象(减少引用计数).
         * 
         * \param index
         * \param zero_clear     引用为0后是否清除对象
         * \return 
         */
        virtual int ReleaseBySerial(int index,bool zero_clear=false)
        {
            if(index==-1)
            {
    //          ErrorHint(u"所释放的资源不存在");
                return(-1);
            }

            ResItem *obj=items.GetItem(index);

            --obj->ref_count;

            if(obj->ref_count>0)
                return obj->ref_count;

            if(zero_clear)
            {
                Clear(obj->value);

                items.DeleteBySerial(index);
            }

            return 0;
        }

        /**
         * 根据key释放一个对象(减少引用计数).
         * 
         * \param key
         * \param zero_clear    引用为0后是否清除对象
         * \return 
         */
        virtual int Release(const K &key,bool zero_clear=false)
        {
            return ReleaseBySerial(items.Find(key),zero_clear);
        }

        /**
         * 根据对象本身减少一次引用计数
         * @param td            对象指针
         * @param zero_clear    引用为0后是否清除对象
         */
        int Release(V *td,bool zero_clear=false)
        {
            return ReleaseBySerial(items.FindByValue(td),zero_clear);
        }
    };//template<typename K,typename V> class ObjectManage

    /**
     * 使用整型对象类做数标识的对象管理器
     */
    template<typename K,typename V> class IDObjectManage:public ObjectManage<K,V>
    {
        K id_count=0;

    public:

        using ObjectManage<K,V>::ObjectManage;
        virtual ~IDObjectManage()=default;

        virtual K Add(V *value)
        {
            if(!value)return(-1);

            {
                K key;
                uint count;

                if(ObjectManage<K,V>::GetKeyByValue(value,&key,&count,true))
                    return key;
            }

            if(!ObjectManage<K,V>::Add(id_count,value))
                return(-1);

            return id_count++;
        }
    };//template<typename K,typename V> class IDObjectManage:public ObjectManage<K,V>

    template<typename V> using U32ObjectManage=IDObjectManage<uint32,V>;
    template<typename V> using U64ObjectManage=IDObjectManage<uint64,V>;
}//namespace hgl
