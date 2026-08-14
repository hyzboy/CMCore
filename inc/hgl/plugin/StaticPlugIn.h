#pragma once

#include<hgl/plugin/PlugIn.h>
#include<hgl/plugin/PlugInInterface.h>

namespace hgl
{
    /**
     * 内建(静态链接)插件：包装一个静态的 PlugInInterface，行为与外部插件一致。
     * 由 REGISTER_PLUG_IN 在静态初始化期登记，PlugInManager 统一遍历并注册。
     */
    class StaticPlugIn:public PlugIn
    {
        PlugInInterface *plugin_interface;

    public:

        StaticPlugIn(const OSString &n,PlugInInterface *pi)
        {
            plugin_interface=pi;
            name=n;
            ver=0;

            if(plugin_interface)
            {
                if(plugin_interface->GetVersion)ver=plugin_interface->GetVersion();
                if(plugin_interface->GetIntro) intro=plugin_interface->GetIntro();
            }
        }

        virtual ~StaticPlugIn()=default;

        virtual bool GetInterface(uint ver_code,void *data) override
        {
            if(!plugin_interface||!plugin_interface->GetInterface)return(false);
            return plugin_interface->GetInterface(ver_code,data);
        }

        bool GetCapability(uint id,void *data)
        {
            if(!plugin_interface||!plugin_interface->GetCapability)return(false);
            return plugin_interface->GetCapability(id,data);
        }
    };//class StaticPlugIn
}//namespace hgl
