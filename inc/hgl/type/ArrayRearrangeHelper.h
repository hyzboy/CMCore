#pragma once
#include<initializer_list>
#include<type_traits>

namespace hgl
{
    /**
    * 数组重新排列辅助类
    */
    class ArrayRearrangeHelper
    {
        int64 data_count;         ///<数据总量
        int64 left_count;         ///<剩余数量
        int64 data_offset;        ///<当前访问偏移

        int64 field_count;        ///<分段数量

        struct Field
        {
            int64 start;
            int64 count;
        };

        Field *field_list;
        int64 field_index;

    public:

        ArrayRearrangeHelper(int64 dc,int64 fc)
        {
            data_count=dc;
            left_count=dc;
            field_count=fc;
            data_offset=0;

            field_list=new Field[fc];

            field_index=0;
        }

        ~ArrayRearrangeHelper()
        {
            delete[] field_list;
        }

        /**
        * 添加一个分段
        */
        bool AddField(int64 count)
        {
            if(count<0)return(false);
            if(count>left_count)return(false);
            if(field_index>=field_count)return(false);

            field_list[field_index].start=data_offset;
            field_list[field_index].count=count;

            data_offset+=count;
            left_count-=count;
            ++field_index;
            return(true);
        }

        /**
        * 添加多个分段
        */
        bool AddField(const std::initializer_list<int64> &count_list)
        {
            for(const int64 count:count_list)
                if(!AddField(count))
                   return(false);

            return(true);
        }

        /**
        * 结束分段（如果还有剩余数据，将会被添加到最后一个分段）
        */
        bool Finish()
        {
            if(left_count>0)
            {
                if(field_index>=field_count)return(false);

                field_list[field_index].start=data_offset;
                field_list[field_index].count=left_count;
                ++field_index;
            }

            return(true);
        }

        void Reset()
        {
            data_offset=0;
            left_count=data_count;
            field_index=0;
        }

        /**
        * 重新排列数据到一个新的数组中
        */
        template<typename T>
        bool Rearrange(T *new_array,const T *old_array,const int64 *index)
        {
            if(!Finish())
                return(false);

            T *p=new_array;
            Field *f;
            int64 constructed = 0;

            try
            {
                for(int64 i=0;i<field_index;i++)
                {
                    if(*index<0||*index>=field_index)
                        return(false);

                    f=field_list+(*index);

                    // 对于非平凡类型，需要使用移动构造而不是mem_copy
                    copy_construct_range<T>(p, old_array + f->start, f->count);
                    constructed += f->count;

                    p+=f->count;
                    ++index;
                }
            }
            catch(...)
            {
                // 如果构造失败，析构已构造的对象
                if(constructed > 0 && !std::is_trivially_destructible_v<T>)
                {
                    destroy_range<T>(new_array, constructed);
                }
                return(false);
            }

            return(true);
        }

        template<typename T>
        bool Rearrange(T *new_array,const T *old_array,const std::initializer_list<int64> &index)
        {
            return Rearrange(new_array,old_array,index.begin());
        }
    };//class ArrayRearrangeHelper

    template<typename T>
    inline bool ArrayRearrange(       T *new_array,
                                const T *old_array,const int64 count,
                                const std::initializer_list<int64> &field_list,
                                const std::initializer_list<int64> &index)
    {
        int64 field_count=(int)field_list.size();

        int64 total=0;

        for(const int64 fc:field_list)total+=fc;

        if(total<count)
            ++field_count;

        ArrayRearrangeHelper arh(count,field_count);

        if(!arh.AddField(field_list))
            return(false);

        return arh.Rearrange(new_array,old_array,index);
    }
}//namespace hgl
