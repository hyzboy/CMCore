#include<hgl/plugin/PlugIn.h>
#include<hgl/log/Log.h>
#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/time/DateTime.h>
#include<hgl/type/ValueArray.h>
#include<hgl/type/UnorderedMap.h>
#include<hgl/thread/RWLock.h>
#include<hgl/filesystem/FileSystem.h>

DEFINE_LOGGER_MODULE(LogInfo)

namespace hgl
{
    namespace logger
    {
        RWLockObject<ValueArray<Logger *>> log_list;        //记录器列表

        struct CategoryFilter
        {
            LogLevel default_level=LogLevel::Verbose;
            UnorderedMap<LogOutputterId,LogLevel> outputter_levels;
        };

        struct LogFilterState
        {
            RWLock lock;
            UnorderedMap<OSString,LogCategoryId> category_ids;
            UnorderedMap<LogCategoryId,CategoryFilter> category_filters;
            LogCategoryId next_category_id=1;
            LogOutputterId next_outputter_id=1;
        };

        LogFilterState &GetLogFilterState()
        {
            static LogFilterState state;
            return state;
        }

        bool IsLogLevelEnabled(const LogCategoryId category_id,
                               const LogOutputterId outputter_id,
                               const LogLevel outputter_level,
                               const LogLevel message_level)
        {
            LogLevel threshold=outputter_level;

            if(category_id!=InvalidLogCategoryId)
            {
                LogFilterState &state=GetLogFilterState();
                OnlyReadLock lock(state.lock);

                const CategoryFilter *category=state.category_filters.GetValuePointer(category_id);

                if(category)
                {
                    const LogLevel *outputter_override=category->outputter_levels.GetValuePointer(outputter_id);

                    if(outputter_override)
                        threshold=*outputter_override;
                    else
                    if(int(category->default_level)>int(threshold))
                        threshold=category->default_level;
                }
            }

            return int(message_level)>=int(threshold);
        }

        bool AddLogger(Logger *log)
        {
            if(!log)
                return(false);

            OnlyWriteLock owl(log_list);

            if(log_list->Contains(log))        //重复添加
                return(false);

            if(log->outputter_id==InvalidLogOutputterId)
            {
                LogFilterState &state=GetLogFilterState();
                OnlyWriteLock filter_lock(state.lock);

                if(state.next_outputter_id==InvalidLogOutputterId)
                    return(false);

                log->outputter_id=state.next_outputter_id++;
            }

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
                if(IsLogLevelEnabled(msg->meta.category_id,
                                     (*log)->GetOutputterId(),
                                     (*log)->GetLevel(),
                                     msg->meta.level))
                    (*log)->Write(msg);

                ++log;
            }
        }

        LogCategoryId RegisterLogCategory(const OSString &name,const LogLevel default_level)
        {
            if(name.IsEmpty())
                return(InvalidLogCategoryId);

            LogFilterState &state=GetLogFilterState();
            OnlyWriteLock lock(state.lock);

            if(const LogCategoryId *existing=state.category_ids.GetValuePointer(name))
                return(*existing);

            if(state.next_category_id==InvalidLogCategoryId)
                return(InvalidLogCategoryId);

            const LogCategoryId category_id=state.next_category_id++;
            CategoryFilter filter;
            filter.default_level=default_level;

            if(!state.category_filters.Add(category_id,filter))
                return(InvalidLogCategoryId);

            if(!state.category_ids.Add(name,category_id))
            {
                state.category_filters.DeleteByKey(category_id);
                return(InvalidLogCategoryId);
            }

            return(category_id);
        }

        LogCategoryId FindLogCategory(const OSString &name)
        {
            LogFilterState &state=GetLogFilterState();
            OnlyReadLock lock(state.lock);

            const LogCategoryId *category_id=state.category_ids.GetValuePointer(name);
            return category_id?*category_id:InvalidLogCategoryId;
        }

        bool SetLogCategoryLevel(const LogCategoryId category_id,const LogLevel level)
        {
            LogFilterState &state=GetLogFilterState();
            OnlyWriteLock lock(state.lock);

            CategoryFilter *filter=state.category_filters.GetValuePointer(category_id);

            if(!filter)
                return(false);

            filter->default_level=level;
            return(true);
        }

        bool HasLogOutputter(const LogOutputterId outputter_id)
        {
            OnlyReadLock lock(log_list);

            Logger **log=log_list->GetData();

            for(int i=0;i<log_list->GetCount();i++)
            {
                if((*log)->GetOutputterId()==outputter_id)
                    return(true);

                ++log;
            }

            return(false);
        }

