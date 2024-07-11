#include<hgl/type/IDName.h>

namespace hgl
{
    template<typename SC>
    bool RegistryIDName(const size_t hash_code,ConstStringView<SC> &csv,const SC *name_string,const int name_length)
    {
        static ObjectMap<size_t,ConstStringSet<SC>> css_map;

        ConstStringSet<SC> *css;

        if(!css_map.Get(hash_code,css))
        {
            css=new ConstStringSet<SC>;
            css_map.Add(hash_code,css);
        }

        return(css->AddString(csv,name_string,name_length)>=0);
    }

#define REGISTRY_ID_NAME(type)  bool RegistryIDName_##type(const size_t hash_code,ConstStringView<type> &csv,const type *name,const int length){return RegistryIDName(hash_code,csv,name,length);}

    REGISTRY_ID_NAME(char)
    REGISTRY_ID_NAME(wchar_t)
    REGISTRY_ID_NAME(u8char)
    REGISTRY_ID_NAME(u16char)
    REGISTRY_ID_NAME(os_char)

#undef REGISTRY_ID_NAME
}//namespace hgl
