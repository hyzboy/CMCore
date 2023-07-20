#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/ArrayRearrangeHelper.h>
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

        const   bool    IsEmpty         ()const{count==0;}                                          ///<是否为空

        const   T *     GetData()const{return items;}
                T *     data()        {return items;}

    public:

                T *     begin       (){return items;}                                               ///<取得阵列起始地址指针
                T *     end         (){return items+count;}                                         ///<取得阵列结束地址指针

        const   T *     begin       ()const{return items;}                                          ///<取得阵列起始地址指针
        const   T *     end         ()const{return items+count;}                                    ///<取得阵列结束地址指针

    public:

        /**
         * 分配指定空间出来，供未来使用
         */
        void Alloc(int size)
        {
            if(size<=alloc_count)
                return;

            alloc_count=power_to_2(size);

            if(!items)
                items=hgl_align_malloc<T>(alloc_count);
            else
                items=hgl_align_realloc<T>(items,alloc_count);
        }

        /**
         * 设置阵列长度
         */
        void SetCount(int size)																	///<设置阵列长度(注：非字节数)
        {
            Alloc(size);

            count=size;
        }

        /**
         * 增加阵列长度
         */
        void AddCount(int size)
        {
            SetCount(count+size);
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

                    //int mid_start=move_number;
                    //int mid_count=count-mid_start;

                    //hgl_cpy(new_items,          items+mid_start,mid_count   );     //mid
                    //hgl_cpy(new_items+mid_count,items,          move_number );     //old

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

                    //int left_start=move_number;
                    //int left_count=new_index-left_start;

                    //int right_start=new_index;
                    //int right_count=count-right_start;

                    //hgl_cpy(new_items,                          items+left_start,   left_count);     //left
                    //hgl_cpy(new_items+left_count,               items,              move_number);    //old
                    //hgl_cpy(new_items+left_count+move_number,   right_start,        right_count);    //right

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

                    //hgl_cpy(new_items,              items+old_index,move_number);   //old
                    //hgl_cpy(new_items+move_number,  items,          old_index);     //left

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

                    //int mid_count=old_index-new_index;

                    //hgl_cpy(new_items,                      items,          new_index   );  //left
                    //hgl_cpy(new_items+new_index,            items+old_index,move_number );  //old
                    //hgl_cpy(new_items+new_index+move_number,items+new_index,mid_count   );  //mid

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
    };//template<typename T> class DataArray
}//namespace hgl
