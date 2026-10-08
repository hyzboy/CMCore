#pragma once
#include<hgl/object/Object.h>

namespace hgl
{
    /**
     */
    class TickObject
    {
    public:

        TickObject()=default;
        virtual ~TickObject()=default;

        virtual void Tick(double delta_time)=0;
    };//class TickObject
}//namespcae hgl
