#pragma once

#include<hgl/type/DataArray.h>
namespace hgl
{
    /**
     * 有序合集</br>
     * 集合数据列表中不允许数据出现重复性，同时它会将数据排序</br>
     * 我们删除了批量添加数据的Add函数，因为每一次添加数据都会导致一次插入，这样效率太低了。我们建议将数据全部添加到List，再转换为SortedSets。</br>
     */
    template<typename T> class SortedSets
    {
    protected:

        DataArray<T> data_list;

        bool    FindPos(const T &flag,int &pos)const                                                ///<查找数据如果插入后，会所在的位置，返回是否存在这个数据
                {return FindInsertPositionInSortedArray(&pos,data_list,flag);}

        int     FindPos(const T &flag)const{int pos;return FindPos(flag,pos)?pos:-1;}               ///<查找数据如果插入后，会所在的位置

    public: //属性

                T *             GetData     ()const{return data_list.GetData();}                    ///<取得数据指针
                int             GetCount    ()const{return data_list.GetCount();}                   ///<取得数据总量

        const   bool            IsEmpty     ()const{return data_list.IsEmpty();}                    ///<确认列表是否为空

                T *             begin       ()const{return data_list.begin();}
                T *             end         ()const{return data_list.end();}
                T *             last        ()const{return data_list.last();}

    public:

        operator        DataArray<T> & ()       {return data_list;}                                 ///<取得原始数据阵列
        operator const  DataArray<T> & ()const  {return data_list;}                                 ///<取得原始数据阵列

    public:

        SortedSets()=default;
        virtual ~SortedSets()=default;

        void    SetCount        (int count){data_list.SetCount(count);}                             ///<指定数据数量，一般用于批量加载前的处理
        void    PreAlloc        (int count){data_list.Alloc(count);}                                ///<预分配指定数量的数据空间

        /**
         * 查找数据是否存在
         * @param flag 数据
         * @return 数据所在索引，-1表示不存在
         */
        int     Find            (const T &flag)const
        {
            return FindDataPositionInSortedArray(data_list,flag);
        }

        bool    IsMember        (const T &v)const{return(Find(v)!=-1);}                     ///<确认是否成员

        /**
        * 添加一个数据
        * @param data 数据
        * @return 插入的位置
        * @reutrn -1 数据已存在，添加失败
        */
        int     Add             (const T &data)
        {
            if(data_list.GetCount()<=0)
            {
                data_list.SetCount(1);

                data_list.WriteAt(data,0);

                return 0;
            }
            else
            {
                int pos;

                if(FindPos(data,pos))
                    return(-1);         //数据已存在

                data_list.Insert(pos,data);

                return(pos);
            }
        }

        /*
         * 添加一批数据
         */
        int     Add             (T *dl,const int count)
        {
            if(!dl||count<=0)return -1;

            if(count==1)
                return Add(*dl);

            data_list.Alloc(data_list.GetCount()+count);

            {
                int pos;
                int result=0;

                for(int i=0;i<count;i++)
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

        bool    DeleteAt        (const int pos){return data_list.DeleteMove(pos,1);}        ///<删除一个数据,使用序号

        bool    Delete          (const T &data)                                             ///<删除一个数据
        {
            int pos=Find(data);

            if(pos==-1)return(false);

            return DeleteAt(pos);
        }

        /**
        * 删除一批数据
        * @param dp 数据指针
        * @param count 数据个数
        * @return 成功删除的数据个数
        */
        int Delete(T *dp,const int count)
        {
            int total=0;
            int pos;

            for(int i=0;i<count;i++)
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

        void    Free            (){data_list.Free();}                                       ///<清除数据，并释放内存
        void    Clear           (){data_list.Clear();}                                      ///<清除数据，但不释放内存

        bool    Get             (const int index,T &data)                                   ///<根据序列号取得指定数据
        {
            if(index<0||index>=data_list.GetCount())
                return(false);

            data=*(data_list.GetData()+index);
            return(true);
        }

        bool    GetFirst        (T &data){return hgl_cpy(data,*begin());}                   ///<取得最前面一个数据
        bool    GetLast         (T &data){return hgl_cpy(data,*last());}                    ///<取得最后面一个数据

        void    operator       =(const SortedSets<T> &set){data_list=set.data_list;}        ///<等号操作符重载
    };//template<typename T> class SortedSets
}//namespace hgl
