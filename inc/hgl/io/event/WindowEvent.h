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
        Close,

        DpiChanged,
        DisplayChange,
        SetCursor,
        MouseActivate,
        InputLangChange,
        ImeStartComposition,
        ImeComposition,
        ImeEndComposition
    };//enum class WindowEventID

    union WindowEventData
    {
        uint64 data;

        struct
        {
            uint16 width,height;
        };

        struct
        {
            uint32 wparam;
            uint32 lparam;
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

                    case WindowEventID::DpiChanged:         OnDpiChanged(uint16(wed->wparam & 0xFFFF), uint16((wed->wparam >> 16) & 0xFFFF));break;
                    case WindowEventID::DisplayChange:      OnDisplayChange(uint16(wed->lparam & 0xFFFF), uint16((wed->lparam >> 16) & 0xFFFF), wed->wparam);break;
                    case WindowEventID::SetCursor:          OnSetCursor(wed->wparam, wed->lparam);break;
                    case WindowEventID::MouseActivate:      OnMouseActivate(wed->wparam, wed->lparam);break;
                    case WindowEventID::InputLangChange:    OnInputLangChange(wed->wparam, wed->lparam);break;
                    case WindowEventID::ImeStartComposition:OnImeStartComposition(wed->wparam, wed->lparam);break;
                    case WindowEventID::ImeComposition:     OnImeComposition(wed->wparam, wed->lparam);break;
                    case WindowEventID::ImeEndComposition:  OnImeEndComposition(wed->wparam, wed->lparam);break;
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

        virtual void OnDpiChanged(uint16,uint16){}
        virtual void OnDisplayChange(uint16,uint16,uint32){}
        virtual void OnSetCursor(uint32,uint32){}
        virtual void OnMouseActivate(uint32,uint32){}
        virtual void OnInputLangChange(uint32,uint32){}
        virtual void OnImeStartComposition(uint32,uint32){}
        virtual void OnImeComposition(uint32,uint32){}
        virtual void OnImeEndComposition(uint32,uint32){}
    };//class WindowEvent:public EventDispatcher
}//namespace hgl::io
