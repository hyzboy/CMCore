#pragma once

#include<stdint.h>
#include<stddef.h>
#include<type_traits>

namespace hgl
{
    /**
     * 在指定NUMA节点上分配内存
     * @param size 要分配的内存大小(字节)
     * @param numa_node NUMA节点ID，-1表示使用默认节点
     * @return 分配的内存指针，失败返回nullptr
     */
    void* NumaAlloc(size_t size, int numa_node = -1);

    /**
     * 释放NUMA内存
     * @param ptr 要释放的内存指针
     * @param size 内存大小(字节)
     */
    void NumaFree(void* ptr, size_t size);

    /**
     * 在指定NUMA节点上分配对象
     * @tparam T 对象类型
     * @tparam Args 构造函数参数类型
     * @param numa_node NUMA节点ID，-1表示使用默认节点
     * @param args 构造函数参数
     * @return 分配的对象指针，失败返回nullptr
     */
    template<typename T, typename... Args>
    T* NumaNew(int numa_node, Args&&... args)
    {
        void* ptr = NumaAlloc(sizeof(T), numa_node);
        if (!ptr) return nullptr;

        return new (ptr) T(static_cast<Args&&>(args)...);
    }

    /**
     * 释放NUMA对象
     * @tparam T 对象类型
     * @param ptr 要释放的对象指针
     */
    template<typename T>
    void NumaDelete(T* ptr)
    {
        if (!ptr) return;

        ptr->~T();
        NumaFree(ptr, sizeof(T));
    }

    /**
     * 在指定NUMA节点上分配数组
     * @tparam T 数组元素类型
     * @param count 数组元素数量
     * @param numa_node NUMA节点ID，-1表示使用默认节点
     * @return 分配的数组指针，失败返回nullptr
     */
    template<typename T>
    T* NumaNewArray(size_t count, int numa_node = -1)
    {
        if (count == 0) return nullptr;

        size_t total_size = sizeof(T) * count;
        void* ptr = NumaAlloc(total_size, numa_node);
        if (!ptr) return nullptr;

        T* array = static_cast<T*>(ptr);

        // 对于非平凡类型，需要调用构造函数
        if (!std::is_trivially_constructible<T>::value)
        {
            for (size_t i = 0; i < count; ++i)
            {
                new (&array[i]) T();
            }
        }

        return array;
    }

    /**
     * 释放NUMA数组
     * @tparam T 数组元素类型
     * @param ptr 要释放的数组指针
     * @param count 数组元素数量
     */
    template<typename T>
    void NumaDeleteArray(T* ptr, size_t count)
    {
        if (!ptr || count == 0) return;

        // 对于非平凡类型，需要调用析构函数
        if (!std::is_trivially_destructible<T>::value)
        {
            for (size_t i = 0; i < count; ++i)
            {
                ptr[i].~T();
            }
        }

        NumaFree(ptr, sizeof(T) * count);
    }

    /**
     * 设置内存策略，将内存优先分配到指定的NUMA节点
     * @param numa_node NUMA节点ID
     * @return 是否设置成功
     */
    bool SetMemoryPolicy(int numa_node);

    /**
     * 恢复默认内存策略
     * @return 是否恢复成功
     */
    bool ResetMemoryPolicy();

    /**
     * 获取指定内存地址所在的NUMA节点
     * @param ptr 内存地址
     * @return NUMA节点ID，失败返回-1
     */
    int GetMemoryNode(void* ptr);

    /**
     * 将内存页面迁移到指定的NUMA节点
     * @param ptr 内存地址
     * @param size 内存大小(字节)
     * @param numa_node 目标NUMA节点ID
     * @return 是否迁移成功
     */
    bool MigrateMemory(void* ptr, size_t size, int numa_node);
}//namespace hgl
