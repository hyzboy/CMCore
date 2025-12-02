#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
namespace hgl
{

    /**
     * @brief CN: 将十进制整数字符串转换为有符号数值。
     * @brief EN: Convert a decimal integer string to a signed numeric value.
     *
     * @tparam ResultT 结果类型（整型）。EN: Result numeric type.
     * @tparam CharT 字符类型（例如 char / wchar_t）。EN: Character type (e.g. char, wchar_t).
     * @param[in] str CN: 指向以 null 结尾的字符串. EN: pointer to a null-terminated string.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功（输入非空返回 true）。EN: true if parsing succeeded (non-null input).
     */
    template<typename ResultT, typename CharT>
    inline bool stoi(const CharT *str, ResultT &out_result)
    {
        if(!str)
        {
            out_result = ResultT(0);
            return false;
        }

        out_result = ResultT(0);
        bool positive = true;

        if(*str == '+')
            ++str;

        if(*str == '-')
        {
            positive = false;
            ++str;
        }

        while(*str && *str >= '0' && *str <= '9')
        {
            out_result *= 10;
            out_result += ResultT(*str - '0');
            ++str;
        }

        if(!positive)
            out_result = -out_result;

        return true;
    }

    /**
     * @brief CN: 将定长（或指定长度）十进制整数字符串转换为有符号数值。
     * @brief EN: Convert a decimal integer string with specified max length to a signed numeric value.
     *
     * @tparam ResultT 结果类型（整型）。EN: Result numeric type.
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向字符串起始位置（未必以 null 结尾）。EN: pointer to the string start (may not be null-terminated).
     * @param[in] length CN: 可读取的最大字符数。EN: maximum number of characters to read.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功（输入合法返回 true）。EN: true if parsing succeeded (valid input).
     */
    template<typename ResultT, typename CharT>
    inline bool stoi(const CharT *str, std::size_t length, ResultT &out_result)
    {
        if(!str || length == 0)
        {
            out_result = ResultT(0);
            return false;
        }

        out_result = ResultT(0);
        bool positive = true;

        if(*str == '+')
        {
            ++str; --length;
        }

        if(length > 0 && *str == '-')
        {
            positive = false;
            ++str; --length;
        }

        while(length > 0 && *str && *str >= '0' && *str <= '9')
        {
            out_result *= 10;
            out_result += ResultT(*str - '0');
            ++str; --length;
        }

        if(!positive)
            out_result = -out_result;

        return true;
    }

    /**
     * @brief CN: 将十进制整数字符串转换为无符号数值。
     * @brief EN: Convert a decimal integer string to an unsigned numeric value.
     *
     * @tparam ResultT 结果类型（无符号）。EN: Result unsigned numeric type.
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向以 null 结尾的字符串. EN: pointer to a null-terminated string.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功。EN: true if parsing succeeded.
     */
    template<typename ResultT, typename CharT>
    inline bool stou(const CharT *str, ResultT &out_result)
    {
        if(!str)
        {
            out_result = ResultT(0);
            return false;
        }

        out_result = ResultT(0);

        while(*str && *str >= '0' && *str <= '9')
        {
            out_result *= 10;
            out_result += ResultT(*str - '0');
            ++str;
        }

        return true;
    }

    /**
     * @brief CN: 将定长（或指定长度）十进制整数字符串转换为无符号数值。
     * @brief EN: Convert a decimal integer string with specified max length to an unsigned numeric value.
     *
     * @tparam ResultT 结果类型（无符号）。EN: Result unsigned numeric type.
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向字符串起始位置。EN: pointer to the string start.
     * @param[in] length CN: 可读取的最大字符数。EN: maximum number of characters to read.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功。EN: true if parsing succeeded.
     */
    template<typename ResultT, typename CharT>
    inline bool stou(const CharT *str, std::size_t length, ResultT &out_result)
    {
        if(!str || length == 0)
        {
            out_result = ResultT(0);
            return false;
        }

        out_result = ResultT(0);

        while(length > 0 && *str && *str >= '0' && *str <= '9')
        {
            out_result *= 10;
            out_result += ResultT(*str - '0');
            ++str; --length;
        }

        return true;
    }

