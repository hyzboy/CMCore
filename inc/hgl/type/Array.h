#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/ObjectArray.h>

namespace hgl
{
    // 编译期选择底层容器类型
    template<typename T>
    using ArrayType = std::conditional_t<
        std::is_trivially_copyable_v<T>,
        DataArray<T>,
        ObjectArray<T>
    >;
}
