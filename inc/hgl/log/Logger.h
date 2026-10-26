#pragma once

#include<hgl/type/String.h>
#include<hgl/log/LogLevel.h>
#include<hgl/log/LogCategory.h>

namespace hgl::logger
{
    struct LogMessage;

    /**
    * 日志输出基类
    */
    class Logger
    {
    protected:

        LogLevel min_level;                                                                     ///<最小输出级别
        Logger *parent;                                                                         ///<上级输出器
        LogOutputterId outputter_id=InvalidLogOutputterId;
        OSString outputter_name;

        friend bool AddLogger(Logger *);
        friend bool SetLogOutputterLevel(LogOutputterId,LogLevel);

    public:

        Logger(LogLevel l,Logger *pl=nullptr){min_level=l;parent=pl;}
        virtual ~Logger()=default;

        const LogLevel  GetLevel    ()const{return min_level;}                                  ///<取得最小输出级别
        Logger *        GetParent   (){return parent;}                                          ///<取得上级日志输出器
        LogOutputterId  GetOutputterId()const{return outputter_id;}
        const OSString &GetOutputterName()const{return outputter_name;}

        void SetOutputterName(const OSString &name){outputter_name=name;}

        virtual void Close()=0;                                                                 ///<关闭日志

        virtual void Write(const LogMessage *)=0;                                               ///<输出一行日志
    };//class Logger

    bool InitLogger(const OSString &app_name);
}//namespace hgl::logger