    /**
     * @brief CN: 将十六进制字符串转换为无符号数值（不带前缀）。
     * @brief EN: Convert a hexadecimal string (without 0x prefix) to an unsigned numeric value.
     *
     * @tparam ResultT 结果类型（无符号）。EN: Result unsigned numeric type.
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向以 null 结尾的十六进制字符串. EN: pointer to a null-terminated hex string.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功。EN: true if parsing succeeded.
     */
    template<typename ResultT, typename CharT>
    inline bool xtou(const CharT *str, ResultT &out_result)
    {
        if(!str)
        {
            out_result = ResultT(0);
            return false;
        }

        out_result = ResultT(0);

        while(*str && hgl::is_hex_digit(*str))
        {
            out_result *= 16;

            if(*str >= '0' && *str <= '9')
                out_result += ResultT(*str - '0');
            else if(*str >= 'a' && *str <= 'f')
                out_result += ResultT(*str - 'a') + 10;
            else if(*str >= 'A' && *str <= 'F')
                out_result += ResultT(*str - 'A') + 10;

            ++str;
        }

        return true;
    }

    /**
     * @brief CN: 将十进制浮点数字符串转换为浮点数。
     * @brief EN: Convert a decimal floating-point string to a floating value.
     *
     * @tparam ResultT 结果类型（浮点）。EN: Result floating numeric type.
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向以 null 结尾的字符串. EN: pointer to a null-terminated string.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功。EN: true if parsing succeeded.
     */
    template<typename ResultT, typename CharT>
    inline bool stof(const CharT *str, ResultT &out_result)
    {
        if(!str)
        {
            out_result = ResultT(0);
            return false;
        }

        bool positive = true;
        out_result = ResultT(0);

        if(*str == '+')
            ++str;

        if(*str == '-')
        {
            positive = false;
            ++str;
        }

        while(*str && *str >= '0' && *str <= '9')
        {
            out_result *= 10;
            out_result += ResultT(*str - '0');
            ++str;
        }

        if(*str != '.')
        {
            if(!positive) out_result = -out_result;
            return true;
        }

        ++str; // skip dot

        ResultT place = ResultT(0.1);
        while(*str && *str >= '0' && *str <= '9')
        {
            out_result += ResultT(*str - '0') * place;
            place *= ResultT(0.1);
            ++str;
        }

        if(!positive) out_result = -out_result;
        return true;
    }

    /**
     * @brief CN: 将定长（或指定长度）十进制浮点数字符串转换为浮点数。
     * @brief EN: Convert a decimal floating-point string with specified max length to a floating value.
     *
     * @tparam ResultT 结果类型（浮点）。EN: Result floating numeric type.
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向字符串起始位置。EN: pointer to the string start.
     * @param[in] length CN: 可读取的最大字符数。EN: maximum number of characters to read.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功。EN: true if parsing succeeded.
     */
    template<typename ResultT, typename CharT>
    inline bool stof(const CharT *str, std::size_t length, ResultT &out_result)
    {
        if(!str || length == 0)
        {
            out_result = ResultT(0);
            return false;
        }

        bool positive = true;
        out_result = ResultT(0);

        if(*str == '+')
        {
            ++str; --length;
        }

        if(length > 0 && *str == '-')
        {
            positive = false;
            ++str; --length;
        }

        while(length > 0 && *str && *str >= '0' && *str <= '9')
        {
            out_result *= 10;
            out_result += ResultT(*str - '0');
            ++str; --length;
        }

        if(length == 0 || *str != '.')
        {
            if(!positive) out_result = -out_result;
            return true;
        }

        ++str; --length; // skip dot

        ResultT place = ResultT(0.1);
        while(length > 0 && *str && *str >= '0' && *str <= '9')
        {
            out_result += ResultT(*str - '0') * place;
            place *= ResultT(0.1);
            ++str; --length;
        }

        if(!positive) out_result = -out_result;
        return true;
    }

