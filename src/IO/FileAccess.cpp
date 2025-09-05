﻿#include<hgl/platform/Platform.h>
#include<hgl/io/FileAccess.h>
#include<hgl/log/LogInfo.h>

#if HGL_OS != HGL_OS_Windows
#include<unistd.h>
#else
#include<io.h>
#endif//HGL_OS!=HGL_OS_Windows
namespace hgl
{
    namespace io
    {
        FileAccess::FileAccess()
        {
            fp=-1;
            mode=FileOpenMode::None;
            hgl_zero(file_state);
        }

        FileAccess::~FileAccess()
        {
            Close();
        }

        int OpenFile(const os_char *fn,FileOpenMode fom);

        bool FileAccess::Open(const OSString &fn,FileOpenMode fom)
        {
            Close();

            if(fn.IsEmpty())
            {
                LogError(OS_TEXT("Error,filename is NULL"));
                return(false);
            }

            fp=OpenFile(fn.c_str(),fom);

            if(fp==-1)
            {
                LogError(OS_TEXT("open file error,filename: ")+fn);
                return(false);
            }

            Log.SetLoggerInstanceName(U8String::numberOf(fp));

            LogVerbose(OS_TEXT("open file success,filename: ")+fn+OS_TEXT(", OpenMode:")+OSString::numberOf((int)fom));

            filename=fn;
            mode=fom;
            return(true);
        }

        void CloseFile(int fp);

        void FileAccess::Close()
        {
            if(fp==-1)return;

            CloseFile(fp);

            fp=-1;
            mode=FileOpenMode::None;
        }

        void FileAccess::CloseRead()
        {
            if(fp==-1)return;

            if(!CanWrite())
                Close();

            mode=FileOpenMode::OnlyWrite;
        }

        void FileAccess::CloseWrite()
        {
            if(fp==-1)return;

            if(!CanRead())
                Close();

            mode=FileOpenMode::OnlyRead;
        }

        bool FileAccess::CanRead() const
        {
            if(fp==-1)return(false);

            if(mode==FileOpenMode::OnlyWrite)return(false);

            return true;
        }

        bool FileAccess::CanWrite() const
        {
            if(fp==-1)return(false);

            if(mode==FileOpenMode::OnlyRead)return(false);

            return(true);
        }

        bool FileAccess::CanSeek() const
        {
            if(fp==-1)return(false);
            if(mode<FileOpenMode::Create||mode>=FileOpenMode::Append)return(false);

            return(true);
        }

        int64 FileAccess::Seek(int64 offset,SeekOrigin orign)
        {
            if(!CanSeek())return(-1);

            return hgl_lseek64(fp,offset,(int)orign);
        }

        int64 FileAccess::Tell()const
        {
            if(fp==-1)return(-1);

            return hgl_tell64(fp);
        }

        int64 FileAccess::GetSize()
        {
            if(!CanSize())return(-1);

            hgl_fstat64(fp,&file_state);
            return(file_state.st_size);
        }

        int64 FileAccess::AvailableRead()
        {
            if(fp==-1)return(-1);

            if(mode!=FileOpenMode::OnlyRead
             &&mode!=FileOpenMode::ReadWrite)
                return(-1);

            return GetSize()-Tell();
        }

        bool FileAccess::Restart()
        {
            if(!CanRestart())return(false);

            return(Seek(0,SeekOrigin::Begin)==0);
        }

        int64 FileAccess::Read(void *buf,int64 size)
        {
            if(!CanRead())return(-1);

            return hgl_read64(fp,buf,size);
        }

        int64 FileAccess::Peek(void *buf,int64 size)
        {
            if(!CanPeek())return(-1);

            int result=hgl_read64(fp,buf,size);

            if(result>0)
                hgl_lseek64(fp,-result,(int)SeekOrigin::Current);        //回移这个长度

            return result;
        }

        int64 FileAccess::Write(const void *buf,int64 size)
        {
            if(!CanWrite())return(-1);

            return hgl_write64(fp,buf,size);
        }
    }//namespace io
}//namespace hgl
