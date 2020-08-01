#ifndef HGL_RES_POOL_INCLUDE
#define HGL_RES_POOL_INCLUDE

#include<hgl/type/Map.h>
namespace hgl
{
    template<typename T> struct RefData
    {
        int ref_count;
        T data;

    public:

        RefData(const T &value)
        {
            ref_count=1;
            data=value;
        }
    };//template<typename T> struct RefData

    template<typename K,typename V> class ResPool
    {
    protected:

        using ActiveItem=RefData<V>;

        MapObject<K,ActiveItem> active_items;               ///<活跃的数据
        Map<K,V> idle_items;                                ///<引用计数为0的

    public:

        virtual ~ResPool()=default;

        bool KeyExist(const K &key)
        {
            if(active_items.KeyExist(key))
                return(true);

            if(idle_items.KeyExist(key))
                return(true);

            return(false);
        }

        bool Get(const K &key,V &value)
        {
            int pos;
            ActiveItem *ai;
                
            if(active_items.Get(key,ai))                    //在活跃列表中找
            {
                ++ai->ref_count;
                value=ai->data;
                return(true);
            }

            pos=idle_items.GetValueAndSerial(key,value);
            if(pos>0)                   //在限制列表中找
            {
                active_items.Add(key,new ActiveItem(value));
                idle_items.DeleteBySerial(pos);
                return(true);
            }

            return(false);
        }

        void Add(const K &key,V &value)
        {
            active_items.Add(key,new ActiveItem(value));
        }

        void Release(const K &key)
        {
            int pos;
            ActiveItem *ai;

            pos=active_items.GetValueAndSerial(key,ai);

            if(pos>0)
            {
                --ai->ref_count;

                if(ai->ref_count==0)
                {
                    idle_items.Add(key,ai->data);
                    active_items.DeleteBySerial(pos);
                }

                return;
            }
        }
    };//template<typename K,typename V> class ResPool
}//namespace hgl
#endif//HGL_RES_POOL_INCLUDE
