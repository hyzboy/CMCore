#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/Charset.h>
#include<windows.h>

namespace hgl
{
    namespace logger
    {
        class LogWinDialog:public Logger
        {
            U16String name;

            u16char buf[4096]{};

        public:

            LogWinDialog(const OSString &n,LogLevel ll):Logger(ll)
            {
                name=n;
            }

            bool Create(const U16String &)
            {
                return(true);
            }

            void Close() override
            {
            }

            void Write(const LogMessage *msg) override
            {
                if(!msg)
                    return;

                const u16char *text=msg->text.message_u16;
                int text_length=msg->text.message_u16_length;

                if(!text||text_length<=0)
                {
                    if(msg->text.message_u8&&msg->text.message_u8_length>0)
                    {
                        const U16String wide=ToOSString(U8String(msg->text.message_u8,msg->text.message_u8_length));

                        text=wide.c_str();
                        text_length=wide.Length();
                    }
                }

                if(!text||text_length<=0)
                    return;

                MessageBoxW(nullptr,text,name,MB_OK);
            }
        };//class LogWinDialog

        Logger *CreateLoggerDialog(const OSString &n,LogLevel ll)
        {
            return(new LogWinDialog(n,ll));
        }
    }//namespace logger
}//namespace hgl
