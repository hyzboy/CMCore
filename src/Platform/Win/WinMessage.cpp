#include<hgl/platform/WinWindow.h>
#include<hgl/io/event/KeyboardEvent.h>
#include<hgl/io/event/MouseEvent.h>
#include<hgl/io/event/WindowEvent.h>
#include<hgl/io/event/PointerEvent.h>
#include<Windows.h>

#ifdef _DEBUG
#include<hgl/log/Log.h>
#endif//_DEBUG

namespace hgl
{
    using namespace io;

    namespace
    {
        #define PROP_DPIISOLATION          L"PROP_ISOLATION"

        static KeyboardButton KeyConvert[256]{};
        static void (*WMProc[2048])(EventDispatcher *,uint32,uint32){};                 //消息处理队列

        void InitKeyConvert()
        {
            int i;

            KeyConvert[VK_ESCAPE    ]=KeyboardButton::Esc;
            for(i=VK_F1;i<=VK_F12;i++)KeyConvert[i]=(KeyboardButton)(int(KeyboardButton::F1)+i-VK_F1);

            KeyConvert['`'          ]=KeyboardButton::Grave;
            for(i='0';i<='9';i++)KeyConvert[i]=KeyboardButton(int(KeyboardButton::_0)+i-'0');
            KeyConvert['-'          ]=KeyboardButton::Minus;
            KeyConvert['='          ]=KeyboardButton::Equals;
            KeyConvert['\\'         ]=KeyboardButton::BackSlash;
            KeyConvert[VK_BACK      ]=KeyboardButton::BackSpace;

            KeyConvert[VK_TAB       ]=KeyboardButton::Tab;
            KeyConvert['Q'          ]=KeyboardButton::Q;
            KeyConvert['W'          ]=KeyboardButton::W;
            KeyConvert['E'          ]=KeyboardButton::E;
            KeyConvert['R'          ]=KeyboardButton::R;
            KeyConvert['T'          ]=KeyboardButton::T;
            KeyConvert['Y'          ]=KeyboardButton::Y;
            KeyConvert['U'          ]=KeyboardButton::U;
            KeyConvert['I'          ]=KeyboardButton::I;
            KeyConvert['O'          ]=KeyboardButton::O;
            KeyConvert['P'          ]=KeyboardButton::P;
            KeyConvert['['          ]=KeyboardButton::LeftBracket;
            KeyConvert[']'          ]=KeyboardButton::RightBracket;

            KeyConvert[VK_CAPITAL   ]=KeyboardButton::CapsLock;
            KeyConvert['A'          ]=KeyboardButton::A;
            KeyConvert['S'          ]=KeyboardButton::S;
            KeyConvert['D'          ]=KeyboardButton::D;
            KeyConvert['F'          ]=KeyboardButton::F;
            KeyConvert['G'          ]=KeyboardButton::G;
            KeyConvert['H'          ]=KeyboardButton::H;
            KeyConvert['J'          ]=KeyboardButton::J;
            KeyConvert['K'          ]=KeyboardButton::K;
            KeyConvert['L'          ]=KeyboardButton::L;
            KeyConvert[';'          ]=KeyboardButton::Semicolon;
            KeyConvert['\''         ]=KeyboardButton::Apostrophe;
            KeyConvert[VK_RETURN    ]=KeyboardButton::Enter;

            KeyConvert[VK_LSHIFT    ]=KeyboardButton::LeftShift;
            KeyConvert['Z'          ]=KeyboardButton::Z;
            KeyConvert['X'          ]=KeyboardButton::X;
            KeyConvert['C'          ]=KeyboardButton::C;
            KeyConvert['V'          ]=KeyboardButton::V;
            KeyConvert['B'          ]=KeyboardButton::B;
            KeyConvert['N'          ]=KeyboardButton::N;
            KeyConvert['M'          ]=KeyboardButton::M;
            KeyConvert[','          ]=KeyboardButton::Comma;
            KeyConvert['.'          ]=KeyboardButton::Period;
            KeyConvert['/'          ]=KeyboardButton::Slash;
            KeyConvert[VK_RSHIFT    ]=KeyboardButton::RightShift;

            KeyConvert[VK_LCONTROL  ]=KeyboardButton::LeftCtrl;
            KeyConvert[VK_LWIN      ]=KeyboardButton::LeftOS;
            KeyConvert[VK_LMENU     ]=KeyboardButton::LeftAlt;
            KeyConvert[VK_SPACE     ]=KeyboardButton::Space;
            KeyConvert[VK_RMENU     ]=KeyboardButton::RightAlt;
            KeyConvert[VK_RWIN      ]=KeyboardButton::RightOS;
            KeyConvert[VK_RCONTROL  ]=KeyboardButton::RightCtrl;

            KeyConvert[VK_PAUSE     ]=KeyboardButton::Pause;
        //        KeyConvert[VK_CLEAR     ]=KeyboardButton::Clear;

            KeyConvert[VK_NUMPAD0   ]=KeyboardButton::Num0;
            KeyConvert[VK_NUMPAD1   ]=KeyboardButton::Num1;
            KeyConvert[VK_NUMPAD2   ]=KeyboardButton::Num2;
            KeyConvert[VK_NUMPAD3   ]=KeyboardButton::Num3;
            KeyConvert[VK_NUMPAD4   ]=KeyboardButton::Num4;
            KeyConvert[VK_NUMPAD5   ]=KeyboardButton::Num5;
            KeyConvert[VK_NUMPAD6   ]=KeyboardButton::Num6;
            KeyConvert[VK_NUMPAD7   ]=KeyboardButton::Num7;
            KeyConvert[VK_NUMPAD8   ]=KeyboardButton::Num8;
            KeyConvert[VK_NUMPAD9   ]=KeyboardButton::Num9;

            KeyConvert[VK_DECIMAL   ]=KeyboardButton::NumDecimal;
            KeyConvert[VK_DIVIDE    ]=KeyboardButton::NumDivide;
            KeyConvert[VK_MULTIPLY  ]=KeyboardButton::NumMultiply;
            KeyConvert[VK_SUBTRACT  ]=KeyboardButton::NumSubtract;
            KeyConvert[VK_ADD       ]=KeyboardButton::NumAdd;

            KeyConvert[VK_UP        ]=KeyboardButton::Up;
            KeyConvert[VK_DOWN      ]=KeyboardButton::Down;
            KeyConvert[VK_LEFT      ]=KeyboardButton::Left;
            KeyConvert[VK_RIGHT     ]=KeyboardButton::Right;

            KeyConvert[VK_INSERT    ]=KeyboardButton::Insert;
            KeyConvert[VK_DELETE    ]=KeyboardButton::Delete;
            KeyConvert[VK_HOME      ]=KeyboardButton::Home;
            KeyConvert[VK_END       ]=KeyboardButton::End;
            KeyConvert[VK_PRIOR     ]=KeyboardButton::PageUp;
            KeyConvert[VK_NEXT      ]=KeyboardButton::PageDown;

            KeyConvert[VK_NUMLOCK   ]=KeyboardButton::NumLock;
            KeyConvert[VK_SCROLL    ]=KeyboardButton::ScrollLock;

            //KeyConvert[VK_SHIFT       ]=kbLeftShift;
            //KeyConvert[VK_CONTROL ]=kbLeftCtrl;
            //KeyConvert[VK_MENU        ]=kbLeftAlt;

            KeyConvert[VK_OEM_1     ]=KeyboardButton::Semicolon;
            KeyConvert[VK_OEM_PLUS  ]=KeyboardButton::Equals;
            KeyConvert[VK_OEM_COMMA ]=KeyboardButton::Comma;
            KeyConvert[VK_OEM_MINUS ]=KeyboardButton::Minus;
            KeyConvert[VK_OEM_PERIOD]=KeyboardButton::Period;
            KeyConvert[VK_OEM_2     ]=KeyboardButton::Slash;
            KeyConvert[VK_OEM_3     ]=KeyboardButton::Grave;
            KeyConvert[VK_OEM_4     ]=KeyboardButton::LeftBracket;
            KeyConvert[VK_OEM_5     ]=KeyboardButton::BackSlash;
            KeyConvert[VK_OEM_6     ]=KeyboardButton::RightBracket;
            KeyConvert[VK_OEM_7     ]=KeyboardButton::Apostrophe;
        }