    /**
     * @brief CN: 解析带指数部分的浮点字符串（例如 "1.23e-4"）。
     * @brief EN: Parse floating string with exponent (e.g. "1.23e-4").
     *
     * @tparam ResultT 结果类型（浮点）。EN: Result floating numeric type.
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向以 null 结尾的字符串. EN: pointer to a null-terminated string.
     * @param[out] out_result CN: 解析结果输出。EN: parsed result output.
     * @return bool CN: 解析是否成功。EN: true if parsing succeeded.
     */
    template<typename ResultT, typename CharT>
    inline bool etof(const CharT *str, ResultT &out_result)
    {
        if(!str) { out_result = ResultT(0); return false; }

        double base = 0.0;
        if(!hgl::stof(str, base))
        {
            out_result = ResultT(0);
            return false;
        }

        // find 'e' or 'E'
        const CharT *p = str;
        while(*p && *p != 'e' && *p != 'E') ++p;

        if(!*p)
        {
            out_result = ResultT(base);
            return true;
        }

        // parse exponent after e/E
        double exp_val = 0.0;
        if(hgl::stof(p + 1, exp_val))
        {
            out_result = ResultT(base * std::pow(10.0, exp_val));
            return true;
        }

        out_result = ResultT(base);
        return false;
    }

    /**
     * @brief CN: 将字符串解析为布尔值（支持 T/t/Y/y/M/m/1 表示 true）。
     * @brief EN: Parse a string to boolean (supports T/t/Y/y/M/m/1 for true).
     *
     * @tparam CharT 字符类型。EN: Character type.
     * @param[in] str CN: 指向以 null 结尾的字符串. EN: pointer to a null-terminated string.
     * @param[out] out_value CN: 解析后的布尔值。EN: parsed boolean value.
     * @return bool CN: 解析是否认定为 true（否则返回 false）。EN: true if input indicates true.
     */
    template<typename CharT>
    inline bool stob(const CharT *str, bool &out_value)
    {
        if(!str)
        {
            out_value = false;
            return false;
        }

        CharT c = *str;
        if(c == 'T' || c == 't' || c == 'Y' || c == 'y' || c == 'M' || c == 'm' || c == '1')
        {
            out_value = true;
            return true;
        }

        out_value = false;
        return false;
    }

    /**
     * @brief CN: 将有符号整数转换为字符串（右对齐形式），返回写入长度。
     * @brief EN: Convert a signed integer to string (right-aligned style), return number of characters written.
     *
     * @tparam CharT 字符类型。EN: Character type.
     * @tparam IntT 整型类型。EN: Integer type.
     * @param[out] buffer CN: 输出字符串缓冲区。EN: output buffer.
     * @param[in] buffer_size CN: 缓冲区可容纳的字符数。EN: buffer capacity.
     * @param[in] value CN: 要转换的数值。EN: value to convert.
     * @return int CN: 写入的字符数（不含终止符），若失败返回 -1。EN: number of characters written (excluding terminator), or -1 on failure.
     */
    template<typename CharT, typename IntT>
    inline int itos_rl(CharT *buffer, int buffer_size, const IntT value)
    {
        if(!buffer || buffer_size <= 0) return -1;

        CharT *p = buffer;
        CharT buf[sizeof(IntT) * 8];
        CharT *bp = buf;

        IntT v = value;
        using UnsignedT = std::make_unsigned_t<IntT>;
        UnsignedT uvalue;

        if(v < 0)
        {
            *p++ = static_cast<CharT>('-');
            --buffer_size;
            // avoid UB for INT_MIN
            uvalue = static_cast<UnsignedT>(-(v + 1)) + 1;
        }
        else
        {
            uvalue = static_cast<UnsignedT>(v);
        }

        do
        {
            *bp++ = static_cast<CharT>(static_cast<int>(uvalue % 10) + '0');
            uvalue = uvalue / 10;
        } while(uvalue != 0);

        // write digits in reverse
        while(bp != buf && buffer_size-- > 0)
        {
            --bp;
            *p++ = *bp;
        }

        if(buffer_size > 0)
            *p = 0;

        return static_cast<int>(p - buffer);
    }

