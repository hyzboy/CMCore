#pragma once
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Comp.h>
#include <cstddef>
namespace hgl
{
    /**
     * @brief CN: 在以 null 结尾的字符串列表中查找指定字符串的索引。
     * @brief EN: Find the index of a string in a null-terminated array of strings.
     *
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] list CN: 以 null 结尾的字符串数组（最后一个元素为空指针）. EN: null-terminated array of string pointers.
     * @param[in] str CN: 要查找的字符串. EN: string to find.
     * @return int CN/EN: 找到返回索引，未找到返回 -1. EN: index of found string or -1 if not found.
     */
    template<typename CharT>
    inline int find_str_in_array(const CharT **list, const CharT *str)
    {
        if(!str || !list) return -1;

        int index = 0;
        while(list[index] && list[index][0])
        {
            if(hgl::stricmp(list[index], str) == 0)
                return index;
            ++index;
        }

        return -1;
    }

    /**
     * @brief CN: 在指定长度的字符串数组中查找字符串（可以限定目标字符串长度）。
     * @brief EN: Find a string in an array of given count; can limit target length.
     *
     * @tparam CharT 字符类型。EN: character type.
     * @param[in] count CN: 数组中元素数量. EN: number of elements in array.
     * @param[in] str_array CN: 字符串数组指针. EN: array of string pointers.
     * @param[in] str CN: 要查找的字符串. EN: string to find.
     * @param[in] str_len CN: 指定目标字符串长度，0 表示按实际长度比较. EN: length of target string to compare; 0 means use strlen.
     * @return int CN/EN: 找到返回索引，未找到返回 -1. EN: index of found string or -1 if not found.
     */
    template<typename CharT>
    inline int find_str_in_array(int count, const CharT **str_array, const CharT *str, int str_len = 0)
    {
        if(count <= 0 || !str || !str_array) return -1;

        if(str_len <= 0)
            str_len = hgl::strlen(str);

        for(int i = 0; i < count; ++i)
        {
            int len = hgl::strlen(str_array[i]);
            if(len != str_len) continue;
            if(hgl::stricmp(str_array[i], str, static_cast<std::size_t>(str_len)) == 0) return i;
        }

        return -1;
    }

    /**
     * @brief CN: 判断字符串是否存在于以 null 结尾的字符串列表中。
     * @brief EN: Check whether a string exists in a null-terminated array of strings.
     */
    template<typename CharT>
    inline bool string_in_list(const CharT **list, const CharT *str)
    {
        return hgl::find_str_in_array(list, str) != -1;
    }
}//namespace hgl
