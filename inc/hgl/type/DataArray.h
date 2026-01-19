#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ArrayRearrangeHelper.h>
#include<hgl/type/ArrayItemProcess.h>
namespace hgl
{
    /**
     * 数据阵列
     * 
     * 支持 trivial 和 non-trivial 类型。
     * 注意：对于 non-trivial 类型（如 std::string），会使用构造/析构和移动语义，
     *       性能可能不如 trivial 类型。
     * 
     * 如果确定只使用 trivial 类型（如 int, float, POD struct），建议使用 TrivialDataArray 以获得更好的性能。
     */
    template<typename T> class DataArray:Comparator<DataArray<T>>
    {
    protected:

        T *items;
        int64 count;                                                                                ///<当前数据数量
        int64 alloc_count;                                                                          ///<已分配的数据数量

    public:

                int64   GetCount        ()const{return count;}                                      ///<取得数据数量(注：非字节数)
        const   int64   GetAllocCount   ()const{return alloc_count;}                                ///<取得已分配的阵列大小(注：非字节数)
        const   int64   GetTotalBytes   ()const{return count*sizeof(T);}                            ///<取得阵列已使用的字节数
        const   int64   GetAllocBytes   ()const{return alloc_count*sizeof(T);}                      ///<取得阵列已分配空间字节数

        const   bool    IsEmpty         ()const{return(count==0);}                                  ///<是否为空

                T *     GetData         ()const{return items;}
                T *     data            ()const{return items;}

    public:

                T *     begin           ()const{return items;}                                      ///<取得阵列起始地址指针
                T *     end             ()const{return items+count;}                                ///<取得阵列结束地址指针
                T *     last            ()const{return (count>0)?(items+(count-1)):nullptr;}        ///<取得阵列最后一个数据地址指针

        const int       compare         (const DataArray<T> &vil)const override
        {
            if(count==vil.count)
                return mem_compare<T>(items,vil.items,count);

            int cmp_count=hgl_min(count,vil.count);

            int result=mem_compare<T>(items,vil.items,cmp_count);

            if(result)
                return(result);

            return count-vil.count;
        }

        void SetData(T *data,int64 data_count)
        {
            Free();

            items=data;
            alloc_count=count=data_count;            
        }

    public:

        /**
         * 分配指定空间出来，供未来使用
         */
        bool Reserve(int64 size)
        {
            if(size<=alloc_count)
                return(true);

            const int64 new_alloc_count = power_to_2(size);

            if(!items)
            {
                items=hgl_align_malloc<T>(new_alloc_count);
                alloc_count = new_alloc_count;
            }
            else
            {
                // 对于 non-trivial 类型，不能直接使用 realloc
                if constexpr(!std::is_trivially_copyable_v<T>)
                {
                    // 分配新内存
                    T* new_items = hgl_align_malloc<T>(new_alloc_count);
                    
                    if(new_items)
                    {
                        // 移动已构造的对象到新位置
                        for(int64 i = 0; i < count; i++)
                        {
                            new (new_items + i) T(std::move(items[i]));  // 移动构造
                            items[i].~T();  // 析构旧对象
                        }
                        
                        // 释放旧内存
                        hgl_free(items);
                        items = new_items;
                        alloc_count = new_alloc_count;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    // trivially copyable 类型可以安全使用 realloc
                    items=hgl_align_realloc<T>(items,new_alloc_count);
                    alloc_count = new_alloc_count;
                }
            }

            return(items!=nullptr);
        }

        /**
         * 设置阵列长度
         * @param size 阵列长度
         * @return 设置成功后的阵列长度
         */
        int64 Resize(int64 size)                                                                  ///<设置阵列长度(注：非字节数)
        {
            const int64 old_count = count;
            
            Reserve(size);

            // 对于 non-trivial 类型，需要构造新增的对象
            if constexpr(!std::is_trivially_constructible_v<T>)
            {
                if(size > old_count)
                {
                    // 构造新增的元素
                    for(int64 i = old_count; i < size; i++)
                    {
                        new (items + i) T();  // placement new with default constructor
                    }
                }
                else if(size < old_count)
                {
                    // 析构移除的元素
                    for(int64 i = size; i < old_count; i++)
                    {
                        items[i].~T();
                    }
                }
            }

            count=size;
            return count;
        }

        /**
         * 增加阵列长度
         * @param size 增加的长度
         * @return 增加后的阵列长度
         */
        int64 Expand(int64 size)
        {
            return Resize(count+size);
        }

    public:

        DataArray()
        {
            items=nullptr;
            count=0;
            alloc_count=0;
        }

        DataArray(int64 size)
        {
            items=nullptr;
            count=0;
            alloc_count=0;
            
            if(size > 0)
            {
                Resize(size);  // 使用 Resize 来正确构造对象
            }
        }

        virtual ~DataArray()
        {
            Free();
        }

        void Free()
        {
            // 对于 non-trivial 类型，需要先析构所有对象
            destroy_range<T>(items, count);
            
            SAFE_FREE(items);

            count=0;
            alloc_count=0;
        }

        void Clear()
        {
            count=0;
        }

        void Zero()
        {
            if(items)
                mem_zero(items,alloc_count);
        }

        /**
         * 解除数据关联
         */
        void Unlink()
        {
            items=nullptr;
            count=0;
            alloc_count=0;
        }

        operator T *()const
        {
            return items;
        }

        T *operator ->()const
        {
            return items;
        }

        T &operator[](int64 n)
        {
            return items[n];
        }

        const T &operator[](int64 n)const
        {
            return items[n];
        }

        void Append(const T &obj)
        {
            if(count>=alloc_count)
                Reserve(count+1);

            items[count]=obj;
            ++count;
        }

        T *At(int64 n)const
        {
            return (n<0||n>=count)?nullptr:items+n;
        }

        const bool ReadAt(T &obj,int64 const index)const
        {
            if(index<0||index>=count)return(false);

            mem_copy(obj,items[index]);
            return(true);
        }

        const bool ReadAt(T *obj,const int64 start,const int64 num)const
        {
            if(!obj||start<0||start+num>count)return(false);

            mem_copy(obj,items+start,num);
            return(true);
        }

        const bool WriteAt(const T &obj,const int64 index)
        {
            if(index<0||index>=count)return(false);

            mem_copy(items[index],obj);
            return(true);
        }

        const bool WriteAt(const T *obj,const int64 start,const int64 num)
        {
            if(!obj||start<0||start+num>count)return(false);

            mem_copy(items+start,obj,num);
            return(true);
        }

        /**
        * 复制整个阵列
        */
        void operator = (const DataArray<T> &da)
        {
            if(da.count<=0)
            {
                count=0;
                return;
            }

            Resize(da.GetCount());

            mem_copy<T>(items,da.items,count);
        }

        void operator = (const std::initializer_list<T> &l)
        {
            Resize((int64)l.size());

            mem_copy<T>(items,l.begin(),count);
        }

        /**
        * 删除列表中的指定项(不关心顺序，如果删除中间的数据，可能会将最后面的数据拿过来填补)
        * @param start 要删除的数据项的索引起始值
        * @param delete_count 要删除的数据项数量
        * @return 是否成功
        */
        bool Delete(int64 start,int64 delete_count=1)
        {
            if(!items)return(false);
            if(start>=count)return(false);

            if(start<0)
            {
                delete_count+=start;
                start=0;
            }

            if(start+delete_count>count)
                delete_count=count-start;

            if(delete_count<=0)return(false);

            const int64 end_count=count-(start+delete_count);               //删除的数据段后面的数据个数

            if(end_count>0)
            {
                if(end_count<=delete_count)                                 //后面的数据个数比删除的数据个数少，直接整体移动
                {
                    //[------------][***********]
                    //      ^             v
                    //      ^             v
                    //      +<<<<<<<<<<<<<+

                    mem_copy<T>(items+start,items+start+delete_count,end_count);
                }
                else                                                        //后面的数据个数比删除的数据个数多，那就只移动等长的最后一段数据
                {
                    //[---][**********][***]
                    //  ^                v
                    //  ^                v
                    //  +<<<<<<<<<<<<<<<<+

                    mem_copy<T>(items+start,items+(count-delete_count),delete_count);
                }
            }
            //else{后面都没数据了，那就啥都不用干了}

            // 对于 non-trivial 类型，析构被移除的元素
            destroy_range<T>(items, count - delete_count, count);

            count-=delete_count;

            return(true);
        }

        /**
        * 删除列表中的指定项(关心顺序，删除中间数据后，会将后面的数据整体前移)
        * @param start 要删除的数据项的索引值
        * @return 是否成功
        */
        bool DeleteShift(int64 start,int64 delete_count=1)
        {
            if(!items)return(false);
            if(start>=count)return(false);

            if(start<0)
            {
                delete_count+=start;
                start=0;
            }

            if(start+delete_count>count)
                delete_count=count-start;

            if(delete_count<=0)return(false);

            const int64 end_count=count-(start+delete_count);

            if(end_count>0)
                mem_move<T>(items+start,items+start+delete_count,end_count);  // 使用 mem_move 而不是 mem_copy

            // 对于 non-trivial 类型，析构被移除的元素
            destroy_range<T>(items, count - delete_count, count);

            count-=delete_count;

            return(true);
        }

        /**
        * 交换列表中两个数据的位置
        * @param a 第一个数据的位置
        * @param b 第二个数据的位置
        */
        void Exchange(int64 a,int64 b)
        {
            hgl_swap(items[a],items[b]);
        }

        /**
        * 移动数据一定量的数据到新的位置
        * @param new_index 新的位置
        * @param old_index 旧的位置
        * @param move_number 要移动的数据个数
        */
        bool Move(int64 new_index,int64 old_index,int64 move_number=1)
        {
            if(!items)return(false);
            if(new_index==old_index)return(false);
            if(old_index<0||old_index>=count)return(false);

            if(new_index<0)new_index=0;
            if(new_index>=count)new_index=count;

            if(old_index+move_number>count)
                move_number=count-old_index;  // 修正：限制为从old_index到结尾的元素数量

            if(move_number<=0)return(false);

            //直接创建新缓冲区复制过去，放弃旧缓冲区
            const int64 new_alloc_count=power_to_2(count);
            T *new_items=hgl_align_malloc<T>(new_alloc_count);

            bool result;

            if(old_index==0)            // [old]------
            {
                if(new_index>=count)
                {
                    //   0                end count
                    //   +---++-----------+
                    //   [old]            ^new index
                    //        [----mid----]

                    //   [----mid----][old]

                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                move_number,            //old
                                                count-move_number       //mid
                                            },
                                            {
                                                1,  //mid
                                                0   //old
                                            });
                }
                else
                {
                    //   0                end count
                    //   +---+-----++-----+
                    //   [old]      ^new index
                    //        [left][right]

                    //   [left][old][right]

                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                move_number,            //old
                                                new_index-move_number   //left
                                                                        //right,we don't write,and then it should to auto compute
                                            },
                                            {
                                                1,  //left
                                                0,  //old
                                                2   //right
                                            });
                }
            }
            else if(old_index+move_number>=count)// -----[old]
            {
                if(new_index==0)
                {
                    //   0                 end count
                    //   +-------------+---+
                    //   ^new_index    [old]
                    //   [----left----]

                    //   [old][----left----]

                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                old_index,      //left
                                                move_number,    //old
                                            },
                                            {
                                                1,              //old
                                                0               //left
                                            });
                }
                else
                {
                    //   0                end count
                    //   +----++-----++---+
                    //         ^new_index
                    //   [left][-mid-][old]

                    //   [left][old][-mid-]

                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                new_index,              //left
                                                old_index-new_index,    //mid
                                                                        //old,we don't write,and then it should to auto compute
                                            },
                                            {
                                                0,  //left
                                                2,  //old
                                                1   //mid
                                            });
                }
            }
            else  // ----[old]----
            {
                if(new_index==0)
                {
                    //   new_index
                    //   v
                    //   v                  end count
                    // 0 +----++-----++-----+
                    //   [left][-old-][right]

                    //   [-old-][left][right]
                    
                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                old_index,      //left
                                                move_number,    //old
                                                                //right,we don't write,and then it should to auto compute
                                            },
                                            {
                                                1,  //left
                                                0,  //old
                                                2   //right
                                            });
                }
                else
                if(new_index>=count)
                {
                    //                  new_index
                    //                  v
                    // 0                v
                    // +----++---++-----+ end count
                    // [left][old][right]

                    // [left][right][old]

                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                old_index,      //left
                                                move_number,    //old
                                                                //right,we don't write,and then it should to auto compute
                                            },
                                            {
                                                0,  //left
                                                2,  //right
                                                1   //old
                                            });
                }
                else if(new_index<old_index)
                {
                    //       new_index
                    //       v
                    // 0     v                end count
                    // +----++---++---++-----+
                    // [left][mid][old][right]

                    // [left][old][mid][right]

                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                new_index,              //left
                                                old_index-new_index,    //mid
                                                move_number,            //old
                                                                        //right,we don't write,and then it should to auto compute
                                            },
                                            {
                                                0,  //left
                                                2,  //old
                                                1,  //mid
                                                3   //right
                                            });
                }
                else
                {
                    //                 new_index
                    //                 v
                    // 0               v     end count
                    // +----++---++---++-----+
                    // [left][old][mid][right]

                    // [left][mid][old][right]

                    result=ArrayRearrange(  new_items,items,count,
                                            {
                                                old_index,              //left
                                                move_number,            //old
                                                new_index-(old_index+move_number),  //mid
                                                                        //right,we don't write,and then it should to auto compute
                                            },
                                            {
                                                0,  //left
                                                2,  //mid
                                                1,  //old
                                                3   //right
                                            });
                }
            }

            if(result)
            {
                // 对于非平凡类型，需要显式调用旧对象的析构函数
                destroy_range<T>(items, count);
                
                hgl_free(items);
                items=new_items;
                alloc_count=new_alloc_count;

                return(true);
            }
            else
            {
                // ArrayRearrange 失败，释放新分配的内存
                hgl_free(new_items);
                return(false);
            }
        }

        /**
         * 查找数据在数组中的位置
         * @param data 要查找的数据
         * @param start 开始查找的位置
         * @param find_count 要查找的最大数量
         * @return <0 未找到或其它错误
         */
        const int64 Find(const T &data,const int64 start=0,int64 find_count=-1)const
        {
            if(!items||count<=0||start<0||start>=count)return(-1);

            if(find_count<0||find_count>count-start)
                find_count=count-start;

            return FindDataPositionInArray<T>(items+start,find_count,data);
        }

        /**
         * 检查数据是否存在于数组中
         * @param data 要查找的数据
         * @return true 存在，false 不存在
         */
        bool Contains(const T &data) const
        {
            return Find(data) >= 0;
        }

        /**
         * 在指定位置插入数据
         */
        bool Insert(int64 pos,const T *data,const int64 data_number)
        {
            if(!data||data_number<=0)
                return(false);

            if(pos<0)pos=0;
            if(pos>count)pos=count;

            if(count+data_number>alloc_count)
            {
                int64 new_alloc_count=power_to_2(alloc_count+data_number);

                T *new_items=hgl_align_malloc<T>(new_alloc_count);

                // 对于 non-trivial 类型，需要使用移动/复制构造到未初始化内存
                if constexpr(std::is_trivially_copyable_v<T>)
                {
                    if(pos>0)       mem_copy(new_items,items,pos);
                                    mem_copy(new_items+pos,data,data_number);
                    if(pos<count)   mem_copy(new_items+pos+data_number,items+pos,(count-pos));
                }
                else
                {
                    // 移动构造已存在的元素到新位置
                    if(pos>0)
                        move_construct_range(new_items, items, pos);
                    
                    // 复制构造要插入的元素
                    copy_construct_range(new_items+pos, data, data_number);
                    
                    // 移动构造后面的元素
                    if(pos<count)
                        move_construct_range(new_items+pos+data_number, items+pos, count-pos);
                    
                    // 析构旧元素
                    destroy_range<T>(items, count);
                }

                hgl_free(items);
                items=new_items;
                alloc_count=new_alloc_count;
            }
            else
            {
                // 空间足够，在原地插入
                if constexpr(std::is_trivially_copyable_v<T>)
                {
                    if(pos<count)
                        mem_move(items+pos+data_number,items+pos,count-pos);

                    mem_copy(items+pos,data,data_number);
                }
                else
                {
                    // Non-trivial 类型：需要小心处理未初始化内存
                    // 策略：从后往前，先移动构造到未初始化区域，再移动赋值到已初始化区域
                    if(pos<count)
                    {
                        int64 move_count = count - pos;
                        int64 new_end = count + data_number;
                        
                        // 从后往前处理：最后 data_number 个位置是未初始化的
                        for(int64 i = move_count - 1; i >= 0; i--)
                        {
                            int64 src_idx = pos + i;
                            int64 dst_idx = pos + data_number + i;
                            
                            if(dst_idx >= count)
                            {
                                // 目标位置是未初始化内存，使用移动构造
                                new (items + dst_idx) T(std::move(items[src_idx]));
                                items[src_idx].~T();  // 析构源对象
                            }
                            else
                            {
                                // 目标位置已初始化，使用移动赋值
                                items[dst_idx] = std::move(items[src_idx]);
                            }
                        }
                    }
                    
                    // 复制构造新元素（这些位置现在是未初始化或已析构）
                    for(int64 i = 0; i < data_number; i++)
                    {
                        new (items + pos + i) T(data[i]);
                    }
                }
            }

            count+=data_number;

            return(true);
        }

        bool Insert(int64 pos,const T &data)
        {
            return Insert(pos,&data,1);
        }

        /**
        * 统计出不在without_list中的数据，产生的结果洗脑result_list
        */
        void WithoutList(DataArray<T> &result_list,const DataArray<T> &without_list)
        {
            result_list.Clear();
            const int64 count=this->GetCount();

            if(count<=0)return;

            result_list.Clear();
            result_list.Reserve(count);

            int64 result=0;

            T *p=result_list.items;

            for(const T &sp:*this)
            {
                if(!without_list.Contains(sp))
                {
                    *p=sp;
                    ++p;
                    ++result;
                }
            }

            result_list.Resize(result);
        }

        //int64     Intersection    (SortedSet<T> &result,const SortedSet<T> &sets);          ///<取得与指定合集的交集
        //int64     Intersection    (const SortedSet<T> &set);                                 ///<取得与指定合集的交集数量

        ///**
        //    * 取得与指定交集is的合集，但排斥cs合集中的数据
        //    * @param result 结果合集
        //    * @param is 求交集的合集
        //    * @param cs 求排斥的合集
        //    * @return 结果数量
        //    */
        //int64     Intersection    (SortedSet<T> &result,const SortedSet<T> &is,const SortedSet<T> &cs);

        //int64     Difference      (const SortedSet<T> &is);                                  ///<求差集数量

        ///**
        // * 求当前合集与另一个数据集的交集
        // * @param result 结果存放合集
        // * @param list 要计算交集的数据集
        // * @return 交集数量
        // */
        //int64 Intersection(SortedSet<T> &result,const SortedSet<T> &list)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(0);

        //    if(list.GetCount()<=0)
        //        return(0);

        //    data_list.Enum([&](T &obj)
        //    {
        //        if(list->Contains(obj))
        //            result.Add(obj);
        //    });

        //    return result.GetCount();
        //}

        //int64 Intersection(const SortedSet<T> &list)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(0);

        //    if(list.GetCount()<=0)
        //        return(0);

        //    int64 count=0;

        //    T *obj=data_list.GetData();
        //    for(int64 i=0;i<data_list.GetCount();i++)
        //    {
        //        if(list.Contains(*obj))
        //            ++count;

        //        ++obj;
        //    }

        //    return count;
        //}

        //int64 Intersection(SortedSet<T> &result,const SortedSet<T> &il,const SortedSet<T> &cl)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(0);

        //    if(il.GetCount()<=0)
        //        return(0);

        //    T *obj=data_list.GetData();
        //    for(int64 i=0;i<data_list.GetCount();i++)
        //    {
        //        if(il.Contains(*obj))
        //            if(!cl.Contains(*obj))
        //                result.Add(*obj);
        //        ++obj;
        //    }

        //    return result.GetCount();
        //}

        //int64 Difference(const DataArray &is)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(is.GetCount());

        //    if(is.GetCount()<=0)
        //        return(data_list.GetCount());

        //    int64 count=0;

        //    T *obj=data_list.GetData();
        //    for(int64 i=0;i<data_list.GetCount();i++)
        //    {
        //        if(!is.Contains(*obj))
        //            ++count;

        //        ++obj;
        //    }

        //    return count;
        //}

        //int64     Clear           (const SortedSet<T> &clear_sets);                          ///<清除指定合集中所有数据
    };//template<typename T> class DataArray

    /**
     * Trivial 类型专用数据阵列（高性能版本）
     * 
     * 仅支持 trivial 类型（trivially copyable 且 trivially destructible）。
     * 相比通用的 DataArray，TrivialDataArray 有以下优势：
     * 1. 移除了所有 if constexpr 分支，代码更简洁
     * 2. 不需要调用构造/析构函数，性能更高
     * 3. 可以使用 realloc、memcpy、memmove 等高效内存操作
     * 4. 适用于：int, float, double, POD struct 等类型
     * 
     * 示例：
     *   TrivialDataArray<int> numbers;
     *   TrivialDataArray<Vector3f> vertices;
     *   TrivialDataArray<MyPODStruct> data;
     */
    template<typename T> 
    class TrivialDataArray : public DataArray<T>
    {
        // 编译时检查：只允许 trivial 类型
        static_assert(std::is_trivially_copyable_v<T>, 
                      "TrivialDataArray requires trivially copyable type. Use DataArray<T> for non-trivial types like std::string.");
        static_assert(std::is_trivially_destructible_v<T>, 
                      "TrivialDataArray requires trivially destructible type. Use DataArray<T> for types with custom destructors.");
        
    public:
        using DataArray<T>::items;
        using DataArray<T>::count;
        using DataArray<T>::alloc_count;

        // 使用基类的构造函数
        TrivialDataArray() : DataArray<T>() {}
        TrivialDataArray(int64 size) : DataArray<T>(size) {}

        // 优化的 Reserve（移除 if constexpr 分支）
        bool Reserve(int64 size)
        {
            if(size <= alloc_count)
                return true;

            const int64 new_alloc_count = power_to_2(size);

            if(!items)
            {
                items = hgl_align_malloc<T>(new_alloc_count);
                alloc_count = new_alloc_count;
            }
            else
            {
                // Trivial 类型可以安全使用 realloc
                items = hgl_align_realloc<T>(items, new_alloc_count);
                alloc_count = new_alloc_count;
            }

            return (items != nullptr);
        }

        // 优化的 Resize（不需要构造/析构）
        int64 Resize(int64 size)
        {
            Reserve(size);
            count = size;
            return count;
        }

        // 优化的 Free（不需要析构）
        void Free()
        {
            SAFE_FREE(items);
            count = 0;
            alloc_count = 0;
        }

        // 优化的 Clear（直接清零计数即可）
        void Clear()
        {
            count = 0;
        }

        // 优化的 Append（使用直接赋值）
        void Append(const T &obj)
        {
            if(count >= alloc_count)
                Reserve(count == 0 ? 8 : count * 2);  // 指数增长

            items[count] = obj;
            ++count;
        }

        // 优化的 Delete（直接使用 memcpy）
        bool Delete(int64 start, int64 delete_count = 1)
        {
            if(!items) return false;
            if(start >= count) return false;

            if(start < 0)
            {
                delete_count += start;
                start = 0;
            }

            if(start + delete_count > count)
                delete_count = count - start;

            if(delete_count <= 0) return false;

            const int64 end_count = count - (start + delete_count);

            if(end_count > 0)
            {
                if(end_count <= delete_count)
                    mem_copy<T>(items + start, items + start + delete_count, end_count);
                else
                    mem_copy<T>(items + start, items + (count - delete_count), delete_count);
            }

            count -= delete_count;
            return true;
        }

        // 优化的 DeleteShift（直接使用 memmove）
        bool DeleteShift(int64 start, int64 delete_count = 1)
        {
            if(!items) return false;
            if(start >= count) return false;

            if(start < 0)
            {
                delete_count += start;
                start = 0;
            }

            if(start + delete_count > count)
                delete_count = count - start;

            if(delete_count <= 0) return false;

            const int64 end_count = count - (start + delete_count);

            if(end_count > 0)
                mem_move<T>(items + start, items + start + delete_count, end_count);

            count -= delete_count;
            return true;
        }

        // 优化的 Insert（直接使用 memcpy/memmove）
        bool Insert(int64 pos, const T *data, const int64 data_number)
        {
            if(!data || data_number <= 0)
                return false;

            if(pos < 0) pos = 0;
            if(pos > count) pos = count;

            if(count + data_number > alloc_count)
            {
                int64 new_alloc_count = power_to_2(alloc_count + data_number);
                T *new_items = hgl_align_malloc<T>(new_alloc_count);

                if(pos > 0)       mem_copy(new_items, items, pos);
                                  mem_copy(new_items + pos, data, data_number);
                if(pos < count)   mem_copy(new_items + pos + data_number, items + pos, (count - pos));

                hgl_free(items);
                items = new_items;
                alloc_count = new_alloc_count;
            }
            else
            {
                // 空间足够，在原地插入
                if(pos < count)
                    mem_move(items + pos + data_number, items + pos, count - pos);

                mem_copy(items + pos, data, data_number);
            }

            count += data_number;
            return true;
        }

        bool Insert(int64 pos, const T &data)
        {
            return Insert(pos, &data, 1);
        }
    };
}//namespace hgl
