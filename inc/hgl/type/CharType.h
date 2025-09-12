#pragma once

/**
 * CN:  在这个文件里我们实现了一些字符与字符串处理的函数
 *      这些函数的实现并不是为了追求性能，或炫技，而是为了统一不同编译器、标准库对一些边界、非法值的处理方式。
 *
 *      EN:  This file implements some character and string handling functions.
 *      The purpose is not to pursue performance or show off skills,
 *      but to unify the handling of certain edge cases and illegal values
 *      across different compilers and standard libraries.
 */

#include <hgl/TypeFunc.h>
#include <type_traits>
#include <cstdint>

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
     * 测试字符是否是emoji表情<br>
     * 参见https://unicode.org/Public/emoji/12.0/emoji-data.txt
     *
     * NOTE: emoji handling is temporarily disabled here. See TODO below.
     */
#if 0 // TODO: Revisit emoji detection and replace with proper Unicode property checks or a lookup table.
    template<typename T>
    inline constexpr bool is_emoji(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isemoji requires integral/character type");
        // Generic/smaller-width types (char, u16char, etc.) should only handle
        // the common single-unit emoji/punctuation characters to avoid
        // comparing against very large Unicode code points they cannot represent.
        if(ch==0x23) return true;           //#
        if(ch==0x2A) return true;           //*
        if(ch>=0x30 && ch<=0x39) return true; //0-9
        if(ch==0xA9) return true;           //©
        if(ch==0xAE) return true;           //®

        return false;
    }

    // u32char specialization: supports full range checks
    template<>
    inline constexpr bool is_emoji(u32char ch) noexcept
    {
        if(ch==0x23) return true;           //#
        if(ch==0x2A) return true;           //*
        if(ch>=0x30 && ch<=0x39) return true; //0-9
        if(ch==0xA9) return true;           //©
        if(ch==0xAE) return true;           //®
        if(ch>=0x203C && ch<=0x1FFFD) return true;

        return false;
    }
