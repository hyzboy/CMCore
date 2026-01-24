/**
* @file ActiveDataManager.h
* @brief CN:活动数据管理模板类，支持通过ID管理活跃数据。\nEN:Active data manager template class, supports managing active data by ID.
*/
#pragma once

#include <hgl/type/ActiveMemoryBlockManager.h>

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
        ValueBuffer<T> data_array;

    public:

        ActiveDataManager()=default;
        virtual ~ActiveDataManager() = default;

        /**
        * @brief CN:预分配容量。\nEN:Reserve capacity.
        */
        void Reserve(int c)
        {
            aim.Reserve(c);
            data_array.Reserve(c);
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
        * @brief CN:获取活跃ID数组。\nEN:Get active ID array.
        */
        const ValueBuffer<int> &GetActiveArray() const
        {
            return aim.GetActiveArray();
        }

        /**
        * @brief CN:获取闲置ID数组。\nEN:Get idle ID array.
        */
        const Queue<int> &GetIdleArray() const
        {
            return aim.GetIdleArray();
        }

    public:

        /**
        * @brief CN:写入指定ID的数据。\nEN:Write data at specified ID.
        */
        bool WriteData(const T &d, const int id)
        {
            return data_array.WriteAt(d, id);
        }

        /**
        * @brief CN:批量写入数据（指针数组）。\nEN:Batch write data (pointer array).
        */
        int WriteDataArray(const T **da, const int *idp, const int count)
        {
            int result = 0;

            for (int i = 0; i < count; i++)
            {
                if (data_array.WriteAt(**da, *idp))
                    ++result;

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
                if (data_array.WriteAt(*da, *idp))
                    ++result;

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
            return data_array.At(id);
        }

        /**
        * @brief CN:读取指定ID的数据。\nEN:Read data at specified ID.
        */
        bool GetData(T &d, const int id) const
        {
            return data_array.ReadAt(d, id);
        }

        /**
        * @brief CN:批量获取数据（指针数组）。\nEN:Batch get data (pointer array).
        */
        bool GetData(T **da, const int *idp, const int count) const
        {
            int result = 0;

            for (int i = 0; i < count; i++)
            {
                *da = data_array.At(*idp);

                if (*da)
                    ++result;

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
                if (data_array.ReadAt(*da, *idp))
                    ++result;

                ++da;
                ++idp;
            }

            return result;
        }

    public:

        /**
        * @brief CN:创建活跃ID。\nEN:Create active ID.
        */
        void CreateActive(int *id, const int count = 1)
        {
            aim.CreateActive(id, count);

            data_array.Expand(count);
        }

        /**
        * @brief CN:创建闲置ID。\nEN:Create idle ID.
        */
        void CreateIdle(int *idp = nullptr, const int count = 1)
        {
            aim.CreateIdle(idp, count);

            data_array.Expand(count);
        }

        /**
        * @brief CN:创建闲置ID（无返回）。\nEN:Create idle ID (no return).
        */
        void CreateIdle(const int count = 1)
        {
            CreateIdle(nullptr, count);
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

            int idle_count = GetIdleCount();
            int get_count = (idle_count < count) ? idle_count : count;

            // Try to get IDs from idle pool first
            if (get_count > 0)
            {
                if (!aim.Get(id, get_count))
                    return false;
                
                id += get_count;
            }

            // Create new active IDs if needed
            int create_count = count - get_count;
            if (create_count > 0)
            {
                aim.CreateActive(id, create_count);
                data_array.Expand(create_count);
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
        * @brief CN:清空所有活跃数据（但不释放内存）。\nEN:Clear all active data (without freeing memory).
        */
        void Clear()
        {
            aim.ReleaseAllActive();
            data_array.Clear();
        }

        /**
        * @brief CN:清空所有数据并释放内存。\nEN:Clear all data and free memory.
        */
        void Free()
        {
            aim.Free();
            data_array.Free();
        }

        /**
        * @brief CN:获取一个闲置数据指针。\nEN:Get an idle data pointer.
        */
        T *GetIdle()
        {
            int id;

            if (!aim.Get(&id))
                return nullptr;

            return data_array.At(id);
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
