#include<hgl/plugin/PlugInManager.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/plugin/ExternalPlugIn.h>

namespace hgl
{
    using namespace filesystem;

    PlugInManager::PlugInManager(const OSString &n)
    {
        name=OS_TEXT("CMP.")+n;

        OSString pn;

        if(filesystem::GetCurrentPath(pn))
        {
            AddFindPath(pn);

            pn=JoinPathWithFilename(pn,OS_TEXT("Plug-ins"));
            AddFindPath(pn);
        }

        if(filesystem::GetCurrentProgramPath(pn))
        {
            AddFindPath(pn);

            pn=JoinPathWithFilename(pn,OS_TEXT("Plug-ins"));
            AddFindPath(pn);
        }
    }

    bool PlugInManager::HasPluginFile(const OSString &pi_name,OSString *fullpath) const
    {
        if(pi_name.IsEmpty())return(false);

        const std::size_t fp_count=findpath.GetCount();
        if(fp_count<=0)return(false);

        OSString pi_filename=name+OS_TEXT(".")+pi_name+HGL_PLUGIN_EXTNAME;
        OSString pi_fullfilename;

        for(std::size_t i=0;i<fp_count;i++)
        {
            pi_fullfilename=JoinPathWithFilename(findpath[i],pi_filename);
            if(FileExist(pi_fullfilename))
            {
                if(fullpath)*fullpath=pi_fullfilename;
                return true;
            }
        }

        return false;
    }

    bool PlugInManager::ExistsByFilename(const OSString &full_filename) const
    {
        if(full_filename.IsEmpty())return(false);
        return FileExist(full_filename);
    }

    bool PlugInManager::IsLoaded(const OSString &pi_name) const
    {
        return this->Find(pi_name)!=nullptr;
    }

    PlugInStatus PlugInManager::GetStatus(const OSString &pi_name) const
    {
        PlugIn *pi=(const_cast<PlugInManager*>(this))->Find(pi_name);
        if(pi) return PlugInStatus::COMPLETE;

        OSString fp;
        if(HasPluginFile(pi_name,&fp))
            return PlugInStatus::NO_LOAD;

        return PlugInStatus::NONE;
    }

    int PlugInManager::Scan(OSStringList &out_names) const
    {
        out_names.Clear();

        const std::size_t fp_count=findpath.GetCount();
        if(fp_count<=0)return 0;

        OSString prefix=name+OS_TEXT(".");
        OSString ext=OS_TEXT("")+OSString(HGL_PLUGIN_EXTNAME);

        for(std::size_t i=0;i<fp_count;i++)
        {
            ArrayList<FileInfo> fi_list;
            if(GetFileInfoList(fi_list,findpath[i],false,true,false)<=0)
                continue;

            const int count=fi_list.GetCount();
            for(int n=0;n<count;n++)
            {
                const FileInfo *fi=fi_list.At(n);
                if(!fi||!fi->is_file)continue;

                OSString fname=fi->name;
                if(!fname.EndsWithIgnoreCase(ext))continue;
                if( fname.StartsWithIgnoreCase(prefix))continue;

                OSString plugin_name=fname.SubString(prefix.Length(),fname.Length()-prefix.Length()-ext.Length());

                #if HGL_OS == HGL_OS_Windows
                    if(out_names.CaseFind(plugin_name)==-1)
                        out_names.Add(plugin_name);
                #else
                    if(out_names.Find(plugin_name)==-1)
                        out_names.Add(plugin_name);
                #endif
            }
        }

        return out_names.GetCount();
    }

    int PlugInManager::ScanDetailed(hgl::ObjectList<PlugInInfo> &out_infos,bool probe) const
    {
        out_infos.Clear();

        OSStringList names;
        Scan(names);

        const int ncount=names.GetCount();
        for(int i=0;i<ncount;i++)
        {
            PlugInInfo *info=new PlugInInfo;

            info->name=names[i];
            info->version=0;
            info->status=PlugInStatus::NONE;
            info->intro.Clear();
            info->filename.Clear();

            OSString fullpath;
            if(HasPluginFile(info->name,&fullpath))
            {
                info->filename=fullpath;
                info->status=PlugInStatus::NO_LOAD;
            }

            if(probe && !fullpath.IsEmpty())
            {
                ExternalPlugIn epi;
                if(epi.Load(info->name,fullpath))
                {
                    info->version=epi.GetVersion();
                    info->intro=epi.GetIntro();
                    info->status=PlugInStatus::COMPLETE;
                    epi.Free();
                }
                else
                {
                    info->status=PlugInStatus::LOAD_FAILED;
                }
            }

            out_infos.Add(info);
        }

        return out_infos.GetCount();
    }

    bool PlugInManager::ProbePlugin(const OSString &pi_name,PlugInInfo &out_info) const
    {
        out_info.name=pi_name;
        out_info.filename.Clear();
        out_info.version=0;
        out_info.intro.Clear();
        out_info.status=PlugInStatus::NONE;

        OSString fullpath;
        if(!HasPluginFile(pi_name,&fullpath))
        {
            return false;
        }

        out_info.filename=fullpath;
        out_info.status=PlugInStatus::NO_LOAD;

        ExternalPlugIn epi;
        if(epi.Load(pi_name,fullpath))
        {
            out_info.version=epi.GetVersion();
            out_info.intro=epi.GetIntro();
            out_info.status=PlugInStatus::COMPLETE;
            epi.Free();
            return true;
        }

        out_info.status=PlugInStatus::LOAD_FAILED;
        return false;
    }

    bool PlugInManager::RegisterPlugin(PlugIn *pi)
    {
        if(!pi)return(false);

        const OSString &pi_name=pi->GetName();

        if(this->Find(pi_name))
            return(false);

        return this->Add(pi_name,pi);
    }

    uint PlugInManager::UnregisterPlugin(const OSString &pi_name)
    {
        PlugIn *pi=this->Find(pi_name);

        if(!pi)return(0);

        return this->Release(pi);
    }

    bool PlugInManager::AddFindPath(const OSString &path)
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

    PlugIn *PlugInManager::LoadPlugin(const OSString &pi_name,const OSString &filename)
    {
        if(pi_name.IsEmpty())return(nullptr);
        if(filename.IsEmpty())return(nullptr);

        PlugIn *pi=this->Get(pi_name);

        if(pi)return pi;

        if(!FileExist(filename))return(nullptr);

        ExternalPlugIn *epi=new ExternalPlugIn;

        epi->Load(pi_name,filename);

        if(this->Add(pi_name,epi))
            return epi;

        delete epi;
        return(nullptr);
    }

    PlugIn *PlugInManager::LoadPlugin(const OSString &pi_name)
    {
        if(pi_name.IsEmpty())return(nullptr);

        PlugIn *pi=this->Get(pi_name);

        if(pi)return(pi);

        const std::size_t fp_count=findpath.GetCount();

        if(fp_count<=0)return(nullptr);

        OSString pi_filename=name+OS_TEXT(".")+pi_name+HGL_PLUGIN_EXTNAME;
        OSString pi_fullfilename;
        ExternalPlugIn *epi=new ExternalPlugIn;

        for(std::size_t i=0;i<fp_count;i++)
        {
            pi_fullfilename=JoinPathWithFilename(findpath[i],pi_filename);

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

    bool PlugInManager::UnloadPlugin(const OSString &pi_name)
    {
        if(pi_name.IsEmpty())return(false);

        this->Release(pi_name);
        return(true);
    }
}//namespace hgl
