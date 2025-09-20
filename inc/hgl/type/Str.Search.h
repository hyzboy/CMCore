#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Case.h>
#include <hgl/type/Str.Copy.h>
#include <hgl/type/Str.Comp.h>
#include <cstddef>
#include <type_traits>
#include <utility>
namespace hgl
{
    /**
     * @brief CN: 在字符串 haystack 中查找另一个字符串 needle。
     * @brief EN: Search for the string `needle` inside `haystack`.
     *
     * @param[in] haystack CN: 要搜索的字符串起始地址. EN: pointer to the string to search in.
     * @param[in] haystack_len CN: haystack 的长度（以字符为单位）. EN: length of haystack in characters.
     * @param[in] needle CN: 要查找的字符串起始地址. EN: pointer to the string to search for.
     * @param[in] needle_len CN: needle 的长度（以字符为单位）. EN: length of needle in characters.
     * @return const CharT* CN: 找到时返回指向 haystack 中首次匹配的指针，否则返回 nullptr。. EN: pointer to the first occurrence inside haystack if found, otherwise nullptr.
     */
    template<typename CharT, typename SearchCharT>
    inline const CharT *strstr(const CharT *haystack, const std::size_t haystack_len, const SearchCharT *needle, const std::size_t needle_len)
    {
        if(!haystack || !needle) return nullptr;
        if(haystack_len==0 || needle_len==0) return nullptr;
        if(haystack_len < needle_len) return nullptr;

        for(std::size_t i=0;i+needle_len<=haystack_len;++i)
        {
            const CharT *h = haystack + i;
            const SearchCharT *n = needle;
            std::size_t s = needle_len;

            while(s>0 && hgl::char_eq(*h,*n))
            {
                ++h; ++n; --s;
            }

            if(s==0) return haystack + i;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename SearchCharT>
    inline CharT *strstr(CharT *haystack, const std::size_t haystack_len, const SearchCharT *needle, const std::size_t needle_len)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strstr<const CharT,SearchCharT>(static_cast<const CharT*>(haystack), haystack_len, needle, needle_len));
    }

    /**
     * @brief CN: 在字符串 haystack 中反向查找 needle（从后向前搜索）。
     * @brief EN: Search for `needle` inside `haystack` starting from the end (reverse search).
     *
     * @param[in] haystack CN: 要搜索的字符串起始地址. EN: pointer to the string to search in.
     * @param[in] haystack_len CN: haystack 的长度（以字符为单位）. EN: length of haystack in characters.
     * @param[in] needle CN: 要查找的字符串起始地址. EN: pointer to the string to search for.
     * @param[in] needle_len CN: needle 的长度（以字符为单位）. EN: length of needle in characters.
     * @return const CharT* CN: 找到时返回指向 haystack 中最后一次匹配的指针，否则返回 nullptr。. EN: pointer to the last occurrence inside haystack if found, otherwise nullptr.
     */
    template<typename CharT, typename SearchCharT>
    inline const CharT *strrstr(const CharT *haystack, const std::size_t haystack_len, const SearchCharT *needle, const std::size_t needle_len)
    {
        if(!haystack || !needle) return nullptr;
        if(haystack_len==0 || needle_len==0) return nullptr;
        if(haystack_len < needle_len) return nullptr;

        // iterate starting positions in reverse
        std::size_t count = haystack_len - needle_len + 1;
        for(std::size_t idx = count; idx>0; --idx)
        {
            std::size_t i = idx - 1;
            const CharT *h = haystack + i;
            const SearchCharT *n = needle;
            std::size_t s = needle_len;

            while(s>0 && hgl::char_eq(*h,*n))
            {
                ++h; ++n; --s;
            }

            if(s==0) return haystack + i;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename SearchCharT>
    inline CharT *strrstr(CharT *haystack, const std::size_t haystack_len, const SearchCharT *needle, const std::size_t needle_len)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strrstr<const CharT,SearchCharT>(static_cast<const CharT*>(haystack), haystack_len, needle, needle_len));
    }

