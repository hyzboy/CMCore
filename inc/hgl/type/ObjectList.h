#pragma once

#include<hgl/type/ArrayList.h>
#include<hgl/type/LifecycleManager.h>

namespace hgl
{
    /**
     * 对象列表：拥有并在Clear/Free时delete元素指针。
     */
    template<typename T> class ObjectList:public ArrayList<T *>
    {
    public:
        using ItemPointer=T *;

        ObjectList()=default;
        virtual ~ObjectList(){Free();}

        ItemPointer operator[](int idx) const { return this->data_array.GetData()[idx]; }

        bool Insert(int index,const ItemPointer &obj)  { return ArrayList<T *>::Insert(index,obj); }

        virtual void Free()    { Clear(); ArrayList<T *>::Free(); }
        virtual void Clear()    { T **ptr=this->data_array.GetData(); int n=this->data_array.GetCount(); LifecycleTraitsOwningPtr<T>::destroy(ptr,n); ArrayList<T *>::Clear(); }
        virtual bool Contains(const ItemPointer &flag)const    { return ArrayList<T *>::Find((T *)flag)!=-1; }

        virtual bool Unlink(int index){return ArrayList<T *>::Delete(index);}              ///< 断开索引
        virtual bool UnlinkMove(int index){return ArrayList<T *>::DeleteShift(index);}     ///< 断开并前移
        virtual bool Unlink(int start,int number){return ArrayList<T *>::Delete(start,number);} 
        virtual bool UnlinkByValue(ItemPointer &ip){return ArrayList<T *>::DeleteByValue(ip);}   
        virtual void UnlinkByValue(ItemPointer *ip,int n){ArrayList<T *>::DeleteByValue(ip,n);}  
        virtual void UnlinkAll(){ArrayList<T *>::Clear();}

    private:
        bool _DeleteRange(int index,int num)
        {
            if(index<0||num<=0||index+num>ArrayList<T *>::GetCount())
                return false;
            T **ptr=this->data_array.GetData()+index;
            LifecycleTraitsOwningPtr<T>::destroy(ptr,num);
            return true;
        }

    public:
        // 不使用 override 以避免与 ArrayList 原 Delete/ DeleteByValue 签名冲突
        bool DeleteAtOwn(int index)
        {
            if(!_DeleteRange(index,1)) return false;
            return ArrayList<T *>::Delete(index);
        } 

        bool DeleteRangeOwn(int index,int number)
        {
            if(!_DeleteRange(index,number))return false;
            return ArrayList<T *>::Delete(index,number);
        } 

        bool DeleteByValueOwn(ItemPointer &ip) 
        {
            int idx=ArrayList<T *>::Find(ip);
            if(idx==-1)return false;
            _DeleteRange(idx,1);
            return ArrayList<T *>::Delete(idx);
        } 

        void DeleteByValueOwn(ItemPointer *ip,int n) 
        {
            if(!ip||n<=0)return;
            for(int i=0;i<n;i++) DeleteByValueOwn(ip[i]);
        } 
    };//class ObjectList
}//namespace hgl
