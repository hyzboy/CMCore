#ifndef HGL_PLUGIN_INCLUDE
#define HGL_PLUGIN_INCLUDE

#include<hgl/type/String.h>
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
        OSString name;
        UTF16String intro;

        OSString filename;                                                                          ///<插件文件名

    public:

        const uint          GetVersion  ()const{return ver;}                                        ///<取得插件版本
        const OSString &    GetName     ()const{return name;}                                       ///<取得插件名称
        const UTF16String & GetIntro    ()const{return intro;}                                      ///<取得插件介绍

    public:

        PlugIn()
        {
            ref_count=0;
            ver=0;
        }

        virtual ~PlugIn()=default;

        uint add_ref(){return ++ref_count;}
        uint release_ref(){return --ref_count;}

        virtual bool GetInterface(uint,void *)=0;
    };//class PlugIn
}//namespace hgl
#endif//HGL_PLUGIN_INCLUDE
