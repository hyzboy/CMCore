#pragma once

#include<typeinfo>
#include<type_traits>

namespace hgl
{
    template<typename T> inline constexpr const size_t GetTypeHash()noexcept{return typeid(T).hash_code();} ///<取得一个类型的哈希值
    template<typename T> inline constexpr const char * GetTypeName()noexcept{return typeid(T).name();}      ///<取得一个类型的名称
    
    // 1. 判断是否是类或结构体
    template<typename T>
    constexpr bool IsClassOrStruct() noexcept
    {
        return std::is_class_v<T>;
    }

    // 2. 判断是否有平凡(trivial)的默认构造函数
    template<typename T>
    constexpr bool HasTrivialConstructor() noexcept
    {
        return std::is_trivially_default_constructible_v<T>;
    }

    // 3. 判断是否有平凡的析构函数
    template<typename T>
    constexpr bool HasTrivialDestructor() noexcept
    {
        return std::is_trivially_destructible_v<T>;
    }

    // 4. 判断是否是平凡类型(Trivial Type) - C++20推荐用法
    template<typename T>
    constexpr bool IsTrivial() noexcept
    {
        return std::is_trivial_v<T>;
    }

    // 5. 判断是否是标准布局类型(Standard Layout Type)
    template<typename T>
    constexpr bool IsStandardLayout() noexcept
    {
        return std::is_standard_layout_v<T>;
    }

    // 6. 判断是否是POD类型(Plain Old Data) - 同时满足trivial和standard layout
    // 注意：C++20已弃用is_pod，使用组合判断代替
    template<typename T>
    constexpr bool IsPODType() noexcept
    {
        return std::is_trivial_v<T>&&std::is_standard_layout_v<T>;
    }

    // 7. 判断是否可以平凡复制
    template<typename T>
    constexpr bool IsTriviallyCopyable() noexcept
    {
        return std::is_trivially_copyable_v<T>;
    }

    // 8. 判断是否有自定义的析构函数
    template<typename T>
    constexpr bool HasCustomDestructor() noexcept
    {
        return !std::is_trivially_destructible_v<T>&&std::is_destructible_v<T>;
    }

    // 9. 判断是否可以默认构造
    template<typename T>
    constexpr bool IsDefaultConstructible() noexcept
    {
        return std::is_default_constructible_v<T>;
    }

    // 10. 判断是否可以移动构造
    template<typename T>
    constexpr bool IsMoveConstructible() noexcept
    {
        return std::is_move_constructible_v<T>;
    }

    // 11. 判断是否可以移动赋值
    template<typename T>
    constexpr bool IsMoveAssignable() noexcept
    {
        return std::is_move_assignable_v<T>;
    }

    // 12. 判断是否是聚合类型 (C++17+)
    template<typename T>
    constexpr bool IsAggregate() noexcept
    {
        return std::is_aggregate_v<T>;
    }
}//namespace hgl
