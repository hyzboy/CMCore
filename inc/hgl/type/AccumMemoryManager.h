#pragma once

#include<hgl/type/DataArray.h>

namespace hgl
{
    /**
     * �ۼ��ڴ����<br>
     * ���ڲ��Ϸ���̶��������ڴ�飬������̬���������ͳһ�ͷŵ������
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

        int64 total_bytes=0;           ///<���ֽ���

        DataArray<AccumMemoryManager::Block> block_list;
        DataArray<char> data_array;     ///<����

    public:

        AccumMemoryManager()=default;
        ~AccumMemoryManager()=default;

        const int64 GetTotalBytes()const{return total_bytes;}
        const int64 GetBlockCount()const{return block_list.GetCount();}

        const void *Acquire(const int64 size)
        {
            if(size<=0)return(nullptr);

            Block b;

            b.offset=total_bytes;
            b.size=size;

            data_array.AddCount(size);

            return data_array.GetPointer(b.offset);
        }

        void Clear()
        {
            total_bytes=0;
            block_list.Clear();
            data_array.Clear();
        }

        void Free()
        {
            total_bytes=0;
            block_list.Free();
            data_array.Free();
        }
    };//class AccumMemoryManager
}//namespace
