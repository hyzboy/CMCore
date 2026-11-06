#include<hgl/util/hash/HashFile.h>
#include<hgl/util/hash/Hasher.h>
#include<hgl/io/FileInputStream.h>

namespace hgl::util::hash
{
    namespace
    {
        /// 极小的 RAII 守卫：避免把 DestroyHasher 遗漏在任何提前 return 上。
        struct HasherGuard
        {
            Hasher *hasher;

            explicit HasherGuard(Hasher *h):hasher(h){}
            ~HasherGuard(){ DestroyHasher(hasher); }

            HasherGuard(const HasherGuard &) = delete;
            HasherGuard &operator=(const HasherGuard &) = delete;
        };

        bool HashFileStreaming(Algorithm algo, const OSString &filename, void *hash_code)
        {
            io::OpenFileInputStream opener(filename);

            if(!opener)
                return false;

            HasherGuard guard(CreateHasher(algo));

            if(!guard.hasher)
                return false;

            guard.hasher->Init();

            uint8 *buffer = static_cast<uint8 *>(hgl_malloc(HASH_FILE_BUFFER_SIZE));

            if(!buffer)
                return false;

            bool ok = true;

            while(true)
            {
                const int64 read_size = opener->Read(buffer, HASH_FILE_BUFFER_SIZE);

                if(read_size < 0)
                {
                    ok = false;
                    break;
                }

                if(read_size == 0)
                    break;

                guard.hasher->Update(buffer, static_cast<size_t>(read_size));
            }

            hgl_free(buffer);

            if(!ok)
                return false;

            guard.hasher->Final(hash_code);
            return true;
        }

        bool HashFileBuffered(Algorithm algo, const OSString &filename, void *hash_code)
        {
            io::OpenFileInputStream opener(filename);

            if(!opener)
                return false;

            const int64 size = opener->GetSize();

            if(size < 0)
                return false;

            if(static_cast<uint64>(size) > HASH_FILE_MAX_BUFFERED_BYTES)
                return false;

            if(size == 0)
                return Hash(algo, nullptr, 0, hash_code);

            uint8 *data = static_cast<uint8 *>(hgl_malloc(static_cast<size_t>(size)));

            if(!data)
                return false;

            const int64 read_size = opener->Read(data, size);

            if(read_size != size)
            {
                hgl_free(data);
                return false;
            }

            const bool ok = Hash(algo, data, static_cast<size_t>(size), hash_code);

            hgl_free(data);
            return ok;
        }
    }//namespace

    bool HashFile(Algorithm algo, const OSString &filename, void *hash_code)
    {
        if(!hash_code || GetDigestSize(algo) == 0)
            return false;

        if(IsStreamable(algo))
            return HashFileStreaming(algo, filename, hash_code);

        return HashFileBuffered(algo, filename, hash_code);
    }

    bool HashFileHex(Algorithm algo, const OSString &filename, char *out_hex, size_t out_hex_size)
    {
        const size_t digest_size = GetDigestSize(algo);

        if(digest_size == 0 || !out_hex || out_hex_size < digest_size * 2 + 1)
            return false;

        uint8 digest[64];

        if(digest_size > sizeof(digest))
            return false;

        if(!HashFile(algo, filename, digest))
            return false;

        DigestToLowerHex(digest, digest_size, out_hex);
        return true;
    }
}//namespace hgl::util::hash
