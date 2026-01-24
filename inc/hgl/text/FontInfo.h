#pragma once

#include<hgl/type/Map.h>

namespace hgl
{
    enum class FontType
    {
        Raster,
        Vector,
        TrueType
    };

    constexpr const size_t HGL_FONT_NAME_MAX_LENGTH=128;

    struct FontInfo
    {
        os_char name[HGL_FONT_NAME_MAX_LENGTH];
        FontType type;

        uint charset;

        //尺寸仅用于Raster字体
        uint height;

        uint ascent;                ///<上升
        uint descent;               ///<下降
        uint internal_leading;      ///<内部行距
        uint external_leading;      ///<外部行距
        uint ave_char_width;        ///<平均字宽
        uint max_char_width;        ///<最大字宽
        uint weight;                ///<权重
        uint overhang;              ///<超出部分
        uint digitized_aspect_x;    ///<横向分辨率
        uint digitized_aspect_y;    ///<纵向分辨率
        os_char first_char;
        os_char last_char;
        os_char default_char;
        os_char break_char;
        bool italic;                ///<斜体
        bool underlined;            ///<下划线
        bool struck_out;            ///<删除线
        uint pitch_and_family;      ///<字体类型

    public:

        FontInfo()
        {
            mem_zero(*this);
        }
    };

    using FontInfoList=ValueArray<FontInfo>;

    void EnumOSFonts(FontInfoList *);
}//namespace hgl