    /**
     * @brief CN: 在字符串 haystack 中查找 needle，忽略大小写（ASCII）。
     * @brief EN: Case-insensitive search (ASCII) for `needle` inside `haystack`.
     *
     * @param[in] haystack CN: 要搜索的字符串起始地址. EN: pointer to the string to search in.
     * @param[in] haystack_len CN: haystack 的长度（以字符为单位）. EN: length of haystack in characters.
     * @param[in] needle CN: 要查找的字符串起始地址. EN: pointer to the string to search for.
     * @param[in] needle_len CN: needle 的长度（以字符为单位）. EN: length of needle in characters.
     * @return const CharT* CN: 找到时返回指向 haystack 中首次匹配的指针，否则返回 nullptr。. EN: pointer to the first occurrence inside haystack if found, otherwise nullptr.
     */
    template<typename CharT, typename SearchCharT>
    inline const CharT *stristr(const CharT *haystack, const std::size_t haystack_len, const SearchCharT *needle, const std::size_t needle_len)
    {
        if(!haystack || !needle) return nullptr;
        if(haystack_len==0 || needle_len==0) return nullptr;
        if(haystack_len < needle_len) return nullptr;

        for(std::size_t i=0;i+needle_len<=haystack_len;++i)
        {
            const CharT *h = haystack + i;
            const SearchCharT *n = needle;
            std::size_t s = needle_len;

            bool ok = true;

            while(s>0)
            {
                if(hgl::compare_char_icase(*h,*n) != 0)
                {
                    ok = false;
                    break;
                }
                ++h; ++n; --s;
            }

            if(ok) return haystack + i;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename SearchCharT>
    inline CharT *stristr(CharT *haystack, const std::size_t haystack_len, const SearchCharT *needle, const std::size_t needle_len)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::stristr<const CharT,SearchCharT>(static_cast<const CharT*>(haystack), haystack_len, needle, needle_len));
    }

    /**
     * @brief CN: 在以 null 结尾的字符串中查找字符 ch。
     * @brief EN: Find character `ch` in a null-terminated string `str`.
     *
     * @param[in] str CN: 要搜索的以 null 结尾的字符串. EN: null-terminated string to search.
     * @param[in] ch CN: 要查找的字符. EN: character to find.
     * @return const CharT* CN: 指向首次匹配字符的指针，未找到或输入为空则返回 nullptr。. EN: pointer to the first matching character or nullptr if not found or input is null.
     */
    template<typename CharT, typename C>
    inline const CharT *strchr(const CharT *str, C ch)
    {
        if(!str) return nullptr;

        while(*str)
        {
            if(hgl::char_eq(*str,ch)) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strchr(CharT *str, C ch)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strchr<const CharT,C>(static_cast<const CharT*>(str), ch));
    }

    /**
     * @brief CN: 在字符串中查找字符 ch，限定最大搜索长度 n（以字符为单位）。
     * @brief EN: Find character `ch` in `str` with a maximum length `n`.
     *
     * @param[in] str CN: 要搜索的字符串起始地址. EN: pointer to the string to search.
     * @param[in] ch CN: 要查找的字符. EN: character to find.
     * @param[in] n CN: 最大搜索长度（字符数）. EN: maximum number of characters to consider.
     * @return const CharT* CN/EN: pointer to the first matching character or nullptr on failure.
     */
    template<typename CharT, typename C>
    inline const CharT *strchr(const CharT *str, C ch, std::size_t n)
    {
        if(!str || n==0) return nullptr;

        while(n-- && *str)
        {
            if(hgl::char_eq(*str,ch)) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strchr(CharT *str, C ch, std::size_t n)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strchr<const CharT,C>(static_cast<const CharT*>(str), ch, n));
    }

    /**
     * @brief CN: 在字符串中查找多个字符集合中的任意一个字符。
     * @brief EN: Find any character from a set inside a null-terminated string.
     *
     * @param[in] str CN: 要搜索的以 null 结尾的字符串. EN: null-terminated string to search.
     * @param[in] set CN: 字符集合指针. EN: pointer to character set.
     * @param[in] set_count CN: 集合中的字符数量. EN: number of characters in the set.
     * @return const CharT* CN/EN: pointer to first matching character or nullptr on failure.
     */
    template<typename CharT, typename C>
    inline const CharT *strchr(const CharT *str, const C *set, const std::size_t set_count)
    {
        if(!str || !set || set_count==0) return nullptr;

        while(*str)
        {
            if(hgl::strchr(set, *str, set_count))
                return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strchr(CharT *str, C *set, const std::size_t set_count)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strchr<const CharT,C>(static_cast<const CharT*>(str), set, set_count));
    }

