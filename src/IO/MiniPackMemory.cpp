#include<hgl/io/MiniPack.h>
#include"MiniPackInfoBlock.h"
#include<hgl/filesystem/FileSystem.h>
#include<hgl/io/MMapFile.h>
#include<hgl/io/FileAccess.h>
#include<hgl/log/Log.h>
#include<cstring>

DEFINE_LOGGER_MODULE(MiniPackMemory)

namespace hgl::io::minipack
{
    namespace
    {
        // Shared parse routine for both raw memory buffer and file mapping
        bool ParseMiniPack(char *data, int64 file_size, uint32 &data_start, FileEntryList *&fel_out, uint32 &available)
        {
            if(!data || file_size < static_cast<int64>(MiniPackFileHeaderSize))
            {
                MLogError(MiniPackMemory,"Load file to memory failed or file too small.");
                return false;
            }

            const MiniPackFileHeader *header = reinterpret_cast<const MiniPackFileHeader *>(data);

            if(std::memcmp(header->magic, MiniPackMagicID, MiniPackMagicIDBytes))
            {
                MLogError(MiniPackMemory,"Magic number error.");
                return false;
            }

            data_start = static_cast<uint32>(MiniPackFileHeaderSize + header->info_size);
            if(file_size < data_start)
            {
                MLogError(MiniPackMemory,"File truncated: header info_size exceeds file size.");
                return false;
            }

            const char *info_block = data + MiniPackFileHeaderSize;
            FileEntryList *fel = ParseInfoBlock(info_block);
            if(!fel)
            {
                return false;
            }

            // Validate entries are within data block
            available = static_cast<uint32>(file_size - data_start);
            for(uint32 i=0; i<fel->count; ++i)
            {
                const uint64 end_pos = static_cast<uint64>(fel->offset[i]) + static_cast<uint64>(fel->length[i]);
                if(end_pos > available)
                {
                    MLogError(MiniPackMemory, "Entry %u out of bounds: end %llu > data size %u", i, static_cast<unsigned long long>(end_pos), available);
                    delete fel;
                    return false;
                }
            }

            fel_out = fel;
            return true;
        }

        // Base class that implements all accessors on top of a memory view
        class MiniPackMemoryBase:public MiniPackMemory
        {
        protected:
            FileEntryList *entry_list {nullptr};
            uint8 *data_block {nullptr};
            uint32 data_size {0};

            MiniPackMemoryBase(FileEntryList *fel, uint8 *db, uint32 ds)
                : entry_list(fel), data_block(db), data_size(ds) {}

        public:
            ~MiniPackMemoryBase() override
            {
                delete entry_list; // free name pointer array; strings point into info block
            }

            uint GetFileCount() const override
            {
                return entry_list ? entry_list->count : 0;
            }

            int32 FindFile(const AnsiStringView &filename) const override
            {
                if(!entry_list || filename.IsEmpty() || !entry_list->count)
                    return -1;

                for(uint32 i=0; i<entry_list->count; ++i)
                {
                    if(filename.Length()!=entry_list->name_length[i])
                        continue;

                    if(filename.CaseComp(entry_list->name[i], entry_list->name_length[i])==0)
                        return static_cast<int32>(i);
                }

                return -1;
            }

            uint32 GetFileLength(int32 index) const override
            {
                if(!entry_list || index<0 || index>=static_cast<int32>(entry_list->count))
                    return 0;

                return entry_list->length[index];
            }

            void *Map(int32 index) override
            {
                if(!entry_list || !data_block)
                    return nullptr;

                if(index<0 || index>=static_cast<int32>(entry_list->count))
                    return nullptr;

                const uint32 off = entry_list->offset[index];
                const uint32 len = entry_list->length[index];
                if(off+len>data_size)
                    return nullptr;

                return data_block + off;
            }
        };

        class MiniPackMemoryInMemory:public MiniPackMemoryBase
        {
            char *file_data {nullptr};

        public:
            MiniPackMemoryInMemory(char *fd, int64 fs, FileEntryList *fel, const uint32 data_start)
                : MiniPackMemoryBase(fel,
                                      reinterpret_cast<uint8 *>(fd) + data_start,
                                      (fs>data_start)?static_cast<uint32>(fs-data_start):0)
                , file_data(fd)
            {
            }

            ~MiniPackMemoryInMemory() override
            {
                delete[] file_data;    // frees the whole file buffer
            }
        };

        class MiniPackMemoryMapped:public MiniPackMemoryBase
        {
            hgl::MMapFile *mm {nullptr};

        public:
            MiniPackMemoryMapped(hgl::MMapFile *mmap, FileEntryList *fel, const uint32 data_start)
                : MiniPackMemoryBase(fel,
                                      reinterpret_cast<uint8 *>(mmap->data()) + data_start,
                                      (mmap->size()>data_start)?static_cast<uint32>(mmap->size()-data_start):0)
                , mm(mmap)
            {
            }

            ~MiniPackMemoryMapped() override
            {
                delete mm;         // unmap and close file
            }
        };
    }

    // Load whole file to memory and use owning buffer
    MiniPackMemory *GetMiniPackMemory(const OSStringView &filename)
    {
        if(filename.IsEmpty())
            return nullptr;

        // Convert view to owning string for filesystem APIs
        OSString fname(filename.c_str(), filename.Length());

        char *data = nullptr;
        int64 file_size = filesystem::LoadFileToMemory(fname, (void **)&data);

        if(!data)
            return nullptr;

        uint32 data_start = 0; FileEntryList *fel = nullptr; uint32 available = 0;
        if(!ParseMiniPack(data, file_size, data_start, fel, available))
        {
            delete[] data;
            return nullptr;
        }

        return new MiniPackMemoryInMemory(data, file_size, fel, data_start);
    }

    // Use file mapping to access file without copying
    MiniPackMemory *GetMiniPackFileMapping(const OSStringView &filename)
    {
        if(filename.IsEmpty())
            return nullptr;

        // Convert view to owning string for mapping API
        OSString fname(filename.c_str(), filename.Length());

        hgl::MMapFile *mm = OpenMMapFileOnlyRead(fname);
        if(!mm)
            return nullptr;

        char *data = static_cast<char *>(mm->data());

        // Header-less size sanity will happen in ParseMiniPack
        int64 file_size = static_cast<int64>(mm->size());

        uint32 data_start = 0; FileEntryList *fel = nullptr; uint32 available = 0;
        if(!ParseMiniPack(data, file_size, data_start, fel, available))
        {
            delete mm;
            return nullptr;
        }

        return new MiniPackMemoryMapped(mm, fel, data_start);
    }
}// namespace hgl::io::minipack
