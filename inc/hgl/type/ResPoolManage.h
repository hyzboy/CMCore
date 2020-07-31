#ifndef HGL_RES_POOL_MANAGE_INCLUDE
#define HGL_RES_POOL_MANAGE_INCLUDE

#include<hgl/type/Pool.h>
#include<hgl/type/ResManage.h>

namespace hgl
{
    template <typename K,typename V,typename OP> class _ResPoolManage:public ResManage<K,V>
    {
    protected:

        OP *data_pool;

    public:

        virtual V *     Create  (const K &key)  override{return data_pool->Acquire();}
        virtual void    Clear   (V *obj)        override{data_pool->Release(obj);}

    public:

        _ResPoolManage(OP *op):data_pool(op){}
        virtual ~_ResPoolManage()=default;
    };//template <typename K,typename V,typename OP> class _ResPoolManage:public ResManage<K,V>

    /**
     * 资源池是Pool/ResManage两个模板的组合应用
     */
    template <typename K,typename V> class ResPoolManage:public _ResPoolManage<K,V,ObjectPool<V> >
    {
    public:

        ResPoolManage():_ResPoolManage(new ObjectPool<V>){}
        virtual ~ResPoolManage()
        {
            SAFE_CLEAR(this->data_pool);
        }
    };//template <typename K,typename V> class ResPoolManage:public _ResPoolManage<K,V,ObjectPool<V> >
}//namespace hgl
#endif//HGL_RES_POOL_MANAGE_INCLUDE
