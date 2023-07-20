#pragma once
#include<initializer_list>

namespace hgl
{
    /**
    * 数组重新排列辅助类
    */
    class ArrayRearrangeHelper
    {
        int data_count;         ///<数据总量
        int left_count;         ///<剩余数量
        int data_offset;        ///<当前访问偏移

        int field_count;        //分段数量
        
        struct Field
        {
            int start;
            int count;
        };
        
        Field *field_list;
        int field_index;

    public:

        ArrayRearrangeHelper(int dc,int fc)
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
        bool AddField(int count)
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
        bool AddField(const std::initializer_list<int> &count_list)
        {
            for(const int count:count_list)
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
        bool Rearrange(T *new_array,const T *old_array,const int *index)
        {
            if(!Finish())
                return(false);

            T *p=new_array;
            Field *f;

            for(int i=0;i<field_index;i++)
            {
                if(*index<0||*index>=field_index)
                    return(false);

                f=field_list+(*index);

                hgl_cpy<T>(p,old_array+f->start,f->count);

                p+=f->count;
                ++index;
            }

            return(true);
        }

        template<typename T>
        bool Rearrange(T *new_array,const T *old_array,const std::initializer_list<int> &index)
        {
            return Rearrange(new_array,old_array,index.begin());
        }
    };//class ArrayRearrangeHelper

    template<typename T>
    inline bool ArrayRearrange(T *new_array,const T *old_array,const int count,const std::initializer_list<int> &field_list,const std::initializer_list<int> &index)
    {
        int field_count=(int)field_list.size();

        int total=0;

        for(const int fc:field_list)total+=fc;

        if(total<count)
            ++field_count;

        ArrayRearrangeHelper arh(count,field_count);

        if(!arh.AddField(field_list))
            return(false);

        return arh.Rearrange(new_array,old_array,index);
    }
}//namespace hgl

