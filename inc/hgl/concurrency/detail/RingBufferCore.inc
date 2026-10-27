#ifndef HGL_CONCURRENCY_RING_BUFFER_CORE
#define HGL_CONCURRENCY_RING_BUFFER_CORE

/**
 * @brief CN: 环形缓冲区的游标基础实现。
 *        EN: Core cursor implementation for the ring buffer.
 * @details CN: read_pos 与 write_pos 使用单调递增的逻辑位置表示已消费与已写入数量；
 *              两者相等代表空，差值代表当前占用量。实际访问槽位时再对容量取模。
 *              逻辑位置越过容量边界后整体减去一个容量，以避免长期运行时整数溢出；
 *              不能分别对两个游标取模，否则“空”状态会与刚好绕满一圈的状态混淆。
 *          EN: read_pos and write_pos are monotonic logical positions for consumed and
 *              written elements. Equal positions mean empty; their difference is the
 *              current occupancy. Modulo is applied only when indexing storage. Once
 *              positions cross a capacity boundary, both are reduced by one capacity
 *              to prevent long-running integer overflow. Reducing them independently
 *              would make empty indistinguishable from a completely wrapped buffer.
 */
//--------------------------------------------------------------------------------------------------
namespace hgl
{
    template<typename T>
    RingBuffer<T>::RingBuffer(int size)
    {
        buffer_size=size;

        buffer=new T[size];

//         memset(buffer,0,size);        //有write_pos/read_pos没有根本读不出数据，所以无需清0

        read_pos=0;
        write_pos=0;
    }

    template<typename T>
    RingBuffer<T>::~RingBuffer()
    {
        delete[] buffer;
    }

    template<typename T>
    void RingBuffer<T>::Clear()
    {
        read_pos=0;
        write_pos=0;
    }

    template<typename T>
    void RingBuffer<T>::SafeClear()
    {
        Lock();
        Clear();
        Unlock();
    }

    template<typename T>
    void RingBuffer<T>::ClampPosition()
    {
        if(read_pos<buffer_size
         ||write_pos<buffer_size)return;    //将指针减去整数部分，以免溢出

        //不可以用read_pos%=buffer_size，write_pos%=buffer_size。
        //因为有可能read_pos=0,write_pos=buffersize，都用%buffer_size就会形成两个都为0的错误。
        //一般也就破一次界限，所以效能影响无所谓。

        read_pos-=buffer_size;
        write_pos-=buffer_size;
    }
}//namespace hgl
#endif//HGL_CONCURRENCY_RING_BUFFER_CORE
