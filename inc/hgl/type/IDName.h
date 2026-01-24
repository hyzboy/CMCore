#pragma once

#include<hgl/type/ConstStringSet.h>
#include<hgl/type/OrderedManagedSet.h>

namespace hgl
{
    /**
     * ID-Name 注册表模板
     * 为每个 MANAGER 类型维护一个独立的 ConstStringSet 实例
     */
    template<typename MANAGER, typename SC>
    class IDNameRegistry
    {
    private:
        static ConstStringSet<SC>* GetInstance()
        {
            static ConstStringSet<SC> instance;
            return &instance;
        }

    public:
        // 注册一个名称，返回其 ID
        static int Register(const SC *name_string, int name_length)
        {
            if(!name_string || name_length <= 0)
                return -1;

            return GetInstance()->Add(name_string, name_length);
        }

        // 根据 ID 获取名称字符串
        static const SC* GetName(int id)
        {
            if(id < 0)
                return nullptr;

            return GetInstance()->GetString(id);
        }

        // 根据 ID 获取 ConstStringView
        static const ConstStringView<SC>* GetView(int id)
        {
            if(id < 0)
                return nullptr;

            return GetInstance()->GetStringView(id);
        }

        // 根据名称获取 ID（查找已存在的）
        static int GetID(const SC *name_string, int name_length)
        {
            if(!name_string || name_length <= 0)
                return -1;

            return GetInstance()->GetID(name_string, name_length);
        }

        // 检查名称是否已注册
        static bool Contains(const SC *name_string, int name_length)
        {
            return GetID(name_string, name_length) >= 0;
        }

        // 获取注册的名称总数
        static int GetCount()
        {
            return GetInstance()->GetCount();
        }

        // 清空所有注册的名称
        static void Clear()
        {
            GetInstance()->Clear();
        }
    };

    /**
    * 顺序ID+名称数据结构模板<br>
    * 按添加进来的名字先后顺序一个个产生连续的序号，所有数据只可读不可写
    * 
    * 设计意图：
    * - 通过 ConstStringSet 统计所有不重复的字符串
    * - 比较时直接使用序列号（ID）进行比较以提升速度
    * - 保存时使用字符串
    */
    template<typename SC, typename MANAGER> 
    class OrderedIDName
    {
    public:

        using SelfClass = OrderedIDName<SC, MANAGER>;
        using Registry = IDNameRegistry<MANAGER, SC>;

    private:

        int id;  // 存储字符串的 ID

    public:

        // ==================== 静态方法 ====================

        static size_t GetClassHC()
        {
            static SelfClass self;
            static size_t class_hc = typeid(self).hash_code();
            return class_hc;
        }

        // ==================== 访问器 ====================

        int GetID() const { return id; }

        const SC* GetName() const
        {
            return Registry::GetName(id);
        }

        int GetNameLength() const
        {
            if(id < 0)
                return 0;

            const auto* view = Registry::GetView(id);
            return view ? view->length : 0;
        }

        const SC* ToString() const { return GetName(); }

        bool IsValid() const { return id >= 0; }

        // ==================== 构造与赋值 ====================

        OrderedIDName() : id(-1) {}

        OrderedIDName(const SC *name_string)
        {
            if(name_string && *name_string)
                id = Registry::Register(name_string, hgl::strlen(name_string));
            else
                id = -1;
        }

        OrderedIDName(const SC *name_string, int name_length)
        {
            if(name_string && name_length > 0)
                id = Registry::Register(name_string, name_length);
            else
                id = -1;
        }

        OrderedIDName(const SelfClass &other) : id(other.id) {}

        // ==================== 赋值操作符 ====================

        void operator=(const SC *name_string)
        {
            if(name_string && *name_string)
                id = Registry::Register(name_string, hgl::strlen(name_string));
            else
                id = -1;
        }

        void operator=(const String<SC> &name_string)
        {
            if(!name_string.IsEmpty())
                id = Registry::Register(name_string.c_str(), name_string.Length());
            else
                id = -1;
        }

        void operator=(const SelfClass &other)
        {
            id = other.id;
        }

        // ==================== 比较操作符 ====================

        std::strong_ordering operator<=>(const OrderedIDName &other) const
        {
            return id <=> other.id;
        }

        bool operator==(const OrderedIDName &other) const
        {
            return id == other.id;
        }

        // ==================== 辅助方法 ====================

        void Clear()
        {
            id = -1;
        }

        // 设置名称
        void Set(const SC *name_string, int name_length)
        {
            if(name_string && name_length > 0)
                id = Registry::Register(name_string, name_length);
            else
                id = -1;
        }

    };//class OrderedIDName

    // ==================== 宏定义 ====================

    #define HGL_DEFINE_IDNAME(name, char_type)   \
        struct IDName##_##name##_Manager{}; \
        using name = hgl::OrderedIDName<char_type, IDName##_##name##_Manager>; \
        using name##Set = hgl::OrderedManagedSet<name>;

    #define HGL_DEFINE_ANSI_IDNAME(name)    HGL_DEFINE_IDNAME(name, char)
    #define HGL_DEFINE_WIDE_IDNAME(name)    HGL_DEFINE_IDNAME(name, wchar_t)
    #define HGL_DEFINE_U8_IDNAME(name)      HGL_DEFINE_IDNAME(name, u8char)
    #define HGL_DEFINE_U16_IDNAME(name)     HGL_DEFINE_IDNAME(name, u16char)
    #define HGL_DEFINE_OS_IDNAME(name)      HGL_DEFINE_IDNAME(name, os_char)

}//namespace hgl
