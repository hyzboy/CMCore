#pragma once

#include<hgl/type/DataArray.h>
namespace hgl
{
    /**
     * 有序合集</br>
     * 集合数据列表中不允许数据出现重复性，同时它会将数据排序</br>
     * 我们删除了批量添加数据的Add函数，因为每一次添加数据都会导致一次插入，这样效率太低了。我们建议将数据全部添加到List，再转换为SortedSets。</br>
     */
    template<typename T> class SortedSet
    {
    protected:

        DataArray<T> data_list;

        bool    FindPos(const T &flag,int64 &pos)const                                              ///<查找数据如果插入后，会所在的位置，返回是否存在这个数据
                {return FindInsertPositionInSortedArray(&pos,data_list,flag);}

        int64   FindPos(const T &flag)const{int64 pos;return FindPos(flag,pos)?pos:-1;}             ///<查找数据如果插入后，会所在的位置

    public: //属性

                T *     GetData         ()const{return data_list.GetData();}                    ///<取得数据指针
                int64   GetCount        ()const{return data_list.GetCount();}                   ///<取得数据总量
                int64   GetAllocCount   ()const{return data_list.GetAllocCount();}              ///<取得已分配空间数量

        const   bool    IsEmpty         ()const{return data_list.IsEmpty();}                    ///<确认列表是否为空

                T *     begin           ()const{return data_list.begin();}
                T *     end             ()const{return data_list.end();}
                T *     last            ()const{return data_list.last();}

    public:

        operator        DataArray<T> & ()       {return data_list;}                                 ///<取得原始数据阵列
        operator const  DataArray<T> & ()const  {return data_list;}                                 ///<取得原始数据阵列

    public:

        SortedSet()=default;
        virtual ~SortedSet()=default;

        void    SetCount        (int64 count){data_list.SetCount(count);}                           ///<指定数据数量，一般用于批量加载前的处理
        void    PreAlloc        (int64 count){data_list.Alloc(count);}                              ///<预分配指定数量的数据空间

        /**
         * 查找数据是否存在
         * @param flag 数据
         * @return 数据所在索引，-1表示不存在
         */
        int64   Find            (const T &flag)const
        {
            return FindDataPositionInSortedArray(data_list,flag);
        }

        bool    Contains        (const T &v)const{return(Find(v)!=-1);}                     ///<确认是否成员

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
                data_list.SetCount(1);

                data_list.WriteAt(data,0);

                return 0;
            }
            else
            {
                int64 pos;

                if(FindPos(data,pos))
                    return(-1);         //数据已存在

                data_list.Insert(pos,data);

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

            data_list.Alloc(data_list.GetCount()+count);

            {
                int64 pos;
                int64 result=0;

                for(int64 i=0;i<count;i++)
                {
                    if(FindPos(*dl,pos))
                        break;

                    data_list.Insert(pos,*dl);

                    ++dl;
                    ++result;
                }

                return result;
            }
        }

        bool    DeleteAt        (const int64 pos){return data_list.DeleteMove(pos,1);}      ///<删除一个数据,使用序号

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

        int64 Delete(const DataArray<T> &da)
        {
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

        bool    GetFirst        (T &data){return hgl_cpy(data,*begin());}                   ///<取得最前面一个数据
        bool    GetLast         (T &data){return hgl_cpy(data,*last());}                    ///<取得最后面一个数据

        void    operator       =(const SortedSet<T> &set){data_list=set.data_list;}        ///<等号操作符重载
    };//template<typename T> class SortedSet
}//namespace hgl
