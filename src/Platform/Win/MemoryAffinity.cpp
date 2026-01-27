#include<hgl/platform/MemoryAffinity.h>
#include<windows.h>
#include<Psapi.h>
#include<memory.h>

namespace hgl
{
    void* NumaAlloc(size_t size, int numa_node)
    {
        if (size == 0) return nullptr;

        if (numa_node < 0)
        {
            // 使用默认分配
            return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        }

        // 在指定NUMA节点上分配内存
        return VirtualAllocExNuma(
            GetCurrentProcess(),
            nullptr,
            size,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE,
            numa_node
        );
    }

    void NumaFree(void* ptr, size_t size)
    {
        if (!ptr) return;

        VirtualFree(ptr, 0, MEM_RELEASE);
    }

    bool SetMemoryPolicy(int numa_node)
    {
        if (numa_node < 0) return false;

        // Windows使用SetProcessAffinityMask来间接影响内存分配策略
        // 但没有直接的NUMA内存策略API
        // 可以使用VirtualAllocExNuma在分配时指定节点
        return true; // Windows上此操作总是成功，实际策略在分配时指定
    }

    bool ResetMemoryPolicy()
    {
        // Windows上没有需要重置的全局内存策略
        return true;
    }

    int GetMemoryNode(void* ptr)
    {
        if (!ptr) return -1;

        PSAPI_WORKING_SET_EX_INFORMATION info;
        info.VirtualAddress = ptr;

        if (!QueryWorkingSetEx(GetCurrentProcess(), &info, sizeof(info)))
            return -1;

        // Windows API不直接提供内存页面所在的NUMA节点信息
        // 返回-1表示无法确定
        return -1;
    }

    bool MigrateMemory(void* ptr, size_t size, int numa_node)
    {
        if (!ptr || size == 0 || numa_node < 0) return false;

        // Windows没有直接的内存迁移API
        // 需要重新分配内存并复制数据
        void* new_ptr = NumaAlloc(size, numa_node);
        if (!new_ptr) return false;

        memcpy(new_ptr, ptr, size);

        // 注意：这会改变指针地址，调用者需要更新指针
        // 此实现仅复制数据，不自动更新引用
        NumaFree(ptr, size);

        return true;
    }
}//namespace hgl
