#include<hgl/type/LoadStringList.h>
#include<hgl/type/LoadString.h>
#include<hgl/type/SplitString.h>
#include<hgl/filesystem/FileSystem.h>

namespace hgl
{
    /**
     * 加载一个原始文本块到UTF8StringList
     */
    int LoadStringListFromText(UTF8StringList &sl,const void *data,const int size,const CharSet &cs)
    {
        UTF8String str;

        LoadStringFromText(str,data,size,cs);

        return SplitToStringListByEnter<u8char>(sl,str);
    }

    /**
     * 加载一个原始文本块到UTF16StringList
     */
    int LoadStringListFromText(UTF16StringList &sl,const void *data,const int size,const CharSet &cs)
    {
        UTF16String str;

        LoadStringFromText(str,data,size,cs);

        return SplitToStringListByEnter<u16char>(sl,str);
    }

    /**
     * 加载一个原始文本文件到UTF8StringList
     */
    int LoadStringListFromTextFile(UTF8StringList &sl,const OSString &filename,const CharSet &cs)
    {
        uchar *data;

        const int size=filesystem::LoadFileToMemory(filename,(void **)&data);

        if(size<=0)
            return size;

        return LoadStringListFromText(sl,data,size,cs);
    }

    /**
     * 加载一个原始文本文件到UTF16StringList
     */
    int LoadStringListFromTextFile(UTF16StringList &sl,const OSString &filename,const CharSet &cs)
    {
        uchar *data;

        const int size=filesystem::LoadFileToMemory(filename,(void **)&data);

        if(size<=0)
            return size;

        return LoadStringListFromText(sl,data,size,cs);
    }
}//namespace hgl
