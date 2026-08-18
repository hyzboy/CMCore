#include<hgl/platform/ExternalModule.h>
#include<hgl/platform/Platform.h>

namespace hgl
{
    ExternalModule *LoadExternalModule(const os_char *filename)
    {
        ExternalModulePointer fp=LoadLibraryW(filename);

        if(!fp)
        {
            const uint dw=GetLastError();

            os_char *str=nullptr;

            //FORMAT_MESSAGE_ALLOCATE_BUFFER 模式要求传"指针变量的地址"，函数会把分配的缓冲区写回该变量
            FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,nullptr,dw,
                            MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),reinterpret_cast<os_char *>(&str),0,nullptr);

            GLogError(OS_TEXT("LoadModule <")+OSString(filename)+OS_TEXT("> error! os info: ")+OSString::numberOf(dw)+OSString::charOf(',')+(str?OSString(str):OSString(OS_TEXT("(unknown)"))));

            if(str)LocalFree(str);

            return(nullptr);
        }

        return(new ExternalModule(fp));
    }

    /**
    * 加载一个外部模块
    * @param name 模块文件名称
    * @return 是否加载成功
    */
    bool ExternalModule::Load(const os_char *name)
    {
        Clear();

        fp=LoadLibraryW(name);

        if(!fp)
        {
            const uint dw=GetLastError();

            os_char *str=nullptr;

            //FORMAT_MESSAGE_ALLOCATE_BUFFER 模式要求传"指针变量的地址"，函数会把分配的缓冲区写回该变量
            FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,nullptr,dw,
                            MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),reinterpret_cast<os_char *>(&str),0,nullptr);

            LogError(OS_TEXT("LoadModule <")+OSString(name)+OS_TEXT("> error! os info: ")+OSString::numberOf(dw)+OSString::charOf(',')+(str?OSString(str):OSString()));

            if(str)LocalFree(str);

            return(false);
        }

        return(true);
    }
}//namespace hgl

