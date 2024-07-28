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

namespace hgl
{
    using Quatf=glm::quat;
    constexpr const Quatf IdentityQuatf=Quatf(1,0,0,0);

#define DEFINE_MATRIX(num)  using Matrix##num##f=glm::mat##num;  \
                            constexpr const Matrix##num##f Identity##num##f=Matrix##num##f(1.0f); \
                            inline bool IsIdentity(const Matrix##num##f &m){return(hgl_cmp(m,Identity##num##f)==0);}    \
                            inline int FastMatrixComp(const Matrix##num##f &m1,const Matrix##num##f &m2){return hgl_cmp(m1,m2);}

    DEFINE_MATRIX(2)
    DEFINE_MATRIX(3)
    DEFINE_MATRIX(4)
    DEFINE_MATRIX(2x4)
    DEFINE_MATRIX(3x4)
    DEFINE_MATRIX(4x2)
    DEFINE_MATRIX(4x3)

#undef DEFINE_MATRIX

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

    inline Matrix3f TransformMatrix(const Matrix3f &root,const Matrix3f &child)
    {
        return root*child;
    }

    inline Matrix3f TransformMatrix(const Matrix4f &root,const Matrix3f &child)
    {
        return Matrix3f(root*Matrix4f(child));
    }

    inline Matrix4f TransformMatrix(const Matrix4f &root,const Matrix4f &child)
    {
        return root*child;
    }

    /**
     * 带误差的比较两个浮点矩阵是否相等
     * @param m1 矩阵1
     * @param m2 矩阵2
     * @param deviation 允许的误差值
     */
    template<typename M>
    inline float DeviationMatrixComp(const M &m1,const M &m2,const float deviation=HGL_FLOAT_SMALL)
    {
        float *f1=(float *)&m1;
        float *f2=(float *)&m2;
        float gap;

        for(int i=0;i<sizeof(M)/sizeof(float);i++)
        {
            gap=*f1-*f2;

            if(fabsf(gap)<deviation)
                return(gap);
        }

        return 0;
    }

    // 函数用于从 glm::mat4 中提取平移、旋转和缩放
    bool DecomposeTransform(const Matrix4f & transform, Vector3f & outTranslation, Quatf & outRotation, Vector3f & outScale);

    /**
     * 变换矩阵<Br>
     * 便于分散管理平移、旋转、缩放等数值
     */
    class TransformMatrix4f
    {
    protected:

        Matrix4f matrix;
        Matrix4f inverse_matrix;
        Matrix4f transpose_inverse_matrix;

        bool matrix_dirty;

        Vector3f translation_vector;

        Quatf rotation_quat;
        Vector3f rotation_axis;
        float rotate_angle;

        Vector3f scale_vector;

    protected:

        void UpdateMatrix()
        {
            if(!matrix_dirty)
                return;

            matrix=translate(translation_vector)*ToMatrix(rotation_quat)*scale(scale_vector);
            inverse_matrix=inverse(matrix);
            transpose_inverse_matrix=transpose(inverse_matrix);

            matrix_dirty=false;
        }

        void UpdateQuat()
        {
            rotation_quat=RotationQuat(rotate_angle,rotation_axis);
            matrix_dirty=true;
        }

    public:

        const Matrix4f &GetMatrix()
        {
            UpdateMatrix();
            return matrix;
        }

        const Matrix4f &GetInverseMatrix()
        {
            UpdateMatrix();
            return inverse_matrix;
        }

        const Vector3f &GetTranslation  ()const{return translation_vector;}
        const Vector3f &GetScale        ()const{return scale_vector;}

        const Quatf &   GetRotationQuat ()const{return rotation_quat;}
        const Vector3f &GetRotationAxis ()const{return rotation_axis;}
        const float     GetRotateAngle  ()const{return rotate_angle;}

        void SetTranslation(const Vector3f &v)
        {
            translation_vector=v;
            matrix_dirty=true;
        }

        void SetRotation(const Quatf &q)
        {
            rotation_quat=q;
            ExtractedQuat(q,rotation_axis,rotate_angle);
            matrix_dirty=true;
        }

        void SetRotation(const Vector3f &axis,float angle)
        {
            rotation_axis=axis;
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetRotationAxis(const Vector3f &axis)
        {
            rotation_axis=axis;
            UpdateQuat();
        }

        void SetRotateAngle(float angle)
        {
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetScale(const Vector3f &v)
        {
            scale_vector=v;
            matrix_dirty=true;
        }

    public:

        TransformMatrix4f()
        {
            matrix=Identity4f;
            inverse_matrix=Identity4f;

            matrix_dirty=false;

            translation_vector=Vector3f(0,0,0);
            rotation_quat=Quatf(1,0,0,0);
            rotation_axis=Vector3f(0,0,0);
            rotate_angle=0;
            scale_vector=Vector3f(1,1,1);
        }

        TransformMatrix4f(const Matrix4f &m)
        {
            SetFromMatrix4f(m);
        }

        void SetFromMatrix4f(const Matrix4f &m)
        {
            matrix=m;
            inverse_matrix=inverse(m);

            matrix_dirty=false;

            DecomposeTransform(m,translation_vector,rotation_quat,scale_vector);

            ExtractedQuat(rotation_quat,rotation_axis,rotate_angle);
        }

        inline Vector3f TransformPosition(const Vector3f &v)
        {
            return Vector3f(matrix*Vector4f(v,1.0f));
        }

        inline Vector3f TransformDirection(const Vector3f &v)
        {
            return Vector3f(matrix*Vector4f(v,0.0f));
        }

        inline Vector3f TransformNormal(const Vector3f &v)
        {
            return normalize(Vector3f(transpose_inverse_matrix*Vector4f(v,0.0f)));
        }

        inline Matrix3f TransformMatrix(const Matrix3f &child)
        {
            return Matrix3f(matrix*Matrix4f(child));
        }

        inline Matrix4f TransformMatrix(const Matrix4f &child)
        {
            return matrix*child;
        }
    };//TransformMatrix4f

    constexpr const size_t TransformMatrix4fLength=sizeof(TransformMatrix4f);
}//namespace hgl
#endif//HGL_ALGORITHM_MATH_VECTOR_MATRIX_INCLUDE
