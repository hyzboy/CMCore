#include<hgl/math/Transform.h>

namespace hgl
{
    Transform Lerp(const Transform &from,const Transform &to,const float t)
    {
        Transform result;

        result.SetTranslation(  from.GetTranslation()*(1.0f-t) + to.GetTranslation()  *t);
        result.SetRotation(     SLerpQuat(from.GetRotationQuat(),to.GetRotationQuat(),t));
        result.SetScale(        from.GetScale()      *(1.0f-t) + to.GetScale()       *t);

        return result;
    }

    void Transform::UpdateQuat()
    {
        if(IsNearlyZero(rotate_angle))
        {
            if(is_zero_rotate)
                return;

            is_zero_rotate=true;
            rotation_quat=IdentityQuatf;
            // rotation changed to identity, mark matrix dirty so it will be updated
            matrix_dirty=true;
            return;
        }
        else
        {
            is_zero_rotate=false;
            rotation_quat=RotationQuat(rotate_angle,rotation_axis);
        }

        matrix_dirty=true;
    }

    void Transform::UpdateMatrix()
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
            matrix=TranslateMatrix(translation_vector)*ToMatrix(rotation_quat)*ScaleMatrix(scale_vector);
            inverse_matrix=inverse(matrix);
            transpose_inverse_matrix=transpose(inverse_matrix);

            is_identity=false;
        }

        matrix_dirty=false;

        UpdateVersion();
    }

    void Transform::SetToIdentity()
    {
        is_identity=true;
        is_zero_rotate=true;

        matrix=Identity4f;
        inverse_matrix=Identity4f;
        transpose_inverse_matrix=Identity4f;

        matrix_dirty=false;

        translation_vector=ZeroVector3f;
        rotation_quat=IdentityQuatf;
        rotation_axis=ZeroVector3f;
        rotate_angle=0;
        scale_vector=OneVector3f;
    }

    const bool Transform::operator == (const Transform &t)
    {
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

        if(translation_vector!=t.translation_vector)return(false);
        if(rotation_quat!=t.rotation_quat)return(false);
        if(scale_vector!=t.scale_vector)return(false);

        return(true);
    }

    void Transform::SetFromMatrix4f(const Matrix4f &m)
    {
        if(is_identity)
        {
            if(IsIdentityMatrix(m))
                return;
        }

        is_identity=IsIdentityMatrix(m);

        if(is_identity)
        {
            SetToIdentity();
            UpdateVersion();
            return;
        }

        matrix=m;
        inverse_matrix=inverse(m);
        transpose_inverse_matrix=transpose(inverse_matrix);

        matrix_dirty=false;

        DecomposeTransform(m,translation_vector,rotation_quat,scale_vector);

        ExtractedQuat(rotation_quat,rotation_axis,rotate_angle);
    }
}//namespace hgl
