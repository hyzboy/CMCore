#ifndef HGL_IO_KEYBOARD_EVENT_INCLUDE
#define HGL_IO_KEYBOARD_EVENT_INCLUDE

#include<hgl/io/event/InputEvent.h>
namespace hgl
{
    namespace io
    {
        /**
        * ����ö�ٶ���
        */
        enum class KeyboardButton
        {
            NONE=0,
                                //��������
            Esc,                ///<ESC

                                //F���ܼ�
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

            Grave,              //<`��(���������ּ�1��ߵİ�ť)

                                //10������
            _0,                 ///<���ּ�0
            _1,                 ///<���ּ�1
            _2,                 ///<���ּ�2
            _3,                 ///<���ּ�3
            _4,                 ///<���ּ�4
            _5,                 ///<���ּ�5
            _6,                 ///<���ּ�6
            _7,                 ///<���ּ�7
            _8,                 ///<���ּ�8
            _9,                 ///<���ּ�9

            Minus,              ///< - (����)
            Equals,             ///< = (�Ⱥ�)
            BackSlash,          ///< \ (��б��)
            BackSpace,          ///< �˸��

            Tab,                ///<Tab��

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

            CapsLock,           ///<��д������

            Semicolon,          ///<; (�ֺ�)
            Apostrophe,         ///<' (������)
            Enter,              ///<�س���

            LeftShift,          ///<��ߵ�Shift��

            Comma,              ///<, (����)
            Period,             ///<. (���)
            Slash,              ///</ (����)
            RightShift,         ///<�ұߵ�Shift��

            LeftCtrl,           ///<��ߵ�Ctrl��
            LeftOS,             ///<��ߵ�OS��(Win/Apple��)
            LeftAlt,            ///<��ߵ�Alt��
            Space,              ///<�ո��
            RightAlt,           ///<�ұߵ�Alt��
            RightOS,            ///<�ұߵ�OS��(Win/Apple��)
            RightMenu,          ///<�ұߵ�Menu��
            RightCtrl,          ///<�ұߵ�Ctrl��

                                //�м�����
            PrintScreen,        ///<��ӡ��Ļ��
            ScrollLock,         ///<����������
            Pause,              ///<��ͣ��

            Insert,             ///<�����
            Delete,             ///<ɾ����
            Home,               ///<���׼�
            End,                ///<��β��
            PageUp,             ///<��ǰ��ҳ��
            PageDown,           ///<���ҳ��

            Up,                 ///<������
            Down,               ///<������
            Left,               ///<������
            Right,              ///<������

                                //С������
            NumLock,            ///<С���� ����������

            NumAdd,             ///<С���� +
            NumSubtract,        ///<С���� -
            NumMultiply,        ///<С���� *
            NumDivide,          ///<С���� /

            Num0,               ///<С���� 0
            Num1,               ///<С���� 1
            Num2,               ///<С���� 2
            Num3,               ///<С���� 3
            Num4,               ///<С���� 4
            Num5,               ///<С���� 5
            Num6,               ///<С���� 6
            Num7,               ///<С���� 7
            Num8,               ///<С���� 8
            Num9,               ///<С���� 9

            NumDecimal,         ///<С���� . (С����/ɾ����)
            NumEnter,           ///<С���� �س���

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

        class KeyboardEvent:public InputEvent
        {
        public:

            KeyboardEvent():InputEvent(InputEventSource::Keyboard){}
            virtual ~KeyboardEvent()=default;

            EventProcResult OnEvent(const EventHeader &header,const uint64 data) override
            {
                if(InputEvent::OnEvent(header,data)==EventProcResult::Break)
                    return EventProcResult::Break;

                switch(KeyboardEventID(header.id))
                {
                    case KeyboardEventID::Pressed:  if(OnPressed    (KeyboardButton(((KeyboardEventData *)&data)->key)))return EventProcResult::Break;break;
                    case KeyboardEventID::Released: if(OnReleased   (KeyboardButton(((KeyboardEventData *)&data)->key)))return EventProcResult::Break;break;
                    case KeyboardEventID::Char:     if(OnChar       (               ((KeyboardEventData *)&data)->ch ) )return EventProcResult::Break;break;
                }

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
             * ĳ�����Ƿ���״̬
             */
            const bool HasPressed(const KeyboardButton &kb)const
            {
                if(!RangeCheck(kb))return(false);

                return press_states[size_t(kb)];
            }

            /**
             * �趨��ǰʱ��
             */
            void SetCurTime(const double &ct)
            {
                cur_time=ct;
            }

            /**
             * ��ȡ�������µ�ʱ�� 
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
