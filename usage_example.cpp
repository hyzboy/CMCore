/**
 * Color3ub/Color4ub Usage Example
 * 
 * This file shows simple usage examples of the new Color3ub and Color4ub classes
 * with their conversion capabilities.
 */

#include<hgl/color/Color3ub.h>
#include<hgl/color/Color4ub.h>
#include<hgl/color/Color3f.h>
#include<hgl/color/Color4f.h>

int main()
{
    // 创建Color3ub颜色 (RGB 0-255范围)
    hgl::Color3ub redColor(255, 0, 0);
    hgl::Color3ub greenColor(0, 255, 0);
    hgl::Color3ub customColor(180, 120, 60);
    
    // 创建Color4ub颜色 (RGBA 0-255范围)
    hgl::Color4ub semiTransparentBlue(0, 0, 255, 128);
    hgl::Color4ub opaqueYellow(255, 255, 0, 255);
    
    // 转换为float版本 (0.0-1.0范围)
    hgl::Color3f redFloat = redColor.ToColor3f();
    hgl::Color4f blueFloat = semiTransparentBlue.ToColor4f();
    
    // 从float版本转换回来
    hgl::Color3f floatColor(0.6f, 0.8f, 0.4f);
    hgl::Color3ub byteColor = floatColor.ToColor3ub();
    
    // 使用赋值操作符进行转换
    hgl::Color3ub convertedColor;
    convertedColor = floatColor;  // 自动转换
    
    hgl::Color3f anotherFloatColor;
    anotherFloatColor = customColor;  // 自动转换
    
    // 使用构造函数转换
    hgl::Color3f constructedFromByte(customColor);
    hgl::Color3ub constructedFromFloat(floatColor);
    
    // 颜色运算
    hgl::Color3ub mixedColor = redColor + greenColor;  // 颜色混合
    hgl::Color3ub scaledColor = customColor * 0.8f;   // 缩放颜色
    
    // 获取32位颜色值
    uint32 rgba8 = semiTransparentBlue.ToRGBA8();
    uint32 bgra8 = semiTransparentBlue.ToBGRA8();
    uint32 argb8 = semiTransparentBlue.ToARGB8();
    
    return 0;
}