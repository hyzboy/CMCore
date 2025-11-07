#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/Stack.h>
#include<hgl/type/SmallMap.h>

namespace hgl
{
    using MonotonicID=int32;

    /**
      * 单调ID数据列表(仅支持原生数据类型，不支持对象)
      */
    template<typename T,typename I=MonotonicID> class MonotonicIDList
    {
        I next_id=0;

        DataArray<T> data_array;
        SmallMap<I,int32> id_to_location_map;
        SmallMap<int32,I> location_to_id_map;  // 反向映射：位置 -> ID
        Stack<int32> free_location;

    public:

        T *Add()
        {
            int location=-1;

            if(free_location.IsEmpty())
            {
                location=data_array.GetCount();

                data_array.Expand(1);
            }
            else
            {
                free_location.Pop(location);
            }

            T *p=data_array.At(location);

            id_to_location_map.Add(next_id,location);
            location_to_id_map.Add(location,next_id);  // 维护反向映射

            ++next_id;

            return p;
        }

        bool Add(const T &value)
        {
            T *p=Add();

            if(!p)
                return(false);

            hgl_cpy<T>(*p,value);

            return(true);
        }

        bool Remove(const I &id)
        {
            int32 location;

            if(!id_to_location_map.GetAndDelete(id,location)) //Map::Delete的作用是获取这个数据并删除
                return(false);

            location_to_id_map.DeleteByKey(location);  // 同时删除反向映射

            free_location.Push(location);

            return(true);
        }

        bool Contains(const I &id)const
        {
            return id_to_location_map.ContainsKey(id);
        }

        T *Get(const I &id)
        {
            int32 location;

            if(!id_to_location_map.Get(id,location))
                return(nullptr);

            return data_array.At(location);
        }

        int GetLocation(const I &id) const
        {
            int32 location;

            if(!id_to_location_map.Get(id,location))
                return -1;

            return location;
        }

        /**
        * 收缩数据存储空间
        */
        int Shrink()
        {
            if(free_location.IsEmpty())
                return 0;

            // 获取当前数据数组的大小
            int32 total_count=data_array.GetCount();
            int32 free_count=free_location.GetCount();

            // 如果所有位置都是空的，直接清空
            if(free_count>=total_count)
            {
                data_array.Clear();
                free_location.Clear();
                id_to_location_map.Clear();
                location_to_id_map.Clear();
                return free_count;
            }

            // 从后向前查找有效数据，填充到 free_location 中的空洞
            int32 moved_count=0;

            while(!free_location.IsEmpty()&&total_count>0)
            {
                int32 free_loc;
                free_location.Pop(free_loc);

                // 如果空洞位置已经在末尾或之后，无需移动
                if(free_loc>=total_count-1)
                {
                    --total_count;
                    continue;
                }

                // 从末尾找一个有效数据位置
                int32 last_valid_loc=total_count-1;

                // 使用反向映射快速查找末尾位置对应的ID（O(log n)而非O(n)）
                I last_valid_id;
                if(!location_to_id_map.Get(last_valid_loc,last_valid_id))
                {
                    // 如果末尾也是空洞，继续向前查找
                    --total_count;
                    free_location.Push(free_loc); // 把当前空洞放回去，稍后处理
                    continue;
                }

                // 将末尾的有效数据移动到空洞位置
                T *src=data_array.At(last_valid_loc);
                T *dst=data_array.At(free_loc);
                hgl_cpy<T>(*dst,*src);

                // 更新双向映射
                id_to_location_map.Change(last_valid_id,free_loc);
                location_to_id_map.DeleteByKey(last_valid_loc);
                location_to_id_map.ChangeOrAdd(free_loc,last_valid_id);  // 使用 ChangeOrAdd 而非 Change

                // 缩小数组大小
                --total_count;
                ++moved_count;
            }

            // 收缩 data_array 到实际使用的大小
            int32 original_count=data_array.GetCount();
            data_array.Resize(total_count);

            // 清空 free_location，因为所有空洞都已处理
            free_location.Clear();

            return original_count-total_count; // 返回收缩的数量
        }
        
        /**
        * ID 重映射结构
        */
        struct IDRemap
        {
            I old_id;
            I new_id;

            IDRemap()=default;
            IDRemap(I old_val,I new_val):old_id(old_val),new_id(new_val) {}
        };

        /**
        * 重新编号：让所有数据的ID从0开始连续编号，使ID与位置一致
        * @param remap_list 输出参数，返回旧ID到新ID的映射列表
        * @return 重新编号的数据数量
        */
        int Reindex(DataArray<IDRemap> &remap_list)
        {
            // 先执行 Shrink，确保数据紧密排列
            Shrink();

            const int32 count=data_array.GetCount();
            if(count<=0)
            {
                remap_list.Clear();
                return 0;
            }

            // 清空并预分配映射列表
            remap_list.Clear();
            remap_list.Resize(count);

            // 构建旧ID到新ID的映射
              // 遍历 location_to_id_map，location 就是新ID
            for(int32 new_id=0; new_id<count; ++new_id)
            {
                I old_id;
                if(location_to_id_map.Get(new_id,old_id))
                {
                    IDRemap remap;
                    remap.old_id=old_id;
                    remap.new_id=static_cast<I>(new_id);
                    remap_list[new_id]=remap;
                }
            }

            // 重建映射表
            id_to_location_map.Clear();
            location_to_id_map.Clear();

            for(int32 i=0; i<count; ++i)
            {
                I new_id=static_cast<I>(i);
                id_to_location_map.Add(new_id,i);
                location_to_id_map.Add(i,new_id);
            }

            // 重置 next_id 为当前数据数量
            next_id=static_cast<I>(count);

            return count;
        }

        /**
          * 重新编号的简化版本，不返回映射表
          * @return 重新编号的数据数量
            */
        int Reindex()
        {
            DataArray<IDRemap> temp_list;
            return Reindex(temp_list);
        }
    };//template<typename T,typename I = MonotonicID> class MonotonicIDList
}//namespace hgl