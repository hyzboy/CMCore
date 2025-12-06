#include<hgl/platform/ExternalModule.h>
#include<hgl/platform/Platform.h>

namespace hgl
{
    ExternalModule *LoadExternalModule(const os_char *filename)
    {
        ExternalModulePointer fp=LoadLibraryW(filename);

        if(!fp)
        {
            uint dw=GetLastError();
            os_char *str=nullptr;

            FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,nullptr,dw,
                            MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),str,0,nullptr);

            GLogError(OS_TEXT("LoadModule <")+OSString(filename)+OS_TEXT("> error! os info: ")+OSString::numberOf(dw)+OSString::charOf(',')+str);

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
            u16char *str=nullptr;

            FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,nullptr,dw,
                            MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),str,0,nullptr);

            LogError(L"LoadModule <"+U16String(name)+L"> error! os info: "+U16String::numberOf(dw)+U16String::charOf(L',')+str);

            return(false);
        }

        return(true);
    }
}//namespace hgl

