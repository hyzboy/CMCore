#pragma once

/**
 * CN:  在这里我们自行实现了C标准库的一些字符和字符串处理函数，
 *      这个行为并不是为了追求性能，或是在炫技。而是因为不同编译器所附带的标准库在一些边界、非法值上处理不一致。
 *
 *      比如你在一些字符串处理函数中传入非法长度或是nullptr，不同实现的标准库都有正确的错误处理，但错误的处理方式确不一定完全一样。
 *
 *      EN:  Here we have implemented some character and string handling functions from the C standard library ourselves.
 *      This is not done for performance or to show off skills,
 *      but because the standard libraries provided by different compilers handle certain edge cases and illegal values inconsistently.
 *
 *      For example,
 *          if you pass an illegal length or nullptr to some string handling functions,
 *          different implementations of the standard library may have correct error handling,
 *          but the way they handle errors is not necessarily the same.
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
    inline constexpr bool isemoji(T ch) noexcept
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
    inline constexpr bool isemoji(u32char ch) noexcept
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
    inline constexpr bool islower(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "islower requires integral/character type");
        return(ch>='a'&&ch<='z');
    }

    /**
     * 测试当前字符是否为大写字母
     */
    template<typename T>
    inline constexpr bool isupper(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isupper requires integral/character type");
        return(ch>='A'&&ch<='Z');
    }

    /**
     * 测试当前字符是否为字母
     */
    template<typename T>
    inline constexpr bool isalpha(T ch) noexcept
    {
        return(hgl::islower(ch)||hgl::isupper(ch));
    }

    /**
     * 测试当前字符是否为10进制数字
     */
    template<typename T>
    inline constexpr bool isdigit(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isdigit requires integral/character type");
        return(ch>='0'&&ch<='9');
    }

    /**
     * 测试当前字符串是否为10进制数字以及小数点、正负符号、指数字符
     */
    template<typename T>
    inline constexpr bool isfloat(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isfloat requires integral/character type");
        return hgl::isdigit(ch)
        || ch == '-' 
        || ch == '+' 
        || ch == '.' 
        || ch == 'E' 
        || ch == 'e' 
        || ch == 'f' 
        || ch == 'F';
    }

    template<typename T>
    inline constexpr bool isinteger(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isinteger requires integral/character type");
        return hgl::isdigit(ch)
        || ch == '-' 
        || ch == '+';
    }

    /**
     * 测试当前字符是否为16进制数用字符(0-9,A-F)
     */
    template<typename T>
    inline constexpr bool isxdigit(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isxdigit requires integral/character type");
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
    inline constexpr bool isxdigits(const T *str,int length) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isxdigits requires integral/character type");
        if(!str||length<=0)
            return(false);

        while(*str&&length)
        {
            if(!hgl::isxdigit(*str))
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
    inline constexpr bool isslash(const T &ch)
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isslash requires integral/character type");
        return (ch == '\\') || (ch == '/');
    }

    template<typename T> inline constexpr bool isspace(T);

    /**
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline constexpr bool isspace(u32char ch) noexcept
    {
        return(ch==0
        ||ch==U' '              //半角空格
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
    inline constexpr bool isspace(u16char ch) noexcept
    {
        return(ch==0
        ||ch==U16_TEXT(' ')    //半角空格
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
    inline constexpr bool isspace(char ch) noexcept
    {
        return(ch==' '              //半角空格
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
    inline constexpr bool isspace(char8_t ch) noexcept
    {
        return(ch==u8' '              //半角空格
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
    inline constexpr bool isalnum(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isalnum requires integral/character type");
        return(hgl::isalpha(ch)||hgl::isdigit(ch));
    }

    /**
     * 测试当前字符是否为代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline constexpr bool iscodechar(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "iscodechar requires integral/character type");
        return(hgl::isalnum(ch)||ch=='_');
    }

    /**
     * 测试当前字符是否不是代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline constexpr bool notcodechar(T ch) noexcept
    {
        return(!hgl::iscodechar(ch));
    }

    /**
     * 测试当前字符是否为文件名可用字符(不含路径分隔符)
     */
    template<typename T>
    inline constexpr bool isfilenamechar(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isfilenamechar requires integral/character type");
        return(ch=='.'||hgl::iscodechar(ch));
    }

    /**
     * 测试当前字符是否不是文件名可用字符
     */
    template<typename T>
    inline constexpr bool notfilenamechar(T ch) noexcept
    {
        return(!hgl::isfilenamechar(ch));
    }

    /**
     * 测试当前字符是否为BASE64编码字符
     */
    template<typename T>
    inline constexpr bool isbase64(T c) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "isbase64 requires integral/character type");
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
    inline constexpr T tolower(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "tolower requires integral/character type");
        if(ch>='A'&&ch<='Z')
            return ch+('a'-'A');
        else
            return ch;
    }

    /**
     * 如果当前字符为小写英文字符，则转换为大写
     */
    template<typename T>
    inline constexpr T toupper(T ch) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "toupper requires integral/character type");
        if(ch>='a'&&ch<='z')
            return ch+('A'-'a');
        else
            return ch;
    }

    /**
     * 比较两个字符的大小(英文不区分大小写)
     */
    template<typename S,typename D>
    inline constexpr int chricmp(S src,D dst) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<S>::type>::value && std::is_integral<typename std::remove_cv<D>::type>::value,
            "chricmp requires integral/character types");
        return static_cast<int>(hgl::tolower(src)) - static_cast<int>(hgl::tolower(dst));
    }

    /**
    * 检测字符串是否符合代码命名规则（仅可使用字母和数字、下划线，不能使用数字开头）
    */
    template<typename T> inline constexpr bool check_codestr(const T *str) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "check_codestr requires integral/character type");

        if(!str)return(false);

        if((!hgl::isalpha(*str))&&(*str!='_'))       //不是字母或下划线
            return(false);

        // allow single-character identifiers; validate remaining characters if any
        ++str;

        while(*str)
            if(!hgl::iscodechar(*str++))
                return(false);

        return(true);
    }

    /**
    * 检测字符串是否包含不可程序使用字符
    */
    template<typename T> inline constexpr bool check_error_char(const T *str) noexcept
    {
        static_assert(std::is_integral<typename std::remove_cv<T>::type>::value, "check_error_char requires integral/character type");

        if(!str)return(false);

        // list of characters considered invalid for identifiers/filenames (space handled by isspace())
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
            static_cast<T>('\''),
            static_cast<T>(':'),
            static_cast<T>('"'),
            static_cast<T>('*'),
            static_cast<T>('&'),
            static_cast<T>('!'),
            static_cast<T>(0)
        };

        const T *sp;

        while(*str)
        {
            if(hgl::isspace(*str))
                return(false);

            sp=err_chr;

            while(*sp)
            {
                if(*str == *sp)
                    return(false);

                ++sp;
            }

            ++str;
        }

        return(true);
    }
}//namespace hgl
