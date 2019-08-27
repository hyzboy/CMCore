#include<hgl/plugin/PlugInManage.h>
#include<hgl/filesystem/FileSystem.h>

namespace hgl
{
    using namespace filesystem;

    bool PlugInManage::RegistryPlugin(PlugIn *pi)
    {
        if(!pi)return(false);

        const OSString &pi_name=pi->GetName();

        if(this->Find(pi_name))
            return(false);

        return this->Add(pi_name,pi);
    }

    uint PlugInManage::UnregistryPlugin(const OSString &pi_name)
    {
        PlugIn *pi=this->Find(pi_name);

        if(!pi)return(0);

        return this->Release(pi);
    }

    bool PlugInManage::AddFindPath(const OSString &path)
    {
        if(path.IsEmpty())return(false);
        if(!IsDirectory(path))return(false);

        #if HGL_OS == HGL_OS_Windows
            if(findpath.CaseFind(path)!=-1)     //Windows平台无视大小写
        #else
            if(findpath.Find(path)!=-1)
        #endif//
                return(false);

        findpath.Add(path);
        return(true);
    }

    PlugIn *PlugInManage::LoadPlugin(const OSString &pi_name,const OSString &filename)
    {
        if(pi_name.IsEmpty())return(nullptr);
        if(filename.IsEmpty())return(nullptr);

        PlugIn *pi=this->Get(pi_name);

        if(pi)return pi;

        if(!FileExist(filename))return(false);

        ExternalPlugIn *epi=new ExternalPlugIn;

        epi->Load(pi_name,filename);

        if(this->Add(pi_name,epi))
            return epi;

        delete epi;
        return(nullptr);
    }

    PlugIn *PlugInManage::LoadPlugin(const OSString &pi_name)
    {
        if(pi_name.IsEmpty())return(nullptr);

        PlugIn *pi=this->Get(pi_name);

        if(pi)return(pi);

        const uint fp_count=findpath.GetCount();

        if(fp_count<=0)return(nullptr);

#if HGL_OS == HGL_OS_Windows
        OSString pi_filename=name+L'.'+pi_name+HGL_PLUGIN_EXTNAME;
#else
        OSString pi_filename=ToOSString(name)+'.'+ToOString(pi_name)+HGL_PLUGIN_EXTNAME;
#endif//HGL_OS == HGL_OS_Windows

        OSString pi_fullfilename;
        ExternalPlugIn *epi=new ExternalPlugIn;

        for(uint i=0;i<fp_count;i++)
        {
            pi_fullfilename=MergeFilename(findpath[i],pi_filename);

            if(!FileExist(pi_fullfilename))continue;

            if(epi->Load(pi_name,pi_fullfilename))
            {
                if(this->Add(pi_name,epi))
                    return epi;
            }

            epi->Free();
        }

        delete epi;
        return(nullptr);
    }

    bool PlugInManage::UnloadPlugin(const OSString &pi_name)
    {
        if(pi_name.IsEmpty())return(nullptr);

        this->Release(pi_name);
        return(true);
    }
}//namespace hgl
