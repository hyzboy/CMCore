#pragma once

#include<hgl/CoreType.h>
#include<vector>

namespace hgl
{
    /**
     * 累计内存管理<br>
     * 用于不断分配固定容量的内存块，但不动态调整，最后统一释放的情况。
     */
    class AccumMemoryManager
    {
    public:

        struct Block
        {
            int64 offset;
            int64 size;
        };

    private:

        std::vector<Block>      block_list;                     ///<数据块列表
        std::vector<char>       data_array;                     ///<数据

    public:

        AccumMemoryManager()=default;
        ~AccumMemoryManager()=default;

        const int64 GetTotalBytes()const{return data_array.size();}    ///<取得总共申请的内存总字节数
        const int64 GetBlockCount()const{return block_list.size();}         ///<取得内存数据块数量

        Block *Acquire(const int64 size)                                        ///<申请一块内存
        {
            if(size<=0)return(nullptr);

            block_list.emplace_back();
            Block *b = &block_list.back();

            b->offset   = data_array.size();
            b->size     = size;

            data_array.resize(data_array.size() + size);

            return b;
        }

        void *Access(const Block *b)                                            ///<访问一块内存
        {
            return b ? (data_array.data() + b->offset) : nullptr;
        }

        void Clear()
        {
            block_list.clear();
            data_array.clear();
        }
    };//class AccumMemoryManager
}//namespace
