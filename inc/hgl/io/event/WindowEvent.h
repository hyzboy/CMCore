#pragma once

#include<hgl/io/event/EventDispatcher.h>
#include<hgl/math/Vector.h>

namespace hgl::io
{
    using namespace hgl::math;

    enum class WindowEventID
    {
        Create,
        Active,
        Resize,
        Close
    };//enum class WindowEventID

    union WindowEventData
    {
        uint64 data;

        struct
        {
            uint16 width,height;
        };

        bool active;
    };

    constexpr size_t WindowEventDataBytes=sizeof(WindowEventData);

    class WindowEvent:public EventDispatcher
    {
        WindowEventData *wed=nullptr;

        Vector2i WindowSize{};

    public:

        WindowEvent():EventDispatcher(InputEventSource::Window){}
        virtual ~WindowEvent()=default;

        const Vector2i &GetWindowSize()const{return WindowSize;}

        virtual EventProcResult OnEvent(const EventHeader &header,const uint64 data) override
        {
            if(header.type==InputEventSource::Window)
            {
                wed=(WindowEventData *)&data;

                switch(WindowEventID(header.id))
                {
                    case WindowEventID::Active: OnActive(wed->active);break;
                    case WindowEventID::Close:  OnClose();break;
                    case WindowEventID::Resize: WindowSize.x = wed->width;
                                                WindowSize.y = wed->height;
                                                OnResize(wed->width,wed->height);break;

                    case WindowEventID::Create: WindowSize.x = wed->width;
                                                WindowSize.y = wed->height;
                                                OnCreate(wed->width,wed->height);break;
                }
            }

            if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
                return EventProcResult::Break;

            return EventProcResult::Continue;
        }

    public:

        virtual void OnCreate(uint w,uint h){}
        virtual void OnResize(uint w,uint h){}
        virtual void OnActive(bool){}
        virtual void OnClose (){}
    };//class WindowEvent:public EventDispatcher
}//namespace hgl::io
