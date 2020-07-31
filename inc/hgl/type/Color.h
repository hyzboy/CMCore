#ifndef HGL_COLOR_TABLE_INCLUDE
#define HGL_COLOR_TABLE_INCLUDE

#include<hgl/platform/Platform.h>
namespace hgl
{
    /**
    * 颜色数据定义
    */
    struct COLOR_DEF            ///颜色数据定义
    {
        int red,green,blue;     ///<三原色
        int yum;                ///<亮度

        float r,g,b;            ///<三原色浮点值
        float y;                ///<亮度浮点值

        char eng_name[32];      ///<英文名称
        u16char chs_name[16];   ///<中文名称
    };

    /**
    * 颜色枚举
    */
    enum class COLOR          ///颜色枚举
    {
        AliceBlue=0,          ///<艾利斯兰
        AndroidGreen,         ///<安卓绿
        AntiqueWhite,         ///<古董白
        AppleGreen,           ///<苹果绿
        Aqua,                 ///<浅绿色
        AquaMarine,           ///<碧绿色

        ArdenRed,             ///<雅顿红(注：商业使用需获得Elizabeth Arden公司授权)

        Azure,                ///<天蓝色
        BananaMania,          ///<香蕉黄(芯)
        BananaYellow,         ///<香蕉黄(皮)
        Beige,                ///<米色
        Bisque,               ///<桔黄色
        Black,                ///<黑色
        BlanchedAlmond,       ///<白杏色
        Blue,                 ///<蓝色
        BlueViolet,           ///<紫罗兰色
        Brown,                ///<褐色
        BurlyWood,            ///<实木色
        CadetBlue,            ///<军兰色
        CaribbeanGreen,       ///<加勒比海绿
        Chartreuse,           ///<黄绿色
        CherryBlossomPink,    ///<樱桃花粉
        Chocolate,            ///<巧克力色
        Coral,                ///<珊瑚色
        CornflowerBlue,       ///<菊花兰
        Cornsilk,             ///<米绸色
        Crimson,              ///<暗深红
        Cyan,                 ///<青色
        DarkBlue,             ///<暗蓝色
        DarkCyan,             ///<暗青色
        DarkGoldenrod,        ///<暗金黄
        DarkGray,             ///<暗灰色
        DarkGreen,            ///<暗绿色
        DarkGrey,             ///<暗白色
        DarkKhaki,            ///<暗黄褐色
        DarkMagenta,          ///<暗洋红
        DarkOliveGreen,       ///<暗橄榄绿
        DarkOrange,           ///<暗桔黄
        DarkOrchid,           ///<暗紫色
        DarkRed,              ///<暗红色
        DarkSalmon,           ///<暗肉色
        DarkSeaGreen,         ///<暗海兰
        DarkSlateBlue,        ///<暗灰兰
        DarkSlateGray,        ///<墨绿色
        DarkSlateGrey,        ///<暗灰绿
        DarkTurquoise,        ///<暗宝石绿
        DarkViolet,           ///<暗紫罗兰
        DeepPink,             ///<深粉红
        DeepSkyBlue,          ///<深天蓝
        DimGray,              ///<暗灰色
        DimGrey,              ///<暗灰白
        DodgerBlue,           ///<闪兰色
        FireBrick,            ///<火砖色
        FloralWhite,          ///<花白色
        ForestGreen,          ///<森林绿
        FrenchBeige,          ///<法国米色
        FrenchBlue,           ///<法国兰
        FrenchLilac,          ///<法国丁香色
        Fuchsia,              ///<紫红色
        Gainsboro,            ///<淡灰色
        GhostWhite,           ///<幽灵白
        Gold,                 ///<金色
        Goldenrod,            ///<金麒麟色
        GoldenYellow,         ///<金黄
        Gray,                 ///<灰色
        Green,                ///<绿色
        GreenYellow,          ///<蓝绿色
        Grey,                 ///<灰白色
        HollywoodCerise,      ///<好莱坞樱桃红
        Honeydew,             ///<蜜色
        HotPink,              ///<火热粉
        HunterGreen,          ///<猎人绿
        IndianGreen,          ///<印度绿
        IndianRed,            ///<印度红
        IndianYellow,         ///<印度黄
        Indigo,               ///<靛青色
        Ivory,                ///<象牙白
        Khaki,                ///<黄褐色
        Lavender,             ///<淡紫色
        LavenderBlush,        ///<淡紫红
        LawnGreen,            ///<草绿色
        Lemon,                ///<柠檬色
        LemonYellow,          ///<柠檬黄
        LemonChiffon,         ///<柠檬绸
        LightBlue,            ///<亮蓝色
        LightCoral,           ///<亮珊瑚色
        LightCyan,            ///<亮青色
        LightGoldenrodYellow, ///<亮金黄
        LightGray,            ///<亮灰色
        LightGreen,           ///<亮绿色
        LightGrey,            ///<亮灰白
        LightPink,            ///<亮粉红
        LightSalmon,          ///<亮肉色
        LightSeaGreen,        ///<亮海蓝
        LightSkyBlue,         ///<亮天蓝
        LightSlateGray,       ///<亮蓝灰
        LightSlateGrey,       ///<亮蓝白
        LightSteelBlue,       ///<亮钢兰
        LightYellow,          ///<亮黄色
        Lime,                 ///<酸橙色
        LimeGreen,            ///<橙绿色
        Linen,                ///<亚麻色
        Lion,                 ///<獅子棕
        Magenta,              ///<红紫色
        Maroon,               ///<粟色
        MediumAquamarine,     ///<间绿色
        MediumBlue,           ///<间兰色
        MediumOrchid,         ///<间淡紫
        MediumPurple,         ///<间紫色
        MediumSeaGreen,       ///<间海蓝
        MediumSlateBlue,      ///<间暗蓝
        MediumSpringGreen,    ///<间春绿
        MediumTurquoise,      ///<间绿宝石
        MediumVioletRed,      ///<间紫罗兰
        MidNightBlue,         ///<中灰蓝
        Mint,                 ///<薄荷色
        MintCream,            ///<薄荷霜
        MintGreen,            ///<薄荷绿
        MistyRose,            ///<浅玫瑰
        Moccasin,             ///<鹿皮色

