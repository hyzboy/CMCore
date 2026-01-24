#pragma once

#include<hgl/type/ValueArray.h>
#include<hgl/type/HashIDMap.h>
#include<hgl/io/TextOutputStream.h>

namespace hgl
{
    // ==================== 改进版 ConstStringView ====================
    template<typename SC> struct ConstStringView
    {
        ValueBuffer<SC> *str_data;    // 指向字符串数据池
        int id;                      // 顺序号
        int length;                  // 字符串长度
        size_t offset;              // 在数据池中的偏移

    public:

        ConstStringView() : str_data(nullptr), id(-1), length(0), offset(0) {}

        const SC *GetString() const
        {
            return str_data ? str_data->GetData() + offset : nullptr;
        }

        size_t GetLength() const { return length; }

        // 用于 OrderedValueSet 的比较（只比较字符串内容）
        std::strong_ordering operator<=>(const ConstStringView<SC> &other) const
        {
            if(length != other.length)
                return length <=> other.length;

            const SC *s1 = GetString();
            const SC *s2 = other.GetString();

            if(!s1 || !s2)
                return s1 <=> s2;

            return hgl::strcmp(s1, s2, length) <=> 0;
        }

        bool operator==(const ConstStringView<SC> &other) const
        {
            if(length != other.length)
                return false;

            const SC *s1 = GetString();
            const SC *s2 = other.GetString();

            return s1 && s2 && hgl::strcmp(s1, s2, length) == 0;
        }
    };

