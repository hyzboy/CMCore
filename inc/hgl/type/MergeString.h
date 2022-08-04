#ifndef HGL_MERGE_STRING_INCLUDE
#define HGL_MERGE_STRING_INCLUDE

#include<hgl/type/StringList.h>
namespace hgl
{
    /**
     * ��һ���ַ����б����ϳ�һ���ַ���
     * @param sl Ҫ���ϵ��ַ����б�
     * @param end_line ���з�
     * @return �ַ���
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

        T *str=new T[total_chars+1];
        T *tp=str;

        sp=sl.GetDataList();
        for(int i=0;i<line;i++)
        {
            hgl_cpy(tp,(*sp)->c_str(),(*sp)->Length());
            tp+=(*sp)->Length();
            hgl_cpy(tp,end_line.c_str(),end_line.Length());
            tp+=end_line.Length();

            ++sp;
        }

        str[total_chars]=0;
        return String<T>::newOf(str,total_chars);
    }
}//namespace hgl
#endif//HGL_MERGE_STRING_INCLUDE