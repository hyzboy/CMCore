#include<hgl/log/Logger.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/thread/ThreadMutex.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/io/DataOutputStream.h>
#include<hgl/io/TextOutputStream.h>

namespace hgl
{
    using namespace filesystem;

    namespace logger
    {
        class LogFile:public Logger
        {
            ThreadMutex lock;

            io::FileOutputStream fos;
            io::TextOutputStream *tos;

        public:

            LogFile(LogLevel ll):Logger(ll)
            {
                tos=nullptr;
            }

            ~LogFile()
            {
                SAFE_CLEAR(tos);
            }

            bool Create(const OSString &project_code)
            {
                OSString local_app_data_path;
                OSString cmgdk_path;
                OSString fn;
                os_char filename[HGL_MAX_PATH];
                os_char num[16]={'.',0};

                if(!GetLocalAppdataPath(local_app_data_path))
                    return(false);

                cmgdk_path=filesystem::MergeFilename(local_app_data_path,OS_TEXT(".cmgdk"));

                if(!filesystem::MakePath(cmgdk_path))
                    return(false);

                fn=filesystem::MergeFilename(cmgdk_path,project_code);

                for(uint i=0;i<=0xFFFF;i++)
                {
                    strcpy(filename,HGL_MAX_PATH,fn);

                    if(i)
                    {
                        utos(num+1,14,i);
                        strcat(filename,HGL_MAX_PATH,num,sizeof(num));
                    }

                    strcat(filename,HGL_MAX_PATH,OS_TEXT(".log.txt"),8);

                    if(fos.CreateTrunc(filename))//创建成功
                    {
                        tos=io::CreateTextOutputStream<os_char>(&fos);
                        tos->WriteBOM();

                        return(true);
                    }
                }

                return(false);
            }

            void Close()
            {
                SAFE_CLEAR(tos);
                fos.Close();
            }

            void Write(const u16char *str,int size)
            {
                if(tos&&str&&*str&&size>0)
                {
//                    lock.Lock();
                    tos->WriteLine(str,size);
//                    lock.Unlock();
                }
            }

            void Write(const u8char *str,int size)
            {
                if(tos&&str&&*str&&size>0)
                {
//                    lock.Lock();
                    tos->WriteLine(str,size);
//                    lock.Unlock();
                }
            }
        };//class LogFile

        Logger *CreateLoggerFile(const OSString &pc,LogLevel ll)
        {
            if(ll<LogLevel::Error)
                return(nullptr);

            LogFile *lf=new LogFile(ll);

            if(lf->Create(pc))
                return lf;

            delete lf;
            return(nullptr);
        }
    }//namespace logger
}//namespace hgl
