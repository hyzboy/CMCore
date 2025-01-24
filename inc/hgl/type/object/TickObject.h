#pragma once
#include<hgl/type/object/Object.h>

namespace hgl
{
    /**
     */
    class TickObject
    {
        bool tick;

    public:

        const bool IsTickable()const{return tick;}

        void SetTickEnable(bool t){tick=t;}

    public:

        TickObject();
        virtual ~TickObject();

        virtual void Tick(double delta_time)=0;
    };//class TickObject
}//namespcae hgl
