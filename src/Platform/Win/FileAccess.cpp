#include<hgl/io/FileAccess.h>
#include<hgl/log/Log.h>
#include<io.h>
#include<share.h>
#include<fcntl.h>

DEFINE_LOGGER_MODULE(FileAccess)

namespace hgl::io
{
    bool FileAccess::CreateTemp()
    {
        const uint buf_size=HGL_MAX_PATH;

        u16char PathBuffer[buf_size];
        u16char TempName[buf_size];

        GetTempPathW(buf_size,PathBuffer);

        GetTempFileNameW(PathBuffer,L"NEW",0,TempName);

        MLogVerbose(FileAccess,OS_TEXT("创建临时文件: ")+OSString(TempName))

        return Open(TempName,FileOpenMode::Create);
    }

    int OpenFile(const os_char *fn,FileOpenMode fom)
    {
        if(!fn||!*fn)
        {
            MLogError(FileAccess,OS_TEXT("文件名错误"))
            return(-1);
        }

        int fp;

        errno_t result;

        if(fom==FileOpenMode::Create       )result=_wsopen_s(&fp,fn,_O_BINARY|_O_WRONLY|_O_CREAT               ,_SH_DENYNO,S_IREAD|_S_IWRITE);else
        if(fom==FileOpenMode::CreateTrunc  )result=_wsopen_s(&fp,fn,_O_BINARY|_O_WRONLY|_O_CREAT|_O_TRUNC      ,_SH_DENYNO,S_IREAD|_S_IWRITE);else
//          if(fom==FileOpenMode::CreateTemp   )result=_wsopen_s(&fp,fn,_O_BINARY|_O_WRONLY|_O_CREAT|_O_TEMPORARY  ,_SH_DENYNO,S_IREAD|_S_IWRITE);else //某些平台不支持，所以全都不使用fomCreateTemp,统一使用CreateTemp
        if(fom==FileOpenMode::OnlyRead     )result=_wsopen_s(&fp,fn,_O_BINARY|_O_RDONLY                        ,_SH_DENYNO,S_IREAD|_S_IWRITE);else
        if(fom==FileOpenMode::OnlyWrite    )result=_wsopen_s(&fp,fn,_O_BINARY|_O_WRONLY                        ,_SH_DENYNO,S_IREAD|_S_IWRITE);else
        if(fom==FileOpenMode::ReadWrite    )result=_wsopen_s(&fp,fn,_O_BINARY|_O_RDWR                          ,_SH_DENYNO,S_IREAD|_S_IWRITE);else
        if(fom==FileOpenMode::Append       )result=_wsopen_s(&fp,fn,_O_BINARY|_O_APPEND                        ,_SH_DENYNO,S_IREAD|_S_IWRITE);else
        {
            MLogError(FileAccess,OS_TEXT("不支持的文件打开模式"))
            RETURN_ERROR(-1);
        }

        MLogVerbose(FileAccess,OS_TEXT("打开文件: ")+OSString(fn)+OS_TEXT(", 模式: ")+OSString::numberOf((int)fom)+OS_TEXT(", 结果: ")+(result==0?OS_TEXT("成功"):OS_TEXT("失败")))

        return(fp);
    }

    void CloseFile(int fp)
    {
        _close(fp);
    }

    int64 FileAccess::Read(int64 offset,void *buf,int64 size)
    {
        if(!CanRead())return(-1);

        if(_lseeki64(fp,offset,(int)SeekOrigin::Begin)==offset)
            return _read(fp,buf,size);
        else
        {
            MLogError(FileAccess,OS_TEXT("定位文件指针失败"))
            return -1;
        }
    }

    int64 FileAccess::Write(int64 offset,const void *buf,int64 size)
    {
        if(!CanWrite())return(-1);

        if(_lseeki64(fp,offset,(int)SeekOrigin::Begin)==offset)
            return _write(fp,buf,size);
        else
        {
            MLogError(FileAccess,OS_TEXT("定位文件指针失败"))
            return -1;
        }
    }
}//namespace hgl::io
