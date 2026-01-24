/**
 * @file UnorderedSet.h
 * @brief CN:高性能无序集合模板（基于连续内存优化）\nEN:High-performance unordered set template (optimized with contiguous memory)
 */
#pragma once

#include <hgl/type/ActiveDataManager.h>
#include <hgl/type/HashIDMap.h>

namespace hgl
{
    // ==================== 管理型无序对象集合（Managed Object Set）====================

    /**
     * @brief CN:管理型无序对象集合（管理对象指针和生命周期）\nEN:Managed unordered object set (manages object pointers and lifetime)
     * @tparam T CN:对象类型（非指针），必须支持 operator==\nEN:Object type (non-pointer), must support operator==
     * @tparam MAX_COLLISION CN:最大哈希冲突数\nEN:Maximum hash collision count
     * 
     * CN:本版本用于管理非平凡类型对象：\nEN:This version manages non-trivial type objects:
     * - CN:自动管理对象生命周期（new/delete）\nEN:Automatic object lifetime management (new/delete)
     * - CN:支持带虚函数、动态内存的复杂对象\nEN:Supports complex objects with virtual functions, dynamic memory
     * - CN:使用连续内存存储指针，缓存友好\nEN:Uses contiguous memory for pointers, cache-friendly
     */
    template<typename T, int MAX_COLLISION = 4>
    class UnorderedManagedSet
    {
        // 编译期检查：T 不能是指针类型
        static_assert(!std::is_pointer_v<T>, 
            "UnorderedManagedSet does not accept pointer types directly. "
            "Use the object type itself (e.g., UnorderedManagedSet<MyClass>).");

    protected:
        using ThisClass = UnorderedManagedSet<T, MAX_COLLISION>;

        /**
         * @brief CN:指针管理器（连续内存存储指针）\nEN:Pointer manager (contiguous memory for pointers)
         */
        ActiveDataManager<T*> ptr_manager;

        /**
         * @brief CN:哈希到ID的映射表\nEN:Hash to ID mapping table
         */
        HashIDMap<MAX_COLLISION> hash_map;

        /**
         * @brief CN:根据值查找ID\nEN:Find ID by value
         */
        int FindID(const T& value) const
        {
            uint64 hash = ComputeFNV1aHash(value);
            return hash_map.Find(hash, [&](int id) {
                if (!ptr_manager.IsActive(id))
                    return false;
                T* existing_ptr;
                return ptr_manager.GetData(existing_ptr, id) && existing_ptr && *existing_ptr == value;
            });
        }

    public:

        UnorderedManagedSet() = default;
        
        virtual ~UnorderedManagedSet()
        {
            Clear();
        }

        // ==================== 容量管理 ====================

        void Reserve(int capacity)
        {
            ptr_manager.Reserve(capacity);
        }

        int GetCount() const
        {
            return ptr_manager.GetActiveCount();
        }

        bool IsEmpty() const
        {
            return GetCount() == 0;
        }

        // ==================== 添加 ====================

        /**
         * @brief CN:添加一个对象指针（推荐）\nEN:Add an object pointer (recommended)
         * @param ptr CN:对象指针，集合会管理其生命周期\nEN:Object pointer, set will manage its lifetime
         * @return CN:成功返回 true，已存在或指针为空返回 false\nEN:true if added, false if already exists or nullptr
         * @warning CN:添加后不要在外部删除该指针\nEN:Don't delete the pointer externally after adding
         */
        bool Add(T* ptr)
        {
            if (!ptr)
                return false;

            uint64 hash = ComputeFNV1aHash(*ptr);

            // 检查是否已存在
            int existing_id = hash_map.Find(hash, [&](int id) {
                if (!ptr_manager.IsActive(id))
                    return false;
                T* existing_ptr;
                return ptr_manager.GetData(existing_ptr, id) && existing_ptr && *existing_ptr == *ptr;
            });

            if (existing_id != -1)
                return false;  // 已存在

            // 获取或创建新ID
            int new_id;
            if (!ptr_manager.GetOrCreate(&new_id, 1))
                return false;  // 容量满或分配失败

            // 写入指针
            if (!ptr_manager.WriteData(ptr, new_id))
            {
                ptr_manager.Release(&new_id, 1);
                return false;
            }

            // 添加到哈希表
            hash_map.Add(hash, new_id);

            return true;
        }

