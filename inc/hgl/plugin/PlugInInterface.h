#pragma once

#include<hgl/type/DataType.h>

namespace hgl
{
    typedef void        (*ClosePlugInPROC)();                                                       ///<关闭插件

    typedef uint32      (*GetPlugInVersionPROC)();                                                  ///<取得插件版本号
    typedef u16char *   (*GetPlugInIntroPROC)();                                                    ///<取得插件介绍信息

    typedef bool        (*GetPlugInInterfacePROC)(uint32,void *);                                   ///<取得插件接口
    typedef bool        (*GetCapabilityPROC)(uint32,void *);                                        ///<取得插件能力

    /**
     * 插件能力ID
     */
    enum class PlugInCapability : uint32
    {
        None            = 0,
        FileExtensions  = 1,            ///<支持的文件扩展名列表，data 为 PlugInFileExtensions*
        MenuInfo        = 2,            ///<菜单显示信息，data 为 PlugInMenuInfo*
    };//enum class PlugInCapability

    /**
     * 文件格式插件上报的扩展名列表（纯C布局，跨DLL边界安全）
     */
    struct PlugInFileExtensions
    {
        uint32              count;      ///<扩展名数量
        const char *const  *items;      ///<扩展名数组(不含点号，如 "wav"/"ogg")
    };//struct PlugInFileExtensions

    /**
     * 过滤器插件上报的菜单信息（纯C布局，跨DLL边界安全）
     */
    struct PlugInMenuInfo
    {
        const u16char      *display_name;   ///<菜单显示名
        const u16char      *category;       ///<分组名(可为nullptr)
        int                 priority;       ///<排序优先级(越大越靠前)
    };//struct PlugInMenuInfo

    struct PlugInInterface
    {
        ClosePlugInPROC             ClosePlugIn;

        GetPlugInVersionPROC        GetVersion;
        GetPlugInIntroPROC          GetIntro;

        GetPlugInInterfacePROC      GetInterface;
        GetCapabilityPROC           GetCapability;
    };//struct PlugInInterface

    typedef PlugInInterface *(*InitPlugInPROC)();                                                   ///<初始化插件

    /**
     * 内建(静态链接)插件登记描述：REGISTER_PLUG_IN 在静态链接模式下登记的内容。
     * 纯 C 布局，静态初始化期可用。
     */
    struct StaticPlugInDesc
    {
        const os_char    *name;          ///<插件内部名(如 OS_TEXT("Wav"))
        const os_char    *category;      ///<类目名(如 OS_TEXT("Audio"))
        InitPlugInPROC     init;          ///<初始化回调，返回 PlugInInterface*
    };//struct StaticPlugInDesc

    /**
     * 内建插件登记器：构造时把自身挂入进程内全局链表，
     * 由 PlugInManager 在初始化期统一遍历并注册。
     */
    struct StaticPlugInRegistrar
    {
        StaticPlugInRegistrar *next;
        StaticPlugInDesc       desc;

        StaticPlugInRegistrar(const StaticPlugInDesc &d):next(nullptr),desc(d)
        {
            next=GetHead();
            GetHead()=this;
        }

        static StaticPlugInRegistrar *&GetHead()
        {
            static StaticPlugInRegistrar *head=nullptr;
            return head;
        }
    };//struct StaticPlugInRegistrar
}//namespace hgl

/**
 * 让同一份插件源码既可编译为 DLL/SO(由文件系统扫描发现)，
 * 也可静态链接进程序(由本宏在静态初始化期登记为内建插件)。
 *
 * 用法(放在 InitPlugIn 定义之后)：
 *     REGISTER_PLUG_IN(Audio, Wav)
 *
 * 定义 __MAKE_PLUGIN__(编译为 DLL/SO)时本宏为空；
 * 否则(静态链接)向全局链表登记一条 StaticPlugInDesc。
 */
#ifdef __MAKE_PLUGIN__
    #define REGISTER_PLUG_IN(category,name)             /* DLL/SO 模式：无需静态登记 */
#else
    #define REGISTER_PLUG_IN(category,name) \
        static ::hgl::StaticPlugInRegistrar hgl_static_plugin_##name( \
            ::hgl::StaticPlugInDesc{ OS_TEXT(#name), OS_TEXT(#category), &InitPlugIn });
#endif
