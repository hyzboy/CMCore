#ifndef HGL_IO_KEYBOARD_EVENT_INCLUDE
#define HGL_IO_KEYBOARD_EVENT_INCLUDE

#include<hgl/io/event/EventDispatcher.h>
namespace hgl
{
    namespace io
    {
        /**
        * 按键枚举定义
        */
        enum class KeyboardButton
        {
            NONE=0,
                                //主键盘区
            Esc,                ///<ESC

                                //F功能键
            F1,                 ///<F1
            F2,                 ///<F2
            F3,                 ///<F3
            F4,                 ///<F4
            F5,                 ///<F5
            F6,                 ///<F6
            F7,                 ///<F7
            F8,                 ///<F8
            F9,                 ///<F9
            F10,                ///<F10
            F11,                ///<F11
            F12,                ///<F12

            Grave,              //<`号(主键盘数字键1左边的按钮)

                                //10个数字
            _0,                 ///<数字键0
            _1,                 ///<数字键1
            _2,                 ///<数字键2
            _3,                 ///<数字键3
            _4,                 ///<数字键4
            _5,                 ///<数字键5
            _6,                 ///<数字键6
            _7,                 ///<数字键7
            _8,                 ///<数字键8
            _9,                 ///<数字键9

            Minus,              ///< - (减号)
            Equals,             ///< = (等号)
            BackSlash,          ///< \ (反斜杠)
            BackSpace,          ///< 退格键

            Tab,                ///<Tab键

            A,                  ///<A
            B,                  ///<B
            C,                  ///<C
            D,                  ///<D
            E,                  ///<E
            F,                  ///<F
            G,                  ///<G
            H,                  ///<H
            I,                  ///<I
            J,                  ///<J
            K,                  ///<K
            L,                  ///<L
            M,                  ///<M
            N,                  ///<N
            O,                  ///<O
            P,                  ///<P
            Q,                  ///<Q
            R,                  ///<R
            S,                  ///<S
            T,                  ///<T
            U,                  ///<U
            V,                  ///<V
            W,                  ///<W
            X,                  ///<X
            Y,                  ///<Y
            Z,                  ///<Z

            LeftBracket,        ///<[
            RightBracket,       ///<]

            CapsLock,           ///<大写锁定键

            Semicolon,          ///<; (分号)
            Apostrophe,         ///<' (单引号)
            Enter,              ///<回车键

            LeftShift,          ///<左边的Shift键

            Comma,              ///<, (逗号)
            Period,             ///<. (句号)
            Slash,              ///</ (除号)
            RightShift,         ///<右边的Shift键

            LeftCtrl,           ///<左边的Ctrl键
            LeftOS,             ///<左边的OS键(Win/Apple键)
            LeftAlt,            ///<左边的Alt键
            Space,              ///<空格键
            RightAlt,           ///<右边的Alt键
            RightOS,            ///<右边的OS键(Win/Apple键)
            RightMenu,          ///<右边的Menu键
            RightCtrl,          ///<右边的Ctrl键

                                //中键盘区
            PrintScreen,        ///<打印屏幕键
            ScrollLock,         ///<滚动锁定键
            Pause,              ///<暂停键

            Insert,             ///<插入键
            Delete,             ///<删除键
            Home,               ///<行首键
            End,                ///<行尾键
            PageUp,             ///<向前翻页键
            PageDown,           ///<向后翻页键

            Up,                 ///<↑光标键
            Down,               ///<↓光标键
            Left,               ///<←光标键
            Right,              ///<→光标键

                                //小键盘区
            NumLock,            ///<小键盘 数字锁定键

            NumAdd,             ///<小键盘 +
            NumSubtract,        ///<小键盘 -
            NumMultiply,        ///<小键盘 *
            NumDivide,          ///<小键盘 /

            Num0,               ///<小键盘 0
            Num1,               ///<小键盘 1
            Num2,               ///<小键盘 2
            Num3,               ///<小键盘 3
            Num4,               ///<小键盘 4
            Num5,               ///<小键盘 5
            Num6,               ///<小键盘 6
            Num7,               ///<小键盘 7
            Num8,               ///<小键盘 8
            Num9,               ///<小键盘 9

