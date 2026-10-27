#pragma once

#include<hgl/sync/Mutex.h>
#include<hgl/io/InputStream.h>
#include<hgl/io/OutputStream.h>
namespace hgl
{
    /**
     * @brief CN: 固定容量的环形数据流，提供游标式读写及互斥安全便捷接口。
     *        EN: Fixed-capacity ring stream with cursor-based and mutex-safe read/write APIs.
     * @details CN: ReadStart/Read/ReadEnd 与 WriteStart/Write/WriteEnd 是分段访问协议；
     *              基础接口不会自动加锁，只适合调用方已串行化访问的场景。多线程共享时
     *              使用 Safe* 系列：成功取得正容量后必须配对调用相应 End，结束调用会
     *              提交游标并释放内部 Mutex。TryStart 返回负数表示未取得锁，返回零表示
     *              没有可用数据/空间；这两种情况都不应再调用 End。
     *              Read/Write 一次最多传输当前区间内可用数量，返回值表示实际传输量。
     *              Peek 只窥视不消费，Skip 消费数据但不复制到目标缓冲区。容量必须为正；
     *              流适配器及访问守卫都不拥有缓冲区，调用方须保证底层对象持续有效。
     *          EN: ReadStart/Read/ReadEnd and WriteStart/Write/WriteEnd form segmented
     *              access protocols. The basic APIs do not lock and require external
     *              serialization; use Safe* APIs when sharing across threads. After a
     *              positive-capacity Safe*Start, pair the call with the matching End,
     *              which commits the cursor and releases the internal mutex. TryStart
     *              returns a negative value when the lock was not acquired and zero when
     *              no data or space is available; do not call End in either case.
     *              Read and Write return the amount actually transferred. Peek does not
     *              consume data, while Skip consumes without copying. Capacity must be
     *              positive. Stream adapters and guards are non-owning; the buffer must
     *              outlive their use.
     */
    template<typename T> class RingBuffer:protected Mutex
    {
    protected:

        T *buffer;
        int buffer_size;

        int read_pos,write_pos;

        int read_cur,write_cur;
        int read_off,write_off;
        int read_max,write_max;
        int read_count,write_count;

        bool read_peek;

    protected:

        void ClampPosition();

        int _GetReadSize()const{return write_pos-read_pos;}
        int _GetWriteSize()const{return buffer_size-(write_pos-read_pos);}

        int _SafeWriteStart();
        int _SafeReadStart(bool);

        int _Write(const T *,int);
        int _Read(T *,int,bool);

    public:

        RingBuffer(int);                                                                            ///<本类构造函数
        virtual ~RingBuffer();                                                                      ///<本类析构函数

        const int GetBufferSize()const{return buffer_size;}                                         ///< @brief CN: 返回固定槽位数，不等于当前可读/可写数量。 EN: Return fixed capacity, not the current readable or writable amount.

        void Clear();                                                                               ///< @brief CN: 清空缓冲区；仅在调用方保证无并发访问时使用。 EN: Clear the buffer; call only when the caller guarantees no concurrent access.
        void SafeClear();                                                                           ///< @brief CN: 加锁清空缓冲区。 EN: Clear the buffer while holding its mutex.

    public: //写处理函数

        int WriteStart();                                                                           ///< @brief CN: 开始一次非互斥写访问并准备游标。 EN: Begin an externally serialized write and prepare its cursor.
        int GetWriteSize()const{return write_max;}                                                  ///< @brief CN: 返回本次 WriteStart 后剩余可写数量。 EN: Return writable slots remaining in the current write region.
        int Write(const T *,int);                                                                   ///< @brief CN: 写入本次写区间并返回实际写入数量。 EN: Write to the current region and return the amount written.
        int WriteEnd();                                                                             ///< @brief CN: 提交已写游标；基础接口不负责解锁。 EN: Commit the write cursor; the basic API does not unlock.

        int SafeTryWriteStart();                                                                    ///< @brief CN: 非阻塞加锁并开始写入；负值表示锁忙，零表示无空间。 EN: Try-lock and begin writing; negative means busy, zero means no space.
        int SafeWriteStart();                                                                       ///< @brief CN: 阻塞加锁并开始写入；无空间时内部自动解锁。 EN: Lock and begin writing; automatically unlock when no space is available.
        int SafeWriteEnd();                                                                         ///< @brief CN: 提交写入并释放内部锁。 EN: Commit the write and release the internal lock.

        int SafeGetWriteSize();                                                                     ///< @brief CN: 加锁查询当前空余数量。 EN: Return the current free capacity while holding the lock.
        int SafeWrite(const T *,int);                                                               ///< @brief CN: 单次加锁写入，返回实际写入数量。 EN: Write once under the lock and return the amount written.

    public: //读处理函数

        int ReadStart(bool=true);                                                                   ///< @brief CN: 开始一次非互斥读取；false 表示仅窥视不消费。 EN: Begin an externally serialized read; false means peek without consuming.
        int GetReadSize()const{return read_max;}                                                    ///< @brief CN: 返回本次 ReadStart 后剩余可读数量。 EN: Return readable slots remaining in the current read region.
        int Read(T *,int,bool=true);                                                                ///< @brief CN: 读取本次读区间；最后参数 false 时不推进读取游标。 EN: Read from the current region; a false final argument does not advance the cursor.
        int Peek(T *ptr,int size){return Read(ptr,size,false);}                                     ///< @brief CN: 读取副本但不消费数据。 EN: Copy data without consuming it.
        int Skip(int size){return Read(0,size,true);}                                               ///< @brief CN: 消费指定数量数据而不复制到目标缓冲区。 EN: Consume the specified amount without copying it to a destination.
        int ReadEnd();                                                                              ///< @brief CN: 提交消费游标；基础接口不负责解锁。 EN: Commit the read cursor; the basic API does not unlock.

        int SafeTryReadStart(bool=true);                                                            ///< @brief CN: 非阻塞加锁并开始读取；负值表示锁忙，零表示无数据。 EN: Try-lock and begin reading; negative means busy, zero means no data.
        int SafeReadStart(bool=true);                                                               ///< @brief CN: 阻塞加锁并开始读取；无数据时内部自动解锁。 EN: Lock and begin reading; automatically unlock when no data is available.
        int SafeReadEnd();                                                                          ///< @brief CN: 提交读取并释放内部锁。 EN: Commit the read and release the internal lock.

        int SafeGetReadSize();                                                                      ///< @brief CN: 加锁查询当前积压数量。 EN: Return the current readable amount while holding the lock.
        bool SafeTryGetReadSize(int &);                                                             ///< @brief CN: 非阻塞加锁查询；锁忙时返回 false。 EN: Try to query under the lock; return false when busy.
        int SafeRead(T *,int,bool=true);                                                            ///< @brief CN: 单次加锁读取，返回实际消费数量。 EN: Read once under the lock and return the amount consumed.
        int SafePeek(T *ptr,int size){return SafeRead(ptr,size,false);}                             ///< @brief CN: 单次加锁窥视，不消费数据。 EN: Peek once under the lock without consuming data.
    };//class RingBuffer
}//namespace hgl

