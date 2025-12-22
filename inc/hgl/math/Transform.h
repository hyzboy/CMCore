#pragma once

#include<hgl/type/VersionData.h>
#include<hgl/math/MatrixOperations.h>

namespace hgl::math
{
    /**
     * 变换矩阵<Br>
     * 便于分散管理平移、旋转、缩放等数值
     */
    class Transform
    {
        uint32 version;     ///<版本号（注：用于记录任何变化，而不是matrix）

        void UpdateVersion()
        {
            if(version>=0x70000000)
                version=0;
            else
                ++version;
        }

    protected:

        bool is_identity;
        bool is_zero_rotate;
        bool matrix_dirty;

        //matrix其实用的少，所以在不取matrix时，并不会计算。
        Matrix4f matrix;
        Matrix4f inverse_matrix;
        Matrix4f transpose_inverse_matrix;

        Vector3f translation_vector;
        Quatf rotation_quat;
        Vector3f rotation_axis;
        float rotate_angle;

        Vector3f scale_vector;

    protected:

        void UpdateQuat();

    public:

        void UpdateMatrix();

        const uint32    GetVersion  ()const{return version;}
        const bool      IsIdentity  ()const{return is_identity;}
        const Matrix4f &GetMatrix   (){if(matrix_dirty)UpdateMatrix();return matrix;}

        const Matrix4f GetMatrix()const     //不能执行UpdateMatrix时的获取
        {
            if(matrix_dirty)
                return TranslateMatrix(translation_vector)*ToMatrix(rotation_quat)*ScaleMatrix(scale_vector);
            else
                return matrix;
        }

        operator const Matrix4f &   (){return GetMatrix();}
        operator const Transform &  (){UpdateMatrix();return *this;}

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
            if(is_identity)
            {
                if(IsNearlyZero(x)
                 &&IsNearlyZero(y)
                 &&IsNearlyZero(z))
                    return;
            }

            is_identity=false;

            translation_vector.x=x;
            translation_vector.y=y;
            translation_vector.z=z;
            matrix_dirty=true;
            UpdateVersion();
        }

        void SetTranslation(const Vector3f &v)
        {
            if(is_identity)
            {
                if(IsNearlyZero(v.x)
                 &&IsNearlyZero(v.y)
                 &&IsNearlyZero(v.z))
                    return;
            }

            is_identity=false;

            translation_vector=v;
            matrix_dirty=true;
            UpdateVersion();
        }

        void ClearRotation()
        {
            is_zero_rotate=true;
            rotation_quat=IdentityQuatf;
            rotation_axis=ZeroVector3f;
            rotate_angle=0;
            matrix_dirty=true;
            UpdateVersion();
        }

        void SetRotation(const Quatf &q)
        {
            if(is_identity)
            {
                if(IsNearlyEqual(q, IdentityQuatf))
                    return;
            }

            is_identity=false;

            rotation_quat=q;
            ExtractedQuat(q,rotation_axis,rotate_angle);
            is_zero_rotate=IsNearlyEqual(q, IdentityQuatf);
            matrix_dirty=true;
            UpdateVersion();
        }

