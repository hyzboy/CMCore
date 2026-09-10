/**
* @file ActiveDataManager.h
* @brief CN:活动数据管理模板类，支持通过ID管理活跃数据。\nEN:Active data manager template class, supports managing active data by ID.
*/
#pragma once

#include<hgl/type/ActiveMemoryBlockManager.h>
#include<vector>

namespace hgl
{
    /**
    * @brief CN:活动数据管理模板类。\nEN:Active data manager template class.
    * @tparam T CN:数据类型。EN:Data type.
    *
    * CN:通过ActiveIDManager管理活跃的数据ID，在要使用时通过ID来获取或写入数据。\nEN:Manages active data IDs via ActiveIDManager, access or write data by ID when needed.
    */
    template<typename T>
    class ActiveDataManager
    {
    protected:

        /**
        * @brief CN:活跃ID管理器。\nEN:Active ID manager.
        */
        ActiveIDManager aim;

        /**
        * @brief CN:数据数组。\nEN:Data array.
        */
        std::vector<T> data_array;

    public:

        ActiveDataManager()=default;
        virtual ~ActiveDataManager() = default;

        /**
        * @brief CN:预分配容量。\nEN:Reserve capacity.
        */
        void Reserve(int c)
        {
            // 只有 data_array 存在预分配语义（ID侧两个容器均为按需增长）
            if (c > 0)
                data_array.reserve(static_cast<size_t>(c));
        }

        /**
        * @brief CN:获取活跃数据数量。\nEN:Get active data count.
        */
        int GetActiveCount() const
        {
            return aim.GetActiveCount();
        }

        /**
        * @brief CN:获取闲置数据数量。\nEN:Get idle data count.
        */
        int GetIdleCount() const
        {
            return aim.GetIdleCount();
        }

        /**
        * @brief CN:获取总数据数量。\nEN:Get total data count.
        */
        int GetTotalCount() const
        {
            return aim.GetTotalCount();
        }

        /**
        * @brief CN:获取历史最大ID。\nEN:Get history max ID.
        */
        int GetHistoryMaxId() const
        {
            return aim.GetHistoryMaxId();
        }

        /**
        * @brief CN:获取活跃ID列表快照。\nEN:Get snapshot of active ID list
        */
        std::vector<int> GetActiveView() const
        {
            return aim.GetActiveView();
        }

        /**
        * @brief CN:获取闲置ID队列的快照。\nEN:Get idle ID queue snapshot (copy, not reference).
        *
        * CN:返回所有闲置ID的完整快照（按FIFO次序的深拷贝）。\nEN:Returns a complete snapshot (deep copy) of all idle IDs in FIFO order.
        * CN:可以安全地长期保存。\nEN:Safe to keep for long periods.
        */
        std::vector<int> GetIdleView() const
        {
            return aim.GetIdleView();
        }

    public:

        /**
        * @brief CN:写入指定ID的数据。\nEN:Write data at specified ID.
        */
        bool WriteData(const T &d, const int id)
        {
            if(id < 0 || id >= (int)data_array.size()) return false;
            data_array[id] = d;
            return true;
        }

        /**
        * @brief CN:批量写入数据（指针数组）。\nEN:Batch write data (pointer array).
        */
        int WriteDataArray(const T **da, const int *idp, const int count)
        {
            int result = 0;

            for (int i = 0; i < count; i++)
            {
                if (*idp >= 0 && *idp < (int)data_array.size())
                {
                    data_array[*idp] = **da;
                    ++result;
                }

                ++da;
                ++idp;
            }

            return result;
        }

        /**
        * @brief CN:批量写入数据（数据数组）。\nEN:Batch write data (data array).
        */
        int WriteDataArray(const T *da, const int *idp, const int count)
        {
            int result = 0;

            for (int i = 0; i < count; i++)
            {
                if (*idp >= 0 && *idp < (int)data_array.size())
                {
                    data_array[*idp] = *da;
                    ++result;
                }

                ++da;
                ++idp;
            }

            return result;
        }

        /**
        * @brief CN:获取指定ID的数据指针。\nEN:Get data pointer at specified ID.
        */
        T *At(const int id)
        {
            if(id < 0 || id >= (int)data_array.size()) return nullptr;
            return &data_array[id];
        }

        /**
        * @brief CN:读取指定ID的数据。\nEN:Read data at specified ID.
        */
        bool GetData(T &d, const int id) const
        {
            if(id < 0 || id >= (int)data_array.size()) return false;
            d = data_array[id];
            return true;
        }

        /**
        * @brief CN:批量获取数据（指针数组，const版本——不交出可写指针）。\nEN:Batch get data (pointer array, const-correct).
        */
        bool GetData(const T **da, const int *idp, const int count) const
        {
            int result = 0;

            for (int i = 0; i < count; i++)
            {
                if (*idp >= 0 && *idp < (int)data_array.size())
                {
                    *da = &data_array[*idp];
                    ++result;
                }
                else
                {
                    *da = nullptr;
                }

                ++da;
                ++idp;
            }

            return result;
        }

