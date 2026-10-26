#include<hgl/object/RefObject.h>

namespace hgl
{
    RefObject::RefObject()
        :ref_count(0),
         ref_zero_policy(RefZeroPolicy::AutoDestroy)
    {
    }

    void RefObject::OnRefZero()
    {
        switch(ref_zero_policy)
        {
            case RefZeroPolicy::AutoDestroy:
                Release();
                delete this;
                break;

            case RefZeroPolicy::KeepMetaUnloadPayload:
                Release();
                break;
        }
    }
}//namespace hgl
