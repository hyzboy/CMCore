#include<hgl/io/HashInputStream.h>

namespace hgl::io
{
    HashInputStream::HashInputStream(InputStream *source, util::hash::Algorithm algo, bool own_source)
        : source_(source), own_source_(own_source)
    {
        hasher_ = util::hash::CreateHasher(algo);

        if(hasher_)
            hasher_->Init();
    }

    HashInputStream::~HashInputStream()
    {
        util::hash::DestroyHasher(hasher_);

        if(own_source_ && source_)
            delete source_;
    }

    util::hash::Algorithm HashInputStream::GetAlgorithm() const
    {
        return hasher_ ? hasher_->GetAlgorithm() : util::hash::Algorithm::MD5;
    }

    size_t HashInputStream::GetDigestSize() const
    {
        return hasher_ ? hasher_->GetDigestSize() : 0;
    }

    void HashInputStream::Close()
    {
        if(source_)
            source_->Close();
    }

    int64 HashInputStream::Read(void *buf, int64 size)
    {
        if(!source_)
            return 0;

        const int64 read_size = source_->Read(buf, size);

        if(read_size > 0 && hasher_)
            hasher_->Update(buf, static_cast<size_t>(read_size));

        return read_size;
    }

    int64 HashInputStream::Peek(void *buf, int64 size)
    {
        return source_ ? source_->Peek(buf, size) : 0;
    }

    void HashInputStream::GetDigest(void *digest_buf)
    {
        if(hasher_)
            hasher_->Final(digest_buf);
    }

    void HashInputStream::ResetHash()
    {
        if(hasher_)
            hasher_->Init();
    }

    bool HashInputStream::CanRestart() const { return source_ ? source_->CanRestart() : false; }
    bool HashInputStream::CanSeek()    const { return source_ ? source_->CanSeek()    : false; }
    bool HashInputStream::CanSize()    const { return source_ ? source_->CanSize()    : false; }
    bool HashInputStream::CanPeek()    const { return source_ ? source_->CanPeek()    : false; }

    bool HashInputStream::Restart()
    {
        if(!source_)
            return false;

        if(!source_->Restart())
            return false;

        ResetHash();
        return true;
    }

    int64 HashInputStream::Skip(int64 bytes)
    {
        if(!source_ || bytes <= 0)
            return 0;

        // 跳过也要参与哈希，因此真的读出来（相当于丢弃内容）。
        uint8 buffer[4096];
        int64 skipped = 0;

        while(bytes > 0)
        {
            const int64 want = bytes > (int64)sizeof(buffer) ? (int64)sizeof(buffer) : bytes;
            const int64 read_size = Read(buffer, want);

            if(read_size <= 0)
                break;

            skipped += read_size;
            bytes -= read_size;
        }

        return skipped;
    }

    int64 HashInputStream::Seek(int64 pos, SeekOrigin origin)
    {
        return source_ ? source_->Seek(pos, origin) : -1;
    }

    int64 HashInputStream::Tell()      const { return source_ ? source_->Tell()      : -1; }
    int64 HashInputStream::GetSize()   const { return source_ ? source_->GetSize()   : -1; }
    int64 HashInputStream::Available() const { return source_ ? source_->Available() :  0; }
}//namespace hgl::io
