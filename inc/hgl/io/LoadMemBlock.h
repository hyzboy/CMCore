#pragma once

#include<hgl/type/MemBlock.h>
#include<hgl/io/FileInputStream.h>
namespace hgl
{
    /**
     * 加载一个文件到内存块类中
     */
    template<typename T> MemBlock<T> *LoadFileToMemBlock(const OSString &filename)
    {
        io::FileInputStream fis;

        if(!fis.Open(filename))return(nullptr);

        const size_t file_size	=fis.GetSize();
        const size_t size		=(file_size+sizeof(T)-1)/sizeof(T);

        MemBlock<T> *mb=new MemBlock<T>(size);

        fis.Read(mb->data(),file_size);

        return(mb);
    }
}//namespace hgl
