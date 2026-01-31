#pragma once

#include<vector>
#include<hgl/type/ValueArray.h>
#include<hgl/util/hash/QuickHash.h>
#include<absl/container/flat_hash_map.h>
#include<hgl/io/TextOutputStream.h>

namespace hgl
{
    // ==================== 改进版 ConstStringView ====================
    template<typename SC> struct ConstStringView
    {
        std::vector<SC> *str_data;    // 指向字符串数据池
        int id;                      // 顺序号
        int length;                  // 字符串长度
        size_t offset;              // 在数据池中的偏移

    public:

        ConstStringView() : str_data(nullptr), id(-1), length(0), offset(0) {}

        const SC *GetString() const
        {
            return str_data ? str_data->data() + offset : nullptr;
        }

        size_t GetLength() const { return length; }

        // 用于 FlatOrderedSet 的比较（只比较字符串内容）
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

        std::vector<SC> str_data;                         // 字符串数据池
        std::vector<ConstStringView<SC>> str_list;       // 按 ID 顺序存储（值，不是指针）

        // ==================== 哈希优化（使用absl::flat_hash_map） ====================
        absl::flat_hash_map<uint64, std::vector<int>> hash_id_map;

        // 验证哈希碰撞时的真实字符串是否匹配
        bool VerifyMatch(int id, const SC *str, int length) const {
            if(id < 0 || id >= (int)str_list.size())
                return false;

            const auto& view = str_list[id];
            if(view.length != length)
                return false;

            const SC* stored = str_data.data() + view.offset;
            return hgl::strcmp(stored, str, length) == 0;
        }

    public:

        // ==================== 查询接口 ====================

        int GetCount() const { return (int)str_list.size(); }
        int GetTotalLength() const { return (int)str_data.size(); }
        int GetTotalBytes() const { return (int)(str_data.size() * sizeof(SC)); }
        bool IsEmpty() const { return str_data.empty(); }

        const std::vector<SC>& GetStringData() const { return str_data; }

        // ==================== 添加接口（优化后） ====================

        // 返回值：字符串的 ID，失败返回 -1
        int Add(const SC *str, int length)
        {
            if(!str || length <= 0)
                return -1;

            uint64 hash = ComputeOptimalHash(str, length);

            // 快速检查：是否已存在（使用哈希表）
            int found_id = GetID(str, length);
            if(found_id >= 0)
                return found_id;  // 返回已存在的 ID

            // 分配新 ID
            const int new_id = (int)str_list.size();

            // 添加到数据池
            const size_t offset = str_data.size();
            str_data.resize(offset + length + 1);

            SC *save_str = str_data.data() + offset;
            mem_copy<SC>(save_str, str, length);
            save_str[length] = 0;

            // 创建 ConstStringView
            ConstStringView<SC> view;
            view.str_data = &str_data;
            view.id = new_id;
            view.length = length;
            view.offset = offset;

            // 添加到列表
            str_list.push_back(view);

            // 添加到哈希映射（使用独立的管理器）
            hash_id_map[hash].push_back(new_id);

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

            uint64 hash = ComputeOptimalHash(str, length);

            auto it = hash_id_map.find(hash);
            if (it == hash_id_map.end())
                return -1;

            for (int id : it->second) {
                if (VerifyMatch(id, str, length))
                    return id;
            }
            return -1;
        }

        const SC* GetString(int id) const
        {
            if(id < 0 || id >= (int)GetCount())
                return nullptr;

            return str_list[id].GetString();
        }

        const ConstStringView<SC>* GetStringView(int id) const
        {
            if(id < 0 || id >= (int)GetCount())
                return nullptr;

            return &str_list[id];
        }

        const ConstStringView<SC>* operator[](int id) const
        {
            return GetStringView(id);
        }

        // ==================== 迭代器（改进） ====================

        // 返回 std::vector 的迭代器
        auto begin() const { return str_list.begin(); }
        auto end() const { return str_list.end(); }

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
            str_data.clear();
            str_list.clear();
            hash_id_map.clear();
        }

        // ==================== 批量操作 ====================

        void Reserve(int count)
        {
            str_list.reserve(count);
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
            return hash_id_map.GetLoadFactor((int)str_list.size());
        }

        // 获取平均碰撞链长度（用于性能分析）
        float GetAverageCollisionChainLength() const {
            return hash_id_map.GetAverageCollisionChainLength();
        }

        // 获取碰撞槽位溢出次数（性能监控）
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
