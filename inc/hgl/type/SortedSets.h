#ifndef HGL_TYPE_SORTED_SETS_INCLUDE
#define HGL_TYPE_SORTED_SETS_INCLUDE

#include<hgl/type/List.h>
namespace hgl
{
    /**
     * 有序合集</br>
     * 集合数据列表中不允许数据出现重复性，同时它会将数据排序
     */
    template<typename T> class SortedSets
    {
    protected:

        List<T> data_list;

        bool    FindPos(const T &,int &)const;                                                      ///<查找数据如果插入后，会所在的位置，返回是否存在这个数据
        int     FindPos(const T &flag)const{int pos;FindPos(flag,pos);return(pos);}                 ///<查找数据如果插入后，会所在的位置

    public: //属性

                List<T> &   GetList     ()const{return data_list;}                                  ///<取得数据原始列表
                T *         GetData     ()const{return data_list.GetData();}                        ///<取得数据指针
                int         GetCount    ()const{return data_list.GetCount();}                       ///<取得数据总量

                T *         begin       ()const{return data_list.begin();}
                T *         end         ()const{return data_list.end();}

    public:

        SortedSets()=default;
        virtual ~SortedSets()=default;

                void    SetCount        (int count){data_list.SetCount(count);}                     ///<指定数据数量，一般用于批量加载前的处理
                void    PreMalloc       (int count){data_list.PreMalloc(count);}                    ///<预分配指定数量的数据空间

        const   int     Find            (const T &)const;                                           ///<查找数据位置,不存在返回-1
        const   bool    IsMember        (const T &v)const{return(Find(v)!=-1);}                     ///<确认是否成员
                int     Add             (const T &);                                                ///<添加一个数据,返回索引号,返回-1表示数据已存在
                int     Add             (const T *,const int);                                      ///<添加一批数据
                int     Add             (const SortedSets<T> &s){return Add(s.GetData(),s.GetCount());}    ///<添加一批数据
                bool    Update          (const T &);                                                ///<更新一个数据
                bool    Delete          (const T &);                                                ///<删除一个数据
                int     Delete          (T *,const int);                                            ///<删除一批数据
                bool    DeleteBySerial  (int);                                                      ///<删除一个数据,使用序号
                void    Clear           ();                                                         ///<清除数据
                void    ClearData       ();                                                         ///<清除数据，但不释放内存
                void    DeleteClear     (){data_list.DeleteClear();}                                ///<清除所有数据并全部调用delete

                bool    Get             (const int index,T &data)                                   ///<根据序列号取得指定数据
                {
                    if(index<0||index>=data_list.GetCount())
                        return(false);

                    data=*(data_list.GetData()+index);
                    return(true);
                }

                bool    GetBegin        (T &data){return data_list.Begin(data);}                    ///<取得最前面一个数据
                bool    GetEnd          (T &data){return data_list.End(data);}                      ///<取得最后面一个数据

                int     Intersection    (SortedSets<T> &result,const SortedSets<T> &set);           ///<取得与指定合集的交集
                int     Intersection    (const SortedSets<T> &set);                                 ///<取得与指定合集的交集数量

                /**
                 * 取得与指定交集is的合集，但排斥cs合集中的数据
                 * @param result 结果合集
                 * @param is 求交集的合集
                 * @param cs 求排斥的合集
                 * @return 结果数量
                 */
                int     Intersection    (SortedSets<T> &result,const SortedSets<T> &is,const SortedSets<T> &cs);

                int     Difference      (const SortedSets<T> &is);                                  ///<求差集数量

                void    operator       =(const SortedSets<T> &set){data_list=set.data_list;}        ///<等号操作符重载

                bool    Rand            (T &)const;                                                 ///<随机取得一个

        virtual void    Enum            (void (*enum_func)(T &)){data_list.Enum(enum_func);}        ///<枚举所有数据成员
    };//template<typename T> class SortedSets
}//namespace hgl
#include<hgl/type/SortedSets.cpp>
#endif//HGL_TYPE_SORTED_SETS_INCLUDE
