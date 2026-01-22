#pragma once

#include<hgl/type/ArrayList.h>
#include<hgl/io/TextOutputStream.h>

namespace hgl
{
    // ==================== 改进版 ConstStringView ====================
    template<typename SC> struct ConstStringView
    {
        DataArray<SC> *str_data;    // 指向字符串数据池
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

        // 用于 SortedSet 的比较（只比较字符串内容）
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

        DataArray<SC> str_data;                         // 字符串数据池
        ArrayList<ConstStringView<SC>> str_list;        // 按 ID 顺序存储（值，不是指针）

    public:

        // ==================== 查询接口 ====================
        
        int GetCount() const { return str_list.GetCount(); }
        int GetTotalLength() const { return str_data.GetCount(); }
        int GetTotalBytes() const { return str_data.GetCount() * sizeof(SC); }
        bool IsEmpty() const { return str_data.IsEmpty(); }

        const DataArray<SC>& GetStringData() const { return str_data; }

        // ==================== 添加接口（简化） ====================
        
        // 返回值：字符串的 ID，失败返回 -1
        int Add(const SC *str, int length)
        {
            if(!str || length <= 0)
                return -1;

            // 检查是否已存在（线性查找）
            for(int i = 0; i < str_list.GetCount(); i++)
            {
                const auto& view = str_list[i];
                if(view.length == length)
                {
                    const SC* existing = str_data.GetData() + view.offset;
                    if(hgl::strcmp(existing, str, length) == 0)
                        return i;  // 返回已存在的 ID
                }
            }

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

            return new_id;
        }

        // 便捷方法：添加并返回 ConstStringView
        const ConstStringView<SC>* AddAndGet(const SC *str, int length)
        {
            int id = Add(str, length);
            return id >= 0 ? GetStringView(id) : nullptr;
        }

        // ==================== 查询接口 ====================
        
        bool Contains(const SC *str, int length) const
        {
            return GetID(str, length) >= 0;
        }

        int GetID(const SC *str, int length) const
        {
            if(!str || length <= 0)
                return -1;

            // 线性查找
            for(int i = 0; i < str_list.GetCount(); i++)
            {
                const auto& view = str_list[i];
                if(view.length == length)
                {
                    const SC* existing = str_data.GetData() + view.offset;
                    if(hgl::strcmp(existing, str, length) == 0)
                        return i;
                }
            }

            return -1;
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
        
        // ArrayList 使用原始指针作为迭代器
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
