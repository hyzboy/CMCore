#pragma once

#include<hgl/plugin/ExternalPlugIn.h>
#include<hgl/type/ObjectManager.h>
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
    };

    /**
     * 插件管理
     */
    class PlugInManager:public ObjectManager<OSString,PlugIn>
    {
        OSString name;                                                          ///<插件类目名称(必须符合代码名称规则)

        OSStringList findpath;                                                  ///<插件查找目录

    public:

        PlugInManager(const OSString &n);
        virtual ~PlugInManager()=default;

        bool    RegisterPlugin(PlugIn *);                                       ///<注册一个内置插件
        uint    UnregisterPlugin(const OSString &);                             ///<释放一个内置插件

        bool    AddFindPath (const OSString &path);                             ///<添加一个插件查找目录

        bool    HasPluginFile(const OSString &pi_name,OSString *fullpath=nullptr) const;    ///<确认插件文件是否存在
        bool    ExistsByFilename(const OSString &full_filename) const;                          ///<确认指定文件是否存在

        bool    IsLoaded(const OSString &pi_name) const;                         ///<确认插件是否已加载
        PlugInStatus GetStatus(const OSString &pi_name) const;                   ///<取得插件状态

        int     Scan(OSStringList &out_names) const;                             ///<扫描可用插件名称
        int     ScanDetailed(hgl::ManagedArray<PlugInInfo> &out_infos,bool probe=false) const;   ///<扫描可用插件并可选探测
        bool    ProbePlugin(const OSString &pi_name,PlugInInfo &out_info) const; ///<对指定插件做一次信息探测

        PlugIn *LoadPlugin  (const OSString &,const OSString &);                ///<加载一个外部插件，明确指定全路径文件名
        PlugIn *LoadPlugin  (const OSString &);                                 ///<加载一个外部插件，自行查找
        bool    UnloadPlugin(const OSString &);                                 ///<释放一个外部插件
    };//class PlugInManager:public ObjectManager<U16String,PlugIn>

    /**
     * 插件注册模板
     */
    template<typename T> class RegisterPlugInProxy
    {
        T *plugin;

    public:

        RegisterPlugInProxy()
        {
            plugin=new T;
        }

        virtual ~RegisterPlugInProxy()
        {
            delete plugin;
        }

        T *get(){return plugin;}
    };//template<typename T> class RegisterPlugInProxy

    /*
        如Log插件中的Console,File插件，是直接在代码中的，属于内部插件。
        但Log输出至MySQL的插件，是以独立.dll/.so/dylib形式存在的，属于外部插件。
    */

#ifndef __MAKE_PLUGIN__     //内部插件
    #define REGISTER_PLUG_IN(name,classname)    static RegisterPlugInProxy<name,classname> plugin_proxy_##classname;   \
                                                extern "C" void register_plugin_##classname;
#else                       //外部插件
    #define REGISTER_PLUG_IN(name,classname)    static RegisterPlugInProxy<name,classname> plugin_proxy_##classname     \
    extern "C" void register_plugin_##name(void){}
#endif//__MAKE_PLUGIN__
}//namespace hgl