    /**
     * @brief CN: 将有符号整数转换为字符串并返回指针（便捷包装）。
     * @brief EN: Convert signed integer to string and return buffer pointer (convenience wrapper).
     */
    template<typename CharT, typename IntT>
    inline CharT *itos(CharT *str, int size, const IntT num)
    {
        hgl::itos_rl(str, size, num);
        return str;
    }

    /**
     * @brief CN: 将无符号整数转换为字符串。
     * @brief EN: Convert an unsigned integer to string.
     */
    template<typename CharT, typename UIntT>
    inline CharT *utos(CharT *str, int size, UIntT value)
    {
        if(!str || size <= 0) return nullptr;

        CharT *p = str;
        CharT buf[sizeof(UIntT) * 8];
        CharT *bp = buf;

        do
        {
            *bp++ = static_cast<CharT>(static_cast<int>(value % 10) + '0');
            value = value / 10;
        } while(value != 0);

        while(bp != buf && size-- > 0)
        {
            --bp;
            *p++ = *bp;
        }

        if(size > 0)
            *p = 0;

        return str;
    }

    /**
     * @brief CN: 将无符号整数按指定进制转换为字符串。
     * @brief EN: Convert an unsigned integer to string with specified base.
     */
    template<typename CharT, typename UIntT>
    inline CharT *utos(CharT *str, int size, UIntT value, unsigned int base, bool upper=true)
    {
        if(!str || size <= 0) return nullptr;

        CharT *p = str;
        CharT buf[sizeof(UIntT) * 8];
        CharT *bp = buf;

        const CharT A = static_cast<CharT>(upper ? 'A' : 'a');

        do
        {
            unsigned int m = static_cast<unsigned int>(value % base);
            if(m < 10)
                *bp++ = static_cast<CharT>(m + '0');
            else
                *bp++ = static_cast<CharT>((m - 10) + A);

            value /= base;
        } while(value != 0);

        while(bp != buf && size-- > 0)
        {
            --bp;
            *p++ = *bp;
        }

        if(size > 0)
            *p = 0;

        return str;
    }

    template<int BITS> struct htos_bits
    {
        template<typename CharT, typename UIntT> static CharT *conv(CharT *str,int size,UIntT value,bool upper);
    };

    template<> struct htos_bits<1>{template<typename CharT,typename UIntT> static inline CharT *conv(CharT *str,int size,UIntT value,bool upper){return hgl::utos(str,size,static_cast<uint8  >(value),16,upper);}};
    template<> struct htos_bits<2>{template<typename CharT,typename UIntT> static inline CharT *conv(CharT *str,int size,UIntT value,bool upper){return hgl::utos(str,size,static_cast<uint16 >(value),16,upper);}};
    template<> struct htos_bits<4>{template<typename CharT,typename UIntT> static inline CharT *conv(CharT *str,int size,UIntT value,bool upper){return hgl::utos(str,size,static_cast<uint32 >(value),16,upper);}};
    template<> struct htos_bits<8>{template<typename CharT,typename UIntT> static inline CharT *conv(CharT *str,int size,UIntT value,bool upper){return hgl::utos(str,size,static_cast<uint64 >(value),16,upper);}};

    /**
     * @brief CN: 将无符号整数转换为十六进制字符串（根据类型字节数选择宽度）。
     * @brief EN: Convert an unsigned integer to hex string (width based on type size).
     */
    template<typename CharT,typename UIntT>
    inline CharT *htos(CharT *str,int size,UIntT value,bool upper=true)
    {
        return htos_bits<sizeof(UIntT)>::template conv<CharT,UIntT>(str,size,value,upper);
    }

    template<typename CharT,typename UIntT> inline CharT *htos_upper(CharT *str,int size,UIntT value){return hgl::htos<CharT,UIntT>(str,size,value,true);} 
    template<typename CharT,typename UIntT> inline CharT *htos_lower(CharT *str,int size,UIntT value){return hgl::htos<CharT,UIntT>(str,size,value,false);} 

