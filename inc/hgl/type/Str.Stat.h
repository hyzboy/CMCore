#pragma once

namespace hgl
{
    /**
    * 统计在字符串中某个字符的出现次数
    * @param str 字符串
    * @param ch
    * @return 出现次数
    */
    template<typename T>
    inline const int stat_char(T *str,T ch)
    {
        if(!str)return(0);

        int count=0;

        while(*str)
        {
            if(*str==ch)
                ++count;

            ++str;
        }

        return(count);
    }

    /**
    * 统计一个字符串的行数
    */
    template<typename T>
    inline const int stat_line(T *str)
    {
        if(!str)return(0);

        int len = hgl::strlen(str);
        if(len<=0) return 0;

        T *p=str+len-1;

        if(*p=='\n')return(hgl::stat_char(str,'\n'));
        else return(hgl::stat_char(str,'\n')+1);
    }
}//namespace hgl
