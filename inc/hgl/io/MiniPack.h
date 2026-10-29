#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/String.h>
#include<hgl/log/Log.h>

namespace hgl::io
{
    class InputStream;
}

namespace hgl::io::minipack
{
    class MiniPack
    {
    public:

        virtual ~MiniPack() = default;

        virtual uint    GetFileCount()const = 0;

        virtual int32   FindFile(const AnsiStringView &)const = 0;
        virtual uint32  GetFileLength(int32 index)const = 0;

    public:

        bool    IsFileExist(const AnsiStringView &name)const{ return FindFile(name) != -1; }

        uint32  GetFileLength(const AnsiStringView &name)const{ return GetFileLength(FindFile(name)); }
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

        bool    ReadFile(const AnsiStringView &name,void *buf,uint32 start,uint32 size){ return ReadFile(FindFile(name),buf,start,size); }

        template<typename T>
        bool    ReadFileData(const AnsiStringView &name,T *buf)
        {
            return ReadFile(name,buf,0,sizeof(T));
        }
    };//class MiniPackReader

    /**
    * MiniPack 内存包读取器<br>
    * 这个读取器它会把整个 MiniPack 文件读入内存中，然后所有的文件读取都在内存中进行
    */
    class MiniPackMemory:public MiniPack
    {
        OBJECT_LOGGER

    public:

        MiniPackMemory(const OSStringView &fn):MiniPack()
        {
            Log.SetLoggerInstanceName(OSString(fn));
        }

        virtual ~MiniPackMemory() = default;

        virtual void *Map(int32)=0;

        void *Map(const AnsiStringView &name){ return Map(FindFile(name)); }

        template<typename T>
        T *MapData(const AnsiStringView &name)
        {
            const int32 index=FindFile(name);

            if(index<0)
            {
                LogError("File <%s> not found in minipack.",name.c_str());
                return(nullptr);
            }

            if(sizeof(T)!=GetFileLength(index))
            {
                LogError("File <%s> size mismatch in minipack.",name.c_str());
                return(nullptr);
            }

            return (T *)Map(index);
        }
    };//class MiniPackMemory

    MiniPackReader *GetMiniPackReader(::hgl::io::InputStream *);
    MiniPackReader *GetMiniPackReader(const OSStringView &);
    MiniPackMemory *GetMiniPackMemory(const OSStringView &);
    MiniPackMemory *GetMiniPackFileMapping(const OSStringView &);

    /**
     * @brief 从**已读入内存**的字节构造 MiniPackMemory（异步加载路径用）。
     *
     * 不拷贝、也不接管 `data`：返回的读取器只是一层视图，调用方必须保证这块内存在
     * 读取器销毁之前一直有效（典型用法：`LoadedFileManager` 回调期间解析完毕并立即销毁）。
     *
     * @param name 仅用于日志与诊断的资产名（不需要是真实文件）
     * @param data 完整 minipack 字节
     * @param size 字节数
     */
    MiniPackMemory *GetMiniPackMemoryFromBytes(const OSStringView &name,const void *data,uint32 size);
}//namespace hgl::io::minipack