#include<hgl/concurrency/detail/RingBufferCore.inc>
#include<hgl/concurrency/detail/RingBufferRead.inc>
#include<hgl/concurrency/detail/RingBufferWrite.inc>

namespace hgl
{
    namespace io
    {
        /** @brief CN: 非拥有型 InputStream 适配器；调用期间 RingBuffer 必须保持有效。 EN: Non-owning InputStream adapter; the RingBuffer must remain valid while in use. */
        class RingInputStream:public InputStream
        {
            RingBuffer<char> *rb;

        public:

            RingInputStream     (RingBuffer<char> *_rb){rb=_rb;}

            RingBuffer<char> *GetRingBuffer(){return rb;}

            int     Start       (){return rb?rb->ReadStart():-1;}
            int     End         (){return rb?rb->ReadEnd():-1;}
            int     SafeStart   (){return rb?rb->SafeReadStart():-1;}
            int     SafeEnd     (){return rb?rb->SafeReadEnd():-1;}

            void    Close       (){}

        public:

            int64   Read        (void *buf,int64 size){return rb?rb->Read((char *)buf,size):-1;}    ///< @brief CN: 读取数据。 EN: Read data.
            int64   Peek        (void *buf,int64 size){return rb?rb->Peek((char *)buf,size):-1;}    ///< @brief CN: 预览数据但不消费。 EN: Peek at data without consuming it.

