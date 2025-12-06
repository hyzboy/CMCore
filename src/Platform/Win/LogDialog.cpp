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
                if (!msg||!msg->message||msg->message_length<=0)return;

                MessageBoxW(nullptr,msg->message,name,MB_OK);
            }
        };//class LogWinDialog

        Logger *CreateLoggerDialog(const OSString &n,LogLevel ll)
        {
            return(new LogWinDialog(n,ll));
        }
    }//namespace logger
}//namespace hgl
