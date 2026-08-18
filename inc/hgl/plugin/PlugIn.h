#pragma once

#include<hgl/type/String.h>
namespace hgl
{
    /**
    * 插件基类
    */
    class PlugIn                                                                                    ///插件
    {
    protected:

        uint ver;
        OSString name;
        U16String intro;

    public:

        uint                GetVersion  ()const{return ver;}                                        ///<取得插件版本
        const OSString &    GetName     ()const{return name;}                                       ///<取得插件名称
        const U16String &   GetIntro    ()const{return intro;}                                      ///<取得插件介绍

    public:

        PlugIn()
        {
            ver=0;
        }

        virtual ~PlugIn()=default;

        virtual bool GetInterface(uint,void *)=0;

        /**
         * 查询插件能力(见 PlugInCapability)
         * @param id   能力ID
         * @param data 能力数据结构指针
         * @return 是否支持该能力
         */
        virtual bool GetCapability(uint id,void *data){return false;}
    };//class PlugIn
}//namespace hgl
