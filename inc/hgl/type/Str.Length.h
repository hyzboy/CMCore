#pragma once

namespace hgl
{
    /**
    * 对字符串计算字串长度
    * @param str 要计算长度的字符串指针
    * @return 字符串长度
    */
    template<typename T>
    inline const int strlen(const T *str)
    {
        if(str&&*str)
        {
            const T *start=str;

            while(*str)
                ++str;

            return int(str-start);
        }

        return(0);
    }

    /**
    * 对字符串计算字串长度
    * @param str 要计算长度的字符串指针
    * @param max_len 最大长度
    * @return 字符串长度
    */
    template<typename T>
    inline const int strlen(const T *str,uint max_len)
    {
        if(str&&*str)
        {
            const T *start=str;

            do
            {
                ++str;
                --max_len;

            }while(max_len>0&&*str);

            return int(str-start);
        }

        return(0);
    }
}//namespace hgl
