#pragma once

#include<hgl/util/hash/Hash.h>
#include<hgl/type/String.h>

namespace hgl::util::hash
{
    /**
     * 文件哈希 —— 建立在 CMHash 之上的流式封装。
     *
     * 为什么这一层在 CMCore 而不是 CMHash：
     * CMHash 只做「内存块 / 状态机」的算法计算，没有文件系统与流的概念。
     * 需要读盘的部分统一放在这里，CMHash 因此保持零 IO 依赖。
     */

    /// 流式读取的分块大小
    constexpr size_t HASH_FILE_BUFFER_SIZE = 64 * 1024;

    /**
     * 对不支持分块的算法（Murmur3 / CityHash），只能整块计算，
     * 这里给出可缓冲的上限；超过上限明确失败，而不是悄悄截断。
     */
    constexpr uint64 HASH_FILE_MAX_BUFFERED_BYTES = 1024ull * 1024ull * 1024ull;

    /**
     * 计算文件的摘要。
     *
     * 支持分块的算法走 64KB 分块流式累加；其余算法整块读入后一次性计算。
     * 空文件是合法输入（各算法都有确定的空串摘要）。
     *
     * @param hash_code 输出缓冲区，容量必须 >= GetDigestSize(algo)
     * @return 打开失败、读取失败、超出缓冲上限或算法非法时返回 false
     */
    bool HashFile(Algorithm algo, const OSString &filename, void *hash_code);

    template<Algorithm algo>
    inline bool HashFile(const OSString &filename, void *hash_code)
    {
        return HashFile(algo, filename, hash_code);
    }

    /**
     * 计算文件摘要并输出小写十六进制。
     *
     * @param out_hex      输出缓冲区
     * @param out_hex_size 缓冲区字节数，必须 >= GetDigestSize(algo)*2+1
     */
    bool HashFileHex(Algorithm algo, const OSString &filename, char *out_hex, size_t out_hex_size);

    template<Algorithm algo>
    inline bool HashFileHex(const OSString &filename, char *out_hex, size_t out_hex_size)
    {
        return HashFileHex(algo, filename, out_hex, out_hex_size);
    }
}//namespace hgl::util::hash
