#include<hgl/plugin/ExternalPlugIn.h>
#include<hgl/plugin/PlugInInterface.h>

namespace hgl
{
    ExternalPlugIn::ExternalPlugIn()
    {
        status=PlugInStatus::NONE;
        pi_module=nullptr;
    }
    
    ExternalPlugIn::~ExternalPlugIn()
    {
        this->Free();
    }

    void ExternalPlugIn::Free()
    {
        if(!pi_module)return;
        
        plugin_interface->Close();

        delete pi_module;
        pi_module=nullptr;

        status=PlugInStatus::NO_LOAD;
    }

    bool ExternalPlugIn::Load(const OSString &pn,const OSString &fn)
    {
        if(pi_module)
            return(true);

        if(status==PlugInStatus::LOAD_FAILED)       //过去是加载失败
            return(false);                          //直接拒绝加载

        pi_module=LoadExternalModule(fn.c_str());

        if(pi_module)
        {
            InitPlugInPROC init_proc=(InitPlugInPROC)pi_module->GetFunc("InitPlugIn");

            if(init_proc!=nullptr)
            {
                plugin_interface=init_proc();

                if(plugin_interface)
                {                    
                    status=PlugInStatus::COMPLETE;

                    name    =pn;
                    filename=fn;

                    ver     =plugin_interface->GetVersion();
                    intro   =plugin_interface->GetIntro();
                    return(true);
                }
            }

            delete pi_module;
            pi_module=nullptr;
        }

        status=PlugInStatus::LOAD_FAILED;
        return(false);
    }
}//namespace hgl
