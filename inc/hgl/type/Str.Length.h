#pragma once
#include <hgl/type/CharType.h>
#include <cstddef>
namespace hgl
{
    /**
     * @brief CN: 计算以 NUL 结尾字符串的长度（字符数）。
     * @brief EN: Compute the length (number of characters) of a NUL-terminated string.
     *
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] str CN: 指向以 NUL 结尾的字符串，若为 nullptr 返回 0。EN: pointer to a NUL-terminated string; returns 0 if nullptr.
     * @return int CN/EN: 字符串长度（不含终止符）。EN: length excluding the terminator.
     */
    template<typename CharT>
    inline const int strlen(const CharT *str)
    {
        if(str && *str)
        {
            const CharT *start = str;
            while(*str) ++str;
            return int(str - start);
        }

        return 0;
    }

    /**
     * @brief CN: 计算以 NUL 结尾字符串在给定最大长度内的实际长度（不会超过 max_len）。
     * @brief EN: Compute the length of a NUL-terminated string up to a maximum limit (does not exceed max_len).
     *
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] str CN: 指向以 NUL 结尾的字符串，若为 nullptr 返回 0。EN: pointer to a NUL-terminated string; returns 0 if nullptr.
     * @param[in] max_len CN: 最大检查长度。EN: maximum number of characters to check.
     * @return int CN/EN: 字符串在限制内的长度（不含终止符）。EN: length within the given limit.
     */
    template<typename CharT>
    inline const int strlen(const CharT *str, std::size_t max_len)
    {
        if(str && *str)
        {
            const CharT *start = str;
            while(max_len > 0 && *str)
            {
                ++str;
                --max_len;
            }
            return int(str - start);
        }

        return 0;
    }
}//namespace hgl
