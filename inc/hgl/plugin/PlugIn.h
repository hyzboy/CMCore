#ifndef HGL_PLUGIN_INCLUDE
#define HGL_PLUGIN_INCLUDE

#include<hgl/type/BaseString.h>
#include<hgl/plugin/PlugInInterface.h>
namespace hgl
{    
    /**
    * 插件基类
    */
    class PlugIn                                                                                    ///插件
    {
    protected:
    
        uint ref_count;

        uint ver;
        UTF16String name;
        UTF16String intro;

        OSString filename;                                                                          ///<插件文件名

        PlugInInterface *plugin_interface;

    public:

        const uint          GetVersion  ()const{return ver;}                                        ///<取得插件版本
        const UTF16String & GetIntro    ()const{return intro;}                                      ///<取得插件介绍
        const UTF16String & GetName     ()const{return name;}                                       ///<取得插件名称

    public:

        PlugIn()
        {
            ref_count=0;
            ver=0;
            plugin_interface=nullptr;
        }

        virtual ~PlugIn();

        uint add_ref(){return ++ref_count;}
        uint release_ref(){return --ref_count;}
    };//class PlugIn
}//namespace hgl
#endif//HGL_PLUGIN_INCLUDE
