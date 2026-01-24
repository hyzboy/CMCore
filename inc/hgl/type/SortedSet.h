#pragma once

#include<hgl/type/ValueBuffer.h>
#include<hgl/type/PtrArray.h>
namespace hgl
{
    /**
     * 有序合集（平凡类型专用版本）</br>
     * 集合数据列表中不允许数据出现重复性，同时它会将数据排序</br>
     * 使用 ValueBuffer 和 memcpy/memmove 进行高效操作，仅支持平凡可复制类型</br>
     * <b>重要限制：</b>仅支持平凡可复制类型（trivially copyable types）。
     * 非平凡类型（包含虚函数、动态内存、自定义构造/析构等）请使用 SortedObjectSet</br>
     *
     * @tparam T 必须支持 operator< 用于排序，且必须是平凡可复制类型（trivially copyable）
     * @see SortedObjectSet 非平凡类型版本
     */
    template<typename T> class SortedSet
    {
    protected:

        static_assert(std::is_trivially_copyable_v<T>,
                      "SortedSet requires trivially copyable types. For non-trivial types (with custom copy/move semantics, virtual functions, dynamic memory), use SortedObjectSet instead.");

        ValueBuffer<T> data_list;

        bool    FindPos(const T &flag,int64 &pos)const                                              ///<查找数据如果插入后，会所在的位置，返回是否存在这个数据
                {return FindInsertPositionInSortedArray(&pos,data_list,flag);}

        int64   FindPos(const T &flag)const{int64 pos;return FindPos(flag,pos)?pos:-1;}             ///<查找数据如果插入后，会所在的位置

    public: //属性

                T *     GetData         ()      {return data_list.GetData();}                       ///<取得数据指针
        const   T *     GetData         ()const {return data_list.GetData();}                       ///<取得数据指针（只读）
                int64   GetCount        ()const {return data_list.GetCount();}                      ///<取得数据总量
                int64   GetAllocCount   ()const {return data_list.GetAllocCount();}                 ///<取得已分配空间数量

        const   bool    IsEmpty         ()const {return data_list.IsEmpty();}                       ///<确认列表是否为空

                T *     begin           ()      {return data_list.begin();}
                T *     end             ()      {return data_list.end();}
                T *     last            ()      {return data_list.last();}
        const   T *     begin           ()const {return data_list.begin();}
        const   T *     end             ()const {return data_list.end();}
        const   T *     last            ()const {return data_list.last();}

        operator        ValueBuffer<T> &  ()      {return data_list;}                                 ///<取得原始数据阵列
        operator const  ValueBuffer<T> &  ()const {return data_list;}                                 ///<取得原始数据阵列

    public:

        SortedSet()=default;
        virtual ~SortedSet()=default;

        void    Resize          (int64 count){data_list.Resize(count);}                         ///<指定数据数量，一般用于批量加载前的处理
        void    Reserve         (int64 count){data_list.Reserve(count);}                        ///<预分配指定数量的数据空间

        /**
         * 查找数据是否存在
         * @param flag 数据
         * @return 数据所在索引，-1表示不存在
         */
        int64   Find            (const T &flag)const
        {
            return FindDataPositionInSortedArray(data_list,flag);
        }

        bool    Contains        (const T &v)const{return(Find(v)!=-1);}                         ///<确认是否成员

        /**
        * 添加一个数据
        * @param data 数据
        * @return 插入的位置
        * @reutrn -1 数据已存在，添加失败
        */
        int64   Add             (const T &data)
        {
            if(data_list.GetCount()<=0)
            {
                data_list.Resize(1);

                data_list.WriteAt(data,0);

                return 0;
            }
            else
            {
                int64 pos;

                if(FindPos(data,pos))
                    return(-1);         //数据已存在

                // ValueBuffer::Insert expects a pointer and count
                data_list.Insert(pos,&data,1);

                return(pos);
            }
        }

        /*
         * 添加一批数据
         */
        int64   Add             (T *dl,const int64 count)
        {
            if(!dl||count<=0)return -1;

            if(count==1)
                return Add(*dl);

            data_list.Reserve(data_list.GetCount()+count);

            {
                int64 pos;
                int64 result=0;

                for(int64 i=0;i<count;i++)
                {
                    if(FindPos(*dl,pos))
                        break;

                    data_list.Insert(pos,dl,1);

                    ++dl;
                    ++result;
                }

                return result;
            }
        }

        bool    DeleteAt        (const int64 pos){return data_list.DeleteShift(pos,1);}      ///<删除一个数据,使用序号

        bool    Delete          (const T &data)                                             ///<删除一个数据
        {
            int64 pos=Find(data);

            if(pos==-1)return(false);

            return DeleteAt(pos);
        }

        /**
        * 删除一批数据
        * @param dp 数据指针
        * @param count 数据个数
        * @return 成功删除的数据个数
        */
        int64 Delete(const T *dp,const int64 count)
        {
            if(IsEmpty()||!dp||count<=0)
                return 0;

            int64 total=0;
            int64 pos;

            for(int64 i=0;i<count;i++)
            {
                pos=Find(*dp);
                if(pos!=-1)
                {
                    DeleteAt(pos);
                    ++total;
                }

                ++dp;
            }

            return total;
        }

        int64 Delete(const ValueBuffer<T> &da)
        {
            if(da.IsEmpty())
                return 0;

            return Delete(da.GetData(),da.GetCount());
        }

        void    Free            (){data_list.Free();}                                       ///<清除数据，并释放内存
        void    Clear           (){data_list.Clear();}                                      ///<清除数据，但不释放内存

        bool    Get             (const int64 index,T &data) const                           ///<根据序列号取得指定数据
        {
            if(index<0||index>=data_list.GetCount())
                return(false);

            data=*(data_list.GetData()+index);
            return(true);
        }

        bool    GetFirst        (T &data)const                                              ///<取得第一个数据
        {
            const T *ptr = begin();
            if(!ptr)
                return(false);

            data = *ptr;
            return(true);
        }

        bool    GetLast         (T &data)const                                              ///<取得最后面一个数据
        {
            const T *ptr = last();
            if(!ptr)
                return(false);

            data = *ptr;
            return(true);
        }

        void    operator       =(const SortedSet<T> &set){data_list=set.data_list;}        ///<等号操作符重载

        /**
         * @brief 相等比较运算符
         * CN: 比较两个已排序集合中的所有元素是否完全相同
         * EN: Check if two sorted sets have identical elements
         *
         * @param other 要比较的另一个已排序集合
         * @return 如果两个集合的元素相同，返回 true；否则返回 false
         */
        bool operator==(const SortedSet<T>& other) const
        {
            if (data_list.GetCount() != other.data_list.GetCount())
                return false;

            return hgl::mem_compare(data_list.GetData(), other.data_list.GetData(), data_list.GetCount()) == 0;
        }

        /**
         * @brief 不相等比较运算符
         * CN: 检查两个已排序集合中的元素是否不同
         * EN: Check if two sorted sets have different elements
         *
         * @param other 要比较的另一个已排序集合
         * @return 如果两个集合的元素不同，返回 true；否则返回 false
         */
        bool operator!=(const SortedSet<T>& other) const
        {
            return !(*this == other);
        }

    };//template<typename T> class SortedSet

