#include<hgl/platform/MemoryAffinity.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>

// NUMA support (optional, may not be available on all systems)
#ifdef __has_include
#  if __has_include(<numa.h>)
#    define HGL_HAS_NUMA 1
#    include<numa.h>
#  endif
#  if __has_include(<numaif.h>)
#    include<numaif.h>
#  endif
#endif

namespace hgl
{
    void* NumaAlloc(size_t size, int numa_node)
    {
        if (size == 0) return nullptr;

#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            if (numa_node < 0)
            {
                // 使用默认策略
                return numa_alloc(size);
            }
            else
            {
                // 在指定节点上分配
                return numa_alloc_onnode(size, numa_node);
            }
        }
#endif

        // 回退到标准分配
        // 使用mmap以支持大页面和更好的控制
        void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        
        if (ptr == MAP_FAILED)
            return nullptr;

        return ptr;
    }

    void NumaFree(void* ptr, size_t size)
    {
        if (!ptr) return;

#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            numa_free(ptr, size);
            return;
        }
#endif

        // 使用munmap释放
        munmap(ptr, size);
    }

    bool SetMemoryPolicy(int numa_node)
    {
        if (numa_node < 0) return false;

#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            // 设置内存绑定策略
            struct bitmask *nodemask = numa_allocate_nodemask();
            numa_bitmask_clearall(nodemask);
            numa_bitmask_setbit(nodemask, numa_node);
            
            numa_set_membind(nodemask);
            
            numa_free_nodemask(nodemask);
            return true;
        }
#endif

        return false;
    }

    bool ResetMemoryPolicy()
    {
#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            // 恢复默认策略
            numa_set_localalloc();
            return true;
        }
#endif

        return false;
    }

    int GetMemoryNode(void* ptr)
    {
        if (!ptr) return -1;

#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            int node = -1;
            if (get_mempolicy(&node, nullptr, 0, ptr, MPOL_F_NODE | MPOL_F_ADDR) == 0)
            {
                return node;
            }
        }
#endif

        return -1;
    }

    bool MigrateMemory(void* ptr, size_t size, int numa_node)
    {
        if (!ptr || size == 0 || numa_node < 0) return false;

#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            // 创建节点掩码
            struct bitmask *from_nodes = numa_allocate_nodemask();
            struct bitmask *to_nodes = numa_allocate_nodemask();
            
            numa_bitmask_setall(from_nodes);  // 从所有节点
            numa_bitmask_clearall(to_nodes);
            numa_bitmask_setbit(to_nodes, numa_node);  // 到指定节点
            
            // 迁移内存页面
            numa_migrate_pages(0, from_nodes, to_nodes);
            
            numa_free_nodemask(from_nodes);
            numa_free_nodemask(to_nodes);
            
            return true;
        }
#endif

        return false;
    }
}//namespace hgl
