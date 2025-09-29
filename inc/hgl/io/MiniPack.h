#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/String.h>

namespace hgl::io::minipack
{
    enum class NameEncoding:uint8
    {
        UTF8        =0,
        UTF16_LE    =1
    };//enum class NameEncoding

    class MiniPack
    {
    public:

        virtual ~MiniPack() = default;

        virtual uint    GetFileCount()const = 0;

        virtual int32   FindFile(const U8String &)const = 0;
        virtual uint32  GetFileLength(int32 index)const = 0;
        virtual uint32  ReadFile(int32 index,void *buf,uint32 start,uint32 size) = 0;

    public:

        bool    IsFileExist(const U8String &name)const{ return FindFile(name) != -1; }

        uint32  GetFileLength(const U8String &name)const{ return GetFileLength(FindFile(name)); }
    };//class MiniPack

    /**
    * MiniPack 读取器基类<br>
    * 这个读取器它只会读取文件列表等基础信息，只有你真正调用ReadFile时才会去读取文件数据
    */
    class MiniPackReader:public MiniPack
    {
    public:

        virtual ~MiniPackReader() = default;

        virtual uint32  ReadFile(int32 index,void *buf,uint32 start,uint32 size) = 0;

    public:

        bool    ReadFile(const U8String &name,void *buf,uint32 start,uint32 size){ return ReadFile(FindFile(name),buf,start,size); }
    };//class MiniPackReader

    /**
    * MiniPack 内存包读取器<br>
    * 这个读取器它会把整个 MiniPack 文件读入内存中，然后所有的文件读取都在内存中进行
    */
    class MiniPackMemory:public MiniPack
    {
    public:

        virtual ~MiniPackMemory() = default;

        void *Map(int32);
        void *Map(const U8String &name){ return Map(FindFile(name)); }
    };//class MiniPackMemory

    MiniPackReader *GetMiniPackReader(const OSString &);
}//namespace hgl::io::minipack
