#ifndef HGL_EXTERNAL_PLUG_IN_INCLUDE
#define HGL_EXTERNAL_PLUG_IN_INCLUDE

#include<hgl/plugin/PlugIn.h>
#include<hgl/platform/ExternalModule.h>
namespace hgl
{    
    /**
     * 外部插件状态
     */
    enum class PlugInStatus
    {
        NONE,                           ///<无此插件
        LOAD_FAILED,                    ///<加载失败
        NO_LOAD,                        ///<可用未加载
        COMPLETE,                       ///<成功加载可用
    };//enum class PlugInStatus

    /**
     * 外部插件
     */
    class ExternalPlugIn:public PlugIn
    {
        PlugInStatus status;

        OSString filename;                                  ///<插件对应的真实文件系统名称

        ExternalModule *pi_module;

    public:

        PlugInStatus GetStatus()const{return status;}

    public:

        ExternalPlugIn();
        virtual ~ExternalPlugIn();

        void Free();                                        ///<释放插件文件
        bool Load(const OSString &,const OSString &);       ///<加载插件
    };//class ExternalPlugIn:public PlugIn
}//namespace hgl
#endif//HGL_EXTERNAL_PLUG_IN_INCLUDE
