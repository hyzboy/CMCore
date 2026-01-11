#pragma once

#include<hgl/plugin/PlugIn.h>
#include<hgl/platform/ExternalModule.h>
namespace hgl
{
    struct PlugInInterface;

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

        PlugInInterface *plugin_interface;

    public:

        PlugInStatus GetStatus()const{return status;}

    public:

        ExternalPlugIn();
        virtual ~ExternalPlugIn();

                void Free();                                                    ///<释放插件文件
                bool Load(const OSString &,const OSString &);                   ///<加载插件

        virtual bool GetInterface(uint,void *) override;
    };//class ExternalPlugIn:public PlugIn
}//namespace hgl
