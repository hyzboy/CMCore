#include<hgl/Charset.h>

#include<windows.h>

namespace hgl
{
    namespace
    {
        enum class CharCodePage:uint16
        {
            NONE=0,
            IBM437=437,
            GBK=936,
            Big5=950,
            GB2312=20936,
            GB18030=54936,
            ShiftJIS=932,
            EUC_JP=20932,
            ISO2022JP=50220,
            csISO2022JP=50221,
            JISX=50222,
            Korean=949,
            MacJanpan=10001,
            MacTraditionalChinese=10002,
            MacSimplifiedChinese=10008,
            ISO_8859_1=28591,
            ISO_8859_2=28592,
            ISO_8859_3=28593,
            ISO_8859_4=28594,
            ISO_8859_5=28595,
            ISO_8859_6=28596,
            ISO_8859_7=28597,
            ISO_8859_8=28598,
            ISO_8859_9=28599,
            ISO_8859_13=28603,
            ISO_8859_15=28605,
            UTF7=65000,
            UTF8=65001,
            UTF16LE=1200,
            UTF16BE=1201,
            UTF32LE=12000,
            UTF32BE=12001,
            END
        };

        struct CodePageAndCharSet
        {
            uint16 codepage;
            const char *charset;
        };

    #define HGL_CODE_PAGE_AND_CHAR_SET(codepage,charset) {uint16(CharCodePage::codepage),charset}

        constexpr CodePageAndCharSet CodePage2CharSet[]=
        {
            HGL_CODE_PAGE_AND_CHAR_SET(NONE, "us-ascii"),
            HGL_CODE_PAGE_AND_CHAR_SET(IBM437, "IBM437"),
            HGL_CODE_PAGE_AND_CHAR_SET(GBK, "gbk"),
            HGL_CODE_PAGE_AND_CHAR_SET(Big5, "big5"),
            HGL_CODE_PAGE_AND_CHAR_SET(GB2312, "gb2312"),
            HGL_CODE_PAGE_AND_CHAR_SET(GB18030, "gb18030"),
            HGL_CODE_PAGE_AND_CHAR_SET(ShiftJIS, "shift-jis"),
            HGL_CODE_PAGE_AND_CHAR_SET(EUC_JP, "EUC-JP"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO2022JP, "iso-2022-jp"),
            HGL_CODE_PAGE_AND_CHAR_SET(csISO2022JP, "csISO2022JP"),
            HGL_CODE_PAGE_AND_CHAR_SET(JISX, "iso-2022-jp"),
            HGL_CODE_PAGE_AND_CHAR_SET(Korean, "ks_c_5601-1987"),
            HGL_CODE_PAGE_AND_CHAR_SET(MacJanpan, "x-mac-japanese"),
            HGL_CODE_PAGE_AND_CHAR_SET(MacTraditionalChinese, "x-mac-chinesetrad"),
            HGL_CODE_PAGE_AND_CHAR_SET(MacSimplifiedChinese, "x-mac-chinesesimp"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_1, "iso-8859-1"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_2, "iso-8859-2"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_3, "iso-8859-3"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_4, "iso-8859-4"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_5, "iso-8859-5"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_6, "iso-8859-6"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_7, "iso-8859-7"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_8, "iso-8859-8"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_9, "iso-8859-9"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_13, "iso-8859-13"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_15, "iso-8859-15"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF7, "utf-7"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF8, "utf-8"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE, "utf-16le"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE, "utf-16be"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF32LE, "utf-32le"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF32BE, "utf-32be"),
        };

        constexpr int CharSetCount=sizeof(CodePage2CharSet)/sizeof(CodePageAndCharSet);

        constexpr CodePageAndCharSet CodeSet2CharPage[]=
        {
            HGL_CODE_PAGE_AND_CHAR_SET(NONE, "us-ascii"),
            HGL_CODE_PAGE_AND_CHAR_SET(IBM437, "IBM437"),
            HGL_CODE_PAGE_AND_CHAR_SET(GBK, "gbk"),
            HGL_CODE_PAGE_AND_CHAR_SET(Big5, "big5"),
            HGL_CODE_PAGE_AND_CHAR_SET(Big5, "bigfive"),
            HGL_CODE_PAGE_AND_CHAR_SET(GB2312, "gb2312"),
            HGL_CODE_PAGE_AND_CHAR_SET(GB18030, "gb18030"),
            HGL_CODE_PAGE_AND_CHAR_SET(ShiftJIS, "shift_jis"),
            HGL_CODE_PAGE_AND_CHAR_SET(EUC_JP, "EUC-JP"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO2022JP, "iso-2022-jp"),
            HGL_CODE_PAGE_AND_CHAR_SET(csISO2022JP, "csISO2022JP"),
            HGL_CODE_PAGE_AND_CHAR_SET(JISX, "iso-2022-jp"),
            HGL_CODE_PAGE_AND_CHAR_SET(Korean, "ks_c_5601-1987"),
            HGL_CODE_PAGE_AND_CHAR_SET(MacJanpan, "x-mac-japanese"),
            HGL_CODE_PAGE_AND_CHAR_SET(MacTraditionalChinese, "x-mac-chinesetrad"),
            HGL_CODE_PAGE_AND_CHAR_SET(MacSimplifiedChinese, "x-mac-chinesesimp"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_1, "iso-8859-1"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_2, "iso-8859-2"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_3, "iso-8859-3"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_4, "iso-8859-4"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_5, "iso-8859-5"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_6, "iso-8859-6"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_7, "iso-8859-7"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_8, "iso-8859-8"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_9, "iso-8859-9"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_13, "iso-8859-13"),
            HGL_CODE_PAGE_AND_CHAR_SET(ISO_8859_15, "iso-8859-15"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF7, "utf-7"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF8, "utf-8"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE, "utf-16le"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE, "utf-16be"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF32LE, "utf-32le"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF32BE, "utf-32be"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE, "utf-16"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE, "unicodeFFFE"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16LE, "ucs-2le"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF16BE, "ucs-2be"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF32LE, "ucs-4le"),
            HGL_CODE_PAGE_AND_CHAR_SET(UTF32BE, "ucs-4be"),
        };

