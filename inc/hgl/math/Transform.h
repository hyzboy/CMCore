#pragma once
#include<hgl/math/Matrix.h>

namespace hgl
{
    /**
     * 变换矩阵<Br>
     * 便于分散管理平移、旋转、缩放等数值
     */
    class Transform
    {
    protected:

        uint32 version;

        bool is_identity;
        bool is_zero_rotate;
        bool matrix_dirty;

        Matrix4f matrix;
        Matrix4f inverse_matrix;
        Matrix4f transpose_inverse_matrix;

        Vector3f translation_vector;

        Quatf rotation_quat;
        Vector3f rotation_axis;
        float rotate_angle;

        Vector3f scale_vector;

    protected:

        void UpdateQuat()
        {
            if(IsNearlyZero(rotate_angle))
            {
                if(is_zero_rotate)
                    return;

                is_zero_rotate=true;
                rotation_quat=IdentityQuatf;
                return;
            }
            else
            {
                is_zero_rotate=false;
                rotation_quat=RotationQuat(rotate_angle,rotation_axis);
            }

            matrix_dirty=true;
        }

    public:

        void UpdateMatrix()
        {
            if(!matrix_dirty)
                return;

            if(translation_vector==ZeroVector3f
               &&rotation_quat==IdentityQuatf
               &&scale_vector==OneVector3f)
            {
                if(is_identity)     //如果之前就是空的，那不更新版本号
                {
                    matrix_dirty=false;
                    return;
                }

                SetToIdentity();
            }
            else
            {
                matrix=translate(translation_vector)*ToMatrix(rotation_quat)*scale(scale_vector);
                inverse_matrix=inverse(matrix);
                transpose_inverse_matrix=transpose(inverse_matrix);

                is_identity=false;
            }

            matrix_dirty=false;

            if(version>0xF0000000)
                version=0;
            else
                ++version;
        }

        const uint32 GetVersion()
        {
            UpdateMatrix();
            return version;
        }

        const bool IsIdentity()
        {
            UpdateMatrix();
            return is_identity;
        }

        const Matrix4f &GetMatrix()
        {
            UpdateMatrix();
            return matrix;
        }

        const Matrix4f GetMatrix()const     //不能执行UpdateMatrix时的获取
        {
            if(matrix_dirty)
                return translate(translation_vector)*ToMatrix(rotation_quat)*scale(scale_vector);
            else
                return matrix;
        }

        operator const Matrix4f &(){return GetMatrix();}

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

        void SetTranslation(const float x,const float y,const float z)
        {
            translation_vector.x=x;
            translation_vector.y=y;
            translation_vector.z=z;
            matrix_dirty=true;
        }

        void SetTranslation(const Vector3f &v)
        {
            translation_vector=v;
            matrix_dirty=true;
        }

        void ClearRotation()
        {
            rotation_quat=IdentityQuatf;
            rotation_axis=ZeroVector3f;
            rotate_angle=0;
            matrix_dirty=true;
        }

        void SetRotation(const Quatf &q)
        {
            rotation_quat=q;
            ExtractedQuat(q,rotation_axis,rotate_angle);
            matrix_dirty=true;
        }

        void SetRotation(const Vector3f &axis,const float angle)
        {
            rotation_axis=axis;
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetRotation(const AXIS &axis,const float angle)
        {
            rotation_axis=GetAxisVector(axis);
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetRotateAngle(float angle)
        {
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetScale(const float &v)
        {
            scale_vector=Vector3f(v,v,v);
            matrix_dirty=true;
        }

        void SetScale(const float x,const float y,const float z)
        {
            scale_vector.x=x;
            scale_vector.y=y;
            scale_vector.z=z;

            matrix_dirty=true;
        }

        void SetScale(const Vector3f &v)
        {
            scale_vector=v;
            matrix_dirty=true;
        }

    public:

        Transform()
        {
            SetToIdentity();
        }

        Transform(const Matrix4f &m)
        {
            SetFromMatrix4f(m);
        }

        Transform(const Transform &t)
        {
            hgl_cpy(*this,t);
        }

        void SetToIdentity()
        {
            is_identity=true;
            is_zero_rotate=true;

            matrix=Identity4f;
            inverse_matrix=Identity4f;

            matrix_dirty=false;

            translation_vector=ZeroVector3f;
            rotation_quat=IdentityQuatf;
            rotation_axis=ZeroVector3f;
            rotate_angle=0;
            scale_vector=OneVector3f;
        }

        const bool operator == (const Transform &t)
        {
            UpdateMatrix();

            if(is_identity)
            {
                if(t.is_identity)
                    return(true);
                else
                    return(false);
            }
            else
            {
                if(t.is_identity)
                    return(false);
            }

            const Matrix4f tm=t.GetMatrix();

            return matrix==tm;
        }

        void operator = (const Transform &t)
        {
            hgl_cpy(*this,t);

            UpdateMatrix();
        }

        void SetFromMatrix4f(const Matrix4f &m)
        {
            is_identity=IsIdentityMatrix(m);

            if(is_identity)
            {
                SetToIdentity();
                return;
            }

            matrix=m;
            inverse_matrix=inverse(m);

            matrix_dirty=false;

            DecomposeTransform(m,translation_vector,rotation_quat,scale_vector);

            ExtractedQuat(rotation_quat,rotation_axis,rotate_angle);
        }

        inline Vector3f TransformPosition(const Vector3f &v)
        {
            UpdateMatrix();
            return Vector3f(matrix*Vector4f(v,1.0f));
        }

        inline Vector3f TransformDirection(const Vector3f &v)
        {
            UpdateMatrix();
            return Vector3f(matrix*Vector4f(v,0.0f));
        }

        inline Vector3f TransformNormal(const Vector3f &v)
        {
            UpdateMatrix();
            return normalize(Vector3f(transpose_inverse_matrix*Vector4f(v,0.0f)));
        }

        inline Matrix3f TransformMatrix(const Matrix3f &child)
        {
            UpdateMatrix();
            return Matrix3f(matrix*Matrix4f(child));
        }

        inline Matrix4f TransformMatrix(const Matrix4f &child)
        {
            UpdateMatrix();
            return matrix*child;
        }

        inline Transform TransformTransform(const Transform &child)
        {
            UpdateMatrix();
            const Matrix4f &child_matrix=child.GetMatrix();

            return Transform(matrix*child_matrix);
        }

        inline Transform TransformTransform(const Transform &child)const
        {
            const Matrix4f &cur_matrix=GetMatrix();
            const Matrix4f &child_matrix=child.GetMatrix();

            return Transform(cur_matrix*child_matrix);
        }
    };//Transform

    constexpr const size_t TransformMatrix4fLength=sizeof(Transform);

    Transform Lerp(const Transform &from,const Transform &to,const float t);
}//namespace hgl
