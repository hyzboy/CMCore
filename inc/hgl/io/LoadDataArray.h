#pragma once

#include<vector>
#include<hgl/io/FileInputStream.h>
namespace hgl
{
    /**
     * 加载一个文件到数据阵列中
     */
    template<typename T> static std::vector<T> LoadFileToDataArray(const OSString &filename)
    {
        io::FileInputStream fis;

        if(!fis.Open(filename))return std::vector<T>();

        const size_t file_size    =fis.GetSize();
        const size_t size        =(file_size+sizeof(T)-1)/sizeof(T);

        std::vector<T> mb(size);

        fis.Read(mb.data(),file_size);

        return mb;
    }
}//namespace hgl
