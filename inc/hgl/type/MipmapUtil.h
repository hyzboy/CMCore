#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/type/CompareUtil.h>
#include<cmath>

namespace hgl
{
    //==================================================================================================
    // Mipmap 级别计算 / Mipmap Level Calculation
    //==================================================================================================
    
    inline const uint GetMipLevel(const uint size)
    {
        return static_cast<uint>(floor(log2(size)))+1;
    }

    inline  const uint GetMipLevel(const uint width,const uint height)
    {
        return GetMipLevel(hgl_max(width,height));
    }

    inline  const uint GetMipLevel(const uint width,const uint height,const uint depth)
    {
        return GetMipLevel(hgl_max(hgl_max(width,height),depth));
    }
}//namespace hgl
