#pragma once
#include<hgl/type/DataType.h>
#include<hgl/type/object/ObjectBaseInfo.h>

namespace hgl
{
    /**
     * 基础对象
     */
    class Object
    {
        ObjectBaseInfo object_info;

    public:

        const ObjectBaseInfo &  GetObjectBaseInfo   () const noexcept { return object_info; }                          ///<获取对象简单信息

        const size_t            GetTypeHash         () const noexcept { return object_info.hash_code; }                ///<获取对象数据类型的hash值
        const size_t            GetUniqueID         () const noexcept { return object_info.unique_id; }                ///<获取对象的唯一序列号

    public:

//        template<typename T> friend class SafePtr;
//        template<typename T> friend struct DefaultObjectAllocator;

        NO_COPY(Object)
        NO_MOVE(Object)

        Object(const ObjectBaseInfo &oi) noexcept { object_info=oi; }
        virtual ~Object()=default;

        //virtual bool Initailize()=0;
        //virtual void Deinitailize()=0;
    };//class Object

    template<typename T> inline T *New(const SourceCodeLocation &scl)
    {
        static size_t new_count=0;

        ObjectBaseInfo obi;

        obi.hash_code=GetTypeHash<T>();
        obi.unique_id=new_count;
        obi.scl=scl;

        ++new_count;

        T *obj=new T(obi);

        return obj;
    }

    #define NewObject(T,obj) T *obj=New<T>(HGL_SCL_HERE);

    //template<typename T> struct SafePtrData
    //{
    //    T *ptr;
    //    int count;

    //private:

    //    SafePtrData(T *p)
    //    {
    //        ptr=p;
    //        count=0;
    //    }

    //    ~SafePtrData()=default;

    //    template<typename T> friend class DefaultObjectManager;
    //};

    ///**
    // * 安全访问指针<Br>
    // * 其本质类似于的WeakPtr，但是不同的是:
    // * <ul>
    // *      <li>SafePtr不使用atom计数器，所以它不是线程安全的</li>
    // *      <li>SafePtr不会自动释放指针，它是在访问时检查指针是否有效，如果无效则返回nullptr</li>
    // * </ul>
    // */
    //template<typename T> class SafePtr
    //{
    //    SafePtrData<T> *data;

    //public:

    //    SafePtr()
    //    {
    //        data=nullptr;
    //    }

    //    SafePtr(SafePtrData<T> *spd)
    //    {
    //        data=spd;

    //        if(data)
    //            ++data->count;
    //    }

    //public:

    //    virtual ~SafePtr()
    //    {
    //        Release();
    //    }

    //            T *Get()        {return data?data->ptr:nullptr;}
    //    const   T *Get() const  {return data?data->ptr:nullptr;}

    //    T *operator->() { return  Get(); }
    //    T &operator* () { return *Get(); }

    //    const T *operator->() const { return Get(); }

    //    const bool operator==(const T *ptr) const noexcept { return Get()==ptr; }
    //    const bool operator!=(const T *ptr) const noexcept { return Get()!=ptr; }

    //    const bool operator==(const SafePtr<T> &sp) const { return Get()==sp.Get(); }
    //    const bool operator!=(const SafePtr<T> &sp) const { return Get()!=sp.Get(); }

    //    const bool IsValid() const noexcept                                                         ///<当前对象指针是否有效
    //    {
    //        return data&&data->ptr;
    //    }

    //    SafePtr<T> &operator=(SafePtr<T> &sp)
    //    {
    //        if(!sp.IsValid())
    //        {
    //            Release();
    //            return *this;
    //        }

    //        if(data)
    //        {
    //            if(data->ptr==sp.data->ptr)
    //                return *this;

    //            Release();
    //        }

    //        data=sp.data;
    //        ++data->count;

    //        return *this;
    //    }

    //    template<typename OT>
    //    SafePtr<T> &operator=(SafePtr<OT> &spd)
    //    {
    //        if(T::StaticTypeHash()!=OT::StaticTypeHash())
    //        {
    //            Release();
    //            return *this;
    //        }

    //        if(data!=spd.data)
    //        {
    //            Release();
    //            data=spd.data;
    //            ++data->count;
    //        }

    //        return *this;
    //    }

    //    SafePtr<T> &operator=(Object *obj)=delete;

    //    /**
    //     * 强制释放对象(不管所有权问题，强制释放)
    //     */
    //    void Destory()
    //    {
    //        if(!data)
    //            return;

    //        if(!data->ptr)
    //            return;

    //        ObjectManager *om=data->ptr->GetObjectManager();

    //        if(!om)
    //        {
    //            //std::cerr<<"SafePtr<"<<GetTypeName<T>()<<">::Destory() error, manager is null."<<std::endl;
    //            return;
    //        }
    //    
    //        //std::cout<<"SafePtr<"<<GetTypeName<T>()<<">::Destory() serial:"<<data->ptr->GetSerialNumber()<<std::endl;

    //        DefaultObjectManager<T> *dom=static_cast<DefaultObjectManager<T> *>(om);

    //        dom->ReleaseObject(data);

    //        data=nullptr;
    //    }

    //    /**
    //     * 释放对象(释放所有权，不代表会被释放。当所有权计数为0时会被释放)
    //     * 
    //     * \return 依然持有对象的数量
    //     */
    //    int Release()
    //    {
    //        if(!data)
    //            return -1;

    //        //if(data->ptr)
    //        //{
    //        //    std::cout<<"SafePtr<"<<GetTypeName<T>()<<">::Release() serial:"<<data->ptr->GetSerialNumber()<<std::endl;
    //        //}

    //        int result;

    //        if(data->count==1)
    //        {
    //            Destory();
    //            result=0;
    //        }
    //        else
    //        {
    //            --data->count;

    //            result=data->count;
    //        }

    //        data=nullptr;
    //        return result;
    //    }
    //};//template<typename T> class SafePtr
}//namespace hgl

