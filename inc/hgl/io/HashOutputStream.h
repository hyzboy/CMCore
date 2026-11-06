#pragma once

#include<hgl/io/OutputStream.h>
#include<hgl/util/hash/Hasher.h>

namespace hgl::io
{
    /**
     * 边写边算的输出流。
     *
     * 与输入流版本同理：包装 CMHash 的 `Hasher`，算法可在运行期决定。
     */
    class HashOutputStream:public OutputStream
    {
        OutputStream *dest_ = nullptr;
        util::hash::Hasher *hasher_ = nullptr;
        bool own_dest_ = false;

    public:

        HashOutputStream(OutputStream *dest, util::hash::Algorithm algo, bool own_dest = false);
        ~HashOutputStream() override;

        HashOutputStream(const HashOutputStream &) = delete;
        HashOutputStream &operator=(const HashOutputStream &) = delete;

        bool IsValid() const { return dest_ != nullptr && hasher_ != nullptr; }

        util::hash::Algorithm GetAlgorithm() const;
        size_t GetDigestSize() const;

        void Close() override;

        int64 Write(const void *buf, int64 size) override;

        /// 结束本次计算并取出摘要；之后不应继续 Write()
        void GetDigest(void *digest_buf);

        /// 复位散列状态（不影响目标流位置）
        void ResetHash();

        bool CanRestart() const override;
        bool CanSeek() const override;
        bool CanSize() const override;

        bool Restart() override;
        int64 Seek(int64 pos, SeekOrigin origin = SeekOrigin::Begin) override;
        int64 Tell() const override;
        int64 GetSize() const override;
        int64 Available() const override;
    };//class HashOutputStream
}//namespace hgl::io