        bool SetLogCategoryOutputterLevel(const LogCategoryId category_id,
                                          const LogOutputterId outputter_id,
                                          const LogLevel level)
        {
            if(outputter_id==InvalidLogOutputterId||!HasLogOutputter(outputter_id))
                return(false);

            LogFilterState &state=GetLogFilterState();
            OnlyWriteLock lock(state.lock);

            CategoryFilter *filter=state.category_filters.GetValuePointer(category_id);

            if(!filter)
                return(false);

            if(filter->outputter_levels.GetValuePointer(outputter_id))
                return filter->outputter_levels.Change(outputter_id,level);

            return filter->outputter_levels.Add(outputter_id,level);
        }

        bool ClearLogCategoryOutputterLevel(const LogCategoryId category_id,
                                            const LogOutputterId outputter_id)
        {
            LogFilterState &state=GetLogFilterState();
            OnlyWriteLock lock(state.lock);

            CategoryFilter *filter=state.category_filters.GetValuePointer(category_id);

            if(!filter)
                return(false);

            return filter->outputter_levels.DeleteByKey(outputter_id);
        }

        LogOutputterId FindLogOutputter(const OSString &name)
        {
            OnlyReadLock lock(log_list);

            Logger **log=log_list->GetData();

            for(int i=0;i<log_list->GetCount();i++)
            {
                if((*log)->GetOutputterName()==name)
                    return(*log)->GetOutputterId();

                ++log;
            }

            return(InvalidLogOutputterId);
        }

        bool SetLogOutputterLevel(const LogOutputterId outputter_id,const LogLevel level)
        {
            if(outputter_id==InvalidLogOutputterId)
                return(false);

            OnlyWriteLock lock(log_list);

            Logger **log=log_list->GetData();

            for(int i=0;i<log_list->GetCount();i++)
            {
                if((*log)->GetOutputterId()==outputter_id)
                {
                    (*log)->min_level=level;
                    return(true);
                }

                ++log;
            }

            return(false);
        }

        bool IsLogEnabled(const LogCategoryId category_id,const LogLevel level)
        {
            OnlyReadLock lock(log_list);

            Logger **log=log_list->GetData();

            for(int i=0;i<log_list->GetCount();i++)
            {
                if(IsLogLevelEnabled(category_id,(*log)->GetOutputterId(),(*log)->GetLevel(),level))
                    return(true);

                ++log;
            }

            return(false);
        }

        bool IsLogEnabledForOutputter(const LogCategoryId category_id,
                                      const LogOutputterId outputter_id,
                                      const LogLevel level)
        {
            OnlyReadLock lock(log_list);

            Logger **log=log_list->GetData();

            for(int i=0;i<log_list->GetCount();i++)
            {
                if((*log)->GetOutputterId()==outputter_id)
                    return IsLogLevelEnabled(category_id,outputter_id,(*log)->GetLevel(),level);

                ++log;
            }

            return(false);
        }

        bool PutLogHeader()
        {
            OnlyReadLock orl(log_list);

            if(log_list->GetCount()<=0)
                return(false);

            CalendarDate d;
            TimeOfDay t;

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

            msg.meta.category_id         =LogLogInfo.GetCategoryId();
            msg.meta.object_type_info    =nullptr;
            msg.meta.object_instance_name=OS_TEXT("LogInfo");
            msg.meta.source_location     =std::source_location::current();
            msg.meta.level               =LogLevel::Verbose;
            msg.meta.message_encoding    =LogTextEncoding::Native;
            const U8String str_u8   =ToU8String(str);
            const U16String str_u16 =to_u16(str_u8);

            msg.text.message_u8          =str_u8.c_str();
            msg.text.message_u8_length   =str_u8.Length();
            msg.text.message_u16         =str_u16.c_str();
            msg.text.message_u16_length  =str_u16.Length();

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

        static LogInterface LogInterface5=
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
            }

            bool GetInterface(uint ver,void *data) override
            {
                if(ver!=5||!data)
                    return(false);

                memcpy(data,&LogInterface5,sizeof(LogInterface));
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

            if(pi->GetInterface(5,log_interface))
                if(log_interface->Init())
                    return(pi);

            delete pi;
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
            if((!msg.text.message_u8||msg.text.message_u8_length<=0)
            && (!msg.text.message_u16||msg.text.message_u16_length<=0))
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
