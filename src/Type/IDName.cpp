#include<hgl/type/IDName.h>

namespace hgl
{
    template<typename SC>
    bool RegistryIDName(const char *tag,ConstStringView<SC> &csv,const SC *name_string,const int name_length)
    {
        static ObjectMap<AnsiString,ConstStringSet<SC>> css_map;

        ConstStringSet<SC> *css;

        const AnsiString IDNameTag=tag;

        if(!css_map.Get(IDNameTag,css))
        {
            css=new ConstStringSet<SC>;
            css_map.Add(IDNameTag,css);
        }

        return(css->AddString(csv,name_string,name_length)>=0);
    }

#define REGISTRY_ID_NAME(type)  bool RegistryIDName_##type(const char *tag,ConstStringView<type> &csv,const type *name,const int length){return RegistryIDName(tag,csv,name,length);}

    REGISTRY_ID_NAME(char)
    REGISTRY_ID_NAME(wchar_t)
    REGISTRY_ID_NAME(u8char)
    REGISTRY_ID_NAME(u16char)
    REGISTRY_ID_NAME(os_char)

#undef REGISTRY_ID_NAME
}//namespace hgl
