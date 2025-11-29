#include<hgl/type/StringViewList.h>
#include<hgl/io/LoadString.h>
#include<hgl/filesystem/FileSystem.h>

namespace hgl
{
    int LoadStringViewListFromText(U8StringViewList& sl, const void* data, const int size, const CharSet& cs)
    {
        if(!data||size<=0)
            return 0;

        U8String str;

        if (LoadStringFromText(str, data, size, cs) <= 0)
            return 0;

        sl.Clear();

        sl = U8StringViewList(str);
        return sl.GetCount();
    }

    int LoadStringViewListFromText(U16StringViewList& sl, const void* data, const int size, const CharSet& cs)
    {
        if(!data||size<=0)
            return 0;

        U16String str;

        if (LoadStringFromText(str, data, size, cs) <= 0)
            return 0;

        sl.FromString(str);
        
        return sl.GetCount();
    }

    int LoadStringViewListFromTextFile(U8StringViewList& sl, const OSString& filename, const CharSet& cs)
    {
        uchar* data;
        const int size = filesystem::LoadFileToMemory(filename, (void**)&data);

        if (size <= 0)
            return size;

        return LoadStringViewListFromText(sl, data, size, cs);
    }

    int LoadStringViewListFromTextFile(U16StringViewList& sl, const OSString& filename, const CharSet& cs)
    {
        uchar* data;

        const int size = filesystem::LoadFileToMemory(filename, (void**)&data);

        if (size <= 0)
            return size;

        return LoadStringViewListFromText(sl, data, size, cs);
    }
}
