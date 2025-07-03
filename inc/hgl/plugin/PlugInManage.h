#ifndef HGL_PLUG_IN_MANAGE_INCLUDE
#define HGL_PLUG_IN_MANAGE_INCLUDE

#include<hgl/plugin/ExternalPlugIn.h>
#include<hgl/type/ObjectManage.h>
#include<hgl/type/StringList.h>
namespace hgl
{
    /**
     * 插件管理
     */
    class PlugInManage:public ObjectManage<OSString,PlugIn>
    {
        OSString name;                                                          ///<插件类目名称(必须符合代码名称规则)

        OSStringList findpath;                                                  ///<插件查找目录

    public:

        PlugInManage(const OSString &n);
        virtual ~PlugInManage()=default;

        bool    RegisterPlugin(PlugIn *);                                       ///<注册一个内置插件
        uint    UnregisterPlugin(const OSString &);                             ///<释放一个内置插件

        bool    AddFindPath (const OSString &path);                             ///<添加一个插件查找目录

        PlugIn *LoadPlugin  (const OSString &,const OSString &);                ///<加载一个外部插件，明确指定全路径文件名
        PlugIn *LoadPlugin  (const OSString &);                                 ///<加载一个外部插件，自行查找
        bool    UnloadPlugin(const OSString &);                                 ///<释放一个外部插件
    };//class PlugInManage:public ObjectManage<U16String,PlugIn>

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
    extern "C" void register_plugin_##name(void)
    {
        
    }
#endif//__MAKE_PLUGIN__  
}//namespace hgl
#endif//HGL_PLUG_IN_MANAGE_INCLUDE