    /**
     * @brief CN: 在字符串中查找第一个不等于 ch 的字符。
     * @brief EN: Find the first character in `str` that is not equal to `ch`.
     *
     * @param[in] str CN: 要搜索的以 null 结尾的字符串. EN: null-terminated string to search.
     * @param[in] ch CN: 要排除的字符. EN: character to exclude.
     * @return const CharT* CN/EN: pointer to the first character not equal to ch or nullptr.
     */
    template<typename CharT, typename C>
    inline const CharT *strechr(const CharT *str, C ch)
    {
        if(!str) return nullptr;

        while(*str)
        {
            if(!hgl::char_eq(*str,ch)) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strechr(CharT *str, C ch)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strechr<const CharT,C>(static_cast<const CharT*>(str), ch));
    }

    /**
     * @brief CN: 在字符串中查找第一个不等于 ch 的字符，限定最大长度 n。
     * @brief EN: Find first character not equal to `ch` within max length `n`.
     *
     * @param[in] str CN: 要搜索的字符串起始地址. EN: pointer to the string to search.
     * @param[in] ch CN: 要排除的字符. EN: character to exclude.
     * @param[in] n CN: 最大搜索长度（字符数）. EN: maximum number of characters to consider.
     * @return const CharT* CN/EN: pointer to the first character not equal to ch or nullptr.
     */
    template<typename CharT, typename C>
    inline const CharT *strechr(const CharT *str, C ch, std::size_t n)
    {
        if(!str || n==0) return nullptr;

        while(n-- && *str)
        {
            if(!hgl::char_eq(*str,ch)) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strechr(CharT *str, C ch, std::size_t n)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strechr<const CharT,C>(static_cast<const CharT*>(str), ch, n));
    }

    /**
     * @brief CN: 在字符串中查找第一个不属于字符集合 set 的字符。
     * @brief EN: Find the first character in `str` that is not in the character set `set`.
     *
     * @param[in] str CN: 要搜索的以 null 结尾的字符串. EN: null-terminated string to search.
     * @param[in] set CN: 字符集合指针. EN: pointer to character set.
     * @param[in] set_count CN: 集合中的字符数量. EN: number of characters in the set.
     * @return const CharT* CN/EN: pointer to the first character not in set or nullptr.
     */
    template<typename CharT, typename C>
    inline const CharT *strechr(const CharT *str, const C *set, const std::size_t set_count)
    {
        if(!str || !set || set_count==0) return nullptr;

        while(*str)
        {
            if(!hgl::strchr(set, *str, set_count)) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strechr(CharT *str, C *set, const std::size_t set_count)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strechr<const CharT,C>(static_cast<const CharT*>(str), set, set_count));
    }

    /**
     * @brief CN: 从字符串结尾向前查找字符 ch，限定字符串的长度 length（以字符为单位）。
     * @brief EN: Reverse search for character `ch` within the first `length` characters of `str`.
     *
     * @param[in] str CN: 字符串起始地址（可能不以 null 结尾）. EN: pointer to the string (may not be null-terminated).
     * @param[in] length CN: 考虑的字符长度. EN: number of characters to consider.
     * @param[in] ch CN: 要查找的字符. EN: character to find.
     * @return const CharT* CN/EN: pointer to the last matching character or nullptr if not found.
     */
    template<typename CharT, typename C>
    inline const CharT *strrchr(const CharT *str, const std::size_t length, const C ch)
    {
        if(!str || length==0) return nullptr;

        for(std::size_t i = length; i>0; --i)
        {
            const CharT *p = str + (i-1);
            if(hgl::char_eq(*p,ch)) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strrchr(CharT *str, const std::size_t length, const C ch)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strrchr<const CharT,C>(static_cast<const CharT*>(str), length, ch));
    }

