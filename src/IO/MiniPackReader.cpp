#include<hgl/io/MiniPack.h>
#include"MiniPackInfoBlock.h"
#include<hgl/io/DataInputStream.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/log/Log.h>

DEFINE_LOGGER_MODULE(MiniPackReader)

namespace hgl::io::minipack
{
    namespace
    {
        class MiniPackReaderFromStream:public MiniPackReader
        {
            InputStream *is;

            char *info_block;

            FileEntryList *entry_list;

            int32 data_start;

        public:

            MiniPackReaderFromStream(InputStream *i,char *ib,FileEntryList *fel,const int32 start)
            {
                is=i;
                info_block = ib;
                entry_list=fel;

                data_start=start;
            }

            ~MiniPackReaderFromStream() override
            {
                delete is;
                delete[] info_block;
                delete entry_list;
            }

            uint GetFileCount() const override
            {
                return entry_list->count;
            }

            int32   FindFile(const AnsiStringView &filename)const override
            {
                if(filename.IsEmpty()||!entry_list->count)
                    return -1;

                for(uint32 i = 0;i < entry_list->count;i++)
                {
                    if(filename.Length()!=entry_list->name_length[i])
                        continue;

                    if(filename.CaseComp(entry_list->name[i],entry_list->name_length[i])==0)
                        return i;
                }

                return -1;
            }

            uint32  GetFileLength(int32 index)const override
            {
                if(index<0||index>=static_cast<int32>(entry_list->count))
                    return 0;

                return entry_list->length[index];
            }

            uint32  ReadFile(int32 index,void *buf,uint32 start,uint32 size) override
            {
                if(index<0||index>=static_cast<int32>(entry_list->count))
                    return 0;

                if(!buf||size==0)
                    return 0;

                if(start>=entry_list->length[index])
                    return 0;

                if(start+size>entry_list->length[index])
                    size=entry_list->length[index]-start;

                if(is->Seek(data_start+entry_list->offset[index]+start,SeekOrigin::Begin)!=data_start+entry_list->offset[index]+start)
                {
                    MLogError(MiniPackReader,"Seek to file %d offset %u failed.",index,entry_list->offset[index]+start);
                    return 0;
                }

                return is->ReadFully(buf,size);
            }
        };
    }

    MiniPackReader *GetMiniPackReader(InputStream *is)
    {
        if(!is)
        {
            MLogError(MiniPackReader,"InputStream is nullptr.");
            return(nullptr);
        }

        // 所有权约定：只要传入了非空流，**本函数就接管其所有权**（成功时由
        // MiniPackReaderFromStream 的析构负责释放，失败时在各错误分支就地释放）。
        // 此前只有 ParseInfoBlock 失败分支 delete 了流，其余错误分支都漏掉了，
        // 造成同一接口的成功/失败路径所有权语义不一致。
        MiniPackFileHeader header;

        if(is->ReadFully(&header,MiniPackFileHeaderSize) != MiniPackFileHeaderSize)
        {
            MLogError(MiniPackReader,"Read file header failed.");
            delete is;
            return(nullptr);
        }

        if(memcmp(header.magic,MiniPackMagicID,MiniPackMagicIDBytes))
        {
            MLogError(MiniPackReader,"Magic number error.");
            delete is;
            return(nullptr);
        }

        char *info_block = new char[header.info_size];

        if(is->Read(info_block,header.info_size)!=header.info_size)
        {
            MLogError(MiniPackReader,"Read info block failed.");
            delete[] info_block;
            delete is;
            return(nullptr);
        }

        FileEntryList *fel = ParseInfoBlock(info_block);

        if(!fel)
        {
            delete is;
            delete[] info_block;
            return(nullptr);
        }

        return(new MiniPackReaderFromStream(is,info_block,fel,MiniPackFileHeaderSize + header.info_size));
    }

    MiniPackReader *GetMiniPackReader(const OSStringView &filename)
    {
        FileInputStream *fis = new FileInputStream;

        // Convert view to owning string for APIs expecting OSString
        OSString fname(filename.c_str(), filename.Length());

        if(!fis->Open(fname))
        {
            delete fis;
            return(nullptr);
        }

        // 失败的流已由 GetMiniPackReader(InputStream*) 接管并释放
        return GetMiniPackReader(fis);
    }
}//namespace hgl::io::minipack