        constexpr int CharPageCount=sizeof(CodeSet2CharPage)/sizeof(CodePageAndCharSet);
    }//namespace

    CharSet UTF8CharSet     ((uint16)CharCodePage::UTF8);
    CharSet UTF16LECharSet  ((uint16)CharCodePage::UTF16LE);
    CharSet UTF16BECharSet  ((uint16)CharCodePage::UTF16BE);

    const char *FindCharSet(const uint16 ccp)
    {
        for(int i=0;i<CharSetCount;i++)
            if(CodePage2CharSet[i].codepage==ccp)
                return CodePage2CharSet[i].charset;
        return nullptr;
    }

    inline uint16 FindCodePage(const char *char_set)
    {
        for(int i=0;i<CharPageCount;i++)
            if(hgl::stricmp(CodeSet2CharPage[i].charset,(char *)char_set)==0)
                return CodeSet2CharPage[i].codepage;
        return (uint16)CharCodePage::NONE;
    }

    int get_utf16_length(const CharSet &cs,const void *src,const int src_size)
    {
        const int src_str_size=(src_size==-1)?hgl::strlen((const char *)src):src_size;
        const int len=MultiByteToWideChar((UINT)cs.codepage,0,(const char *)src,src_str_size,0,0);
        if(len<=0)return len;
        return len;
    }
    
    int to_utf16(const CharSet &cs,u16char **dst,const void *src,const int src_size)
    {
        const int src_str_size=(src_size==-1)?hgl::strlen((const char *)src):src_size;
        const int len=MultiByteToWideChar((UINT)cs.codepage,0,(const char *)src,src_str_size,0,0);
        if(len<=0)return len;
        *dst=new u16char[len+1];
        MultiByteToWideChar((UINT)cs.codepage,0,(const char *)src,src_str_size,*dst,len);
        (*dst)[len]=0;
        return len;
    }
    
    int to_utf16(const CharSet &cs,u16char *dst,const int dst_size,const void *src,const int src_size)
    {
        if(dst_size<=0)return dst_size;
        const int src_str_size=(src_size==-1)?hgl::strlen((const char *)src):src_size;
        int len=MultiByteToWideChar((UINT)cs.codepage,0,(const char *)src,src_str_size,dst,dst_size);
        if(len<dst_size) dst[len]=0;
        return len;
    }

    int to_utf8(const CharSet &cs,u8char **dst,const void *src,const int src_size)
    {
        u16char *u16str; int u16size=to_utf16(cs,&u16str,src,src_size); if(u16size<=0)return u16size; int u8_size; *dst = u16_to_u8(u16str, u16size, u8_size); delete[] u16str; return u8_size;
    }

    int get_ansi_length(const CharSet &cs,const u16char *src,const int src_size)
    {
        const int src_str_size=(src_size==-1)?hgl::strlen(src):src_size;
        const int len=WideCharToMultiByte((UINT)cs.codepage,0,src,src_str_size,0,0,0,0);
        if(len<=0)return len;
        return len;
    }

    int utf16_to(const CharSet &cs,char **dst,const u16char *src,const int src_size)
    {
        const int src_str_size=(src_size==-1)?hgl::strlen(src):src_size;
        const int len=WideCharToMultiByte((UINT)cs.codepage,0,src,src_str_size,0,0,0,0);
        if(len<=0)return len;
        *dst=new char[len+1];
        WideCharToMultiByte((UINT)cs.codepage,0,src,src_str_size,*dst,len,0,0);
        (*dst)[len]=0; return len;
    }

    int utf8_to(const CharSet &cs,char **dst,const u8char *src,const int src_size)
    {
        int u16str_size; u16char *u16str=u8_to_u16(src,src_size,u16str_size); if(!u16str)return 0; int result=utf16_to(cs,dst,u16str,u16str_size); delete[] u16str; return result; }
    
    AnsiString ToAnsiString(const CharSet &cs,const U16String &str)
    {
        const int len=WideCharToMultiByte((UINT)cs.codepage,0,str.c_str(),str.Length(),0,0,0,0);

        if(len<=0)
            return AnsiString();

        AnsiString new_string;

        char *ansi_str=new_string.Resize(len);

        WideCharToMultiByte((UINT)cs.codepage,0,str.c_str(),str.Length(),ansi_str,len,0,0);

        return new_string;
    }
}//namespace hgl