        KeyboardButton ConvertOSKey(uint key)
        {
            if(key>=256)return(KeyboardButton::NONE);
            if(KeyConvert[key]==KeyboardButton::NONE)return(KeyboardButton::NONE);

            if(key==VK_SHIFT)
            {
                if((GetAsyncKeyState(VK_LSHIFT)>>15)&1)
                    return KeyboardButton::LeftShift;
                else
                    return KeyboardButton::RightShift;
            }
            else
            if(key==VK_CONTROL)
            {
                if((GetAsyncKeyState(VK_LCONTROL)>>15)&1)
                    return KeyboardButton::LeftCtrl;
                else
                    return KeyboardButton::RightCtrl;
            }
            if(key==VK_MENU)
            {
                if((GetAsyncKeyState(VK_LMENU)>>15)&1)
                    return KeyboardButton::LeftAlt;
                else
                    return KeyboardButton::RightAlt;
            }

    #ifdef _DEBUG
            if(KeyConvert[key]==KeyboardButton::NONE)
            {
                wchar_t name[64];

                ::GetKeyNameTextW(key,name,64);

                GLogVerbose( WideString(L"Unknow Key: " )+WideString::numberOf(key)
                            +WideString(L" ,name: "     )+WideString(name));
            }
    #endif _DEBUG

            return KeyConvert[key];
        }
        
