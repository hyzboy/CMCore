#pragma once

#include<hgl/type/String.h>
#include<hgl/io/event/WindowEvent.h>
#include<hgl/io/event/InputMapping.h>

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

        io::InputMapper input_mapper;

    protected:

        void OnCreate(uint,uint) override;
        void OnResize(uint,uint) override;
        void OnActive(bool) override;
        void OnClose() override;

        void OnDpiChanged(uint16,uint16) override {}
        void OnDisplayChange(uint16,uint16,uint32) override {}
        void OnSetCursor(uint32,uint32) override {}
        void OnMouseActivate(uint32,uint32) override {}
        void OnInputLangChange(uint32,uint32) override {}
        void OnImeStartComposition(uint32,uint32) override {}
        void OnImeComposition(uint32,uint32) override {}
        void OnImeEndComposition(uint32,uint32) override {}

        io::EventProcResult OnEvent(const io::EventHeader &header,const uint64 data) override;

        virtual bool MessageProc()=0;
        virtual bool WaitMessage()=0;

    public:

        const uint GetWidth()const{return width;}
        const uint GetHeight()const{return height;}

    public:

        Window(const OSString &);
        virtual ~Window()=default;

        io::InputMapper &GetInputMapper(){return input_mapper;}

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
