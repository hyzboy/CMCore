#pragma once

#include<ankerl/unordered_dense.h>
#include<vector>

namespace hgl
{
    /**
     * @brief 高效双向映射容器
     * 已通过20万级数量压力测试
     *
     * 架构设计：
     * - 底层数据：KEY向量 + VALUE向量（各存一份）
     * - 索引层：两个ankerl hashmap，分别存KEY->索引对 和 VALUE->索引对
     *
     * 优势：
     * 1. 内存高效：KEY和VALUE各只存一份，hashmap只存纯索引
     * 2. 序列化友好：KEY/VALUE向量本身就是序列化格式
     * 3. 访问快速：直接通过索引访问，减少指针跳跃
     * 4. 结构清晰：明确分离数据层和索引层
     *
     * 使用示例：
     * @code
     * BidirectionalMap<int, std::string> map;
     * map.Add(1, "one");
     * map.Add(2, "two");
     *
     * // 从KEY查询
     * std::string value;
     * map.Get(1, value);  // value = "one"
     *
     * // 从VALUE查询
     * int key;
     * map.GetByValue("one", key);  // key = 1
     *
     * // 删除（自动同步两个映射）
     * map.DeleteByKey(1);
     * map.DeleteByValue("two");
     *
     * // 获取KEY/VALUE向量用于序列化
     * const ValueArray<int>& keys = map.GetKeys();
     * const ValueArray<std::string>& values = map.GetValues();
     * @endcode
     *
     * @tparam K Key 类型
     * @tparam V Value 类型
     */
    template<typename K, typename V>
    class BidirectionalMap
    {
    protected:
        // ============================================================
        // 底层数据存储（各存一份）
        // ============================================================
        std::vector<K> keys;    ///< 所有KEY的向量
        std::vector<V> values;  ///< 所有VALUE的向量

        // ============================================================
        // 索引对结构（纯整数，极小开销）
        // ============================================================
        struct IndexPair
        {
            int key_idx;        ///< 该对在KEY向量中的索引
            int value_idx;      ///< 该对在VALUE向量中的索引
        };

        // ============================================================
        // 索引映射（ankerl hashmap，只存索引对，极高效率）
        // ============================================================
        ankerl::unordered_dense::map<K, IndexPair> forward;  ///< K -> IndexPair
        ankerl::unordered_dense::map<V, IndexPair> reverse;  ///< V -> IndexPair

    public:
        BidirectionalMap() = default;
        virtual ~BidirectionalMap() = default;

        // ============================================================
        // 迭代器支持
        // ============================================================

        /**
         * @brief KEY-VALUE对迭代器
         */
        class KVIterator
        {
        private:
            const BidirectionalMap* map;
            int index;

        public:
            using difference_type = int;
            using value_type = std::pair<const K&, const V&>;
            using pointer = std::pair<const K*, const V*>;
            using reference = std::pair<const K&, const V&>;
            using iterator_category = std::forward_iterator_tag;

            KVIterator(const BidirectionalMap* m, int idx) : map(m), index(idx) {}

            reference operator*() const
            {
                return {map->keys[index], map->values[index]};
            }

            KVIterator& operator++()
            {
                ++index;
                return *this;
            }

            KVIterator operator++(int)
            {
                KVIterator tmp = *this;
                ++index;
                return tmp;
            }

            bool operator==(const KVIterator& other) const
            {
                return index == other.index;
            }

            bool operator!=(const KVIterator& other) const
            {
                return index != other.index;
            }
        };

        /**
         * @brief KEY迭代器
         */
        class KeyIterator
        {
        private:
            typename std::vector<K>::const_iterator iter;

        public:
            using difference_type = typename std::vector<K>::iterator::difference_type;
            using value_type = K;
            using pointer = const K*;
            using reference = const K&;
            using iterator_category = std::forward_iterator_tag;

            KeyIterator(typename std::vector<K>::const_iterator it) : iter(it) {}

            reference operator*() const { return *iter; }
            pointer operator->() const { return &(*iter); }

            KeyIterator& operator++()
            {
                ++iter;
                return *this;
            }

            KeyIterator operator++(int)
            {
                KeyIterator tmp = *this;
                ++iter;
                return tmp;
            }

            bool operator==(const KeyIterator& other) const { return iter == other.iter; }
            bool operator!=(const KeyIterator& other) const { return iter != other.iter; }
        };

        /**
         * @brief VALUE迭代器
         */
        class ValueIterator
        {
        private:
            typename std::vector<V>::const_iterator iter;