        MozillaBlue,          ///<Mozilla Firefox Blue
        MozillaCharcoal,      ///<Mozilla charcoal
        MozillaLightBlue,     ///<Mozilla Firefox Light Blue
        MozillaLightOrange,   ///<Mozilla Firefox Light orange
        MoziilaNightBlue,     ///<Mozilla Nightly Blue
        MozillaOrange,        ///<Mozilla Firefox Orange
        MozillaRed,           ///<Mozilla red
        MozillaSand,          ///<Mozilla sand
        MozillaYellow,        ///<Mozilla Firefox Yellow

        NavajoWhite,          ///<纳瓦白
        Navy,                 ///<海军色

        NiveaBlue,            ///<妮维雅蓝(注：商业使用需获得Beiersdorf AG授权)

        NokiaBlue,            ///<诺基亚蓝

        OldLace,              ///<老花色
        Olive,                ///<橄榄色
        Olivedrab,            ///<深绿褐色
        Orange,               ///<橙色
        OrangeRed,            ///<红橙色
        Orchid,               ///<淡紫色
        PaleGoldenrod,        ///<苍麒麟色
        PaleGreen,            ///<苍绿色
        PaleTurquoise,        ///<苍宝石绿
        PaleVioletRed,        ///<苍紫罗兰色
        Papayawhip,           ///<番木色
        Peachpuff,            ///<桃色
        Pear,                 ///<梨色
        Peru,                 ///<秘鲁色
        Pink,                 ///<粉红色

                                //SONY PlayStation 商标用配色(注：商业使用需获得SONY授权)
        PlayStationBlue,
        PlayStationLightBlue,

        Plum,                 ///<洋李色

        PornHubRed,           ///<PornHub红
        PornHubYellow,        ///<PornHub黄

        PowderBlue,           ///<粉蓝色
        Purple,               ///<紫色
        Red,                  ///<红色
        Rose,                 ///<玫瑰红
        RosyBrown,            ///<褐玫瑰红
        RoyalBlue,            ///<皇家蓝
        Ruby,                 ///<宝石红
        SaddleBrown,          ///<重褐色
        Salmon,               ///<鲜肉色

        SamsungBlue,          ///<三星蓝

        SandyBrown,           ///<沙褐色
        SeaGreen,             ///<海绿色
        SeaShell,             ///<海贝色
        Sienna,               ///<赭色
        Silver,               ///<银色
        SkyBlue,              ///<天蓝色
        SlateBlue,            ///<石蓝色
        SlateGray,            ///<灰石色
        SlateGrey,            ///<白灰石色
        Snow,                 ///<雪白色
        SpringGreen,          ///<春绿色
        SteelBlue,            ///<钢兰色

        SUSEBlack,            ///<SUSE黑
        SUSEGreen,            ///<SUSE绿

        Tan,                  ///<茶色
        Teal,                 ///<水鸭色
        Thistle,              ///<蓟色

        TiffanyBlue,          ///<蒂芙尼蓝(知更鸟蛋蓝/勿忘我蓝)(注：商业使用需获取Tiffany公司授权)

        Tomato,               ///<西红柿色
        Turquoise,            ///<青绿色

        UbuntuOrange,         ///<Ubuntu橙色
        UbuntuLightAubergine,
        UbuntuMidAubergine,
        UbuntuDarkAubergine,

        Violet,               ///<紫罗兰色
        Wheat,                ///<浅黄色
        White,                ///<白色
        WhiteSmoke,           ///<烟白色
        Yellow,               ///<黄色
        YellowGreen,          ///<黄绿色

        BEGIN_RANGE =AliceBlue,
        END_RANGE   =YellowGreen,
        RANGE_SIZE  =(END_RANGE-BEGIN_RANGE)+1
    };//enum COLOR_ENUM

    extern COLOR_DEF prv_color[size_t(COLOR::RANGE_SIZE)];
}//namespace hgl
#endif//HGL_COLOR_TABLE_INCLUDE
