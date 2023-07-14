#pragma once

#include<hgl/type/MemBlock.h>
#include<hgl/filesystem/FileSystem.h>
namespace hgl
{
    /**
     * 保存一个内存块到文件
     */
    template<typename T> bool SaveMemBlockToFile(const OSString &filename,const MemBlock<T> &mb)
    {
        const size_t size=mb.bytes();

        if(size<=0)return(true);

        return(hgl::filesystem::SaveMemoryToFile(filename,mb.data(),mb.bytes())==size);
    }
}//namespace hgl
