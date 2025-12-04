#pragma once

#include<hgl/type/StrChar.h>

namespace hgl::io::minipack
{
    const char MiniPackMagicID[8] = { 'M','i','n','i','P','a','c','k' };
    constexpr const size_t MiniPackMagicIDBytes = sizeof(MiniPackMagicID);

#pragma pack(push,1)
    struct MiniPackFileHeader
    {
        char    magic[MiniPackMagicIDBytes];
        uint32  info_size;
    };
#pragma pack(pop)

    constexpr const size_t MiniPackFileHeaderSize = sizeof(MiniPackFileHeader);

    struct FileEntryList
    {
        uint32      count;

        const uint8 *     name_length;    ///<每个文件名的长度(直接指向info_block的指针，无需分配释放)
        const char **     name;           ///<所有的文件名（除整体是new char *[]外，所有指针均是直接指向AttributeMeta内存区的指针，无需分配释放）
        const uint32 *    offset;
        const uint32 *    length;

    public:

        ~FileEntryList()
        {
            delete[] name;
        }
    };

    FileEntryList *ParseInfoBlock(const char *info_block);
}//namespace hgl::io::minipack
