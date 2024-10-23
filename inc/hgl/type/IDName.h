#pragma once

#include<hgl/type/ConstStringSet.h>
#include<typeinfo>

namespace hgl
{
    template<typename SC>
    bool RegistryIDName(const size_t hash_code,ConstStringView<SC> &csv,const SC *name_string,const int name_length);

    /**
    * 顺序ID+名称数据结构模板<br>
    * 按添加进来的名字先后顺序一个个产生连续的序号，所有数据只可读不可写
    */
    template<typename SC,int CLASS_COUNTER> class OrderedIDName
    {
    public:

        using SelfClass=OrderedIDName<SC,CLASS_COUNTER>;

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

            RegistryIDName<SC>(typeid(*this).hash_code(),csv,name_string,name_length);
        }

    public:

        static  const size_t    GetClassID      ()                                                      ///<获取类ID
        {
            static SelfClass self;
            static size_t class_id=typeid(self).hash_code();

            return class_id;
        }          

                const int       GetID           ()const{return csv.id;}                                 ///<获取名称ID
                const SC *      GetName         ()const{return csv.GetString();}                        ///<获取名称字符串
                const int       GetNameLength   ()const{return csv.length;}                             ///<获取名称字符串长度

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

        const int Comp(const OrderedIDName &oin)const{return GetID()-oin.GetID();}

        const int Comp(const OrderedIDName *oin)const{return GetID()-oin->GetID();}

        CompOperator(const OrderedIDName &,Comp)
        CompOperator(const OrderedIDName *,Comp)
    };//class IDName

#define DefineIDName(name,type) using name=OrderedIDName<type,__COUNTER__>; //使用__COUNTER__是为了让typeid()不同
    
    DefineIDName(AnsiIDName,    char)
    DefineIDName(WideIDName,    wchar_t)
    DefineIDName(UTF8IDName,    u8char)
    DefineIDName(UTF16IDName,   u16char)
    DefineIDName(OSIDName,      os_char)
}//namespace hgl
