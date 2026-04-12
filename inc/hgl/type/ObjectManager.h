/**
* @file ObjectManager.h
 * @brief CN:对象管理模板类，支持引用计数和ID分配。\nEN:Object management template class, supports reference counting and ID allocation.
 */
#pragma once

#include <ankerl/unordered_dense.h>
#include <hgl/type/DataType.h>
#include <functional>
#include <memory>
#include <vector>

namespace hgl
{
    // AI NOTE: Object manager stores key->ManagedObject with ref_count.
    // Get increments ref_count; Release decrements and optionally deletes.
    /**
    * @brief CN:带引用计数的管理对象。\nEN:Managed object with reference counting.
    */
    template<typename V>
    struct ManagedObject
    {
        /**
        * @brief CN:对象指针。\nEN:Object pointer.
        */
        V* value;

        /**
        * @brief CN:引用计数。\nEN:Reference count.
        */
        int ref_count;

        ManagedObject() : value(nullptr), ref_count(1) {}
        ManagedObject(V* v) : value(v), ref_count(1) {}
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
        * @brief CN:管理对象类型。\nEN:Managed object type.
        */
        using ManagedObj = ManagedObject<V>;

    protected:

        /**
        * @brief CN:对象映射表。\nEN:Object map table.
        */
        using MapType = ankerl::unordered_dense::map<K, ManagedObj, std::hash<K>, std::equal_to<K>>;
        MapType items;

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
            for (auto& kv : items)
            {
                ManagedObj& obj = kv.second;
                if (obj.value)
                {
                    delete obj.value;
                    obj.value = nullptr;
                }
            }

            items.clear();
        }

        /**
        * @brief CN:清除引用计数为0的对象。\nEN:Clear objects with zero reference count.
        */
        virtual void ClearFree()
        {
            // 收集需要删除的键
            std::vector<K> keys_to_delete;

            for (auto& kv : items)
            {
                const K& key = kv.first;
                ManagedObj& obj = kv.second;
                if (obj.ref_count <= 0)
                {
                    if (obj.value)
                    {
                        delete obj.value;
                        obj.value = nullptr;
                    }
                    keys_to_delete.push_back(key);
                }
            }

            // 删除收集到的键
            for (const K& key : keys_to_delete)
            {
                items.erase(key);
            }
        }

        /**
        * @brief CN:获取对象数量。\nEN:Get object count.
        * @return CN:对象数量。EN:Number of objects.
        */
        const int GetCount() const
        {
            return static_cast<int>(items.size());
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

            if (items.contains(key))
            {
                return false;
            }

            items.emplace(key, ManagedObj(obj));
            return true;
        }

        /**
        * @brief CN:查找对象。\nEN:Find object.
        * @param key CN:键。EN:Key.
        * @return CN:对象指针或nullptr。EN:Object pointer or nullptr.
        */
        virtual V *Find(const K &key) const
        {
            auto it = items.find(key);
            return it != items.end() ? it->second.value : nullptr;
        }

        /**
        * @brief CN:获取对象并增加引用计数。\nEN:Get object and increase reference count.
        * @param key CN:键。EN:Key.
        * @return CN:对象指针或nullptr。EN:Object pointer or nullptr.
        */
        virtual V *Get(const K &key)
        {
            auto it = items.find(key);
            if (it != items.end())
            {
                ++it->second.ref_count;
                return it->second.value;
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
            for (auto& kv : items)
            {
                const K& k = kv.first;
                ManagedObj& obj = kv.second;
                if (obj.value == value)
                {
                    if (key)
                    {
                        *key = k;
                    }

                    if (count)
                    {
                        *count = obj.ref_count;
                    }

                    if (add_ref)
                    {
                        ++obj.ref_count;
                    }

                    return true;
                }
            }

            return false;
        }

        /**
        * @brief CN:根据键获取引用计数。\nEN:Get reference count by key.
        * @param key CN:对象键。EN:Object key.
        * @return CN:引用计数，键不存在返回-1。EN:Reference count, or -1 if key not found.
        */
        virtual int GetRefCount(const K &key) const
        {
            auto it = items.find(key);
            return it != items.end() ? it->second.ref_count : -1;
        }

        /**
        * @brief CN:根据对象指针获取引用计数。\nEN:Get reference count by object pointer.
        * @param value CN:对象指针。EN:Object pointer.
        * @return CN:引用计数，对象不存在返回-1。EN:Reference count, or -1 if object not found.
        */
        virtual int GetRefCount(V *value) const
        {
            int result = -1;

            for (const auto& kv : items)
            {
                const ManagedObj& obj = kv.second;
                if (obj.value == value)
                {
                    result = obj.ref_count;
                    break;
                }
            }

            return result;
        }

        /**
        * @brief CN:通过键释放对象。\nEN:Release object by key.
        * @param key CN:对象键。EN:Object key.
        * @param zero_clear CN:为true时引用计数为0则删除对象。EN:If true, delete object when ref_count is zero.
        * @return CN:剩余引用计数。EN:Remaining reference count.
        */
        virtual int Release(const K &key, bool zero_clear = false)
        {
            auto it = items.find(key);
            if (it == items.end())
            {
                return -1;
            }

            ManagedObj& obj = it->second;

            --obj.ref_count;
            int remaining_count = obj.ref_count;

            if (remaining_count <= 0 && zero_clear)
            {
                if (obj.value)
                {
                    delete obj.value;
                    obj.value = nullptr;
                }
                items.erase(it);
            }

            return remaining_count;
        }

        /**
        * @brief CN:通过对象指针释放对象。\nEN:Release object by pointer.
        * @param value CN:对象指针。EN:Object pointer.
        * @param zero_clear CN:为true时引用计数为0则删除对象。EN:If true, delete object when ref_count is zero.
        * @return CN:剩余引用计数。EN:Remaining reference count.
        * @warning CN:此API性能较差，需要遍历所有对象。建议使用Release(const K &key)。\nEN:This API is slow as it enumerates all items. Consider using Release(const K &key) instead.
        */
        [[deprecated("Performance warning: This API enumerates all items. Use Release(const K &key) for better performance.")]]
        int Release(V *value, bool zero_clear = false)
        {
            K found_key;
            bool found = false;
            int remaining_count = -1;

            for (auto& kv : items)
            {
                const K& k = kv.first;
                ManagedObj& obj = kv.second;
                if (obj.value == value)
                {
                    found_key = k;
                    found = true;
                    break;
                }
            }

            if (found)
            {
                remaining_count = Release(found_key, zero_clear);
            }

            return remaining_count;
        }
    };

