#include<hgl/io/LoadStringList.h>
#include<hgl/io/LoadString.h>
#include<hgl/type/SplitString.h>
#include<hgl/filesystem/FileSystem.h>

namespace hgl
{
    /**
     * 加载一个原始文本块到U8StringList
     */
    int LoadStringListFromText(U8StringList &sl,const void *data,const int size,const CharSet &cs)
    {
        U8String str;

        LoadStringFromText(str,data,size,cs);

        return SplitToStringListByEnter<u8char>(sl,str);
    }

    /**
     * 加载一个原始文本块到U16StringList
     */
    int LoadStringListFromText(U16StringList &sl,const void *data,const int size,const CharSet &cs)
    {
        U16String str;

        LoadStringFromText(str,data,size,cs);

        return SplitToStringListByEnter<u16char>(sl,str);
    }

    /**
     * 加载一个原始文本文件到U8StringList
     */
    int LoadStringListFromTextFile(U8StringList &sl,const OSString &filename,const CharSet &cs)
    {
        uchar *data;

        const int size=filesystem::LoadFileToMemory(filename,(void **)&data);

        if(size<=0)
            return size;

        return LoadStringListFromText(sl,data,size,cs);
    }

    /**
     * 加载一个原始文本文件到U16StringList
     */
    int LoadStringListFromTextFile(U16StringList &sl,const OSString &filename,const CharSet &cs)
    {
        uchar *data;

        const int size=filesystem::LoadFileToMemory(filename,(void **)&data);

        if(size<=0)
            return size;

        return LoadStringListFromText(sl,data,size,cs);
    }
}//namespace hgl
