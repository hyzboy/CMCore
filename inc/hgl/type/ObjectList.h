#pragma once

#include<hgl/type/ArrayList.h>
#include<hgl/type/LifecycleTraits.h>

namespace hgl
{
    /**
     * 对象列表：
     * 本容器保存指针类型并对这些指针拥有所有权（Owning Ptr）。
     * 当调用 Clear()/Free()/Delete 系列函数时，会对所包含的元素执行 delete 操作
     * （通过 LifecycleTraitsOwningPtr<T>::destroy 统一处理）。
     *
     * 设计要点：
     * - 继承自 ArrayList<T *>，保留其大部分接口；
     * - 新增以 Own 命名的方法（如 DeleteAtOwn），用于先释放对象再从列表中移除；
     * - 提供 Unlink 系列方法仅断开关联（不 delete），以支持不同的内存管理策略；
     */
    template<typename T> class ObjectList : public ArrayList<T *>
    {
    public:
        using ItemPointer = T *;

        ObjectList() = default;
        virtual ~ObjectList() { Free(); }

        // 访问运算符，返回指针
        ItemPointer operator[](int idx) const { return this->data_array.GetData()[idx]; }

        // 插入元素（保留原 ArrayList 行为）
        bool Insert(int index, const ItemPointer &obj) { return ArrayList<T *>::Insert(index, obj); }

        // 释放与清理
        // Free: 先 Clear（对元素 delete），再调用基类 Free
        virtual void Free() { Clear(); ArrayList<T *>::Free(); }

        // Clear: 对内部所有指针执行销毁（delete），然后清空数组
        virtual void Clear()
        {
            T **ptr = this->data_array.GetData();
            int n = this->data_array.GetCount();
            LifecycleTraitsOwningPtr<T>::destroy(ptr, n);
            ArrayList<T *>::Clear();
        }

        // 判断是否包含指定指针（按值比较）
        virtual bool Contains(const ItemPointer &flag) const { return ArrayList<T *>::Find((T *)flag) != -1; }

        // Unlink 系列：仅断开关联（不 delete），等同于基类的 Delete/ Clear 行为
        virtual bool Unlink(int index)                    { return ArrayList<T *>::Delete(index); }          ///< 断开指定索引
        virtual bool UnlinkMove(int index)                { return ArrayList<T *>::DeleteShift(index); }     ///< 断开并前移其后的元素
        virtual bool Unlink(int start, int number)        { return ArrayList<T *>::Delete(start, number); }  ///< 断开一段范围
        virtual bool UnlinkByValue(ItemPointer &ip)       { return ArrayList<T *>::DeleteByValue(ip); }      ///< 通过值断开（单个）
        virtual void UnlinkByValue(ItemPointer *ip, int n){ ArrayList<T *>::DeleteByValue(ip, n); }          ///< 通过数组断开
        virtual void UnlinkAll()                          { ArrayList<T *>::Clear(); }                       ///< 断开所有（不 delete）

    private:
        // 内部辅助：删除范围的元素并先行销毁这些指针
        bool _DeleteRange(int index, int num)
        {
            if (index < 0 || num <= 0 || index + num > ArrayList<T *>::GetCount())
                return false;

            T **ptr = this->data_array.GetData() + index;
            LifecycleTraitsOwningPtr<T>::destroy(ptr, num);

            return true;
        }

    public:
        // 以下以 Own 结尾的方法会先 delete 对象（如果存在），再从列表中移除对应条目。
        // 不使用 override 是为避免与 ArrayList 中 Delete/DeleteByValue 的签名冲突。

        // 删除指定索引并销毁该对象
        bool DeleteAtOwn(int index)
        {
            if (!_DeleteRange(index, 1))
                return false;

            return ArrayList<T *>::Delete(index);
        }

        // 删除一段范围并销毁这些对象
        bool DeleteRangeOwn(int index, int number)
        {
            if (!_DeleteRange(index, number))
                return false;

            return ArrayList<T *>::Delete(index, number);
        }

        // 通过值查找并销毁，然后从列表移除（单个）
        bool DeleteByValueOwn(ItemPointer &ip)
        {
            int idx = ArrayList<T *>::Find(ip);
            if (idx == -1) return false;

            _DeleteRange(idx, 1);
            return ArrayList<T *>::Delete(idx);
        }

        // 通过数组的值逐个销毁并移除
        void DeleteByValueOwn(ItemPointer *ip, int n)
        {
            if (!ip || n <= 0) return;

            for (int i = 0; i < n; ++i)
                DeleteByValueOwn(ip[i]);
        }
    };// class ObjectList
}// namespace hgl
