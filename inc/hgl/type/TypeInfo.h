#pragma once

#include<typeinfo>

namespace hgl
{
    template<typename T> inline constexpr const size_t GetTypeHash()noexcept{return typeid(T).hash_code();} ///<取得一个类型的哈希值
    template<typename T> inline constexpr const char * GetTypeName()noexcept{return typeid(T).name();}      ///<取得一个类型的名称
}//namespace hgl
