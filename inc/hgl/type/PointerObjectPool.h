#pragma once

#include<hgl/type/Stack.h>

namespace hgl
{
    // AI NOTE: Pool of owning pointers for non-trivial objects.
    // Acquire returns an existing idle object; no auto-allocation.
    /**
     * 指针对象池 - 用于 non-trivial 类型（如含有复杂初始化逻辑的类）
     * 维护指针列表，支持动态增长，自动调用构造/析构函数
     * 适用场景：non-trivial 类型、需要动态增长、需要完整生命周期管理
     *
     * 使用方式：
     *  1. Init() - 初始化空池
     *  2. AddObject(obj) - 手动添加预创建的对象到池（用户负责构造参数）
     *  3. Acquire() - 获取对象指针（池空时创建新对象，但新对象仅调用无参构造）
     *  4. Release(ptr) - 归还对象指针（对象保持有效，等待重用）
     *  5. 析构时自动清理所有对象
     *
     * @tparam T 对象类型（非指针，如 MyClass、SpatialAudioSource 等）
     */
    template<typename T> class PointerObjectPool
    {
    protected:

        Stack<T*> idle_objects;             ///<闲置对象列表

    public:

        PointerObjectPool()
        {
        }

        /**
         * 初始化对象池
         * @return 成功返回 true
         */
        bool Init()
        {
            // 只初始化空池，不预分配对象
            // 用户需通过 AddObject() 手动添加对象
            return true;
        }

        ~PointerObjectPool()
        {
            // 清理所有闲置对象
            T *obj;
            while(idle_objects.Pop(obj))
            {
                delete obj;
            }
        }

        /**
         * 添加对象到池中
         * @param obj 对象指针（由用户创建，带有合适的参数）
         * @return 成功返回 true
         */
        bool AddObject(T *obj)
        {
            if(!obj)
                return false;

            return idle_objects.Push(obj);
        }

        /**
         * 从池中获取一个对象
         * @return 对象指针。如果池有闲置对象则返回闲置对象，否则返回 nullptr
         *         （用户需通过 AddObject() 确保池中有足够对象，或自行处理 nullptr 情况）
         */
        T *Acquire()
        {
            T *obj;
            if(idle_objects.Pop(obj))
                return obj;
            return nullptr;  // 池空返回 nullptr，不尝试创建
        }

        /**
         * 将对象归还到池中
         * @param obj 对象指针（必须是通过 Acquire() 或 AddObject() 加入的）
         * @return 成功返回 true
         */
        bool Release(T *obj)
        {
            if(!obj)
                return false;

            return idle_objects.Push(obj);
        }

        /**
         * 获取当前闲置对象数
         * @return 闲置对象数量
         */
        int GetIdleCount() const
        {
            return idle_objects.GetCount();
        }
    };//template<typename T> class PointerObjectPool
}//namespace hgl
