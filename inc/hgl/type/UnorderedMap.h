#pragma once

#include<ankerl/unordered_dense.h>
#include<hgl/type/ValueArray.h>
#include<vector>

namespace hgl
{
    /**
     * 现代无序映射 - 混合架构（ankerl + 分离式序列化）
     *
     * 核心特性：
     * 1. 主存储：ankerl::unordered_dense::map（极致查找性能，5.5x vs ManagedPool）
     * 2. 序列化支持：按需生成 key/value 独立数组
     * 3. 快速恢复：从独立数组重建 map
     * 4. 兼容性：API 与原 UnorderedMap<K,V> 保持一致
     *
     * 使用场景：
     * - 正常操作：直接用 ankerl map（极致性能）
     * - 状态保存：GetKeyArray() + GetValueArray() 生成独立数组
     * - 状态恢复：RebuildFromArrays(keys, values) 快速重建
     *
     * @tparam K Key 类型（任意，trivially 或 non-trivially 都支持）
     * @tparam V Value 类型（任意，trivially 或 non-trivially 都支持）
     */
    template<typename K, typename V>
    class UnorderedMap
    {
    protected:
        // 主存储：ankerl 提供极致性能
        ankerl::unordered_dense::map<K, V> map_data;

    public:
        UnorderedMap() = default;
        virtual ~UnorderedMap() = default;

        // ============================================================
        // 基础 API（兼容 UnorderedMap<K,V> 接口）
        // ============================================================

        /**
         * 获取元素总数
         */
        int GetCount() const { return static_cast<int>(map_data.size()); }

        /**
         * 检查容器是否为空
         */
        bool IsEmpty() const { return map_data.empty(); }

        /**
         * 添加一个键值对
         * 如果 key 已存在，返回 false
         */
        bool Add(const K& key, const V& value) {
            auto [it, inserted] = map_data.try_emplace(key, value);
            return inserted;
        }

        /**
         * 获取指定 key 对应的 value
         */
        bool Get(const K& key, V& value) const {
            auto it = map_data.find(key);
            if (it == map_data.end()) return false;
            value = it->second;
            return true;
        }

        /**
         * 获取指定 key 对应的 value 指针（非 const 版本）
         */
        V* GetValuePointer(const K& key) {
            auto it = map_data.find(key);
            return (it != map_data.end()) ? &it->second : nullptr;
        }

        /**
         * 获取指定 key 对应的 value 指针（const 版本）
         */
        const V* GetValuePointer(const K& key) const {
            auto it = map_data.find(key);
            return (it != map_data.end()) ? &it->second : nullptr;
        }

        /**
         * 获取指定 key 对应的 value 引用（非 const 版本）
         * 注：调用者必须确保 key 存在，否则行为未定义
         */
        V& GetValueRef(const K& key) {
            auto it = map_data.find(key);
            return it->second;
        }

        /**
         * 获取指定 key 对应的 value 引用（const 版本）
         * 注：调用者必须确保 key 存在，否则行为未定义
         */
        const V& GetValueRef(const K& key) const {
            auto it = map_data.find(key);
            return it->second;
        }

        /**
         * 根据 key 删除元素
         */
        bool DeleteByKey(const K& key) {
            return map_data.erase(key) > 0;
        }

        /**
         * 检查是否包含指定 key
         */
        bool ContainsKey(const K& key) const {
            return map_data.contains(key);
        }

        /**
         * 更改或添加（如果存在则更新，不存在则添加）
         */
        bool ChangeOrAdd(const K& key, const V& value) {
            map_data[key] = value;
            return true;
        }

        /**
         * 更改指定 key 的 value（如果 key 不存在则返回 false）
         */
        bool Change(const K& key, const V& value) {
            auto it = map_data.find(key);
            if (it == map_data.end()) return false;
            it->second = value;
            return true;
        }

        /**
         * 清空所有元素（但不释放内存）
         */
        void Clear() {
            map_data.clear();
        }

        /**
         * 清空并释放内存
         */
        void Free() {
            ankerl::unordered_dense::map<K, V> empty;
            map_data.swap(empty);
        }

        // ============================================================
        // 迭代器支持（支持 range-based for 循环）
        // ============================================================

        /**
         * @brief 获取迭代器起始位置
         * @note 支持 for(auto [k, v] : map) 语法
         */
        auto begin() {
            return map_data.begin();
        }

        /**
         * @brief 获取迭代器结束位置
         */
        auto end() {
            return map_data.end();
        }

        /**
         * @brief 获取 const 迭代器起始位置
         */
        auto begin() const {
            return map_data.begin();
        }

