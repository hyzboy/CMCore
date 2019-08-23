#ifndef HGL_PLUG_IN_MANAGE_INCLUDE
#define HGL_PLUG_IN_MANAGE_INCLUDE

#include<hgl/plugin/ExternalPlugIn.h>
#include<hgl/type/ResManage.h>
namespace hgl
{
    /**
     * 插件管理
     */
    template<typename T,typename C> class PlugInManage:public ResManage<C,PlugIn>
    {
        OSString name;                                                                              ///<插件类目名称(必须符合代码名称规则)

    public:

        PlugInManage(const OSString &n)
        {
            name=n;
        }

        virtual ~PlugInManager()=default;
    };//template<typename T> class PlugInManage

    /**
     * 插件注册模板
     */
    template<typename T> class RegistryPlugInProxy
    {
        SharedPtr<T> plugin;

    public:

        RegistryPlugInProxy()
        {
            plugin=new T;
        }

        virtual ~RegistryPlugInProxy()=default;

        T *get(){return *plugin;}
    };//template<typename T> class RegistryPlugInProxy

    /*
        内部插件:
                    如Log一类必须存在的插件，直接在代码中一块儿编译链接

        外部插件:
                    如音频音频图片解码编码、压缩解压缩等根据需要加载释放，以独立的.dll/.so/.dylib文件形式存在

        除log console/log file外，其它所有插件可以是内部插件也可以是外部插件
    */

#ifndef __MAKE_PLUGIN__     //内部插件
    #define REGISTRY_PLUG_IN(name,classname) static RegistryPlugInProxy<classname> plugin_proxy_##classname;
#else                       //外部插件
    #define REGISTRY_PLUG_IN(name,classname)    
    static 
    extern "C" void registry_plugin_##name(void)
    {
        
    }
#endif//__MAKE_PLUGIN__  
}//namespace hgl
#endif//HGL_PLUG_IN_MANAGE_INCLUDE
