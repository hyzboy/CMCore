#ifndef HGL_SAVE_STRING_LIST_INCLUDE
#define HGL_SAVE_STRING_LIST_INCLUDE

#include<hgl/type/StringList.h>

namespace hgl
{
    template<typename T,ByteOrderMask bom> struct WriteStringToDOS
    {
        bool WriteString(io::DataOutputStream *dos,const T &str);
    };

    template<typename T> struct WriteStringToDOS<T,ByteOrderMask::UTF8>
    {
        bool WriteString(io::DataOutputStream *dos,const T &str)
        {
            return dos->WriteUTF8String(str);
        }
    };

    template<typename T> struct WriteStringToDOS<T,ByteOrderMask::UTF16LE>
    {
        bool WriteString(io::DataOutputStream *dos,const T &str)
        {
            return dos->WriteUTF16LEString(str);
        }
    };

    template<typename T> struct WriteStringToDOS<T,ByteOrderMask::UTF16BE>
    {
        bool WriteString(io::DataOutputStream *dos,const T &str)
        {
            return dos->WriteUTF16BEString(str);
        }
    };

    template<typename T,ByteOrderMask bom> int WriteStringList(io::DataOutputStream *dos,const StringList<T> &sl)
    {
        WriteStringToDOS<T,bom> wtd;

        const int32 count=sl.GetCount();
        int result=0;

        if(!dos->WriteInt32(count))
            return(-2);

        for(int32 i=0;i<count;i++)
        {
            if(!wtd.WriteString(dos,sl[i]))
                return(-3);

            result++;
        }

        return(result);
    };

    template<typename T> int SaveUTF8StringList(io::DataOutputStream *dos,const StringList<T> &sl)
    {
        return WriteStringList<T,ByteOrderMask::UTF8>(dos,sl);
    }

    template<typename T> int SaveUTF16LEStringList(io::DataOutputStream *dos,const StringList<T> &sl)
    {
        return WriteStringList<T,ByteOrderMask::UTF16LE>(dos,sl);
    }

    template<typename T> int SaveUTF16BEStringList(io::DataOutputStream *dos,const StringList<T> &sl)
    {
        return WriteStringList<T,ByteOrderMask::UTF16BE>(dos,sl);
    }
}//namespace hgl
#endif//HGL_SAVE_STRING_LIST_INCLUDE