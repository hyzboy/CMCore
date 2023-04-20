#ifndef HGL_RES_MANAGE_INCLUDE
#define HGL_RES_MANAGE_INCLUDE

#include<hgl/type/Map.h>
namespace hgl
{
    template<typename K,typename V> struct RefKeyValue:public KeyValue<K,V *>       ///<带引用计数的Key/value数据结构
    {
        int ref_count;              ///<引用计数

    public:

        RefKeyValue():KeyValue<K,V *>()
        {
            ref_count=1;
        }
    };

    /**
    * 资源管理器,它没有缓冲管理，仅仅是管理数据，并保证不会被重复加载
    */
    template<typename K,typename V> class ResManage
    {
    protected:

        using ResItem=RefKeyValue<K,V>;

        _Map<K,V *,ResItem> items;

        int ReleaseBySerial(int,bool);

    protected:

        virtual void Clear(V *obj){delete obj;}                                 ///<资源释放虚拟函数(缺省为直接delete对象)

    public:

        virtual ~ResManage();

        virtual void        Clear();                                            ///<清除所有数据
        virtual void        ClearFree();                                        ///<清除所有引用计数为0的数据

                const int   GetCount()const{return items.GetCount();}           ///<取得数据数量

        virtual bool        Add(const K &,V *);                                 ///<添加一个数据
        virtual V *         Find(const K &);                                    ///<查找一个数据(不增加引用计数)
        virtual V *         Get(const K &);                                     ///<取得一个数据(增加引用计数)

        virtual bool        ValueExist(V *);                                    ///<确认这个数据是否存在
        virtual bool        GetKeyByValue(V *,K *,uint *,bool inc_ref=false);   ///<取得一个数据的Key和引用次数

        virtual int         Release(const K &,bool zero_clear=false);           ///<释放一个数据
        virtual int         Release(V *,bool zero_clear=false);                 ///<释放一个数据
    };//template<typename F,typename T> class ResManage

    /**
     * 使用int类做数标致的资源管理器
     */
    template<typename K,typename V> class IDResManage:public ResManage<K,V>
    {
        K id_count=0;

    public:

        using ResManage<K,V>::ResManage;
        virtual ~IDResManage()=default;

        virtual K Add(V *value)
        {
            if(!value)return(-1);

            {
                K key;
                uint count;

                if(ResManage<K,V>::GetKeyByValue(value,&key,&count,true))
                    return key;
            }

            if(!ResManage<K,V>::Add(id_count,value))
                return(-1);

            return id_count++;
        }
    };//template<typename F,typename T> class IDResManage:public ResManage<F,T>
}//namespace hgl
#include<hgl/type/ResManage.cpp>
#endif//HGL_RES_MANAGE_INCLUDE
