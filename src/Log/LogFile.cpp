#include<hgl/log/Logger.h>
#include<hgl/log/LogMessage.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/io/DataOutputStream.h>
#include<hgl/io/TextOutputStream.h>
#include<hgl/time/Time.h>

namespace hgl
{
    using namespace filesystem;

    namespace logger
    {
        class LogFile:public Logger
        {
            io::FileOutputStream fos;
            io::TextOutputStream *tos;

        public:

            LogFile(LogLevel ll):Logger(ll)
            {
                SetOutputterName(OS_TEXT("File"));
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
                OSString base_filename;

                if(!GetLocalAppdataPath(local_app_data_path))
                    return(false);

                cmgdk_path=filesystem::JoinPathWithFilename(local_app_data_path,OS_TEXT(".cmgdk"));

                if(!filesystem::MakePath(cmgdk_path))
                    return(false);

                base_filename=filesystem::JoinPathWithFilename(cmgdk_path,project_code);
                const OSString run_stamp=OSString::numberOf(GetTimeMs());

                for(uint i=0;i<=0xFFFF;i++)
                {
                    const OSString filename=base_filename+OS_TEXT(".")+run_stamp+
                                            OS_TEXT(".")+OSString::numberOf(i)+OS_TEXT(".log");

                    if(filesystem::FileExist(filename))
                        continue;

                    if(fos.Create(filename))
                    {
                        tos=io::CreateTextOutputStream<u8char>(&fos);

                        if(!tos)
                        {
                            fos.Close();
                            return(false);
                        }

                        tos->WriteBOM();

                        return(true);
                    }

                    if(!filesystem::FileExist(filename))
                        return(false);
                }

                return(false);
            }

            void Close()
            {
                SAFE_CLEAR(tos);
                fos.Close();
            }

            void Write(const LogMessage *msg)
            {
                if (!msg)
                    return;

                if(!tos)
                    return;

                if(msg->text.message_u8&&msg->text.message_u8_length>0)
                {
                    tos->WriteLine(msg->text.message_u8,msg->text.message_u8_length);
                    return;
                }

                if(msg->text.message_u16&&msg->text.message_u16_length>0)
                {
                    const U8String text=to_u8(msg->text.message_u16,msg->text.message_u16_length);

                    if(text.Length()>0)
                        tos->WriteLine(text.c_str(),text.Length());
                }
            }
        };//class LogFile

        Logger *CreateLoggerFile(const OSString &pc,LogLevel ll)
        {
            LogFile *lf=new LogFile(ll);

            if(lf->Create(pc))
                return lf;

            delete lf;
            return(nullptr);
        }
    }//namespace logger
}//namespace hgl
