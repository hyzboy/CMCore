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

    class MiniPackReader
    {
    public:

        virtual uint    GetFileCount()const=0;

        virtual int32   FindFile(const U8String &)const = 0;
        virtual int32   FindFile(const OSString &)const = 0;

        virtual uint32  GetFileLength(int32 index)const = 0;
        virtual uint32  ReadFile(int32 index,void *buf,uint32 start,uint32 size) = 0;

    public:

        bool    IsFileExist(const U8String &name)const{ return FindFile(name) != -1;}
        bool    IsFileExist(const OSString &name)const{ return FindFile(name) != -1;}

        uint32  GetFileLength(const U8String &name)const{ return GetFileLength(FindFile(name)); }
        uint32  GetFileLength(const OSString &name)const{ return GetFileLength(FindFile(name)); }

        bool    ReadFile(const U8String &name,void *buf,uint32 start,uint32 size){ return ReadFile(FindFile(name),buf,start,size); }
        bool    ReadFile(const OSString &name,void *buf,uint32 start,uint32 size){ return ReadFile(FindFile(name),buf,start,size); }
    };//class MiniPack

    MiniPackReader *GetMiniPack(const OSString &);
}//namespace hgl::io::minipack
