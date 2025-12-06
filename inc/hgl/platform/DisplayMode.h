#pragma once

namespace hgl
{
    bool GetDesktopDisplayMode(int &width,int &height,int &bit,int &vsync);
	bool SetDisplayMode(int width,int height,int bit,int vsync);
	bool RestartDisplayMode();
}//namespace hgl
