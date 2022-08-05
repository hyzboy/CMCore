#ifndef HGL_LOAD_STRING_LIST_INCLUDE
#define HGL_LOAD_STRING_LIST_INCLUDE

#include<hgl/type/StringList.h>

namespace hgl
{
    template<typename T,ByteOrderMask bom> struct ReadStringFromDIS
    {
        bool ReadString(io::DataInputStream *dis,T &str);
    };

    template<typename T> struct ReadStringFromDIS<T,ByteOrderMask::UTF8>
    {
        bool ReadString(io::DataInputStream *dis,T &str)
        {
            return dis->ReadUTF8String(str);
        }
    };

    template<typename T> struct ReadStringFromDIS<T,ByteOrderMask::UTF16LE>
    {
        bool ReadString(io::DataInputStream *dis,T &str)
        {
            return dis->ReadUTF16LEString(str);
        }
    };

    template<typename T> struct ReadStringFromDIS<T,ByteOrderMask::UTF16BE>
    {
        bool ReadString(io::DataInputStream *dis,T &str)
        {
            return dis->ReadUTF16BEString(str);
        }
    };

    /**
     * 从DataInputStream流中读取一个字符串列表
     * @param sl 字符串列表处理类
     * @param dis 数据输入流
     * @return 字符串行数
     */
    template<typename T,ByteOrderMask bom> int LoadStringList(StringList<T> &sl,io::DataInputStream *dis)
    {
        if(!dis)return(-1);

        int count;
        int result=0;

        if(!dis->ReadInt32(count))
            return(-2);

        ReadStringFromDIS<String<T>,bom> rsfd;

        String<T> str;

        for(int i=0;i<count;i++)
        {
            if(!rsfd.ReadString(dis,str))
                break;

            sl.Add(str);
            result++;
        }

        return(result);
    }//int LoadStringList

    inline int LoadUTF8StringList      (UTF8StringList &   sl,io::DataInputStream *dis){return LoadStringList<u8char,  ByteOrderMask::UTF8     >(sl,dis);}
    inline int LoadUTF16LEStringList   (UTF16StringList &  sl,io::DataInputStream *dis){return LoadStringList<u16char, ByteOrderMask::UTF16LE  >(sl,dis);}
    inline int LoadUTF16BEStringList   (UTF16StringList &  sl,io::DataInputStream *dis){return LoadStringList<u16char, ByteOrderMask::UTF16BE  >(sl,dis);}

    int LoadStringListFromText(UTF8StringList &sl,const void *data,const int size,const CharSet &cs);                   ///<加载一个原始文本块到UTF8StringList
    int LoadStringListFromText(UTF16StringList &sl,const void *data,const int size,const CharSet &cs);                  ///<加载一个原始文本块到UTF16StringList
    int LoadStringListFromTextFile(UTF8StringList &sl,const OSString &filename,const CharSet &cs);                      ///<加载一个原始文本文件到UTF8StringList
    int LoadStringListFromTextFile(UTF16StringList &sl,const OSString &filename,const CharSet &cs);                     ///<加载一个原始文本文件到UTF16StringList
}//namespace hgl
#endif//HGL_LOAD_STRING_LIST_INCLUDE