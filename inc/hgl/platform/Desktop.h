#pragma once

#include<hgl/type/String.h>
#include<hgl/math/Vector.h>

namespace hgl::bitmap
{
    template<typename T,uint C> class Bitmap;
    using BitmapRGBA8=Bitmap<Vector4u8,4>;
}

/**
* 与本地操作系统相关功能函数的名字空间
*/
namespace hgl::os        ///操作系统相关功能名字空间
{
#if HGL_OS == HGL_OS_Windows
    void PopupWebBrowser(const OSString &);                                                     ///<弹出网页浏览器
    void PopupEmailClient(const OSString &,const OSString &sub);                                ///<弹出电子邮件客户端

    /**
    * 快捷方式数据结构
    */
    struct ShortCut
    {
        OSString short_cut_filename;    ///<注意不要带扩展名
        OSString source_filename;       ///<原文件名称
        OSString work_directory;        ///<工作目录
        OSString param;                 ///<参数
        OSString icon_filename;         ///<图标文件
        OSString descript;              ///<备注
    };//struct ShortCut

    bool CreateShortCut(const ShortCut &);                                                      ///<创建快捷方式

    bitmap::BitmapRGBA8 *GetFileIcon(const os_char *,int &width,int &height);                   ///<取得文件的系统图标(仅Vista以上可用)
#endif//HGL_OS == HGL_OS_Windows

#if (HGL_OS==HGL_OS_Windows)||(HGL_OS==HGL_OS_MacOS)
    void CopyTextToClipboard(const os_char *);                                                  ///<复制字符串到剪贴板
    const os_char *GetTextFromClipboard();                                                      ///<从剪贴板取得字符串
#endif//(HGL_OS==HGL_OS_Windows)||(HGL_OS==HGL_OS_MacOS)
}//namespace hgl::os
