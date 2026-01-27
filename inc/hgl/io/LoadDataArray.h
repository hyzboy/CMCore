#pragma once

#include<hgl/type/ValueBuffer.h>
#include<hgl/io/FileInputStream.h>
namespace hgl
{
    /**
     * 加载一个文件到数据阵列中
     */
    template<typename T> static ValueBuffer<T> *LoadFileToDataArray(const OSString &filename)
    {
        io::FileInputStream fis;

        if(!fis.Open(filename))return(nullptr);

        const size_t file_size    =fis.GetSize();
        const size_t size        =(file_size+sizeof(T)-1)/sizeof(T);

        ValueBuffer<T> *mb=new ValueBuffer<T>(size);

        fis.Read(mb->data(),file_size);

        return(mb);
    }
}//namespace hgl
