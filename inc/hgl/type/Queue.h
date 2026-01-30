/**
 * Queue.h 实现详解（设计与原理概述）
 *
 * 1) 设计目标
 * - 提供一个高性能、可扩展的队列（FIFO），兼顾“批量追加”和“顺序弹出”。
 * - 统一为单一容器 `Queue<T>`：按值存储、仅支持平凡可拷贝类型（trivially copyable）。
 * - 对于非平凡类型，请使用指针形式 `Queue<T*>`，由调用方显式管理对象生命周期。
 *
 * 2) 核心数据结构
 * - `std::vector<T> data_array[2]`：读段(read)与写段(write)的双缓冲结构。
 *   - `read_index` 指向当前读段，`write_index` 指向当前写段。
 *   - `read_offset` 标记读段的起始读取位置（避免每次 Pop 都整体搬移数据）。
 * - 初始化：`write_index=0, read_index=1, read_offset=0`。
 *   - 首次 Push 写入 data_array[0]，Pop 优先从 data_array[1] 读取；当读段为空且写段有数据时触发段切换（见 SwapIndex）。
 *
 * 3) 段切换策略（SwapIndex）
 * - 当读段已读尽（`read_offset >= data_array[read_index].GetCount()`）且写段非空：
 *   - 清空旧读段（`data_array[read_index].Clear()`），交换 read/write 段，`read_offset` 复位为 0。
 *   - 保证顺序弹出的成本近似 O(1)，避免整体数据拼接/搬移。
 *
 * 4) Push / Pop / Peek 的语义与复杂度
 * - `Push(const T &)` / `Push(T *data,int count)` 始终追加到写段尾部，扩容由 `std::vector<T>` 负责，摊销近 O(1)。
 * - `Peek(T &)`：只读不前进。若读段可读则从 `read_offset` 读取，否则若写段非空则读取写段首元素；两段皆空返回 false。
 * - `Pop(T &)`：顺序弹出并前进 `read_offset`。读段耗尽且写段非空时会先段切换；两段皆空返回 false。
 * - 复杂度：Push/Peek/Pop 在多数情形下均为 O(1) 摊销；段切换为 O(1)。
 *
 * 5) 迭代器（只读）
 * - 提供只读 `ConstIterator`，遍历队列的逻辑视图：
 *   - `begin()`：若读段仍有未读数据，则从 `read_offset` 开始；否则直接从写段开头开始。
 *   - `end()`：对齐到写段末尾位置。
 *   - `++`：读段迭代到末尾后自动切换到写段，从 0 继续，不越界、不回退。
 * - 迭代与 Push/Pop 解耦：不会修改 `read_offset`，也不会触发段切换（只读视角）。
 *
 * 6) 常用查询
 * - `Contains(const T&)`：在读段的未读区间（从 `read_offset` 起）与写段全区间进行顺序查找。
 * - `GetCount()`：逻辑元素总数 = 读段元素数 + 写段元素数 - `read_offset`。
 * - `GetAllocCount()`：返回两段底层数组已分配容量之和，便于观测内存预留情况。
 *
 * 7) GetArray 与未读快照（Unread Snapshot）
 * - `GetArray()`：返回当前“写段”的引用（便于与 Active 接口兼容的场景）。
 * - `GetUnreadSnapshot()`：返回队列的稳定快照（`std::vector<T>`），按顺序合并“读段的未读部分”与“写段的全部数据”。
 *   - 快照不会改变队列内部状态（不影响 `read_offset`、读写段内容）。
 *   - 若使用指针类型 `Queue<U*>`，快照包含的是指针序列；对象的生命周期仍由调用方管理，容器不负责 delete。
 *
 * 8) 类型与约束
 * - `Queue<T>` 仅支持 `std::is_trivially_copyable_v<T>` 为真：适合 `int/float/enum/POD` 等值类型。
 * - 对于非平凡类型，使用 `Queue<T*>`（指针队列），由调用方显式 new/delete 或采用其它所有权策略。
 * - 兼容别名：`ValueQueue<T> = Queue<T>`，`ManagedQueue<T> = Queue<T*>`（用于迁移旧代码）。
 *
 * 9) 清理接口
 * - `Clear()`：清空两段元素计数并复位 `read_offset`，保留已分配容量。
 * - `Free()`：在 `Clear()` 的基础上释放两段底层数组的内存空间（容量归零）。
 *
 * 10) 边界条件与线程安全
 * - 空队列：`Peek/Pop` 返回 false；`begin()/end()` 迭代区间为空。
 * - 大量交替 `Push/Pop` 不产生整体搬移；读尽后由段切换承接顺序。
 * - 未内置并发控制；如需多线程使用，请在更高层面加锁或采用无锁方案。
 *
 * 11) 潜在改进方向
 * - 迭代器 `end()` 语义可进一步精化，使仅读段存在数据时更语义化。
 * - 提供批量 Pop 接口，降低多次 `ReadAt` 调用成本，在高吞吐场景可能更优。
 */
