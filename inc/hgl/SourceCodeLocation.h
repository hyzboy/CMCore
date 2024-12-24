#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    struct SourceCodeLocation
    {
        const char *    file;               ///<源文件
        size_t          line;               ///<行号
        const char *    func;               ///<函数
    };

    #define HGL_SOURCE_CODE_LOCATION    __FILE__,__LINE__,__HGL_FUNC__

    #define HGL_SCL_HERE    SourceCodeLocation(HGL_SOURCE_CODE_LOCATION)
}//namespace hgl
