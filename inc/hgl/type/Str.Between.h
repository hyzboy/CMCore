#pragma once
#include <hgl/type/Str.Length.h>
#include <cstddef>

namespace hgl
{
    /**
     * @brief CN: 在 str 中查找 start_char 与 end_char 之间的子串（受长度限制）。
     * @brief EN: Find substring between start_char and end_char in str (bounded by length).
     *
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] str CN: 源字符串指针（可能不以 NUL 结尾），若为 nullptr 返回 nullptr。EN: source string pointer (may not be NUL-terminated).
     * @param[in] str_length CN: 要处理的最大字符数。EN: maximum number of characters to consider.
     * @param[in] start_char CN: 起始分隔字符（不包含在返回子串内）。EN: starting delimiter character (not included in returned substring).
     * @param[in] end_char CN: 结束分隔字符（不包含在返回子串内）。EN: ending delimiter character (not included in returned substring).
     * @param[out] between_length CN: 输出子串长度（字符数）。EN: out parameter receiving the length of the substring.
     * @return const CharT* CN: 返回子串起始指针，未找到返回 nullptr。EN: pointer to the start of substring or nullptr if not found.
     */
    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, std::size_t str_length, const CharT start_char, const CharT end_char, std::size_t *between_length)
    {
        if(!str || str_length == 0 || !between_length) return nullptr;

        const CharT *start = nullptr;
        const CharT *p = str;
        std::size_t remaining = str_length;

        // find start_char
        while(*p && remaining > 0)
        {
            --remaining;
            if(hgl::char_eq(*p, start_char))
            {
                ++p; // move past start_char
                start = p;
                break;
            }
            ++p;
        }

        if(!start) return nullptr;

        // find end_char
        while(*p && remaining > 0)
        {
            --remaining;
            if(hgl::char_eq(*p, end_char))
            {
                *between_length = static_cast<std::size_t>(p - start);
                return start;
            }
            ++p;
        }

        return nullptr;
    }

    // compatibility overloads using int for legacy callers
    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, int str_length, const CharT start_char, const CharT end_char, int *between_length)
    {
        if(str_length <= 0 || !between_length) return nullptr;
        std::size_t sl = static_cast<std::size_t>(str_length);
        std::size_t bl = 0;
        const CharT *res = getbetween<CharT>(str, sl, start_char, end_char, &bl);
        if(res)
            *between_length = static_cast<int>(bl);
        else
            *between_length = 0;
        return res;
    }

    /**
     * @brief CN: 查找在给定长度内两个相同边界字符之间的子串（等同于 start_char == end_char）。
     * @brief EN: Find substring between two identical edge characters within given length.
     */
    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, std::size_t str_length, const CharT edge_char, std::size_t *between_length)
    {
        return hgl::getbetween<CharT>(str, str_length, edge_char, edge_char, between_length);
    }

    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, int str_length, const CharT edge_char, int *between_length)
    {
        return hgl::getbetween<CharT>(str, str_length, edge_char, between_length);
    }

    /**
     * @brief CN: 在以 NUL 结尾的字符串中查找 start_char 与 end_char 之间的子串（不指定长度）。
     * @brief EN: Find substring between start_char and end_char in a NUL-terminated string.
     */
    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, const CharT start_char, const CharT end_char, std::size_t *between_length)
    {
        if(!str) return nullptr;
        return hgl::getbetween<CharT>(str, static_cast<std::size_t>(hgl::strlen(str)), start_char, end_char, between_length);
    }

    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, const CharT start_char, const CharT end_char, int *between_length)
    {
        if(!str || !between_length) return nullptr;
        int len = hgl::strlen(str);
        return hgl::getbetween<CharT>(str, len, start_char, end_char, between_length);
    }

    /**
     * @brief CN: 在以 NUL 结尾的字符串中查找两个相同边界字符之间的子串（不指定长度）。
     * @brief EN: Find substring between identical edge characters in a NUL-terminated string.
     */
    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, const CharT edge_char, std::size_t *between_length)
    {
        return hgl::getbetween<CharT>(str, edge_char, edge_char, between_length);
    }

    template<typename CharT>
    inline const CharT *getbetween(const CharT *str, const CharT edge_char, int *between_length)
    {
        if(!str || !between_length) return nullptr;
        int len = hgl::strlen(str);
        return hgl::getbetween<CharT>(str, len, edge_char, between_length);
    }
}
