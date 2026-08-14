#pragma once

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

        for(const String<T> *str : sl)
        {
            total_chars+=str->Length();
        }

        total_chars+=sl.GetCount()*end_line.Length();

        String<T> fullname;

        T *tp=fullname.Resize(total_chars);

        for(const String<T> *str : sl)
        {
            mem_copy(tp,str->c_str(),str->Length());
            tp+=str->Length();
            mem_copy(tp,end_line.c_str(),end_line.Length());
            tp+=end_line.Length();
        }

        return fullname;
    }
}//namespace hgl