            NumDecimal,         ///<小键盘 . (小数点/删除键)
            NumEnter,           ///<小键盘 回车键

            ENUM_CLASS_RANGE(NONE,NumEnter)
        };//enum KeyboardButton

        enum class KeyboardEventID
        {
            Pressed=0,
            Released,
            Char
        };

        union KeyboardEventData
        {
            uint64 data;
            uint32 key;
            os_char ch;
        };

        class KeyboardEvent:public EventDispatcher
        {
        public:

            KeyboardEvent():EventDispatcher(InputEventSource::Keyboard){}
            virtual ~KeyboardEvent()=default;

            EventProcResult OnEvent(const EventHeader &header,const uint64 data) override
            {
                if(header.type==InputEventSource::Keyboard)
                {
                    switch(KeyboardEventID(header.id))
                    {
                        case KeyboardEventID::Pressed:  if(OnPressed    (KeyboardButton(((KeyboardEventData *)&data)->key)))return EventProcResult::Break;break;
                        case KeyboardEventID::Released: if(OnReleased   (KeyboardButton(((KeyboardEventData *)&data)->key)))return EventProcResult::Break;break;
                        case KeyboardEventID::Char:     if(OnChar       (               ((KeyboardEventData *)&data)->ch ) )return EventProcResult::Break;break;
                    }
                }

                if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
                    return EventProcResult::Break;

                return EventProcResult::Continue;
            }

            virtual bool OnPressed(const KeyboardButton &kb){return(false);}
            virtual bool OnReleased(const KeyboardButton &kb){return(false);}
            virtual bool OnChar(const wchar_t &){return(false);}
        };//class KeyboardEvent

        class KeyboardStateEvent:public KeyboardEvent
        {
        protected:

            double cur_time;
            bool press_states[size_t(KeyboardButton::RANGE_SIZE)];
            double pressed_time[size_t(KeyboardButton::RANGE_SIZE)];

        public:

            KeyboardStateEvent():KeyboardEvent()
            {
                cur_time=0;
                hgl_zero(press_states);
                hgl_zero(pressed_time);
            }

            virtual ~KeyboardStateEvent()=default;

            virtual bool OnPressed(const KeyboardButton &kb) override
            {
                if(!RangeCheck(kb))return(false);

                press_states[(size_t)kb]=true;
                pressed_time[(size_t)kb]=cur_time;

                return(true);
            }

            virtual bool OnReleased(const KeyboardButton &kb) override
            {
                if(!RangeCheck(kb))return(false);

                press_states[(size_t)kb]=false;                
                pressed_time[(size_t)kb]=0;

                return(true);
            }

        public:

            /**
             * 某按键是否按下状态
             */
            const bool HasPressed(const KeyboardButton &kb)const
            {
                if(!RangeCheck(kb))return(false);

                return press_states[size_t(kb)];
            }

            /**
             * 设定当前时间
             */
            void SetCurTime(const double &ct)
            {
                cur_time=ct;
            }

            /**
             * 获取按键按下的时间 
             */
            const double GetPressedTime(const KeyboardButton &kb)const
            {   
                if(!RangeCheck(kb))return(0);

                return pressed_time[size_t(kb)];
            }

            const bool ShiftPressed()const
            {
                return press_states[size_t(KeyboardButton::LeftShift)]
                     ||press_states[size_t(KeyboardButton::RightShift)];
            }

            const bool CtrlPressed()const
            {
                return press_states[size_t(KeyboardButton::LeftCtrl)]
                     ||press_states[size_t(KeyboardButton::RightCtrl)];
            }

            const bool AltPressed()const
            {
                return press_states[size_t(KeyboardButton::LeftAlt)]
                     ||press_states[size_t(KeyboardButton::RightAlt)];
            }
        };//class KeyboardStateEvent
    }//namespace io
}//namespace hgl
#endif//HGL_IO_KEYBOARD_EVENT_INCLUDE
