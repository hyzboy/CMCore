#pragma once

#include<hgl/object/Object.h>
#include<hgl/thread/Atomic.h>

namespace hgl
{
    /**
    * 引用计数归零时的处理策略
    */
    enum class RefZeroPolicy:uint8
    {
        AutoDestroy=0,          ///<归零时调用Release()并delete自身(默认)
        KeepMetaUnloadPayload,  ///<归零时只调用Release()(由MetaObject实现为卸载负载)，对象壳由外部销毁
    };//enum class RefZeroPolicy

    class RefObject;

    /**
    * 对引用计数对象执行一次解引用
    * @return 归零并已按策略处理时返回true(此时指针可能已失效)
    */
    template<typename T> bool ReleaseRefObject(T *obj)
    {
        if(!obj)
            return(false);

        return(obj->RefDec()==0);
    }

    /**
    * 引用计数对象基类
    *
    * 在Object的身份/生命周期/日志之上，增加原子引用计数与归零策略：
    * - RefInc()/RefDec() 线程安全
    * - 归零时按 GetRefZeroPolicy() 决定销毁方式
    *
    * 注意：使用 AutoDestroy 策略时不得在栈上创建对象，必须配合 RefPtr<> 使用。
    */
    class RefObject:public Object
    {
    public:

        NO_COPY(RefObject)
        NO_MOVE(RefObject)

    private:

        atom_uint32     ref_count;                                  ///<原子引用计数
        RefZeroPolicy   ref_zero_policy;                            ///<归零策略

    protected:

        /**
        * 引用计数归零回调(默认按策略实现)
        * @note 使用 AutoDestroy 策略时本函数返回后对象可能已被销毁
        */
        virtual void OnRefZero();

    public:

        RefObject();
        virtual ~RefObject()=default;

    public:     // 引用计数

        const uint32 GetRefCount()const{return ref_count.load();}

        /**
        * 增加引用计数
        * @return 增加后的引用计数
        */
        const uint32 RefInc()
        {
            return(++ref_count);
        }

        /**
        * 减少引用计数，归零时触发 OnRefZero()
        * @return 减少后的引用计数
        */
        const uint32 RefDec()
        {
            const uint32 left=--ref_count;

            if(left==0)
                OnRefZero();

            return(left);
        }

    public:     // 归零策略

        void SetRefZeroPolicy(const RefZeroPolicy policy){ref_zero_policy=policy;}

        const RefZeroPolicy GetRefZeroPolicy()const{return(ref_zero_policy);}
    };//class RefObject

    /**
    * 引用计数对象的自动管理指针
    *
    * 构造时 RefInc()，析构时 RefDec()；支持拷贝与移动。
    */
    template<typename T> class RefPtr
    {
        T *ptr;

    public:

        RefPtr():ptr(nullptr){}

        explicit RefPtr(T *obj):ptr(obj)
        {
            if(ptr)
                ptr->RefInc();
        }

        RefPtr(const RefPtr<T> &other):ptr(other.ptr)
        {
            if(ptr)
                ptr->RefInc();
        }

        RefPtr(RefPtr<T> &&other):ptr(other.ptr)
        {
            other.ptr=nullptr;
        }

        ~RefPtr()
        {
            Reset();
        }

    public:

        /**
        * 放弃当前引用(不负责销毁指针本身)
        */
        void Reset()
        {
            T *obj=ptr;

            if(!obj)
                return;

            ptr=nullptr;

            ReleaseRefObject(obj);
        }

        /**
        * 接管新指针(先释放旧引用)
        */
        void Reset(T *obj)
        {
            if(obj==ptr)
                return;

            Reset();

            ptr=obj;

            if(ptr)
                ptr->RefInc();
        }

        /**
        * 交出指针所有权：不做 RefDec()，调用者需自行管理
        */
        T *Detach()
        {
            T *obj=ptr;

            ptr=nullptr;

            return(obj);
        }

    public:

        RefPtr<T> &operator=(const RefPtr<T> &other)
        {
            if(this!=&other)
                Reset(other.ptr);

            return(*this);
        }

        RefPtr<T> &operator=(RefPtr<T> &&other)
        {
            if(this!=&other)
            {
                Reset();

                ptr=other.ptr;
                other.ptr=nullptr;
            }

            return(*this);
        }

        RefPtr<T> &operator=(T *obj)
        {
            Reset(obj);

            return(*this);
        }

    public:

        T *Get()const{return(ptr);}

        T *operator->()const{return(ptr);}

        T &operator*()const{return(*ptr);}

        explicit operator bool()const{return(ptr!=nullptr);}

        const bool operator==(const T *obj)const{return(ptr==obj);}

        const bool operator!=(const T *obj)const{return(ptr!=obj);}
    };//template<typename T> class RefPtr
}//namespace hgl
