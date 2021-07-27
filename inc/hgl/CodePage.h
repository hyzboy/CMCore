#ifndef HGL_CODE_PAGE_INCLUDE
#define HGL_CODE_PAGE_INCLUDE

#include<hgl/platform/Platform.h>
#include<hgl/type/String.h>
namespace hgl
{
    struct CodePageAndCharSet
    {
        CharCodePage codepage;
        CharSetName charset;
    };

    constexpr struct CodePageAndCharSet CodePage2CharSet[]=
    {
        {CharCodePage::NONE,           "us-ascii"  },

        {CharCodePage::GBK,            "gbk"       },
        {CharCodePage::Big5,           "big5"      },
        {CharCodePage::GB2312,         "gb2312"    },
        {CharCodePage::GB18030,        "gb18030"   },

        {CharCodePage::ShiftJIS,       "shift-jis"     },
        {CharCodePage::JISX,           "iso-2022-jp"   },

        {CharCodePage::Korean,         "ks_c_5601-1987"},

        {CharCodePage::MacJanpan,              "x-mac-japanese"    },
        {CharCodePage::MacTraditionalChinese,  "x-mac-chinesetrad" },
        {CharCodePage::MacSimplifiedChinese,   "x-mac-chinesesimp" },

        {CharCodePage::ISO_8859_1,         "iso-8859-1"},
        {CharCodePage::ISO_8859_2,         "iso-8859-2"},
        {CharCodePage::ISO_8859_3,         "iso-8859-3"},
        {CharCodePage::ISO_8859_4,         "iso-8859-4"},
        {CharCodePage::ISO_8859_5,         "iso-8859-5"},
        {CharCodePage::ISO_8859_6,         "iso-8859-6"},
        {CharCodePage::ISO_8859_7,         "iso-8859-7"},
        {CharCodePage::ISO_8859_8,         "iso-8859-8"},
        {CharCodePage::ISO_8859_9,         "iso-8859-9"},
        {CharCodePage::ISO_8859_13,        "iso-8859-13"},
        {CharCodePage::ISO_8859_15,        "iso-8859-15"},

        {CharCodePage::UTF7,           "utf-7"     },
        {CharCodePage::UTF8,           "utf-8"     },

        {CharCodePage::UTF16LE,        "utf-16le"  },
        {CharCodePage::UTF16BE,        "utf-16be"  },
        {CharCodePage::UTF32LE,        "utf-32le"  },
        {CharCodePage::UTF32BE,        "utf-32be"  },
    };//const struct

    constexpr int CharSetCount=sizeof(CodePage2CharSet)/sizeof(CodePageAndCharSet);

    inline const char *FindCharSet(CharCodePage ccp)
    {
        for(int i=0;i<CharSetCount;i++)
            if(CodePage2CharSet[i].codepage==ccp)
                return CodePage2CharSet[i].charset;

        return 0;
    }

    constexpr struct CodePageAndCharSet CodeSet2CharPage[]=
    {
        {CharCodePage::NONE,           "us-ascii"  },

        {CharCodePage::GBK,            "gbk"       },

        {CharCodePage::Big5,           "big5"      },
        {CharCodePage::Big5,           "bigfive"   },

        {CharCodePage::GB2312,         "gb2312"    },
        {CharCodePage::GB18030,        "gb18030"   },

        {CharCodePage::ShiftJIS,       "shift_jis"     },
        {CharCodePage::JISX,           "iso-2022-jp"   },

        {CharCodePage::Korean,         "ks_c_5601-1987"},

        {CharCodePage::MacJanpan,              "x-mac-japanese"    },
        {CharCodePage::MacTraditionalChinese,  "x-mac-chinesetrad" },
        {CharCodePage::MacSimplifiedChinese,   "x-mac-chinesesimp" },
     
        {CharCodePage::ISO_8859_1,         "iso-8859-1"},
        {CharCodePage::ISO_8859_2,         "iso-8859-2"},
        {CharCodePage::ISO_8859_3,         "iso-8859-3"},
        {CharCodePage::ISO_8859_4,         "iso-8859-4"},
        {CharCodePage::ISO_8859_5,         "iso-8859-5"},
        {CharCodePage::ISO_8859_6,         "iso-8859-6"},
        {CharCodePage::ISO_8859_7,         "iso-8859-7"},
        {CharCodePage::ISO_8859_8,         "iso-8859-8"},
        {CharCodePage::ISO_8859_9,         "iso-8859-9"},
        {CharCodePage::ISO_8859_13,        "iso-8859-13"},
        {CharCodePage::ISO_8859_15,        "iso-8859-15"},

        {CharCodePage::UTF7,           "utf-7"      },
        {CharCodePage::UTF8,           "utf-8"      },
        {CharCodePage::UTF16LE,        "utf-16le"   },
        {CharCodePage::UTF16BE,        "utf-16be"   },
        {CharCodePage::UTF32LE,        "utf-32le"   },
        {CharCodePage::UTF32BE,        "utf-32be"   },

        {CharCodePage::UTF16LE,        "utf-16"     },
        {CharCodePage::UTF16BE,        "unicodeFFFE"},

        {CharCodePage::UTF16LE,        "ucs-2le"    },
        {CharCodePage::UTF16BE,        "ucs-2be"    },
        {CharCodePage::UTF32LE,        "ucs-4le"    },
        {CharCodePage::UTF32BE,        "ucs-4be"    }
    };//const struct CharSet Characters

