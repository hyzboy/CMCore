#pragma once

namespace hgl
{
    /**
    * 查找一个字符串在一个字符串列表中的序列号
    * @param list 对应的字符串列表,以0结尾,如:const char *target_list[]={"self","line","circle","\0"};
    * @param str 要查找的字节串
    * @return 返回查找出的序号,-1表示失败
    */
    template<typename T>
    inline const int find_str_in_array(const T **list,const T *str)
    {
        if(!str||!list)return(-1);

        int index=0;

        do
        {
            if(*list[index]==0)
                return(-1);

            if(hgl::stricmp(list[index],str)==0)
                return index;

            ++index;
        }while(*list[index]);

        return(-1);
    }

    /**
    * 查找一个字符串在一个字符串列表中的序列号
    * @param count 字符串列表中的字符串数量
    * @param str_array 对应的字符串列表
    * @param str 要查找的字节串
    * @param str_len要查找的字节串长度(0表示不限定长度)
    * @return 返回查找出的序号,-1表示失败
    */
    template<typename T>
    inline const int find_str_in_array(int count,const T **str_array,const T *str,int str_len=0)
    {
        if(str_len<=0)
            str_len=hgl::strlen(str);

        int len;

        for(int i=0;i<count;i++)
        {
            len=hgl::strlen(str_array[i]);

            if(len!=str_len)continue;

            if(hgl::stricmp(str_array[i],str,len)==0)return(i);
        }

        return(-1);
    }

    /**
    * 判断一个字符串是否在当前字符串列表中
    */
    template<typename T> inline bool string_in_list(const T **list,const T *str)
    {
        return hgl::find_str_in_array(list,str)!=-1;
    }
}//namespace hgl
