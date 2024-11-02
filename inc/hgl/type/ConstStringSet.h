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
        const int GetLength()const{return length;}

        const int Comp(const ConstString &cs)const
        {
            if(length<cs.length)return(-1);
            if(length>cs.length)return( 1);

            return memcmp(str,cs.str,length);
        }

        CompOperator(const ConstString &,Comp)
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

        int Comp(const ConstStringView<SC> &csv)const
        {
            if(length!=csv.length)return(length-csv.length);

            return hgl::strcmp(GetString(),csv.GetString(),length);
        }

        CompOperator(const ConstStringView &,Comp)
    };

    template<typename SC> class ConstStringSet
    {
    private:

        DataArray<SC> str_data;                                 ///<字符串数据

        SortedSet<ConstStringView<SC>> str_set;                ///<字符串集合

        List<ConstStringView<SC>> str_list;                     ///<字符串列表
        Map<int,size_t> str_offset_map;                         ///<字符串映射

    public:

        const int GetCount      ()const{return str_list.GetCount();}                                    ///<取得字符串数量
        const int GetTotalLength()const{return str_data.GetCount();}                                    ///<取得字符串总长度
        const int GetTotalBytes ()const{return str_data.GetCount()*sizeof(SC);}                         ///<取得字符串总字节数

        const DataArray<SC> &GetStringData()const{return str_data;}                                     ///<取得字符串数据

        const List<ConstStringView<SC>> &GetConstStringList()const{return str_list;}                    ///<取得字符串列表

    public:

        const bool Contains(const SC *str,int length)const                          ///<判断字符串是否为合集成员
        {
            if(!str||!*str||length<=0)return(-1);

            ConstStringView<SC> csv;

            csv.length=length;
            csv.str=str;

            return(str_set.FindPos(csv)!=-1);
        }

        const int GetID(const SC *str,int length)const                              ///<取得字符串ID
        {
            if(!str||!*str||length<=0)return(-1);

            ConstStringView<SC> csv;

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
            return str_list.At(id);
        }

    public:

        ConstStringSet(){}

        virtual ~ConstStringSet()=default;

        int AddString(ConstStringView<SC> &csv,const SC *str,int length)            ///<添加一个字符串
        {
            if(!str||!*str||length<=0)
                return(-1);

            csv.id=GetID(str,length);

            if(csv.id>=0)
            {
                str_list.Get(csv.id,csv);

                return csv.id;
            }

            csv.str_data=&str_data;
            csv.id      =str_set.GetCount();
            csv.length  =length;
            csv.offset  =str_data.GetCount();

            {
                str_data.AddCount(length+1);
            
                SC *save_str=str_data.GetData()+csv.offset;

                hgl_cpy<SC>(save_str,str,length);

                save_str[length]=0;
            }

            str_set.Add(csv);

            str_list.Add(csv);
            str_offset_map.Add(csv.id,csv.offset);

            return csv.id;
        }
    };//class ConstStringSet

    using ConstWideStringSet    =ConstStringSet<wchar_t>;
    using ConstUTF8StringSet    =ConstStringSet<u8char>;
    using ConstUTF16StringSet   =ConstStringSet<u16char>;
//    using ConstUTF32StringSet   =ConstStringSet<u32char>;
    using ConstOSStringSet      =ConstStringSet<os_char>;

    template<typename SC> bool SaveToTextStream(hgl::io::TextOutputStream *tos,const ConstStringSet<SC> *css,bool output_id=false);
    template<typename SC> bool SaveToTextFile(const OSString &filename,const ConstStringSet<SC> *css,bool output_id=false,bool output_bom=true);

    //template<typename SC> bool SaveToDataStream(hgl::io::DataOutputStream *dos,const ConstStringSet<SC> *css);
}//namespace hgl