        /**
         * @brief CN:添加一个对象（会创建副本，不推荐）\nEN:Add an object (creates a copy, not recommended)
         * @note CN:此方法会内部 new 对象副本，推荐直接使用 Add(T*)\nEN:This method creates a copy internally, prefer Add(T*)
         */
        bool Add(const T& value)
        {
            T* new_obj = new T(value);
            if (!new_obj)
                return false;

            if (!Add(new_obj))
            {
                delete new_obj;
                return false;
            }

            return true;
        }

        /**
         * @brief CN:批量添加对象（会创建副本）\nEN:Add multiple objects (creates copies)
         */
        int Add(const T* values, int count)
        {
            if (!values || count <= 0)
                return 0;

            int added = 0;
            for (int i = 0; i < count; i++)
            {
                if (Add(values[i]))
                    ++added;
            }
            return added;
        }

        /**
         * @brief CN:批量添加对象指针\nEN:Add multiple object pointers
         */
        int Add(T** ptrs, int count)
        {
            if (!ptrs || count <= 0)
                return 0;

            int added = 0;
            for (int i = 0; i < count; i++)
            {
                if (Add(ptrs[i]))
                    ++added;
            }
            return added;
        }

        // ==================== 查找 ====================

        bool Contains(const T& value) const
        {
            return FindID(value) != -1;
        }

        int Find(const T& value) const
        {
            return FindID(value);
        }

        // ==================== 删除 ====================

        /**
         * @brief CN:删除指定对象（会调用析构函数）\nEN:Delete specified object (calls destructor)
         */
        bool Delete(const T& value)
        {
            int id = FindID(value);
            if (id == -1)
                return false;

            // 获取指针并删除对象
            T* ptr;
            if (ptr_manager.GetData(ptr, id) && ptr)
            {
                delete ptr;
                
                // 清空指针以防止悬空
                T* null_ptr = nullptr;
                ptr_manager.WriteData(null_ptr, id);
            }

            // 释放ID
            return ptr_manager.Release(&id, 1) > 0;
        }

        /**
         * @brief CN:批量删除对象\nEN:Delete multiple objects
         */
        int Delete(const T* values, int count)
        {
            if (!values || count <= 0)
                return 0;

            int deleted = 0;
            for (int i = 0; i < count; i++)
            {
                if (Delete(values[i]))
                    ++deleted;
            }
            return deleted;
        }

        // ==================== Unlink（不删除对象）====================

        /**
         * @brief CN:从集合中移除对象（不删除对象）\nEN:Remove object from set (without deleting)
         * @note CN:对象指针不会被 delete，调用者需要自行管理内存\nEN:Object pointer won't be deleted, caller must manage memory
         */
        bool UnlinkByValue(const T& value)
        {
            int id = FindID(value);
            if (id == -1)
                return false;

            // 获取指针（但不删除对象）
            T* ptr;
            if (ptr_manager.GetData(ptr, id) && ptr)
            {
                // 清空指针以防止后续误用
                T* null_ptr = nullptr;
                ptr_manager.WriteData(null_ptr, id);
            }

            // 释放ID（只移除引用，不删除对象）
            return ptr_manager.Release(&id, 1) > 0;
        }

        /**
         * @brief CN:批量移除对象（不删除）\nEN:Batch unlink objects (without deleting)
         */
        int Unlink(const T* values, int count)
        {
            if (!values || count <= 0)
                return 0;

            int unlinked = 0;
            for (int i = 0; i < count; i++)
            {
                if (UnlinkByValue(values[i]))
                    ++unlinked;
            }
            return unlinked;
        }

