#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ArrayRearrangeHelper.h>
#include<hgl/type/ArrayItemProcess.h>
namespace hgl
{
    /**
     * 数据阵列
     */
    template<typename T> class DataArray
    {
    protected:

        T *items;
        int count;                                                                                  ///<当前数据数量
        int alloc_count;                                                                            ///<已分配的数据数量

    public:

                int     GetCount	    ()const{return count;}										///<取得数据数量(注：非字节数)
        const	int     GetAllocCount   ()const{return alloc_count;}								///<取得已分配的阵列大小(注：非字节数)
        const	int     GetBytes		()const{return count*sizeof(T);}							///<取得阵列已使用的字节数
        const	int     GetAllocBytes   ()const{return alloc_count*sizeof(T);}						///<取得阵列已分配空间字节数

        const   bool    IsEmpty         ()const{return(count==0);}                                  ///<是否为空

                T *     GetData         ()const{return items;}
                T *     data            ()const{return items;}

    public:

                T *     begin           ()const{return items;}                                      ///<取得阵列起始地址指针
                T *     end             ()const{return items+count;}                                ///<取得阵列结束地址指针
                T *     last            ()const{return items+(count-1);}                            ///<取得阵列最后一个数据地址指针

    public:

        /**
         * 分配指定空间出来，供未来使用
         */
        bool Alloc(int size)
        {
            if(size<=alloc_count)
                return(true);

            alloc_count=power_to_2(size);

            if(!items)
                items=hgl_align_malloc<T>(alloc_count);
            else
                items=hgl_align_realloc<T>(items,alloc_count);

            return(items!=nullptr);
        }

        /**
         * 设置阵列长度
         * @param size 阵列长度
         * @return 设置成功后的阵列长度
         */
        int SetCount(int size)																	///<设置阵列长度(注：非字节数)
        {
            Alloc(size);

            count=size;
            return count;
        }

        /**
         * 增加阵列长度
         * @param size 增加的长度
         * @return 增加后的阵列长度
         */
        int AddCount(int size)
        {
            return SetCount(count+size);
        }

    public:

        DataArray()
        {
            items=nullptr;
            count=0;
            alloc_count=0;
        }

        DataArray(int size)
        {
            if(size<=0)
                items=nullptr;
            else
                items=(T *)hgl_malloc(size*sizeof(T));

            if(items)
            {
                count=size;
                alloc_count=size;
            }
            else
            {
                count=0;
                alloc_count=0;
            }
        }

        virtual ~DataArray()
        {
            Clear();
        }

        void Clear()
        {
            SAFE_FREE(items);

            count=0;
            alloc_count=0;
        }

        void ClearData()
        {
            count=0;
        }

        void Zero()
        {
            if(items)
                hgl_zero(items,alloc_count);
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

        T &operator[](int n)
        {
            return items[n];
        }

        const T &operator[](int n)const
        {
            return items[n];
        }

        T *GetPointer(int n)const
        {
            return (n<0||n>=count)?nullptr:items+n;
        }

        const bool ReadAt(T &obj,int const index)const
        {
            if(index<0||index>=count)return(false);

            hgl_cpy(obj,items[index]);
            return(true);
        }

        const bool ReadAt(T *obj,int const start,const int num)const
        {
            if(!obj||start<0||start+num>count)return(false);

            hgl_cpy(obj,items+start,num);
            return(true);
        }

        const bool WriteAt(const T &obj,const int index)
        {
            if(index<0||index>=count)return(false);

            hgl_cpy(items[index],obj);
            return(true);
        }

        const bool WriteAt(const T *obj,const int start,const int num)
        {
            if(!obj||start<0||start+num>count)return(false);

            hgl_cpy(items+start,obj,num);
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

            SetCount(da.GetCount());

            hgl_cpy<T>(items,da.items,count);
        }

        void operator = (const std::initializer_list<T> &l)
        {
            SetCount((int)l.size());

            hgl_cpy<T>(items,l.begin(),count);
        }

        /**
        * 删除列表中的指定项(不关心顺序，如果删除中间的数据，可能会将最后面的数据拿过来填补)
        * @param start 要删除的数据项的索引起始值
        * @param delete_count 要删除的数据项数量
        * @return 是否成功
        */
        bool Delete(int start,int delete_count=1)
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

                    hgl_cpy<T>(items+start,items+start+delete_count,end_count);
                }
                else                                                        //后面的数据个数比删除的数据个数多，那就只移动等长的最后一段数据
                {
                    //[---][**********][***]
                    //  ^                v
                    //  ^                v
                    //  +<<<<<<<<<<<<<<<<+

                    hgl_cpy<T>(items+start,items+(count-delete_count),delete_count);
                }
            }
            //else{后面都没数据了，那就啥都不用干了}

            count-=delete_count;

