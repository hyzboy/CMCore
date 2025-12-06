#if defined(_WIN32)
#error "mmap_posix.cpp should not be built on Windows"
#endif

#include <hgl/io/MMapFile.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

namespace hgl
{
    class MMapFileImpl {
    public:
        int fd = -1;
    };

    class PosixMMapFile final : public MMapFile
    {
        MMapFileImpl impl_{};
    public:
        ~PosixMMapFile() override
        {
            if (data_) { munmap(data_, size_); data_ = nullptr; }
            if (impl_.fd >= 0) { close(impl_.fd); impl_.fd = -1; }
        }

        static PosixMMapFile* Create(const OSString &filename,
                                     int oflags,
                                     mode_t mode,
                                     int prot,
                                     int mapFlags,
                                     size_t requestedSize,
                                     bool useFileSize,
                                     bool ensureAtLeastRequested,
                                     MMapFile::Error &outErr)
        {
            outErr = MMapFile::Error::Ok;

            PosixMMapFile *mm = new PosixMMapFile();

            mm->impl_.fd = open(filename.c_str(), oflags, mode);
            if (mm->impl_.fd < 0) { outErr = MMapFile::Error::OpenFileFailed; delete mm; return nullptr; }

            size_t actualSize = requestedSize;

            if (useFileSize)
            {
                off_t fileSize = lseek(mm->impl_.fd, 0, SEEK_END);
                if (fileSize < 0) { outErr = MMapFile::Error::GetFileSizeFailed; delete mm; return nullptr; }
                actualSize = static_cast<size_t>(fileSize);

                if (ensureAtLeastRequested && requestedSize > actualSize)
                {
                    if (ftruncate(mm->impl_.fd, static_cast<off_t>(requestedSize)) != 0)
                    { outErr = MMapFile::Error::SetFileSizeFailed; delete mm; return nullptr; }
                    actualSize = requestedSize;
                }
            }
            else
            {
                if (requestedSize==0) { outErr = MMapFile::Error::InvalidArgument; delete mm; return nullptr; }
                if (ftruncate(mm->impl_.fd, static_cast<off_t>(requestedSize)) != 0)
                { outErr = MMapFile::Error::SetFileSizeFailed; delete mm; return nullptr; }
                actualSize = requestedSize;
            }

            void* ptr = mmap(nullptr, actualSize, prot, mapFlags, mm->impl_.fd, 0);
            if (ptr == MAP_FAILED) { outErr = MMapFile::Error::MapViewFailed; delete mm; return nullptr; }

            mm->data_ = ptr;
            mm->size_ = actualSize;
            return mm;
        }
    };

    // 基类析构无需清理（由派生类负责）
    MMapFile::~MMapFile() = default;

    // Free helper functions returning pointer or nullptr on failure
    MMapFile* CreateMMapFile(const OSString &filename, size_t size, MMapFile::Error *err)
    {
        MMapFile::Error e; auto *mm = PosixMMapFile::Create(filename,
                                     O_RDWR | O_CREAT | O_TRUNC,
                                     0666,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED,
                                     size,
                                     false,
                                     false,
                                     e);
        if (err) *err = e; return mm;
    }

    MMapFile* OpenMMapFile(const OSString &filename, size_t size, MMapFile::Error *err)
    {
        MMapFile::Error e; auto *mm = PosixMMapFile::Create(filename,
                                     O_RDWR,
                                     0666,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED,
                                     size,
                                     true,
                                     true,
                                     e);
        if (err) *err = e; return mm;
    }

    MMapFile* OpenMMapFileOnlyRead(const OSString &filename, MMapFile::Error *err)
    {
        MMapFile::Error e; auto *mm = PosixMMapFile::Create(filename,
                                     O_RDONLY,
                                     0666,
                                     PROT_READ,
                                     MAP_SHARED,
                                     0,
                                     true,
                                     false,
                                     e);
        if (err) *err = e; return mm;
    }

    void* MMapFile::data() { return data_; }
    size_t MMapFile::size() const { return size_; }
}//namespace hgl
