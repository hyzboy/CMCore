#include<hgl/io/MiniPack.h>
#include"MiniPackInfoBlock.h"
#include<hgl/filesystem/FileSystem.h>
#include<hgl/log/Log.h>
#include<cstring>

DEFINE_LOGGER_MODULE(MiniPackMemory)

namespace hgl::io::minipack
{
    namespace
    {
        class MiniPackMemoryInMemory:public MiniPackMemory
        {
            char *file_data {nullptr};
            int64 file_size {0};

            FileEntryList *entry_list {nullptr};

            // Non-owning view into file_data
            uint8 *data_block {nullptr};
            uint32 data_size {0};

        public:
            MiniPackMemoryInMemory(char *fd, int64 fs, FileEntryList *fel, const uint32 data_start)
            {
                file_data  = fd;
                file_size  = fs;
                entry_list = fel;

                data_block = reinterpret_cast<uint8 *>(fd) + data_start;
                const int64 ds = fs - data_start;
                data_size = ds > 0 ? static_cast<uint32>(ds) : 0;
            }

            ~MiniPackMemoryInMemory() override
            {
                delete entry_list;     // only frees name table array, names point into info region within file_data
                delete[] file_data;    // frees the whole file buffer
            }

            uint GetFileCount() const override
            {
                return entry_list ? entry_list->count : 0;
            }

            int32 FindFile(const U8String &filename) const override
            {
                if(!entry_list || filename.IsEmpty() || !entry_list->count)
                    return -1;

                for(uint32 i=0; i<entry_list->count; ++i)
                {
                    if(filename.Length()!=entry_list->name_length[i])
                        continue;

                    if(filename.Comp(entry_list->name[i], entry_list->name_length[i])==0)
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
    }

    MiniPackMemory *GetMiniPackMemory(const OSString &filename)
    {
        if(filename.IsEmpty())
            return nullptr;

        char *data = nullptr;
        int64 file_size = filesystem::LoadFileToMemory(filename, (void **)&data);

        if(!data || file_size < static_cast<int64>(MiniPackFileHeaderSize))
        {
            if(data) delete[] data;
            MLogError(MiniPackMemory,"Load file to memory failed or file too small.");
            return nullptr;
        }

        const MiniPackFileHeader *header = reinterpret_cast<const MiniPackFileHeader *>(data);

        if(std::memcmp(header->magic, MiniPackMagicID, MiniPackMagicIDBytes))
        {
            MLogError(MiniPackMemory,"Magic number error.");
            delete[] data;
            return nullptr;
        }

        const uint32 data_start = static_cast<uint32>(MiniPackFileHeaderSize + header->info_size);
        if(file_size < data_start)
        {
            MLogError(MiniPackMemory,"File truncated: header info_size exceeds file size.");
            delete[] data;
            return nullptr;
        }

        const char *info_block = data + MiniPackFileHeaderSize;
        FileEntryList *fel = ParseInfoBlock(info_block);
        if(!fel)
        {
            delete[] data;
            return nullptr;
        }

        // Validate entries are within data block
        const uint32 available = static_cast<uint32>(file_size - data_start);
        for(uint32 i=0; i<fel->count; ++i)
        {
            const uint64 end_pos = static_cast<uint64>(fel->offset[i]) + static_cast<uint64>(fel->length[i]);
            if(end_pos > available)
            {
                MLogError(MiniPackMemory, "Entry %u out of bounds: end %llu > data size %u", i, static_cast<unsigned long long>(end_pos), available);
                delete fel;
                delete[] data;
                return nullptr;
            }
        }

        return new MiniPackMemoryInMemory(data, file_size, fel, data_start);
    }
}// namespace hgl::io::minipack