        void SetRotation(const Vector3f &axis,const float angle)
        {
            if(is_identity)
            {
                if(IsNearlyZero(angle))
                    return;
            }

            is_identity=false;

            rotation_axis=axis;
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetRotation(const AXIS &axis,const float angle)
        {
            if(is_identity)
            {
                if(IsNearlyZero(angle))
                    return;
            }

            is_identity=false;

            rotation_axis=GetAxisVector(axis);
            rotate_angle=angle;
            UpdateQuat();
        }

        void SetRotateAngle(float angle)
        {
            if(is_identity)
            {
                if(IsNearlyZero(angle))
                    return;
            }

            is_identity=false;

            rotate_angle=angle;
            UpdateQuat();
        }

        void SetScale(const float &v)
        {
            if(is_identity)
            {
                if(IsNearlyEqual(v,1.0f))
                    return;
            }
            is_identity=false;

            scale_vector=Vector3f(v,v,v);
            matrix_dirty=true;
            UpdateVersion();
        }

        void SetScale(const float x,const float y,const float z)
        {
            if(is_identity)
            {
                if(IsNearlyEqual(x,1.0f)
                 &&IsNearlyEqual(y,1.0f)
                 &&IsNearlyEqual(z,1.0f))
                    return;
            }

            is_identity=false;

            scale_vector.x=x;
            scale_vector.y=y;
            scale_vector.z=z;

            matrix_dirty=true;
            UpdateVersion();
        }

        void SetScale(const Vector3f &v)
        {
            if(is_identity)
            {
                if(IsNearlyEqual(v.x,1.0f)
                 &&IsNearlyEqual(v.y,1.0f)
                 &&IsNearlyEqual(v.z,1.0f))
                    return;
            }

            is_identity=false;

            scale_vector=v;
            matrix_dirty=true;
            UpdateVersion();
        }

    public:

        Transform()
        {
            SetToIdentity();
            version=0;
        }

        Transform(const Matrix4f &m)
        {
            SetFromMatrix4f(m);
            version=0;
        }

        Transform(const Transform &t)
            : version(0)
            , is_identity(t.is_identity)
            , is_zero_rotate(t.is_zero_rotate)
            , matrix_dirty(t.matrix_dirty)
            , matrix(t.matrix)
            , inverse_matrix(t.inverse_matrix)
            , transpose_inverse_matrix(t.transpose_inverse_matrix)
            , translation_vector(t.translation_vector)
            , rotation_quat(t.rotation_quat)
            , rotation_axis(t.rotation_axis)
            , rotate_angle(t.rotate_angle)
            , scale_vector(t.scale_vector)
        {
        }

        void SetToIdentity();

        const bool operator == (const Transform &t);

        void operator = (const Transform &t)
        {
            if(this == &t)
                return;

            if(operator==(t))
                return;

            // 逐成员复制，避免使用 mem_copy
            is_identity = t.is_identity;
            is_zero_rotate = t.is_zero_rotate;
            matrix_dirty = t.matrix_dirty;
            matrix = t.matrix;
            inverse_matrix = t.inverse_matrix;
            transpose_inverse_matrix = t.transpose_inverse_matrix;
            translation_vector = t.translation_vector;
            rotation_quat = t.rotation_quat;
            rotation_axis = t.rotation_axis;
            rotate_angle = t.rotate_angle;
            scale_vector = t.scale_vector;

            UpdateVersion();
        }

        void SetFromMatrix4f(const Matrix4f &m)
        {
            DecomposeTransform(m,translation_vector,rotation_quat,scale_vector);

            UpdateVersion();    
        }

        const bool IsLastVersion()const{return !matrix_dirty;}

        inline Vector3f TransformPosition   (const Vector3f &v){if(matrix_dirty)UpdateMatrix();return Vector3f(matrix*Vector4f(v,1.0f));}
        inline Vector3f TransformDirection  (const Vector3f &v){if(matrix_dirty)UpdateMatrix();return Vector3f(matrix*Vector4f(v,0.0f));}
        inline Vector3f TransformNormal     (const Vector3f &v){if(matrix_dirty)UpdateMatrix();return normalize(Vector3f(transpose_inverse_matrix*Vector4f(v,0.0f)));}
        inline Matrix3f TransActionMatrix     (const Matrix3f &child){if(matrix_dirty)UpdateMatrix();return Matrix3f(matrix*Matrix4f(child));}
        inline Matrix4f TransActionMatrix     (const Matrix4f &child){if(matrix_dirty)UpdateMatrix();return matrix*child;}

        inline Vector3f InverseTransformPosition    (const Vector3f &v){if(matrix_dirty)UpdateMatrix();return Vector3f(inverse_matrix*Vector4f(v,1.0f));}
        inline Vector3f InverseTransformDirection   (const Vector3f &v){if(matrix_dirty)UpdateMatrix();return Vector3f(inverse_matrix*Vector4f(v,0.0f));}
        inline Vector3f InverseTransformNormal      (const Vector3f &v){if(matrix_dirty)UpdateMatrix();return normalize(Vector3f(transpose(matrix)*Vector4f(v,0.0f)));}
        inline Matrix3f InverseTransformMatrix      (const Matrix3f &child){if(matrix_dirty)UpdateMatrix();return Matrix3f(inverse_matrix*Matrix4f(child));}
        inline Matrix4f InverseTransformMatrix      (const Matrix4f &child){if(matrix_dirty)UpdateMatrix();return inverse_matrix*child;}

        inline Transform TransformTransform(const Transform &child)
        {
            if(matrix_dirty)UpdateMatrix();
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

    const Transform IdentityTransform;

    constexpr const size_t TransformMatrix4fLength=sizeof(Transform);

    Transform Lerp(const Transform &from,const Transform &to,const float t);
}//namespace hgl::math