        /**
        * @brief CN:批量获取数据（数据数组）。\nEN:Batch get data (data array).
        */
        bool GetData(T *da, const int *idp, const int count) const
        {
            int result = 0;

            for (int i = 0; i < count; i++)
            {
                if (*idp >= 0 && *idp < (int)data_array.size())
                {
                    *da = data_array[*idp];
                    ++result;
                }

                ++da;
                ++idp;
            }

            return result;
        }

    public:

        /**
        * @brief CN:创建活跃ID。\nEN:Create active ID.
        * @return CN:实际创建的ID数量（0=失败，容量不足）；\nEN:Number of IDs actually created (0 = failure).
        */
        int CreateActive(int *id, const int count = 1)
        {
            if (!id || count <= 0)
                return 0;

            const int created = aim.CreateActive(id, count);
            if (created <= 0)
                return 0;

            // data_array 的长度必须恒等于ID水位（新ID总是连续追加在水位之后）
            data_array.resize(static_cast<size_t>(aim.GetHistoryMaxId()));
            return created;
        }

        /**
        * @brief CN:创建闲置ID。\nEN:Create idle ID.
        * @return CN:实际创建的ID数量（0=失败，容量不足）；\nEN:Number of IDs actually created (0 = failure).
        */
        int CreateIdle(int *idp = nullptr, const int count = 1)
        {
            if (count <= 0)
                return 0;

            const int created = aim.CreateIdle(idp, count);
            if (created <= 0)
                return 0;

            // data_array 的长度必须恒等于ID水位（新ID总是连续追加在水位之后）
            data_array.resize(static_cast<size_t>(aim.GetHistoryMaxId()));
            return created;
        }

        /**
        * @brief CN:创建闲置ID（无返回）。\nEN:Create idle ID (no return).
        * @return CN:实际创建的ID数量（0=失败，容量不足）；\nEN:Number of IDs actually created (0 = failure).
        */
        int CreateIdle(const int count = 1)
        {
            return CreateIdle(nullptr, count);
        }

        /**
        * @brief CN:激活ID。\nEN:Activate ID.
        */
        bool Get(int *id, const int count = 1)
        {
            return aim.Get(id, count);
        }

        /**
        * @brief CN:获取或创建活跃ID。\nEN:Get or create active ID.
        */
        bool GetOrCreate(int *id, const int count = 1)
        {
            if (!id || count <= 0)
                return false;

            const int idle_count = GetIdleCount();
            const int get_count = (idle_count < count) ? idle_count : count;

            // Try to get IDs from idle pool first
            if (get_count > 0)
            {
                if (!aim.Get(id, get_count))
                    return false;

                id += get_count;
            }

            // Create new active IDs if needed
            const int create_count = count - get_count;
            if (create_count > 0)
            {
                // 全有或全无：创建失败时把已取出的闲置ID归还，保证返回false时不产生副作用
                if (aim.CreateActive(id, create_count) != create_count)
                {
                    if (get_count > 0)
                        aim.Release(id - get_count, get_count);

                    return false;
                }

                data_array.resize(static_cast<size_t>(aim.GetHistoryMaxId()));
            }

            return true;
        }

        /**
        * @brief CN:释放指定ID。\nEN:Release specified ID.
        */
        int Release(const int *id, const int count = 1)
        {
            return aim.Release(id, count);
        }

        /**
        * @brief CN:释放所有活跃ID。\nEN:Release all active IDs.
        */
        int ReleaseAllActive()
        {
            return aim.ReleaseAllActive();
        }

        /**
        * @brief CN:把所有活跃ID归还闲置池（不清理数据槽内容、不释放内存）。\nEN:Return all active IDs to the idle pool (slots are kept, memory is not freed).
        *
        * CN:数据槽保留旧值，直到对应ID被重新取出并写入；ID水位与 data_array 长度均不变。\nEN:Data slots keep their values until the ID is fetched again and rewritten.
        */
        void Clear()
        {
            aim.ReleaseAllActive();
            // 保留 data_array 的长度以便后续重用，不重置 id_count
        }

        /**
        * @brief CN:清空所有数据并释放内存。\nEN:Clear all data and free memory.
        */
        void Free()
        {
            aim.Free();
            data_array.clear();
            data_array.shrink_to_fit();
        }

        /**
        * @brief CN:获取一个闲置数据指针。\nEN:Get an idle data pointer.
        */
        T *GetIdle()
        {
            int id;

            if (!aim.Get(&id))
                return nullptr;

            if (id < 0 || id >= (int)data_array.size())
                return nullptr;
            return &data_array[id];
        }

        /**
        * @brief CN:判断ID是否活跃。\nEN:Check if ID is active.
        */
        bool IsActive(const int id) const
        {
            return aim.IsActive(id);
        }
    };
} // namespace hgl
