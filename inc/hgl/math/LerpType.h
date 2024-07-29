#pragma once

#include<hgl/math/Vector.h>

namespace hgl
{
    namespace graph
    {
        /**
        * 插值算法
        */
        enum class LerpType
        {
            None,           ///<无插值
            Linear,         ///<线性插值
            Cos,            ///<Cos插值
            Cubic,          ///<三次插值
            Hermite,        ///<Hermite插值
            Bezier,         ///<贝塞尔插值
            CatmullRom,     ///<CatmullRom插值
            BSpline,        ///<B样条插值

            ENUM_CLASS_RANGE(None,BSpline)
        };
    }//namespace graph
}//namespace hgl