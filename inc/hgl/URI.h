#ifndef HGL_URI_INCLUDE
#define HGL_URI_INCLUDE

#include<hgl/type/String.h>
namespace hgl
{
    const uint16 GetSchemePort(const char *scheme);
        
    /**
     * 统一地址标识符类<br>
     * <p>注：此类只处理名称，与真实文件系统/网络系统没有任何相关操作</p>
     * 
     * <p>example 1:
     * http:// www.hyzgame.com / index      .php
     * ------- ---------------   -----       ---
     * scheme   path            main_name  ext_name
     * 
     * scheme:      http
     * path:        www.hyzgame.com
     * main_name:   index
     * ext_name:    php
     * </p>
     * 
     * <p> example 2:
     * file:// C:/windows/system32/1234.dll
     * 
     * scheme:      file
     * driver:      C
     * path:        Windows\System32
     * main_name:   1234
     * ext_name:    dll
     * fullname:    C:\windows\system32\1234.dll
     * </p>
     * 
     * <p> example 3:
     * C:\windows\system32\1234.dll
     * 
     * scheme:
     * driver:      C
     * path:        Windows\System32
     * main_name:   1234
     * ext_name:    dll
     * fullname:    C:\windows\system32\1234.dll
     * </p>
     */
    class URI
    {
    public:
        
        enum class SchemeType
        {
            Unknow=0,
            LocalFile,                  ///<本地文件系统
            ArachivesFile,              ///<档案文件(压缩包/tar/ISO镜像之类)
            Device,                     ///<设备虚拟文件
            Network,                    ///<网络
        };

    protected:

        OSString    origin_name;
        
        OSString    scheme_name;        ///<协议名称(如http,ftp之类)
        SchemeType  scheme_type;        ///<协议
        
        UTF8String  network_host;       ///<网络主机(IP或网址)
        uint16      network_port;       ///<网络协议默认端口

        OSString    fullname;           ///<完整文件名
        
        OSString    path_name;          ///<路径名
        OSString    main_name;          ///<主名称
        OSString    ext_name;           ///<文件扩展名
        
        UTF8String  url;

    public:

        URI(const OSString &);
        URI(const OSString &scheme,const OSString &path,const OSString &filename);
        ~URI()=default;

        const bool isFile();
        const bool isDevice();
        const bool isNetwork();

    public:

        const OSString &path();
        const OSString &main();
        const OSString &ext();
        const OSString &full();

        const UTF8String &URL();
    };//class Filename
}//namespace hgl
#endif//HGL_URI_INCLUDE
