#pragma once

/**
 * CN:  在这个文件里我们实现了一些字符与字符串处理的函数
 *      这些函数的实现并不是为了追求性能，或炫技，而是为了统一不同编译器、标准库对一些边界、非法值的处理方式。
 *
 * EN:  This file implements some character and string handling functions.
 *      The purpose is not to pursue performance or show off skills,
 *      but to unify the handling of certain edge cases and illegal values
 *      across different compilers and standard libraries.
 */

#include<hgl/type/Constants.h>
#include<type_traits>
#include <cstdint>
#include <cstddef>
#include <utility>

namespace hgl
{
    //  32  空格
    //  33  !
    //  34  "
    //  35  #
    //  36  $
    //  37  %
    //  38  &
    //  39  '
    //  40  (
    //  41  )
    //  42  *
    //  43  +
    //  44  ,
    //  45  -
    //  46  .
    //  47  /
    //  48  0123456789
    //  58  :
    //  59  ;
    //  60  <
    //  61  =
    //  62  >
    //  63  ?
    //  64  @
    //  65  ABCDEFGHIJKLMNOPQRSTUVWXYZ
    //  91  [
    //  92  \
    //  93  ]
    //  94  ^
    //  95  _
    //  96  `
    //  97  abcdefghijklmnopqrstuvwxyz
    //  123 {
    //  124 |
    //  125 }
    //  126 ~

    /**
     * @brief
     * CN: 测试当前字符是否为小写字母（ASCII范围）
     * EN: Test whether the current character is a lowercase ASCII letter.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果为小写字母返回 true，否则返回 false。
     * EN: Returns true if it is a lowercase letter, otherwise false.
     */
    template<typename T>
    constexpr bool is_lower_alpha(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_lower_alpha requires integral/character type");
        return(ch>='a'&&ch<='z');
    }

    /**
     * @brief
     * CN: 测试当前字符是否为大写字母（ASCII范围）
     * EN: Test whether the current character is an uppercase ASCII letter.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果为大写字母返回 true，否则返回 false。
     * EN: Returns true if it is an uppercase letter, otherwise false.
     */
    template<typename T>
    constexpr bool is_upper_alpha(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_upper_alpha requires integral/character type");
        return(ch>='A'&&ch<='Z');
    }

    /**
     * @brief
     * CN: 测试当前字符是否为字母（ASCII 小写或大写）
     * EN: Test whether the current character is an ASCII letter (lowercase or uppercase).
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果为字母返回 true，否则返回 false。
     * EN: Returns true if it is a letter, otherwise false.
     */
    template<typename T>
    constexpr bool is_alpha(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_alpha requires integral/character type");
        return(hgl::is_lower_alpha(ch)||hgl::is_upper_alpha(ch));
    }

    /**
     * @brief
     * CN: 测试当前字符是否为十进制数字（'0'..'9'）
     * EN: Test whether the current character is a decimal digit ('0'..'9').
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果为数字返回 true，否则返回 false。
     * EN: Returns true if it is a digit, otherwise false.
     */
    template<typename T>
    constexpr bool is_digit(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_digit requires integral/character type");
        return(ch>='0'&&ch<='9');
    }

