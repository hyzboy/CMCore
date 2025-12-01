#pragma once

#include<hgl/math/Vector.h>

namespace hgl
{
    namespace graph
    {
        /**
        * @brief 插值算法类型枚举
        * 定义各种不同的插值方法
        */
        enum class LerpType
        {
            None,           ///<无插值
            Linear,         ///<线性插值 - 直接线性进程
            Cos,            ///<Cos插值 - 使用余弦曲线平滑过渡
            Cubic,          ///<三次插值 - 三次Hermite平滑曲线
            Hermite,        ///<Hermite插值 - 与Cubic相同
            Bezier,         ///<贝塞尔插值 - 带两个控制点的平滑曲线
            CatmullRom,     ///<CatmullRom插值 - 通过所有控制点的平滑曲线
            BSpline,        ///<B样条插值 - 统一的B样条曲线

            ENUM_CLASS_RANGE(None,BSpline)
        };

        /**
         * @brief 获取插值类型的字符串表示
         * @param type 插值类型
         * @return 插值类型的名称字符串
         */
        inline const char* GetLerpTypeName(LerpType type)
        {
            switch (type)
            {
                case LerpType::None:        return "None";
                case LerpType::Linear:      return "Linear";
                case LerpType::Cos:         return "Cosine";
                case LerpType::Cubic:       return "Cubic";
                case LerpType::Hermite:     return "Hermite";
                case LerpType::Bezier:      return "Bezier";
                case LerpType::CatmullRom:  return "CatmullRom";
                case LerpType::BSpline:     return "BSpline";
                default:                    return "Unknown";
            }
        }

        /**
         * @brief 获取插值类型的描述
         * @param type 插值类型
         * @return 插值类型的详细描述
         */
        inline const char* GetLerpTypeDescription(LerpType type)
        {
            switch (type)
            {
                case LerpType::None:
                    return "无插值";
                case LerpType::Linear:
                    return "直接线性进程，从起点到终点的直线过渡";
                case LerpType::Cos:
                    return "使用余弦曲线进行平滑缓动，产生自然的过渡效果";
                case LerpType::Cubic:
                    return "平滑的三次Hermite缓动曲线";
                case LerpType::Hermite:
                    return "Hermite插值（与Cubic相同）";
                case LerpType::Bezier:
                    return "带两个控制点的平滑贝塞尔曲线（三次贝塞尔）";
                case LerpType::CatmullRom:
                    return "通过所有控制点的平滑曲线（Catmull-Rom样条）";
                case LerpType::BSpline:
                    return "统一的B样条曲线";
                default:
                    return "未知插值类型";
            }
        }

        /**
         * @brief 插值控制点信息结构体
         * 指定给定插值类型所需的控制点数量
         */
        struct LerpControlPointInfo
        {
            LerpType type;              ///< 插值类型
            int required_points;        ///< 所需的控制点数
            const char* name;           ///< 插值类型名称
            const char* description;    ///< 插值类型描述
        };

        /**
         * @brief 获取插值类型的控制点信息
         * @param type 插值类型
         * @return 控制点信息
         */
        inline LerpControlPointInfo GetLerpControlPointInfo(LerpType type)
        {
            switch (type)
            {
                case LerpType::None:
                    return {type, 0, "None", "无插值"};
                
                case LerpType::Linear:
                case LerpType::Cos:
                case LerpType::Cubic:
                case LerpType::Hermite:
                    return {type, 2, "Linear/Cos/Cubic/Hermite", "2点插值（起点，终点）"};
                
                case LerpType::Bezier:
                case LerpType::CatmullRom:
                case LerpType::BSpline:
                    return {type, 4, "Bezier/CatmullRom/BSpline", "4点插值（p0, p1, p2, p3）"};
                
                default:
                    return {type, 0, "Unknown", "未知插值类型"};
            }
        }

    }//namespace graph
}//namespace hgl