    template<typename CharT, typename C>
    inline const CharT *strrchr(const CharT *str, const C ch)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strrchr<const CharT,C>(static_cast<const CharT*>(str), hgl::strlen(str), ch));
    }

    template<typename CharT, typename C>
    inline const CharT *strrchr(CharT *str, const C ch)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strrchr<const CharT,C>(static_cast<const CharT*>(str), hgl::strlen(str), ch));
    }

    /**
     * @brief CN: 从字符串结尾向前查找属于字符集合 set 的最后一个字符，限定字符串的长度 length。
     * @brief EN: Reverse search for the last character in `str` that is contained in the character set `set`, within the first `length` characters.
     *
     * @param[in] str CN: 字符串起始地址（可能不以 null 结尾）. EN: pointer to the string.
     * @param[in] length CN: 考虑的字符长度. EN: number of characters to consider.
     * @param[in] set CN: 字符集合指针. EN: pointer to character set.
     * @param[in] set_count CN: 集合中的字符数量. EN: number of characters in the set.
     * @return const CharT* CN/EN: pointer to the last character in set or nullptr.
     */
    template<typename CharT, typename C>
    inline const CharT *strrchr(const CharT *str, const std::size_t length, const C *set, const std::size_t set_count)
    {
        if(!str || length==0 || !set || set_count==0) return nullptr;

        for(std::size_t i = length; i>0; --i)
        {
            const CharT *p = str + (i-1);
            if(hgl::strchr(set, *p, set_count)) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strrchr(CharT *str, const std::size_t length, const C *set, const std::size_t set_count)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strrchr<const CharT,C>(static_cast<const CharT*>(str), length, set, set_count));
    }

    /**
     * @brief CN: 从字符串结尾向前查找字符 ch，忽略结尾 offset 个字符（offset 表示从结尾算起要跳过的字符数）。
     * @brief EN: Reverse search for character `ch` starting before `offset` characters from the end of the considered range.
     *
     * @param[in] str CN: 字符串起始地址（可能不以 null 结尾）. EN: pointer to the string.
     * @param[in] length CN: 考虑的字符长度. EN: number of characters to consider.
     * @param[in] offset CN: 从末尾跳过的字符数（必须 < length）. EN: number of characters to skip from the end (must be < length).
     * @param[in] ch CN: 要查找的字符. EN: character to find.
     * @return const CharT* CN/EN: pointer to the matching character or nullptr.
     */
    template<typename CharT, typename C>
    inline const CharT *strrchr(const CharT *str, const std::size_t length, const std::size_t offset, const C ch)
    {
        if(!str || length==0 || offset>=length) return nullptr;

        std::size_t start = length - offset;
        for(std::size_t i = start; i>0; --i)
        {
            const CharT *p = str + (i-1);
            if(hgl::char_eq(*p,ch)) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strrchr(CharT *str, const std::size_t length, const std::size_t offset, const C ch)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strrchr<const CharT,C>(static_cast<const CharT*>(str), length, offset, ch));
    }

    /**
     * @brief CN: 从字符串结尾向前查找属于字符集合 set 的最后一个字符，忽略结尾 offset 个字符。
     * @brief EN: Reverse search for the last character in `str` that is contained in `set`, skipping `offset` characters from the end.
     *
     * @param[in] str CN: 字符串起始地址. EN: pointer to the string.
     * @param[in] length CN: 考虑的字符长度. EN: number of characters to consider.
     * @param[in] offset CN: 从末尾跳过的字符数. EN: number of characters to skip from the end.
     * @param[in] set CN: 字符集合指针. EN: pointer to character set.
     * @param[in] set_count CN: 集合中的字符数量. EN: number of characters in the set.
     * @return const CharT* CN/EN: pointer to the matching character or nullptr.
     */
    template<typename CharT, typename C>
    inline const CharT *strrchr(const CharT *str, const std::size_t length, const std::size_t offset, const C *set, const std::size_t set_count)
    {
        if(!str || length==0 || offset>=length || !set || set_count==0) return nullptr;

        std::size_t start = length - offset;
        for(std::size_t i = start; i>0; --i)
        {
            const CharT *p = str + (i-1);
            if(hgl::strchr(set, *p, set_count)) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename CharT, typename C>
    inline CharT *strrchr(CharT *str, const std::size_t length, const std::size_t offset, const C *set, const std::size_t set_count)
        requires (!std::is_const_v<CharT>)
    {
        return const_cast<CharT*>(hgl::strrchr<const CharT,C>(static_cast<const CharT*>(str), length, offset, set, set_count));
    }

    /**
     * @brief CN: 为给定文件名更改扩展名并将结果写入 new_filename（带最大长度限制）。
     * @brief EN: Replace the extension of a filename and write result into new_filename with a maximum buffer length.
     *
     * @param[out] new_filename CN: 用来接收新文件名的缓冲区. EN: output buffer to receive new filename.
     * @param[in] old_filename CN: 原始文件名. EN: original filename.
     * @param[in] max_len CN: new_filename 的缓冲区大小（以字符为单位）. EN: capacity of new_filename in characters.
     * @param[in] new_extname CN: 新扩展名（不含分隔符）. EN: new extension (without the separator).
     * @param[in] separative_char CN: 扩展名分隔字符，默认 '.'. EN: extension separator character, default '.'.
     */
    template<typename CharT>
    inline void replace_extname(CharT *new_filename, const CharT *old_filename, std::size_t max_len, const CharT *new_extname, const CharT &separative_char=CharT('.'))
    {
        if(!new_filename||!old_filename||max_len==0) return;

        const std::size_t old_len = hgl::strlen(old_filename);
        const CharT *p = hgl::strrchr(old_filename, old_len, separative_char);

        if(p)
        {
            std::size_t prefix_len = static_cast<std::size_t>(p - old_filename + 1);
            std::size_t written = static_cast<std::size_t>(hgl::strcpy(new_filename, max_len, old_filename, prefix_len));
            std::size_t remain = (written<=max_len)?(max_len - written):0;
            if(remain>0 && new_extname)
                hgl::strcpy(new_filename+written, remain, new_extname);
        }
        else
        {
            std::size_t written = static_cast<std::size_t>(hgl::strcpy(new_filename, max_len, old_filename, old_len));
            std::size_t remain = (written<=max_len)?(max_len - written):0;
            if(remain>1)
            {
                new_filename[written]=separative_char;
                ++written; --remain;
                if(new_extname && remain>0)
                    hgl::strcpy(new_filename+written, remain, new_extname);
                else if(remain>0)
                    new_filename[written]=0;
            }
            else if(remain>0)
            {
                new_filename[written]=0;
            }
        }
    }

    /**
     * @brief CN: 为给定文件名（就地）更改扩展名（替换或追加）。
     * @brief EN: Replace or append extension to a filename in-place.
     *
     * @param[in,out] filename CN: 目标文件名缓冲区（将被修改）. EN: filename buffer to modify in-place.
     * @param[in] new_extname CN: 新扩展名（不含分隔符）. EN: new extension to set.
     */
    template<typename CharT>
    inline void replace_extname(CharT *filename, const CharT *new_extname)
    {
        std::size_t len = hgl::strlen(filename);
        const CharT *p = hgl::strrchr(filename, len, CharT('.'));

        if(p)
        {
            // p points inside filename; need mutable to write after dot
            CharT *w = const_cast<CharT*>(p) + 1;
            hgl::strcpy(w, new_extname);
        }
        else
        {
            // concatenate
            hgl::strcat(filename, CharT('.'));
            hgl::strcat(filename, new_extname);
        }
    }

    /**
     * @brief CN: 在字符串中替换所有出现的 old_ch 为 new_ch。
     * @brief EN: Replace all occurrences of old_ch by new_ch in a null-terminated string.
     *
     * @param[in,out] str CN: 要修改的以 null 结尾的字符串. EN: null-terminated string to modify.
     * @param[in] old_ch CN: 要被替换的字符. EN: character to be replaced.
     * @param[in] new_ch CN: 用来替换的字符. EN: replacement character.
     * @return int CN/EN: 返回替换发生的次数. EN: number of replacements performed.
     */
    template<typename CharT>
    inline int replace(CharT *str, const CharT old_ch, const CharT new_ch)
    {
        if(!str) return 0;

        int count=0;
        while(*str)
        {
            if(hgl::char_eq(*str, old_ch))
            {
                *str = new_ch;
                ++count;
            }
            ++str;
        }
        return count;
    }
}//namespace hgl
