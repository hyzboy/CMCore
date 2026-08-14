#pragma once

#include<hgl/plugin/ExternalPlugIn.h>
#include<hgl/plugin/StaticPlugIn.h>
#include<hgl/type/ObjectManager.h>
#include<hgl/type/ManagedArray.h>
#include<hgl/type/StringList.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    struct PlugInInfo
    {
        OSString        name;
        OSString        filename;
        PlugInStatus    status;
        uint            version;
        U16String       intro;
        U16String       display_name;      ///<菜单显示名(能力 MenuInfo)
        U16String       category;          ///<分组名(能力 MenuInfo)
        int             priority;          ///<排序优先级(能力 MenuInfo,越大越靠前)
        AnsiStringList  extensions;        ///<支持的文件扩展名(能力 FileExtensions)
    };

    /**
     * 插件管理
     */
    class PlugInManager:public ManagedObjectRegistry<OSString,PlugIn>
    {
        OSString name;                                                          ///<插件类目名称(必须符合代码名称规则)
        OSString category_name;                                                 ///<插件类目(构造时传入的原始名称，如 "Audio")

        OSStringList findpath;                                                  ///<插件查找目录

        mutable bool probe_cache_valid;                                         ///<探测缓存是否有效
        mutable ManagedArray<PlugInInfo> probe_cache;                           ///<探测结果缓存

    public:

        PlugInManager(const OSString &n);
        virtual ~PlugInManager()=default;

        bool    RegisterPlugin(PlugIn *);                                       ///<注册一个内置插件
        uint    UnregisterPlugin(const OSString &);                             ///<释放一个内置插件

        void    LoadStaticPlugIns();                                            ///<加载所有静态登记的内建插件

        bool    AddFindPath (const OSString &path);                             ///<添加一个插件查找目录

        bool    HasPluginFile(const OSString &pi_name,OSString *fullpath=nullptr) const;    ///<确认插件文件是否存在
        bool    ExistsByFilename(const OSString &full_filename) const;                          ///<确认指定文件是否存在

        bool    IsLoaded(const OSString &pi_name) const;                         ///<确认插件是否已加载
        PlugInStatus GetStatus(const OSString &pi_name) const;                   ///<取得插件状态

        int     Scan(OSStringList &out_names) const;                             ///<扫描可用插件名称
        int     ScanDetailed(hgl::ManagedArray<PlugInInfo> &out_infos,bool probe=false) const;   ///<扫描可用插件并可选探测
        int     ScanAndProbe(hgl::ManagedArray<PlugInInfo> &out_infos,bool force=false);         ///<扫描并探测插件(带缓存)，用于菜单注册等场景
        bool    ProbePlugin(const OSString &pi_name,PlugInInfo &out_info) const; ///<对指定插件做一次信息探测

        PlugIn *LoadPlugin  (const OSString &,const OSString &);                ///<加载一个外部插件，明确指定全路径文件名
        PlugIn *LoadPlugin  (const OSString &);                                 ///<加载一个外部插件，自行查找
        bool    UnloadPlugin(const OSString &);                                 ///<释放一个外部插件

    private:

        void    AppendStaticPlugIns(hgl::ManagedArray<PlugInInfo> &out_infos) const;    ///<把静态登记的内建插件追加到扫描结果
    };//class PlugInManager:public ManagedObjectRegistry<U16String,PlugIn>
}//namespace hgl
