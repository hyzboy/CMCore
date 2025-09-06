#include<hgl/plugin/PlugIn.h>
#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/type/DateTime.h>
#include<hgl/type/ArrayList.h>
#include<hgl/thread/RWLock.h>
#include<hgl/filesystem/FileSystem.h>

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

        template<typename T>
        void WriteLog(LogLevel level,const T *str,int size)
        {
            OnlyReadLock orl(log_list);

            const int n=log_list->GetCount();

            if(n<=0)return;

            Logger **log=log_list->GetData();

            for(int i=0;i<n;i++)
            {
                if((*log)->GetLevel()<=level)
                    (*log)->Write(str,size);

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

            WriteLog(LogLevel::Verbose,str.c_str(),str.Length());

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

            void (*WriteUTF16)(LogLevel,const u16char *,int);                                           ///<输出一行日志
            void (*WriteUTF8)(LogLevel,const u8char *,int);                                             ///<输出一行日志
        };//struct LogInterface

        static LogInterface LogInterface3=
        {
            AddLogger,

            PutLogHeader,
            CloseAllLog,

            WriteLog<u16char>,
            WriteLog<u8char>
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
                if(ver!=3||!data)
                    return(false);

                memcpy(data,&LogInterface3,sizeof(LogInterface));
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

            if(pi->GetInterface(3,log_interface))
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

            #if HGL_OS == HGL_OS_Windows
                log_interface->WriteUTF16(msg.level,msg.message,msg.message_length);
            #else
                log_interface->WriteUTF8(msg.level,msg.message,msg.message_length);
            #endif//
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
