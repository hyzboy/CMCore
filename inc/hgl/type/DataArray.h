#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ArrayRearrangeHelper.h>
#include<hgl/type/ArrayItemProcess.h>
#include<type_traits>
#include<utility>
#include<algorithm>

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

        static_assert(std::is_trivially_copyable_v<T>, "DataArray only supports trivially copyable types; use non-trivial version instead.");

        T *items;                                                                                   ///<数据指针
        int64 count;                                                                                ///<当前数据数量
        int64 alloc_count;                                                                          ///<已分配的容量

    public:

        /**
         * @brief 默认构造，初始化为空阵列。
         */
                DataArray()
                {
                    items=nullptr;
                    count=0;
                    alloc_count=0;
                }

        /**
         * @brief 预分配指定长度的阵列并置零。
         * @param size 预期元素数量（非字节数）。
         */
        explicit    DataArray(int64 size)
                {
                    items=nullptr;
                    count=0;
                    alloc_count=0;

                    if(size>0)
                        Resize(size);
                }

        /**
         * @brief 析构函数，释放内部缓冲。
         */
        virtual     ~DataArray(){ Free(); }

        // 基本属性
        /** @brief 取得元素数量（非字节数）。 */
                int64   GetCount        ()const{return count;}                                      ///<取得元素数量（非字节数）
        /** @brief 取得已分配的阵列容量（非字节数）。 */
        const   int64   GetAllocCount   ()const{return alloc_count;}                                ///<取得已分配的阵列容量（非字节数）
        /** @brief 取得已使用的字节数。 */
        const   int64   GetTotalBytes   ()const{return count*sizeof(T);}                            ///<取得已使用的字节数
        /** @brief 取得已分配的总字节数。 */
        const   int64   GetAllocBytes   ()const{return alloc_count*sizeof(T);}                      ///<取得已分配的总字节数
        /** @brief 是否为空。 */
        const   bool    IsEmpty         ()const{return count==0;}                                   ///<是否为空

        /** @brief 返回底层数据指针。 */
                T *     GetData         ()const{return items;}                                      ///<返回底层数据指针
        /** @brief 返回底层数据指针（STL 风格）。 */
                T *     data            ()const{return items;}                                      ///<返回底层数据指针（STL风格）

        // 迭代
        /** @brief 取得起始迭代器。 */
                T *     begin           ()const{return items;}                                      ///<取得起始迭代器
        /** @brief 取得尾后迭代器。 */
                T *     end             ()const{return items+count;}                                ///<取得尾后迭代器
        /** @brief 取得最后一个元素指针。 */
                T *     last            ()const{return (count>0)?(items+(count-1)):nullptr;}        ///<取得最后一个元素指针

        // 访问
        /** @brief 隐式转换为原始指针。 */
        operator T *()const{return items;}                                                          ///<隐式转换为原始指针
        /** @brief 成员访问运算符。 */
        T *operator ->()const{return items;}                                                        ///<成员访问运算符

        /** @brief 下标访问（可写）。 */
        T &operator[](int64 n){ return items[n]; }                                                  ///<下标访问（可写）
        /** @brief 下标访问（只读）。 */
        const T &operator[](int64 n)const{ return items[n]; }                                       ///<下标访问（只读）

        /**
         * @brief 安全访问指定位置。
         * @param n 索引。
         * @return 指向元素的指针，越界时返回 nullptr。
         */
        T *At(int64 n)const{ return (n<0||n>=count)?nullptr:items+n; }

        // 比较
        /**
         * @brief 与另一数组逐元素比较。
         * @param other 待比较的数组。
         * @return 0 相等，<0 当前数组更小，>0 当前数组更大。
         */
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
        /**
         * @brief 从指定位置开始查找数据。
         * @param data 要查找的值。
         * @param start 起始索引。
         * @param find_count 最大查找数量，负数表示查找到尾。
         * @return 找到返回索引，未找到返回 -1。
         */
        const int64 Find(const T &data,const int64 start=0,int64 find_count=-1)const
        {
            if(!items||count<=0||start<0||start>=count)return -1;

            if(find_count<0||find_count>count-start)
                find_count=count-start;

            const int64 rel=FindDataPositionInArray<T>(items+start,find_count,data);
            return (rel<0)?-1:start+rel;
        }

        /** @brief 判断是否包含指定元素。 */
        bool Contains(const T &data)const{ return Find(data)>=0; }

        // 清零
        /** @brief 将当前有效范围清零。 */
        void Zero()
        {
            if(!items||count<=0)return;

            std::fill_n(items,static_cast<size_t>(count),T{});
        }

        // 读写
        /**
         * @brief 读取指定位置元素。
         * @param obj 输出对象。
         * @param index 索引。
         * @return true 读取成功。
         */
        const bool ReadAt(T &obj,const int64 index)const
        {
            if(index<0||index>=count)return false;

            obj=items[index];
            return true;
        }

        /**
         * @brief 读取一段连续数据。
         * @param obj 输出缓冲。
         * @param start 起始索引。
         * @param num 读取数量。
         * @return true 成功。
         */
        const bool ReadAt(T *obj,const int64 start,const int64 num)const
        {
            if(!obj||start<0||start+num>count)return false;

            std::copy_n(items+start,static_cast<size_t>(num),obj);
            return true;
        }

        /**
         * @brief 写入单个元素到指定位置。
         * @param obj 待写入数据。
         * @param index 目标索引。
         * @return true 成功。
         */
        const bool WriteAt(const T &obj,const int64 index)
        {
            if(index<0||index>=count)return false;

            items[index]=obj;
            return true;
        }

        /**
         * @brief 将连续数据写入指定位置。
         * @param obj 源数据指针。
         * @param start 起始索引。
         * @param num 写入数量。
         * @return true 成功。
         */
        const bool WriteAt(const T *obj,const int64 start,const int64 num)
        {
            if(!obj||start<0||start+num>count)return false;

            std::copy_n(obj,static_cast<size_t>(num),items+start);
            return true;
        }

        // 容量管理
        /**
         * @brief 预留至少给定数量的空间，不缩容。
         * @param size 目标容量。
         * @return true 成功分配或已有足够空间。
         */
        bool Reserve(int64 size)
        {
            if(size<=0)return false;
            if(size<=alloc_count)return true;                                                       //不缩容

            const int64 new_alloc=power_to_2(size);
            T *new_items=hgl_align_malloc<T>(new_alloc);

            if(items)
            {
                std::copy_n(items,static_cast<size_t>(count),new_items);
                hgl_free(items);
            }

            items=new_items;
            alloc_count=new_alloc;
            return true;
        }

        /**
         * @brief 调整长度，扩展部分清零。
         * @param size 新长度。
         * @return 调整后的长度。
         */
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
                std::fill_n(items+old_count,static_cast<size_t>(size-old_count),T{});

            count=size;
            return count;
        }

        /**
         * @brief 在当前长度基础上扩展。
         * @param size 增量。
         * @return 扩展后的长度。
         */
        int64 Expand(int64 size){ return Resize(count+size); }

        // 内存管理
        /**
         * @brief 直接接管外部数据指针。
         * @param data 外部指针。
         * @param data_count 元素数量。
         */
        void SetData(T *data,int64 data_count)
        {
            Free();
            items=data;
            alloc_count=count=data_count;
        }

        /** @brief 释放内部缓冲并重置计数。 */
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

        /** @brief 清空元素计数但保留缓冲。 */
        void Clear(){ count=0; }

        /** @brief 解除与外部缓冲的关联，不释放内存。 */
        void Unlink()
        {
            items=nullptr;
            count=0;
            alloc_count=0;
        }

        // 修改
        /**
         * @brief 追加单个元素，必要时自动扩容。
         * @param obj 要追加的元素。
         */
        void Append(const T &obj)
        {
            if(count>=alloc_count)
                Reserve(alloc_count+1);

            items[count]=obj;
            ++count;
        }

        /**
         * @brief 交换两个位置的元素。
         * @param a 位置 A。
         * @param b 位置 B。
         */
        void Exchange(int64 a,int64 b)
        {
            hgl_swap(items[a],items[b]);
        }

        /**
         * @brief 顺序删除指定区间。
         * @param start 起始索引。
         * @param delete_count 删除数量。
         * @return true 删除成功。
         */
        bool Delete(int64 start,int64 delete_count=1)
        {
            if(start<0||start>=count)return false;
            if(delete_count<=0)return false;

            if(start+delete_count>count)
                delete_count=count-start;

            // [left][delete][right]
            //   ^start        ^start+delete_count
            //   移动 right 段覆盖 delete 段，实现顺序删除
            std::move(items+start+delete_count,items+count,items+start);

            count-=delete_count;
            return true;
        }

        /**
         * @brief 顺序删除（与 Delete 同义）。
         * @param start 起始索引。
         * @param delete_count 删除数量。
         * @return true 删除成功。
         */
        bool DeleteShift(int64 start,int64 delete_count=1)
        {
            // 顺序删除，与 Delete 相同，整体左移填补空洞
            return Delete(start,delete_count);
        }

        /**
         * @brief 将一段数据移动到新位置，保持顺序。
         * @param new_index 目标索引。
         * @param old_index 源起始索引。
         * @param move_number 移动数量。
         */
        void Move(int64 new_index,int64 old_index,int64 move_number=1)
        {
            if(!items)return; if(move_number<=0)return; if(old_index<0||old_index>=count)return;

            if(new_index<0)new_index=0;
            if(new_index>count)new_index=count;

            if(old_index+move_number>count)
                move_number=count-old_index;

            if(move_number<=0)return;

            // 目标落在自身区间，等同无操作
            if(new_index>=old_index&&new_index<=old_index+move_number)
                return;

            // 临时缓冲存放要移动的数据块，避免整段重分配
            T *temp=hgl_align_malloc<T>(move_number);
            if(!temp)return;

            std::copy_n(items+old_index,static_cast<size_t>(move_number),temp);

            if(new_index<old_index)            // 提前：右移 [new_index, old_index) 段
            {
                //   new_index            old_index              old_index+move_number
                //   v                    v                      v
                // [----left----][ old ][----mid----][ right ]
                // [ old ][----left+mid----][ right ]
                std::move_backward(items+new_index,items+old_index,items+old_index+move_number);
                std::copy_n(temp,static_cast<size_t>(move_number),items+new_index);
            }
            else                                // 推后：左移 (old_index+move_number, new_index) 段
            {
                //   old_index          old_index+move_number         new_index
                //   v                  v                             v
                // [ left ][ old ][------mid------][ right ]
                // [ left ][------mid------][ old ][ right ]
                const int64 mid_len=new_index-(old_index+move_number);
                std::move(items+old_index+move_number,items+old_index+move_number+mid_len,items+old_index);
                std::copy_n(temp,static_cast<size_t>(move_number),items+old_index+mid_len);
            }

            hgl_free(temp);
        }

        // 赋值辅助
        /**
         * @brief 拷贝赋值，长度自动调整。
         * @param da 源数组。
         */
        void operator = (const DataArray<T> &da)
        {
            if(da.count<=0){ count=0; return; }

            Resize(da.GetCount());
            std::copy_n(da.items,static_cast<size_t>(count),items);
        }

        /**
         * @brief 使用初始化列表赋值。
         * @param l 初始化列表。
         */
        void operator = (const std::initializer_list<T> &l)
        {
            Resize((int64)l.size());
            std::copy_n(l.begin(),static_cast<size_t>(count),items);
        }

        // 集合运算
        /**
         * @brief 生成不包含指定列表元素的新列表。
         * @param result_list 输出结果列表。
         * @param without_list 需要排除的列表。
         */
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
