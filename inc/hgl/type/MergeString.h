#ifndef HGL_MERGE_STRING_INCLUDE
#define HGL_MERGE_STRING_INCLUDE

#include<hgl/type/StringList.h>
namespace hgl
{
    /**
     * 将一个字符串列表整合成一个字符串
     * @param sl 要整合的字符串列表
     * @param end_line 换行符
     * @return 字符串
     */
    template<typename T> String<T> MergeString(const StringList<T> &sl,const String<T> &end_line)
    {
        int total_chars=0;

        const int line=sl.GetCount();
        String<T> **sp=sl.GetDataList();

        for(int i=0;i<line;i++)
        {
            total_chars+=(*sp)->Length();

            ++sp;
        }

        total_chars+=line*end_line.Length();

        String<T> fullname;

        T *tp=fullname.Resize(total_chars);

        sp=sl.GetDataList();
        for(int i=0;i<line;i++)
        {
            hgl_cpy(tp,(*sp)->c_str(),(*sp)->Length());
            tp+=(*sp)->Length();
            hgl_cpy(tp,end_line.c_str(),end_line.Length());
            tp+=end_line.Length();

            ++sp;
        }

        return fullname;
    }
}//namespace hgl
#endif//HGL_MERGE_STRING_INCLUDE
