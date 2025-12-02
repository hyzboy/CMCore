#pragma once
#include <hgl/type/Str.Number.h>
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Search.h>
#include <hgl/type/Str.Copy.h>
#include <cstddef>
namespace hgl
{

    /**
     * @brief CN: 从字符串中解析数值数组的抽象接口（例如用于解析 "1,2,3"）。
     * @brief EN: Abstract interface for parsing numeric arrays from a string (e.g. "1,2,3").
     *
     * @tparam CharT 字符类型。EN: character type.
     * @tparam NumT 数值类型。EN: numeric value type.
     */
    template<typename CharT, typename NumT>
    struct ParseNumberArray
    {
        virtual ~ParseNumberArray() = default;
        virtual bool IsChar(CharT ch) const = 0;                      ///< CN: 判断当前字符是否属于数字文本的一部分。EN: Test whether character is part of a number token.
        virtual bool ToNumber(const CharT *str, NumT &out) const = 0;  ///< CN: 将以 str 开始的数字文本解析为数值（遇到分隔符时结束）。EN: Parse number starting at str into out.
    };

    template<typename CharT, typename NumT>
    struct ParseIntArray : public ParseNumberArray<CharT,NumT>
    {
        bool IsChar(CharT ch) const override { return hgl::is_integer_char(ch); }
        bool ToNumber(const CharT *str, NumT &out) const override { return hgl::stoi(str, out); }
    };

    template<typename CharT, typename NumT>
    struct ParseUIntArray : public ParseNumberArray<CharT,NumT>
    {
        bool IsChar(CharT ch) const override { return hgl::is_digit(ch); }
        bool ToNumber(const CharT *str, NumT &out) const override { return hgl::stou(str, out); }
    };

    template<typename CharT, typename NumT>
    struct ParseFloatArray : public ParseNumberArray<CharT,NumT>
    {
        bool IsChar(CharT ch) const override { return hgl::is_float_char(ch); }
        bool ToNumber(const CharT *str, NumT &out) const override { return hgl::etof(str, out); }
    };

    template<typename CharT, typename NumT>
    struct ParseHexArray : public ParseNumberArray<CharT,NumT>
    {
        bool IsChar(CharT ch) const override { return hgl::is_hex_digit(ch); }
        bool ToNumber(const CharT *str, NumT &out) const override { return hgl::xtou(str, out); }
    };

    /**
     * @brief CN: 将字符串解析为数值数组（写入到 result 缓冲区），遇到分隔符或 end_char 停止。
     * @brief EN: Parse numbers from a string into result array; stop on delimiters or end_char.
     *
     * @tparam CharT 字符类型。EN: character type.
     * @tparam NumT 数值类型。EN: numeric value type.
     * @param[in] pna CN: 指向解析器实现的指针. EN: pointer to parser implementation.
     * @param[in] str CN: 源字符串指针（以 NUL 终止）. EN: source string pointer (NUL-terminated).
     * @param[out] result CN: 输出数值缓冲区. EN: output buffer for parsed values.
     * @param[in] max_count CN: 能够写入 result 的最大元素数. EN: maximum number of elements to write into result.
     * @param[in] end_char CN: 可选终止字符（默认 0 表示不使用）. EN: optional end character (default 0 means none).
     * @param[out] end_pointer CN: 如果非空，返回解析停止时的字符串位置. EN: optional out pointer receiving position where parsing stopped.
     * @return int CN: 返回成功解析的元素数量，出错返回 -1. EN: number of parsed elements on success, -1 on error.
     */
    template<typename CharT, typename NumT>
    inline int parse_number_array(ParseNumberArray<CharT,NumT> *pna, const CharT *str, NumT *result, std::size_t max_count, const CharT end_char = 0, const CharT **end_pointer = nullptr)
    {
        if(!pna || !str || !result || max_count == 0) return -1;

        const CharT *p = str;
        const CharT *sp = str;
        std::size_t remaining = max_count;
        int parsed = 0;

        while(*p && *p != end_char)
        {
            if(pna->IsChar(*p))
            {
                ++p;
                continue;
            }

            if(pna->ToNumber(sp, *result))
            {
                ++parsed;
                --remaining;

                if(remaining == 0)
                {
                    if(end_pointer) *end_pointer = p;
                    return parsed;
                }

                ++result;
                ++p;
                sp = p;
                continue;
            }

            if(end_pointer) *end_pointer = p;
            return parsed;
        }

        if(p > sp)
        {
            pna->ToNumber(sp, *result);
            ++parsed;
        }

        if(end_pointer) *end_pointer = p;
        return parsed;
    }

