#ifndef HGL_RES_POOL_MANAGE_INCLUDE
#define HGL_RES_POOL_MANAGE_INCLUDE

#include<hgl/type/Pool.h>
#include<hgl/type/ResManage.h>

namespace hgl
{
    template <typename K,typename V,typename OP> class _ResPoolManage:public ResManage<K,V>
    {
    protected:

        OP data_pool;

    public:

        virtual V *     Create  (const K &key)  override{return data_pool.Acquire();}
        virtual void    Clear   (V *obj)        override{data_pool.Release(obj);}

    public:

        using ResManage<K,V>::ResManage;
        virtual ~_ResPoolManage()=default;
    };//template <typename K,typename V,typename OP> class _ResPoolManage<K,V>:public ResManage<K,V>

    /**
     * 资源池是Pool/ResManage两个模板的组合应用
     */
    template <typename K,typename V> class ResPoolManage:public _ResPoolManage<K,V,ObjectPool<V> >
    {
    public:

        using _ResPoolManage<K,V,ObjectPool<V> >::_ResPoolManage;
        virtual ~ResPoolManage()=default;
    };//template <typename K,typename V> class ResPoolManage<K,V>:public ResManage<K,V>
}//namespace hgl
#endif//HGL_RES_POOL_MANAGE_INCLUDE
