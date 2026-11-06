#include<hgl/io/HashOutputStream.h>

namespace hgl::io
{
    HashOutputStream::HashOutputStream(OutputStream *dest, util::hash::Algorithm algo, bool own_dest)
        : dest_(dest), own_dest_(own_dest)
    {
        hasher_ = util::hash::CreateHasher(algo);

        if(hasher_)
            hasher_->Init();
    }

    HashOutputStream::~HashOutputStream()
    {
        util::hash::DestroyHasher(hasher_);

        if(own_dest_ && dest_)
            delete dest_;
    }

    util::hash::Algorithm HashOutputStream::GetAlgorithm() const
    {
        return hasher_ ? hasher_->GetAlgorithm() : util::hash::Algorithm::MD5;
    }

    size_t HashOutputStream::GetDigestSize() const
    {
        return hasher_ ? hasher_->GetDigestSize() : 0;
    }

    void HashOutputStream::Close()
    {
        if(dest_)
            dest_->Close();
    }

    int64 HashOutputStream::Write(const void *buf, int64 size)
    {
        if(!dest_)
            return 0;

        const int64 write_size = dest_->Write(buf, size);

        if(write_size > 0 && hasher_)
            hasher_->Update(buf, static_cast<size_t>(write_size));

        return write_size;
    }

    void HashOutputStream::GetDigest(void *digest_buf)
    {
        if(hasher_)
            hasher_->Final(digest_buf);
    }

    void HashOutputStream::ResetHash()
    {
        if(hasher_)
            hasher_->Init();
    }

    bool HashOutputStream::CanRestart() const { return dest_ ? dest_->CanRestart() : false; }
    bool HashOutputStream::CanSeek()    const { return dest_ ? dest_->CanSeek()    : false; }
    bool HashOutputStream::CanSize()    const { return dest_ ? dest_->CanSize()    : false; }

    bool HashOutputStream::Restart()
    {
        if(!dest_)
            return false;

        if(!dest_->Restart())
            return false;

        ResetHash();
        return true;
    }

    int64 HashOutputStream::Seek(int64 pos, SeekOrigin origin)
    {
        return dest_ ? dest_->Seek(pos, origin) : -1;
    }

    int64 HashOutputStream::Tell()      const { return dest_ ? dest_->Tell()      : -1; }
    int64 HashOutputStream::GetSize()   const { return dest_ ? dest_->GetSize()   : -1; }
    int64 HashOutputStream::Available() const { return dest_ ? dest_->Available() :  0; }
}//namespace hgl::io