        public:
            using difference_type = typename std::vector<V>::iterator::difference_type;
            using value_type = V;
            using pointer = const V*;
            using reference = const V&;
            using iterator_category = std::forward_iterator_tag;

            ValueIterator(typename std::vector<V>::const_iterator it) : iter(it) {}

            reference operator*() const { return *iter; }
            pointer operator->() const { return &(*iter); }

            ValueIterator& operator++()
            {
                ++iter;
                return *this;
            }

            ValueIterator operator++(int)
            {
                ValueIterator tmp = *this;
                ++iter;
                return tmp;
            }

            bool operator==(const ValueIterator& other) const { return iter == other.iter; }
            bool operator!=(const ValueIterator& other) const { return iter != other.iter; }
        };

        /**
         * @brief KEY视图（支持range-based for循环）
         */
        class KeysView
        {
        private:
            const BidirectionalMap* map;

        public:
            KeysView(const BidirectionalMap* m) : map(m) {}

            KeyIterator begin() const { return KeyIterator(map->keys.begin()); }
            KeyIterator end() const { return KeyIterator(map->keys.end()); }
        };

        /**
         * @brief VALUE视图（支持range-based for循环）
         */
        class ValuesView
        {
        private:
            const BidirectionalMap* map;

        public:
            ValuesView(const BidirectionalMap* m) : map(m) {}

            ValueIterator begin() const { return ValueIterator(map->values.begin()); }
            ValueIterator end() const { return ValueIterator(map->values.end()); }
        };

        /**
         * @brief 获取KEY-VALUE对迭代器开始位置
         * @note 支持 for(auto [k, v] : map)
         */
        KVIterator begin() const { return KVIterator(this, 0); }

        /**
         * @brief 获取KEY-VALUE对迭代器结束位置
         */
        KVIterator end() const { return KVIterator(this, static_cast<int>(keys.size())); }

        /**
         * @brief 获取KEY视图
         * @note 支持 for(auto k : map.Keys())
         */
        KeysView Keys() const { return KeysView(this); }

        /**
         * @brief 获取VALUE视图
         * @note 支持 for(auto v : map.Values())
         */
        ValuesView Values() const { return ValuesView(this); }

        /**
         * @brief 获取KEY向量（只读）
         */
        const std::vector<K>& GetKeys() const { return keys; }

        /**
         * @brief 获取VALUE向量（只读）
         */
        const std::vector<V>& GetValues() const { return values; }

        /**
         * @brief 获取KEY向量（可写）
         */
        std::vector<K>& GetKeys() { return keys; }

        /**
         * @brief 获取VALUE向量（可写）
         */
        std::vector<V>& GetValues() { return values; }

        // ============================================================
        // 基础操作
        // ============================================================

        /**
         * @brief 添加一个双向映射对
         * @param key 键
         * @param value 值
         * @return 成功返回 true，如果 key 或 value 已存在返回 false
         *
         * @note 如果操作失败，两个映射都不会被修改（事务性）
         */
        bool Add(const K& key, const V& value)
        {
            // 检查 key 和 value 是否已存在
            if (forward.contains(key) || reverse.contains(value))
                return false;

            // 添加到数据向量
            int key_idx = static_cast<int>(keys.size());
            int value_idx = static_cast<int>(values.size());

            keys.push_back(key);
            values.push_back(value);

            // 添加到索引映射
            IndexPair pair{key_idx, value_idx};
            forward[key] = pair;
            reverse[value] = pair;

            return true;
        }

        /**
         * @brief 添加一个双向映射对（移动语义版本）
         * @param key 键（右值引用）
         * @param value 值（右值引用）
         * @return 成功返回 true，如果 key 或 value 已存在返回 false
         *
         * @note 如果操作失败，两个映射都不会被修改（事务性）
         */
        bool Add(K&& key, V&& value)
        {
            // 检查 key 和 value 是否已存在
            if (forward.contains(key) || reverse.contains(value))
                return false;

            // 添加到数据向量
            int key_idx = static_cast<int>(keys.size());
            int value_idx = static_cast<int>(values.size());

            keys.push_back(std::move(key));
            values.push_back(std::move(value));

            // 添加到索引映射（使用移动后的对象可能失败，但已经push_back了）
            const K& final_key = keys.back();
            const V& final_value = values.back();

            IndexPair pair{key_idx, value_idx};
            forward[final_key] = pair;
            reverse[final_value] = pair;

            return true;
        }

