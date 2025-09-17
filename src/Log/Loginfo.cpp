#include<hgl/plugin/PlugIn.h>
#include<hgl/log/Log.h>
#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/type/DateTime.h>
#include<hgl/type/ArrayList.h>
#include<hgl/thread/RWLock.h>
#include<hgl/filesystem/FileSystem.h>

DEFINE_LOGGER_MODULE(LogInfo)

namespace hgl
{
    namespace logger
    {
        RWLockObject<ArrayList<Logger *>> log_list;        //记录器列表

        bool AddLogger(Logger *log)
        {
            if(!log)
                return(false);

            OnlyWriteLock owl(log_list);

            if(log_list->Contains(log))        //重复添加
                return(false);

            log_list->Add(log);
            return(true);
        }

        void WriteLog(const LogMessage *msg)
        {
            OnlyReadLock orl(log_list);

            const int n=log_list->GetCount();

            if(n<=0)return;

            Logger **log=log_list->GetData();

            for(int i=0;i<n;i++)
            {
                if((*log)->GetLevel()<=msg->level)
                    (*log)->Write(msg);

                ++log;
            }
        }

        bool PutLogHeader()
        {
            OnlyReadLock orl(log_list);

            if(log_list->GetCount()<=0)
                return(false);

            Date d;
            Time t;

            ToDateTime(d,t);
            
            OSString cur_path;
            OSString cur_program;
    
            filesystem::GetCurrentPath(cur_path);
            filesystem::GetCurrentProgram(cur_program);

            const OSString str= OS_TEXT("Starting Log at ")+
                                OSString::numberOf(d.GetYear  ())+OS_TEXT("-")+
                                OSString::numberOf(d.GetMonth ())+OS_TEXT("-")+
                                OSString::numberOf(d.GetDay   ())+OS_TEXT(" ")+
                                OSString::numberOf(t.GetHour  ())+OS_TEXT(":")+
                                OSString::numberOf(t.GetMinute())+OS_TEXT(":")+
                                OSString::numberOf(t.GetSecond())+OS_TEXT("\n")+
                                OS_TEXT("Current program: ") + cur_program.c_str() + OS_TEXT("\n")+
                                OS_TEXT("Current path: ") + cur_path.c_str() + OS_TEXT("\n");
            LogMessage msg;

            msg.object_type_info    =nullptr;
            msg.object_instance_name=OS_TEXT("LogInfo");
            msg.source_location     =std::source_location::current();
            msg.level               =LogLevel::Verbose;
            msg.message             =str.c_str();
            msg.message_length      =str.Length();

            WriteLog(&msg);

            return(true);
        }

        void CloseAllLog()
        {
            OnlyWriteLock owl(log_list);

            const int n=log_list->GetCount();

            if(n<=0)return;

            Logger **log=log_list->GetData();

            for(int i=0;i<n;i++)
            {
                (*log)->Close();
                delete *log;

                ++log;
            }

            log_list->Free();
        }
    }//namespace logger

    namespace logger
    {
        /**
         * 日志插件接口
         */
        struct LogInterface
        {
            bool (*Add)(Logger *);                                                                      ///<增加一个日志输出

            bool (*Init)();                                                                             ///<初始化日志输出
            void (*Close)();                                                                            ///<关闭所有日志输出

            void (*Write)(const LogMessage *);                                                          ///<输出一行日志
        };//struct LogInterface

        static LogInterface LogInterface4=
        {
            AddLogger,

            PutLogHeader,
            CloseAllLog,

            WriteLog
        };

        /**
         * 日志插件
         */
        class LogPlugIn:public PlugIn                                                               ///日志插件
        {
        public:

            LogPlugIn()
            {
                ver=1;
                name=OS_TEXT("Log");
                intro=U16_TEXT("unicode text log module.");

                filename=OS_TEXT("Loginfo.cpp");
            }

            bool GetInterface(uint ver,void *data) override
            {
                if(ver!=4||!data)
                    return(false);

                memcpy(data,&LogInterface4,sizeof(LogInterface));
                return(true);
            }
        };//class LogPlugIn
    }//namespace logger

    namespace logger
    {
        static LogInterface *log_interface=nullptr;

        PlugIn *InitLog()
        {
            PlugIn *pi=new LogPlugIn;

            log_interface=new LogInterface;

            if(pi->GetInterface(4,log_interface))
                if(log_interface->Init())
                    return(pi);

            delete log_interface;
            log_interface=nullptr;

            return(nullptr);
        }

        void CloseLog()
        {
            if(!log_interface)return;

            log_interface->Close();
            delete log_interface;
            log_interface=nullptr;
        }

        void LogOutput(const LogMessage &msg)
        {
            if(!msg.message||msg.message_length<=0)
                return;

            if(!log_interface)
                return;

            log_interface->Write(&msg);
        }
    }//namespace logger

    namespace logger
    {
        Logger *CreateLoggerConsole (LogLevel);
        Logger *CreateLoggerFile    (const OSString &,LogLevel);
    
        /**
         * 独立的日志系统初始化<br>
         * 供不整体使用SDK的应用程序使用
         */
        bool InitLogger(const OSString &app_name)
        {
            AddLogger(CreateLoggerConsole(LogLevel::Verbose));
            AddLogger(CreateLoggerFile(app_name,LogLevel::Verbose));

            return InitLog();
        }
    }//namespace logger
}//namespace hgl
