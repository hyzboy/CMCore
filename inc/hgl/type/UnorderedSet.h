#pragma once

#include<hgl/type/HashIDMap.h>
#include<hgl/type/ValueArray.h>
#include<hgl/type/Pool.h>

namespace hgl
{
    /**
    * 基于哈希的无序集合模板（O(1) 平均查找时间）
    */
    template<typename T, int MAX_COLLISION = 4>
    class UnorderedSetTemplate
    {
    protected:

        using ThisClass = UnorderedSetTemplate<T, MAX_COLLISION>;

        using DataPool = ObjectPool<T>;
        using DataList = ValueArray<T *>;

        DataPool data_pool;
        DataList data_list;
        HashIDMap<MAX_COLLISION> hash_map;

    public:

        T **begin() const { return data_list.begin(); }
        T **end() const { return data_list.end(); }

    public: // 方法

        UnorderedSetTemplate() = default;
        virtual ~UnorderedSetTemplate() = default;

        const int   GetCount() const { return data_list.GetCount(); }           ///<取得数据总量
        const bool  IsEmpty() const { return data_list.IsEmpty(); }             ///<是否为空

        // ==================== 添加 ====================
        /**
         * 添加一个数据，如果数据已存在，返回 false
         * @param value 要添加的数据
         * @return 成功返回 true，已存在返回 false
         */
        bool Add(const T& value)
        {
            uint64 hash = ComputeFNV1aHash(value);

            // 检查是否已存在
            int existing_id = hash_map.Find(hash, [&](int id) {
                return *data_list[id] == value;
            });

            if(existing_id != -1)
                return false;  // 已存在

            // 从池中获取或创建新数据
            T* new_data = nullptr;
            if(!data_pool.GetOrCreate(new_data))
                return false;  // 池满或分配失败

            *new_data = value;

            int new_id = data_list.GetCount();
            data_list.Add(new_data);

            if(!hash_map.Add(hash, new_id)) {
                // 哈希表满，但数据已添加
                // 可以考虑记录警告日志
            }

            return true;
        }

        /**
         * 添加一批数据
         * @param data 数据指针
         * @param count 数据个数
         * @return 成功添加的数据个数
         */
        int Add(const T* data, int count)
        {
            if(!data || count <= 0)
                return 0;

            int added_count = 0;
            for(int i = 0; i < count; i++)
            {
                if(Add(data[i]))
                    ++added_count;
            }
            return added_count;
        }

        // ==================== 查找 ====================
        /**
         * 查找数据是否存在
         * @param value 要查找的数据
         * @return 数据所在索引，-1表示不存在
         */
        int Find(const T& value) const
        {
            uint64 hash = ComputeFNV1aHash(value);
            return hash_map.Find(hash, [&](int id) {
                return *data_list[id] == value;
            });
        }

        bool Contains(const T& value) const { return (Find(value) != -1); }    ///<确认这个数据是否存在

        // ==================== 获取 ====================
        T* GetItem(int index) const                                             ///<取指定序号的数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return nullptr;
            return data_list[index];
        }

