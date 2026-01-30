#pragma once

#include<vector>
#include<hgl/filesystem/FileSystem.h>
namespace hgl
{
    /**
     * 保存一个数据阵列到文件
     */
    template<typename T> static bool SaveDataArrayToFile(const OSString &filename,const std::vector<T> &mb)
    {
        if(mb.empty())return(true);

        const size_t size=mb.size() * sizeof(T);

        if(size<=0)return(true);

        return(hgl::filesystem::SaveMemoryToFile(filename,mb.data(),size)==size);
    }
}//namespace hgl
