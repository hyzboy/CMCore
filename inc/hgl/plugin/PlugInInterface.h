#ifndef HGL_PLUG_IN_INTERFACE_INCLUDE
#define HGL_PLUG_IN_INTERFACE_INCLUDE

#include<hgl/type/DataType.h>
namespace hgl
{
    typedef void        (*ClosePlugInPROC)();                                                       ///<关闭插件

    typedef uint32      (*GetPlugInVersionPROC)();                                                  ///<取得插件版本号
    typedef u16char *   (*GetPlugInIntroPROC)();                                                    ///<取得插件介绍信息

    typedef bool        (*GetPlugInInterfacePROC)(uint32,void *);                                   ///<取得插件接口
    typedef void        (*SetPlugInInterfacePROC)(void *,void *);                                   ///<设置插件接口

    typedef bool        (*LoadInterfaceByNamePROC)(const u16char *,uint32,void *);                  ///<取得接口
    typedef bool        (*LoadInterfaceByIndexPROC)(uint32,uint32,void *);                          ///<取得接口

    struct PlugInInterface
    {
        ClosePlugInPROC             Close;

        GetPlugInVersionPROC        GetVersion;
        GetPlugInIntroPROC          GetIntro;

        GetPlugInInterfacePROC      GetInterface;
        SetPlugInInterfacePROC      SetInterface;

        LoadInterfaceByNamePROC     LoadByName;
        LoadInterfaceByIndexPROC    LoadByIndex;
    };//struct PlugInInterface

    typedef PlugInInterface *(*InitPlugInPROC)();                                                   ///<初始化插件
}//namespace hgl
#endif//HGL_PLUG_IN_INTERFACE_INCLUDE
