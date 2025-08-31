#ifndef HGL_COLOR_TABLE_INCLUDE
#define HGL_COLOR_TABLE_INCLUDE

#include<hgl/TypeFunc.h>
#include<hgl/color/Color4f.h>
#include<hgl/math/Vector.h>

namespace hgl
{
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
        Azure,                ///<天蓝色

        BananaMania,          ///<香蕉黄(芯)
        BananaYellow,         ///<香蕉黄(皮)
        Beige,                ///<米色
        Bisque,               ///<桔黄色
        Black,                ///<黑色
        BlanchedAlmond,       ///<白杏色

        BlenderYellow,        ///<Blender Logo 黄色
        BlenderBlue,          ///<Blender Logo 蓝色

        BlenderAxisRed,       ///<Blender坐标轴红色
        BlenderAxisGreen,     ///<Blender坐标轴绿色
        BlenderAxisBlue,      ///<Blender坐标轴蓝色

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
        DarkCharcoal,         ///<炭黑色
        DarkCyan,             ///<暗青色
        DarkGoldenrod,        ///<暗金黄
        DarkGray,             ///<暗灰色
        DarkGreen,            ///<暗绿色
        DarkGrey,             ///<暗白色
        DarkGunmetal,         ///<黑炮铜
        DarkKhaki,            ///<暗黄褐色
        DarkMagenta,          ///<暗洋红
        DarkMidnightBlue,     ///<暗夜蓝
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

        EerieBlack,           ///<怪异黑

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
        MaastrichtBlue,       ///<马斯特里赫特蓝色

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

        OldLace,              ///<老花色
        Olive,                ///<橄榄色
        Olivedrab,            ///<深绿褐色
        Orange,               ///<橙色
        OrangeRed,            ///<红橙色
        Orchid,               ///<淡紫色

        OxfordBlue,           ///<牛津蓝

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

        SUSEPineGreen,        ///<SUSE松绿色
        SUSEJungleGreen,      ///<SUSE从林绿
        SUSEMidnightBlue,     ///<SUSE午夜蓝
        SUSEWaterholeBlue,    ///<SUSE水洞蓝
        SUSEMint,             ///<SUSE薄荷绿
        SUSEPersimmon,        ///<SUSE柿子红
        SUSEFog,              ///<SUSE雾色

        Tan,                  ///<茶色
        Teal,                 ///<水鸭色
        Thistle,              ///<蓟色

        TiffanyBlue,          ///<蒂芙尼蓝(知更鸟蛋蓝/勿忘我蓝)

        Tomato,               ///<西红柿色
        Turquoise,            ///<青绿色

        UbuntuOrange,         ///<Ubuntu橙色

        UbuntuLightAubergine, ///<Ubuntu亮茄皮紫
        UbuntuMidAubergine,   ///<Ubuntu中茄皮紫
        UbuntuDarkAubergine,  ///<Ubuntu暗茄皮紫

        UbuntuWarmGrey,       ///<Ubuntu暖灰色
        UbuntuCoolGrey,       ///<Ubuntu冷灰色
        UbuntuTextGrey,       ///<Ubuntu文本灰

        CanonicalAubergine,   ///<Canonical茄皮紫

        Violet,               ///<紫罗兰色

        Wheat,                ///<浅黄色
        White,                ///<白色
        WhiteSmoke,           ///<烟白色

        Yellow,               ///<黄色
        YellowGreen,          ///<黄绿色

        GrassGreen,           ///<草地绿
        BloodRed,             ///<血红色
        Amber,                ///<琥珀色
        Burgundy,             ///<勃艮第红

        ENUM_CLASS_RANGE(AliceBlue,Burgundy)
    };//enum COLOR_ENUM

    bool GetRGB(const enum class COLOR &ce,Color3ub &);
    bool GetBGR(const enum class COLOR &ce,Color3ub &);

    const uint32 GetRGBA(const enum class COLOR &ce,const uint8 &alpha);
    const uint32 GetABGR(const enum class COLOR &ce,const uint8 &alpha);

    const Color3f GetColor3f(const enum class COLOR &ce);
    const Color4f GetColor4f(const enum class COLOR &ce,const float &alpha=1.0f);

    const Color4ub GetColor4ub(const enum class COLOR &ce,const float &alpha=1.0f);

    const Color3f GetYCbCrColor3f(const enum class COLOR &ce);
    const Color4f GetYCbCrColor4f(const enum class COLOR &ce,const float &alpha);
    
    /**
     * 根据光谱值获取对应的RGB值
     * @param l 光谱值(从400到700)
     */
    const Color3f GetSpectralColor(const double l);
}//namespace hgl
#endif//HGL_COLOR_TABLE_INCLUDE
