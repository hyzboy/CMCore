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
