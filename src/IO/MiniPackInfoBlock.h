#pragma once

#include<hgl/type/DataType.h>

namespace hgl::io::minipack
{
    const char MiniPackMagicID[8] = { 'M','I','N','I','P','A','C','K' };
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

        uint8 *     name_length;    ///<每个文件名的长度(直接指向info_block的指针，无需分配释放)
        char8_t **  name;           ///<所有的文件名（除整体是new char8_t *[]外，所有指针均是直接指向info_block的指针，无需分配释放）
        uint32 *    offset;
        uint32 *    length;

    public:

        ~FileEntryList()
        {
            delete[] name;
        }
    };

    FileEntryList *ParseInfoBlock(const char *info_block);
}//namespace hgl::io::minipack
