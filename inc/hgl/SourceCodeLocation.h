#pragma once

namespace hgl
{
    struct SourceCodeLocation
    {
        const char *    file;               ///<源文件
        const char *    func;               ///<函数
        size_t          line;               ///<行号
    };

    #define HGL_SOURCE_CODE_LOCATION    __FILE__,__FUNCTION__,__LINE__

    #define HGL_SCL_HERE    SourceCodeLocation(HGL_SOURCE_CODE_LOCATION)
}//namespace hgl