        /**
         * @brief CN:移除所有对象（不删除）\nEN:Unlink all objects (without deleting)
         * @warning CN:调用后所有对象指针成为调用者的责任\nEN:After this call, all object pointers become caller's responsibility
         */
        void UnlinkAll()
        {
            // 清空所有指针但不删除对象
            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveArray();
            const int count = active_ids.GetCount();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                T* null_ptr = nullptr;
                ptr_manager.WriteData(null_ptr, id);
            }

            // 清空管理器
            ptr_manager.ReleaseAllActive();
            hash_map.Clear();
        }

        /**
         * @brief CN:清空所有对象（会删除所有对象）\nEN:Clear all objects (deletes all objects)
         */
        void Clear()
        {
            // 删除所有对象
            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveArray();
            const int count = active_ids.GetCount();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                T** ptr_ptr = ptr_manager.At(id);
                if (ptr_ptr && *ptr_ptr)
                {
                    delete *ptr_ptr;
                    *ptr_ptr = nullptr;
                }
            }

            // 清空管理器
            ptr_manager.ReleaseAllActive();
            hash_map.Clear();
        }

        // ==================== 获取数据 ====================

        bool Get(int index, T& value) const
        {
            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveArray();
            if (index < 0 || index >= active_ids.GetCount())
                return false;

            int id = active_ids[index];
            T* ptr;
            if (ptr_manager.GetData(ptr, id) && ptr)
            {
                value = *ptr;
                return true;
            }
            return false;
        }

        bool GetFirst(T& value) const
        {
            return Get(0, value);
        }

        bool GetLast(T& value) const
        {
            int count = GetCount();
            return count > 0 ? Get(count - 1, value) : false;
        }

        // ==================== 遍历 ====================

        template<typename F>
        void Enum(F&& func) const
        {
            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveArray();
            const int count = active_ids.GetCount();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                T* ptr;
                if (ptr_manager.GetData(ptr, id) && ptr)
                    func(*ptr);
            }
        }

        template<typename F>
        void EnumMutable(F&& func)
        {
            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveArray();
            const int count = active_ids.GetCount();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                T** ptr_ptr = ptr_manager.At(id);
                if (ptr_ptr && *ptr_ptr)
                    func(**ptr_ptr);
            }
        }

        // ==================== 集合运算 ====================

        int Union(const ThisClass& other)
        {
            int added = 0;
            other.Enum([&](const T& value) {
                if (Add(value))
                    ++added;
            });
            return added;
        }

        int Intersect(const ThisClass& other)
        {
            int removed = 0;
            
            ValueArray<T> to_remove;
            Enum([&](const T& value) {
                if (!other.Contains(value))
                    to_remove.Add(value);
            });

            for (int i = 0; i < to_remove.GetCount(); i++)
            {
                if (Delete(to_remove[i]))
                    ++removed;
            }

            return removed;
        }

        int Difference(const ThisClass& other)
        {
            int removed = 0;
            other.Enum([&](const T& value) {
                if (Delete(value))
                    ++removed;
            });
            return removed;
        }

        // ==================== 比较运算 ====================

        bool operator==(const ThisClass& other) const
        {
            if (GetCount() != other.GetCount())
                return false;

            bool all_match = true;
            Enum([&](const T& value) {
                if (!other.Contains(value))
                    all_match = false;
            });

            return all_match;
        }

        bool operator!=(const ThisClass& other) const
        {
            return !(*this == other);
        }

        // ==================== 直接指针访问（高级用法）====================

        /**
         * @brief CN:获取活跃ID数组\nEN:Get active ID array
         */
        const ValueBuffer<int>& GetActiveArray() const
        {
            return ptr_manager.GetActiveArray();
        }

        /**
         * @brief CN:通过ID直接访问对象指针\nEN:Direct object pointer access by ID
         * @warning CN:注意对象生命周期，不要存储返回的指针\nEN:Be careful with object lifetime, don't store returned pointer
         */
        T* At(int id)
        {
            T** ptr_ptr = ptr_manager.At(id);
            return ptr_ptr ? *ptr_ptr : nullptr;
        }

        const T* At(int id) const
        {
            return const_cast<ThisClass*>(this)->At(id);
        }
    };
} // namespace hgl
