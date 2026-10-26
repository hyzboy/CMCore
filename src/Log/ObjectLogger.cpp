#include<hgl/log/Log.h>
#include<hgl/log/LogMessage.h>
#include<hgl/Charset.h>
#include<hgl/type/StdString.h>
#include<hgl/time/DateTime.h>
#include<hgl/thread/Atomic.h>
#include<cstdarg>
#include<cstring>

namespace hgl::logger
{
    void LogOutput(const LogMessage &msg);

    static hgl::atom_uint64 GlobalLogSequence=0;
    static hgl::atom_uint64 GlobalObjectLoggerId=0;

    static ObjectLoggerId CreateObjectLoggerId()
    {
        return ++GlobalObjectLoggerId;
    }

    namespace
    {
        inline OSString GetLogLevelText(const LogLevel level)
        {
            switch(level)
            {
                case LogLevel::Verbose: return OS_TEXT("Verbose");
                case LogLevel::Debug:   return OS_TEXT("Debug");
                case LogLevel::Info:    return OS_TEXT("Info");
                case LogLevel::Notice:  return OS_TEXT("Notice");
                case LogLevel::Warning: return OS_TEXT("Warning");
                case LogLevel::Error:   return OS_TEXT("Error");
                case LogLevel::Fatal:   return OS_TEXT("Fatal");
                default:                return OS_TEXT("Unknown");
            }
        }

        inline const char *GetFileNameOnly(const char *path)
        {
            if(!path||!*path)
                return nullptr;

            const char *s1=std::strrchr(path,'/');
            const char *s2=std::strrchr(path,'\\');
            const char *s=s1>s2?s1:s2;

            return s?s+1:path;
        }

        inline OSString BuildDateTimeString()
        {
            CalendarDate d;
            TimeOfDay t;

            ToDateTime(d,t);

            return  OSString::numberOf(d.GetYear()) +OS_TEXT("-")+
                    OSString::numberOf(d.GetMonth())+OS_TEXT("-")+
                    OSString::numberOf(d.GetDay())  +OS_TEXT(" ")+
                    OSString::numberOf(t.GetHour()) +OS_TEXT(":")+
                    OSString::numberOf(t.GetMinute())+OS_TEXT(":")+
                    OSString::numberOf(t.GetSecond());
        }
    }//namespace

    ObjectLogger GlobalLogger;///<全局日志对象
}//namespace hgl::logger

namespace hgl::logger
{
    ObjectLogger::ObjectLogger()
    {
        object_logger_id=CreateObjectLoggerId();
        object_type_info=nullptr;
        object_type_name=OSString(OS_TEXT("Global"));
        object_instance_name.Clear();
        category_id=RegisterLogCategory(object_type_name,LogLevel::Verbose);
    }

    ObjectLogger::ObjectLogger(const std::type_info *info):object_type_info(info)
    {
        object_logger_id=CreateObjectLoggerId();
        object_type_name=info?ToOSString(info->name()):OSString(OS_TEXT("?"));

        object_instance_name.Clear();
        category_id=RegisterLogCategory(object_type_name,LogLevel::Verbose);
    }

    ObjectLogger::ObjectLogger(const os_char *name)
        :ObjectLogger(name,LogLevel::Verbose)
    {
    }

    ObjectLogger::ObjectLogger(const os_char *name,const LogLevel default_level)
    {
        object_logger_id=CreateObjectLoggerId();
        object_type_info=nullptr;

        object_type_name=name?OSString(name):OSString(OS_TEXT("?"));
        object_instance_name.Clear();
        category_id=RegisterLogCategory(object_type_name,default_level);
    }

    ObjectLogger::ObjectLogger(const ObjectLogger &other)
        :object_type_info(other.object_type_info),
         object_logger_id(CreateObjectLoggerId()),
         category_id(other.category_id),
         object_type_name(other.object_type_name),
         object_instance_name(other.object_instance_name),
         bound_object_id(other.bound_object_id),
         bound_object_type_id(other.bound_object_type_id),
         module_message_count(other.module_message_count),
         time_output_interval(other.time_output_interval),
         source_location_min_level(other.source_location_min_level),
         output_module_name(other.output_module_name),
         output_level_name(other.output_level_name)
    {
    }

    ObjectLogger &ObjectLogger::operator=(const ObjectLogger &other)
    {
        if(this==&other)
            return *this;

        object_type_info=other.object_type_info;
        category_id=other.category_id;
        object_type_name=other.object_type_name;
        object_instance_name=other.object_instance_name;
        bound_object_id=other.bound_object_id;
        bound_object_type_id=other.bound_object_type_id;
        module_message_count=other.module_message_count;
        time_output_interval=other.time_output_interval;
        source_location_min_level=other.source_location_min_level;
        output_module_name=other.output_module_name;
        output_level_name=other.output_level_name;
        log_buffer_u8.clear();
        log_buffer_u16.clear();

        return *this;
    }