        /**
         * @brief 添加一个双向映射对（混合语义版本1：键右值）
         */
        bool Add(K&& key, const V& value)
        {
            // 检查 key 和 value 是否已存在
            if (forward.contains(key) || reverse.contains(value))
                return false;

            int key_idx = static_cast<int>(keys.size());
            int value_idx = static_cast<int>(values.size());

            keys.push_back(std::move(key));
            values.push_back(value);

            const K& final_key = keys.back();
            const V& final_value = values.back();

            IndexPair pair{key_idx, value_idx};
            forward[final_key] = pair;
            reverse[final_value] = pair;

            return true;
        }

        /**
         * @brief 添加一个双向映射对（混合语义版本2：值右值）
         */
        bool Add(const K& key, V&& value)
        {
            // 检查 key 和 value 是否已存在
            if (forward.contains(key) || reverse.contains(value))
                return false;

            int key_idx = static_cast<int>(keys.size());
            int value_idx = static_cast<int>(values.size());

            keys.push_back(key);
            values.push_back(std::move(value));

            const K& final_key = keys.back();
            const V& final_value = values.back();

            IndexPair pair{key_idx, value_idx};
            forward[final_key] = pair;
            reverse[final_value] = pair;

            return true;
        }

        /**
         * @brief 根据 KEY 获取 VALUE
         * @param key 键
         * @param value 输出参数，存储值
         * @return 找到返回 true，否则返回 false
         */
        bool Get(const K& key, V& value) const
        {
            auto it = forward.find(key);
            if (it == forward.end())
                return false;

            const int value_idx = it->second.value_idx;
            if (value_idx < 0 || value_idx >= (int)values.size())
                return false;

            value = values[value_idx];
            return true;
        }

        /**
         * @brief 根据 VALUE 获取 KEY
         * @param value 值
         * @param key 输出参数，存储键
         * @return 找到返回 true，否则返回 false
         */
        bool GetByValue(const V& value, K& key) const
        {
            auto it = reverse.find(value);
            if (it == reverse.end())
                return false;

            const int key_idx = it->second.key_idx;
            if (key_idx < 0 || key_idx >= (int)keys.size())
                return false;

            key = keys[key_idx];
            return true;
        }

        /**
         * @brief 获取 VALUE 对应的指针（const 版本）
         * @param key 键
         * @return 找到返回指针，否则返回 nullptr
         */
        const V* GetValuePointer(const K& key) const
        {
            auto it = forward.find(key);
            if (it == forward.end())
                return nullptr;

            const int value_idx = it->second.value_idx;
            if (value_idx < 0 || value_idx >= (int)values.size())
                return nullptr;

            return &values[value_idx];
        }

        /**
         * @brief 获取 VALUE 对应的指针（非 const 版本）
         * @param key 键
         * @return 找到返回指针，否则返回 nullptr
         */
        V* GetValuePointer(const K& key)
        {
            auto it = forward.find(key);
            if (it == forward.end())
                return nullptr;

            const int value_idx = it->second.value_idx;
            if (value_idx < 0 || value_idx >= (int)values.size())
                return nullptr;

            return &values[value_idx];
        }

        /**
         * @brief 获取 KEY 对应的指针（const 版本）
         * @param value 值
         * @return 找到返回指针，否则返回 nullptr
         */
        const K* GetKeyPointer(const V& value) const
        {
            auto it = reverse.find(value);
            if (it == reverse.end())
                return nullptr;

            return &keys[it->second.key_idx];
        }

        /**
         * @brief 获取 KEY 对应的指针（非 const 版本）
         * @param value 值
         * @return 找到返回指针，否则返回 nullptr
         */
        K* GetKeyPointer(const V& value)
        {
            auto it = reverse.find(value);
            if (it == reverse.end())
                return nullptr;

            return &keys[it->second.key_idx];
        }

        // ============================================================
        // 删除操作（自动同步两个映射）
        // ============================================================

