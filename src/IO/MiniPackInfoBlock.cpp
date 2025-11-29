#include<hgl/io/MiniPack.h>
#include"MiniPackInfoBlock.h"
#include<hgl/log/Log.h>

DEFINE_LOGGER_MODULE(MiniPackInfoBlock)

namespace hgl::io::minipack
{
    FileEntryList *ParseInfoBlock(const char *info_block)
    {
        if(!info_block)return(nullptr);

        const uint8 *ptr = (const uint8 *)info_block;
        uint32 version = *(uint32 *)ptr;
        if(version != 1)
        {
            MLogError(MiniPackInfoBlock,"Unsupported version: %d",version);
            return(nullptr);
        }

        ptr += 4;
        uint32 entries_number = *(uint32 *)ptr;
        if(entries_number == 0)
        {
            MLogError(MiniPackInfoBlock,"No entries.");
            return(nullptr);
        }

        ptr += 4;

        FileEntryList *fel = new FileEntryList{};

        fel->count = entries_number;

        fel->name_length = ptr;
        ptr += entries_number;

        fel->name = new const char *[entries_number];

        for(uint32 i = 0;i < entries_number;i++)
        {
            fel->name[i] = (const char *)ptr;
            ptr += fel->name_length[i] + 1;
        }

        fel->offset = (uint32 *)ptr;
        ptr += entries_number * 4;
        fel->length = (uint32 *)ptr;
        ptr += entries_number * 4;

        return fel;
    }
}//namespace hgl::io::minipack
