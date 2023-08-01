#pragma once

#include<hgl/TypeFunc.h>
namespace hgl
{
    /**
    * ԭ�������������ڴ���ص�
    */
    template<typename T> struct RawLifetimeCallback
    {
        virtual bool Create(T *){return true;}
        virtual void Clear(T *){return;}

        virtual void Copy(T *dst,T *src,int count)
        {
            hgl_cpy(dst,src,count);
        }
    };

    /**
    * �����������ڴ���ص� 
    */
    template<typename T> struct ObjectLifetimeCallback:public RawLifetimeCallback<T *>
    {
        virtual bool Create(T **) override=0;
        virtual void Clear(T **) override=0;
    };//struct ObjectLifetimeCallback

    /**
    * ȱʡ�����������ڴ���ص� 
    */
    template<typename T> struct DefaultObjectLifetimeCallback:public ObjectLifetimeCallback<T>
    {
        virtual bool Create(T **obj) override {*obj=new T;return(true);}
        virtual void Clear(T **obj) override {if(obj&&*obj)delete *obj;}
    };//struct DefaultObjectLifetimeCallback
}//namespace hgl