        /**
         * @brief 根据 KEY 删除映射对
         * @param key 键
         * @return 成功删除返回 true，key 不存在返回 false
         *
         * @note 自动删除对应的 VALUE 映射，并从向量中移除数据
         */
        bool DeleteByKey(const K& key)
        {
            auto it = forward.find(key);
            if (it == forward.end())
                return false;

            const int key_del_idx = it->second.key_idx;      // 要删除的KEY在keys中的位置
            const int value_del_idx = it->second.value_idx;  // 对应VALUE在values中的位置

            // 验证索引的有效性
            if (key_del_idx < 0 || key_del_idx >= (int)keys.size() ||
                value_del_idx < 0 || value_del_idx >= (int)values.size())
                return false;  // 索引无效！严重错误

            const V value = values[value_del_idx];

            // 验证一致性
            if (keys[key_del_idx] != key || values[value_del_idx] != value)
                return false;  // 严重错误！

            // 从哈希表中删除
            forward.erase(it);
            reverse.erase(value);

            // ============================================================
            // 处理KEYS向量（交换删除）
            // ============================================================
            const int last_key_idx = static_cast<int>(keys.size()) - 1;
            if (key_del_idx != last_key_idx)
            {
                const K last_key = keys[last_key_idx];
                keys[key_del_idx] = last_key;

                // 更新last_key的key_idx（通过forward查找它的value_idx）
                auto fwd_it = forward.find(last_key);
                if (fwd_it != forward.end())
                {
                    fwd_it->second.key_idx = key_del_idx;
                    const int last_key_value_idx = fwd_it->second.value_idx;
                    if (last_key_value_idx >= 0 && last_key_value_idx < (int)values.size())
                    {
                        const V& last_key_value = values[last_key_value_idx];
                        auto rev_it = reverse.find(last_key_value);
                        if (rev_it != reverse.end())
                            rev_it->second.key_idx = key_del_idx;
                    }
                }
            }
            keys.pop_back();

            // ============================================================
            // 处理VALUES向量（交换删除，独立处理）
            // ============================================================
            const int last_value_idx = static_cast<int>(values.size()) - 1;
            if (value_del_idx != last_value_idx)
            {
                const V last_value = values[last_value_idx];
                values[value_del_idx] = last_value;

                // 更新last_value的value_idx
                // reverse map还存有这个last_value的映射
                auto rev_it = reverse.find(last_value);
                if (rev_it != reverse.end())
                {
                    // rev_it->second.key_idx是这个value对应的key在keys中的位置
                    // 注意：这个key_idx是正确的，因为keys交换时已经更新了
                    // 但我们需要通过这个key_idx找到keys中的key，然后查forward更新
                    const int owner_key_idx = rev_it->second.key_idx;
                    if (owner_key_idx >= 0 && owner_key_idx < (int)keys.size())
                    {
                        const K owner_key = keys[owner_key_idx];
                        auto fwd_it = forward.find(owner_key);
                        if (fwd_it != forward.end())
                            fwd_it->second.value_idx = value_del_idx;
                    }
                    rev_it->second.value_idx = value_del_idx;
                }
            }
            values.pop_back();

            return true;
        }

        /**
         * @brief 根据 VALUE 删除映射对
         * @param value 值
         * @return 成功删除返回 true，value 不存在返回 false
         *
         * @note 自动删除对应的 KEY 映射，并从向量中移除数据
         */
        bool DeleteByValue(const V& value)
        {
            auto it = reverse.find(value);
            if (it == reverse.end())
                return false;

            const int key_del_idx = it->second.key_idx;      // 对应KEY在keys中的位置
            const int value_del_idx = it->second.value_idx;  // 要删除的VALUE在values中的位置
            const K key = keys[key_del_idx];

            // 验证一致性
            if (values[value_del_idx] != value || keys[key_del_idx] != key)
                return false;  // 严重错误！

            // 从哈希表中删除
            reverse.erase(it);
            forward.erase(key);

            // ============================================================
            // 处理KEYS向量（交换删除，独立处理）
            // ============================================================
            const int last_key_idx = static_cast<int>(keys.size()) - 1;
            if (key_del_idx != last_key_idx)
            {
                const K last_key = keys[last_key_idx];
                keys[key_del_idx] = last_key;

                // 更新last_key的key_idx
                auto fwd_it = forward.find(last_key);
                if (fwd_it != forward.end())
                {
                    fwd_it->second.key_idx = key_del_idx;
                    const int last_key_value_idx = fwd_it->second.value_idx;
                    if (last_key_value_idx >= 0 && last_key_value_idx < (int)values.size())
                    {
                        const V& last_key_value = values[last_key_value_idx];
                        auto rev_it = reverse.find(last_key_value);
                        if (rev_it != reverse.end())
                            rev_it->second.key_idx = key_del_idx;
                    }
                }
            }
            keys.pop_back();

            // ============================================================
            // 处理VALUES向量（交换删除）
            // ============================================================
            const int last_value_idx = static_cast<int>(values.size()) - 1;
            if (value_del_idx != last_value_idx)
            {
                const V last_value = values[last_value_idx];
                values[value_del_idx] = last_value;

                // 更新last_value的value_idx
                // reverse map还存有这个last_value的映射
                auto rev_it = reverse.find(last_value);
                if (rev_it != reverse.end())
                {
                    // rev_it->second.key_idx是这个value对应的key在keys中的位置
                    const int owner_key_idx = rev_it->second.key_idx;
                    if (owner_key_idx >= 0 && owner_key_idx < (int)keys.size())
                    {
                        const K owner_key = keys[owner_key_idx];
                        auto fwd_it = forward.find(owner_key);
                        if (fwd_it != forward.end())
                            fwd_it->second.value_idx = value_del_idx;
                    }
                    rev_it->second.value_idx = value_del_idx;
                }
            }
            values.pop_back();

            return true;
        }

