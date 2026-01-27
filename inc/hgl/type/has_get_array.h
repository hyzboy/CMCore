#pragma once

#include <type_traits>

namespace hgl
{
    /**
    * @brief CN:检测容器是否提供 GetArray() 方法。\nEN:Detects if container provides GetArray() method.
    * @tparam C CN:容器类型。EN:Container type.
    */
    template<typename C>
    class has_get_array
    {
        template<typename X> static auto test(int) -> decltype(std::declval<X>().GetArray(), std::true_type());
        template<typename>   static auto test(...) -> std::false_type;
    public:

        /**
        * @brief CN:是否支持 GetArray()。\nEN:Whether GetArray() is supported.
        */
        static constexpr bool value = decltype(test<C>(0))::value;
    };
}//namespace hgl