    constexpr int CharPageCount=sizeof(CodeSet2CharPage)/sizeof(CodePageAndCharSet);

    inline CharCodePage FindCodePage(const u8char *char_set)
    {
        for(int i=0;i<CharPageCount;i++)
            if(!charset_cmp(CodePage2CharSet[i].charset,char_set))
                return CodePage2CharSet[i].codepage;

        return CharCodePage::NONE;
    }

    struct CharSet
    {
        CharCodePage codepage;
        CharSetName charset;

    public:

        CharSet()
        {
            codepage=CharCodePage::NONE;
            hgl::strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,"us-ascii");
        }

        CharSet(CharCodePage ccp,const char *cs)
        {
            codepage=ccp;
            hgl::strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,cs);
        }

        CharSet(CharCodePage);
        CharSet(const u8char *);

        CharSet(const CodePageAndCharSet &cs)
        {
            codepage=cs.codepage;
            strcpy(charset,CHAR_SET_NAME_MAX_LENGTH,cs.charset);
        }

        int _Comp(const CharSet &data)const{return (size_t)codepage-(size_t)data.codepage;} \
        CompOperator(const CharSet &,_Comp)
    };//struct CharacterSet

    inline CharSet::CharSet(CharCodePage ccp)
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
     * 使用指定字符集转换字符串到 u16char *字符串
     * @param charset   字符集
     * @param dst       u16char *字符串缓冲区，自动分配，需手动delete[]
     * @param src       原始字符串
     * @param src_size  字符串长度,-1表示全部
     * @return 转换成功后的字符串字符数
     */
    int to_utf16(const CharSet &charset,u16char **dst,const void *src,const int src_size=-1);

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

    int             u16_to_u8(u8char *,int,const u16char *,const int=-1);                           ///<转换u16char *到utf8格式的u8char *
    int             u8_to_u16(u16char *,int,const u8char *,const int=-1);                           ///<转换utf8格式的u8char *到u16char *

    u8char *        u16_to_u8(const u16char *,const int,int &);                                     ///<转换u16char *到utf8格式的u8char *
    u16char *       u8_to_u16(const u8char *,const int,int &);                                      ///<转换utf8格式的u8char *到u16char *

    inline u8char * u16_to_u8(const u16char *str)
    {
        int len;
        return u16_to_u8(str,hgl::strlen<u16char>(str)+1,len);
    }

    inline u16char *u8_to_u16(const u8char *str)
    {
        int len;
        return u8_to_u16(str,hgl::strlen<u8char>(str)+1,len);
    }

    inline UTF16String to_u16(const u8char *u8_str,int length)
    {
        int wlen;
        u16char *ws=u8_to_u16(u8_str,length,wlen);

        return UTF16String::newOf(ws,wlen);
    }

    inline UTF16String to_u16(const UTF8String &u8str)
    {
        return to_u16(u8str.c_str(),u8str.Length());
    }

    inline UTF16String to_u16(const u8char *str)
    {
        int wlen;

        u16char *ws=u8_to_u16(str,hgl::strlen(str),wlen);
        
        return UTF16String::newOf(ws,wlen);
    }

    inline UTF8String to_u8(const u16char *wide_str,int length)
    {
        int ulen;

        u8char *us=u16_to_u8(wide_str,length,ulen);

        return UTF8String::newOf(us,ulen);
    }

    inline UTF8String to_u8(const UTF16String &ws)
    {
        return to_u8(ws.c_str(),ws.Length());
    }

#if HGL_OS == HGL_OS_Windows
    inline OSString ToOSString(const u8char *str){return to_u16(str);}
    inline OSString ToOSString(const UTF8String &str){return to_u16(str.c_str(), (int)(str.Length()));}

    inline UTF8String ToUTF8String(const os_char *str){return to_u8(str,strlen(str));}
    inline UTF8String ToUTF8String(const OSString &str){return to_u8(str);}
#else
    inline OSString ToOSString(const char *str){return OSString(str);}
    inline OSString ToOSString(const UTF8String &str){return str;}

    inline UTF8String ToUTF8String(const os_char *str){return UTF8String(str);}
    inline UTF8String ToUTF8String(const OSString &str){return str;}
#endif//

    const BOMFileHeader *ParseBOM(const void *input);

    bool BOM2CharSet(CharSet *cs,const BOMFileHeader *bom);
}//namespace hgl
#endif//HGL_CODE_PAGE_INCLUDE