            return(true);
        }

        /**
        * 删除列表中的指定项(关心顺序，删除中间数据后，会将后面的数据整体前移)
        * @param start 要删除的数据项的索引值
        * @return 是否成功
        */
        bool DeleteMove(int start,int delete_count=1)
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

            const int end_count=count-(start+delete_count);

            if(end_count>0)
                hgl_cpy<T>(items+start,items+start+delete_count,end_count);

            count-=delete_count;

            return(true);
        }

        /**
        * 交换列表中两个数据的位置
        * @param a 第一个数据的位置
        * @param b 第二个数据的位置
        */
        void Exchange(int a,int b)
        {
            hgl_swap(items[a],items[b]);
        }

        /**
        * 移动数据一定量的数据到新的位置
        * @param new_index 新的位置
        * @param old_index 旧的位置
        * @param move_number 要移动的数据个数
        */
        bool Move(int new_index,int old_index,int move_number=1)
        {
            if(!items)return(false);
            if(new_index==old_index)return(false);
            if(old_index<0||old_index>=count)return(false);

            if(new_index<0)new_index=0;
            if(new_index>=count)new_index=count;

            if(old_index+move_number>count)
                move_number=count-move_number;

            if(move_number<=0)return(false);

            //直接创建新缓冲区复制过去，放弃旧缓冲区
            const int new_alloc_count=power_to_2(count);
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
                hgl_free(items);
                items=new_items;
                alloc_count=new_alloc_count;

                return(true);
            }
            else
            {
                hgl_free(new_items);
                return(false);
            }
        }

        /**
         * 查找数据在数组中的位置
         * @param data 要查找的数据
         * @return <0 未找到或其它错误
         */
        const int Find(const T &data)const
        {
            return FindDataPositionInArray<T>(items,count,data);
        }

        /**
         * 在指定位置插入数据
         */
        bool Insert(int pos,const T *data,const int data_number)
        {
            if(!data||data_number<=0)
                return(false);

            if(pos<0)pos=0;
            if(pos>count)pos=count;

            if(count+data_number>alloc_count)
            {
                int new_alloc_count=alloc_count+data_number;

                T *new_items=hgl_align_malloc<T>(new_alloc_count);

                hgl_cpy(new_items,items,pos);
                hgl_cpy(new_items+pos,data,data_number);
                hgl_cpy(new_items+pos+data_number,items+pos,(count-pos));

                hgl_free(items);
                items=new_items;
                alloc_count=new_alloc_count;
            }
            else
            {
                if(pos<count)
                    hgl_move(items+pos+data_number,items+pos,count-pos);

                hgl_cpy(items+pos,data,data_number);
            }

            count+=data_number;

            return(true);
        }

        bool Insert(int pos,const T &data)
        {
            return Insert(pos,&data,1);
        }

        /**
            * 统计出不在without_list中的数据，产生的结果写入result_list
            */
        void WithoutList(DataArray<T> &result_list,const DataArray<T> &without_list)
        {
            result_list.ClearData();
            const int count=this->GetCount();

            if(count<=0)return;

            result_list.ClearData();
            result_list.PreAlloc(count);

            int result=0;

            T *p=result_list.items;

            for(const T *sp:*this)
            {
                if(!without_list.IsExist(*sp))
                {
                    *p=*sp;
                    ++p;
                    ++result;
                }
            }

            result_list.SetCount(result);
        }

        //int     Intersection    (SortedSets<T> &result,const SortedSets<T> &sets);          ///<取得与指定合集的交集
        //int     Intersection    (const SortedSets<T> &set);                                 ///<取得与指定合集的交集数量

        ///**
        //    * 取得与指定交集is的合集，但排斥cs合集中的数据
        //    * @param result 结果合集
        //    * @param is 求交集的合集
        //    * @param cs 求排斥的合集
        //    * @return 结果数量
        //    */
        //int     Intersection    (SortedSets<T> &result,const SortedSets<T> &is,const SortedSets<T> &cs);

        //int     Difference      (const SortedSets<T> &is);                                  ///<求差集数量

        ///**
        // * 求当前合集与另一个数据集的交集
        // * @param result 结果存放合集
        // * @param list 要计算交集的数据集
        // * @return 交集数量
        // */
        //int Intersection(SortedSets<T> &result,const SortedSets<T> &list)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(0);

        //    if(list.GetCount()<=0)
        //        return(0);

        //    data_list.Enum([&](T &obj)
        //    {
        //        if(list->IsMember(obj))
        //            result.Add(obj);
        //    });

        //    return result.GetCount();
        //}

        //int Intersection(const SortedSets<T> &list)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(0);

        //    if(list.GetCount()<=0)
        //        return(0);

        //    int count=0;

        //    T *obj=data_list.GetData();
        //    for(int i=0;i<data_list.GetCount();i++)
        //    {
        //        if(list.IsMember(*obj))
        //            ++count;

        //        ++obj;
        //    }

        //    return count;
        //}

        //int Intersection(SortedSets<T> &result,const SortedSets<T> &il,const SortedSets<T> &cl)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(0);

        //    if(il.GetCount()<=0)
        //        return(0);

        //    T *obj=data_list.GetData();
        //    for(int i=0;i<data_list.GetCount();i++)
        //    {
        //        if(il.IsMember(*obj))
        //            if(!cl.IsMember(*obj))
        //                result.Add(*obj);
        //        ++obj;
        //    }

        //    return result.GetCount();
        //}

        //int Difference(const DataArray &is)
        //{
        //    if(data_list.GetCount()<=0)
        //        return(is.GetCount());

        //    if(is.GetCount()<=0)
        //        return(data_list.GetCount());

        //    int count=0;

        //    T *obj=data_list.GetData();
        //    for(int i=0;i<data_list.GetCount();i++)
        //    {
        //        if(!is.IsMember(*obj))
        //            ++count;

        //        ++obj;
        //    }

        //    return count;
        //}

        //int     Clear           (const SortedSets<T> &clear_sets);                          ///<清除指定合集中所有数据
    };//template<typename T> class DataArray
}//namespace hgl
