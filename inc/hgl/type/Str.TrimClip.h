#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <cstddef>
namespace hgl
{

    /**
     * @brief CN: 截去字符串前端所有的指定字符。
     * @brief EN: Trim leading characters matching predicate from a string range。
     *
     * @tparam CharT 字符类型。EN: character type。
     * @param[in] src CN: 源字符串起始指针（不修改）. EN: source string pointer。
     * @param[in,out] len CN: 输入为字符串长度，返回为修剪后长度. EN: input length, returns trimmed length。
     * @param[in] trimfunc CN: 判定是否为要截取字符的函数，默认 is_space<CharT>. EN: predicate to test characters to trim (default is_space)。
     * @return const CharT* CN: 返回新的字符串起始指针（可能位于原始缓冲区内部），若 len 为 0 返回 nullptr. EN: pointer to trimmed string start, or nullptr if length becomes 0。
     */
    template<typename CharT>
    inline const CharT *trimleft(const CharT *src, std::size_t &len, bool (*trimfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(!src || len == 0) return nullptr;

        const CharT *p = src;
        while(len > 0 && *p && trimfunc(*p))
        {
            ++p;
            --len;
        }

        if(len == 0) return nullptr;
        return p;
    }

    // compatibility overload for int& length
    template<typename CharT>
    inline const CharT *trimleft(const CharT *src, int &len, bool (*trimfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(len <= 0) return nullptr;
        std::size_t l = static_cast<std::size_t>(len);
        const CharT *r = trimleft<CharT>(src, l, trimfunc);
        len = r ? static_cast<int>(l) : 0;
        return r;
    }

    /**
     * @brief CN: 截去字符串尾端所有的指定字符。
     * @brief EN: Trim trailing characters matching predicate from a string range。
     *
     * @tparam CharT 字符类型。EN: character type。
     * @param[in] src CN: 源字符串起始指针（不修改）. EN: source string pointer。
     * @param[in,out] len CN: 输入为字符串长度，返回为修剪后长度. EN: input length, returns trimmed length。
     * @param[in] trimfunc CN: 判定是否为要截取字符的函数，默认 is_space<CharT>. EN: predicate to test characters to trim (default is_space)。
     * @return const CharT* CN: 返回新的字符串起始指针（通常为 src），若 len 为 0 返回 nullptr. EN: pointer to (possibly same) string start, or nullptr if length becomes 0。
     */
    template<typename CharT>
    inline const CharT *trimright(const CharT *src, std::size_t &len, bool (*trimfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(!src || len == 0) return nullptr;

        const CharT *p = src + (len - 1);

        while(len > 0 && trimfunc(*p))
        {
            --p;
            --len;
        }

        if(len == 0) return nullptr;
        return src;
    }

    // compatibility overload for int& length
    template<typename CharT>
    inline const CharT *trimright(const CharT *src, int &len, bool (*trimfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(len <= 0) return nullptr;
        std::size_t l = static_cast<std::size_t>(len);
        const CharT *r = trimright<CharT>(src, l, trimfunc);
        len = r ? static_cast<int>(l) : 0;
        return r;
    }

    /**
     * @brief CN: 截去字符串前端和尾端的所有指定字符。
     * @brief EN: Trim both leading and trailing characters matching predicate。
     *
     * @tparam CharT 字符类型。EN: character type。
     * @param[in] src CN: 源字符串起始指针. EN: source string pointer。
     * @param[in,out] len CN: 输入为字符串长度，返回为修剪后长度. EN: input length, returns trimmed length。
     * @param[in] trimfunc CN: 判定是否为要截取字符的函数，默认 is_space<CharT>. EN: predicate to test characters to trim (default is_space)。
     * @return const CharT* CN: 返回新的字符串起始指针或 nullptr（若 len 为 0）. EN: pointer to trimmed string start or nullptr if length becomes 0。
     */
    template<typename CharT>
    inline const CharT *trim(const CharT *src, std::size_t &len, bool (*trimfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(!src || len == 0) return nullptr;

        const CharT *sp = src;
        const CharT *ep = src + (len - 1);

        while(len > 0 && *sp && trimfunc(*sp))
        {
            ++sp;
            --len;
        }

        while(len > 0 && trimfunc(*ep))
        {
            --ep;
            --len;
        }

        if(len == 0) return nullptr;
        return sp;
    }

    // compatibility overload for int& length
    template<typename CharT>
    inline const CharT *trim(const CharT *src, int &len, bool (*trimfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(len <= 0) return nullptr;
        std::size_t l = static_cast<std::size_t>(len);
        const CharT *r = trim<CharT>(src, l, trimfunc);
        len = r ? static_cast<int>(l) : 0;
        return r;
    }

    /**
     * @brief CN: 截取字符串前端直到遇到不可用字符（clip predicate），返回截取长度。
     * @brief EN: Clip left portion of string up to a predicate, returning the clipped length。
     *
     * @tparam CharT 字符类型。EN: character type。
     * @param[in] src CN: 源字符串指针. EN: source string pointer。
     * @param[in,out] len CN: 输入为字符串长度，返回为截取后长度（即被截取的部分长度）. EN: input length; returns length of clipped prefix。
     * @param[in] clipfunc CN: 判定不可用字符的函数，默认 is_space<CharT>. EN: predicate to identify clip characters (default is_space)。
     * @return const CharT* CN: 返回起始指针（原始 src）或 nullptr 若无可截取部分或参数不合法。EN: returns src or nullptr if no clip or invalid input。
     */
    template<typename CharT>
    inline const CharT *clipleft(const CharT *src, std::size_t &len, bool (*clipfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(!src || len == 0) return nullptr;

        const CharT *p = src;
        const CharT *end = src + len;

        while(p < end && *p && !clipfunc(*p))
            ++p;

        if(p == src) return nullptr;

        len = static_cast<std::size_t>(p - src);
        return src;
    }

    // compatibility overload for int& length
    template<typename CharT>
    inline const CharT *clipleft(const CharT *src, int &len, bool (*clipfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(len <= 0) return nullptr;
        std::size_t l = static_cast<std::size_t>(len);
        const CharT *r = clipleft<CharT>(src, l, clipfunc);
        len = r ? static_cast<int>(l) : 0;
        return r;
    }

    /**
     * @brief CN: 截去字符串尾端的字符串（直到遇到可截取字符）。
     * @brief EN: Clip right portion of string up to a predicate, returning start pointer and length of clipped suffix。
     *
     * @tparam CharT 字符类型。EN: character type。
     * @param[in] src CN: 源字符串指针. EN: source string pointer。
     * @param[in,out] len CN: 输入为字符串长度，返回为截取后的长度（即被截取的后缀长度）. EN: input length; returns length of clipped suffix。
     * @param[in] clipfunc CN: 判定不可用字符的函数，默认 is_space<CharT>. EN: predicate to identify clip characters (default is_space)。
     * @return const CharT* CN: 返回截取后部分的起始指针或 nullptr 若无可截取部分或参数不合法。EN: returns pointer to clipped suffix start or nullptr if none。
     */
    template<typename CharT>
    inline const CharT *clipright(const CharT *src, std::size_t &len, bool (*clipfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(!src || len == 0) return nullptr;

        const CharT *p = src + (len - 1);
        const CharT *begin = src;

        while(p >= begin && !clipfunc(*p))
            --p;

        if(p < begin) return nullptr;

        len = static_cast<std::size_t>((src + len) - (p + 1));
        return p + 1;
    }

    // compatibility overload for int& length
    template<typename CharT>
    inline const CharT *clipright(const CharT *src, int &len, bool (*clipfunc)(const CharT &) = hgl::is_space<CharT>)
    {
        if(len <= 0) return nullptr;
        std::size_t l = static_cast<std::size_t>(len);
        const CharT *r = clipright<CharT>(src, l, clipfunc);
        len = r ? static_cast<int>(l) : 0;
        return r;
    }
}//namespace hgl
