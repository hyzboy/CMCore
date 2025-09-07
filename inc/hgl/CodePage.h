#ifndef HGL_CODE_PAGE_INCLUDE
#define HGL_CODE_PAGE_INCLUDE

#include<hgl/utf.h>
namespace hgl
{
    struct CodePageAndCharSet
    {
        uint16 codepage;
        CharSetName charset;
    };
    
    #define HGL_CODE_PAGE_AND_CHAR_SET(codepage,charset) {uint16(CharCodePage::codepage),charset}
    
    constexpr struct CodePageAndCharSet CodePage2CharSet[]=
    {

        HGL_CODE_PAGE_AND_CHAR_SET(NONE,                    "us-ascii"  ),
        HGL_CODE_PAGE_AND_CHAR_SET(IBM437,                  "IBM437"    ),

        HGL_CODE_PAGE_AND_CHAR_SET(GBK,                     "gbk"       ),
        HGL_CODE_PAGE_AND_CHAR_SET(Big5,                    "big5"      ),
        HGL_CODE_PAGE_AND_CHAR_SET(GB2312,                  "gb2312"    ),
        HGL_CODE_PAGE_AND_CHAR_SET(GB18030,                 "gb18030"   ),
        
        HGL_CODE_PAGE_AND_CHAR_SET(ShiftJIS,                "shift-jis"     ),
        HGL_CODE_PAGE_AND_CHAR_SET(EUC_JP,                  "EUC-JP"        ),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO2022JP,               "iso-2022-jp"   ),
        HGL_CODE_PAGE_AND_CHAR_SET(csISO2022JP,             "csISO2022JP"   ),
        HGL_CODE_PAGE_AND_CHAR_SET(JISX,                    "iso-2022-jp"   ),

        HGL_CODE_PAGE_AND_CHAR_SET(Korean,                  "ks_c_5601-1987"),