        // ============================================================
        // 检查操作
        // ============================================================

        /**
         * @brief 检查是否包含指定的 KEY
         * @param key 键
         * @return 存在返回 true，否则返回 false
         */
        bool ContainsKey(const K& key) const
        {
            return forward.contains(key);
        }

        /**
         * @brief 检查是否包含指定的 VALUE
         * @param value 值
         * @return 存在返回 true，否则返回 false
         */
        bool ContainsValue(const V& value) const
        {
            return reverse.contains(value);
        }

        // ============================================================
        // 修改操作
        // ============================================================

        /**
         * @brief 修改指定 KEY 的 VALUE（KEY 必须存在）
         * @param key 键
         * @param new_value 新值
         * @return 成功返回 true，KEY 不存在或新值已被其他 KEY 使用返回 false
         *
         * @note 如果新值与其他 KEY 的映射冲突，操作失败
         */
        bool Change(const K& key, const V& new_value)
        {
            auto it = forward.find(key);
            if (it == forward.end())
                return false;

            const int value_idx = it->second.value_idx;
            if (value_idx < 0 || value_idx >= (int)values.size())
                return false;

            // 检查新值是否已被使用
            const V old_value = values[value_idx];
            if (old_value == new_value)
                return true;

            if (reverse.contains(new_value))
                return false;

            // 更新数据向量和反向索引
            values[value_idx] = new_value;
            reverse.erase(old_value);
            reverse[new_value] = {it->second.key_idx, value_idx};

            return true;
        }

        /**
         * @brief 修改或添加（如果 KEY 不存在则添加，存在则更新）
         * @param key 键
         * @param value 值
         * @return 总是返回 true
         *
         * @note 如果 VALUE 被其他 KEY 使用，会自动删除该旧映射
         */
        bool ChangeOrAdd(const K& key, const V& value)
        {
            // 首先检查KEY是否存在
            auto key_it = forward.find(key);

            if (key_it != forward.end())
            {
                // KEY 存在，尝试更新
                const int current_key_idx = key_it->second.key_idx;
                const int current_value_idx = key_it->second.value_idx;
                const V old_value = values[current_value_idx];

                if (old_value == value)
                    return true;  // 值相同，无需修改

                // 检查新值是否被其他KEY占用
                auto value_it = reverse.find(value);
                if (value_it != reverse.end())
                {
                    // 新值被其他KEY占用，需要删除那个KEY的映射
                    const int other_key_idx = value_it->second.key_idx;
                    const K other_key = keys[other_key_idx];

                    // 防御：不应该是同一个KEY
                    if (other_key == key)
                        return true;

                    // 删除其他映射，这会改变向量
                    DeleteByKey(other_key);

                    // DeleteByKey后，我们需要重新查询当前KEY的信息，
                    // 因为向量可能被重新排列
                    key_it = forward.find(key);
                    if (key_it == forward.end())
                        return false;  // 严重错误
                }

                // 此时key_it是最新的，安全访问
                const int final_value_idx = key_it->second.value_idx;
                const V final_old_value = values[final_value_idx];

                // 删除旧值的映射
                reverse.erase(final_old_value);

                // 更新新值
                values[final_value_idx] = value;
                reverse[value] = {key_it->second.key_idx, final_value_idx};

                return true;
            }
            else
            {
                // KEY 不存在，需要检查VALUE是否被占用
                auto value_it = reverse.find(value);
                if (value_it != reverse.end())
                {
                    // VALUE 已被占用，删除旧映射
                    const K old_key = keys[value_it->second.key_idx];
                    DeleteByKey(old_key);
                    // value_it已经失效，但我们不再使用它
                }

                // 添加新映射
                return Add(key, value);
            }
        }

        // ============================================================
        // 容量操作
        // ============================================================

