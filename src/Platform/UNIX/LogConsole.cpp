#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/CodePage.h>
#include<hgl/thread/ThreadMutex.h>
#include<unistd.h>
#include<pthread.h>

#ifdef LOG_INFO_TIME
#include<hgl/Time.h>
#endif//LOG_INFO_TIME

namespace hgl
{
    namespace logger
    {
        constexpr uint LOG_BUF_SIZE=4096;

        /**
        * unix控制台日志插件接口
        */
        class LogUnixConsole:public Logger
        {
            char log_buf[LOG_BUF_SIZE];

#ifdef LOGINFO_THREAD_MUTEX
            ThreadMutex mutex;
#endif//LOGINFO_THREAD_MUTEX

        public:

            LogUnixConsole(LogLevel ll):Logger(ll)
            {
            }

            bool Create(const OSString &)
            {
                return(true);
            }

            void Close(){}

#ifdef LOG_INFO_THREAD
            void WriteThreadID()
            {
                memcpy(log_buf,"[Thread:",8);

                htos(log_buf+8,128-9,pthread_self());
                strcat(log_buf,LOG_BUF_SIZE,']');

                write(STDOUT_FILENO,log_buf,strlen(log_buf));
            }
#endif//LOG_INFO_THREAD

#ifdef LOG_INFO_TIME
            void WriteTime()
            {
                memcpy(log_buf,"[Time:",6);

                ftos(log_buf+6,128-strlen(log_buf),GetDoubleTime());
                strcat(log_buf,LOG_BUF_SIZE,']');

                write(STDOUT_FILENO,log_buf,strlen(log_buf));
            }
#endif//LOG_INFO_TIME

            void Write(const LogMessage *msg) override
            {
                if(!msg||!msg->message||msg->message_length<=0)
                    return;

            #ifdef LOGINFO_THREAD_MUTEX
                mutex.Lock();
            #endif//LOGINFO_THREAD_MUTEX

                #ifdef LOG_INFO_THREAD
                    WriteThreadID();
                #endif//LOG_INFO_THREAD

                #ifdef LOG_INFO_TIME
                    WriteTime();
                #endif//LOG_INFO_TIME

                    if constexpr(sizeof(os_char)==sizeof(char))
                    {
                        write(STDOUT_FILENO,reinterpret_cast<const char *>(msg->message),msg->message_length);
                        write(STDOUT_FILENO,"\n",1);
                    }
                    else
                    {
                        const int len=u16_to_u8(log_buf,LOG_BUF_SIZE,reinterpret_cast<const u16char *>(msg->message),msg->message_length);

                        if(len>0)
                        {
                            log_buf[len]='\n';
                            write(STDOUT_FILENO,log_buf,len+1);
                        }
                    }
            #ifdef LOGINFO_THREAD_MUTEX
                mutex.Unlock();
            #endif//LOGINFO_THREAD_MUTEX
            }
        };//class LogInterface

        Logger *CreateLoggerConsole(LogLevel ll)
        {
            return(new LogUnixConsole(ll));
        }
    }//logger
}//namespace hgl
