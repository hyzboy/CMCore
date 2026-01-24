#pragma once

#include<hgl/type/ValueBuffer.h>
#include<hgl/filesystem/FileSystem.h>
namespace hgl
{
    /**
     * 保存一个数据阵列到文件
     */
    template<typename T> static bool SaveDataArrayToFile(const OSString &filename,const ValueBuffer<T> &mb)
    {
        const size_t size=mb.bytes();

        if(size<=0)return(true);

        return(hgl::filesystem::SaveMemoryToFile(filename,mb.data(),mb.bytes())==size);
    }
}//namespace hgl
