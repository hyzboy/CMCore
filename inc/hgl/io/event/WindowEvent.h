#ifndef HGL_IO_WINDOW_EVENT_INCLUDE
#define HGL_IO_WINDOW_EVENT_INCLUDE

#include<hgl/io/event/InputEvent.h>
namespace hgl
{
    namespace io
    {
        enum class WindowEventID
        {
            Active,
            Resize,
            Close
        };//enum class WindowEventID

        union WindowEventData
        {
            uint64 data;

            struct
            {
                int16 width,height;
            };

            bool active;
        };

        constexpr size_t WindowEventDataBytes=sizeof(WindowEventData);

        class WindowEvent:public InputEvent
        {
            WindowEventData *wed;

        public:

            WindowEvent():InputEvent(InputEventSource::Window){wed=nullptr;}
            virtual ~WindowEvent()=default;

            virtual EventProcResult OnEvent(const EventHeader &header,const uint64 data) override
            {
                if(header.type==InputEventSource::Window)
                {
                    wed=(WindowEventData *)&data;

                    switch(WindowEventID(header.id))
                    {
                        case WindowEventID::Active:OnActive  (wed->active)           ;break;
                        case WindowEventID::Resize:OnResize  (wed->width,wed->height);break;
                        case WindowEventID::Close: OnClose   ()                      ;break;
                    }
                }

                if(InputEvent::OnEvent(header,data)==EventProcResult::Break)
                    return EventProcResult::Break;

                return EventProcResult::Continue;
            }

        public:

            virtual void OnResize(uint w,uint h){}
            virtual void OnActive(bool){}
            virtual void OnClose (){}
        };//class WindowEvent:public InputEvent
    }//namespace io
}//namespace hgl
#endif//HGL_IO_WINDOW_EVENT_INCLUDE
