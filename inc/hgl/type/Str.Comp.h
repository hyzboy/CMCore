#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Case.h>
#include <type_traits>
#include <utility>

namespace hgl
{
    /**
     * @brief
     * CN: 比较两个以 NUL 结尾的字符串的大小（逐字符比较）。
     * EN: Compare two NUL-terminated strings lexicographically (character by character).
     *
     * @tparam S  源字符串类型 / source string character type
     * @tparam D  目标字符串类型 / destination string character type
     * @param  src       CN: 要比较的字符串 / source string to compare
     *                    EN: pointer to the first string (may be nullptr)
     * @param  dst       CN: 要比较的字符串 / destination string to compare
     *                    EN: pointer to the second string (may be nullptr)
     * @return           CN: 返回负数/零/正数，分别表示 src < dst、src == dst、src > dst
     *                    EN: Returns negative/zero/positive to indicate src < dst, src == dst, src > dst
     */
    template<typename S,typename D>
    inline int strcmp(const S *src,const D *dst)
    {
        if(!src)
            return (!dst) ? 0 : -1;

        if(!dst)
            return 1;

        while((*src) && (*dst))
        {
            if(!char_eq(*src,*dst))
            {
                using CT = std::common_type_t<S,D>;
                const CT a = static_cast<CT>(*src);
                const CT b = static_cast<CT>(*dst);
                return static_cast<int>(static_cast<int64_t>(a) - static_cast<int64_t>(b));
            }

            ++src;
            ++dst;
        }

        // If both ended, equal; otherwise shorter is less
        if(*src) return 1;
        if(*dst) return -1;
        return 0;
    }

    /**
     * @brief
     * CN: 比较两个具有指定长度的字符串（按给定长度范围内逐字符比较）。
     * EN: Compare two strings with specified lengths (lexicographically within given lengths).
     *
     * @tparam S  源字符串类型 / source string character type
     * @tparam D  目标字符串类型 / destination string character type
     * @param  src       CN: 要比较的字符串1 / first string pointer
     *                    EN: pointer to the first string (may be nullptr)
     * @param  src_size  CN: 要比较的字符串1长度 / length of the first string (must be >= 0)
     *                    EN: number of characters to consider from src
     * @param  dst       CN: 要比较的字符串2 / second string pointer
     *                    EN: pointer to the second string (may be nullptr)
     * @param  dst_size  CN: 要比较的字符串2长度 / length of the second string (must be >= 0)
     *                    EN: number of characters to consider from dst
     * @return           CN: 返回负数/零/正数，分别表示 src < dst、src == dst、src > dst
     *                    EN: Returns negative/zero/positive to indicate src < dst, src == dst, src > dst
     */
    template<typename S,typename D>
    inline int strcmp(const S *src, std::size_t src_size, const D *dst, std::size_t dst_size)
    {
        if(!src)
            return (!dst) ? 0 : -1;

        if(!dst)
            return 1;

        if(src_size==0)
            return (dst_size==0) ? 0 : -1;

        if(dst_size==0)
            return 1;

        while(src_size>0 && dst_size>0)
        {
            if(!hgl::char_eq(*src,*dst))
            {
                using CT = std::common_type_t<S,D>;
                const CT a = static_cast<CT>(*src);
                const CT b = static_cast<CT>(*dst);
                return static_cast<int>(static_cast<int64_t>(a) - static_cast<int64_t>(b));
            }

            ++src; ++dst; --src_size; --dst_size;
        }

        if(src_size>dst_size) return 1;
        if(src_size<dst_size) return -1;
        return 0;
    }

    /**
     * @brief
     * CN: 比较两个字符串的大小，并限制最大比较长度（类似 strncmp）。
     * EN: Compare two strings with a maximum comparison length (like strncmp).
     *
     * @tparam S  源字符串类型 / source string character type
     * @tparam D  目标字符串类型 / destination string character type
     * @param  src       CN: 要比较的字符串 / source string pointer
     *                    EN: pointer to the first string (may be nullptr)
     * @param  dst       CN: 要比较的字符串 / destination string pointer
     *                    EN: pointer to the second string (may be nullptr)
     * @param  count     CN: 比较的最大长度 / maximum number of characters to compare (must be >= 0)
     *                    EN: maximum comparison length (size_t)
     * @return           CN: 返回负数/零/正数，分别表示 src < dst、src == dst（在 count 范围内）、src > dst
     *                    EN: Returns negative/zero/positive to indicate src < dst, src == dst (within count), src > dst
     */
    template<typename S,typename D>
    inline int strcmp(const S *src,const D *dst,std::size_t count)
    {
        if(count==0) return 0;

        if(!src)
            return (!dst) ? 0 : -1;

        if(!dst)
            return 1;

        while(count>0 && (*src) && (*dst))
        {
            if(!hgl::char_eq(*src,*dst))
            {
                using CT = std::common_type_t<S,D>;
                const CT a = static_cast<CT>(*src);
                const CT b = static_cast<CT>(*dst);
                return static_cast<int>(static_cast<int64_t>(a) - static_cast<int64_t>(b));
            }

            ++src; ++dst; --count;
        }

        if(count==0) return 0;

        if(*src) return 1;
        if(*dst) return -1;
        return 0;
    }

