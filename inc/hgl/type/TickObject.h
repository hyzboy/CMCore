#pragma once

#include<hgl/type/_Object.h>

namespace hgl
{
    class TickObject:virtual public _Object
    {
    private:

        bool enable_tick=false;

    public:

        const bool &IsEnabledTick()const{return enable_tick;}
        void        SetEnableTick(bool et){enable_tick=et;}

    public:

        TickObject()=default;
        virtual ~TickObject()=default;

        virtual void Tick()=0;
    };//class TickObject:public _Object
}//namespace hgl
