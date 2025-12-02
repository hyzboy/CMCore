#pragma once

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

    DEFINE_MATRIX(2)
    DEFINE_MATRIX(3)
    DEFINE_MATRIX(4)
    DEFINE_MATRIX(2x4)
    DEFINE_MATRIX(3x4)
    DEFINE_MATRIX(4x2)
    DEFINE_MATRIX(4x3)

//#undef DEFINE_MATRIX

    inline const bool IsNearlyEqual(const Quatf &q1,const Quatf &q2,const float err=HGL_FLOAT_ERROR)
    {
        return glm::all(glm::epsilonEqual(q1,q2,err));
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
    Matrix4f OrthoMatrix( float left,
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
    Matrix4f OrthoMatrix(float width,float height,float znear,float zfar);

    /**
     * 生成一个正角视图矩阵
     * @param width 宽
     * @param height 高
     */
    Matrix4f OrthoMatrix(float width,float height);

    /**
     * 生成一个透视矩阵
     * @param aspect_ratio 宽高比
     * @param field_of_view 视野
     * @param znear 近截面
     * @param zfar 远截面
     */
    Matrix4f PerspectiveMatrix(   float field_of_view,
                            float aspect_ratio,                                
                            float znear,
                            float zfar);
    /**
     * 产生一个lookat变换矩阵
     * @param eye 眼睛位置
     * @param target 目标位置
     * @param up 向上向量
     */
    Matrix4f LookAtMatrix(const Vector3f &eye,const Vector3f &target,const Vector3f &up=AxisVector::Z);

    Vector2i ProjectToScreen(
        const Vector3f& world_pos,
        const Matrix4f& view,
        const Matrix4f& projection,
        const Vector2u& viewport_size);

    Vector3f UnProjectToWorld(
        const Vector2i &win_pos,
        const Matrix4f &view,
        const Matrix4f &projection,
        const Vector2u &viewport_size);

    inline Matrix4f TranslateMatrix(const Vector3f &v)
    {
        return glm::translate(Matrix4f(1.0f),v);
    }

    inline Matrix4f TranslateMatrix(float x,float y,float z)
    {
        return glm::translate(Matrix4f(1.0f),Vector3f(x,y,z));
    }

    inline Matrix4f TranslateMatrix(float x,float y)
    {
        return glm::translate(Matrix4f(1.0f),Vector3f(x,y,1.0f));
    }

    inline Matrix4f ScaleMatrix(const Vector3f &v)
    {
        return glm::scale(Matrix4f(1.0f),v);
    }

    inline Matrix4f ScaleMatrix(float x,float y,float z)
    {
        return glm::scale(Matrix4f(1.0f),Vector3f(x,y,z));
    }

    inline Matrix4f ScaleMatrix(float x,float y)
    {
        return glm::scale(Matrix4f(1.0f),Vector3f(x,y,1.0f));
    }

    inline Matrix4f ScaleMatrix(float s)
    {
        return glm::scale(Matrix4f(1.0f),Vector3f(s,s,s));
    }

    inline Matrix4f AxisXRotate(float rad){return glm::rotate(Matrix4f(1.0f),rad,AxisVector::X);} 
    inline Matrix4f AxisYRotate(float rad){return glm::rotate(Matrix4f(1.0f),rad,AxisVector::Y);} 
    inline Matrix4f AxisZRotate(float rad){return glm::rotate(Matrix4f(1.0f),rad,AxisVector::Z);} 

    inline Matrix4f AxisRotate(float rad,const Vector3f &axis)
    {
        return glm::rotate(Matrix4f(1.0f),rad,axis);
    }

    inline Matrix4f AxisRotate(float rad,float x,float y,float z)
    {
        return glm::rotate(Matrix4f(1.0f),rad,Vector3f(x,y,z));
    }
    
    inline Matrix4f AxisRotate(float rad,float x,float y)
    {
        return AxisRotate(rad,x,y,1.0f);
    }

    inline Matrix4f AxisRotate(float rad,const Vector4f &axis)
    {
        return AxisRotate(rad,Vector3f(axis.x,axis.y,axis.z));
    }
    
    // New: Matrix3f variants returning 3x3 rotation matrices
    inline Matrix3f AxisRotate3(float rad,const Vector3f &axis)
    {
        return glm::mat3(glm::rotate(Matrix4f(1.0f), rad, axis));
    }

    inline Matrix3f AxisRotate3(float rad,float x,float y,float z)
    {
        return AxisRotate3(rad, Vector3f(x,y,z));
    }

    inline Matrix3f AxisRotate3(float rad,float x,float y)
    {
        return AxisRotate3(rad, Vector3f(x,y,1.0f));
    }

    inline Matrix3f AxisRotate3(float rad,const Vector4f &axis)
    {
        return AxisRotate3(rad, Vector3f(axis.x, axis.y, axis.z));
    }

    // Degree-based convenience overloads
    inline Matrix3f AxisRotate3Deg(float deg,const Vector3f &axis)
    {
        return AxisRotate3(glm::radians(deg), axis);
    }

    inline Matrix3f AxisRotate3Deg(float deg,float x,float y,float z)
    {
        return AxisRotate3Deg(deg, Vector3f(x,y,z));
    }

    inline Matrix3f AxisRotate3Deg(float deg,const Vector4f &axis)
    {
        return AxisRotate3Deg(deg, Vector3f(axis.x, axis.y, axis.z));
    }

    inline Vector3f AxisRotate(const Vector3f &v3f,float rad,const Vector3f &axis)
    {
        Vector4f result = AxisRotate(rad, axis)*Vector4f(v3f, 1.0f);

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

    inline const Vector3f &GetRotateAxis(const Quatf &quat)
    {
        return glm::axis(quat);
    }

    inline const float GetRotateAngle(const Quatf &quat)
    {
        return glm::degrees(glm::angle(quat));
    }

    inline Quatf LerpQuat(const Quatf &from,const Quatf &to,const float t)
    {
        return glm::lerp(from,to,t);
    }

    inline Quatf SLerpQuat(const Quatf &from,const Quatf &to,const float t)
    {
        return glm::slerp(from,to,t);
    }

    inline Matrix4f MakeMatrix(const Vector3f &move,const Quatf &rotate_quat,const Vector3f &scale_xyz)
    {
        return glm::translate(Identity4f,move)
              *glm::toMat4(rotate_quat)
              *glm::scale(Identity4f,scale_xyz);
    }

    inline Matrix4f MakeMatrix(const Vector3f &move,const Vector3f &rotate_axis,const float &rotate_angle,const Vector3f &scale_xyz)
    {
        return glm::translate(Identity4f,move)
              *glm::rotate(Identity4f,glm::radians(rotate_angle),rotate_axis)
              *glm::scale(Identity4f,scale_xyz);
    }

    /**
    * 获取相对矩阵
    * @param self_matrix        自身矩阵
    * @param reference_matrix   参照矩阵
    */
    inline Matrix4f RelativeMatrix(const Matrix4f &self_matrix,const Matrix4f &reference_matrix)
    {
        return inverse(reference_matrix)*self_matrix;
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

    const Matrix4f GetRotateMatrix(const Vector3f &world_position,const Vector3f &old_direction,const Vector3f &new_direction);    
    const Quatf GetRotateQuat(const Vector3f &world_position,const Vector3f &old_direction,const Vector3f &new_direction);

    // 函数用于从 glm::mat4 中提取平移、旋转和缩放
    bool DecomposeTransform(const Matrix4f & transform, Vector3f & outTranslation, Quatf & outRotation, Vector3f & outScale);
}//namespace hgl
