/**
 * Queue.h 实现详解（设计与原理概述）
 *
 * 1) 设计目标
 * - 提供一个高性能、可扩展的队列（先进先出），同时兼顾“批量追加”与“顺序弹出”。
 * - 统一接口，支持两种底层存储：平凡类型使用 ValueBuffer<T>，非平凡类型使用 PtrArray<T>。
 * - 通过双缓冲（read/write 两段）减少数据搬移，提升 Push/Pop 的整体效率。
 *
 * 2) 核心数据结构
 * - ArrayT data_array[2]: 两段数组，逻辑上分别作为“读段(read_array)”与“写段(write_array)”。
 *   - 成员 read_index 指向当前读段，write_index 指向当前写段。
 *   - 成员 read_offset 标记读段的起始读取位置（避免每次 Pop 都整体移动数据）。
 * - 初始化时：write_index=0, read_index=1, read_offset=0。这样第一次 Push 会写到 data_array[0]，
 *   而 Pop 会优先从 data_array[1] 读取；当读段为空且写段有数据时，会触发段切换（详见 SwapIndex）。
 *
 * 3) 段切换策略（SwapIndex）
 * - 当当前读段已全部读完（read_offset >= data_array[read_index].GetCount()），且写段非空时：
 *   - 清空旧读段（data_array[read_index].Clear()），将写段切换为新的读段、读段切换为新的写段，read_offset 置 0。
 *   - 这一策略避免了在每次 Pop 时移动或拼接数组，从而保证顺序弹出的成本几乎为 O(1)。
 *
 * 4) Push/Pop/Peek 的语义与复杂度
 * - Push(const T &data) / Push(T *data,int count): 始终追加到写段尾部。
 *   - 写段扩容由底层 ArrayT（ValueBuffer 或 PtrArray）负责，通常按指数增长，摊销成本近 O(1)。
 * - Peek(T &data): 只读取但不前进。
 *   - 若读段可读（read_offset < 读段元素数），直接从读段当前位置读取；否则若写段非空，则从写段的第 0 位读取。
 *   - 若两段皆为空，返回 false。
 * - Pop(T &data): 顺序弹出一个元素并前进 read_offset。
 *   - 若读段已读完且写段非空，触发段切换（见上）；否则从读段当前位置读取并 ++read_offset。
 *   - 若两段皆为空，返回 false。
 * - 复杂度：Push/Peek/Pop 在绝大多数情况下均为 O(1) 摊销；偶发的段切换为 O(1)。
 *
 * 5) 迭代器（只读）
 * - 提供 ConstIterator，用于遍历队列的“逻辑视图”。
 *   - begin(): 若读段仍有未读数据（read_offset < read_count），从读段的 read_offset 开始；否则直接从写段开头开始。
 *   - end(): 固定为写段末尾位置（写段元素数）。
 *   - ++ 运算：在读段迭代至末尾后，自动切换到写段，从 0 开始继续迭代；不回退、不越界。
 * - 迭代器视图与 Pop/Push 解耦：迭代不会修改 read_offset，也不会触发段切换（只读视角）。
 *
 * 6) Contains/GetCount/GetAllocCount
 * - Contains(const T &data): 依次在“读段的未读区间（从 read_offset 起）”与“写段的全部区间”进行查找。
 * - GetCount(): 返回逻辑元素总数 = 读段元素数 + 写段元素数 - read_offset。
 * - GetAllocCount(): 返回两段底层数组已分配容量之和，便于观测内存预留情况。
 *
 * 7) GetArray 与 GetData 快照
 * - GetArray(): 返回当前“写段”的引用（便于与 Active 接口兼容的场景）。
 * - GetData(): 返回当前队列的“稳定快照”，将读段的未读部分与写段全部顺序合并：
 *   - 若 T 为平凡类型（trivially copyable），返回 ValueBuffer<T> 并以一次性批量拷贝合并；
 *   - 若 T 为非平凡类型，返回 PtrArray<T>，通过逐对象复制构造进行合并；
 *   - 快照不会改变队列内部状态（不影响 read_offset、read/write 段内容）。
 *
 * 8) 类型别名与约束
 * - ValueQueue<T>: 继承 QueueTemplate<T, ValueBuffer<T>>；限定 T 必须是平凡类型（static_assert）。
 *   - 代表“按值存储”的高性能队列，适合 int/float/POD 等。
 * - ManagedQueue<T>: 继承 QueueTemplate<T, PtrArray<T>>；限定 T 必须是非平凡类型（static_assert）。
 *   - 代表“对象管理”队列：底层使用显式构造/析构的容器，适合带资源管理的类类型。
 *   - 特别说明：ManagedQueue::Push(T *ptr) 当前实现会复制对象内容进队列，但不会删除传入指针本身；
 *     若调用方以 new 创建并传入，则仍需自行 delete 以避免泄露。可根据项目约定进一步明确所有权策略。
 *
 * 9) 清理接口
 * - Clear(): 清空两段元素计数，并将 read_offset 复位为 0，但保留已分配容量。
 * - Free(): 调用 Clear() 后释放两段底层数组的内存空间（容量归零）。
 *
 * 10) 边界条件与线程安全
 * - 空队列：Peek/Pop 均返回 false；begin() 与 end() 的迭代区间为空。
 * - 段完全读尽后，下一次 Pop 会触发段切换并从新读段继续；大量交替 Push/Pop 的场景不会产生整体搬移。
 * - 本实现未提供显式的线程同步；如需在多线程环境使用，请在更高层面加锁或采用无锁方案。
 *
 * 11) 潜在改进方向
 * - 迭代器 end() 语义可进一步精化，使其对仅读段有数据时更语义化（当前以写段长度作为尾后对齐）。
 * - ManagedQueue::Push(T *ptr) 的所有权语义可选择：复制并 delete、或改为严格“所有权转移”，需结合项目规范。
 * - 提供批量 Pop 接口以减少逐个 ReadAt 的调用成本，在高吞吐场景可能更优。
 */
