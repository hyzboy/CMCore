#pragma once
#include<hgl/type/DataType.h>

namespace hgl
{
    /**
     * 基础对象
     */
    class Object
    {
    public:

        NO_COPY(Object)
        NO_MOVE(Object)

        Object()=default;
        virtual ~Object()=default;
    };//class Object
}//namespace hgl
