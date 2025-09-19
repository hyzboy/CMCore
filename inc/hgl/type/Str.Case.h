#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <cstddef>
namespace hgl
{

    /**
     * @brief CN: 将字符串中的所有字母字符转换为大写。
     * @brief EN: Convert all alphabetical characters in a null-terminated string to uppercase.
     *
     * @param[in,out] s CN: 以 null 结尾的字符串缓冲区，将被就地修改。EN: null-terminated string buffer to modify in-place.
     */
    template<typename CharT>
    inline void to_upper_char(CharT *s)
    {
        if(!s) return;

        while(*s)
        {
            if(*s>='a' && *s<='z')
                *s = static_cast<CharT>(*s - 32);

            ++s;
        }
    }

    /**
     * @brief CN: 将字符串中的所有字母字符转换为小写。
     * @brief EN: Convert all alphabetical characters in a null-terminated string to lowercase.
     *
     * @param[in,out] s CN: 以 null 结尾的字符串缓冲区，将被就地修改。EN: null-terminated string buffer to modify in-place.
     */
    template<typename CharT>
    inline void to_lower_char(CharT *s)
    {
        if(!s) return;

        while(*s)
        {
            if(*s>='A' && *s<='Z')
                *s = static_cast<CharT>(*s + 32);

            ++s;
        }
    }

    /**
     * @brief CN: 将 src 指向的字符串转换为大写并写入到 dst（不修改 src）。
     * @brief EN: Convert string pointed by src to uppercase and write result to dst (src unchanged).
     *
     * @param[in] src CN: 源 null 结尾字符串。EN: source null-terminated string.
     * @param[out] dst CN: 目标缓冲区（必须能容纳结果）。EN: destination buffer to receive result.
     */
    template<typename CharT>
    inline void to_upper_char(const CharT *src, CharT *dst)
    {
        if(!dst) return;

        if(!src)
        {
            *dst = 0;
            return;
        }

        do
        {
            if(*src>='a' && *src<='z')
                *dst = static_cast<CharT>(*src - 32);
            else
                *dst = *src;

            ++src;
            ++dst;
        } while(*src);

        *dst = 0;
    }

    /**
     * @brief CN: 将 src 指向的字符串转换为小写并写入到 dst（不修改 src）。
     * @brief EN: Convert string pointed by src to lowercase and write result to dst (src unchanged).
     *
     * @param[in] src CN: 源 null 结尾字符串。EN: source null-terminated string.
     * @param[out] dst CN: 目标缓冲区（必须能容纳结果）。EN: destination buffer to receive result.
     */
    template<typename CharT>
    inline void to_lower_char(const CharT *src, CharT *dst)
    {
        if(!dst) return;

        if(!src)
        {
            *dst = 0;
            return;
        }

        do
        {
            if(*src>='A' && *src<='Z')
                *dst = static_cast<CharT>(*src + 32);
            else
                *dst = *src;

            ++src;
            ++dst;
        } while(*src);

        *dst = 0;
    }

