#pragma once

#include<string.h>

namespace hgl
{
    /**
    * 比较处理模板基类<br>
    * 注：从此类派生的，不可以使用memset(this,0,sizeof())，因为这会破坏Comparator<>纯虚函数表
    */
    template<typename T> class Comparator                                                               ///比较处理模板基类
    {
    public:

        Comparator()=default;
        virtual ~Comparator()=default;

        /**
        * 比较函数，需被特例化或派生实现. 正确情况下此函数不应该会被调用
        */
        virtual const int compare(const T &other)const=0;

        const bool operator > (const T &i)const{return compare(i)>0;}
        const bool operator < (const T &i)const{return compare(i)<0;}
        const bool operator >=(const T &i)const{return compare(i)>=0;}
        const bool operator <=(const T &i)const{return compare(i)<=0;}
        const bool operator ==(const T &i)const{return compare(i)==0;}
        const bool operator !=(const T &i)const{return compare(i)!=0;}
    };//class Comparator

    template<typename T> class ComparatorData:public Comparator<T>
    {   
    public:

        ComparatorData()=default;
        virtual ~ComparatorData()=default;

        const int compare(const T &other)const override final
        {
            return memcmp(this,&other,sizeof(T));
        }
    };//class ComparatorData

    template<typename T> class ItemComparator
    {
    public:

        ItemComparator()=default;
        virtual ~ItemComparator()=default;

        static const int compare(const T &a,const T &b);

        static void cpy(T *dst,const T *src)
        {
            memcpy(dst,src,sizeof(T));
        }

        static void exchange(T &a,T &b)
        {
            T temp;
            cpy(&temp,&a);
            cpy(&a,&b);
            cpy(&b,&temp);
        }
    };
}//namespace hgl
