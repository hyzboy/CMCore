#include<hgl/Charset.h>

namespace hgl
{
    CharSet DefaultCharSet();

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

        memcpy(cs->charset,bom->char_set,sizeof(CharSetName));
        return(true);
    }
}//namespace hgl