    /**
     * @brief CN: 复制 source 到 target，并将字符转换为小写。
     * @brief EN: Copy source to target while converting characters to lowercase.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源 null 结尾字符串。EN: source null-terminated string.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint lower_cpy(CharT *target, const CharT *source)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source)
        {
            if(*source>='A' && *source<='Z')
                *target = static_cast<CharT>(*source + 32);
            else
                *target = *source;

            ++target;
            ++source;
            ++count;
        }

        *target = 0;
        return count;
    }

    /**
     * @brief CN: 复制 source 到 target，并将字符转换为大写。
     * @brief EN: Copy source to target while converting characters to uppercase.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源 null 结尾字符串。EN: source null-terminated string.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint upper_cpy(CharT *target, const CharT *source)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source)
        {
            if(*source>='a' && *source<='z')
                *target = static_cast<CharT>(*source - 32);
            else
                *target = *source;

            ++target;
            ++source;
            ++count;
        }

        *target = 0;
        return count;
    }

    /**
     * @brief CN: 复制 source 中最多 source_max 个字符到 target，并将字符转换为小写。
     * @brief EN: Copy at most source_max characters from source to target while converting to lowercase.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源字符串。EN: source string.
     * @param[in] source_max CN: 要读取的最大字符数。EN: maximum number of characters to read from source.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint lower_cpy(CharT *target, const CharT *source, std::size_t source_max)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source && source_max>0)
        {
            if(*source>='A' && *source<='Z')
                *target = static_cast<CharT>(*source + 32);
            else
                *target = *source;

            ++target;
            ++source;
            ++count;
            --source_max;
        }

        *target = 0;
        return count;
    }

    /**
     * @brief CN: 复制 source 中最多 source_max 个字符到 target，并将字符转换为大写。
     * @brief EN: Copy at most source_max characters from source to target while converting to uppercase.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源字符串。EN: source string.
     * @param[in] source_max CN: 要读取的最大字符数。EN: maximum number of characters to read from source.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint upper_cpy(CharT *target, const CharT *source, std::size_t source_max)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source && source_max>0)
        {
            if(*source>='a' && *source<='z')
                *target = static_cast<CharT>(*source - 32);
            else
                *target = *source;

            ++target;
            ++source;
            ++count;
            --source_max;
        }

        *target = 0;
        return count;
    }

    /**
     * @brief CN: 复制 source 到 target，将字符转换为小写，并移除空格字符。
     * @brief EN: Copy source to target converting to lowercase and skipping space characters.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源 null 结尾字符串。EN: source null-terminated string.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint lower_clip_cpy(CharT *target, const CharT *source)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source)
        {
            if(*source != static_cast<CharT>(' '))
            {
                if(*source>='A' && *source<='Z')
                    *target = static_cast<CharT>(*source + 32);
                else
                    *target = *source;

                ++target;
                ++count;
            }

            ++source;
        }

        *target = 0;
        return count;
    }

    /**
     * @brief CN: 复制 source 到 target，将字符转换为大写，并移除空格字符。
     * @brief EN: Copy source to target converting to uppercase and skipping space characters.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源 null 结尾字符串。EN: source null-terminated string.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint upper_clip_cpy(CharT *target, const CharT *source)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source)
        {
            if(*source != static_cast<CharT>(' '))
            {
                if(*source>='a' && *source<='z')
                    *target = static_cast<CharT>(*source - 32);
                else
                    *target = *source;

                ++target;
                ++count;
            }

            ++source;
        }

        *target = 0;
        return count;
    }

    /**
     * @brief CN: 复制 source 中最多 source_max 个字符到 target，转换为小写并移除空格。
     * @brief EN: Copy at most source_max characters from source to target, converting to lowercase and skipping spaces.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源字符串。EN: source string.
     * @param[in] source_max CN: 要读取的最大字符数。EN: maximum number of characters to read from source.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint lower_clip_cpy(CharT *target, const CharT *source, std::size_t source_max)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source && source_max>0)
        {
            if(*source != static_cast<CharT>(' '))
            {
                if(*source>='A' && *source<='Z')
                    *target = static_cast<CharT>(*source + 32);
                else
                    *target = *source;

                ++target;
                ++count;
            }

            ++source;
            --source_max;
        }

        *target = 0;
        return count;
    }

    /**
     * @brief CN: 复制 source 中最多 source_max 个字符到 target，转换为大写并移除空格。
     * @brief EN: Copy at most source_max characters from source to target, converting to uppercase and skipping spaces.
     *
     * @param[out] target CN: 目标缓冲区。EN: destination buffer.
     * @param[in] source CN: 源字符串。EN: source string.
     * @param[in] source_max CN: 要读取的最大字符数。EN: maximum number of characters to read from source.
     * @return uint CN: 返回写入到 target 的字符数（不含终止符）。EN: number of characters written to target (excluding terminator).
     */
    template<typename CharT>
    inline const uint upper_clip_cpy(CharT *target, const CharT *source, std::size_t source_max)
    {
        if(!target || !source) return 0;
        uint count = 0;

        while(*source && source_max>0)
        {
            if(*source != static_cast<CharT>(' '))
            {
                if(*source>='a' && *source<='z')
                    *target = static_cast<CharT>(*source - 32);
                else
                    *target = *source;

                ++target;
                ++count;
            }

            ++source;
            --source_max;
        }

        *target = 0;
        return count;
    }
}//namespace hgl
