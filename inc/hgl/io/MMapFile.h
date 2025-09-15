#pragma once

#include<hgl/type/String.h>

namespace hgl
{
    /**
     * 映射一个文件到内存
     */
    class MMapFile
    {
        void* impl_ = nullptr; // pointer to platform-specific implementation struct
        void* data_ = nullptr;
        size_t size_ = 0;

    public:

        MMapFile(const OSString &filename, size_t size, bool readOnly = true);
        ~MMapFile();

        void *data();
        size_t size() const;

        // non-copyable
        MMapFile(const MMapFile&) = delete;
        MMapFile& operator=(const MMapFile&) = delete;
    };
} // namespace hgl