        static EventHeader event_header;
        static WindowEventData window_event_data;

        void WMProcDestroy(EventDispatcher *ie,uint32,uint32)
        {
            event_header.type   =InputEventSource::Window;
            event_header.index  =0;
            event_header.id     =(uint16)WindowEventID::Close;

            ie->OnEvent(event_header,window_event_data.data);
            PostQuitMessage(0);
        }

        static MouseEventData mouse_event_data;

        #define WMEF_MOUSE(mouse_button,action)   void WMProcMouse##mouse_button##action(EventDispatcher *ie,uint32 wParam,uint32 lParam)    \
            {   \
                mouse_event_data.x=LOWORD(lParam); \
                mouse_event_data.y=HIWORD(lParam); \
                mouse_event_data.button=(uint8)MouseButton::mouse_button;    \
                \
                event_header.type   =InputEventSource::Mouse;  \
                event_header.index  =0;   \
                event_header.id     =(uint16)MouseEventID::action; \
                \
                ie->OnEvent(event_header,mouse_event_data.data);    \
            }

            WMEF_MOUSE(Left,Pressed);
            WMEF_MOUSE(Left,Released);
            WMEF_MOUSE(Left,DblClicked);

            WMEF_MOUSE(Mid,Pressed);
            WMEF_MOUSE(Mid,Released);
            WMEF_MOUSE(Mid,DblClicked);

            WMEF_MOUSE(Right,Pressed);
            WMEF_MOUSE(Right,Released);
            WMEF_MOUSE(Right,DblClicked);

            void WMProcMouseMove(EventDispatcher *ie,uint32 wParam,uint32 lParam)
            {
                mouse_event_data.x=LOWORD(lParam);
                mouse_event_data.y=HIWORD(lParam);
                mouse_event_data.button=0;
                
                event_header.type   =InputEventSource::Mouse; 
                event_header.index  =0;
                event_header.id     =(uint16)MouseEventID::Move;
                
                ie->OnEvent(event_header,mouse_event_data.data);
            }
        #undef WMEF_MOUSE

        #define WMEF2(name) void name(EventDispatcher *ie,uint32 wParam,uint32 lParam)
            WMEF2(WMProcMouseWheel)
            {
                const int zDelta=GET_WHEEL_DELTA_WPARAM(wParam);
                //const uint key=(uint)ConvertOSKey(GET_KEYSTATE_WPARAM(wParam));
                
                mouse_event_data.x=0;
                mouse_event_data.y=zDelta;
                mouse_event_data.button=0;

                event_header.type   =InputEventSource::Mouse; 
                event_header.index  =0;   
                event_header.id     =(uint16)MouseEventID::Wheel;
                
                ie->OnEvent(event_header,mouse_event_data.data);
            }

            WMEF2(WMProcMouseHWheel)
            {
                const int zDelta=GET_WHEEL_DELTA_WPARAM(wParam);
                //const uint key=(uint)ConvertOSKey(GET_KEYSTATE_WPARAM(wParam));
                
                mouse_event_data.x=zDelta;
                mouse_event_data.y=0;
                mouse_event_data.button=0;

                event_header.type   =InputEventSource::Mouse; 
                event_header.index  =0;   
                event_header.id     =(uint16)MouseEventID::Wheel;
                
                ie->OnEvent(event_header,mouse_event_data.data);
            }