#pragma once

#include<hgl/type/ValueBuffer.h>
#include<hgl/type/PtrArray.h>
#include<type_traits>
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

        ValueBuffer<T> data_array[2];

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
                    int read_count = queue->data_array[queue->read_index].GetCount();
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
            int read_count = data_array[read_index].GetCount();
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
            return ConstIterator(this, 1, data_array[write_index].GetCount());
        }

    public: //兼容 Active 接口的简易 GetArray()

        ValueBuffer<T> &GetArray(){ return data_array[write_index]; }
        const ValueBuffer<T> &GetArray() const { return data_array[write_index]; }

    protected:

        /**
         * @brief 获取队列中所有未读取数据的完整视图（包括 read 和 write 两个数组）
         * @return ValueBuffer<T> 合并后的所有队列数据
         */
        ValueBuffer<T> GetUnreadSnapshotData() const
        {
            ValueBuffer<T> result;

            // 获取 read_array 中的有效数据（从 read_offset 开始）
            int read_count = data_array[read_index].GetCount();
            int valid_read_count = (read_count > read_offset) ? (read_count - read_offset) : 0;

            // 获取 write_array 中的数据
            int write_count = data_array[write_index].GetCount();

            // 总大小
            int total_count = valid_read_count + write_count;

            if(total_count <= 0)
                return result;  // 返回空 buffer

            // 分配空间
            result.Resize(total_count);

            // 复制 read_array 中的有效部分
            if(valid_read_count > 0)
            {
                result.WriteAt(data_array[read_index].GetData() + read_offset, 0, valid_read_count);
            }

            // 复制 write_array 的全部数据
            if(write_count > 0)
            {
                result.WriteAt(data_array[write_index].GetData(), valid_read_count, write_count);
            }

            return result;
        }

    public: //属性

        const int GetAllocCount() const
        {
            return data_array[0].GetAllocCount()+data_array[1].GetAllocCount();
        }

        const int GetCount() const
        {
            return data_array[0].GetCount()+data_array[1].GetCount()-read_offset;
        }

        virtual bool Reserve(int count)                                          ///<预分配数据空间
        {
            if(!data_array[0].Reserve(count))return(false);
            if(!data_array[1].Reserve(count))return(false);
            return true;
        }

        const bool IsEmpty() const{return GetCount()==0;}                         ///<确认列表是否为空

        const bool Contains(const T &data)const
        {
            if(data_array[read_index].Find(data,read_offset)!=-1)
                return true;
            return data_array[write_index].Find(data)!=-1;
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

            int offset=data_array[write_index].GetCount();

            if(!data_array[write_index].Expand(count))
                return false;

            data_array[write_index].WriteAt(data,offset,count);

            return true;
        }

        virtual bool Push(const T &data){return Push((T*)&data,1);}               ///<压入一个数据

        virtual bool Peek(T &data)                                                ///<尝试访问一个数据
        {
            if(read_offset >= data_array[read_index].GetCount())
            {
                if(data_array[write_index].GetCount()<=0)
                   return false;
                data_array[write_index].ReadAt(data,0);
            }
            else
            {
                data_array[read_index].ReadAt(data,read_offset);
            }
            return true;
        }

        virtual bool Pop(T &data)                                                 ///<弹出一个数据
        {
            if(data_array[read_index].GetCount()<=read_offset)
            {
                if(data_array[write_index].GetCount()<=0)
                   return false;
                data_array[read_index].Clear();
                SwapIndex();
            }

            data_array[read_index].ReadAt(data,read_offset);
            ++read_offset;
            return true;
        }

    public:

        virtual void Clear()                                                      ///<清除所有数据
        {
            data_array[0].Clear();
            data_array[1].Clear();
            read_offset=0;
        }

        virtual void Free()                                                       ///<清除所有数据并释放内存
        {
            Clear();
            data_array[0].Free();
            data_array[1].Free();
        }

        /**
         * @brief 获取队列中所有未读取数据的完整视图（包括 read 和 write 两个数组）
         * @return ValueBuffer<T> 合并后的所有队列数据
         */
        ValueBuffer<T> GetUnreadSnapshot() const
        {
            return GetUnreadSnapshotData();
        }
    }; // class Queue
}//namespace hgl