        /**
         * @brief 获取 const 迭代器结束位置
         */
        auto end() const {
            return map_data.end();
        }

        /**
         * @brief 获取 const 迭代器起始位置
         */
        auto cbegin() const {
            return map_data.cbegin();
        }

        /**
         * @brief 获取 const 迭代器结束位置
         */
        auto cend() const {
            return map_data.cend();
        }

        // ============================================================
        // 序列化支持：分离式 Key/Value 访问
        // ============================================================

        /**
         * 获取所有 Keys 到独立数组（用于快速序列化）
         * 支持 ValueArray<K>、std::vector<K> 等
         *
         * @param key_array 输出的 key 数组
         * @return key 数量
         */
        template<typename KeyArray>
        int GetKeyArray(KeyArray& key_array) const {
            ClearContainer(key_array);
            const int count = GetCount();
            if (count > 0) {
                ReserveContainer(key_array, count);

                for (const auto& [key, value] : map_data) {
                    AddToContainer(key_array, key);
                }
            }
            return (int)GetContainerSize(key_array);
        }

        /**
         * 获取所有 Values 到独立数组（用于快速序列化）
         * 支持 ValueArray<V>、std::vector<V> 等
         *
         * @param value_array 输出的 value 数组
         * @return value 数量
         */
        template<typename ValueArray>
        int GetValueArray(ValueArray& value_array) const {
            ClearContainer(value_array);
            const int count = GetCount();
            if (count > 0) {
                ReserveContainer(value_array, count);

                for (const auto& [key, value] : map_data) {
                    AddToContainer(value_array, value);
                }
            }
            return (int)GetContainerSize(value_array);
        }

        /**
         * 同时获取 Key 和 Value 数组（保证顺序一致）
         * 这是保存状态时最高效的方式
         *
         * @param key_array 输出的 key 数组
         * @param value_array 输出的 value 数组
         * @return 元素数量
         */
        template<typename KeyArray, typename ValueArray>
        int GetKeyValueArrays(KeyArray& key_array, ValueArray& value_array) const {
            ClearContainer(key_array);
            ClearContainer(value_array);

            const int count = GetCount();
            if (count <= 0) return 0;

            ReserveContainer(key_array, count);
            ReserveContainer(value_array, count);

            for (const auto& [key, value] : map_data) {
                AddToContainer(key_array, key);
                AddToContainer(value_array, value);
            }

            return count;
        }

        /**
         * 从独立的 Key/Value 数组重建 Map（用于快速反序列化）
         * 此操作会先清空现有内容
         *
         * @param keys Key 数组（可以是 ValueArray<K>、std::vector<K> 等）
         * @param values Value 数组
         * @param count 元素数量（可选，默认使用数组大小，-1 表示自动）
         * @return 成功重建的元素数量
         */
        template<typename KeyArray, typename ValueArray>
        int RebuildFromArrays(const KeyArray& keys, const ValueArray& values, int count = -1) {
            Clear();

            // 确定实际数量
            int key_count = GetArrayCount(keys);
            int value_count = GetArrayCount(values);
            int actual_count = (count < 0) ? std::min(key_count, value_count) :
                              std::min(count, std::min(key_count, value_count));

            if (actual_count <= 0) return 0;

            // 预分配空间
            map_data.reserve(actual_count);

            // 重建 map
            for (int i = 0; i < actual_count; ++i) {
                map_data[GetArrayElement(keys, i)] = GetArrayElement(values, i);
            }

            return actual_count;
        }

        /**
         * 批量添加（从独立数组）
         * 与 RebuildFromArrays 不同，此函数不会清空现有内容，只添加新元素
         *
         * @param keys Key 数组
         * @param values Value 数组
         * @param count 元素数量（可选）
         * @return 实际添加的元素数量（可能少于提供的数量，如果有 key 冲突）
         */
        template<typename KeyArray, typename ValueArray>
        int AddFromArrays(const KeyArray& keys, const ValueArray& values, int count = -1) {
            int key_count = GetArrayCount(keys);
            int value_count = GetArrayCount(values);
            int actual_count = (count < 0) ? std::min(key_count, value_count) :
                              std::min(count, std::min(key_count, value_count));

            int added = 0;
            for (int i = 0; i < actual_count; ++i) {
                if (Add(GetArrayElement(keys, i), GetArrayElement(values, i))) {
                    ++added;
                }
            }

            return added;
        }

        // ============================================================
        // 枚举接口（兼容原 Map API）
        // ============================================================

