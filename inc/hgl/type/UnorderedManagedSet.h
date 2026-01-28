/**
 * @file UnorderedSet.h
 * @brief CN:高性能无序集合模板（基于连续内存优化）\nEN:High-performance unordered set template (optimized with contiguous memory)
 */
#pragma once

#include <type_traits>
#include <vector>
#include <hgl/type/FNV1aHash.h>
#include <hgl/type/ActiveDataManager.h>
#include <absl/container/flat_hash_map.h>

namespace hgl
{
    // ==================== 管理型无序对象集合（Managed Object Set）====================

    /**
     * @brief CN:管理型无序对象集合（基于指针地址管理）\nEN:Managed unordered object set (pointer address-based management)
     * @tparam T CN:对象类型（非指针），必须支持 operator==\nEN:Object type (non-pointer), must support operator==
     *
     * CN:本版本用于管理非平凡类型对象：\nEN:This version manages non-trivial type objects:
     * - CN:自动管理对象生命周期（new/delete）\nEN:Automatic object lifetime management (new/delete)
     * - CN:支持带虚函数、动态内存的复杂对象\nEN:Supports complex objects with virtual functions, dynamic memory
     * - CN:使用连续内存存储指针，缓存友好\nEN:Uses contiguous memory for pointers, cache-friendly
     * - CN:基于指针地址去重（零开销哈希）\nEN:Pointer address-based deduplication (zero-overhead hashing)
     */
    template<typename T>
    class UnorderedManagedSet
    {
        // 编译期检查：T 不能是指针类型
        static_assert(!std::is_pointer_v<T>,
            "UnorderedManagedSet does not accept pointer types directly. "
            "Use the object type itself (e.g., UnorderedManagedSet<MyClass>).");

        // 编译期检查：T 必须是非平凡类型，平凡类型请使用 UnorderedValueSet
        static_assert(!std::is_trivially_copyable_v<T>,
            "UnorderedManagedSet is for non-trivially-copyable types; use UnorderedValueSet for trivial types.");

    protected:
        using ThisClass = UnorderedManagedSet<T>;

        /**
         * @brief CN:指针管理器（连续内存存储指针）\nEN:Pointer manager (contiguous memory for pointers)
         */
        ActiveDataManager<T*> ptr_manager;

        /**
         * @brief CN:哈希到ID的映射表\nEN:Hash to ID mapping table
         */
        absl::flat_hash_map<uint64, std::vector<int>> hash_map;

        /**
         * @brief CN:根据指针地址查找ID\nEN:Find ID by pointer address
         */
        int FindIDByPtr(const T* ptr) const
        {
            if (!ptr)
                return -1;

            uint64 hash = reinterpret_cast<uint64>(ptr);  // ✅ 零开销：直接用地址
            auto it = hash_map.find(hash);
            if (it == hash_map.end())
                return -1;
            
            for (int id : it->second) {
                if (!ptr_manager.IsActive(id))
                    continue;
                T* stored;
                if (ptr_manager.GetData(stored, id) && stored == ptr)
                    return id;  // ✅ 地址比较
            }
            return -1;
        }

        // 重建哈希表：用于数据结构变化后
        void RebuildHashMap()
        {
            hash_map.clear();

            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveView();
            const int count = active_ids.GetCount();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                if (!ptr_manager.IsActive(id))
                    continue;

                T* ptr = nullptr;
                if (!ptr_manager.GetData(ptr, id) || !ptr)
                    continue;

                uint64 hash = reinterpret_cast<uint64>(ptr);  // ✅ 零开销：直接用地址
                hash_map[hash].push_back(id);
            }
        }

    public:

        UnorderedManagedSet() = default;

        virtual ~UnorderedManagedSet()
        {
            Free();
        }

        // ==================== 容量管理 ====================

        void Reserve(int capacity)
        {
            ptr_manager.Reserve(capacity);
        }

