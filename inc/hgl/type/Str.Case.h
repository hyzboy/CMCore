#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
namespace hgl
{

    /**
    * 将一个字符串中的字母全部改为大写
    * @param src 要处理的字符串
    */
    template<typename T>
    inline void to_upper_char(T *str)
    {
        if(!str)return;

        while(*str)
        {
            if(*str>='a'&&*str<='z')
                *str-=32;

            ++str;
        }
    }

    /**
    * 将一个字符串中的字母全部改为小写
    * @param src 要处理的字符串
    */
    template<typename T>
    inline void to_lower_char(T *str)
    {
        if(!str)return;

        while(*str)
        {
            if(*str>='A'&&*str<='Z')
                *str+=32;

            ++str;
        }
    }

    /**
    * 将一个字符串中的字母全部改为大写
    * @param src 要处理的字符串
    */
    template<typename T>
    inline void to_upper_char(const T *src,T *dst)
    {
        if(!dst)return;

        if(!src)
        {
            *dst=0;
            return;
        }

        do
        {
            if(*src>='a'&&*src<='z')
                *dst=*src-32;
            else
                *dst=*src;

            ++src;
            ++dst;
        }while(*src);

        *dst=0;
    }

    /**
    * 将一个字符串中的字母全部改为小写
    * @param src 要处理的字符串
    */
    template<typename T>
    inline void to_lower_char(const T *src,T *dst)
    {
        if(!dst)return;

        if(!src)
        {
            *dst=0;
            return;
        }

        do
        {
            if(*src>='A'&&*src<='Z')
                *dst=*src+32;
            else
                *dst=*src;

            ++src;
            ++dst;
        }while(*src);

        *dst=0;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成小写
    */
    template<typename T>
    inline const uint lower_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source>='A'&&*source<='Z')
                *target=*source+32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
        }

        *target=0;
        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写
    */
    template<typename T>
    inline const uint upper_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source>='a'&&*source<='z')
                *target=*source-32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
        }

        *target=0;
        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成小写
    */
    template<typename T>
    inline const uint lower_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source>='A'&&*source<='Z')
                *target=*source+32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
            --source_max;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写
    */
    template<typename T>
    inline const uint upper_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source>='a'&&*source<='z')
                *target=*source-32;
            else
                *target=*source;

            ++target;
            ++source;
            ++count;
            --source_max;
        }

        *target=0;
        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成小写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint lower_clip_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source!=' ')
            {
                if(*source>='A'&&*source<='Z')
                    *target=*source+32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint upper_clip_cpy(T *target,const T *source)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source)
        {
            if(*source!=' ')
            {
                if(*source>='a'&&*source<='z')
                    *target=*source-32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，将字符串全部转换成小写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint lower_clip_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source!=' ')
            {
                if(*source>='A'&&*source<='Z')
                    *target=*source+32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
            --source_max;
        }

        *target=0;

        return count;
    }

    /**
    * 复制一个字符串，并将字符串全部转换成大写，同时清空字符串中的空格
    */
    template<typename T>
    inline const uint upper_clip_cpy(T *target,const T *source,int source_max)
    {
        if(!target||!source)return 0;
        uint count=0;

        while(*source&&source_max>0)
        {
            if(*source!=' ')
            {
                if(*source>='a'&&*source<='z')
                    *target=*source-32;
                else
                    *target=*source;

                ++target;
                ++count;
            }

            ++source;
            --source_max;
        }

        *target=0;
        return count;
    }
}//namespace hgl
