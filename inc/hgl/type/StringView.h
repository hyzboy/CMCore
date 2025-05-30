#pragma once

#include<hgl/type/StrChar.h>
#include<hgl/Comparator.h>

namespace hgl
{
    template<typename T> class StringInstance;
    template<typename T> class String;

    template<typename T> class StringView:public Comparator<StringView<T>>
    {
        T *str_data;
        int32 str_length;

    public:

        StringView()
        {
            str_data=nullptr;
            str_length=0;
        }

        StringView(const T *str,const int len)
        {
            str_data=(T *)str;
            str_length=len;
        }

        StringView(const T *str)
        {
            str_data=(T *)str;
            str_length=0;
            if(str_data)
                str_length=hgl::strlen(str_data);
        }

        StringView(const StringView<T> &str)
        {
            str_data=str.str_data;
            str_length=str.str_length;
        }

        StringView(const StringInstance<T> &si)=delete;
        StringView(const String<T> &s)=delete;

        ~StringView()=default;

        operator const T *()const{return str_data;}

        const T *c_str()const{return str_data;}
        const int32 length()const{return str_length;}
        const int32 GetLength()const{return str_length;}

        const bool IsEmpty()const{return str_data&&str_length>0;}
        
        const int compare(const StringView<T> &other)const override
        {
            return hgl::strcmp(str_data,str_length,other.str_data,other.str_length);
        }
    };//class StringView

    using U8StringView  =StringView<u8char>;
    using U16StringView =StringView<u16char>;
    using U32StringView =StringView<u32char>;
    
    using OSStringView  =StringView<os_char>;
    using AnsiStringView=StringView<char>;
    using WStringView   =StringView<wchar_t>;
}//namespace hgl
