#pragma once

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

    struct RefPoolStats
    {
         int active;        ///<有多少个key是活跃的
         int idle;          ///<有多少个key是闲置的
         int can_free;      ///<不在active/idle中，但可以从idle中释放的个数
         int not_found;     ///<active/idle中都不存在的key有多少个
    };

    /**
     * 附带引用计数的对象池列表
     */
    template<typename K,typename V> class RefObjectPool
    {
    protected:

        using ActiveItem=RefData<V>;

        ObjectMap<K,ActiveItem> active_items;               ///<活跃的数据
        Map<K,V> idle_items;                                ///<引用计数为0的

        /**
         * Helper function to check if a position is valid (>= 0)
         * @param pos Position value to check
         * @return true if position is valid (>= 0)
         */
        inline bool Found(int pos) const { return pos >= 0; }

    public:

        virtual ~RefObjectPool()=default;

        const int GetActiveCount()const{return active_items.GetCount();}        ///<取得活跃项数量
        const int GetIdleCount()const{return idle_items.GetCount();}            ///<取得闲置项数量

        /**
         * 根据key列表统计数据
         * @param ststs     统计结果
         * @param key_list  要扫描的key列表
         * @param key_count 要扫描的key数量
         */
        void Stats(RefPoolStats &stats,const K *key_list,const int key_count)
        {
            mem_zero(stats);

            const K *kp=key_list;
            for(int i=0;i<key_count;i++)
            {
                if(active_items.ContainsKey(*kp))
                    ++(stats.active);
                else
                if(idle_items.ContainsKey(*kp))
                    ++(stats.idle);
                else
                    ++(stats.not_found);

                ++kp;
            }

            stats.can_free=idle_items.GetCount()-(stats.idle);
        }

        /**
         * 确定指定key是否在活跃列表
         */
        bool ContainsActiveKey(const K &key)const{return active_items.ContainsKey(key);}

        /**
         * 确定指定key是否在闲置列表
         */
        bool ContainsIdleKey(const K &key)const{return idle_items.ContainsKey(key);}

        /**
         * 确定指定key是否在列表中(包括活跃列表和闲置列表)
         */
        bool ContainsKey(const K &key)const
        {
            if(active_items.ContainsKey(key))
                return(true);

            if(idle_items.ContainsKey(key))
                return(true);

            return(false);
        }

        /**
         * 获取指定key数据的引用计数
         * @param key 要查询的key
         * @return >0 在活跃列表中
         * @return =0 在闲置列表中
         * @return <0 不存在
         */
        int GetRefCount(const K &key)const
        {
            ActiveItem *ai;

            if(active_items.Get(key,ai))                    //在活跃列表中找
                return ai->ref_count;

            if(idle_items.ContainsKey(key))
                return 0;

            return -1;
        }

        /**
         * 根据key获取指定值(并增加引用引数)
         */
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

            // GetValueAndSerial returns the index position if found (>=0), or -1 if not found
            pos=idle_items.GetValueAndSerial(key,value);
            if(Found(pos))                   //在闲置列表中找 - Fixed: was pos>0, now pos>=0 to handle index 0
            {
                active_items.Add(key,new ActiveItem(value));
                idle_items.DeleteAt(pos);
                return(true);
            }

            return(false);
        }

        /**
         * 外部增加一个值
         */
        void Add(const K &key,V &value)
        {
            active_items.Add(key,new ActiveItem(value));
        }

        /**
         * 释放一个值的使用(并释放引用计数)
         */
        void Release(const K &key)
        {
            int pos;
            ActiveItem *ai;

            // GetValueAndSerial returns the index position if found (>=0), or -1 if not found
            pos=active_items.GetValueAndSerial(key,ai);

            if(Found(pos))  //Fixed: was pos>0, now pos>=0 to handle index 0
            {
                --ai->ref_count;

                if(ai->ref_count==0)
                {
                    idle_items.Add(key,ai->data);
                    active_items.DeleteAt(pos);
                }

                return;
            }
        }
    };//template<typename K,typename V> class RefObjectPool
}//namespace hgl
