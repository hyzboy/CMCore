#if !defined(_WIN32)
#error "mmap_win.cpp should only be built on Windows"
#endif

#include <hgl/io/MMapFile.h>
#include <windows.h>
#include <string>

namespace hgl
{
    class MMapFileImpl
    {
    public:
        HANDLE hFile=INVALID_HANDLE_VALUE;
        HANDLE hMap=nullptr;
    };

    // Windows 平台派生类：管理句柄与清理
    class WinMMapFile final : public MMapFile
    {
        MMapFileImpl impl_{};
    public:
        WinMMapFile() = default;
        ~WinMMapFile() override
        {
            if (data_) { UnmapViewOfFile(data_); data_ = nullptr; }
            if (impl_.hMap) { CloseHandle(impl_.hMap); impl_.hMap = nullptr; }
            if (impl_.hFile != INVALID_HANDLE_VALUE) { CloseHandle(impl_.hFile); impl_.hFile = INVALID_HANDLE_VALUE; }
        }

        static WinMMapFile* Create(const OSString &filename,
                                   DWORD desiredAccess,
                                   DWORD shareMode,
                                   DWORD creationDisposition,
                                   DWORD protect,
                                   DWORD mapAccess,
                                   size_t requestedSize,
                                   bool useFileSize,
                                   bool ensureAtLeastRequested,
                                   MMapFile::Error &outErr)
        {
            outErr = MMapFile::Error::Ok;
            if ((useFileSize && requestedSize==0)==false && !useFileSize && requestedSize==0)
            {
                // creation with size==0 is invalid
                outErr = MMapFile::Error::InvalidArgument;
                return nullptr;
            }

            WinMMapFile *mm = new WinMMapFile();

            mm->impl_.hFile = CreateFileW(filename.c_str(), desiredAccess, shareMode, nullptr, creationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (mm->impl_.hFile == INVALID_HANDLE_VALUE)
            {
                outErr = MMapFile::Error::OpenFileFailed;
                delete mm; return nullptr;
            }

            size_t actualSize = requestedSize;

            if (useFileSize)
            {
                LARGE_INTEGER fileSize{};
                if (!GetFileSizeEx(mm->impl_.hFile, &fileSize))
                {
                    outErr = MMapFile::Error::GetFileSizeFailed;
                    delete mm; return nullptr;
                }
                actualSize = static_cast<size_t>(fileSize.QuadPart);

                if (ensureAtLeastRequested && requestedSize > actualSize)
                {
                    LARGE_INTEGER newSize; newSize.QuadPart = static_cast<LONGLONG>(requestedSize);
                    if (!SetFilePointerEx(mm->impl_.hFile, newSize, nullptr, FILE_BEGIN) || !SetEndOfFile(mm->impl_.hFile))
                    {
                        outErr = MMapFile::Error::SetFileSizeFailed;
                        delete mm; return nullptr;
                    }
                    actualSize = requestedSize;
                }
            }
            else
            {
                LARGE_INTEGER newSize; newSize.QuadPart = static_cast<LONGLONG>(requestedSize);
                if (!SetFilePointerEx(mm->impl_.hFile, newSize, nullptr, FILE_BEGIN) || !SetEndOfFile(mm->impl_.hFile))
                {
                    outErr = MMapFile::Error::SetFileSizeFailed;
                    delete mm; return nullptr;
                }
                actualSize = requestedSize;
            }

            mm->impl_.hMap = CreateFileMappingW(mm->impl_.hFile, nullptr, protect, 0, static_cast<DWORD>(actualSize), nullptr);
            if (!mm->impl_.hMap)
            {
                outErr = MMapFile::Error::CreateMappingFailed;
                delete mm; return nullptr;
            }

            mm->data_ = MapViewOfFile(mm->impl_.hMap, mapAccess, 0, 0, actualSize);
            if (!mm->data_)
            {
                outErr = MMapFile::Error::MapViewFailed;
                delete mm; return nullptr;
            }

            mm->size_ = actualSize;
            return mm;
        }
    };

    // 基类析构无需清理（由派生类负责）
    MMapFile::~MMapFile() = default;

    // Free helper functions returning pointer or nullptr on failure
    MMapFile* CreateMMapFile(const OSString &filename, size_t size, MMapFile::Error *err)
    {
        MMapFile::Error e; auto *mm = WinMMapFile::Create(filename,
                                    GENERIC_READ | GENERIC_WRITE,
                                    0,
                                    CREATE_ALWAYS,
                                    PAGE_READWRITE,
                                    FILE_MAP_WRITE,
                                    size,
                                    false,
                                    false,
                                    e);
        if (err) *err = e; return mm;
    }

    MMapFile* OpenMMapFile(const OSString &filename, size_t size, MMapFile::Error *err)
    {
        MMapFile::Error e; auto *mm = WinMMapFile::Create(filename,
                                    GENERIC_READ | GENERIC_WRITE,
                                    0,
                                    OPEN_EXISTING,
                                    PAGE_READWRITE,
                                    FILE_MAP_WRITE,
                                    size,
                                    true,
                                    true,
                                    e);
        if (err) *err = e; return mm;
    }

    MMapFile* OpenMMapFileOnlyRead(const OSString &filename, MMapFile::Error *err)
    {
        MMapFile::Error e; auto *mm = WinMMapFile::Create(filename,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    OPEN_EXISTING,
                                    PAGE_READONLY,
                                    FILE_MAP_READ,
                                    0,
                                    true,
                                    false,
                                    e);
        if (err) *err = e; return mm;
    }

    void *MMapFile::data() { return data_; }
    size_t MMapFile::size() const { return size_; }
}//namespace hgl
