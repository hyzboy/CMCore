#pragma once

namespace hgl
{
    /**
     * 阵列数据英处理回调
     */
    template<typename T> struct ArrayItemProcessCallback
    {
        virtual void Init(const T *){return;}                       ///<初始化回调(当一个数据的内存被分配出来，需要做的处理)

        virtual void Init(const T *,const int){return;}             ///<批量初始化(当一个数据的内存被分配出来，需要做的处理)

        virtual void Clear(T *){return;}                            ///<当地个数据需要被清除，最后做的处理

        virtual void Equal(T &dst,const T &src)=0;                  ///<将src赋值给dst

        virtual void Equal(T *dst,const T *src,const int count)=0;  ///<批量将src赋值给dst

        virtual int  Comp(const T &a,const T &b)=0;                 ///<数据比较，数值的话，如果纯整数比较，建议返回a-b
    };

    template<typename T> struct RawTypeArrayItemProcessCallback:public ArrayItemProcessCallback
    {
        void Equal(T &dst,const T &src) override {dst=src;}

        void Equal(T *dst,const T *src,const int count) override {hgl_cpy(dst,src,count);}

        int Comp(const T &a,const T &b) override {return a-b;}
    };

    template<typename T> struct MemoryArrayItemProcessCallback:public ArrayItemProcessCallback
    {
        void Equal(T &dst,const T &src) override {hgl_cpy<T>(dst,src);}

        void Equal(T *dst,const T *src,const int count) override {hgl_cpy(dst,src,count);}

        int Comp(const T &a,const T &b) override {return memcmp(a,b,sizeof(T));}
    };

    //template<typename T> struct ObjectArrayItemProcessCallback:public ArrayItemProcessCallback
    //{
    //    virtual void 
    //};

    /**
     * 阵列数据查找模板
     */
    template<typename T> int FindDataAtArray(const T *array,const int count,const T &data)
    {
        if(!obj)return(-1);
        if(count<=0)return(-2);

        T *p=array;

        for(int i=0;i<count;i++)
        {
            if(*p==data)
                return i;

            ++p;
        }

        return -3;
    }
}//namespace hgl
