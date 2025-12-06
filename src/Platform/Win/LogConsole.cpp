#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/Charset.h>
#include<windows.h>
#include <string>

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

            u16char buf[LOG_BUF_SIZE]{};

            bool EnableVirtualTerminal()
            {
                DWORD mode;
                if (!GetConsoleMode(static_cast<HANDLE>(console_handle), &mode))
                    return false;

                mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                return SetConsoleMode(static_cast<HANDLE>(console_handle), mode);
            }

            std::string GetAnsiColorByLogLevel(LogLevel level)
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

        public:

            LogWinConsole(LogLevel ll):Logger(ll)
            {
                console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
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
                CloseHandle(console_handle);
            }

            void Write(const LogMessage *msg) override
            {
                if (!msg||!msg->message||msg->message_length<=0)return;

                const std::string color = GetAnsiColorByLogLevel(msg->level);

                WriteConsoleA(console_handle, color.c_str(), (DWORD)color.size(), &result, nullptr);
                WriteConsoleW(console_handle, msg->message, msg->message_length, &result, nullptr);
                WriteConsoleW(console_handle, L"\n", 1, &result, nullptr);
                WriteConsoleA(console_handle, "\033[0m", 4, &result, nullptr); // 重置颜色
            }
        }; // class LogWinConsole

        Logger *CreateLoggerConsole(LogLevel ll)
        {
            return(new LogWinConsole(ll));
        }
    } // namespace logger
} // namespace hgl
