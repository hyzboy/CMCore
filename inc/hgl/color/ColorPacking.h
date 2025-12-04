#pragma once

#include<hgl/CoreType.h>

namespace hgl
{
    /**
    * r/g/b/a四个分量的颜色打包成一个32位整数，glsl中可用unpackUnorm4x8函数进行还原
    */
    constexpr uint32 PackRGBA8(const uint8 r,const uint8 &g,const uint8 &b,const uint8 &a)
    {
        return   static_cast<uint32>(r)      |
                (static_cast<uint32>(g)<< 8) |
                (static_cast<uint32>(b)<<16) |
                (static_cast<uint32>(a)<<24);
    }

    /**
    * r/g/b/a四个分量的颜色打包成一个32位整数，glsl中可用unpackUnorm4x8函数进行还原
    */
    constexpr uint32 PackRGBA8Float(const float r,const float &g,const float &b,const float &a)
    {
        return   static_cast<uint32>(r*255.0f)      |
                (static_cast<uint32>(g*255.0f)<< 8) |
                (static_cast<uint32>(b*255.0f)<<16) |
                (static_cast<uint32>(a*255.0f)<<24);
    }
}//namespace hgl