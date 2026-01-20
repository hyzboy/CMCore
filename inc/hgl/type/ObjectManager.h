/**
* @file ObjectManager.h
* @brief CN:对象管理模板类，支持引用计数和ID分配。\nEN:Object management template class, supports reference counting and ID allocation.
*/
#pragma once

#include <hgl/type/Map.h>

namespace hgl
{
    /**
    * @brief CN:带引用计数的键值对结构。\nEN:Key-value pair structure with reference counting.
    */
    template<typename K, typename V>
    struct RefCountedKeyValue : public KeyValue<K, V *>
    {
        /**
        * @brief CN:引用计数。\nEN:Reference count.
        */
        int ref_count;

        RefCountedKeyValue()
            : KeyValue<K, V *>(), ref_count(1)
        {
        }
    };

    /**
    * @brief CN:对象管理实现模板类。\nEN:Object manager implementation template class.
    * @tparam K CN:键类型。EN:Key type.
    * @tparam V CN:值类型。EN:Value type.
    */
    template<typename K, typename V>
    class ObjectManagerImpl
    {
    public:

        /**
        * @brief CN:键值对象类型。\nEN:Key-value object type.
        */
        using KVObject = RefCountedKeyValue<K, V>;

    protected:

        /**
        * @brief CN:对象映射表。\nEN:Object map table.
        */
        MapTemplate<K, V *, KVObject> items;

    public:

        ObjectManagerImpl() = default;

        virtual ~ObjectManagerImpl()
        {
            Clear();
        }

        /**
        * @brief CN:清空所有对象并释放资源。\nEN:Clear all objects and release resources.
        */
        virtual void Clear()
        {
            int n = items.GetCount();

            while (n--)
            {
                KVObject *obj = items.GetItem(n);

                if (obj && obj->value)
                {
                    delete obj->value;
                }
            }

            items.Clear();
        }

        /**
        * @brief CN:清除引用计数为0的对象。\nEN:Clear objects with zero reference count.
        */
        virtual void ClearFree()
        {
            int n = items.GetCount();

            while (n--)
            {
                KVObject *obj = items.GetItem(n);

                if (obj->ref_count <= 0)
                {
                    if (obj->value)
                    {
                        delete obj->value;
                    }

                    items.DeleteAt(n);
                }
            }
        }

        /**
        * @brief CN:获取对象数量。\nEN:Get object count.
        * @return CN:对象数量。EN:Number of objects.
        */
        const int GetCount() const
        {
            return items.GetCount();
        }

        /**
        * @brief CN:添加对象。\nEN:Add object.
        * @param key CN:键。EN:Key.
        * @param obj CN:对象指针。EN:Object pointer.
        * @return CN:添加成功返回true。EN:Return true if added successfully.
        */
        virtual bool Add(const K &key, V *obj)
        {
            if (!obj)
            {
                return false;
            }

            if (items.ContainsKey(key))
            {
                return false;
            }

            items.Add(key, obj);
            return true;
        }

        /**
        * @brief CN:查找对象。\nEN:Find object.
        * @param key CN:键。EN:Key.
        * @return CN:对象指针或nullptr。EN:Object pointer or nullptr.
        */
        virtual V *Find(const K &key)const
        {
            int index = items.Find(key);

            if (index == -1)
            {
                return nullptr;
            }

            KVObject *obj = items.GetItem(index);
            return obj->value;
        }

        /**
        * @brief CN:获取对象并增加引用计数。\nEN:Get object and increase reference count.
        * @param key CN:键。EN:Key.
        * @return CN:对象指针或nullptr。EN:Object pointer or nullptr.
        */
        virtual V *Get(const K &key)
        {
            int index = items.Find(key);

            if (index != -1)
            {
                KVObject *obj = items.GetItem(index);
                ++obj->ref_count;
                return obj->value;
            }

            return nullptr;
        }

