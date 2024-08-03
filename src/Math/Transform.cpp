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
}//namespace hgl