        bool Get(int index, T& value) const                                     ///<根据序列号取得指定数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            value = *data_list[index];
            return true;
        }

        bool GetFirst(T& value) const                                           ///<取得第一个数据
        {
            if(IsEmpty())
                return false;
            value = *data_list[0];
            return true;
        }

        bool GetLast(T& value) const                                            ///<取得最后面一个数据
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return false;
            value = *data_list[count - 1];
            return true;
        }

        // ==================== 删除 ====================
        /**
         * 根据数据将指定数据从列表中移除
         * @param value 要删除的数据
         * @return 成功返回 true
         */
        virtual bool Delete(const T& value)
        {
            return DeleteAt(Find(value));
        }

        /**
         * 删除一批数据
         * @param data 数据指针
         * @param count 数据个数
         * @return 成功删除的数据个数
         */
        int Delete(const T* data, int count)
        {
            if(!data || count <= 0)
                return 0;

            int deleted_count = 0;
            for(int i = 0; i < count; i++)
            {
                if(Delete(data[i]))
                    ++deleted_count;
            }
            return deleted_count;
        }

        /**
         * 根据序号将指定数据从列表中移除
         * @param index 序号
         * @return 成功返回 true
         */
        virtual bool DeleteAt(int index)
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;

            T* item = data_list[index];
            data_list.Delete(index, 1);
            data_pool.Release(item);

            // 重建哈希映射（因为索引变化了）
            RebuildHashMap();
            return true;
        }

        // ==================== 清除 ====================
        virtual void Free()                                                     ///<清除所有数据，并释放内存
        {
            const int count = data_list.GetCount();
            T **data_ptr = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                data_pool.Release(*data_ptr);
                ++data_ptr;
            }

            data_list.Free();
            hash_map.Clear();
        }

        virtual void Clear()                                                    ///<清除所有数据，但不释放内存
        {
            const int count = data_list.GetCount();
            T **data_ptr = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                data_pool.Release(*data_ptr);
                ++data_ptr;
            }

            data_list.Clear();
            hash_map.Clear();
        }

        // ==================== 列表操作 ====================
        DataList& GetList() { return data_list; }                               ///<取得线性列表
        T** GetDataList() const { return data_list.GetData(); }                 ///<取得纯数据线性列表

        template<typename IT>
        int GetValueList(IT& il_list)                                           ///<取得所有数值合集
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return count;

            T **data_ptr = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                il_list.Add(**data_ptr);
                ++data_ptr;
            }

            return count;
        }

        // ==================== 枚举 ====================
        template<typename F>
        void Enum(F&& func)                                                     ///<枚举所有数据(支持lambda)
        {
            const int count = data_list.GetCount();
            if(count <= 0) return;

            T **data_ptr = data_list.GetData();
            for(int i = 0; i < count; i++)
            {
                func(**data_ptr);
                ++data_ptr;
            }
        }

        // ==================== 集合运算 ====================
        /**
         * 求并集（将另一个集合的所有元素添加到本集合）
         * @param other 另一个集合
         * @return 新增的元素个数
         */
        int Union(const ThisClass& other)
        {
            int added_count = 0;
            const int count = other.GetCount();
            T **data_ptr = other.data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                if(Add(**data_ptr))
                    ++added_count;
                ++data_ptr;
            }

            return added_count;
        }

        /**
         * 求交集（仅保留与另一个集合共有的元素）
         * @param other 另一个集合
         * @return 删除的元素个数
         */
        int Intersect(const ThisClass& other)
        {
            int deleted_count = 0;
            int i = 0;

            while(i < data_list.GetCount())
            {
                if(!other.Contains(*data_list[i]))
                {
                    DeleteAt(i);
                    ++deleted_count;
                    // 不增加 i，因为删除后后面的元素会前移
                }
                else
                {
                    ++i;
                }
            }

            return deleted_count;
        }

        /**
         * 求差集（删除与另一个集合共有的元素）
         * @param other 另一个集合
         * @return 删除的元素个数
         */
        int Difference(const ThisClass& other)
        {
            int deleted_count = 0;
            const int count = other.GetCount();
            T **data_ptr = other.data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                if(Delete(**data_ptr))
                    ++deleted_count;
                ++data_ptr;
            }

            return deleted_count;
        }

        // ==================== 哈希统计接口 ====================
        int GetCollisionCount() const { return hash_map.GetCollisionCount(); }
        float GetLoadFactor() const { return hash_map.GetLoadFactor(data_list.GetCount()); }
        float GetAverageCollisionChainLength() const { return hash_map.GetAverageCollisionChainLength(); }
        int GetCollisionOverflowCount() const { return hash_map.GetCollisionOverflowCount(); }

    protected:

        // 重建哈希映射（在删除后使用）
        void RebuildHashMap()
        {
            hash_map.Clear();

            const int count = data_list.GetCount();
            for(int i = 0; i < count; i++)
            {
                uint64 hash = ComputeFNV1aHash(*data_list[i]);
                hash_map.Add(hash, i);
            }
        }

        void operator=(const ThisClass&);  // 禁用赋值
    };//class UnorderedSetTemplate

    /**
     * 基于哈希的无序集合（平凡类型专用版本）</br>
     * 集合数据列表中不允许数据出现重复性，使用哈希表实现 O(1) 平均查找时间</br>
     * <b>重要限制：</b>仅支持平凡可复制类型（trivially copyable types）。
     * 非平凡类型（包含虚函数、动态内存、自定义构造/析构等）请使用 UnorderedManagedSet</br>
     *
     * @tparam T 必须支持 operator== 用于比较，且必须是平凡可复制类型（trivially copyable）
     * @see UnorderedManagedSet 非平凡类型版本
     */
    template<typename T, int MAX_COLLISION = 4>
    class UnorderedValueSet : public UnorderedSetTemplate<T, MAX_COLLISION>
    {
    public:

        static_assert(std::is_trivially_copyable_v<T>,
                      "UnorderedValueSet requires trivially copyable types. For non-trivial types, use UnorderedManagedSet instead.");

        UnorderedValueSet() = default;
        virtual ~UnorderedValueSet() = default;

        /**
         * @brief 相等比较运算符
         * CN: 比较两个集合中的所有元素是否完全相同（不考虑顺序）
         * EN: Check if two sets have identical elements (order-independent)
         *
         * @param other 要比较的另一个集合
         * @return 如果两个集合的元素相同，返回 true；否则返回 false
         */
        bool operator==(const UnorderedValueSet<T, MAX_COLLISION>& other) const
        {
            if(this->GetCount() != other.GetCount())
                return false;

            // 检查本集合的每个元素是否都在另一个集合中
            const int count = this->data_list.GetCount();
            T **data_ptr = this->data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                if(!other.Contains(**data_ptr))
                    return false;
                ++data_ptr;
            }

            return true;
        }

        /**
         * @brief 不相等比较运算符
         * CN: 检查两个集合中的元素是否不同
         * EN: Check if two sets have different elements
         *
         * @param other 要比较的另一个集合
         * @return 如果两个集合的元素不同，返回 true；否则返回 false
         */
        bool operator!=(const UnorderedValueSet<T, MAX_COLLISION>& other) const
        {
            return !(*this == other);
        }
    };//class UnorderedValueSet

    /**
     * 基于哈希的无序对象集合（非平凡类型专用版本）</br>
     * 集合数据列表中不允许数据出现重复性，使用哈希表实现 O(1) 平均查找时间</br>
     * 本版本支持非平凡类型（包含动态内存、自定义构造/析构等）</br>
     *
     * @tparam T 必须支持 operator== 用于比较，可以是非平凡类型（non-trivial）
     */
    template<typename T, int MAX_COLLISION = 4>
    class UnorderedManagedSet : public UnorderedSetTemplate<T, MAX_COLLISION>
    {
    protected:

        void DeleteObject(T* item)
        {
            if(!item) return;
            delete item;
        }

        void DeleteObject(int index)
        {
            if(index >= 0 && index < this->data_list.GetCount())
                DeleteObject(this->data_list[index]);
        }

    public:

        UnorderedManagedSet() = default;
        virtual ~UnorderedManagedSet() { DeleteAll(); }

        // ==================== Unlink（不删除对象） ====================
        bool UnlinkByValue(const T& value) { return UnlinkBySerial(this->Find(value)); }

        bool UnlinkBySerial(int index)
        {
            if(index < 0 || index >= this->data_list.GetCount())
                return false;
            UnorderedSetTemplate<T, MAX_COLLISION>::DeleteAt(index);
            return true;
        }

        void UnlinkAll() { UnorderedSetTemplate<T, MAX_COLLISION>::Free(); }

        // ==================== Delete（删除对象） ====================
        bool Delete(const T& value) override { return DeleteAt(this->Find(value)); }

        bool DeleteAt(int index) override
        {
            if(index < 0 || index >= this->data_list.GetCount())
                return false;
            DeleteObject(index);
            UnorderedSetTemplate<T, MAX_COLLISION>::DeleteAt(index);
            return true;
        }

        void DeleteAll()
        {
            const int count = this->data_list.GetCount();
            T **data_ptr = this->data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                DeleteObject(*data_ptr);
                ++data_ptr;
            }
            UnorderedSetTemplate<T, MAX_COLLISION>::Free();
        }

        void Clear() { DeleteAll(); }

        /**
         * @brief 相等比较运算符
         * CN: 比较两个集合中的所有对象是否完全相同（使用 operator==）
         * EN: Check if two sets have identical objects (using operator==)
         *
         * @param other 要比较的另一个集合
         * @return 如果两个集合的对象相同，返回 true；否则返回 false
         */
        bool operator==(const UnorderedManagedSet<T, MAX_COLLISION>& other) const
        {
            if(this->GetCount() != other.GetCount())
                return false;

            const int count = this->data_list.GetCount();
            T **data_ptr = this->data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                if(!other.Contains(**data_ptr))
                    return false;
                ++data_ptr;
            }

            return true;
        }

        /**
         * @brief 不相等比较运算符
         * CN: 检查两个集合中的对象是否不同
         * EN: Check if two sets have different objects
         *
         * @param other 要比较的另一个集合
         * @return 如果两个集合的对象不同，返回 true；否则返回 false
         */
        bool operator!=(const UnorderedManagedSet<T, MAX_COLLISION>& other) const
        {
            return !(*this == other);
        }
    };//class UnorderedManagedSet

}//namespace hgl
