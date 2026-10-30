#include<hgl/plugin/PlugInManager.h>
#include<hgl/plugin/PlugInInterface.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/plugin/ExternalPlugIn.h>

namespace hgl
{
    using namespace filesystem;

    namespace
    {
        /**
         * 从已加载的插件填充探测信息(版本/介绍/菜单信息/扩展名)
         */
        template<typename T>
        void FillInfoFromPlugin(T &pi,PlugInInfo &info)
        {
            info.version = pi.GetVersion();
            info.intro   = pi.GetIntro();

            PlugInMenuInfo menu;
            if(pi.GetCapability(uint32(PlugInCapability::MenuInfo),&menu))
            {
                if(menu.display_name)info.display_name=menu.display_name;
                if(menu.category)    info.category    =menu.category;
                info.priority=menu.priority;
            }

            PlugInFileExtensions fe;
            if(pi.GetCapability(uint32(PlugInCapability::FileExtensions),&fe))
            {
                if(fe.items)
                {
                    for(uint32 i=0;i<fe.count;i++)
                        if(fe.items[i])
                            info.extensions.Add(fe.items[i]);
                }
            }
        }
    }

    PlugInManager::PlugInManager(const OSString &n)
    {
        name=OSString(HGL_PLUGIN_FRONTNAME)+n;      //Windows:CMP.  Linux/Apple等:libCMP.
        category_name=n;

        probe_cache_valid=false;

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
        PlugIn *pi=this->Find(pi_name);
        if(pi)
        {
            const ExternalPlugIn *epi=dynamic_cast<const ExternalPlugIn *>(pi);
            if(epi)return epi->GetStatus();         //外部插件：返回真实状态(可能为 LOAD_FAILED)

            return PlugInStatus::COMPLETE;          //静态插件加载即可用
        }

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
        const OSString ext=OSString(HGL_PLUGIN_EXTNAME);

        for(std::size_t i=0;i<fp_count;i++)
        {
            ValueArray<FileInfo> fi_list;
            if(GetFileInfoList(fi_list,findpath[i],false,true,false)<=0)
                continue;

            const int count=fi_list.GetCount();
            for(int n=0;n<count;n++)
            {
                const FileInfo *fi=fi_list.At(n);
                if(!fi||!fi->is_file)continue;

                OSString fname=fi->name;
                if(!fname.EndsWithIgnoreCase(ext))continue;
                if(!fname.StartsWithIgnoreCase(prefix))continue;

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

        //排序:统一忽略大小写,与 Windows 去重(CaseFind)一致,保证跨平台顺序相同
        const int total=out_names.GetCount();
        for(int i=0;i<total-1;i++)
        {
            for(int j=0;j<total-1-i;j++)
            {
                if(out_names[j].CaseComp(out_names[j+1])>0)
                    out_names.Exchange(j,j+1);
            }
        }

        return out_names.GetCount();
    }

    int PlugInManager::ScanDetailed(hgl::ManagedArray<PlugInInfo> &out_infos,bool probe) const
    {
        out_infos.Clear();

        OSStringList names;
        Scan(names);

        const int ncount=names.GetCount();
        for(int i=0;i<ncount;i++)
        {
            PlugInInfo *info=out_infos.Create();

            info->name=names[i];
            info->version=0;
            info->priority=0;
            info->status=PlugInStatus::NONE;
            info->intro.Clear();
            info->display_name.Clear();
            info->category.Clear();
            info->extensions.Clear();
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
                    FillInfoFromPlugin(epi,*info);
                    info->status=PlugInStatus::COMPLETE;
                    epi.Free();
                }
                else
                {
                    info->status=PlugInStatus::LOAD_FAILED;
                }
            }
        }

        AppendStaticPlugIns(out_infos);

        return out_infos.GetCount();
    }

    int PlugInManager::ScanAndProbe(hgl::ManagedArray<PlugInInfo> &out_infos,bool force)
    {
        if(!force && probe_cache_valid)
        {
            out_infos.Clear();

            const int count=probe_cache.GetCount();
            for(int i=0;i<count;i++)
            {
                PlugInInfo *src=probe_cache[i];
                if(!src)continue;

                PlugInInfo *dst=out_infos.Create();
                *dst=*src;
            }

            return out_infos.GetCount();
        }

        ScanDetailed(out_infos,true);

        probe_cache.ClearAndDeallocate();

        const int count=out_infos.GetCount();
        for(int i=0;i<count;i++)
        {
            PlugInInfo *src=out_infos[i];
            if(!src)continue;

            PlugInInfo *dst=probe_cache.Create();
            *dst=*src;
        }

        probe_cache_valid=true;

        return count;
    }

