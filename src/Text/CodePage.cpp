#include<hgl/CodePage.h>

namespace hgl
{
    CharSet DefaultCharSet();

    CharSet UTF8CharSet     ((uint16)CharCodePage::UTF8,   utf8_charset    );
    CharSet UTF16LECharSet  ((uint16)CharCodePage::UTF16LE,utf16le_charset );
    CharSet UTF16BECharSet  ((uint16)CharCodePage::UTF16BE,utf16be_charset );

    /**
     * 转换BOM数据到CharSet结构
     * @param bom BOM数据
     * @param cs 字符集数据结构
     * @return 是否转换成功
     */
    bool BOM2CharSet(CharSet *cs,const BOMFileHeader *bom)
    {
        if(!cs)return(false);
        if(!bom)return(false);

        RANGE_CHECK_RETURN_FALSE(bom->bom)

        cs->codepage=bom->code_page;
        memcpy(cs->charset,bom->char_set,sizeof(CharSetName));
        return(true);
    }
}//namespace hgl
