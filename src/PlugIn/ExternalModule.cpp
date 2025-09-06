﻿#include<hgl/platform/ExternalModule.h>

namespace hgl
{
    ExternalModule::ExternalModule()
    {
        fp=nullptr;
    }

    ExternalModule::ExternalModule(ExternalModulePointer emp)
    {
        fp=emp;
    }

    ExternalModule::~ExternalModule()
    {
        Clear();
    }

    void ExternalModule::Clear()
    {
        if(fp)
        {
            pi_close(fp);

            fp=nullptr;
        }
    }

    void *ExternalModule::GetFunc(const char *name)
    {
        if(fp)
        {
            void *func=(void*)pi_get(fp,name);

            if(!func)
                LogError("don't find "+AnsiString(name));

            return(func);
        }
        else
        {
            LogError("Get func <"+AnsiString(name)+"> error: no load module!");

            return(nullptr);
        }
    }

    void *ExternalModule::FindFunc(const char *name)
    {
        if(fp)
        {
            void *func=(void*)pi_get(fp,name);

            if(func)return(func);

            char _name[HGL_MAX_PATH];

            _name[0]='_';
            strcpy(_name+1,HGL_MAX_PATH-1,name);

            func=(void*)pi_get(fp,_name);

            if(func)return(func);
        }

        return(nullptr);
    }

    /**
     * 读取函数列表
     * @return 成功加载的函数数量
     */
    int ExternalModule::Get(FuncLoad *flp)
    {
        if(!flp)return(-1);

        int count=0;

        while(flp->func_name[0])
        {
            (*(flp->func_pointer))=GetFunc(flp->func_name);

            if(*(flp->func_pointer))
                ++count;

            ++flp;
        }

        return(count);
    }
}//namespace hgl
