#pragma once

#include<hgl/TypeFunc.h>
namespace hgl
{
    /**
    * 原生数据生命周期处理回调
    */
    template<typename T> struct DataLifecycleManager
    {
        virtual bool Create(T *){return true;}
        virtual void Clear(T *){};
        virtual void Clear(T *dst,int count){};

        virtual void Copy(T *dst,T *src,int count)
        {
            hgl_cpy(dst,src,count);
        }

        virtual void Init(T *dst,int count)
        {
            hgl_zero(dst,count);
        }

    public: //活跃事件

        virtual bool OnActive   (T *,int count=1){return true;}         ///<切换到激活
        virtual bool OnIdle     (T *,int count=1){return true;}         ///<切换到空闲
    };

    /**
    * 对像生命周期处理回调 
    */
    template<typename T> struct ObjectLifecycleManager:public DataLifecycleManager<T *>
    {
        virtual bool Create(T **) override {return false;}
        virtual void Clear(T **obj) override {if(obj&&*obj)delete *obj;}

        virtual void Clear(T **obj,int count) override
        {
            if(!obj||count<=0)return;

            for(int i=0;i<count;i++)
            {
                if(*obj)
                    delete *obj;

                *obj=nullptr;
                ++obj;
            }
        }
    };//struct ObjectLifecycleManager

    /**
    * 缺省对像生命周期处理回调 
    */
    template<typename T> struct DefaultObjectLifetimeCallback:public ObjectLifecycleManager<T>
    {
        virtual bool Create(T **obj) override {*obj=new T;return(true);}
    };//struct DefaultObjectLifetimeCallback
}//namespace hgl