#pragma once

#include<vector>
#include<hgl/type/PtrArray.h>
#include<type_traits>
#include<algorithm>
#include<cstring>
namespace hgl
{
    /**
    * Queue模板类用于保存一个先进先出、后进后出的数据队列
    * 支持不同的底层数组类型
    */
    template<typename T>
    class Queue
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "Queue<T> requires T to be trivially copyable. For complex objects, use Queue<T*> instead.");

    protected:

        std::vector<T> data_array[2];

        int read_index;
        int write_index;

        int read_offset;

    protected:

        void SwapIndex()
        {
            if(read_index==0)
            {
                read_index=1;
                write_index=0;
            }
            else
            {
                read_index=0;
                write_index=1;
            }

            read_offset=0;
        }

    public: //迭代器支持

        /**
         * 只读迭代器，用于遍历 Queue 中的所有元素
         * 自动处理两个内部数组的切换
         */
        class ConstIterator
        {
        private:
            const Queue *queue;
            int current_array;  // 0 = read_array, 1 = write_array
            int current_pos;    // 当前数组中的位置

        public:
            ConstIterator(const Queue *q, int array_idx, int pos)
                : queue(q), current_array(array_idx), current_pos(pos)
            {
                // 如果起始位置在 read_array，需要从 read_offset 开始
                if(current_array == 0 && pos < queue->read_offset)
                    current_pos = queue->read_offset;

                // 跳过空的 read_array
                AdvanceIfNeeded();
            }

            bool operator!=(const ConstIterator &other) const
            {
                return current_array != other.current_array || current_pos != other.current_pos;
            }

            bool operator==(const ConstIterator &other) const
            {
                return !(*this != other);
            }

            const T& operator*() const
            {
                int array_index = (current_array == 0) ? queue->read_index : queue->write_index;
                return queue->data_array[array_index][current_pos];
            }

            ConstIterator& operator++()
            {
                ++current_pos;
                AdvanceIfNeeded();
                return *this;
            }

        private:
            void AdvanceIfNeeded()
            {
                // 检查是否需要切换到下一个数组
                if(current_array == 0)
                {
                    int read_count = (int)queue->data_array[queue->read_index].size();
                    if(current_pos >= read_count)
                    {
                        // 切换到 write_array
                        current_array = 1;
                        current_pos = 0;
                    }
                }
            }
        };

        /**
         * 返回指向第一个元素的迭代器
         */
        ConstIterator begin() const
        {
            int read_count = (int)data_array[read_index].size();
            if(read_offset < read_count)
                return ConstIterator(this, 0, read_offset);  // 从 read_array 开始
            else
                return ConstIterator(this, 1, 0);  // 直接从 write_array 开始
        }

        /**
         * 返回指向末尾的迭代器
         */
        ConstIterator end() const
        {
            return ConstIterator(this, 1, (int)data_array[write_index].size());
        }

    public: //兼容 Active 接口的简易 GetArray()

        std::vector<T> &GetArray(){ return data_array[write_index]; }
        const std::vector<T> &GetArray() const { return data_array[write_index]; }

        /**
         * @brief 获取队列中所有未读数据的完整视图（包括 read 和 write 两个数组）
         * @return std::vector<T> 合并后的所有队列数据
         */
        std::vector<T> GetUnreadSnapshot() const
        {
            std::vector<T> result;

            // 获取 read_array 中的有效数据（从 read_offset 开始）
            int read_count = (int)data_array[read_index].size();
            int valid_read_count = (read_count > read_offset) ? (read_count - read_offset) : 0;

            // 获取 write_array 中的数据
            int write_count = (int)data_array[write_index].size();

            // 总大小
            int total_count = valid_read_count + write_count;

            if(total_count <= 0)
                return result;  // 返回空 buffer

            // 分配空间
            result.resize(total_count);

            // 复制 read_array 中的有效部分
            if(valid_read_count > 0)
            {
                memcpy(result.data(), data_array[read_index].data() + read_offset, valid_read_count * sizeof(T));
            }

            // 复制 write_array 的全部数据
            if(write_count > 0)
            {
                memcpy(result.data() + valid_read_count, data_array[write_index].data(), write_count * sizeof(T));
            }

            return result;
        }

    public: //属性

        const int GetAllocCount() const
        {
            return (int)(data_array[0].capacity()+data_array[1].capacity());
        }

        const int GetCount() const
        {
            return (int)(data_array[0].size()+data_array[1].size()-read_offset);
        }

        virtual bool Reserve(int count)                                          ///<预分配数据空间
        {
            data_array[0].reserve(count);
            data_array[1].reserve(count);
            return true;
        }

        const bool IsEmpty() const{return GetCount()==0;}                         ///<确认列表是否为空

        const bool Contains(const T &data)const
        {
            if(std::find(data_array[read_index].begin() + read_offset, data_array[read_index].end(), data) != data_array[read_index].end())
                return true;
            return std::find(data_array[write_index].begin(), data_array[write_index].end(), data) != data_array[write_index].end();
        }

    public: //方法

        Queue()
        {
            write_index=0;
            read_index=1;
            read_offset=0;
        }

        virtual ~Queue()=default;

        virtual bool Push(T *data,int count)                                      ///<压入一批数据
        {
            if(!data||count<=0)return false;

            int offset=(int)data_array[write_index].size();

            data_array[write_index].resize(offset + count);

            memcpy(data_array[write_index].data() + offset, data, count * sizeof(T));

            return true;
        }

        virtual bool Push(const T &data){return Push((T*)&data,1);}               ///<压入一个数据

        virtual bool Peek(T &data)                                                ///<尝试访问一个数据
        {
            if(read_offset >= (int)data_array[read_index].size())
            {
                if((int)data_array[write_index].size()<=0)
                   return false;
                data = data_array[write_index][0];
            }
            else
            {
                data = data_array[read_index][read_offset];
            }
            return true;
        }

        virtual bool Pop(T &data)                                                 ///<弹出一个数据
        {
            if((int)data_array[read_index].size()<=read_offset)
            {
                if((int)data_array[write_index].size()<=0)
                   return false;
                data_array[read_index].clear();
                SwapIndex();
            }

            data = data_array[read_index][read_offset];
            ++read_offset;
            return true;
        }

    public:

        virtual void Clear()                                                      ///<清除所有数据
        {
            data_array[0].clear();
            data_array[1].clear();
            read_offset=0;
        }

        virtual void Free()                                                       ///<清除所有数据并释放内存
        {
            Clear();
            data_array[0].clear();
            data_array[0].shrink_to_fit();
            data_array[1].clear();
            data_array[1].shrink_to_fit();
        }
    }; // class Queue
}//namespace hgl
