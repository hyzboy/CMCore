#pragma once
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Comp.h>
#include <cstddef>
namespace hgl
{
    /**
     * @brief CN: 统计字符串中某个字符出现的次数。
     * @brief EN: Count occurrences of a character in a null-terminated string.
     *
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] str CN: 指向以 NUL 结尾的字符串. EN: pointer to a NUL-terminated string.
     * @param[in] ch CN: 要统计的字符. EN: character to count.
     * @return int CN/EN: 出现次数。EN: number of occurrences.
     */
    template<typename CharT>
    inline int stat_char(const CharT *str, CharT ch)
    {
        if(!str) return 0;

        int count = 0;
        while(*str)
        {
            if(*str == ch) ++count;
            ++str;
        }

        return count;
    }

    /**
     * @brief CN: 统计字符串的行数（以 '\n' 作为行分隔符）。
     * @brief EN: Count number of lines in a string (using '\n' as separator).
     *
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] str CN: 指向以 NUL 结尾的字符串. EN: pointer to a NUL-terminated string.
     * @return int CN/EN: 行数。EN: number of lines.
     */
    template<typename CharT>
    inline int stat_line(const CharT *str)
    {
        if(!str) return 0;

        int len = hgl::strlen(str);
        if(len <= 0) return 0;

        const CharT *p = str + len - 1;

        if(*p == static_cast<CharT>('\n'))
            return hgl::stat_char(str, static_cast<CharT>('\n'));
        else
            return hgl::stat_char(str, static_cast<CharT>('\n')) + 1;
    }
}//namespace hgl
