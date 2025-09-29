#pragma once

#include<hgl/type/String.h>

namespace hgl
{
    /**
     * 映射一个文件到内存（仅保存共用数据）
     */
    class MMapFile
    {
    public:
        // 错误码
        enum class Error
        {
            Ok = 0,
            InvalidArgument,
            OpenFileFailed,
            GetFileSizeFailed,
            SetFileSizeFailed,
            CreateMappingFailed,
            MapViewFailed
        };

    protected:
        void*  data_ = nullptr;
        size_t size_ = 0;

    public:
        MMapFile() = default;
        virtual ~MMapFile();

        void *data();
        size_t size() const;

        MMapFile(const MMapFile&) = delete;
        MMapFile& operator=(const MMapFile&) = delete;
    };

    // 类之外的独立便捷函数：成功返回 MMapFile*，失败返回 nullptr；可选输出错误码
    MMapFile* CreateMMapFile(const OSString &filename, size_t size, MMapFile::Error *err=nullptr);
    MMapFile* OpenMMapFile(const OSString &filename, size_t size, MMapFile::Error *err=nullptr);
    MMapFile* OpenMMapFileOnlyRead(const OSString &filename, MMapFile::Error *err=nullptr);
} // namespace hgl
