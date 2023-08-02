
namespace hgl
{
    template<typename T> class _ObjectPool:public Pool<T *>                                         ///对象池
    {
    protected:

        virtual T *Create()=0;

        virtual void Clear(T *obj)  {   if(obj)delete obj;  }

    public:

        using Pool<T *>::Pool;
        virtual ~_ObjectPool(){Pool<T *>::ClearAll();}

        virtual bool Release(T *obj) override                                                       ///<释放一个数据
        {
            if(!obj)return(true);

            return Pool<T *>::Release(obj);
        }
    };//template<typename T> class _ObjectPool

    template<typename T> class ObjectPool:public _ObjectPool<T>                                     ///对象池
    {
        virtual T *Create()override{return(new T());}

    public:

        using _ObjectPool<T>::_ObjectPool;
        virtual ~ObjectPool(){_ObjectPool<T>::ClearAll();}
    };//template<typename T> class ObjectPool

}//namespace hgl
