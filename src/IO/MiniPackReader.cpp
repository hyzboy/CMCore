#include<hgl/io/MiniPack.h>
#include<hgl/io/DataInputStream.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/io/MemoryInputStream.h>
#include<hgl/log/Log.h>
#include<hgl/type/MemoryBlock.h>

DEFINE_LOGGER_MODULE(MiniPackReader)

namespace hgl::io::minipack
{
    template<typename T> struct FileEntry
    {
        T *name;
        uint8       name_length;

        uint32      offset;
        uint32      length;
    };

    template<typename T>
    class MiniPackReaderFromFile:public MiniPackReader
    {

        DataInputStream *dis;

        char *info_block;

        DataArray<FileEntry> file_list;

    public:


    };

    namespace
    {
        const char kMagic[8] = { 'M','I','N','I','P','A','C','K' };

        enum class NameEncoding: uint8_t
        {
            Utf8 = 0,
            Utf16Le = 1,
        };
    }

    MiniPackReader *GetMiniPack(InputStream *is)
    {
        if(!is)
        {
            MLogError(MiniPackReader,"InputStream is nullptr.");
            return(nullptr);
        }

        DataInputStream *dis=new LEDataInputStream(is);

        char magic[8];

        if(dis->ReadFully(magic,8)!=8)
        {
            MLogError(MiniPackReader,"Read magic number failed.");
            return(nullptr);
        }

        if(memcmp(magic,kMagic,8))
        {
            MLogError(MiniPackReader,"Magic number error.");
            return(nullptr);
        }

        uint32 info_size;
        if(!dis->ReadUint32(info_size))
        {
            MLogError(MiniPackReader,"Read info size failed.");
            return(nullptr);
        }

        char *info_block = new char[info_size];

        dis->Peek(info_block,info_size);

        uint32 version;
        if(!dis->ReadUint32(version))
        {
            MLogError(MiniPackReader,"Read version failed.");
            return(nullptr);
        }

        if(version!=1)
        {
            MLogError(MiniPackReader,"Unsupported version: %d",version);
            return(nullptr);
        }

        uint32 entries_number;
        if(!dis->ReadUint32(entries_number))
        {
            MLogError(MiniPackReader,"Read entries number failed.");
            return(nullptr);
        }

        uint8 name_encoding;
        if(!dis->ReadUint8(name_encoding))
        {
            MLogError(MiniPackReader,"Read name encoding failed.");
            return(nullptr);
        }

        for(uint32 i=0;i<entries_number;i++)
        {

        }
    }

    MiniPackReader *GetMiniPack(const OSString &filename)
    {
        FileInputStream *fis = new FileInputStream;

        if(!fis->Open(filename))
        {
            delete fis;
            return(nullptr);
        }

        return GetMiniPack(fis);
    }
}//namespace hgl::io::minipack
