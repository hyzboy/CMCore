#pragma once

#include<hgl/type/SortedSet.h>
#include<hgl/type/Map.h>
#include<hgl/io/TextOutputStream.h>

namespace hgl
{
    template<typename SC> class ConstString
    {
        const SC *str;
        int length;

    public:

        ConstString(const SC *s,int l)
        {
            str=s;
            length=l;
        }

        const SC *c_str()const{return str;}

        const SC *GetString()const{return str;}
        int GetLength()const{return length;}

        std::strong_ordering operator<=>(const ConstString &cs)const
        {
            if(length != cs.length)
                return length <=> cs.length;

            return (hgl::strcmp(str, cs.str, length) <=> 0);
        }

        bool operator==(const ConstString &cs)const
        {
            return length == cs.length && hgl::strcmp(str, cs.str, length) == 0;
        }
    };

    template<typename SC> struct ConstStringView
    {
        DataArray<SC> *str_data;

        int id;             ///<顺序号
        int length;         ///<字符串长度

        union
        {
            size_t offset;      ///<字符串在整个数据中的偏移(id>=0插入合集时使用)
            const SC *str;      ///<字符串指针(id为-1,未插入合集时使用)
        };

    public:

        const SC *GetString()const
        {
            if(id==-1)
                return str;
            else
                return str_data->GetData()+offset;
        }

        const size_t GetLength()const
        {
            return length;
        }

        std::strong_ordering operator<=>(const ConstStringView<SC> &csv)const
        {
            if(length != csv.length)
                return length <=> csv.length;

            return (hgl::strcmp(GetString(), csv.GetString(), length) <=> 0);
        }

        bool operator==(const ConstStringView<SC> &csv)const
        {
            return length == csv.length && hgl::strcmp(GetString(), csv.GetString(), length) == 0;
        }
    };

    template<typename SC> class ConstStringSet
    {
    private:

        DataArray<SC> str_data;                                 ///<字符串数据

        SortedSet<ConstStringView<SC>> str_set;                 ///<字符串集合

        ArrayList<ConstStringView<SC>> str_list;               ///<字符串列表
        Map<int,size_t> str_offset_map;                         ///<字符串映射

    public:

        const int GetCount      ()const{return str_list.GetCount();}                                    ///<取得字符串数量
        const int GetTotalLength()const{return str_data.GetCount();}                                    ///<取得字符串总长度
        const int GetTotalBytes ()const{return str_data.GetCount()*sizeof(SC);}                         ///<取得字符串总字节数

        const DataArray<SC> &GetStringData()const{return str_data;}                                     ///<取得字符串数据

        const ArrayList<ConstStringView<SC>> &GetConstStringList()const{return str_list;}               ///<取得字符串列表

    public:

        const bool IsEmpty()const{return str_data.IsEmpty();}                                           ///<字符串集合是否为空

        ConstStringView<SC> * const *begin()const{return str_list.GetData();}                           ///<迭代器开始
        ConstStringView<SC> * const *end()const{return str_list.GetData()+str_list.GetCount();}        ///<迭代器结束

        const bool Contains(const SC *str,int length)const                          ///<判断字符串是否为合集成员
        {
            if(!str||!*str||length<=0)return false;

            ConstStringView<SC> csv;

            csv.str_data=nullptr;
            csv.id      =-1;
            csv.length  =length;
            csv.str     =str;

            return(str_set.Find(csv)!=-1);
        }

        const int GetID(const SC *str,int length)const                              ///<取得字符串ID
        {
            if(!str||!*str||length<=0)return(-1);

            ConstStringView<SC> csv;

            csv.str_data=nullptr;
            csv.id      =-1;
            csv.length  =length;
            csv.str     =str;

            int64 index=str_set.Find(csv);
            if(index>=0)
            {
                str_set.Get(index,csv);
                return csv.id;
            }

            return -1;
        }

        const SC *GetString(const int id)const                                      ///<根据ID取得字符串
        {
            if(id<0||id>=GetCount())return(nullptr);
            
            size_t offset;

            if(str_offset_map.Get(id,offset))
                return str_data.GetData()+offset;
            else
                return nullptr;
        }

        const ConstStringView<SC> *GetStringView(const int id)const                  ///<根据ID取得字符串视图
        {
            const ConstStringView<SC> * const *ptr = str_list.At(id);
            return ptr ? *ptr : nullptr;
        }

        const ConstStringView<SC> *operator[](const int id)const                  ///<根据ID取得字符串视图
        {
            const ConstStringView<SC> * const *ptr = str_list.At(id);
            return ptr ? *ptr : nullptr;
        }

    public:

        ConstStringSet(){}

        virtual ~ConstStringSet()=default;

        void Clear()
        {
            str_data.Clear();
            str_set.Clear();
            str_list.Clear();
            str_offset_map.Clear();
        }

        int AddString(ConstStringView<SC> &csv,const SC *str,int length)            ///<添加一个字符串
        {
            if(!str||!*str||length<=0)
                return(-1);

            csv.id=GetID(str,length);

            if(csv.id>=0)
            {
                ConstStringView<SC> *ptr;
                str_list.Get(csv.id, ptr);
                csv = *ptr;

                return csv.id;
            }

            csv.str_data=&str_data;
            csv.id      =str_set.GetCount();
            csv.length  =length;
            csv.offset  =str_data.GetCount();

            {
                str_data.Expand(length+1);
            
                SC *save_str=str_data.GetData()+csv.offset;

                mem_copy<SC>(save_str,str,length);

                save_str[length]=0;
            }

            str_set.Add(csv);

            ConstStringView<SC> *new_view = str_list.Create();
            new_view->str_data = csv.str_data;
            new_view->id = csv.id;
            new_view->length = csv.length;
            new_view->offset = csv.offset;
            
            str_offset_map.Add(csv.id,csv.offset);

            return csv.id;
        }
    };//class ConstStringSet

    using ConstAnsiStringSet  =ConstStringSet<char>;
    using ConstWideStringSet  =ConstStringSet<wchar_t>;
    using ConstU8StringSet    =ConstStringSet<u8char>;
    using ConstU16StringSet   =ConstStringSet<u16char>;
//    using ConstUTF32StringSet   =ConstStringSet<u32char>;
    using ConstOSStringSet      =ConstStringSet<os_char>;

    using ConstAnsiStringView   =ConstStringView<char>;
    using ConstWideStringView   =ConstStringView<wchar_t>;
    using ConstU8StringView     =ConstStringView<u8char>;
    using ConstU16StringView    =ConstStringView<u16char>;
    using ConstOSStringView     =ConstStringView<os_char>;

    template<typename SC> bool SaveToTextStream(hgl::io::TextOutputStream *tos,const ConstStringSet<SC> *css,bool output_id=false);
    template<typename SC> bool SaveToTextFile(const OSString &filename,const ConstStringSet<SC> *css,bool output_id=false,bool output_bom=true);

    //template<typename SC> bool SaveToDataStream(hgl::io::DataOutputStream *dos,const ConstStringSet<SC> *css);
}//namespace hgl