    /**
     * @brief
     * CN: 比较两个字符串的大小，英文字符不区分大小写（case-insensitive for ASCII letters）。
     * EN: Compare two strings case-insensitively for ASCII letters。
     *
     * @tparam S  源字符串类型 / source string character type
     * @tparam D  目标字符串类型 / destination string character type
     * @param  src       CN: 要比较的字符串 / source string pointer
     *                    EN: pointer to the first string (may be nullptr)
     * @param  dst       CN: 要比较的字符串 / destination string pointer
     *                    EN: pointer to the second string (may be nullptr)
     * @return           CN: 返回 -1/0/+1 分别表示 src < dst、src == dst、src > dst（不区分大小写）
     *                    EN: Returns -1/0/+1 for src < dst, src == dst, src > dst (case-insensitive)
     */
    template<typename S,typename D>
    inline int stricmp(const S *src,const D *dst)
    {
        if(!src)
            return (!dst) ? 0 : -1;

        if(!dst)
            return 1;

        while(*src && *dst)
        {
            const int gap = hgl::compare_char_icase(*src, *dst);
            if(gap < 0) return -1;
            if(gap > 0) return 1;
            ++src; ++dst;
        }

        if(*src) return 1;
        if(*dst) return -1;
        return 0;
    }

    /**
     * @brief
     * CN: 比较两个具有指定长度的字符串，英文字符不区分大小写。
     * EN: Compare two strings with specified lengths, case-insensitive for ASCII letters。
     *
     * @tparam S  源字符串类型 / source string character type
     * @tparam D  目标字符串类型 / destination string character type
     * @param  src       CN: 要比较的字符串1 / first string pointer
     *                    EN: pointer to the first string (may be nullptr)
     * @param  src_size  CN: 要比较的字符串1长度 / length of the first string (must be >= 0)
     *                    EN: number of characters to consider from src
     * @param  dst       CN: 要比较的字符串2 / second string pointer
     *                    EN: pointer to the second string (may be nullptr)
     * @param  dst_size  CN: 要比较的字符串2长度 / length of the second string (must be >= 0)
     *                    EN: number of characters to consider from dst
     * @return           CN: 返回负数/零/正数，分别表示 src < dst、src == dst、src > dst
     *                    EN: Returns negative/zero/positive to indicate src < dst, src == dst, src > dst
     */
    template<typename S,typename D>
    inline int stricmp(const S *src,std::size_t src_size,const D *dst,std::size_t dst_size)
    {
        if(!src)
            return (!dst) ? 0 : -1;

        if(!dst)
            return 1;

        if(src_size==0)
            return (dst_size==0) ? 0 : -1;

        if(dst_size==0)
            return 1;

        while(src_size>0 && dst_size>0)
        {
            const int gap = hgl::compare_char_icase(*src, *dst);
            if(gap < 0) return -1;
            if(gap > 0) return 1;

            ++src; ++dst; --src_size; --dst_size;
        }

        if(src_size>dst_size) return 1;
        if(src_size<dst_size) return -1;
        return 0;
    }

    /**
     * @brief
     * CN: 比较两个字符串的大小（不区分大小写），并限制最大比较长度。
     * EN: Case-insensitive compare for two strings with a maximum comparison length。
     *
     * @tparam S  源字符串类型 / source string character type
     * @tparam D  目标字符串类型 / destination string character type
     * @param  src       CN: 要比较的字符串 / source string pointer
     *                    EN: pointer to the first string (may be nullptr)
     * @param  dst       CN: 要比较的字符串 / destination string pointer
     *                    EN: pointer to the second string (may be nullptr)
     * @param  count     CN: 比较的最大长度 / maximum number of characters to compare (must be >= 0)
     *                    EN: maximum comparison length (size_t)
     * @return           CN: 返回 -1/0/+1 分别表示 src < dst、src == dst、src > dst（不区分大小写）
     *                    EN: Returns -1/0/+1 for src < dst, src == dst, src > dst (case-insensitive)
     */
    template<typename S,typename D>
    inline int stricmp(const S *src,const D *dst,std::size_t count)
    {
        if(count==0) return 0;

        if(!src) return (!dst) ? 0 : -1;
        if(!dst) return 1;

        while(count>0 && *src && *dst)
        {
            const int gap = hgl::compare_char_icase(*src, *dst);
            if(gap < 0) return -1;
            if(gap > 0) return 1;
            ++src; ++dst; --count;
        }

        if(count==0) return 0;
        if(*src) return 1;
        if(*dst) return -1;
        return 0;
    }

    /**
     * @brief
     * CN: 字符集专用比较函数：仅比较字母与数字，忽略其它符号与 ASCII 大小写差异。
     * EN: Charset-aware comparison: compares only letters and digits, ignoring symbols and ASCII case differences.
     *
     * @tparam S  源字符串类型 / source string character type
     * @tparam D  目标字符串类型 / destination string character type
     * @param  src       CN: 要比较的字符串 / source string pointer
     *                    EN: pointer to the first string (may be nullptr)
     * @param  dst       CN: 要比较的字符串 / destination string pointer
     *                    EN: pointer to the second string (may be nullptr)
     * @return           CN: 返回 -1/0/+1 分别表示 src < dst、src == dst、src > dst
     *                    EN: Returns -1/0/+1 for src < dst, src == dst, src > dst
     */
    template<typename S,typename D>
    inline int charset_cmp(const S *src,const D *dst)
    {
        if(!src) return (!dst) ? 0 : -1;
        if(!dst) return 1;

        const S *s = src;
        const D *d = dst;

        while(true)
        {
            // advance s to next alnum or to NUL
            while(*s && !hgl::is_alpha_numeric(*s)) ++s;
            // advance d to next alnum or to NUL
            while(*d && !hgl::is_alpha_numeric(*d)) ++d;

            // if both ended, equal
            if(!*s && !*d) return 0;
            // if one ended, shorter is less
            if(!*s) return -1;
            if(!*d) return 1;

            const int gap = hgl::compare_char_icase(*s, *d);
            if(gap < 0) return -1;
            if(gap > 0) return 1;

            ++s; ++d;
        }

        return 0; // unreachable
    }
}//namespace hgl