    // ==================== 改进版 ConstStringSet ====================
    template<typename SC> class ConstStringSet
    {
    private:

        ValueBuffer<SC> str_data;                         // 字符串数据池
        ValueArray<ConstStringView<SC>> str_list;        // 按 ID 顺序存储（值，不是指针）

        // ==================== 哈希优化（使用独立的哈希映射管理器） ====================
        static constexpr int MAX_COLLISION_PER_HASH = 4;  // 每个哈希值最多4个碰撞（可调整）
        HashIDMap<MAX_COLLISION_PER_HASH> hash_id_map;

        // 计算字符串的哈希值（FNV-1a 算法）
        static uint64 ComputeHash(const SC *str, int length) {
            uint64 hash = 14695981039346656037ULL;  // FNV offset basis
            for(int i = 0; i < length; i++) {
                hash ^= (uint64)str[i];
                hash *= 1099511628211ULL;  // FNV prime
            }
            return hash;
        }

        // 验证哈希碰撞时的真实字符串是否匹配
        bool VerifyMatch(int id, const SC *str, int length) const {
            if(id < 0 || id >= str_list.GetCount())
                return false;

            const auto& view = str_list[id];
            if(view.length != length)
                return false;

            const SC* stored = str_data.GetData() + view.offset;
            return hgl::strcmp(stored, str, length) == 0;
        }

    public:

        // ==================== 查询接口 ====================

        int GetCount() const { return str_list.GetCount(); }
        int GetTotalLength() const { return str_data.GetCount(); }
        int GetTotalBytes() const { return str_data.GetCount() * sizeof(SC); }
        bool IsEmpty() const { return str_data.IsEmpty(); }

        const ValueBuffer<SC>& GetStringData() const { return str_data; }

        // ==================== 添加接口（优化后） ====================

        // 返回值：字符串的 ID，失败返回 -1
        int Add(const SC *str, int length)
        {
            if(!str || length <= 0)
                return -1;

            uint64 hash = ComputeHash(str, length);

            // 快速检查：是否已存在（使用哈希表）
            int found_id = GetID(str, length);
            if(found_id >= 0)
                return found_id;  // 返回已存在的 ID

            // 分配新 ID
            const int new_id = str_list.GetCount();

            // 添加到数据池
            const size_t offset = str_data.GetCount();
            str_data.Expand(length + 1);

            SC *save_str = str_data.GetData() + offset;
            mem_copy<SC>(save_str, str, length);
            save_str[length] = 0;

            // 创建 ConstStringView
            ConstStringView<SC> view;
            view.str_data = &str_data;
            view.id = new_id;
            view.length = length;
            view.offset = offset;

            // 添加到列表
            str_list.Add(view);

            // 添加到哈希映射（使用独立的管理器）
            hash_id_map.Add(hash, new_id);

            return new_id;
        }

        // 便捷方法：添加并返回 ConstStringView
        const ConstStringView<SC>* AddAndGet(const SC *str, int length)
        {
            int id = Add(str, length);
            return id >= 0 ? GetStringView(id) : nullptr;
        }

        const ConstStringView<SC> *AddAndGet(const StringView<SC> &sv)
        {
            return AddAndGet(sv.c_str(), sv.Length());
        }

        // ==================== 查询接口（优化后） ====================

        bool Contains(const SC *str, int length) const
        {
            return GetID(str, length) >= 0;
        }

        int GetID(const SC *str, int length) const
        {
            if(!str || length <= 0)
                return -1;

            uint64 hash = ComputeHash(str, length);

            // 使用独立的哈希映射管理器查找
            return hash_id_map.Find(hash, [this, str, length](int id) {
                return VerifyMatch(id, str, length);
            });
        }

        const SC* GetString(int id) const
        {
            if(id < 0 || id >= GetCount())
                return nullptr;

            return str_list[id].GetString();
        }

        const ConstStringView<SC>* GetStringView(int id) const
        {
            if(id < 0 || id >= GetCount())
                return nullptr;

            return &str_list[id];
        }

        const ConstStringView<SC>* operator[](int id) const
        {
            return GetStringView(id);
        }

        // ==================== 迭代器（改进） ====================

        // ValueArray 使用原始指针作为迭代器
        const ConstStringView<SC>* begin() const { return str_list.begin(); }
        const ConstStringView<SC>* end() const { return str_list.end(); }

        // ==================== 生命周期管理 ====================

        ConstStringSet() = default;
        ~ConstStringSet() = default;

        // 禁用拷贝（如果需要，可以实现）
        ConstStringSet(const ConstStringSet&) = delete;
        ConstStringSet& operator=(const ConstStringSet&) = delete;

        // 移动语义
        ConstStringSet(ConstStringSet&&) = default;
        ConstStringSet& operator=(ConstStringSet&&) = default;

        void Clear()
        {
            str_data.Clear();
            str_list.Clear();
            hash_id_map.Clear();
        }

        // ==================== 批量操作 ====================

        void Reserve(int count)
        {
            str_list.Reserve(count);
        }

        // 批量添加
        template<typename Container>
        void AddRange(const Container& strings)
        {
            for(const auto& str : strings)
                Add(str.c_str(), str.length());
        }

        // ==================== 哈希统计接口（性能分析 - 委托给 HashIDMap） ====================

        // 获取哈希碰撞次数
        int GetCollisionCount() const {
            return hash_id_map.GetCollisionCount();
        }

        // 获取哈希表负载因子（用于性能分析）
        float GetLoadFactor() const {
            return hash_id_map.GetLoadFactor(str_list.GetCount());
        }

        // 获取平均碰撞链长度（用于性能分析）
        float GetAverageCollisionChainLength() const {
            return hash_id_map.GetAverageCollisionChainLength();
        }

        // 获取碰撞槽位溢出次数（性能监控）
        int GetCollisionOverflowCount() const {
            return hash_id_map.GetCollisionOverflowCount();
        }

        // 获取最大碰撞槽位大小（配置参数）
        static constexpr int GetMaxCollisionPerHash() {
            return MAX_COLLISION_PER_HASH;
        }
    };

    // ==================== 类型别名 ====================

    using ConstAnsiStringSet = ConstStringSet<char>;
    using ConstWideStringSet = ConstStringSet<wchar_t>;
    using ConstU8StringSet = ConstStringSet<u8char>;
    using ConstU16StringSet = ConstStringSet<u16char>;
    using ConstOSStringSet = ConstStringSet<os_char>;

    using ConstAnsiStringView = ConstStringView<char>;
    using ConstWideStringView = ConstStringView<wchar_t>;
    using ConstU8StringView = ConstStringView<u8char>;
    using ConstU16StringView = ConstStringView<u16char>;
    using ConstOSStringView = ConstStringView<os_char>;

    // ==================== 辅助函数 ====================

    template<typename SC>
    bool SaveToTextStream(io::TextOutputStream *tos, const ConstStringSet<SC> *css, bool output_id = false);

    template<typename SC>
    bool SaveToTextFile(const OSString &filename, const ConstStringSet<SC> *css, bool output_id = false, bool output_bom = true);
}//namespace hgl