        /**
        * @brief CN:通过对象指针获取键和引用计数。\nEN:Get key and reference count by object pointer.
        * @param value CN:对象指针。EN:Object pointer.
        * @param key CN:返回键。EN:Return key.
        * @param count CN:返回引用计数。EN:Return reference count.
        * @param add_ref CN:是否增加引用计数。EN:Whether to increase reference count.
        * @return CN:成功返回true。EN:Return true if found.
        */
        virtual bool GetKeyByValue(V *value, K *key = nullptr, uint *count = nullptr, bool add_ref = false)
        {
            int idx = items.FindByValue(value);

            if (idx == -1)
            {
                return false;
            }

            KVObject *obj = items.GetItem(idx);

            if (key)
            {
                *key = obj->key;
            }

            if (count)
            {
                *count = obj->ref_count;
            }

            if (add_ref)
            {
                ++obj->ref_count;
            }

            return true;
        }

        /**
        * @brief CN:通过序号释放对象。\nEN:Release object by index.
        * @param index CN:对象序号。EN:Object index.
        * @param zero_clear CN:为true时引用计数为0则删除对象。EN:If true, delete object when ref_count is zero.
        * @return CN:剩余引用计数。EN:Remaining reference count.
        */
        virtual int ReleaseBySerial(int index, bool zero_clear = false)
        {
            if (index < 0 || index >= items.GetCount())
            {
                return -1;
            }

            KVObject *obj = items.GetItem(index);
            --obj->ref_count;

            if (obj->ref_count <= 0)
            {
                if (zero_clear && obj->value)
                {
                    delete obj->value;
                }

                if (zero_clear)
                {
                    items.DeleteAt(index);
                }
            }

            return obj->ref_count;
        }

        /**
        * @brief CN:通过键释放对象。\nEN:Release object by key.
        * @param key CN:对象键。EN:Object key.
        * @param zero_clear CN:为true时引用计数为0则删除对象。EN:If true, delete object when ref_count is zero.
        * @return CN:剩余引用计数。EN:Remaining reference count.
        */
        virtual int Release(const K &key, bool zero_clear = false)
        {
            return ReleaseBySerial(items.Find(key), zero_clear);
        }

        /**
        * @brief CN:通过对象指针释放对象。\nEN:Release object by pointer.
        * @param td CN:对象指针。EN:Object pointer.
        * @param zero_clear CN:为true时引用计数为0则删除对象。EN:If true, delete object when ref_count is zero.
        * @return CN:剩余引用计数。EN:Remaining reference count.
        */
        int Release(V *td, bool zero_clear = false)
        {
            return ReleaseBySerial(items.FindByValue(td), zero_clear);
        }
    };

    /**
    * @brief CN:通用对象管理器。\nEN:General purpose object manager.
    * @tparam K CN:键类型。EN:Key type.
    * @tparam V CN:值类型。EN:Value type.
    */
    template<typename K, typename V>
    class ObjectManager : public ObjectManagerImpl<K, V>
    {
    public:

        ObjectManager() = default;

        virtual ~ObjectManager() = default;

    };

    /**
    * @brief CN:带自动ID分配的对象管理器。\nEN:Object manager with automatic ID allocation.
    * @tparam K CN:ID类型。EN:ID type.
    * @tparam V CN:值类型。EN:Value type.
    */
    template<typename K, typename V>
    class AutoIdObjectManager : public ObjectManager<K, V>
    {
    protected:

        /**
        * @brief CN:当前ID计数。\nEN:Current ID count.
        */
        K id_count = 0;

    public:

        using ObjectManager<K, V>::ObjectManager;

        virtual ~AutoIdObjectManager() = default;

        /**
        * @brief CN:添加对象并分配ID。\nEN:Add object and allocate ID.
        * @param value CN:对象指针。EN:Object pointer.
        * @return CN:分配的ID。EN:Allocated ID.
        */
        virtual K Add(V *value)
        {
            if (!value)
            {
                return K(-1);
            }

            {
                K key;
                uint count;
                if (ObjectManager<K, V>::GetKeyByValue(value, &key, &count, true))
                {
                    return key;
                }
            }

            if (!ObjectManager<K, V>::Add(id_count, value))
            {
                return K(-1);
            }

            return id_count++;
        }

    };

    /**
    * @brief CN:以uint32为ID的对象管理器。\nEN:Object manager with uint32 ID.
    */
    template<typename V>
    using Uint32IdManager = AutoIdObjectManager<uint32, V>;

    /**
    * @brief CN:以uint64为ID的对象管理器。\nEN:Object manager with uint64 ID.
    */
    template<typename V>
    using Uint64IdManager = AutoIdObjectManager<uint64, V>;
} // namespace hgl
