#pragma once

#include<vulkan/vulkan.h>

namespace hgl
{
    class Window;

    VkSurfaceKHR CreateVulkanSurface(VkInstance,Window *);
}//namespace hgl
