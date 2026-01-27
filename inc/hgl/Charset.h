#pragma once

#include<hgl/utf.h>
namespace hgl
{
#if HGL_OS == HGL_OS_Windows
    const char *FindCharSet(const uint16 ccp);
    uint16 FindCodePage(const char *char_set);
#endif//HGL_OS == HGL_OS_Windows

    struct CharSet
    {
    #if HGL_OS == HGL_OS_Windows
        uint16 codepage;
    #endif//HGL_OS == HGL_OS_Windows

        CharSetName charset;

    public:

        CharSet()
        {
        #if HGL_OS == HGL_OS_Windows
            codepage=0;
        #endif//HGL_OS == HGL_OS_Windows

            mem_zero(charset);
        }

    #if HGL_OS == HGL_OS_Windows
        CharSet(const uint16 cp)
        {
            codepage=cp;
            hgl::strcpy(charset,sizeof(CharSetName),FindCharSet(cp));
        }
    #endif//

        CharSet(const CharSetName &cs)
        {
            mem_copy(charset,cs);
        #if HGL_OS == HGL_OS_Windows
            codepage=FindCodePage(charset);
        #endif//HGL_OS == HGL_OS_Windows
        }

        std::strong_ordering operator<=>(const CharSet &other)const
        {
        #if HGL_OS == HGL_OS_Windows
            return codepage <=> other.codepage;
        #else
            return (hgl::strcmp(charset, other.charset) <=> 0);
        #endif//
        }

        bool operator==(const CharSet &other)const
        {
        #if HGL_OS == HGL_OS_Windows
            return codepage == other.codepage;
        #else
            return hgl::strcmp(charset, other.charset) == 0;
        #endif//
        }
    };//struct CharacterSet

    extern CharSet DefaultCharSet();

    extern CharSet UTF8CharSet;
    extern CharSet UTF16LECharSet;
    extern CharSet UTF16BECharSet;

    #if HGL_OS == HGL_OS_Windows
        #define OSCharSet   UTF16LECharSet
    #else
        #define OSCharSet   UTF8CharSet
    #endif//

    /**
     * 获取字符串转换到utf16字符串后的长度
     * @param charset   字符集
     * @param src       原始字符串
     * @param src_size  字符串长度,-1表示全部
     * @return 转换成功后的字符串字符数
     */
    int get_utf16_length(const CharSet &charset,const void *src,const int src_size=-1);

    /**
     * 使用指定字符集转换字符串到utf16字符串
     * @param charset   字符集
     * @param dst       utf16字符串缓冲区，自动分配，需手动delete[]
     * @param src       原始字符串
     * @param src_size  字符串长度,-1表示全部
     * @return 转换成功后的字符串字符数
     */
    int to_utf16(const CharSet &charset,u16char **dst,const void *src,const int src_size=-1);

    /**
     * 使用指定字符集转换字符串到utf16字符串
     * @param charset   字符集
     * @param dst       utf16字符串缓冲区
     * @param dst_size  最大可写入长度
     * @param src       原始字符串
     * @param src_size  字符串长度,-1表示全部
     * @return 转换成功后的字符串字符数
     */
    int to_utf16(const CharSet &charset,u16char *dst,const int dst_size,const void *src,const int src_size=-1);

    int to_utf8(const CharSet &charset,u8char **dst,const void *src,const int src_size=-1);

    /**
     * 转换u16char *字符串到指定字符集的字符串
     * @param charset   字符集
     * @param dst       char *字符串缓冲区，自动分配，需手动delete[]
     * @param src       u16char *字符串
     * @param src_size  字符串长度,-1表示全部
     * @return 转换成功后的字符串字符数
     */
    int utf16_to(const CharSet &charset,u8char **dst,const u16char *src,const int src_size=-1);

    int utf8_to(const CharSet &charset,u8char **dst,const u8char *src,const int src_size=-1);

#if HGL_OS == HGL_OS_Windows

    int get_ansi_length(const CharSet &cs,const u16char *src,const int src_size);

    AnsiString ToAnsiString(const CharSet &cs,const U16String &str);
#endif//

    bool BOM2CharSet(CharSet *cs,const BOMFileHeader *bom);
}//namespace hgl
