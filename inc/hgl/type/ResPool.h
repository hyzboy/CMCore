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

        const int GetActiveCount()const{return active_items.GetCount();}        ///<取得活跃项数量
        const int GetIdleCount()const{return idle_items.GetCount();}            ///<取得闲置项数量

        /**
         * 根据key列表统计数据
         * @param key_list          要扫描的key列表
         * @param in_active_list    有多少个key是活跃的
         * @param in_idle_list      有多少个key是闲置的
         * @param out_list          active/idle中都不存在的key有多少个
         * @param idle_left_list    不在active/idle中，但可以从idle中释放的个数
         */
        void Stats(const List<K> &key_list,List<K> &in_active_list,List<K> &in_idle_list,List<K> &out_list,List<K> &idle_left_list)
        {
            in_active_list.ClearData();
            in_idle_list.ClearData();
            out_list.ClearData();
            idle_left_list.ClearData();
            
            const K *kp=key_list.GetData();
            for(int i=0;i<key_list.GetCount();i++)
            {
                if(active_items.KeyExist(*kp))
                    in_active_list.Add(*kp);
                else
                if(idle_items.KeyExist(*kp))
                    in_idle_list.Add(*kp);
                else
                    out_list.Add(*kp);

                ++kp;
            }

            idle_items.WithoutList(idle_left_list,in_idle_list);
        }

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
