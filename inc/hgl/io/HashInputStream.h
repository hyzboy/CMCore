#pragma once

#include<hgl/io/InputStream.h>
#include<hgl/util/hash/Hasher.h>

namespace hgl::io
{
    /**
     * 边读边算的输入流。
     *
     * 与旧模板版本的区别：这里包装的是 CMHash 运行期多态的 `Hasher`，
     * 因此算法可以在运行期决定（例如按清单里写明的 hash_algorithm），
     * 不必为每种算法各写一个模板实例；也不再有「复制 hasher 取摘要」
     * 这种与 Base 删除拷贝构造相冲突的写法。
     *
     * 只接受可流式累加的算法；一次性算法（Murmur3 / CityHash）
     * 由 util::hash::HashFile 整块计算，不走这里。
     */
    class HashInputStream:public InputStream
    {
        InputStream *source_ = nullptr;
        util::hash::Hasher *hasher_ = nullptr;
        bool own_source_ = false;

    public:

        HashInputStream(InputStream *source, util::hash::Algorithm algo, bool own_source = false);
        ~HashInputStream() override;

        HashInputStream(const HashInputStream &) = delete;
        HashInputStream &operator=(const HashInputStream &) = delete;

        bool IsValid() const { return source_ != nullptr && hasher_ != nullptr; }

        util::hash::Algorithm GetAlgorithm() const;
        size_t GetDigestSize() const;

        void Close() override;

        int64 Read(void *buf, int64 size) override;
        int64 Peek(void *buf, int64 size) override;

        /// 结束本次计算并取出摘要；之后不应继续 Read()
        void GetDigest(void *digest_buf);

        /// 复位散列状态（不影响源流位置）
        void ResetHash();

        bool CanRestart() const override;
        bool CanSeek() const override;
        bool CanSize() const override;
        bool CanPeek() const override;

        bool Restart() override;
        int64 Skip(int64 bytes) override;
        int64 Seek(int64 pos, SeekOrigin origin = SeekOrigin::Begin) override;
        int64 Tell() const override;
        int64 GetSize() const override;
        int64 Available() const override;
    };//class HashInputStream
}//namespace hgl::io
