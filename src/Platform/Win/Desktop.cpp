#include<hgl/platform/Desktop.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/2d/Bitmap.h>
#include<shellapi.h>
#include<shobjidl.h>
#include<Windows.h>
#include<wincodec.h>

namespace hgl::os
{
    /**
    * 弹出一个网页浏览器,并自动打开指定的网址。示例: PopupWebBrowser(L"http://www.hyzgame.com.cn");
    * @param url 网址
    */
    void PopupWebBrowser(const wchar_t *url)
    {
        ShellExecuteW(nullptr,nullptr,url,nullptr,nullptr,0);
    }

    /**
    * 弹出邮件客户端软件,并自动指定收件人的email地址。示例: PopupEmailClient(L"hyz@hyzgame.com",L"您好!");
    * @param email 电子邮件地址
    * @param subject 邮件主题
    */
    void PopupEmailClient(const WString &email,const WString &subject)
    {
        WString url=L"mailto:"+email+L"?Subject=\""+subject+L'\"';

        ShellExecuteW(nullptr,nullptr,url.c_str(),nullptr,nullptr,0);
    }

    /**
    * 创建快捷方式
    * @param sc 快捷方式配置
    * @return 是否成功
    */
    bool CreateShortCut(const ShortCut &sc)
    {
        IShellLinkW   *psl=nullptr;
        IPersistFile  *pPf=nullptr;

        WString lnk_filename=sc.short_cut_filename;

        lnk_filename += L".lnk";

        HRESULT hresult;

        hresult=CoCreateInstance(CLSID_ShellLink,nullptr,CLSCTX_INPROC_SERVER,IID_IShellLinkW,(LPVOID*)&psl);

        if(hresult!=S_OK)
            return(false);

        hresult=psl->QueryInterface(IID_IPersistFile,(LPVOID*)&pPf);

        if(hresult!=S_OK)
            return(false);

        if(!sc.work_directory.IsEmpty())
        {
            hresult=psl->SetWorkingDirectory(sc.work_directory.c_str());

            if(hresult!=S_OK)
                return(false);
        }

        if(!sc.param.IsEmpty())
        {
            hresult=psl->SetArguments(sc.param.c_str());

            if(hresult!=S_OK)
                return(false);
        }

        hresult=psl->SetPath(sc.source_filename.c_str());

        if(hresult!=S_OK)
            return(false);

        if(!sc.icon_filename.IsEmpty())
            if(filesystem::FileExist(sc.icon_filename))
            {
                hresult=psl->SetIconLocation(sc.icon_filename.c_str(),0);

                if(hresult!=S_OK)
                    return(false);
            }

        if(!sc.descript.IsEmpty())
        {
            hresult=psl->SetDescription(sc.descript.c_str());

            if(hresult!=S_OK)
                return(false);
        }

        hresult=pPf->Save(lnk_filename.c_str(),TRUE);

        if(hresult!=S_OK)
            return(false);

        if(pPf)
            pPf->Release();

        if(psl)
            psl->Release();

        return true;
    }

    bitmap::BitmapRGBA8 *CreateBitmap(HBITMAP &bmp)
    {
        if (!bmp)
            return nullptr;

        BITMAP bmpInfo;
        if (GetObject(bmp, sizeof(BITMAP), &bmpInfo) == 0)
            return nullptr;

        int width = bmpInfo.bmWidth;
        int height = bmpInfo.bmHeight;
        int bitCount = bmpInfo.bmBitsPixel; // 每像素位数

        bitmap::BitmapRGBA8 *icon_bitmap = new bitmap::BitmapRGBA8();

        if (!icon_bitmap->Create(width, height))
        {
            delete icon_bitmap;
            return nullptr; // 创建失败
        }

        // 准备 BITMAPINFO 结构体
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height; // 负值表示自顶向下
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        // 获取位图数据
        HDC hdc=GetDC(nullptr);
        HDC hdcMem=CreateCompatibleDC(hdc);
        HGDIOBJ oldBitmap=SelectObject(hdcMem,bmp);

        if (!GetDIBits(hdcMem, bmp, 0, height, icon_bitmap->GetData(), &bmi, DIB_RGB_COLORS))
        {
            delete icon_bitmap;
            icon_bitmap=nullptr;
        }

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdc);

        return icon_bitmap;
    }

    /**
    * 取得文件图标
    * @param filename 文件名
    * @param width 图标宽度
    * @param height 图标高度
    * @param color 图标色彩数
    * @return 图标象素数据(请自行delete[],另返回NULL表示失败)
    */
    bitmap::BitmapRGBA8 *GetFileIcon(const wchar_t *filename,int &width,int &height)
    {
        IShellItemImageFactory *pFactory = nullptr;
        IShellItem *pItem = nullptr;
        HBITMAP hBitmap = nullptr;

        // 创建ShellItem
        if (SHCreateItemFromParsingName(filename, nullptr, IID_PPV_ARGS(&pItem)) != S_OK)
            return nullptr;

        // 获取ImageFactory接口
        if (pItem->QueryInterface(IID_PPV_ARGS(&pFactory)) != S_OK)
        {
            pItem->Release();
            return nullptr;
        }

        SIZE size = {width,height}; // 期望尺寸
        SIIGBF flags = SIIGBF_RESIZETOFIT
                        | SIIGBF_BIGGERSIZEOK;

        if (pFactory->GetImage(size, flags, &hBitmap) != S_OK)
            hBitmap = nullptr;

        pFactory->Release();
        pItem->Release();

        if (!hBitmap)
            return nullptr;

        return CreateBitmap(hBitmap);
    }
}//namespace hgl::os
