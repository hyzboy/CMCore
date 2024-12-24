#ifndef HGL_LOGGER_INCLUDE
#define HGL_LOGGER_INCLUDE

#include<hgl/log/LogInfo.h>
namespace hgl
{
    namespace logger
    {
        /**
         * 日志输出基类
         */
        class Logger
        {
        protected:
        
            LogLevel min_level;                                                                     ///<最小输出级别
            Logger *parent;                                                                         ///<上级输出器

        public:

            Logger(LogLevel l,Logger *pl=nullptr){min_level=l;parent=pl;}
            virtual ~Logger()=default;

            const LogLevel  GetLevel    ()const{return min_level;}                                  ///<取得最小输出级别
            Logger *        GetParent   (){return parent;}                                          ///<取得上级日志输出器

            virtual void Close()=0;                                                                 ///<关闭日志

            virtual void Write(const u16char *,int)=0;                                              ///<输出一行u16char日志
            virtual void Write(const u8char *,int)=0;                                               ///<输出一行u8char日志
        };//class Logger
    }//namespace logger

    using namespace logger;
}//namespace hgl
#endif//HGL_LOGGER_INCLUDE