    /**
     * 有序对象集合（非平凡类型专用版本）</br>
     * 集合数据列表中不允许数据出现重复性，同时它会将数据排序</br>
     * 本版本使用 PtrArray 支持非平凡类型（包含动态内存、自定义构造/析构等）</br>
     *
     * @tparam T 必须支持 operator< 用于排序，且必须是非平凡类型（non-trivial）
     */
    template<typename T> class SortedObjectSet
    {
    protected:

        static_assert(!std::is_trivially_copyable_v<T>,
                      "SortedObjectSet requires non-trivial types; use SortedSet for trivially copyable types.");

        PtrArray<T> data_list;

        bool    FindPos(const T &flag,int64 &pos)const
        {
            return FindInsertPositionInSortedArray(&pos,data_list,flag);
        }

        int64   FindPos(const T &flag)const
        {
            int64 pos;
            return FindPos(flag,pos)?pos:-1;
        }

    public: //属性

                T *     GetData         ()      {return data_list.GetData();}                    ///<取得数据指针
        const   T *     GetData         ()const{return data_list.GetData();}                    ///<取得数据指针（只读）
                int64   GetCount        ()const{return data_list.GetCount();}                   ///<取得数据总量
                int64   GetAllocCount   ()const{return data_list.GetAllocCount();}              ///<取得已分配空间数量

        const   bool    IsEmpty         ()const{return data_list.IsEmpty();}                    ///<确认列表是否为空

                T *     begin           ()     {return data_list.begin();}
                T *     end             ()     {return data_list.end();}
                T *     last            ()     {return data_list.last();}

        const   T *     begin           ()const{return data_list.begin();}
        const   T *     end             ()const{return data_list.end();}
        const   T *     last            ()const{return data_list.last();}

    public:

        operator        PtrArray<T> & ()       {return data_list;}                         ///<取得原始数据阵列
        operator const  PtrArray<T> & ()const  {return data_list;}                         ///<取得原始数据阵列

    public:

        SortedObjectSet()=default;
        virtual ~SortedObjectSet(){Clear();}

        void    Resize          (int64 count){data_list.Resize(count);}                        ///<指定数据数量，一般用于批量加载前的处理
        void    Reserve         (int64 count){data_list.Reserve(count);}                       ///<预分配指定数量的数据空间

        /**
         * 查找数据是否存在
         * @param flag 数据
         * @return 数据所在索引，-1表示不存在
         */
        int64   Find            (const T &flag)const
        {
            return FindDataPositionInSortedArray(data_list,flag);
        }

        bool    Contains        (const T &v)const{return(Find(v)!=-1);}                        ///<确认是否成员

        /**
        * 添加一个数据
        * @param data 数据
        * @return 插入的位置
        * @return -1 数据已存在，添加失败
        */
        int64   Add             (const T &data)
        {
            if(data_list.GetCount()<=0)
            {
                data_list.Resize(1);
                data_list.WriteAt(data,0);
                return 0;
            }
            else
            {
                int64 pos;

                if(FindPos(data,pos))
                    return(-1);         //数据已存在

                data_list.Insert(pos,&data,1);
                return(pos);
            }
        }

        /*
         * 添加一批数据
         */
        int64   Add             (T *dl,const int64 count)
        {
            if(!dl||count<=0)return -1;

            if(count==1)
                return Add(*dl);

            data_list.Reserve(data_list.GetCount()+count);

            {
                int64 pos;
                int64 result=0;

                for(int64 i=0;i<count;i++)
                {
                    if(FindPos(*dl,pos))
                        break;

                    data_list.Insert(pos,dl,1);

                    ++dl;
                    ++result;
                }

                return result;
            }
        }

        bool    DeleteAt        (const int64 pos){return data_list.DeleteShift(pos,1);}       ///<删除一个数据,使用序号

        bool    Delete          (const T &data)                                              ///<删除一个数据
        {
            int64 pos=Find(data);

            if(pos==-1)return(false);

            return DeleteAt(pos);
        }

        /**
        * 删除一批数据
        * @param dp 数据指针
        * @param count 数据个数
        * @return 成功删除的数据个数
        */
        int64 Delete(const T *dp,const int64 count)
        {
            if(IsEmpty()||!dp||count<=0)
                return 0;

            int64 total=0;
            int64 pos;

            for(int64 i=0;i<count;i++)
            {
                pos=Find(*dp);
                if(pos!=-1)
                {
                    DeleteAt(pos);
                    ++total;
                }

                ++dp;
            }

            return total;
        }

        int64 Delete(const PtrArray<T> &da)
        {
            if(da.IsEmpty())
                return 0;

            return Delete(da.GetData(),da.GetCount());
        }

        void    Free            (){data_list.Free();}                                        ///<清除数据，并释放内存
        void    Clear           (){data_list.Clear();}                                       ///<清除数据，但不释放内存

        bool    Get             (const int64 index,T &data) const                            ///<根据序列号取得指定数据
        {
            if(index<0||index>=data_list.GetCount())
                return(false);

            data=*(data_list.GetData()+index);
            return(true);
        }

        bool    GetFirst        (T &data)const                                               ///<取得第一个数据
        {
            const T *ptr = begin();
            if(!ptr)
                return(false);

            data = *ptr;
            return(true);
        }

        bool    GetLast         (T &data)const                                               ///<取得最后面一个数据
        {
            const T *ptr = last();
            if(!ptr)
                return(false);

            data = *ptr;
            return(true);
        }
    };//template<typename T> class SortedObjectSet
}//namespace hgl
