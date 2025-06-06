#pragma once

#include<hgl/type/ConstStringSet.h>
#include<hgl/type/SortedSet.h>

namespace hgl
{
    template<typename T,typename SC>
    bool RegistryIDName(ConstStringView<SC> &csv,const SC *name_string,const int name_length)
    {
        if(!T::id_name_set)
            T::id_name_set=new ConstStringSet<SC>;

        return(T::id_name_set->AddString(csv,name_string,name_length)>=0);
    }

    /**
    * 顺序ID+名称数据结构模板<br>
    * 按添加进来的名字先后顺序一个个产生连续的序号，所有数据只可读不可写
    */
    template<typename SC,typename MANAGER> class OrderedIDName:public Comparator<OrderedIDName<SC,MANAGER>>
    {   
    public:

        using SelfClass=OrderedIDName<SC,MANAGER>;

    protected:

        ConstStringView<SC> csv;

    protected:

        void Clear()
        {
            csv.id=-1;
            csv.length=-1;
            csv.str=nullptr;
        }

        void Update(const SC *name_string,const int name_length)
        {
            if(name_length<=0)
            {
                Clear();
                return;
            }

            RegistryIDName<MANAGER,SC>(csv,name_string,name_length);
        }

    public:

        static  const size_t    GetClassHC      ()                                                      ///<获取类HashCode
        {
            static SelfClass self;
            static size_t class_hc=typeid(self).hash_code();

            return class_hc;
        }          

                const int       GetID           ()const{return csv.id;}                                 ///<获取名称ID
                const SC *      GetName         ()const{return csv.GetString();}                        ///<获取名称字符串
                const int       GetNameLength   ()const{return csv.length;}                             ///<获取名称字符串长度

                const SC *      ToString        ()const{return csv.GetString();}                        ///<获取名称字符串

    public:

        OrderedIDName()
        {
            Clear();
        }

        OrderedIDName(const SC *name_string)
        {
            Update(name_string,hgl::strlen(name_string));
        }

        void operator = (const SC *name_string)
        {
            Update(name_string,hgl::strlen(name_string));
        }

        void operator = (const String<SC> &name_string)
        {
            Update(name_string.c_str(),name_string.Length());
        }

        void operator = (const SelfClass &id_name)
        {
            hgl_cpy(csv,id_name.csv);
        }

    public:

        const int compare(const OrderedIDName &oin)const override{return GetID()-oin.GetID();}
    };//class IDName

#define HGL_DEFINE_IDNAME(name,char_type)   struct IDName##_##name##_Manager{static ConstStringSet<char_type> *id_name_set;}; \
                                            ConstStringSet<char_type> *IDName##_##name##_Manager::id_name_set=nullptr; \
                                            using name=OrderedIDName<char_type,IDName##_##name##_Manager>;   \
                                            using name##Set=SortedSet<name>;

    HGL_DEFINE_IDNAME(AIDName,   char)
    HGL_DEFINE_IDNAME(WIDName,   wchar_t)
    HGL_DEFINE_IDNAME(U8IDName,  u8char)
    HGL_DEFINE_IDNAME(U16IDName, u16char)
    HGL_DEFINE_IDNAME(OSIDName,  os_char)
}//namespace hgl