    /**
    * @brief CN:通用对象管理器。\nEN:General purpose object manager.
    * @tparam K CN:键类型。EN:Key type.
    * @tparam V CN:值类型。EN:Value type.
    */
    template<typename K, typename V>
    class ManagedObjectRegistry : public ObjectManagerImpl<K, V>
    {
    public:

        ManagedObjectRegistry() = default;

        virtual ~ManagedObjectRegistry() = default;

    };

    /**
    * @brief CN:带自动ID分配的对象管理器。\nEN:Object manager with automatic ID allocation.
    * @tparam K CN:ID类型。EN:ID type.
    * @tparam V CN:值类型。EN:Value type.
    */
    template<typename K, typename V>
    class AutoIdObjectManager : public ManagedObjectRegistry<K, V>
    {
    protected:

        /**
        * @brief CN:当前ID计数。\nEN:Current ID count.
        */
        K id_count = 0;

    public:

        using ManagedObjectRegistry<K, V>::ManagedObjectRegistry;

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
                if (ManagedObjectRegistry<K, V>::GetKeyByValue(value, &key, &count, true))
                {
                    return key;
                }
            }

            if (!ManagedObjectRegistry<K, V>::Add(id_count, value))
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

    /**
    * @brief CN:基于 shared_ptr 的自动 ID 对象管理器。\nEN:shared_ptr-based object manager with automatic ID allocation.
    *
    * Manager 内部以 shared_ptr<V> 持有对象的所有权。
    * 外部调用方通过 weak_ptr<V> 持有弱引用：Manager 销毁或 Release 后弱引用自动 expired，
    * 无需手动通知，也不会产生悬空指针。
    *
    * @tparam K CN:ID 类型（通常为 int/uint）。EN:ID type (typically int/uint).
    * @tparam V CN:值类型。EN:Value type.
    */
    template<typename K, typename V>
    class SharedObjectManager
    {
    protected:
        using MapType = ankerl::unordered_dense::map<K, std::shared_ptr<V>>;
        MapType items;
        K id_count = 0;

    public:

        SharedObjectManager() = default;
        virtual ~SharedObjectManager() { Clear(); }

        /**
        * @brief CN:清空所有对象。\nEN:Clear all managed objects.
        */
        void Clear() { items.clear(); }

        /**
        * @brief CN:获取当前管理的对象数量。\nEN:Get number of managed objects.
        */
        int GetCount() const { return static_cast<int>(items.size()); }

        /**
        * @brief CN:接管一个裸指针，分配 ID，返回 weak_ptr。\nEN:Take ownership of a raw pointer, assign ID, return weak_ptr.
        * @param value CN:对象裸指针（Manager 接管所有权）。EN:Raw pointer (ownership transferred to Manager).
        * @return CN:分配的 ID，失败返回 K(-1)。EN:Allocated ID, or K(-1) on failure.
        */
        K Add(V *value)
        {
            if(!value) return K(-1);
            K key = id_count++;
            items.emplace(key, std::shared_ptr<V>(value));
            return key;
        }

        /**
        * @brief CN:通过 ID 获取 weak_ptr（不增加引用计数）。\nEN:Get weak_ptr by ID (does not extend lifetime).
        */
        std::weak_ptr<V> Get(const K &key) const
        {
            auto it = items.find(key);
            if(it != items.end()) return it->second;
            return {};
        }

        /**
        * @brief CN:通过 ID 获取 shared_ptr（延长生命周期）。\nEN:Get shared_ptr by ID (extends lifetime).
        */
        std::shared_ptr<V> Acquire(const K &key) const
        {
            auto it = items.find(key);
            if(it != items.end()) return it->second;
            return nullptr;
        }

        /**
        * @brief CN:通过 ID 释放对象（从 Manager 移除，引用计数归零则 delete）。
        * EN:Release object by ID (remove from Manager; deleted when all shared_ptrs expire).
        */
        bool Release(const K &key)
        {
            return items.erase(key) > 0;
        }

        /**
        * @brief CN:通过 shared_ptr 释放对象。EN:Release object by shared_ptr.
        */
        bool Release(const std::shared_ptr<V> &ptr)
        {
            for(auto it = items.begin(); it != items.end(); ++it)
            {
                if(it->second == ptr)
                {
                    items.erase(it);
                    return true;
                }
            }
            return false;
        }

        /**
        * @brief CN:通过裸指针释放对象。EN:Release object by raw pointer.
        */
        bool Release(V *value)
        {
            for(auto it = items.begin(); it != items.end(); ++it)
            {
                if(it->second.get() == value)
                {
                    items.erase(it);
                    return true;
                }
            }
            return false;
        }
    };//class SharedObjectManager

} // namespace hgl