    void ObjectLogger::BindObjectIdentity(const ObjectId object_id,const ObjectTypeId type_id,
                                          const LogCategoryId cat_id,const os_char *type_name)
    {
        bound_object_id      =object_id;
        bound_object_type_id =type_id;

        if(cat_id!=InvalidLogCategoryId)
            category_id=cat_id;

        if(type_name&&*type_name)
            object_type_name=OSString(type_name);
    }

    void ObjectLogger::LogString(const std::source_location &sl,const LogLevel level,const os_char *str,const int size)
    {
        if(!str||size<=0)
            return;

        const uint64 seq=++GlobalLogSequence;
        const uint64 module_seq=++module_message_count;

        OSString prefix=OS_TEXT("[#")+OSString::numberOf(seq)+OS_TEXT("]");
        prefix+=OS_TEXT("[ObjectLogger:")+OSString::numberOf(object_logger_id)+OS_TEXT("]");

        if(!object_instance_name.IsEmpty())
        {
            prefix+=OS_TEXT("[");
            prefix+=object_instance_name;
            prefix+=OS_TEXT("]");
        }

        if(time_output_interval>0
         &&(module_seq%time_output_interval)==0)
        {
            prefix+=OS_TEXT("[");
            prefix+=BuildDateTimeString();
            prefix+=OS_TEXT("]");
        }

        if(output_module_name)
        {
            if(object_instance_name.IsEmpty())
            {
                prefix+=OS_TEXT("[");
                prefix+=(object_type_name.IsEmpty()?OSString(OS_TEXT("Global")):object_type_name);
                prefix+=OS_TEXT("]");
            }
        }

        if(output_level_name)
        {
            prefix+=OS_TEXT("[");
            prefix+=GetLogLevelText(level);
            prefix+=OS_TEXT("]");
        }

        if(int(level)>=int(source_location_min_level))
        {
            const char *file_name=GetFileNameOnly(sl.file_name());

            if(file_name&&*file_name)
            {
                prefix+=OS_TEXT("[");
                prefix+=ToOSString(file_name);
                prefix+=OS_TEXT(":");
                prefix+=OSString::numberOf(uint(sl.line()));
                prefix+=OS_TEXT("]");
            }
        }

        prefix+=OS_TEXT(" ");

        OSString text(str,size);
        OSString final_message=prefix+text;

        const U8String final_message_u8=ToU8String(final_message);
        const U16String final_message_u16=to_u16(final_message_u8);

        LogMessage msg;

        msg.meta.object_logger_id     =object_logger_id;
        msg.meta.object_id           =bound_object_id;
        msg.meta.object_type_id      =bound_object_type_id;
        msg.meta.category_id         =category_id;
        msg.meta.object_type_info    =object_type_info;
        msg.meta.object_instance_name=object_instance_name;
        msg.meta.source_location     =sl;
        msg.meta.level               =level;
    #if HGL_OS == HGL_OS_Windows
        msg.meta.message_encoding    =LogTextEncoding::UTF16;
    #else
        msg.meta.message_encoding    =LogTextEncoding::UTF8;
    #endif
        msg.text.message_u8          =final_message_u8.c_str();
        msg.text.message_u8_length   =final_message_u8.Length();
        msg.text.message_u16         =final_message_u16.c_str();
        msg.text.message_u16_length  =final_message_u16.Length();

        LogOutput(msg);
    }

    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const u8char *fmt, va_list args)
    {
        if(!fmt||!*fmt||!IsEnabled(level))return;
        va_list args_copy;
        va_copy(args_copy,args);
        const int len=std::vsnprintf(nullptr,0,(char *)fmt,args_copy);
        va_end(args_copy);

        if(len>0)
        {
            log_buffer_u8.resize(len+1);

            std::vsnprintf((char *)log_buffer_u8.data(),len+1,(const char *)fmt,args);

        #if HGL_OS == HGL_OS_Windows

            int u16len=get_utf16_length(UTF8CharSet,(const char *)log_buffer_u8.data(),len)+1;

            log_buffer_u16.resize(u16len);

            u16len=u8_to_u16(log_buffer_u16.data(),u16len,log_buffer_u8.data(),len);

            LogString(sl,level,log_buffer_u16.data(),u16len);
        #else
            LogString(sl,level,log_buffer_u8.data(),len);
        #endif//HGL_OS == HGL_OS_Windows
        }
    }

#ifdef HGL_SUPPORT_CHAR8_T
    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const char *fmt, va_list args)
    {
        LogPrintf(sl,level,(const u8char *)fmt,args);
    }
#endif//HGL_SUPPORT_CHAR8_T

#if HGL_OS == HGL_OS_Windows
    void ObjectLogger::LogPrintf(const std::source_location &sl,const LogLevel level,const u16char *fmt, va_list args)
    {
        if(!fmt||!*fmt||!IsEnabled(level))return;
        va_list args_copy;
        va_copy(args_copy,args);
        const int len=std::vswprintf(nullptr,0,fmt,args_copy);
        va_end(args_copy);

        if(len>0)
        {
            log_buffer_u16.resize(len+1);
            std::vswprintf(log_buffer_u16.data(),len+1,fmt,args);

            LogString(sl,level,log_buffer_u16.data(),len);
        }
    }
#endif
}//namespace hgl::logger