            bool    CanRestart  ()const{return false;}                                              ///< @brief CN: 是否可以复位。 EN: Whether the stream can be restarted.
            bool    CanSeek     ()const{return false;}                                               ///< @brief CN: 是否可以定位。 EN: Whether the stream supports seeking.
            bool    CanSize     ()const{return false;}                                               ///< @brief CN: 是否可以取得尺寸。 EN: Whether the stream size is available.
            bool    CanPeek     ()const{return false;}                                              ///< @brief CN: 是否支持预览数据。 EN: Whether the stream supports peeking.

            bool    Restart     (){return false;}                                                   ///< @brief CN: 复位访问指针。 EN: Restart the access position.
            int64   Skip        (int64 size){return rb?rb->Skip(size):-1;}                          ///< @brief CN: 跳过指定字节而不访问。 EN: Skip the specified number of bytes.
            int64   Seek        (int64,SeekOrigin){return -1;}                                      ///< @brief CN: 移动访问指针。 EN: Move the access position.
            int64   Tell        ()const{return -1;}                                                 ///< @brief CN: 返回当前访问位置。 EN: Return the current access position.
            int64   GetSize     ()const{return -1;}                                                 ///< @brief CN: 取得流长度。 EN: Return the stream length.
            int64   Available   ()const{return rb?rb->GetReadSize():-1;}                            ///< @brief CN: 返回当前可立即读取的字节数。 EN: Return the bytes immediately available for reading.
        };//class RingInputStream

        /** @brief CN: 非拥有型 OutputStream 适配器；Close 不销毁或清空底层 RingBuffer。 EN: Non-owning OutputStream adapter; Close does not destroy or clear the RingBuffer. */
        class RingOutputStream:public OutputStream
        {
            RingBuffer<char> *rb;

        public:

            RingOutputStream    (RingBuffer<char> *_rb){rb=_rb;}

            RingBuffer<char> *GetRingBuffer(){return rb;}

            int     Start       (){return rb?rb->WriteStart():-1;}
            int     End         (){return rb?rb->WriteEnd():-1;}
            int     SafeStart   (){return rb?rb->SafeWriteStart():-1;}
            int     SafeEnd     (){return rb?rb->SafeWriteEnd():-1;}

            void    Close       (){}

        public:

            int64   Write       (const void *buf,int64 size){return rb?rb->Write((char *)buf,size):-1;}     ///< @brief CN: 写入数据。 EN: Write data.

            bool    CanRestart  ()const{return false;};                                             ///< @brief CN: 是否可以复位。 EN: Whether the stream can be restarted.
            bool    CanSeek     ()const{return false;};                                             ///< @brief CN: 是否可以定位。 EN: Whether the stream supports seeking.
            bool    CanSize     ()const{return false;}                                              ///< @brief CN: 是否可以取得尺寸。 EN: Whether the stream size is available.

