#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/Charset.h>
#include<hgl/utf.h>
#include<windows.h>

namespace hgl
{
    namespace logger
    {
        constexpr uint LOG_BUF_SIZE=4096;

        class LogWinConsole:public Logger
        {
        private:

            DWORD result=0;

            void *console_handle=nullptr;

            bool is_console=false;                  ///<是否为真正的控制台(而非重定向的管道/文件)

            u8char u8_buf[LOG_BUF_SIZE];            ///<管道/文件模式的UTF-8输出缓冲

            bool EnableVirtualTerminal()
            {
                DWORD mode;
                if (!GetConsoleMode(static_cast<HANDLE>(console_handle), &mode))
                    return false;

                mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                return SetConsoleMode(static_cast<HANDLE>(console_handle), mode);
            }

            const char *GetAnsiColorByLogLevel(LogLevel level)
            {
                switch (level)
                {
                    case LogLevel::Fatal:   return "\033[1;31m";  // 高亮红色
                    case LogLevel::Error:   return "\033[31m";    // 红色
                    case LogLevel::Warning: return "\033[33m";    // 黄色
                    case LogLevel::Notice:  return "\033[32m";    // 绿色
                    case LogLevel::Info:    return "\033[36m";    // 青色
                    case LogLevel::Debug:   return "\033[34m";    // 蓝色
                    case LogLevel::Verbose:
                    default:                return "\033[37m";    // 白色
                }
            }

            void WriteToConsole(const u16char *text,int text_length,LogLevel level)
            {
                const char *color=GetAnsiColorByLogLevel(level);

                WriteConsoleA(console_handle, color, (DWORD)strlen(color), &result, nullptr);
                WriteConsoleW(console_handle, text, text_length, &result, nullptr);
                WriteConsoleW(console_handle, L"\n", 1, &result, nullptr);
                WriteConsoleA(console_handle, "\033[0m", 4, &result, nullptr); // 重置颜色
            }

            void WriteToPipe(const LogMessage *msg)
            {
                int len=0;

                if(msg->text.message_u8&&msg->text.message_u8_length>0)
                {
                    int copy_len=msg->text.message_u8_length;

                    if(copy_len>LOG_BUF_SIZE-2)
                        copy_len=LOG_BUF_SIZE-2;

                    memcpy(u8_buf,msg->text.message_u8,copy_len);
                    len=copy_len;
                }
                else if(msg->text.message_u16&&msg->text.message_u16_length>0)
                {
                    len=u16_to_u8(u8_buf,LOG_BUF_SIZE-1,msg->text.message_u16,msg->text.message_u16_length);
                }

                if(len<=0)
                    return;

                u8_buf[len]='\n';

                WriteFile(static_cast<HANDLE>(console_handle),u8_buf,len+1,&result,nullptr);
            }

        public:

            LogWinConsole(LogLevel ll):Logger(ll)
            {
                SetOutputterName(OS_TEXT("Console"));
                console_handle=GetStdHandle(STD_OUTPUT_HANDLE);

                DWORD mode;
                is_console=(GetConsoleMode(static_cast<HANDLE>(console_handle),&mode)!=0);

                if(is_console)
                    EnableVirtualTerminal(); // 启用虚拟终端模式
            }

            bool Create(const U16String &)
            {
                return(true);
            }

            ~LogWinConsole()
            {
                Close();
            }

            void Close() override
            {
                console_handle=nullptr;
            }

            void Write(const LogMessage *msg) override
            {
                if(!msg)
                    return;

                if(is_console)
                {
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

                    WriteToConsole(text,text_length,msg->meta.level);
                }
                else
                {
                    WriteToPipe(msg);
                }
            }
        }; // class LogWinConsole

        Logger *CreateLoggerConsole(LogLevel ll)
        {
            return(new LogWinConsole(ll));
        }
    } // namespace logger
} // namespace hgl