        /**
         * @brief 获取映射对的总数
         */
        int GetCount() const
        {
            return static_cast<int>(keys.size());
        }

        /**
         * @brief 检查是否为空
         */
        bool IsEmpty() const
        {
            return keys.empty();
        }

        /**
         * @brief 清空所有映射（但不释放内存）
         */
        void Clear()
        {
            keys.clear();
            values.clear();
            forward.clear();
            reverse.clear();
        }

        /**
         * @brief 清空所有映射并释放内存
         */
        void Free()
        {
            std::vector<K>().swap(keys);
            std::vector<V>().swap(values);
        }

        // ============================================================
        // 调试输出
        // ============================================================

        /**
         * @brief 输出完整的内部数据状态（用于调试）
         */
        void DebugDump(const char* label = nullptr) const
        {
            if (label)
                printf("\n========== %s ==========\n", label);
            else
                printf("\n========== BidirectionalMap Debug Dump ==========\n");

            printf("Count: %d\n", GetCount());
            printf("Keys size: %zu, Values size: %zu\n", keys.size(), values.size());

            printf("\nKeys vector:\n");
            for (size_t i = 0; i < keys.size(); ++i)
            {
                printf("  [%zu] = %d\n", i, (int)keys[i]);
            }

            printf("\nValues vector:\n");
            for (size_t i = 0; i < values.size(); ++i)
            {
                printf("  [%zu] = \"%s\"\n", i, values[i].c_str());
            }

            printf("\nForward map (K -> IndexPair):\n");
            for (const auto& [k, idx_pair] : forward)
            {
                printf("  key=%d -> (key_idx=%d, value_idx=%d)\n", (int)k, idx_pair.key_idx, idx_pair.value_idx);
            }

            printf("\nReverse map (V -> IndexPair):\n");
            for (const auto& [v, idx_pair] : reverse)
            {
                printf("  value=\"%s\" -> (key_idx=%d, value_idx=%d)\n", v.c_str(), idx_pair.key_idx, idx_pair.value_idx);
            }

            printf("==============================================\n\n");
        }

        // ============================================================
        // 枚举操作
        // ============================================================

        /**
         * @brief 枚举所有映射对
         * @param func 回调函数，签名为 void(const K& key, const V& value)
         */
        template<typename F>
        void EnumKV(F&& func) const
        {
            for (int i = 0; i < static_cast<int>(keys.size()); ++i)
            {
                func(keys[i], values[i]);
            }
        }

        /**
         * @brief 枚举所有 KEY
         * @param func 回调函数，签名为 void(const K& key)
         */
        template<typename F>
        void EnumKeys(F&& func) const
        {
            for (int i = 0; i < static_cast<int>(keys.size()); ++i)
            {
                func(keys[i]);
            }
        }

        /**
         * @brief 枚举所有 VALUE
         * @param func 回调函数，签名为 void(const V& value)
         */
        template<typename F>
        void EnumValues(F&& func) const
        {
            for (int i = 0; i < static_cast<int>(values.size()); ++i)
            {
                func(values[i]);
            }
        }

        // ============================================================
        // 序列化支持（KEY/VALUE向量本身就是序列化格式）
        // ============================================================

        /**
         * @brief 从 KEY/VALUE 数组重建双向映射（清空现有内容）
         * @param key_array KEY 数组
         * @param value_array VALUE 数组
         * @param count 元素数量（可选，-1 表示自动）
         * @return 成功重建的元素数量
         */
        template<typename KeyArray, typename ValueArray>
        int RebuildFromArrays(const KeyArray& key_array, const ValueArray& value_array, int count = -1)
        {
            Clear();

            int actual_count = std::min(GetArrayCount(key_array), GetArrayCount(value_array));
            if (count >= 0)
                actual_count = std::min(count, actual_count);

            for (int i = 0; i < actual_count; ++i)
            {
                if (!Add(GetArrayElement(key_array, i), GetArrayElement(value_array, i)))
                {
                    // 如果添加失败（冲突），回滚
                    Clear();
                    return 0;
                }
            }

            return actual_count;
        }

    protected:
        // 辅助函数

        template<typename T>
        static int GetArrayCount(const std::vector<T>& arr)
        {
            return static_cast<int>(arr.size());
        }

        template<typename T>
        static const T& GetArrayElement(const std::vector<T>& arr, int index)
        {
            return arr[index];
        }

        template<typename T>
        static const T& GetArrayElement(const T* arr, int index)
        {
            return arr[index];
        }
    };

}//namespace hgl
