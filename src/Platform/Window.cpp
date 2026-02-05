#include<hgl/platform/Window.h>

namespace hgl
{
    Window::Window(const OSString &wn)
    {
        width=height=0;
        full_screen=false;
        win_name=wn;
        active=false;
        is_close=true;
        is_min=false;
    }

    void Window::OnCreate(uint w,uint h)
    {
        width=w;
        height=h;

        if(w==0||h==0)
            is_min=true;
        else
            is_min=false;
    }

    void Window::OnResize(uint w,uint h)
    {
        if(w==width&&height==h)
            return;

        width=w;
        height=h;

        if(w==0||h==0)
        {
            is_min=true;
        }
        else
        {
            is_min=false;
        }
    }

    void Window::OnActive(bool a)
    {
        active=a;
    }

    void Window::OnClose()
    {
        is_close=true;
    }

    io::EventProcResult Window::OnEvent(const io::EventHeader &header,const uint64 data)
    {
        if(header.type >= io::InputEventSource::Keyboard
        && header.type <= io::InputEventSource::Gesture)
        {
            const bool forward_physical = input_mapper.ProcessPhysicalInput(header, data);
            if(!forward_physical)
                return io::EventProcResult::Break;
        }

        return io::WindowEvent::OnEvent(header, data);
    }

    bool Window::Update()
    {
        while(MessageProc());

        if(is_close)
            return(false);

//        if(!active||is_min)
        if(is_min)
            this->WaitMessage();

        return(true);
    }
}//namespace hgl
