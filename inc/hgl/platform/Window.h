#pragma once

#include<hgl/type/String.h>
#include<hgl/io/event/WindowEvent.h>

namespace hgl
{
    class Window:public io::WindowEvent
    {
    protected:

        uint width,height;
        bool full_screen;

        OSString win_name;

        bool active;
        bool is_close;
        bool is_min;

    protected:

        void OnResize(uint,uint) override;
        void OnActive(bool) override;
        void OnClose () override;

        virtual bool MessageProc()=0;
        virtual bool WaitMessage()=0;

    public:

        const uint GetWidth()const{return width;}
        const uint GetHeight()const{return height;}

    public:

        Window(const OSString &);
        virtual ~Window()=default;

        virtual bool Create(uint,uint)=0;
        virtual bool Create(uint,uint,uint)=0;
        virtual void Close()=0;

                bool IsMin()const{return is_min;}
                bool IsClose()const{return is_close;}
                bool IsVisible()const{return (!is_close)&&width&&height;}

        virtual void SetCaption(const OSString &)=0;

        virtual void Show()=0;
        virtual void Hide()=0;

        virtual void ToMinWindow()=0;
        virtual void ToMaxWindow()=0;

        virtual void SetSystemCursor(bool){}

        virtual bool Update() override;
    };//class Window

    Window *CreateRenderWindow(const OSString &win_name);

    void InitNativeWindowSystem();
}//namespace hgl
