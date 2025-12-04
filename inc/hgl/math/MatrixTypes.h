/**
 * MatrixTypes.h - 矩阵类型定义
 * 
 * 包含：
 * - 矩阵类型定义 (Matrix2f, Matrix3f, Matrix4f 等)
 * - Identity 常量
 * - IsIdentityMatrix 判断函数
 * - IsNearlyEqual 比较函数
 */

#pragma once

#include<hgl/math/FloatPrecision.h>
#include<hgl/type/MemoryUtil.h>
#include<glm/glm.hpp>

//注：GLM/CML(OpenGLMode)是列矩阵,计算坐标matrix*pos
//   而MGL是行矩阵，需要反过来pos*matrix

namespace hgl
{
    /**
     * 矩阵类型定义宏
     * 为每个矩阵维度生成：
     * - 类型别名 (Matrix2f, Matrix3f 等)
     * - 字节大小常量 (Matrix2fBytes 等)
     * - 单位矩阵常量 (Identity2f 等)
     * - IsIdentityMatrix 判断函数
     * - IsNearlyEqual 比较函数
     */
#define DEFINE_MATRIX(num)  using Matrix##num##f=glm::mat##num;  \
                            constexpr const size_t Matrix##num##fBytes=sizeof(Matrix##num##f); \
                            const Matrix##num##f Identity##num##f=Matrix##num##f(1.0f); \
                            inline bool IsIdentityMatrix(const Matrix##num##f &m){return(hgl_cmp(m,Identity##num##f)==0);}  \
                            inline bool IsNearlyEqual(const Matrix##num##f &m1,const Matrix##num##f &m2,const float err=HGL_FLOAT_ERROR)    \
                            {   \
                                float *f1=(float *)&m1;\
                                float *f2=(float *)&m2;\
                            \
                                for(int i=0;i<sizeof(Matrix##num##f)/sizeof(float);i++)  \
                                {   \
                                    if(!IsNearlyEqual(*f1,*f2,err))  \
                                        return(false);  \
                            \
                                    ++f1;++f2;  \
                                }   \
                            \
                                return(true);   \
                            }

    // 方阵类型定义
    DEFINE_MATRIX(2)
    DEFINE_MATRIX(3)
    DEFINE_MATRIX(4)
    
    // 非方阵类型定义
    DEFINE_MATRIX(2x4)
    DEFINE_MATRIX(3x4)
    DEFINE_MATRIX(4x2)
    DEFINE_MATRIX(4x3)

//#undef DEFINE_MATRIX
}//namespace hgl