        void Free()
        {
            ptr_manager.Free();
            hash_map.clear();
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
         * @brief CN:添加一个对象指针\nEN:Add an object pointer
         * @param ptr CN:对象指针，集合会管理其生命周期\nEN:Object pointer, set will manage its lifetime
         * @return CN:成功返回 true，已存在（地址重复）或指针为空返回 false\nEN:true if added, false if already exists (duplicate address) or nullptr
         * @warning CN:添加后不要在外部删除该指针\nEN:Don't delete the pointer externally after adding
         * @note CN:基于指针地址去重，不比较对象内容\nEN:Deduplication based on pointer address, not object content
         */
        bool Add(T* ptr)
        {
            if (!ptr)
                return false;

            uint64 hash = reinterpret_cast<uint64>(ptr);  // ✅ 零开销：直接用地址

            // 检查指针是否已存在
            auto it = hash_map.find(hash);
            if (it != hash_map.end()) {
                for (int id : it->second) {
                    if (!ptr_manager.IsActive(id))
                        continue;
                    T* stored;
                    if (ptr_manager.GetData(stored, id) && stored == ptr)
                        return false;  // 指针已存在
                }
            }

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
            hash_map[hash].push_back(new_id);

            return true;
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

        /**
         * @brief CN:批量添加对象（会创建副本）\nEN:Add multiple objects (creates copies)
         */
        int Add(const T* values, int count) = delete;                     ///<非指针批量添加禁用
        bool Add(const T& value) = delete;                                ///<非指针添加禁用

        /**
         * @brief CN:从集合中移除对象指针（不删除对象）\nEN:Remove object pointer from set (without deleting)
         * @note CN:对象指针不会被 delete，调用者需要自行管理内存\nEN:Object pointer won't be deleted, caller must manage memory
         */
        bool Unlink(T* ptr)
        {
            if (!ptr)
                return false;

            int id = FindIDByPtr(ptr);  // ✅ 使用地址查找
            if (id == -1)
                return false;

            // 清空指针以防后续误用
            T* null_ptr = nullptr;
            ptr_manager.WriteData(null_ptr, id);

            // 释放ID（只移除引用，不删除对象）
            return ptr_manager.Release(&id, 1) > 0;
        }

        /**
         * @brief CN:批量移除对象指针（不删除）\nEN:Batch unlink object pointers (without deleting)
         */
        int Unlink(T** ptrs, int count)
        {
            if (!ptrs || count <= 0)
                return 0;

            int unlinked = 0;
            for (int i = 0; i < count; i++)
            {
                if (Unlink(ptrs[i]))
                    ++unlinked;
            }
            return unlinked;
        }

        /**
         * @brief CN:删除指定对象指针（会调用析构函数）\nEN:Delete specified object pointer (calls destructor)
         */
        bool Delete(T* ptr)
        {
            if (!ptr)
                return false;

            int id = FindIDByPtr(ptr);  // ✅ 使用地址查找
            if (id == -1)
                return false;

            T* stored = nullptr;
            if (!ptr_manager.GetData(stored, id) || stored != ptr)
                return false;

            delete stored;

            T* null_ptr = nullptr;
            ptr_manager.WriteData(null_ptr, id);

            return ptr_manager.Release(&id, 1) > 0;
        }

        /**
         * @brief CN:批量删除对象指针\nEN:Delete multiple object pointers
         */
        int Delete(T** ptrs, int count)
        {
            if (!ptrs || count <= 0)
                return 0;

            int deleted = 0;
            for (int i = 0; i < count; i++)
            {
                if (Delete(ptrs[i]))
                    ++deleted;
            }
            return deleted;
        }

        int Delete(const T* values, int count) = delete;                   ///<非指针批量删除禁用
        bool Delete(const T& value) = delete;                              ///<非指针删除禁用

        // ==================== 查询和验证接口 ====================

        /**
         * @brief CN:检查指针是否在集合中\nEN:Check if pointer is in the set
         * @param ptr CN:要检查的指针\nEN:Pointer to check
         * @return CN:存在返回 true\nEN:true if exists
         * @note CN:基于指针地址检查\nEN:Check based on pointer address
         */
        bool Contains(const T* ptr) const
        {
            return FindIDByPtr(ptr) != -1;  // ✅ 简化逻辑
        }

        bool Contains(const T& value) const = delete;                      ///<非指针查询禁用
        int Find(const T& value) const = delete;                           ///<非指针Find禁用

        // ==================== 集合运算 ====================

        int Union(const ThisClass& other) = delete;
        int Intersect(const ThisClass& other) = delete;
        int Difference(const ThisClass& other) = delete;

        // ==================== 比较运算 ====================

        bool operator==(const ThisClass& other) const
        {
            return GetCount() == other.GetCount();
        }

        bool operator!=(const ThisClass& other) const
        {
            return !(*this == other);
        }

        /**
         * @brief CN:获取活跃ID数组\nEN:Get active ID array
         */
        const ValueBuffer<int> &GetActiveView() const
        {
            return ptr_manager.GetActiveView();
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

        // ==================== 哈希统计接口 ====================

        /**
         * @brief CN:获取哈希碰撞槽位数\nEN:Get collision slot count
         */
        int GetCollisionCount() const
        {
            return hash_map.GetCollisionCount();
        }

        /**
         * @brief CN:获取负载因子\nEN:Get load factor
         */
        float GetLoadFactor() const
        {
            return hash_map.GetLoadFactor(GetCount());
        }

        /**
         * @brief CN:获取平均碰撞链长度\nEN:Get average collision chain length
         */
        float GetAverageCollisionChainLength() const
        {
            return hash_map.GetAverageCollisionChainLength();
        }

        /**
         * @brief CN:获取溢出槽位数\nEN:Get collision overflow count
         */
        int GetCollisionOverflowCount() const
        {
            return hash_map.GetCollisionOverflowCount();
        }

        // ==================== 迭代器支持 ====================

        class ConstIterator
        {
        private:
            const UnorderedManagedSet* set;
            int index;

        public:
            ConstIterator(const UnorderedManagedSet* s, int idx) : set(s), index(idx) {}

            const T& operator*() const
            {
                const ValueBuffer<int>& active_ids = set->ptr_manager.GetActiveView();
                int id = active_ids[index];
                T* obj = nullptr;
                set->ptr_manager.GetData(obj, id);
                return *obj;
            }

            const T* operator->() const
            {
                const ValueBuffer<int>& active_ids = set->ptr_manager.GetActiveView();
                int id = active_ids[index];
                T* obj = nullptr;
                set->ptr_manager.GetData(obj, id);
                return obj;
            }

            ConstIterator& operator++()
            {
                ++index;
                return *this;
            }

            ConstIterator operator++(int)
            {
                ConstIterator temp = *this;
                ++index;
                return temp;
            }

            bool operator==(const ConstIterator& other) const
            {
                return set == other.set && index == other.index;
            }

            bool operator!=(const ConstIterator& other) const
            {
                return !(*this == other);
            }
        };

        using const_iterator = ConstIterator;

        ConstIterator begin() const { return ConstIterator(this, 0); }
        ConstIterator end() const { return ConstIterator(this, GetCount()); }
        ConstIterator cbegin() const { return begin(); }
        ConstIterator cend() const { return end(); }

        // ==================== 遍历 ====================

        template<typename F>
        void Enum(F&& func) const
        {
            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveView();
            const int count = active_ids.GetCount();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                T* ptr = nullptr;
                if (ptr_manager.GetData(ptr, id) && ptr)
                    func(*ptr);
            }
        }

        template<typename F>
        void EnumMutable(F&& func)
        {
            const ValueBuffer<int>& active_ids = ptr_manager.GetActiveView();
            const int count = active_ids.GetCount();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                T** ptr_ptr = ptr_manager.At(id);
                if (ptr_ptr && *ptr_ptr)
                    func(**ptr_ptr);
            }

            // 注意：修改对象内容不影响指针地址，无需重建哈希表
            // Note: Modifying object content doesn't affect pointer address, no need to rebuild hash map
        }
    };
} // namespace hgl
