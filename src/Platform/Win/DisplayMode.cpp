#include<wchar.h>
#include<windows.h>
#include<hgl/platform/DisplayMode.h>

namespace hgl
{
	bool SetDisplayMode(int width,int height,int bit,int vsync)
	{
		DEVMODE VideoMode;

		memset(&VideoMode,0,sizeof(DEVMODE));

		VideoMode.dmSize            =sizeof(DEVMODE);
		VideoMode.dmPelsWidth 		=width;
		VideoMode.dmPelsHeight		=height;
		VideoMode.dmBitsPerPel		=bit;
		VideoMode.dmDisplayFrequency=vsync;
		VideoMode.dmFields          =DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL|DM_DISPLAYFREQUENCY;

		if(ChangeDisplaySettings(&VideoMode,CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL)
			return(true);

		VideoMode.dmFields          =DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL;

		return(ChangeDisplaySettings(&VideoMode,CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL);
	}
	
	/**
	* 取得当前桌面显示模式
	* @param width 宽
	* @param height 高
	* @param bit 色彩位数
	* @param vsync 垂直刷新率
	* @return 是否成功
	*/
	bool GetDesktopDisplayMode(int &width,int &height,int &bit,int &vsync)
	{
		DEVMODE VideoMode;

		if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&VideoMode))
			return(false);

		width	=VideoMode.dmPelsWidth;
		height	=VideoMode.dmPelsHeight;
		bit		=VideoMode.dmBitsPerPel;
		vsync	=VideoMode.dmDisplayFrequency;

		return(true);
	}

	bool RestartDisplayMode()
	{
		return(ChangeDisplaySettings(NULL,NULL)==DISP_CHANGE_SUCCESSFUL);
	}
}//namespace hgl