#endif

    /**
     * 测试当前字符是否为小写字母
     */
    template<typename T>
    inline constexpr bool is_lower_alpha(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_lower_alpha requires integral/character type");
        return(ch>='a'&&ch<='z');
    }

    /**
     * 测试当前字符是否为大写字母
     */
    template<typename T>
    inline constexpr bool is_upper_alpha(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_upper_alpha requires integral/character type");
        return(ch>='A'&&ch<='Z');
    }

    /**
     * 测试当前字符是否为字母
     */
    template<typename T>
    inline constexpr bool is_alpha(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_alpha requires integral/character type");
        return(hgl::is_lower_alpha(ch)||hgl::is_upper_alpha(ch));
    }

    /**
     * 测试当前字符是否为10进制数字
     */
    template<typename T>
    inline constexpr bool is_digit(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_digit requires integral/character type");
        return(ch>='0'&&ch<='9');
    }

    /**
     * 测试当前字符串是否为10进制数字以及小数点、正负符号、指数字符
     */
    template<typename T>
    inline constexpr bool is_float_char(T ch) noexcept
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

    template<typename T>
    inline constexpr bool is_integer_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_integer_char requires integral/character type");
        return hgl::is_digit(ch)
        || ch == '-' 
        || ch == '+';
    }

    /**
     * 测试当前字符是否为16进制数用字符(0-9,A-F)
     */
    template<typename T>
    inline constexpr bool is_hex_digit(T ch) noexcept
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
     * 测试当前字符串是否为16进制数用字符
     * @param str 字符串
     * @param length 字符串长度
     */
    template<typename T>
    inline constexpr bool is_hex_digit(const T *str,int length) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_hex_digit requires integral/character type");
        if(!str||length<=0)
            return(false);

        while(*str&&length)
        {
            if(!hgl::is_hex_digit(*str))
                return(false);

            ++str;
            --length;
        }

        return(true);
    }

    /**
     * 是否为斜杠
     */
    template<typename T>
    inline constexpr bool is_slash(const T &ch)
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_slash requires integral/character type");
        return (ch == '\\') || (ch == '/');
    }

    // Provide a safe primary template for is_space. Do not treat NUL (0) as whitespace.
    template<typename T>
    inline constexpr bool is_space(T ch) noexcept
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
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline constexpr bool is_space(u32char ch) noexcept
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
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline constexpr bool is_space(u16char ch) noexcept
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
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline constexpr bool is_space(char ch) noexcept
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
    template<>
    inline constexpr bool is_space(char8_t ch) noexcept
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
     * 测试当前字符是否为字母或数字
     */
    template<typename T>
    inline constexpr bool is_alpha_numeric(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_alpha_numeric requires integral/character type");
        return(hgl::is_alpha(ch)||hgl::is_digit(ch));
    }

    /**
     * 测试当前字符是否为代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline constexpr bool is_identifier_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_identifier_char requires integral/character type");
        return(hgl::is_alpha_numeric(ch)||ch=='_');
    }

    /**
     * 测试当前字符是否不是代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline constexpr bool is_not_identifier_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_not_identifier_char requires integral/character type");
        return(!hgl::is_identifier_char(ch));
    }

    /**
     * 测试当前字符是否为文件名可用字符(不含路径分隔符)
     */
    template<typename T>
    inline constexpr bool is_filename_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_filename_char requires integral/character type");
        return(ch=='.'||hgl::is_identifier_char(ch));
    }

    /**
     * 测试当前字符是否不是文件名可用字符
     */
    template<typename T>
    inline constexpr bool is_not_filename_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "is_not_filename_char requires integral/character type");
        return(!hgl::is_filename_char(ch));
    }

    /**
     * 测试当前字符是否为BASE64编码字符
     */
    template<typename T>
    inline constexpr bool is_base64_char(T c) noexcept
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
     * 如果当前字符为大写英文字符，则转换为小写
     */
    template<typename T>
    inline constexpr T to_lower_char(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "to_lower_char requires integral/character type");
        if(ch>='A'&&ch<='Z')
            return ch+('a'-'A');
        else
            return ch;
    }

    /**
     * 如果当前字符为小写英文字符，则转换为大写
     */
    template<typename T>
    inline constexpr T to_upper_char(T ch) noexcept
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

    // u32char overload
    inline constexpr u32char to_lower_char(u32char ch) noexcept
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

    inline constexpr u32char to_upper_char(u32char ch) noexcept
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

    // u16char overload (maps within BMP)
    inline constexpr u16char to_lower_char(u16char ch) noexcept
    {
        // Promote to 32-bit to reuse above logic safely
        const u32char c = static_cast<u32char>(ch);
        const u32char r = to_lower_char(c);
        return static_cast<u16char>(r);
    }

    inline constexpr u16char to_upper_char(u16char ch) noexcept
    {
        const u32char c = static_cast<u32char>(ch);
        const u32char r = to_upper_char(c);
        return static_cast<u16char>(r);
    }

    /**
     * 比较两个字符的大小(英文不区分大小写)
     */
    template<typename S,typename D>
    inline constexpr int compare_char_icase(S src,D dst) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<S>::type>::value && std::is_integral<typename std::remove_cv<D>::type>::value,
            "compare_char_icase requires integral/character types");
        return static_cast<int>(hgl::to_lower_char(src)) - static_cast<int>(hgl::to_lower_char(dst));
    }

    /**
    * 检测字符串是否符合代码命名规则（仅可使用字母和数字、下划线，不能使用数字开头）
    */
    template<typename T> inline constexpr bool is_valid_identifier(const T *str) noexcept
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
    * 检测字符串是否包含不可程序使用字符
    */
    template<typename T> inline constexpr bool contains_invalid_chars(const T *str) noexcept
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
    template<>
    inline constexpr bool contains_invalid_chars(const u16char *str) noexcept
    {
        if(!str) return false;

        constexpr const char inv[] = "<>,/\\|?%$#@`':\"*&!";

        while(*str)
        {
            if(hgl::is_space(*str))
                return true;

            for(const char *p=inv; *p; ++p)
            {
                if(static_cast<u16char>(*p) == *str)
                    return true;
            }

            ++str;
        }

        return false;
    }

    // u32char specialization
    template<>
    inline constexpr bool contains_invalid_chars(const u32char *str) noexcept
    {
        if(!str) return false;

        constexpr const char inv[] = "<>,/\\|?%$#@`':\"*&!";

        while(*str)
        {
            if(hgl::is_space(*str))
                return true;

            for(const char *p=inv; *p; ++p)
            {
                if(static_cast<u32char>(*p) == *str)
                    return true;
            }

            ++str;
        }

        return false;
    }

}//namespace hgl
