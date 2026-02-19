#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/CodePage.h>
#include<hgl/thread/ThreadMutex.h>
#include<unistd.h>
#include<pthread.h>

#ifdef LOG_INFO_TIME
#include<hgl/Time.h>
#endif//LOG_INFO_TIME

#include<android/log.h>

namespace hgl
{
    namespace logger
    {
        constexpr uint LOG_BUF_SIZE=4096;

        static android_LogPriority ToAndroidPriority(const LogLevel level)
        {
            switch(level)
            {
                case LogLevel::Fatal:   return ANDROID_LOG_FATAL;
                case LogLevel::Error:   return ANDROID_LOG_ERROR;
                case LogLevel::Warning: return ANDROID_LOG_WARN;
                case LogLevel::Notice:  return ANDROID_LOG_INFO;
                case LogLevel::Info:    return ANDROID_LOG_INFO;
                case LogLevel::Debug:   return ANDROID_LOG_DEBUG;
                case LogLevel::Verbose:
                default:                return ANDROID_LOG_VERBOSE;
            }
        }

        /**
        * Android控制台日志插件接口
        */
        class LogAndroidConsole:public Logger
        {
            char thread_string[256];
            char time_string[256];
            char log_buf[LOG_BUF_SIZE];

#ifdef LOGINFO_THREAD_MUTEX
            ThreadMutex mutex;
#endif//LOGINFO_THREAD_MUTEX

        public:

            LogAndroidConsole(LogLevel ll):Logger(ll)
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
                memcpy(thread_string,"[Thread:",8);

                htos(thread_string+8,128-9,pthread_self());
                strcat(thread_string,LOG_BUF_SIZE,']');
            }
#endif//LOG_INFO_THREAD

#ifdef LOG_INFO_TIME
            void WriteTime()
            {
                memcpy(time_string,"[Time:",6);

                ftos(time_string+6,128-strlen(time_string),GetDoubleTime());
                strcat(time_string,LOG_BUF_SIZE,']');
            }
#endif//LOG_INFO_TIME

            void Write(const LogMessage *msg) override
            {
                if(!msg||!msg->message||msg->message_length<=0)
                    return;

            #ifdef LOGINFO_THREAD_MUTEX
                mutex.Lock();
            #endif//LOGINFO_THREAD_MUTEX

                log_buf[0]=0;

                #ifdef LOG_INFO_THREAD
                    WriteThreadID();
                    strcpy(log_buf,LOG_BUF_SIZE,thread_string);
                #endif//LOG_INFO_THREAD

                #ifdef LOG_INFO_TIME
                    WriteTime();
                    strcat(log_buf,LOG_BUF_SIZE,time_string);
                #endif//LOG_INFO_TIME

                    int pos=hgl::strlen(log_buf);

                    if constexpr(sizeof(os_char)==sizeof(char))
                    {
                        int copy_len=msg->message_length;

                        if(copy_len>int(LOG_BUF_SIZE-1-pos))
                            copy_len=LOG_BUF_SIZE-1-pos;

                        if(copy_len>0)
                        {
                            memcpy(log_buf+pos,reinterpret_cast<const char *>(msg->message),copy_len);
                            pos+=copy_len;
                        }
                    }
                    else
                    {
                        const int len=u16_to_u8(log_buf+pos,LOG_BUF_SIZE-pos,reinterpret_cast<const u16char *>(msg->message),msg->message_length);

                        if(len>0)
                            pos+=len;
                    }

                    log_buf[pos]=0;

                    __android_log_write(ToAndroidPriority(msg->level),"",log_buf);

            #ifdef LOGINFO_THREAD_MUTEX
                mutex.Unlock();
            #endif//LOGINFO_THREAD_MUTEX
            }
        };//class LogAndroidConsole:public Logger

        Logger *CreateLoggerConsole(LogLevel ll)
        {
            return(new LogAndroidConsole(ll));
        }
    }//logger
}//namespace hgl

