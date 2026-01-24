#pragma once

#include<hgl/type/ValueBuffer.h>
#include<hgl/type/Stack.h>
#include<hgl/type/SmallMap.h>
#include<type_traits>

namespace hgl
{
    using MonotonicID=int32;

    constexpr const MonotonicID MONOTONIC_ID_INVALID=0;

    /**
     * CN: 单调ID数据列表(仅支持原生数据类型，不支持对象)
     * EN: Monotonic ID data list (only supports primitive data types, not objects)
     *
     * CN: 该模板类提供了一个高效的ID管理系统，用于存储和管理具有单调递增ID的数据。
     *     支持快速添加、删除、查询操作，并提供数据压缩和ID重编号功能。
     * EN: This template class provides an efficient ID management system for storing and managing data with monotonically increasing IDs.
     *     Supports fast add, remove, and query operations, and provides data shrinking and ID reindexing features.
     *
     * @tparam T CN: 存储的数据类型（必须是原生数据类型）
     *   EN: The data type to store (must be a primitive data type)
     * @tparam I CN: ID的数据类型，默认为MonotonicID(int32)
     *        EN: The data type for IDs, defaults to MonotonicID(int32)
     */
    template<typename T,typename I=MonotonicID> class MonotonicIDList
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "MonotonicIDList requires trivially copyable T");

        I id_base=1;                            ///< CN: ID 起始值(默认1) / EN: ID base (default 1)
        I next_id=1;                            ///< CN: 下一个可用的ID / EN: Next available ID

        ValueBuffer<T>  data_array;               ///< CN: 实际数据存储数组 / EN: Actual data storage array
        ValueBuffer<I>  location_to_id;           ///< CN: 位置到ID的并行数组 / EN: Parallel array: location -> ID
        SmallMap<I,int32> id_to_location_map;   ///< CN: ID到位置映射 / EN: Map: ID -> location
        Stack<int32>  free_location;            ///< CN: 空闲位置栈 / EN: Stack of free locations

    public:

        MonotonicIDList() = default;

        explicit MonotonicIDList(I start_id)
        {
            id_base=start_id;
            next_id=start_id;
        }

        static constexpr I InvalidID(){ return static_cast<I>(MONOTONIC_ID_INVALID); }

        bool ResetIDBase(I start_id)
        {
            if(!Empty()) return false;
            id_base=start_id;
            next_id=start_id;
            return true;
        }

        /**
         * CN: 添加一个新元素并返回其指针
         * EN: Add a new element and return its pointer
         *
         * CN: 该方法会自动分配一个新的单调递增ID，并返回指向新分配数据的指针。
         *     如果有空闲位置，则复用空闲位置；否则扩展数据数组。
         * EN: This method automatically assigns a new monotonically increasing ID and returns a pointer to the newly allocated data.
         *     If there are free locations, it reuses them; otherwise, it expands the data array.
         *
         * @return CN: 指向新添加元素的指针，用于后续初始化数据
         *         EN: Pointer to the newly added element for subsequent data initialization
      */
        T *Add()
        {
            int location=-1;

            if(free_location.IsEmpty())
            {
                location=data_array.GetCount();
                data_array.Expand(1);
                // keep parallel location_to_id in sync
                if(location_to_id.GetCount()<=location)
                    location_to_id.Resize(location+1);
            }
            else
            {
                free_location.Pop(location);
            }

            T *p=data_array.At(location);

            id_to_location_map.Add(next_id,location);
            // mark location -> id
            *location_to_id.At(location)=next_id;

            ++next_id;

            return p;
        }

        /**
         * CN: 添加一个新元素并复制给定的值
         * EN: Add a new element and copy the given value
         *
         * CN: 该方法会添加一个新元素并将给定的值复制到新分配的位置。
         * EN: This method adds a new element and copies the given value to the newly allocated location.
         *
         * @param value CN: 要添加的值
         *        EN: The value to add
         * @return CN: 添加成功返回true，失败返回false
         *         EN: Returns true if addition is successful, false otherwise
         */
        bool Add(const T &value)
        {
            T *p=Add();

            if(!p)
                return(false);

            *p=value;

            return(true);
        }

        bool Add(T &&value)
        {
            T *p=Add();
            if(!p) return false;
            *p=static_cast<T&&>(value);
            return true;
        }

        I AddGetID(const T &value)
        {
            const I new_id=next_id;
            Add(value);
            return new_id;
        }

        I AddGetID(T &&value)
        {
            const I new_id=next_id;
            Add(static_cast<T&&>(value));
            return new_id;
        }

        I AddDefaultGetID()
        {
            const I new_id=next_id;
            Add();
            return new_id;
        }

        /**
         * CN: 根据ID移除一个元素
         * EN: Remove an element by ID
         *
         * CN: 该方法会从列表中移除指定ID的元素，但不会立即释放其占用的存储空间。
         *     移除的位置会被标记为空闲，可在后续添加操作中复用。
         *     要真正回收空间，需要调用Shrink()方法。
         * EN: This method removes the element with the specified ID from the list, but does not immediately free its storage space.
         *     The removed location is marked as free and can be reused in subsequent add operations.
         *     To actually reclaim space, call the Shrink() method.
         *
         * @param id CN: 要移除的元素的ID
         *           EN: The ID of the element to remove
         * @return CN: 移除成功返回true，如果ID不存在返回false
         *      EN: Returns true if removal is successful, false if the ID does not exist
         */
        bool Remove(const I &id)
        {
            int32 location;

            if(!id_to_location_map.GetAndDelete(id,location))
                return(false);

            // mark location as free
            if(location>=0 && location<location_to_id.GetCount())
            {
                *location_to_id.At(location)=InvalidID();
            }

            free_location.Push(location);

            return(true);
        }

        /**
         * CN: 检查指定ID的元素是否存在
         * EN: Check if an element with the specified ID exists
         *
         * @param id CN: 要检查的ID
         *        EN: The ID to check
         * @return CN: 如果ID存在返回true，否则返回false
         *         EN: Returns true if the ID exists, false otherwise
         */
        bool Contains(const I &id)const
        {
            return id_to_location_map.ContainsKey(id);
        }

        /**
         * CN: 根据ID获取元素的指针
         * EN: Get a pointer to an element by ID
         *
         * CN: 该方法返回指向指定ID元素的指针，可用于读取或修改数据。
         * EN: This method returns a pointer to the element with the specified ID, which can be used to read or modify the data.
         *
         * @param id CN: 要获取的元素的ID
         *           EN: The ID of the element to retrieve
         * @return CN: 指向元素的指针，如果ID不存在返回nullptr
         *         EN: Pointer to the element, or nullptr if the ID does not exist
         */
        T *Get(const I &id)
        {
            int32 location;

            if(!id_to_location_map.Get(id,location))
                return(nullptr);

            return data_array.At(location);
        }

        const T *Get(const I &id) const
        {
            int32 location;
            if(!id_to_location_map.Get(id,location))
                return nullptr;
            return data_array.At(location);
        }

        bool TryGet(const I &id, T *&out)
        {
            int32 location;
            if(!id_to_location_map.Get(id,location))
            {
                out=nullptr;
                return false;
            }
            out=data_array.At(location);
            return true;
        }

        bool TryGet(const I &id, const T *&out) const
        {
            int32 location;
            if(!id_to_location_map.Get(id,location))
            {
                out=nullptr;
                return false;
            }
            out=data_array.At(location);
            return true;
        }

        /**
         * CN: 获取指定ID元素在数据数组中的位置
         * EN: Get the location of an element with the specified ID in the data array
         *
         * CN: 该方法返回指定ID的元素在底层数据数组中的索引位置。
         * EN: This method returns the index position of the element with the specified ID in the underlying data array.
         *
         * @param id CN: 要查询的元素的ID
         *    EN: The ID of the element to query
         * @return CN: 元素的位置索引，如果ID不存在返回-1
         *         EN: The position index of the element, or -1 if the ID does not exist
         */
        int GetLocation(const I &id) const
        {
            int32 location;

            if(!id_to_location_map.Get(id,location))
                return -1;

            return location;
        }

        int Count() const
        {
            return id_to_location_map.GetCount();
        }

        int StorageSize() const
        {
            return data_array.GetCount();
        }

        bool Empty() const
        {
            return Count()==0;
        }

        void Clear()
        {
            data_array.Clear();
            location_to_id.Clear();
            id_to_location_map.Clear();
            free_location.Clear();
            next_id=id_base;
        }

        /**
        * CN: 收缩数据存储空间
        * EN: Shrink the data storage space
        *
        * CN: 该方法通过移动数据来填充空洞，并收缩底层数组大小，从而回收被删除元素占用的空间。
        *     执行过程：
        *     1. 如果没有空闲位置，直接返回
        *     2. 将末尾的有效数据移动到前面的空洞位置
        *     3. 更新所有相关的映射关系
        *     4. 收缩数据数组到实际使用的大小
        *     5. 清空空闲位置栈
        * EN: This method fills holes by moving data and shrinks the underlying array size, thus reclaiming space occupied by deleted elements.
        *     Execution process:
        *     1. If there are no free locations, return directly
        *     2. Move valid data from the end to earlier hole positions
        *     3. Update all related mapping relationships
        *     4. Shrink the data array to the actual used size
        *     5. Clear the free location stack
        *
        * @return CN: 返回收缩的元素数量
        *         EN: Returns the number of elements shrunk
        */
        int Shrink()
        {
            if(free_location.IsEmpty())
                return 0;

            // CN/EN: 当前存储大小
            int32 total=data_array.GetCount();
            const int32 original_total=total;

            // CN: 如果空洞覆盖全部，直接清空
            // EN: If all entries are free, clear all
            if(free_location.GetCount()>=total)
            {
                Clear();
                return original_total; 
            }

            int32 moved_count=0;

            while(!free_location.IsEmpty() && total>0)
            {
                int32 hole;
                free_location.Pop(hole);

                // 跳过尾部连续空洞
                while(total>0)
                {
                    const I tail_id = *location_to_id.At(total-1);
                    if(tail_id==InvalidID())
                    {
                        --total;
                        continue;
                    }
                    break;
                }

                if(total<=0) break;

                // 如果空洞在尾部或之后，收缩即可
                if(hole>=total-1)
                {
                    --total;
                    continue;
                }

                // 将尾部有效元素搬到空洞
                const int32 tail_loc = total-1;
                const I tail_id = *location_to_id.At(tail_loc);

                T *src = data_array.At(tail_loc);
                T *dst = data_array.At(hole);
                *dst = *src;

                // 更新映射
                id_to_location_map.Change(tail_id, hole);
                *location_to_id.At(hole)=tail_id;
                *location_to_id.At(tail_loc)=InvalidID();

                --total;
                ++moved_count;
            }

            data_array.Resize(total);
            location_to_id.Resize(total);
            free_location.Clear();

            return original_total-total;
        }

        /**
         * CN: ID 重映射结构
         * EN: ID remapping structure
         *
         * CN: 用于存储旧ID到新ID的映射关系，在Reindex操作后返回给调用者。
         * EN: Used to store the mapping relationship from old ID to new ID, returned to the caller after Reindex operation.
         */
        struct IDRemap
        {
            I old_id;  ///< CN: 原始ID / EN: Original ID
            I new_id;  ///< CN: 新分配的ID / EN: Newly assigned ID

            IDRemap()=default;
            IDRemap(I old_val,I new_val):old_id(old_val),new_id(new_val) {}
        };

        /**
        * CN: 重新编号：让所有数据的ID从1开始连续编号，使ID与位置一致
        * EN: Reindex: Renumber all data IDs starting from 1 consecutively, making IDs consistent with positions
        *
        * CN: 该方法会重新分配所有元素的ID，使得ID从1开始连续递增。
        *     这在需要序列化数据或导出数据时特别有用。
        *     执行过程：
        *     1. 首先调用Shrink()确保数据紧密排列
        *     2. 构建旧ID到新ID的映射列表
        *     3. 重建所有映射表，使新ID从1开始连续
        *     4. 重置next_id为当前元素数量+1
        * EN: This method reassigns IDs for all elements, making IDs start from 1 and increase consecutively.
        *     This is particularly useful when serializing or exporting data.
        *  Execution process:
        *     1. First call Shrink() to ensure data is densely packed
        *     2. Build a mapping list from old IDs to new IDs
        *     3. Rebuild all mapping tables so new IDs start from 1 consecutively
        *     4. Reset next_id to the current element count + 1
        *
        * @param remap_list CN: 输出参数，返回旧ID到新ID的映射列表
        *                   EN: Output parameter, returns the mapping list from old IDs to new IDs
        * @return   CN: 重新编号的数据数量
        *           EN: The number of data items reindexed
        */
        int Reindex(ValueBuffer<IDRemap> &remap_list)
        {
            // CN: 先执行 Shrink，确保数据紧密排列
            // EN: Execute Shrink first to ensure data is densely packed
            Shrink();

            const int32 count=data_array.GetCount();
            if(count<=0)
            {
                remap_list.Clear();
                return 0;
            }

            // CN: 清空并预分配映射列表
            // EN: Clear and preallocate the mapping list
            remap_list.Clear();
            remap_list.Resize(count);

            // CN/EN: 构建旧ID到新ID的映射，新ID从 id_base 开始
            for(int32 i=0; i<count; ++i)
            {
                const I old_id = *location_to_id.At(i);
                IDRemap remap;
                remap.old_id=old_id;
                remap.new_id=static_cast<I>(id_base + i);
                remap_list[i]=remap;
            }

            // 重建映射：新ID从 id_base 连续
            id_to_location_map.Clear();
            for(int32 i=0; i<count; ++i)
            {
                const I new_id=static_cast<I>(id_base + i);
                id_to_location_map.Add(new_id, i);
                *location_to_id.At(i)=new_id;
            }

            // 更新 next_id
            next_id=static_cast<I>(id_base + count);

            return count;
        }

        /**
        * CN: 重新编号的简化版本，不返回映射表
        * EN: Simplified version of reindexing without returning the mapping table
        *
        * CN: 该方法执行与Reindex(ValueBuffer<IDRemap>&)相同的重编号操作，
        *     但不返回旧ID到新ID的映射关系，适用于不需要跟踪ID变化的场景。
        * EN: This method performs the same reindexing operation as Reindex(ValueBuffer<IDRemap>&),
        *     but does not return the mapping from old IDs to new IDs, suitable for scenarios where tracking ID changes is not needed.
        *
        * @return   CN: 重新编号的数据数量
        *           EN: The number of data items reindexed
        */
        int Reindex()
        {
            ValueBuffer<IDRemap> temp_list;
            return Reindex(temp_list);
        }
    };//template<typename T,typename I = MonotonicID> class MonotonicIDList
}//namespace hgl