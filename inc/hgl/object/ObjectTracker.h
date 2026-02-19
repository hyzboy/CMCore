#pragma once

// ObjectTracker 完整的 header-only 实现

#include<cstdint>
#include<atomic>
#include<source_location>

#if !defined(__cpp_lib_source_location)
namespace std
{
    struct source_location
    {
        static constexpr source_location current() noexcept { return source_location(); }
        constexpr const char* file_name() const noexcept { return ""; }
        constexpr const char* function_name() const noexcept { return ""; }
        constexpr uint_least32_t line() const noexcept { return 0; }
        constexpr uint_least32_t column() const noexcept { return 0; }
    };
}
#endif

// Set to 1 to enable ObjectTracker.

// 这个内存消耗巨大，会导致程序启动变慢，以及某些调试器或分析器认为程序长期连接不上而失效（如 RenderDoc)

#ifndef HGL_ENABLE_OBJECT_TRACKER
#define HGL_ENABLE_OBJECT_TRACKER 0
#endif

constexpr uint32_t OBJECT_TRACKER_CAPACITY=1000000;

#if HGL_ENABLE_OBJECT_TRACKER

#include<hgl/core/ObjectType.h>
#include<cstdint>
#include<atomic>
#include<vector>
#include<mutex>
#include<chrono>
#include<fstream>
#include<cstring>
#include<source_location>

namespace hgl::utils
{
    /**
     * 源位置捕获结构
     */
    struct SourceLocation
    {
        const char* file;
        uint32_t line;
        uint32_t column;
        const char* function;

        SourceLocation() = default;

        SourceLocation(const std::source_location& loc)
            : file(loc.file_name())
            , line(loc.line())
            , column(loc.column())
            , function(loc.function_name())
        {
        }
    };

    /**
     * 对象分配事件
     */
    struct AllocationEvent
    {
        uint64_t object_id;
        uint64_t timestamp;
        hgl::core::ObjectTypeTag object_type;
        char object_name[64];
        uint32_t stack_depth;
        SourceLocation stack[64];

        AllocationEvent()
            : object_id(0), timestamp(0), object_type(hgl::core::ObjectTypeTag::None),
              stack_depth(0)
        {
            std::memset(object_name, 0, sizeof(object_name));
        }
    };

    /**
     * 对象ID生成器 - 原子自增，无锁
     */
    class ObjectIdGenerator
    {
    private:
        std::atomic<uint64_t> next_id{1};

    public:
        ObjectIdGenerator() = default;
        ~ObjectIdGenerator() = default;

        uint64_t allocate() noexcept
        {
            return next_id.fetch_add(1, std::memory_order_relaxed);
        }
    };

    // 前向声明
    class ScopeCapture;

    /**
     * 本地内存分配追踪器 - 环形缓冲模式
     */
    template<size_t CAPACITY = OBJECT_TRACKER_CAPACITY>
    class AllocationTracker
    {
    private:
        std::vector<AllocationEvent> buffer;
        std::atomic<size_t> write_pos{0};
        std::mutex lock;

        inline static ObjectIdGenerator id_generator;
        inline static thread_local std::vector<SourceLocation> allocation_stack;

    public:
        AllocationTracker()
        {
            buffer.resize(CAPACITY);
        }

        ~AllocationTracker() = default;

        uint64_t record_allocation(
            const char* object_name,
            hgl::core::ObjectTypeTag object_type
        ) noexcept
        {
            uint64_t object_id = id_generator.allocate();

            AllocationEvent evt;
            evt.object_id = object_id;
            evt.timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            evt.object_type = object_type;
            evt.stack_depth = (uint32_t)allocation_stack.size();

            if (object_name)
            {
                std::strncpy(evt.object_name, object_name, sizeof(evt.object_name) - 1);
                evt.object_name[sizeof(evt.object_name) - 1] = '\0';
            }

            size_t copy_depth = std::min(allocation_stack.size(), size_t(64));
            std::memcpy(evt.stack, allocation_stack.data(),
                       copy_depth * sizeof(SourceLocation));
            evt.stack_depth = copy_depth;

            {
                std::lock_guard<std::mutex> g(lock);
                size_t pos = write_pos.fetch_add(1, std::memory_order_relaxed) % CAPACITY;
                buffer[pos] = evt;
            }

            return object_id;
        }

