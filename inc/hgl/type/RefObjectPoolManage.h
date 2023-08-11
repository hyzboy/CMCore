#pragma once

#include<hgl/type/Pool.h>
#include<hgl/type/ObjectManage.h>

namespace hgl
{
    template <typename K,typename V,typename OP> class _RefObjectPoolManage:public ObjectManage<K,V>
    {
    protected:

        OP *data_pool;

    public:

        virtual V *     Create  (const K &key)  override{return data_pool->Acquire();}
        virtual void    Clear   (V *obj)        override{data_pool->Release(obj);}

    public:

        _RefObjectPoolManage(OP *op):data_pool(op){}
        virtual ~_RefObjectPoolManage()=default;
    };//template <typename K,typename V,typename OP> class _RefObjectPoolManage:public ObjectManage<K,V>

    /**
     * 资源池是Pool/ResManage两个模板的组合应用
     */
    template <typename K,typename V> class RefObjectPoolManage:public _RefObjectPoolManage<K,V,ObjectPool<V> >
    {
    public:

        RefObjectPoolManage():_RefObjectPoolManage(new ObjectPool<V>){}
        virtual ~RefObjectPoolManage()
        {
            SAFE_CLEAR(this->data_pool);
        }
    };//template <typename K,typename V> class RefObjectPoolManage:public _RefObjectPoolManage<K,V,ObjectPool<V> >
}//namespace hgl