    bool PlugInManager::ProbePlugin(const OSString &pi_name,PlugInInfo &out_info) const
    {
        out_info.name=pi_name;
        out_info.filename.Clear();
        out_info.version=0;
        out_info.priority=0;
        out_info.intro.Clear();
        out_info.display_name.Clear();
        out_info.category.Clear();
        out_info.extensions.Clear();
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
            FillInfoFromPlugin(epi,out_info);
            out_info.status=PlugInStatus::COMPLETE;
            epi.Free();
            return true;
        }

        out_info.status=PlugInStatus::LOAD_FAILED;
        return false;
    }

    void PlugInManager::AppendStaticPlugIns(hgl::ManagedArray<PlugInInfo> &out_infos) const
    {
        for(StaticPlugInRegistrar *reg=StaticPlugInRegistrar::GetHead();reg;reg=reg->next)
        {
            const StaticPlugInDesc &d=reg->desc;

            if(!d.init||!d.name)continue;

            // 类目不匹配则跳过
            if(d.category && category_name.Comp(d.category)!=0)
                continue;

            // 若外部 DLL 已提供同名插件，则以文件版为准，跳过内建版
            bool exists=false;
            const int ncount=out_infos.GetCount();
            for(int i=0;i<ncount;i++)
            {
                if(out_infos[i] && out_infos[i]->name.Comp(d.name)==0)
                {
                    exists=true;
                    break;
                }
            }
            if(exists)continue;

            PlugInInterface *pi=d.init();
            if(!pi)continue;

            PlugInInfo *info=out_infos.Create();
            info->name=d.name;
            info->filename.Clear();
            info->version=0;
            info->priority=0;
            info->status=PlugInStatus::NO_LOAD;
            info->intro.Clear();
            info->display_name.Clear();
            info->category.Clear();
            info->extensions.Clear();

            StaticPlugIn spi(OSString(d.name),pi);
            FillInfoFromPlugin(spi,*info);
        }
    }

    void PlugInManager::LoadStaticPlugIns()
    {
        for(StaticPlugInRegistrar *reg=StaticPlugInRegistrar::GetHead();reg;reg=reg->next)
        {
            const StaticPlugInDesc &d=reg->desc;

            if(!d.init||!d.name)continue;

            if(d.category && category_name.Comp(d.category)!=0)
                continue;

            const OSString pi_name(d.name);

            if(this->Find(pi_name))continue;        //已加载

            PlugInInterface *pi=d.init();
            if(!pi)continue;

            this->Add(pi_name,new StaticPlugIn(pi_name,pi));
        }
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
        const int remain=this->Release(pi_name,true);
        return remain>=0?1:0;
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

        probe_cache_valid=false;        //查找目录变化，缓存失效

        return(true);
    }

    PlugIn *PlugInManager::LoadPlugin(const OSString &pi_name,const OSString &filename)
    {
        if(pi_name.IsEmpty())return(nullptr);
        if(filename.IsEmpty())return(nullptr);

        LoadStaticPlugIns();

        PlugIn *pi=this->Find(pi_name);

        if(pi)return pi;

        if(!FileExist(filename))return(nullptr);

        ExternalPlugIn *epi=new ExternalPlugIn;

        if(!epi->Load(pi_name,filename))            //加载失败：不登记，避免坏插件对象永久占用注册表
        {
            delete epi;
            return(nullptr);
        }

        if(this->Add(pi_name,epi))
            return epi;

        delete epi;
        return(nullptr);
    }

    PlugIn *PlugInManager::LoadPlugin(const OSString &pi_name)
    {
        if(pi_name.IsEmpty())return(nullptr);

        LoadStaticPlugIns();

        PlugIn *pi=this->Find(pi_name);

        if(pi)return(pi);

        const std::size_t fp_count=findpath.GetCount();

        if(fp_count<=0)return(nullptr);

        OSString pi_filename=name+OS_TEXT(".")+pi_name+HGL_PLUGIN_EXTNAME;
        OSString pi_fullfilename;

        for(std::size_t i=0;i<fp_count;i++)
        {
            pi_fullfilename=JoinPathWithFilename(findpath[i],pi_filename);

            if(!FileExist(pi_fullfilename))continue;

            //每个目录使用全新实例：Load 失败后 ExternalPlugIn 内部状态为 LOAD_FAILED 且拒绝重试，
            //若复用同一实例，首个目录加载失败会导致后续目录全部被拒(多路径查找失效)
            ExternalPlugIn *epi=new ExternalPlugIn;

            if(epi->Load(pi_name,pi_fullfilename))
            {
                if(this->Add(pi_name,epi))
                    return epi;
            }

            delete epi;                             //加载失败或登记失败：丢弃本次尝试，继续下一个目录
        }

        return(nullptr);
    }

    bool PlugInManager::UnloadPlugin(const OSString &pi_name)
    {
        if(pi_name.IsEmpty())return(false);

        return this->Release(pi_name,true)>=0;
    }
}//namespace hgl
