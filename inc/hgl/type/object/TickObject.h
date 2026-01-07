#pragma once
#include<hgl/type/object/Object.h>

namespace hgl
{
    /**
     */
    class TickObject
    {
        bool tickable=true;

    public:

        bool IsTickable()const{return tickable;}

        void SetTickable(bool t){tickable=t;}

    public:

        TickObject()=default;
        virtual ~TickObject()=default;

        virtual void Tick(double delta_time)=0;
    };//class TickObject
}//namespcae hgl
