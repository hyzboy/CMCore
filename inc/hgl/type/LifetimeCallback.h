#pragma once

#include<hgl/TypeFunc.h>
namespace hgl
{
    /**
    * 原生数据生命周期处理回调
    */
    template<typename T> struct RawLifetimeCallback
    {
        virtual bool Create(T *){return true;}
        virtual void Clear(T *){return;}

        virtual void Copy(T *dst,T *src,int count)
        {
            hgl_cpy(dst,src,count);
        }

        virtual void Init(T *dst,int count)
        {
            hgl_zero(dst,count);
        }

    public: //活跃事件

        virtual bool OnCreate   (T *){return true;}         ///<被创建
        virtual bool OnActive   (T *){return true;}         ///<切换到激活
        virtual bool OnIdle     (T *){return true;}         ///<切换到空闲
        virtual bool OnReuse    (T *){return true;}         ///<被重用(空间被重用，旧数据需要清除)
        virtual bool OnRelease  (T *){return true;}         ///<被释放
    };

    /**
    * 对像生命周期处理回调 
    */
    template<typename T> struct ObjectLifetimeCallback:public RawLifetimeCallback<T *>
    {
        virtual bool Create(T **) override=0;
        virtual void Clear(T **) override=0;
    };//struct ObjectLifetimeCallback

    /**
    * 缺省对像生命周期处理回调 
    */
    template<typename T> struct DefaultObjectLifetimeCallback:public ObjectLifetimeCallback<T>
    {
        virtual bool Create(T **obj) override {*obj=new T;return(true);}
        virtual void Clear(T **obj) override {if(obj&&*obj)delete *obj;}
    };//struct DefaultObjectLifetimeCallback
}//namespace hgl
