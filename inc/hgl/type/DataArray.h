#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ArrayRearrangeHelper.h>
#include<hgl/type/ArrayItemProcess.h>
#include<type_traits>
#include<utility>

namespace hgl
{
    /**
     * 数据阵列（基于平凡类型的轻量实现）
     *
     * 仅适用于支持 memcpy/memmove/mem_zero 的类型（trivially copyable）。
     * 非平凡类型请使用具备构造/析构处理的完整版本。
     */
    template<typename T> class DataArray: public Comparator<DataArray<T> >
    {
    protected:

        T *items;                                                                                   ///<数据指针
        int64 count;                                                                                ///<当前数据数量
        int64 alloc_count;                                                                          ///<已分配的容量

    public:

                DataArray()
                {
                    items=nullptr;
                    count=0;
                    alloc_count=0;
                }

        explicit    DataArray(int64 size)
                {
                    items=nullptr;
                    count=0;
                    alloc_count=0;

                    if(size>0)
                        Resize(size);
                }

        virtual     ~DataArray(){ Free(); }

        // 基本属性
                int64   GetCount        ()const{return count;}                                      ///<取得数据数量(注：非字节数)
        const   int64   GetAllocCount   ()const{return alloc_count;}                                ///<取得已分配的阵列大小(注：非字节数)
        const   int64   GetTotalBytes   ()const{return count*sizeof(T);}                            ///<取得阵列已使用的字节数
        const   int64   GetAllocBytes   ()const{return alloc_count*sizeof(T);}                      ///<取得阵列已分配空间字节数
        const   bool    IsEmpty         ()const{return count==0;}                                   ///<是否为空

                T *     GetData         ()const{return items;}
                T *     data            ()const{return items;}

        // 迭代
                T *     begin           ()const{return items;}                                      ///<起始地址
                T *     end             ()const{return items+count;}                                ///<结束地址
                T *     last            ()const{return (count>0)?(items+(count-1)):nullptr;}        ///<最后一个元素

        // 访问
        operator T *()const{return items;}
        T *operator ->()const{return items;}

        T &operator[](int64 n){ return items[n]; }
        const T &operator[](int64 n)const{ return items[n]; }

        T *At(int64 n)const{ return (n<0||n>=count)?nullptr:items+n; }

        // 比较
        const int compare(const DataArray<T> &other)const override
        {
            if(count==other.count)
                return mem_compare<T>(items,other.items,count);

            const int64 cmp_count=hgl_min(count,other.count);
            int result=mem_compare<T>(items,other.items,cmp_count);

            if(result)
                return result;

            return count-other.count;
        }

        // 查找
        const int64 Find(const T &data,const int64 start=0,int64 find_count=-1)const
        {
            if(!items||count<=0||start<0||start>=count)return -1;

            if(find_count<0||find_count>count-start)
                find_count=count-start;

            const int64 rel=FindDataPositionInArray<T>(items+start,find_count,data);
            return (rel<0)?-1:start+rel;
        }

        bool Contains(const T &data)const{ return Find(data)>=0; }

        // 清零
        void Zero()
        {
            if(!items||count<=0)return;

            mem_zero(items,static_cast<size_t>(count));
        }

        // 读写
        const bool ReadAt(T &obj,const int64 index)const
        {
            if(index<0||index>=count)return false;

            mem_copy(obj,items[index]);
            return true;
        }

        const bool ReadAt(T *obj,const int64 start,const int64 num)const
        {
            if(!obj||start<0||start+num>count)return false;

            mem_copy(obj,items+start,num);
            return true;
        }

        const bool WriteAt(const T &obj,const int64 index)
        {
            if(index<0||index>=count)return false;

            mem_copy(items[index],obj);
            return true;
        }

        const bool WriteAt(const T *obj,const int64 start,const int64 num)
        {
            if(!obj||start<0||start+num>count)return false;

            mem_copy(items+start,obj,num);
            return true;
        }

        // 容量管理
        bool Reserve(int64 size)
        {
            if(size<=0)return false;
            if(size<=alloc_count)return true;                                                       //不缩容

            const int64 new_alloc=power_to_2(size);
            T *new_items=hgl_align_malloc<T>(new_alloc);

            if(items)
            {
                mem_copy(new_items,items,count);
                hgl_free(items);
            }

            items=new_items;
            alloc_count=new_alloc;
            return true;
        }

        int64 Resize(int64 size)                                                                     ///<设置阵列长度(注：非字节数)
        {
            const int64 old_count=count;

            if(size<=0)
            {
                Free();
                count=0;
                return 0;
            }

            if(size>alloc_count)
                Reserve(size);

            if(size>old_count)
                mem_zero(items+old_count,static_cast<size_t>(size-old_count));

            count=size;
            return count;
        }

        int64 Expand(int64 size){ return Resize(count+size); }

        // 内存管理
        void SetData(T *data,int64 data_count)
        {
            Free();
            items=data;
            alloc_count=count=data_count;
        }

        void Free()
        {
            if(items)
            {
                hgl_free(items);
                items=nullptr;
                alloc_count=0;
            }

            count=0;
        }

        void Clear(){ count=0; }

        void Unlink()
        {
            items=nullptr;
            count=0;
            alloc_count=0;
        }

        // 修改
        void Append(const T &obj)
        {
            if(count>=alloc_count)
                Reserve(alloc_count+1);

            items[count]=obj;
            ++count;
        }

        void Exchange(int64 a,int64 b)
        {
            hgl_swap(items[a],items[b]);
        }

        bool Delete(int64 start,int64 delete_count=1)
        {
            if(start<0||start>=count)return false;
            if(delete_count<=0)return false;

            if(start+delete_count>count)
                delete_count=count-start;

            for(int64 i=start+delete_count;i<count;++i)
                items[i-delete_count]=std::move(items[i]);

            count-=delete_count;
            return true;
        }

        bool DeleteShift(int64 start,int64 delete_count=1)
        {
            return Delete(start,delete_count);
        }

        bool Insert(int64 pos,const T *data,const int64 data_number)
        {
            if(pos<0||pos>count)return false;
            if(data_number<=0)return true;
            if(!data)return false;

            if(count+data_number>alloc_count)
                Reserve(count+data_number);

            if(pos<count)
            {
                int64 move_count=count-pos;
                for(int64 i=move_count-1;i>=0;--i)
                {
                    int64 src=pos+i;
                    int64 dst=pos+data_number+i;
                    items[dst]=std::move(items[src]);
                }
            }

            for(int64 i=0;i<data_number;++i)
                items[pos+i]=data[i];

            count+=data_number;
            return true;
        }

        void Move(int64 new_index,int64 old_index,int64 move_number=1)
        {
            if(!items)return; if(new_index==old_index)return; if(old_index<0||old_index>=count)return;

            if(new_index<0)new_index=0;
            if(new_index>=count)new_index=count;

            if(old_index+move_number>count)
                move_number=count-old_index;

            if(move_number<=0)return;

            const int64 new_alloc_count=power_to_2(count);
            T *new_items=hgl_align_malloc<T>(new_alloc_count);
            bool result=false;

            if(old_index==0)
            {
                if(new_index>=count)
                    result=ArrayRearrange(new_items,items,count,{move_number,count-move_number},{1,0});
                else
                    result=ArrayRearrange(new_items,items,count,{move_number,new_index-move_number},{1,0,2});
            }
            else if(old_index+move_number>=count)
            {
                if(new_index==0)
                    result=ArrayRearrange(new_items,items,count,{old_index,move_number},{1,0});
                else
                    result=ArrayRearrange(new_items,items,count,{new_index,old_index-new_index},{0,2,1});
            }
            else
            {
                if(new_index==0)
                    result=ArrayRearrange(new_items,items,count,{old_index,move_number},{1,0,2});
                else if(new_index>=count)
                    result=ArrayRearrange(new_items,items,count,{old_index,move_number},{0,2,1});
                else if(new_index<old_index)
                    result=ArrayRearrange(new_items,items,count,{new_index,old_index-new_index,move_number},{0,2,1,3});
                else
                    result=ArrayRearrange(new_items,items,count,{old_index,move_number,new_index-(old_index+move_number)},{0,2,1,3});
            }

            if(result)
            {
                hgl_free(items);
                items=new_items;
                alloc_count=new_alloc_count;
            }
            else
            {
                hgl_free(new_items);
            }
        }

        // 赋值辅助
        void operator = (const DataArray<T> &da)
        {
            if(da.count<=0){ count=0; return; }

            Resize(da.GetCount());
            mem_copy<T>(items,da.items,count);
        }

        void operator = (const std::initializer_list<T> &l)
        {
            Resize((int64)l.size());
            mem_copy<T>(items,l.begin(),count);
        }

        // 集合运算
        void WithoutList(DataArray<T> &result_list,const DataArray<T> &without_list)
        {
            result_list.Clear();
            const int64 local_count=this->GetCount();

            if(local_count<=0)return;

            result_list.Clear();
            result_list.Reserve(local_count);

            for(T &sp:*this)
            {
                if(!without_list.Contains(sp))
                    result_list.Append(sp);
            }
        }
    };
}