        void dump_to_file(const std::string& filename) noexcept
        {
            try
            {
                std::ofstream f(filename, std::ios::binary);
                if (!f.is_open())
                {
                    std::fputs("[ObjectTracker] Failed to open file for dump\n", stderr);
                    return;
                }

                {
                    std::lock_guard<std::mutex> g(lock);
                    for (const auto& evt : buffer)
                    {
                        if (evt.object_id == 0)
                            continue;

                        f.write((const char*)&evt.object_id, sizeof(evt.object_id));
                        f.write((const char*)&evt.timestamp, sizeof(evt.timestamp));
                        f.write((const char*)&evt.object_type, sizeof(evt.object_type));
                        f.write(evt.object_name, sizeof(evt.object_name));
                        f.write((const char*)&evt.stack_depth, sizeof(evt.stack_depth));

                        for (uint32_t i = 0; i < evt.stack_depth; i++)
                        {
                            uint64_t file_hash = (uint64_t)(uintptr_t)evt.stack[i].file;
                            uint64_t func_hash = (uint64_t)(uintptr_t)evt.stack[i].function;

                            f.write((const char*)&file_hash, sizeof(file_hash));
                            f.write((const char*)&evt.stack[i].line, sizeof(evt.stack[i].line));
                            f.write((const char*)&evt.stack[i].column, sizeof(evt.stack[i].column));
                            f.write((const char*)&func_hash, sizeof(func_hash));
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::fprintf(stderr, "[ObjectTracker] Exception: %s\n", e.what());
            }
        }

        static uint32_t get_current_stack(SourceLocation *out, uint32_t max_depth) noexcept
        {
            if (!out || max_depth == 0)
                return 0;

            const size_t depth = allocation_stack.size();
            const size_t copy_depth = std::min(depth, static_cast<size_t>(max_depth));
            if (copy_depth == 0)
                return 0;

            std::memcpy(out, allocation_stack.data(), copy_depth * sizeof(SourceLocation));
            return static_cast<uint32_t>(copy_depth);
        }

    private:
        friend class ScopeCapture;
    };

    // 全局追踪器实例 - extern 声明，单实例
    extern AllocationTracker<OBJECT_TRACKER_CAPACITY>* g_object_tracker;

    /**
     * 栈捕获RAII助手 - 在 AllocationTracker 定义后
     */
    class ScopeCapture
    {
    public:
        ScopeCapture(const std::source_location& loc = std::source_location::current()) noexcept;
        ~ScopeCapture() noexcept;

        ScopeCapture(const ScopeCapture&) = delete;
        ScopeCapture& operator=(const ScopeCapture&) = delete;
    };

    /**
     * 初始化全局对象追踪器
     */
    void initialize_object_tracker();

    /**
     * 清理全局对象追踪器
     */
    void shutdown_object_tracker();

    uint32_t get_current_allocation_stack(SourceLocation *out, uint32_t max_depth) noexcept;

} // namespace hgl::utils

// 便利宏
#define HGL_TRACK_ALLOCATION(name, type) \
    (hgl::utils::g_object_tracker ? hgl::utils::g_object_tracker->record_allocation(name, type) : 0)

#define HGL_CAPTURE_SCOPE() \
    hgl::utils::ScopeCapture __scope_capture__

#else // HGL_ENABLE_OBJECT_TRACKER

namespace hgl::utils
{
    // 前向声明，禁用模式下使用空实现
    template<size_t CAPACITY = OBJECT_TRACKER_CAPACITY>
    class AllocationTracker;

    /**
     * 对象ID生成器 - 原子自增，无锁
     */
    class ObjectIdGenerator
    {
    private:
        std::atomic<uint64_t> next_id{1};

    public:
        ObjectIdGenerator() = default;
        ~ObjectIdGenerator() = default;

        uint64_t allocate() noexcept
        {
            return next_id.fetch_add(1, std::memory_order_relaxed);
        }
    };

    struct SourceLocation
    {
        const char* file = nullptr;
        uint32_t line = 0;
        uint32_t column = 0;
        const char* function = nullptr;

        SourceLocation() = default;

        SourceLocation(const std::source_location& loc)
            : file(loc.file_name())
            , line(loc.line())
            , column(loc.column())
            , function(loc.function_name())
        {
        }
    };

    class ScopeCapture
    {
    public:
        ScopeCapture(const std::source_location& loc = std::source_location::current()) noexcept;
        ~ScopeCapture() noexcept;
        ScopeCapture(const ScopeCapture&) = delete;
        ScopeCapture& operator=(const ScopeCapture&) = delete;
    };

    extern AllocationTracker<OBJECT_TRACKER_CAPACITY>* g_object_tracker;

    void initialize_object_tracker();
    void shutdown_object_tracker();
    uint32_t get_current_allocation_stack(SourceLocation*, uint32_t) noexcept;
}

#define HGL_TRACK_ALLOCATION(name, type) 0
#define HGL_CAPTURE_SCOPE()

#endif // HGL_ENABLE_OBJECT_TRACKER

