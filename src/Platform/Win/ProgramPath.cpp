#include<hgl/type/String.h>
#include<windows.h>
#include<shlobj.h>

namespace hgl
{
    namespace filesystem
    {
        bool GetOSLibraryPath(OSString &result)
        {
            wchar_t dllfn[MAX_PATH];
            UINT size;

            size=::GetSystemDirectoryW(dllfn,MAX_PATH);

            if(size==0)
                return(false);

            result.fromString(dllfn,size);
            return(true);
        }

        bool GetLocalAppdataPath(OSString &result)
        {
            os_char fn[HGL_MAX_PATH];

            if(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, fn)!=S_OK)
                return(false);

            result=fn;
            return(true);
        }

        /**
         * 取得当前程序完整路径名称
         */
        bool GetCurrentProgram(OSString &result)
        {
            os_char *path=new os_char[HGL_MAX_PATH];

            GetModuleFileNameW(nullptr,path,HGL_MAX_PATH);

            result = path;
            delete[] path;

            return(true);
        }

        /**
         * 取得当前程序所在路径
         */
        bool GetCurrentProgramPath(OSString &result)
        {
            os_char *path=new os_char[HGL_MAX_PATH];

            int len=GetModuleFileNameW(nullptr,path,HGL_MAX_PATH);

            os_char *right = const_cast<os_char*>(hgl::strrchr(path,len,HGL_DIRECTORY_SEPARATOR));

            if(right)
            {
                *right=0;
            }

            result = path;
            delete[] path;

            return(true);
        }
    }//namespace filesystem
}//namespace hgl