    /**
     * @brief CN: 便捷函数：解析浮点数数组写入到固定缓冲区。
     * @brief EN: Convenience wrapper to parse float array into a buffer.
     */
    template<typename CharT, typename NumT>
    inline int parse_float_array(const CharT *str, NumT *result, std::size_t max_count, const CharT end_char = 0, const CharT **end_pointer = nullptr)
    {
        ParseFloatArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT>(&pna, str, result, max_count, end_char, end_pointer);
    }

    template<typename CharT, typename NumT>
    inline int parse_int_array(const CharT *str, NumT *result, std::size_t max_count, const CharT end_char = 0, const CharT **end_pointer = nullptr)
    {
        ParseIntArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT>(&pna, str, result, max_count, end_char, end_pointer);
    }

    template<typename CharT, typename NumT>
    inline int parse_uint_array(const CharT *str, NumT *result, std::size_t max_count, const CharT end_char = 0, const CharT **end_pointer = nullptr)
    {
        ParseUIntArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT>(&pna, str, result, max_count, end_char, end_pointer);
    }

    template<typename CharT, typename NumT>
    inline int parse_hex_array(const CharT *str, NumT *result, std::size_t max_count, const CharT end_char = 0, const CharT **end_pointer = nullptr)
    {
        ParseHexArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT>(&pna, str, result, max_count, end_char, end_pointer);
    }

    /**
     * @brief CN: 将数值字符串解析到可调用 Add 的容器中（例如数组列表）。
     * @brief EN: Parse numbers from string into a container with Add(result) method.
     *
     * @tparam CharT 字符类型。EN: character type.
     * @tparam NumT 数值类型。EN: numeric type.
     * @tparam Container 类型必须提供 Add(NumT) 方法。EN: container type must provide Add(NumT).
     * @param[in] pna CN: 解析器实例. EN: parser instance.
     * @param[in] str CN: 源字符串指针. EN: source string.
     * @param[in] str_len CN: 要处理的字符长度. EN: number of characters to process.
     * @param[out] result_list CN: 支持 Add 方法的容器. EN: container to receive parsed values (via Add).
     * @return int CN/EN: 成功解析的元素数量，失败返回 -1.
     */
    template<typename CharT, typename NumT, typename Container>
    inline int parse_number_array(ParseNumberArray<CharT,NumT> *pna, const CharT *str, std::size_t str_len, Container &result_list)
    {
        if(!pna || !str || str_len == 0) return -1;

        const CharT *p = str;
        const CharT *sp = str;
        std::size_t remaining = str_len;
        int parsed = 0;
        NumT tmp;

        while(*p && remaining > 0)
        {
            --remaining;
            if(pna->IsChar(*p))
            {
                ++p;
                continue;
            }

            if(pna->ToNumber(sp, tmp))
            {
                ++parsed;
                result_list.Add(tmp);

                ++p;
                sp = p;
                continue;
            }

            return parsed;
        }

        if(p > sp)
        {
            pna->ToNumber(sp, tmp);
            result_list.Add(tmp);
            ++parsed;
        }

        return parsed;
    }

    template<typename CharT, typename NumT, typename Container>
    inline int parse_float_array(const CharT *str, std::size_t len, Container &result_list)
    {
        ParseFloatArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT, Container>(&pna, str, len, result_list);
    }

    template<typename CharT, typename NumT, typename Container>
    inline int parse_int_array(const CharT *str, std::size_t len, Container &result_list)
    {
        ParseIntArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT, Container>(&pna, str, len, result_list);
    }

    template<typename CharT, typename NumT, typename Container>
    inline int parse_uint_array(const CharT *str, std::size_t len, Container &result_list)
    {
        ParseUIntArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT, Container>(&pna, str, len, result_list);
    }

    template<typename CharT, typename NumT, typename Container>
    inline int parse_hex_array(const CharT *str, std::size_t len, Container &result_list)
    {
        ParseHexArray<CharT, NumT> pna;
        return hgl::parse_number_array<CharT, NumT, Container>(&pna, str, len, result_list);
    }

}//namespace hgl