        /**
         * 枚举所有键值对
         */
        template<typename F>
        void EnumKV(F&& func) {
            for (auto& [key, value] : map_data) {
                func(key, value);
            }
        }

        /**
         * 枚举所有键值对（const 版本）
         */
        template<typename F>
        void EnumKV(F&& func) const {
            for (const auto& [key, value] : map_data) {
                func(key, value);
            }
        }

        /**
         * 枚举所有键
         */
        template<typename F>
        void EnumKeys(F&& func) {
            for (auto& [key, value] : map_data) {
                func(key);
            }
        }

        /**
         * 枚举所有键（const 版本）
         */
        template<typename F>
        void EnumKeys(F&& func) const {
            for (const auto& [key, value] : map_data) {
                func(key);
            }
        }

        /**
         * 枚举所有值
         */
        template<typename F>
        void EnumValues(F&& func) {
            for (auto& [key, value] : map_data) {
                func(value);
            }
        }

        /**
         * 枚举所有值（const 版本）
         */
        template<typename F>
        void EnumValues(F&& func) const {
            for (const auto& [key, value] : map_data) {
                func(value);
            }
        }

        // ============================================================
        // 兼容旧 API（用于与 UnorderedMap<K,V> 通用代码）
        // ============================================================

        /**
         * 获取所有 key 列表（兼容旧 API）
         */
        template<typename IT>
        int GetKeyList(IT& il_list) {
            return GetKeyArray(il_list);
        }

        /**
         * 获取所有 value 列表（兼容旧 API）
         */
        template<typename IT>
        int GetValueList(IT& il_list) {
            return GetValueArray(il_list);
        }

        /**
         * 同时获取 key 和 value 列表（兼容旧 API）
         */
        template<typename ITK, typename ITV>
        int GetList(ITK& key_list, ITV& value_list) {
            return GetKeyValueArrays(key_list, value_list);
        }

    protected:
        // ============================================================
        // 辅助函数：处理不同类型的数组/容器
        // ============================================================

        // -------- 清空容器 --------

        /**
         * 清空 ValueArray<T>
         */
        template<typename T>
        static void ClearContainer(ValueArray<T>& arr) {
            arr.Clear();
        }

        /**
         * 清空 std::vector<T>
         */
        template<typename T>
        static void ClearContainer(std::vector<T>& vec) {
            vec.clear();
        }

        // -------- 预留容量 --------

        /**
         * 为 ValueArray<T> 预留空间
         */
        template<typename T>
        static void ReserveContainer(ValueArray<T>& arr, int capacity) {
            arr.SetMaxCount(capacity);
        }

        /**
         * 为 std::vector<T> 预留空间
         */
        template<typename T>
        static void ReserveContainer(std::vector<T>& vec, int capacity) {
            vec.reserve(capacity);
        }

        // -------- 添加元素 --------

        /**
         * 向 ValueArray<T> 添加元素
         */
        template<typename T>
        static void AddToContainer(ValueArray<T>& arr, const T& value) {
            arr.Add(value);
        }

        /**
         * 向 std::vector<T> 添加元素
         */
        template<typename T>
        static void AddToContainer(std::vector<T>& vec, const T& value) {
            vec.push_back(value);
        }

        // -------- 获取容器大小 --------

        /**
         * 获取 ValueArray<T> 的大小
         */
        template<typename T>
        static size_t GetContainerSize(const ValueArray<T>& arr) {
            return arr.GetCount();
        }

        /**
         * 获取 std::vector<T> 的大小
         */
        template<typename T>
        static size_t GetContainerSize(const std::vector<T>& vec) {
            return vec.size();
        }

        // -------- 原有的数组访问函数 --------

        /**
         * 获取 ValueArray<T> 的大小
         */
        template<typename T>
        static int GetArrayCount(const ValueArray<T>& arr) {
            return arr.GetCount();
        }

        /**
         * 获取 ValueArray<T> 的指定索引元素
         */
        template<typename T>
        static const T& GetArrayElement(const ValueArray<T>& arr, int index) {
            return arr[index];
        }

        /**
         * 获取 std::vector<T> 的大小
         */
        template<typename T>
        static int GetArrayCount(const std::vector<T>& arr) {
            return static_cast<int>(arr.size());
        }

        /**
         * 获取 std::vector<T> 的指定索引元素
         */
        template<typename T>
        static const T& GetArrayElement(const std::vector<T>& arr, int index) {
            return arr[index];
        }

        /**
         * 获取 C 数组指针的元素（需要外部传递 count）
         */
        template<typename T>
        static const T& GetArrayElement(const T* arr, int index) {
            return arr[index];
        }
    };

}//namespace hgl