    /**
     * @brief CN: 将浮点数转换为字符串，保留 fsize 小数位。
     * @brief EN: Convert floating point value to string with fsize fractional digits.
     */
    template<typename CharT,typename FloatT>
    inline CharT *ftos(CharT *str,int size,int fsize,FloatT value)
    {
        const long integer = static_cast<long>(value);
        CharT *p = str;

        if(integer == 0 && value < 0)
            *p++ = static_cast<CharT>('-');

        hgl::itos(p,size,integer);

        int len = hgl::strlen(str);
        if(len >= size) return str;

        p = str + len;
        value -= integer;
        if(value < 0) value = -value;

        const FloatT min_value = static_cast<FloatT>(std::pow(0.1F, fsize));
        if(value < min_value)
        {
            *p = 0;
            return str;
        }

        *p++ = static_cast<CharT>('.');
        ++len;

        while(value > min_value && len < size && fsize-- > 0)
        {
            value *= 10;
            *p++ = static_cast<CharT>('0' + int(value));
            value -= int(value);
            ++len;
        }

        *p = 0;
        return str;
    }

    template<typename CharT,typename FloatT>
    inline CharT *ftos(CharT *str,int size,FloatT value)
    {
        return hgl::ftos(str,size,4,value);
    }

    /**
     * @brief CN: 将整数转换为带单位表示（KB/MB/...），例如用于显示大小。
     * @brief EN: Convert integer to human-readable size string with units (K/M/G...).
     */
    template<typename CharT,typename ValueT>
    inline bool int_to_size(CharT *str,int str_max_length,ValueT value)
    {
        const CharT name[] = { 'K','M','G','T','P','E','Z','Y','B',0 };

        double new_value = static_cast<double>(value);
        int pos = 0;

        while(new_value >= 1024.0 && str_max_length-- > 0)
        {
            pos++;
            new_value /= 1024.0;
            if(name[pos] == 0) return false;
        }

        const float f = static_cast<float>(new_value);
        hgl::ftos(str, str_max_length, 2, f);

        CharT *p = str + hgl::strlen(str);
        *p++ = static_cast<CharT>(' ');
        if(pos) *p++ = name[pos-1];
        *p++ = static_cast<CharT>('B');
        *p = 0;

        return true;
    }

    /**
     * @brief CN: 解析单个十进制字符为其数值（0-9）。
     * @brief EN: Parse a single decimal digit character to its numeric value (0-9).
     *
     * @param[in] ch CN: 字符（以 int 传入以兼容 EOF 等）。EN: character code (int for EOF compatibility).
     * @return int CN/EN: 解析结果（未解析则返回 0）。EN: parsed digit value (0 if not a digit).
     */
    inline int parse_dec_number_char(int ch)
    {
        if(ch >= '0' && ch <= '9')
            return ch - '0';
        return 0;
    }

    /**
     * @brief CN: 将单个字符按给定进制解析成数值（支持大写小写字母）。
     * @brief EN: Parse a single character according to given base into its numeric value (supports upper/lowercase letters).
     *
     * @tparam BASE 进制（如 16）。EN: numeric base (e.g. 16).
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] ch CN: 要解析的字符。EN: character to parse.
     * @return int CN/EN: 解析出的数值，如果不合法返回 0。EN: parsed numeric value, or 0 on invalid input.
     */
    template<unsigned int BASE, typename CharT>
    inline int parse_number_char(CharT ch)
    {
        if(ch >= '0' && ch <= '9')
            return ch - '0';

        if(BASE > 10)
        {
            int max_offset = static_cast<int>(BASE - 11);
            CharT lower_max = static_cast<CharT>(static_cast<int>('a') + max_offset);
            CharT upper_max = static_cast<CharT>(static_cast<int>('A') + max_offset);

            if(ch >= 'a' && ch <= lower_max)
                return 10 + (ch - 'a');

            if(ch >= 'A' && ch <= upper_max)
                return 10 + (ch - 'A');
        }

        return 0;
    }
}//namespace hgl
