#include<hgl/object/ObjectTracker.h>

namespace hgl::utils
{
    // 全局追踪器实例定义 - 单实例
    AllocationTracker<OBJECT_TRACKER_CAPACITY>* g_object_tracker = nullptr;

#if HGL_ENABLE_OBJECT_TRACKER

    // 显式实例化模板
    template class AllocationTracker<OBJECT_TRACKER_CAPACITY>;

    // ScopeCapture 实现
    ScopeCapture::ScopeCapture(const std::source_location& loc) noexcept
    {
        if (g_object_tracker)
        {
            AllocationTracker<OBJECT_TRACKER_CAPACITY>::allocation_stack.push_back(SourceLocation(loc));
        }
    }

    ScopeCapture::~ScopeCapture() noexcept
    {
        if (g_object_tracker && !AllocationTracker<OBJECT_TRACKER_CAPACITY>::allocation_stack.empty())
        {
            AllocationTracker<OBJECT_TRACKER_CAPACITY>::allocation_stack.pop_back();
        }
    }

    // 初始化全局对象追踪器
    void initialize_object_tracker()
    {
        if (!g_object_tracker)
        {
            g_object_tracker = new AllocationTracker<OBJECT_TRACKER_CAPACITY>();
        }
    }

    // 清理全局对象追踪器
    void shutdown_object_tracker()
    {
        if (g_object_tracker)
        {
            delete g_object_tracker;
            g_object_tracker = nullptr;
        }
    }

    uint32_t get_current_allocation_stack(SourceLocation *out, uint32_t max_depth) noexcept
    {
        if (!g_object_tracker)
            return 0;

        return AllocationTracker<OBJECT_TRACKER_CAPACITY>::get_current_stack(out, max_depth);
    }

#else // HGL_ENABLE_OBJECT_TRACKER disabled

    // 禁用模式下的空实现
    ScopeCapture::ScopeCapture(const std::source_location&) noexcept {}
    ScopeCapture::~ScopeCapture() noexcept {}
    void initialize_object_tracker() {}
    void shutdown_object_tracker() {}
    uint32_t get_current_allocation_stack(SourceLocation*, uint32_t) noexcept { return 0; }

#endif // HGL_ENABLE_OBJECT_TRACKER

} // namespace hgl::utils