    /**
     * @brief
     * CN: 判断字符是否可以出现在表示浮点数的文本中。
     * EN: Determine whether the character can appear in text representing a floating-point number.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 若字符是数字、正负号、小数点、科学计数标识（E/e）或浮点后缀（f/F）则返回 true，否则 false。
     * EN: Returns true if the character is a digit, sign, decimal point, exponent marker (E/e), or float suffix (f/F).
     */
    template<typename T>
    constexpr bool is_float_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_float_char requires integral/character type");
        return hgl::is_digit(ch)
        || ch == '-' 
        || ch == '+' 
        || ch == '.' 
        || ch == 'E' 
        || ch == 'e' 
        || ch == 'f' 
        || ch == 'F';
    }

    /**
     * @brief
     * CN: 判断字符是否可以出现在表示整数的文本中（数字或符号）
     * EN: Determine whether the character can appear in text representing an integer (digit or sign).
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果为数字或正负号返回 true，否则 false。
     * EN: Returns true if it is a digit or a sign character, otherwise false.
     */
    template<typename T>
    constexpr bool is_integer_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_integer_char requires integral/character type");
        return hgl::is_digit(ch)
        || ch == '-' 
        || ch == '+';
    }

    /**
     * @brief
     * CN: 测试当前字符是否为十六进制数字字符（0-9, a-f, A-F）
     * EN: Test whether the current character is a hexadecimal digit (0-9, a-f, A-F).
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 若字符为合法的十六进制字符则返回 true，否则 false。
     * EN: Returns true if the character is a valid hexadecimal digit, otherwise false.
     */
    template<typename T>
    constexpr bool is_hex_digit(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_hex_digit requires integral/character type");
        // use character literals for readability
        return (
            (ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F')
        );
    }

    /**
     * @brief
     * CN: 检查宽字符指针所指的字符串在指定长度内是否全部为十六进制字符。
     * EN: Check whether a string pointed by a character pointer consists entirely of hexadecimal digits within the given length.
     *
     * @tparam T 字符类型 / character type
     * @param str 要测试的字符串指针（不能为空） / pointer to the string to test (must not be null)
     * @param length 最大检查长度 / maximum length to check
     * @return
     * CN: 若 str 不为空且在 length 范围内全部为十六进制字符，返回 true；否则返回 false。
     * EN: Returns true if str is non-null and all characters within length are hexadecimal digits; otherwise false.
     */
    template<typename T>
    constexpr bool is_hex_digit(const T *str, std::size_t length) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_hex_digit requires integral/character type");
        if(!str || length==0)
            return(false);

        while(*str && length)
        {
            if(!hgl::is_hex_digit(*str))
                return(false);

            ++str;
            --length;
        }

        return(true);
    }

    /**
     * @brief
     * CN: 测试字符是否为路径或 URL 中可以使用的斜杠字符（'/' 或 '\\'）。
     * EN: Test whether the character is a slash used in paths or URLs ('/' or '\\').
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果字符为正斜杠或反斜杠返回 true，否则 false。
     * EN: Returns true if the character is '/' or '\\', otherwise false.
     */
    template<typename T>
    constexpr bool is_slash(const T &ch)
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_slash requires integral/character type");
        return (ch == '\\') || (ch == '/');
    }

    /**
     * @brief
     * CN: 提供一个对 NUL (0) 不视为空白的通用 is_space 实现（ASCII 控制字符集合）。
     * EN: Provide a generic is_space implementation that does not treat NUL (0) as whitespace (ASCII control characters).
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果字符为普通空白控制字符（' ', '\f', '\n', '\r', '\t', '\v'）或音响/退格（注意：实施中包含 '\a' 和 '\b'），返回 true；否则 false。
     * EN: Returns true if the character is one of the whitespace/control characters listed (note '\a' and '\b' are included in this implementation), otherwise false.
     */
    template<typename T>
    constexpr bool is_space(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_space requires integral/character type");
        // ASCII control/whitespace characters (explicitly exclude NUL)
        return (
            ch == static_cast<T>(' ') ||
            ch == static_cast<T>('\a') ||
            ch == static_cast<T>('\b') ||
            ch == static_cast<T>('\f') ||
            ch == static_cast<T>('\n') ||
            ch == static_cast<T>('\r') ||
            ch == static_cast<T>('\t') ||
            ch == static_cast<T>('\v')
        );
    }

    /**
     * @brief
     * CN: u32char 专用的 is_space 特化，包含全角空格判定并保持与基本模板一致的控制字符集。
     * EN: u32char specialization of is_space; includes fullwidth space checks and the same control characters.
     *
     * @param ch 要测试的 32 位字符 / 32-bit character to test
     * @return
     * CN: 若字符为常见空白或全角空格，返回 true，否则 false。
     * EN: Returns true for common whitespace or fullwidth space, otherwise false.
     */
    template<>
    constexpr bool is_space(u32char ch) noexcept
    {
        return(
        /* no NUL */
        ch==U' '              //半角空格
        ||ch==U32_FULL_WIDTH_SPACE   //全角空格
        ||ch==U'\a'
        ||ch==U'\b'
        ||ch==U'\f'
        ||ch==U'\n'
        ||ch==U'\r'
        ||ch==U'\t'
        ||ch==U'\v');
    }

    /**
     * @brief
     * CN: u16char 专用的 is_space 特化（在 BMP 范围内），使用项目中定义的 U16_TEXT 与全角空格宏。
     * EN: u16char specialization of is_space (within BMP), using project's U16_TEXT and fullwidth space macros.
     *
     * @param ch 要测试的 16 位字符 / 16-bit character to test
     * @return
     * CN: 若字符为常见空白或全角空格，返回 true，否则 false。
     * EN: Returns true for common whitespace or fullwidth space, otherwise false.
     */
    template<>
    constexpr bool is_space(u16char ch) noexcept
    {
        return(
        /* no NUL */
        ch==U16_TEXT(' ')    //半角空格
        ||ch==U16_FULL_WIDTH_SPACE   //全角空格
        ||ch==U16_TEXT('\a')
        ||ch==U16_TEXT('\b')
        ||ch==U16_TEXT('\f')
        ||ch==U16_TEXT('\n')
        ||ch==U16_TEXT('\r')
        ||ch==U16_TEXT('\t')
        ||ch==U16_TEXT('\v'));
    }

    /**
     * @brief
     * CN: char 专用的 is_space 特化（ASCII 单字节）
     * EN: char specialization of is_space (single-byte ASCII).
     *
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 若字符为常见空白或控制字符返回 true，否则 false。
     * EN: Returns true if the character is a common whitespace/control character, otherwise false.
     */
    template<>
    constexpr bool is_space(char ch) noexcept
    {
        return(
        /* no NUL */
        ch==' '              //半角空格
        ||ch=='\a'
        ||ch=='\b'
        ||ch=='\f'
        ||ch=='\n'
        ||ch=='\r'
        ||ch=='\t'
        ||ch=='\v');
    }

    #ifdef HGL_SUPPORT_CHAR8_T
    /**
     * @brief
     * CN: char8_t 专用的 is_space 特化（UTF-8 字面量类型）
     * EN: char8_t specialization of is_space (UTF-8 literal type).
     *
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 若字符为常见空白或控制字符返回 true，否则 false。
     * EN: Returns true if the character is a common whitespace/control character, otherwise false.
     */
    template<>
    constexpr bool is_space(char8_t ch) noexcept
    {
        return(
        /* no NUL */
        ch==u8' '              //半角空格
        ||ch==u8'\a'
        ||ch==u8'\b'
        ||ch==u8'\f'
        ||ch==u8'\n'
        ||ch==u8'\r'
        ||ch==u8'\t'
        ||ch==u8'\v');
    }
    #endif//char8_t

    /**
     * @brief
     * CN: 测试字符是否为字母或数字（字母包含大小写）
     * EN: Test whether the character is alphabetic or numeric (letters include both cases).
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 若为字母或数字返回 true，否则 false。
     * EN: Returns true if it is a letter or digit, otherwise false.
     */
    template<typename T>
    constexpr bool is_alpha_numeric(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_alpha_numeric requires integral/character type");
        return(hgl::is_alpha(ch)||hgl::is_digit(ch));
    }

    /**
     * @brief
     * CN: 测试字符是否符合常见的标识符字符（字母、数字或下划线）
     * EN: Test whether the character is a common identifier character (letter, digit or underscore).
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 若为标识符字符返回 true，否则 false。
     * EN: Returns true if it is an identifier character, otherwise false.
     */
    template<typename T>
    constexpr bool is_identifier_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_identifier_char requires integral/character type");
        return(hgl::is_alpha_numeric(ch)||ch=='_');
    }

    /**
     * @brief
     * CN: 测试字符是否不是标识符字符
     * EN: Test whether the character is not an identifier character.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果不是标识符字符返回 true，否则 false。
     * EN: Returns true if it is not an identifier character, otherwise false.
     */
    template<typename T>
    constexpr bool is_not_identifier_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_not_identifier_char requires integral/character type");
        return(!hgl::is_identifier_char(ch));
    }

    /**
     * @brief
     * CN: 测试字符是否为文件名允许的字符（点或标识符字符），不含路径分隔符。
     * EN: Test whether the character is allowed in a filename (dot or identifier character), excluding path separators.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 若为文件名允许字符返回 true，否则 false。
     * EN: Returns true if it is allowed in a filename, otherwise false.
     */
    template<typename T>
    constexpr bool is_filename_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_filename_char requires integral/character type");
        return(ch=='.'||hgl::is_identifier_char(ch));
    }

    /**
     * @brief
     * CN: 测试字符是否不是文件名允许字符
     * EN: Test whether the character is not allowed in a filename.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要测试的字符 / character to test
     * @return
     * CN: 如果不是文件名允许字符返回 true，否则 false。
     * EN: Returns true if it is not allowed in a filename, otherwise false.
     */
    template<typename T>
    constexpr bool is_not_filename_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_not_filename_char requires integral/character type");
        return(!hgl::is_filename_char(ch));
    }

    /**
     * @brief
     * CN: 测试字符是否为 Base64 编码字符（包含填充字符 '='）
     * EN: Test whether the character is a Base64 character (including padding '=').
     *
     * @tparam T 字符类型 / character type
     * @param c 要测试的字符 / character to test
     * @return
     * CN: 若字符属于 Base64 集合则返回 true，否则 false。
     * EN: Returns true if the character belongs to the Base64 set, otherwise false.
     */
    template<typename T>
    constexpr bool is_base64_char(T c) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_base64_char requires integral/character type");
        // use character literals for clarity and include padding '='
        return (
            c == '+' || c == '/' || c == '=' ||
            (c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z')
        );
    }

    /**
     * @brief
     * CN: 若当前字符为大写英文字符，则转换为小写（仅影响 ASCII 范围）
     * EN: Convert the character to lowercase if it is an ASCII uppercase letter.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要转换的字符 / character to convert
     * @return
     * CN: 返回转换后的字符（非大写字符保持不变）。
     * EN: Returns the converted character (unchanged if not uppercase ASCII).
     */
    template<typename T>
    constexpr T to_lower_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "to_lower_char requires integral/character type");
        if(ch>='A'&&ch<='Z')
            return ch+('a'-'A');
        else
            return ch;
    }

    /**
     * @brief
     * CN: 若当前字符为小写英文字符，则转换为大写（仅影响 ASCII 范围）
     * EN: Convert the character to uppercase if it is an ASCII lowercase letter.
     *
     * @tparam T 字符类型 / character type
     * @param ch 要转换的字符 / character to convert
     * @return
     * CN: 返回转换后的字符（非小写字符保持不变）。
     * EN: Returns the converted character (unchanged if not lowercase ASCII).
     */
    template<typename T>
    constexpr T to_upper_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "to_upper_char requires integral/character type");
        if(ch>='a'&&ch<='z')
            return ch+('A'-'a');
        else
            return ch;
    }

    // Unicode-aware overloads for UTF-16/UTF-32 character types.
    // These provide basic Unicode case support: ASCII, fullwidth Latin letters and common Latin-1 uppercase ranges.
    // Full Unicode case folding is complex and would require a dedicated library (ICU) or a complete mapping table.

    /**
     * @brief
     * CN: u32char 版本的 to_lower_char，支持 ASCII、全角拉丁字符以及部分 Latin-1 扩展范围的简单映射。
     * EN: u32char overload of to_lower_char. Supports ASCII, fullwidth Latin letters and simple Latin-1 ranges.
     *
     * @param ch 要转换的 32 位字符 / 32-bit character to convert
     * @return
     * CN: 返回转换后的字符（若无映射则返回原字符）。
     * EN: Returns the converted character; if no mapping exists, returns the original char.
     */
    constexpr u32char to_lower_char(u32char ch) noexcept
    {
        // ASCII
        if (ch >= U'A' && ch <= U'Z')
            return ch + (U'a' - U'A');

        // Fullwidth Latin uppercase (FF21-FF3A) -> fullwidth lowercase (FF41-FF5A)
        if (ch >= 0xFF21u && ch <= 0xFF3Au)
            return ch + 0x20u;

        // Latin-1 Supplement uppercase ranges that map by +0x20 (simple cases)
        if ((ch >= 0x00C0u && ch <= 0x00D6u) || (ch >= 0x00D8u && ch <= 0x00DEu))
            return ch + 0x20u;

        return ch;
    }

    /**
     * @brief
     * CN: u32char 版本的 to_upper_char，支持 ASCII、全角拉丁字符以及部分 Latin-1 扩展范围的简单映射。
     * EN: u32char overload of to_upper_char. Supports ASCII, fullwidth Latin letters and simple Latin-1 ranges.
     *
     * @param ch 要转换的 32 位字符 / 32-bit character to convert
     * @return
     * CN: 返回转换后的字符（若无映射则返回原字符）。
     * EN: Returns the converted character; if no mapping exists, returns the original char.
     */
    constexpr u32char to_upper_char(u32char ch) noexcept
    {
        // ASCII
        if (ch >= U'a' && ch <= U'z')
            return ch - (U'a' - U'A');

        // Fullwidth Latin lowercase (FF41-FF5A) -> fullwidth uppercase (FF21-FF3A)
        if (ch >= 0xFF41u && ch <= 0xFF5Au)
            return ch - 0x20u;

        // Latin-1 Supplement lowercase ranges that map by -0x20 (simple cases)
        if ((ch >= 0x00E0u && ch <= 0x00F6u) || (ch >= 0x00F8u && ch <= 0x00FEu))
            return ch - 0x20u;

        return ch;
    }

    /**
     * @brief
     * CN: u16char 版本的 to_lower_char，内部提升到 u32char 后复用实现（限 BMP）。
     * EN: u16char overload of to_lower_char; promotes to u32char and reuses the u32char implementation.
     *
     * @param ch 要转换的 16 位字符 / 16-bit character to convert
     * @return
     * CN: 返回转换后的 16 位字符。
     * EN: Returns the converted 16-bit character.
     */
    constexpr u16char to_lower_char(u16char ch) noexcept
    {
        // Promote to 32-bit to reuse above logic safely
        const u32char c = static_cast<u32char>(ch);
        const u32char r = to_lower_char(c);
        return static_cast<u16char>(r);
    }

    /**
     * @brief
     * CN: u16char 版本的 to_upper_char，内部提升到 u32char 后复用实现（限 BMP）。
     * EN: u16char overload of to_upper_char; promotes to u32char and reuses the u32char implementation.
     *
     * @param ch 要转换的 16 位字符 / 16-bit character to convert
     * @return
     * CN: 返回转换后的 16 位字符。
     * EN: Returns the converted 16-bit character.
     */
    constexpr u16char to_upper_char(u16char ch) noexcept
    {
        const u32char c = static_cast<u32char>(ch);
        const u32char r = to_upper_char(c);
        return static_cast<u16char>(r);
    }

    /**
     * @brief
     * CN: 比较两个字符在不区分大小写时的先后关系（使用 to_lower_char 做对比）。
     * EN: Compare two characters case-insensitively using to_lower_char conversion.
     *
     * @tparam S 源字符类型 / source character type
     * @tparam D 目标字符类型 / destination character type
     * @param src 源字符 / source character
     * @param dst 目标字符 / destination character
     * @return
     * CN: 返回 (lower(src) - lower(dst)) 的整型差值（注意对宽字符可能存在截断到 int 的风险）。
     * EN: Returns the integer difference of lower(src) - lower(dst) (note possible truncation to int for wide types).
     */
    template<typename S,typename D>
    constexpr int compare_char_icase(S src,D dst) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<S>::type>::value && std::is_integral<typename std::remove_cv<D>::type>::value,
            "compare_char_icase requires integral/character types");

        // Promote to 32-bit Unicode-aware characters for comparison to avoid truncation
        const u32char ls = hgl::to_lower_char(static_cast<u32char>(src));
        const u32char ld = hgl::to_lower_char(static_cast<u32char>(dst));

        if (ls < ld) return -1;
        if (ls > ld) return  1;
        return 0;
    }

    // safe character equality across possibly different character types
    template<typename A, typename B>
    inline bool char_eq(A a, B b)
    {
        using C = std::common_type_t<A, B>;
        return static_cast<C>(a) == static_cast<C>(b);
    }

    /**
     * @brief
     * CN: 检测字符串是否符合代码命名规则（只能使用字母、数字、下划线，且首字符不能为数字）。
     * EN: Check whether a string is a valid identifier (letters, digits, underscore; cannot start with a digit).
     *
     * @tparam T 字符类型 / character type
     * @param str 指向以 NUL 结尾的字符串 / pointer to NUL-terminated string
     * @return
     * CN: 若 str 为合法标识符返回 true；若 str 为空或包含非法字符返回 false。
     * EN: Returns true if str is a valid identifier; false if str is null or contains invalid characters.
     */
    template<typename T> constexpr bool is_valid_identifier(const T *str) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_valid_identifier requires integral/character type");

        if(!str)return(false);

        if((!hgl::is_alpha(*str))&&(*str!='_'))       //不是字母或下划线
            return(false);

        // allow single-character identifiers; validate remaining characters if any
        ++str;

        while(*str)
            if(!hgl::is_identifier_char(*str++))
                return(false);

        return(true);
    }

    /**
     * @brief
     * CN: 检测字符串是否包含不可用于程序标识或文件名的字符（空白字符单独使用 is_space 检测）。
     * EN: Detect whether a string contains characters invalid for identifiers or filenames (whitespace handled by is_space).
     *
     * @tparam T 字符类型 / character type
     * @param str 指向以 NUL 结尾的字符串 / pointer to NUL-terminated string
     * @return
     * CN: 若包含无效字符或空白返回 true；否则返回 false。若 str 为 nullptr 返回 false。
     * EN: Returns true if the string contains invalid characters or whitespace; false otherwise. If str is nullptr returns false.
     */
    template<typename T> constexpr bool contains_invalid_chars(const T *str) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "contains_invalid_chars requires integral/character type");

        if(!str)return(false);

        // list of characters considered invalid for identifiers/filenames (space handled by is_space())
        constexpr T err_chr[] = {
            static_cast<T>('<'),
            static_cast<T>('>'),
            static_cast<T>(','),
            static_cast<T>('/'),
            static_cast<T>('\\'),
            static_cast<T>('|'),
            static_cast<T>('?'),
            static_cast<T>('%'),
            static_cast<T>('$'),
            static_cast<T>('#'),
            static_cast<T>('@'),
            static_cast<T>('`'),
            static_cast<T>( '\'' ),
            static_cast<T>(':') ,
            static_cast<T>('"'),
            static_cast<T>('*'),
            static_cast<T>('&'),
            static_cast<T>('!'),
            static_cast<T>(0)
        };

        const T *sp;

        while(*str)
        {
            if(hgl::is_space(*str))
                return(true);

            sp=err_chr;

            while(*sp)
            {
                if(*str == *sp)
                    return(true);

                ++sp;
            }

            ++str;
        }

        return(false);
    }

    // u16char specialization
    /**
     * @brief
     * CN: u16char 专用的 contains_invalid_chars 实现，使用将狭义的 ASCII 无效字符与宽字符比较。
     * EN: u16char specialization for contains_invalid_chars. Compares ASCII invalid characters cast to u16char.
     *
     * @param str 指向以 NUL 结尾的 u16 字符串 / pointer to NUL-terminated u16 string
     * @return
     * CN: 若发现无效字符或空白返回 true；若 str 为 nullptr 或未发现无效字符返回 false。
     * EN: Returns true if an invalid char or whitespace is found; false if str is nullptr or none found.
     */
    template<>
    constexpr bool contains_invalid_chars(const u16char *str) noexcept
    {
        if(!str) return false;

        while(*str)
        {
            if(hgl::is_space(*str))
                return true;

            switch(*str)
            {
                case u'<': case u'>': case u',': case u'/': case u'\\': case u'|':
                case u'?': case u'%': case u'$': case u'#': case u'@': case u'`':
                case u'\'': case u':': case u'"': case u'*': case u'&': case u'!':
                    return true;
                default:
                    break;
            }

            ++str;
        }

        return false;
    }

    // u32char specialization
    /**
     * @brief
     * CN: u32char 专用的 contains_invalid_chars 实现，使用宽字符字面量比较以避免字节转化问题。
     * EN: u32char specialization for contains_invalid_chars. Uses wide character literals for comparison.
     *
     * @param str 指向以 NUL 结尾的 u32 字符串 / pointer to NUL-terminated u32 string
     * @return
     * CN: 若发现无效字符或空白返回 true；否则 false。
     * EN: Returns true if an invalid char or whitespace is found; otherwise false.
     */
    template<>
    constexpr bool contains_invalid_chars(const u32char *str) noexcept
    {
        if(!str) return false;

        while(*str)
        {
            if(hgl::is_space(*str))
                return true;

            switch(*str)
            {
                case U'<': case U'>': case U',': case U'/': case U'\\': case U'|':
                case U'?': case U'%': case U'$': case U'#': case U'@': case U'`':
                case U'\'': case U':': case U'"': case U'*': case U'&': case U'!':
                    return true;
                default:
                    break;
            }

            ++str;
        }

        return false;
    }

}//namespace hgl
