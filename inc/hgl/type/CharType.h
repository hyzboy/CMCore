#pragma once

/**
 * CN:  在这里我们自行实现了C标准库的一些字符和字符串处理函数，
 *      这个行为并不是为了追求性能，或是在炫技。而是因为不同编译器所附带的标准库在一些边界、非法值上处理不一致。
 *
 *      比如你在一些字符串处理函数中传入非法长度或是nullptr，不同实现的标准库都有正确的错误处理，但错误的处理方式确不一定完全一样。
 *
 * EN:  Here we have implemented some character and string handling functions from the C standard library ourselves.
 *      This is not done for performance or to show off skills,
 *      but because the standard libraries provided by different compilers handle certain edge cases and illegal values inconsistently.
 *
 *      For example,
 *          if you pass an illegal length or nullptr to some string handling functions,
 *          different implementations of the standard library may have correct error handling,
 *          but the way they handle errors is not necessarily the same.
 */

#include <hgl/TypeFunc.h>

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
     */
    template<typename T>
    inline bool isemoji(T ch)
    {
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
    inline bool isemoji(u32char ch)
    {
        if(ch==0x23) return true;           //#
        if(ch==0x2A) return true;           //*
        if(ch>=0x30 && ch<=0x39) return true; //0-9
        if(ch==0xA9) return true;           //©
        if(ch==0xAE) return true;           //®
        if(ch>=0x203C && ch<=0x1FFFD) return true;

        return false;
    }

    /**
     * 测试当前字符是否为小写字母
     */
    template<typename T>
    inline bool islower(T ch)
    {
        return(ch>='a'&&ch<='z');
    }

    /**
     * 测试当前字符是否为大写字母
     */
    template<typename T>
    inline bool isupper(T ch)
    {
        return(ch>='A'&&ch<='Z');
    }

    /**
     * 测试当前字符是否为字母
     */
    template<typename T>
    inline bool isalpha(T ch)
    {
        return(hgl::islower(ch)||hgl::isupper(ch));
    }

    /**
     * 测试当前字符是否为10进制数字
     */
    template<typename T>
    inline bool isdigit(T ch)
    {
        return(ch>='0'&&ch<='9');
    }

    /**
     * 测试当前字符串是否为10进制数字以及小数点、正负符号、指数字符
     */
    template<typename T>
    inline bool isfloat(T ch)
    {
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
    inline bool isinteger(T ch)
    {
        return hgl::isdigit(ch)
        || ch == '-' 
        || ch == '+';
    }

    /**
     * 测试当前字符是否为16进制数用字符(0-9,A-F)
     */
    template<typename T>
    inline bool isxdigit(T ch)
    {
        // use character literals for readability
        return (
            (ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F') ||
            ch == 'x' || ch == 'X' // allow '0x' style prefix characters if desired
        );
    }

    /**
     * 测试当前字符串是否为16进制数用字符
     * @param str 字符串
     * @param length 字符串长度
     */
    template<typename T>
    inline bool isxdigits(const T *str,int length)
    {
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
    inline bool isslash(T ch)
    {
        return (ch == '\\') || (ch == '/');
    }

    template<typename T> inline bool isspace(T);

    /**
     * 是否为不显示可打印字符(' ','\t','\r','\f','\v','\n')
     */
    template<>
    inline bool isspace(u32char ch)
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
    inline bool isspace(u16char ch)
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
    inline bool isspace(char ch)
    {
        return(ch==0
        ||ch==' '              //半角空格
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
    inline bool isspace(char8_t ch)
    {
        return(ch==0
        ||ch==u8' '              //半角空格
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
    inline bool isalnum(T ch)
    {
        return(hgl::isalpha(ch)||hgl::isdigit(ch));
    }

    /**
     * 测试当前字符是否为代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline bool iscodechar(T ch)
    {
        return(hgl::isalnum(ch)||ch=='_');
    }

    /**
     * 测试当前字符是否不是代码可用字符(仅字母，数字，下划线，常用于文件名之类)
     */
    template<typename T>
    inline bool notcodechar(T ch)
    {
        return(!hgl::iscodechar(ch));
    }

    /**
     * 测试当前字符是否为文件名可用字符(不含路径分隔符)
     */
    template<typename T>
    inline bool isfilenamechar(T ch)
    {
        return(ch=='.'||hgl::iscodechar(ch));
    }

    /**
     * 测试当前字符是否不是文件名可用字符
     */
    template<typename T>
    inline bool notfilenamechar(T ch)
    {
        return(!hgl::isfilenamechar(ch));
    }

    /**
     * 测试当前字符是否为BASE64编码字符
     */
    template<typename T>
    inline bool isbase64(T c)
    {
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
    inline T tolower(T ch)
    {
        if(ch>='A'&&ch<='Z')
            return ch+('a'-'A');
        else
            return ch;
    }

    /**
     * 如果当前字符为小写英文字符，则转换为大写
     */
    template<typename T>
    inline T toupper(T ch)
    {
        if(ch>='a'&&ch<='z')
            return ch+('A'-'a');
        else
            return ch;
    }

    /**
     * 比较两个字符的大小(英文不区分大小写)
     */
    template<typename S,typename D>
    inline int chricmp(S src,D dst)
    {
        return static_cast<int>(hgl::tolower(src)) - static_cast<int>(hgl::tolower(dst));
    }

    /**
    * 检测字符串是否符合代码命名规则（仅可使用字母和数字、下划线，不能使用数字开头）
    */
    template<typename T> inline bool check_codestr(const T *str)
    {
        if(!str)return(false);

        if((!hgl::isalpha(*str))&&(*str!='_'))       //不是字母或下划线
            return(false);

        ++str;

        if(!(*str))                             //不能仅一个字符
            return(false);

        while(*str)
            if(!hgl::iscodechar(*str++))
                return(false);

        return(true);
    }

    /**
    * 检测字符串是否包含不可程序使用字符
    */
    template<typename T> inline bool check_error_char(const T *str)
    {
        if(!str)return(false);

        // list of characters considered invalid for identifiers/filenames
        constexpr char err_chr[] = { ' ',
            '<',
            '>',
            ',',
            '/',
            '\\',
            '|',
            '?',
            '%',
            '$',
            '#',
            '@',
            '`',
            '\'',
            ':',
            '"',
            '*',
            '&',
            '!',
            0};
        const char *sp;

        while(*str)
        {
            if(hgl::isspace(*str))
                return(false);

            sp=err_chr;

            while(*sp)
            {
                if(*str == static_cast<T>(*sp))
                    return(false);

                ++sp;
            }

            ++str;
        }

        return(true);
    }
}//namespace hgl
