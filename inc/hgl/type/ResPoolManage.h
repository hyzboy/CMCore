#ifndef HGL_RES_POOL_MANAGE_INCLUDE
#define HGL_RES_POOL_MANAGE_INCLUDE

#include<hgl/type/Pool.h>
#include<hgl/type/ResManage.h>

namespace hgl
{
    /**
     * 资源池是Pool/ResManage两个模板的组合应用
     */
    template <typename K,typename V> class ResPoolManage:public ResManage<K,V>
    {
    protected:

        ObjectPool<V> data_pool;

    public:

        virtual V *Create(const K &key){return data_pool.Acquire();}
        virtual void Clear(V *obj){data_pool.Release(obj);}

    public:

        virtual ~ResPoolManage()=default;

        virtual void Clear()
        {
            this->items.Clear();
            data_pool.ClearAll();
        }
    };//template <typename K,typename V> class ResPoolManage<K,V>:public ResManage<K,V>
}//namespace hgl
#endif//HGL_RES_POOL_MANAGE_INCLUDE