            WMEF2(WMProcSize)
            {
                event_header.type   =InputEventSource::Window;
                event_header.index  =0;
                event_header.id     =(uint16)WindowEventID::Resize;

                window_event_data.width =LOWORD(lParam);
                window_event_data.height=HIWORD(lParam);

                ie->OnEvent(event_header,window_event_data.data);
            }
        #undef WMEF2

            static PointerEventData pointer_event_data;

        #define WMEF_POINTER(name) void name(EventDispatcher *ie,uint32 wParam,uint32 lParam)
            WMEF_POINTER(WMProcPointerDown)
            {
                UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
                POINTER_INFO pointerInfo;
                
                if(GetPointerInfo(pointerId, &pointerInfo))
                {
                    pointer_event_data.x = pointerInfo.ptPixelLocation.x;
                    pointer_event_data.y = pointerInfo.ptPixelLocation.y;
                    
                    // 设置设备类型
                    switch(pointerInfo.pointerType)
                    {
                        case PT_POINTER:    pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                        case PT_TOUCH:      pointer_event_data.device_type = (uint8)PointerDeviceType::Touch; break;
                        case PT_PEN:        pointer_event_data.device_type = (uint8)PointerDeviceType::Pen; break;
                        case PT_MOUSE:      pointer_event_data.device_type = (uint8)PointerDeviceType::Mouse; break;
                        default:            pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                    }
                    
                    // 设置按钮状态
                    if(pointerInfo.pointerFlags & POINTER_FLAG_FIRSTBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Primary;
                    else if(pointerInfo.pointerFlags & POINTER_FLAG_SECONDBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Secondary;
                    else if(pointerInfo.pointerFlags & POINTER_FLAG_THIRDBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Tertiary;
                    else
                        pointer_event_data.button = (uint8)PointerButton::None;
                    
                    // 如果是触控笔，获取压力值
                    pointer_event_data.pressure = 0;
                    if(pointerInfo.pointerType == PT_PEN)
                    {
                        POINTER_PEN_INFO penInfo;
                        if(GetPointerPenInfo(pointerId, &penInfo))
                        {
                            pointer_event_data.pressure = penInfo.pressure;
                        }
                    }
                    
                    event_header.type   = InputEventSource::Pointer;
                    event_header.index  = 0;
                    event_header.id     = (uint16)PointerEventID::Down;
                    
                    ie->OnEvent(event_header, pointer_event_data.data);
                }
            }

            WMEF_POINTER(WMProcPointerUp)
            {
                UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
                POINTER_INFO pointerInfo;
                
                if(GetPointerInfo(pointerId, &pointerInfo))
                {
                    pointer_event_data.x = pointerInfo.ptPixelLocation.x;
                    pointer_event_data.y = pointerInfo.ptPixelLocation.y;
                    
                    switch(pointerInfo.pointerType)
                    {
                        case PT_POINTER:    pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                        case PT_TOUCH:      pointer_event_data.device_type = (uint8)PointerDeviceType::Touch; break;
                        case PT_PEN:        pointer_event_data.device_type = (uint8)PointerDeviceType::Pen; break;
                        case PT_MOUSE:      pointer_event_data.device_type = (uint8)PointerDeviceType::Mouse; break;
                        default:            pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                    }
                    
                    if(pointerInfo.pointerFlags & POINTER_FLAG_FIRSTBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Primary;
                    else if(pointerInfo.pointerFlags & POINTER_FLAG_SECONDBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Secondary;
                    else if(pointerInfo.pointerFlags & POINTER_FLAG_THIRDBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Tertiary;
                    else
                        pointer_event_data.button = (uint8)PointerButton::None;
                    
                    pointer_event_data.pressure = 0;
                    if(pointerInfo.pointerType == PT_PEN)
                    {
                        POINTER_PEN_INFO penInfo;
                        if(GetPointerPenInfo(pointerId, &penInfo))
                        {
                            pointer_event_data.pressure = penInfo.pressure;
                        }
                    }
                    
                    event_header.type   = InputEventSource::Pointer;
                    event_header.index  = 0;
                    event_header.id     = (uint16)PointerEventID::Up;
                    
                    ie->OnEvent(event_header, pointer_event_data.data);
                }
            }

            WMEF_POINTER(WMProcPointerUpdate)
            {
                UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
                POINTER_INFO pointerInfo;
                
                if(GetPointerInfo(pointerId, &pointerInfo))
                {
                    pointer_event_data.x = pointerInfo.ptPixelLocation.x;
                    pointer_event_data.y = pointerInfo.ptPixelLocation.y;
                    
                    switch(pointerInfo.pointerType)
                    {
                        case PT_POINTER:    pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                        case PT_TOUCH:      pointer_event_data.device_type = (uint8)PointerDeviceType::Touch; break;
                        case PT_PEN:        pointer_event_data.device_type = (uint8)PointerDeviceType::Pen; break;
                        case PT_MOUSE:      pointer_event_data.device_type = (uint8)PointerDeviceType::Mouse; break;
                        default:            pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                    }
                    
                    if(pointerInfo.pointerFlags & POINTER_FLAG_FIRSTBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Primary;
                    else if(pointerInfo.pointerFlags & POINTER_FLAG_SECONDBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Secondary;
                    else if(pointerInfo.pointerFlags & POINTER_FLAG_THIRDBUTTON)
                        pointer_event_data.button = (uint8)PointerButton::Tertiary;
                    else
                        pointer_event_data.button = (uint8)PointerButton::None;
                    
                    pointer_event_data.pressure = 0;
                    if(pointerInfo.pointerType == PT_PEN)
                    {
                        POINTER_PEN_INFO penInfo;
                        if(GetPointerPenInfo(pointerId, &penInfo))
                        {
                            pointer_event_data.pressure = penInfo.pressure;
                        }
                    }
                    
                    event_header.type   = InputEventSource::Pointer;
                    event_header.index  = 0;
                    event_header.id     = (uint16)PointerEventID::Update;
                    
                    ie->OnEvent(event_header, pointer_event_data.data);
                }
            }

            WMEF_POINTER(WMProcPointerEnter)
            {
                UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
                POINTER_INFO pointerInfo;
                
                if(GetPointerInfo(pointerId, &pointerInfo))
                {
                    pointer_event_data.x = pointerInfo.ptPixelLocation.x;
                    pointer_event_data.y = pointerInfo.ptPixelLocation.y;
                    
                    switch(pointerInfo.pointerType)
                    {
                        case PT_POINTER:    pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                        case PT_TOUCH:      pointer_event_data.device_type = (uint8)PointerDeviceType::Touch; break;
                        case PT_PEN:        pointer_event_data.device_type = (uint8)PointerDeviceType::Pen; break;
                        case PT_MOUSE:      pointer_event_data.device_type = (uint8)PointerDeviceType::Mouse; break;
                        default:            pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                    }
                    
                    pointer_event_data.button = (uint8)PointerButton::None;
                    pointer_event_data.pressure = 0;
                    
                    event_header.type   = InputEventSource::Pointer;
                    event_header.index  = 0;
                    event_header.id     = (uint16)PointerEventID::Enter;
                    
                    ie->OnEvent(event_header, pointer_event_data.data);
                }
            }

            WMEF_POINTER(WMProcPointerLeave)
            {
                UINT32 pointerId = GET_POINTERID_WPARAM(wParam);
                POINTER_INFO pointerInfo;
                
                if(GetPointerInfo(pointerId, &pointerInfo))
                {
                    pointer_event_data.x = pointerInfo.ptPixelLocation.x;
                    pointer_event_data.y = pointerInfo.ptPixelLocation.y;
                    
                    switch(pointerInfo.pointerType)
                    {
                        case PT_POINTER:    pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                        case PT_TOUCH:      pointer_event_data.device_type = (uint8)PointerDeviceType::Touch; break;
                        case PT_PEN:        pointer_event_data.device_type = (uint8)PointerDeviceType::Pen; break;
                        case PT_MOUSE:      pointer_event_data.device_type = (uint8)PointerDeviceType::Mouse; break;
                        default:            pointer_event_data.device_type = (uint8)PointerDeviceType::None; break;
                    }
                    
                    pointer_event_data.button = (uint8)PointerButton::None;
                    pointer_event_data.pressure = 0;
                    
                    event_header.type   = InputEventSource::Pointer;
                    event_header.index  = 0;
                    event_header.id     = (uint16)PointerEventID::Leave;
                    
                    ie->OnEvent(event_header, pointer_event_data.data);
                }
            }
        #undef WMEF_POINTER

            static KeyboardEventData keyboard_event_data;

        #define WMEF1(name) void name(EventDispatcher *ie,uint32 wParam,uint32)
            WMEF1(WMProcKeyDown)
            {
                event_header.type   =InputEventSource::Keyboard;
                event_header.index  =0;   
                event_header.id     =(uint16)KeyboardEventID::Pressed;

                keyboard_event_data.key=(uint32)ConvertOSKey(wParam);

                ie->OnEvent(event_header,keyboard_event_data.data);
            }

            WMEF1(WMProcKeyUp)
            {
                event_header.type   =InputEventSource::Keyboard;
                event_header.index  =0;   
                event_header.id     =(uint16)KeyboardEventID::Released;

                keyboard_event_data.key=(uint32)ConvertOSKey(wParam);

                ie->OnEvent(event_header,keyboard_event_data.data);
            }

            WMEF1(WMProcChar)
            {
                event_header.type   =InputEventSource::Keyboard;
                event_header.index  =0;   
                event_header.id     =(uint16)KeyboardEventID::Char;

                keyboard_event_data.ch=(wchar_t)wParam;

                ie->OnEvent(event_header,keyboard_event_data.data);
            }

            WMEF1(WMProcActive)
            {
                //if(JoyPlugIn)
                //    JoyInterface.SetInputActive(wParam);

                event_header.type   =InputEventSource::Window;
                event_header.index  =0;
                event_header.id     =(uint16)WindowEventID::Active;

                window_event_data.active=wParam;

                ie->OnEvent(event_header,window_event_data.data);
            }
        #undef WMEF1
    }//namespace

    void InitMessageProc()
    {
        InitKeyConvert();

        //if(joy)
        //    LoadJoystick(win->hInstance,win->hWnd);

    #define WM_MAP(wm,func) WMProc[wm]=func;

        WM_MAP(WM_CLOSE             ,WMProcDestroy);
        WM_MAP(WM_LBUTTONDOWN       ,WMProcMouseLeftPressed);
        WM_MAP(WM_LBUTTONUP         ,WMProcMouseLeftReleased);
        WM_MAP(WM_LBUTTONDBLCLK     ,WMProcMouseLeftDblClicked);
        WM_MAP(WM_MBUTTONDOWN       ,WMProcMouseMidPressed);
        WM_MAP(WM_MBUTTONUP         ,WMProcMouseMidReleased);
        WM_MAP(WM_MBUTTONDBLCLK     ,WMProcMouseMidDblClicked);
        WM_MAP(WM_RBUTTONDOWN       ,WMProcMouseRightPressed);
        WM_MAP(WM_RBUTTONUP         ,WMProcMouseRightReleased);
        WM_MAP(WM_RBUTTONDBLCLK     ,WMProcMouseRightDblClicked);
        WM_MAP(WM_MOUSEWHEEL        ,WMProcMouseWheel);
        WM_MAP(WM_MOUSEHWHEEL       ,WMProcMouseHWheel);
        WM_MAP(WM_MOUSEMOVE         ,WMProcMouseMove);
        WM_MAP(WM_KEYDOWN           ,WMProcKeyDown);
        WM_MAP(WM_KEYUP             ,WMProcKeyUp);
        WM_MAP(WM_SYSKEYDOWN        ,WMProcKeyDown);
        WM_MAP(WM_SYSKEYUP          ,WMProcKeyUp);
        WM_MAP(WM_CHAR              ,WMProcChar);
        WM_MAP(WM_SYSCHAR           ,WMProcChar);
        WM_MAP(WM_ACTIVATE          ,WMProcActive);
        WM_MAP(WM_SIZE              ,WMProcSize);
        WM_MAP(WM_POINTERDOWN       ,WMProcPointerDown);
        WM_MAP(WM_POINTERUP         ,WMProcPointerUp);
        WM_MAP(WM_POINTERUPDATE     ,WMProcPointerUpdate);
        WM_MAP(WM_POINTERENTER      ,WMProcPointerEnter);
        WM_MAP(WM_POINTERLEAVE      ,WMProcPointerLeave);

    #undef WM_MAP
    }

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if(uMsg<2048)
            if(WMProc[uMsg])
            {
                EventDispatcher *ie=(EventDispatcher *)GetWindowLongPtrW(hWnd,GWLP_USERDATA);

                if(ie)
                    WMProc[uMsg](ie,wParam,lParam);
            }

        return (DefWindowProcW(hWnd, uMsg, wParam, lParam));
    }

    void InitNativeWindowSystem()
    {
        InitMessageProc();
    }
}//namespace hgl
