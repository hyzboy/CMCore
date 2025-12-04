#pragma once

namespace hgl
{
    //==================================================================================================
    // 辅助工具类 / Utility Classes
    //==================================================================================================
    
    template<typename C,typename V> struct ArrayWriter
    {
        C *count;
        V *values;

    public:

        ArrayWriter(C *c,V *v)
        {
            count = c;
            values = v;

            *count = 0;
        }

        ArrayWriter &operator <<(const V &v)
        {
            *values = v;
            ++values;
            ++(*count);

            return *this;
        }
    };
}//namespace hgl