        HGL_CODE_PAGE_AND_CHAR_SET(MacJanpan,               "x-mac-japanese"    ),
        HGL_CODE_PAGE_AND_CHAR_SET(MacTraditionalChinese,   "x-mac-chinesetrad" ),
        HGL_CODE_PAGE_AND_CHAR_SET(MacSimplifiedChinese,    "x-mac-chinesesimp" ),

        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_1,         "iso-8859-1"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_2,         "iso-8859-2"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_3,         "iso-8859-3"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_4,         "iso-8859-4"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_5,         "iso-8859-5"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_6,         "iso-8859-6"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_7,         "iso-8859-7"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_8,         "iso-8859-8"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_9,         "iso-8859-9"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_13,        "iso-8859-13"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_15,        "iso-8859-15"),

        HGL_CODE_PAGE_AND_CHAR_SET(UTF7,           "utf-7"     ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF8,           "utf-8"     ),

        HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE,        "utf-16le"  ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE,        "utf-16be"  ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF32LE,        "utf-32le"  ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF32BE,        "utf-32be"  ),
    };//const struct

    constexpr int CharSetCount=sizeof(CodePage2CharSet)/sizeof(CodePageAndCharSet);

    inline const char *FindCharSet(uint16 ccp)
    {
        for(int i=0;i<CharSetCount;i++)
            if(CodePage2CharSet[i].codepage==ccp)
                return CodePage2CharSet[i].charset;

        return 0;
    }

    constexpr struct CodePageAndCharSet CodeSet2CharPage[]=
    {
        HGL_CODE_PAGE_AND_CHAR_SET(NONE,                    "us-ascii"  ),
        HGL_CODE_PAGE_AND_CHAR_SET(IBM437,                  "IBM437"    ),
        
        HGL_CODE_PAGE_AND_CHAR_SET(GBK,                     "gbk"       ),
        
        HGL_CODE_PAGE_AND_CHAR_SET(Big5,                    "big5"      ),
        HGL_CODE_PAGE_AND_CHAR_SET(Big5,                    "bigfive"   ),

        HGL_CODE_PAGE_AND_CHAR_SET(GB2312,                  "gb2312"    ),
        HGL_CODE_PAGE_AND_CHAR_SET(GB18030,                 "gb18030"   ),
        
        HGL_CODE_PAGE_AND_CHAR_SET(ShiftJIS,                "shift_jis"     ),
        HGL_CODE_PAGE_AND_CHAR_SET(EUC_JP,                  "EUC-JP"        ),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO2022JP,               "iso-2022-jp"   ),
        HGL_CODE_PAGE_AND_CHAR_SET(csISO2022JP,             "csISO2022JP"   ),
        HGL_CODE_PAGE_AND_CHAR_SET(JISX,                    "iso-2022-jp"   ),

        HGL_CODE_PAGE_AND_CHAR_SET(Korean,                  "ks_c_5601-1987"),

        HGL_CODE_PAGE_AND_CHAR_SET(MacJanpan,               "x-mac-japanese"    ),
        HGL_CODE_PAGE_AND_CHAR_SET(MacTraditionalChinese,   "x-mac-chinesetrad" ),
        HGL_CODE_PAGE_AND_CHAR_SET(MacSimplifiedChinese,    "x-mac-chinesesimp" ),
     
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_1,         "iso-8859-1"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_2,         "iso-8859-2"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_3,         "iso-8859-3"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_4,         "iso-8859-4"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_5,         "iso-8859-5"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_6,         "iso-8859-6"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_7,         "iso-8859-7"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_8,         "iso-8859-8"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_9,         "iso-8859-9"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_13,        "iso-8859-13"),
        HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_15,        "iso-8859-15"),

        HGL_CODE_PAGE_AND_CHAR_SET(UTF7,           "utf-7"      ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF8,           "utf-8"      ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE,        "utf-16le"   ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE,        "utf-16be"   ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF32LE,        "utf-32le"   ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF32BE,        "utf-32be"   ),

        HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE,        "utf-16"     ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE,        "unicodeFFFE"),

        HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE,        "ucs-2le"    ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE,        "ucs-2be"    ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF32LE,        "ucs-4le"    ),
        HGL_CODE_PAGE_AND_CHAR_SET(UTF32BE,        "ucs-4be"    )
    };//const struct CharSet Characters

    constexpr int CharPageCount=sizeof(CodeSet2CharPage)/sizeof(CodePageAndCharSet);

    inline uint16 FindCodePage(const u8char *char_set)
    {
        for(int i=0;i<CharPageCount;i++)
            if(!charset_cmp(CodePage2CharSet[i].charset,char_set))
                return CodePage2CharSet[i].codepage;

        return (uint16)CharCodePage::NONE;
    }

    struct CharSet:public Comparator<CharSet>
    {
        uint16 codepage;
        CharSetName charset;

    public:

        CharSet()
        {
            codepage=0;
            hgl::strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,"us-ascii");
        }

        CharSet(const uint16 ccp,const char *cs)
        {
            codepage=ccp;
            hgl::strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,cs);
        }

        CharSet(const uint16);
        CharSet(const u8char *);

        CharSet(const CodePageAndCharSet &cs)
        {
            codepage=cs.codepage;
            strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,cs.charset);
        }

        const int compare(const CharSet &other)const override
        {
            return (size_t)codepage-(size_t)other.codepage;
        }
    };//struct CharacterSet

    inline CharSet::CharSet(const uint16 ccp)
    {
        codepage=ccp;
        hgl::strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,FindCharSet(ccp));
    }

    inline CharSet::CharSet(const u8char *cs)
    {
        codepage=FindCodePage(cs);
        hgl::strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,FindCharSet(codepage));
    }

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
           AnsiString ToAnsiString(const CharSet &cs,const U16String &str);
#endif//
    
    bool BOM2CharSet(CharSet *cs,const BOMFileHeader *bom);
}//namespace hgl
#endif//HGL_CODE_PAGE_INCLUDE