            bool    Restart     (){return false;}                                                   ///< @brief CN: 复位访问指针。 EN: Restart the access position.
            int64   Seek        (int64,SeekOrigin){return -1;}                                      ///< @brief CN: 移动访问指针。 EN: Move the access position.
            int64   Tell        ()const{return -1;}                                                 ///< @brief CN: 返回当前访问位置。 EN: Return the current access position.
            int64   GetSize     ()const{return -1;}                                                 ///< @brief CN: 取得流长度。 EN: Return the stream length.
            int64   Available   ()const{return rb?rb->GetWriteSize():-1;}                           ///< @brief CN: 返回当前可立即写入的字节数。 EN: Return the bytes immediately available for writing.
        };//class RingOutputStream
    }//namespace io

    class RingBufferRead
    {
        RingBuffer<char> *rb;
        int size;

    public:

        RingBufferRead(RingBuffer<char> *_rb)
        {
            rb=_rb;

            size=rb->ReadStart();
        }

        RingBufferRead(io::RingInputStream *ris)
        {
            rb=ris->GetRingBuffer();

            size=rb->ReadStart();
        }

        ~RingBufferRead()
        {
            Unlock();
        }

        void Unlock()
        {
            if(size>0)
            {
                rb->ReadEnd();
                size=0;
            }
        }

        int GetSize() const
        {
            return size;
        }
    };//class RingBufferRead

    class RingBufferSafeRead
    {
        RingBuffer<char> *rb;
        int size;

    public:

        RingBufferSafeRead(RingBuffer<char> *_rb)
        {
            rb=_rb;
            size=-1;
        }

        RingBufferSafeRead(io::RingInputStream *ris)
        {
            rb=ris->GetRingBuffer();
            size=-1;
        }

        int Start()
        {
            size=rb->SafeReadStart();
            return size;
        }

        int TryStart()
        {
            size=rb->SafeTryReadStart();
            return size;
        }

        int GetSize()
        {
            size=rb->GetReadSize();
            return size;
        }

        /** @brief CN: 安全读区守卫；Start 成功返回正值后，作用域结束时自动调用 SafeReadEnd。 EN: Safe read-region guard; calls SafeReadEnd on scope exit after a positive Start result. */
        ~RingBufferSafeRead()
        {
            Unlock();
        }

        void Unlock()
        {
            if(size>0)
            {
                rb->SafeReadEnd();
                size=0;
            }
        }
    };//class RingBufferSafeRead

    class RingBufferWrite
    {
        RingBuffer<char> *rb;
        int size;

    public:

        RingBufferWrite(RingBuffer<char> *_rb)
        {
            rb=_rb;

            size=rb->WriteStart();
        }

        RingBufferWrite(io::RingOutputStream *ros)
        {
            rb=ros->GetRingBuffer();

            size=rb->WriteStart();
        }

        ~RingBufferWrite()
        {
            Unlock();
        }

        void Unlock()
        {
            if(size>0)
            {
                rb->WriteEnd();
                size=0;
            }
        }

        int GetSize() const
        {
            return size;
        }
    };//class RingBufferWrite

    class RingBufferSafeWrite
    {
        RingBuffer<char> *rb;
        int size;

    public:

        RingBufferSafeWrite(RingBuffer<char> *_rb)
        {
            rb=_rb;

            size=-1;
        }

        RingBufferSafeWrite(io::RingOutputStream *ros)
        {
            rb=ros->GetRingBuffer();

            size=-1;
        }

        int Start()
        {
            size=rb->SafeWriteStart();
            return(size);
        }

        int TryStart()
        {
            size=rb->SafeTryWriteStart();
            return(size);
        }

        /** @brief CN: 安全写区守卫；Start 成功返回正值后，作用域结束时自动调用 SafeWriteEnd。 EN: Safe write-region guard; calls SafeWriteEnd on scope exit after a positive Start result. */
        ~RingBufferSafeWrite()
        {
            Unlock();
        }

        int Unlock()
        {
            int result=size;

            if(size>0)
            {
                rb->SafeWriteEnd();
                size=0;
            }

            return result;
        }
    };//class RingBufferSafeWrite
}//namespace hgl
