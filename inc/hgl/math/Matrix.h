#ifndef HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE
#define HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE

#include<hgl/math/Vector.h>
#include<hgl/TypeFunc.h>
//注：GLM/CML(OpenGLMode)是列矩阵,计算坐标matrix*pos
//   而MGL是行矩阵，需要反过来pos*matrix

#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtc/constants.hpp>
#include<glm/gtx/quaternion.hpp>
#include<glm/gtx/euler_angles.hpp>

namespace hgl
{
    using Quatf=glm::quat;
    const Quatf IdentityQuatf(1,0,0,0);     //w,x,y,z

#define DEFINE_MATRIX(num)  using Matrix##num##f=glm::mat##num;  \
                            const Matrix##num##f Identity##num##f=Matrix##num##f(1.0f); \
                            inline bool IsIdentityMatrix(const Matrix##num##f &m){return(hgl_cmp(m,Identity##num##f)==0);}  \
                            inline bool IsNearlyEqual(const Matrix##num##f &m1,const Matrix##num##f &m2)    \
                            {   \
                                float *f1=(float *)&m1;\
                                float *f2=(float *)&m2;\
                            \
                                for(int i=0;i<sizeof(Matrix##num##f)/sizeof(float);i++)  \
                                {   \
                                    if(!IsNearlyEqual(*f1,*f2))  \
                                        return(false);  \
                            \
                                    ++f1;++f2;  \
                                }   \
                            \
                                return(true);   \
                            }

    DEFINE_MATRIX(2)
    DEFINE_MATRIX(3)
    DEFINE_MATRIX(4)
    DEFINE_MATRIX(2x4)
    DEFINE_MATRIX(3x4)
    DEFINE_MATRIX(4x2)
    DEFINE_MATRIX(4x3)

//#undef DEFINE_MATRIX

    inline const bool IsNearlyEqual(const Quatf &q1,const Quatf &q2)
    {
        return glm::all(glm::epsilonEqual(q1,q2,glm::epsilon<float>()));
    }

    inline Matrix4f inverse(const Matrix4f &m)
    {
        return glm::inverse(m);
    }

    inline Matrix4f transpose(const Matrix4f &m)
    {
        return glm::transpose(m);
    }

    /**
     * 生成一个正角视图矩阵
     * @param left 左
     * @param right 右
     * @param top 顶
     * @param bottom 底
     * @param znear 近平面z值
     * @param zfar 远平台z值
     */
    Matrix4f ortho( float left,
                    float right,
                    float bottom,
                    float top,
                    float znear,
                    float zfar );

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     * @param znear 近平面z值
     * @param zfar 远平台z值
     */
    Matrix4f ortho(float width,float height,float znear,float zfar);

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     */
    Matrix4f ortho(float width,float height);

    /**
     * 生成一个透视矩阵
     * @param aspect_ratio 宽高比
     * @param field_of_view 视野
     * @param znear 近截面
     * @param zfar 远截面
     */
    Matrix4f perspective(   float field_of_view,
                            float aspect_ratio,                                
                            float znear,
                            float zfar);
    /**
     * 产生一个lookat变换矩阵
     * @param eye 眼睛位置
     * @param target 目标位置
     * @param up 向上向量
     */
    Matrix4f lookat(const Vector3f &eye,const Vector3f &target,const Vector3f &up);

    inline Matrix4f translate(const Vector3f &v)
    {
        return glm::translate(Matrix4f(1.0f),v);
    }

    inline Matrix4f translate(float x,float y,float z)
    {
        return glm::translate(Matrix4f(1.0f),Vector3f(x,y,z));
    }

    inline Matrix4f translate(float x,float y)
    {
        return translate(x,y,1.0f);
    }

    inline Matrix4f scale(const Vector3f &v)
    {
        return glm::scale(Matrix4f(1.0f),v);
    }

    inline Matrix4f scale(float x,float y,float z)
    {
        return glm::scale(Matrix4f(1.0f),Vector3f(x,y,z));
    }

    inline Matrix4f scale(float x,float y)
    {
        return scale(x,y,1.0f);
    }

    inline Matrix4f scale(float s)
    {
        return glm::scale(Matrix4f(1.0f),Vector3f(s,s,s));
    }

    inline Matrix4f rotate(float angle,const Vector3f &axis)
    {
        return glm::rotate(Matrix4f(1.0f),angle,axis);
    }

    inline Matrix4f rotate(float angle,float x,float y,float z)
    {
        return glm::rotate(Matrix4f(1.0f),angle,Vector3f(x,y,z));
    }
    
    inline Matrix4f rotate(float angle,float x,float y)
    {
        return rotate(angle,x,y,1.0f);
    }

    inline Matrix4f rotate(float angle,const Vector4f &axis)
    {
        return rotate(angle,Vector3f(axis.x,axis.y,axis.z));
    }

    inline Vector3f rotate(const Vector3f &v3f,float angle,const Vector3f &axis)
    {
        Vector4f result = rotate(angle, axis)*Vector4f(v3f, 1.0f);

        return Vector3f(result.x,result.y,result.z);
    }

    inline Quatf RotationQuat(const float angle,const Vector3f &axis)
    {
        return glm::angleAxis(glm::radians(angle),axis);
    }

    inline Matrix4f ToMatrix(const Quatf &quat)
    {
        return glm::toMat4(quat);
    }

    inline void ExtractedQuat(const Quatf &quat,Vector3f &axis,float &angle)
    {
        angle=glm::degrees(glm::angle(quat));
        axis=glm::axis(quat);
    }

    inline Quatf LerpQuat(const Quatf &from,const Quatf &to,const float t)
    {
        return glm::lerp(from,to,t);
    }

    inline Quatf SLerpQuat(const Quatf &from,const Quatf &to,const float t)
    {
        return glm::slerp(from,to,t);
    }

    inline Vector3f TransformPosition(const Matrix4f &m,const Vector3f &v)
    {
        return Vector3f(m*Vector4f(v,1.0f));
    }

    inline Vector3f TransformDirection(const Matrix4f &m,const Vector3f &v)
    {
        return Vector3f(m*Vector4f(v,0.0f));
    }

    inline Vector3f TransformNormal(const Matrix4f &m,const Vector3f &v)
    {
        return normalize(Vector3f(transpose(inverse(m))*Vector4f(v,0.0f)));
    }

    inline Vector3f TransformNormal(const Matrix3f &m,const Vector3f &v)
    {
        return normalize(m*v);
    }

    inline Matrix3f MatrixMult(const Matrix3f &parent,const Matrix3f &child)
    {
        return parent*child;
    }

    inline Matrix3f MatrixMult(const Matrix4f &parent,const Matrix3f &child)
    {
        return Matrix3f(parent*Matrix4f(child));
    }

    inline Matrix4f MatrixMult(const Matrix4f &parent,const Matrix4f &child)
    {
        return parent*child;
    }

    const Matrix4f GetRotateMatrix(const Vector3f &world_position,const Vector3f &old_direction,const Vector3f &new_direction);    
    const Quatf GetRotateQuat(const Vector3f &world_position,const Vector3f &old_direction,const Vector3f &new_direction);

    // 函数用于从 glm::mat4 中提取平移、旋转和缩放
    bool DecomposeTransform(const Matrix4f & transform, Vector3f & outTranslation, Quatf & outRotation, Vector3f & outScale);
}//namespace hgl
#endif//HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE
