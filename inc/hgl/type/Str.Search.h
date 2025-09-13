#pragma once
#include <hgl/type/CharType.h>
#include <hgl/type/Str.Length.h>
#include <hgl/type/Str.Case.h>
#include <hgl/type/Str.Copy.h>
#include <cstddef>
#include <type_traits>
namespace hgl
{
    /**
    * 在字符串str1内查找另一个字符串str2
    */
    template<typename T1,typename T2>
    inline const T1 *strstr(const T1 *str1,const std::size_t size1,const T2 *str2,const std::size_t size2)
    {
        if(!str1||!str2) return nullptr;
        if(size1==0||size2==0) return nullptr;
        if(size1 < size2) return nullptr;

        for(std::size_t i=0;i+size2<=size1;++i)
        {
            const T1 *s1 = str1 + i;
            const T2 *s2 = str2;
            std::size_t s = size2;

            while(s>0 && (*s1)==(*s2))
            {
                ++s1; ++s2; --s;
            }

            if(s==0) return str1 + i;
        }

        return nullptr;
    }

    // non-const overload
    template<typename T1,typename T2>
    inline T1 *strstr(T1 *str1,const std::size_t size1,const T2 *str2,const std::size_t size2)
        requires (!std::is_const_v<T1>)
    {
        return const_cast<T1*>(hgl::strstr<const T1,T2>(static_cast<const T1*>(str1), size1, str2, size2));
    }

    /**
    * 在字符串str1内查找另一个字符串str2(从后向前)
    */
    template<typename T1,typename T2>
    inline const T1 *strrstr(const T1 *str1,const std::size_t size1,const T2 *str2,const std::size_t size2)
    {
        if(!str1||!str2) return nullptr;
        if(size1==0||size2==0) return nullptr;
        if(size1 < size2) return nullptr;

        // iterate starting positions in reverse
        std::size_t count = size1 - size2 + 1;
        for(std::size_t idx = count; idx>0; --idx)
        {
            std::size_t i = idx - 1;
            const T1 *s1 = str1 + i;
            const T2 *s2 = str2;
            std::size_t s = size2;

            while(s>0 && (*s1)==(*s2))
            {
                ++s1; ++s2; --s;
            }

            if(s==0) return str1 + i;
        }

        return nullptr;
    }

    // non-const overload
    template<typename T1,typename T2>
    inline T1 *strrstr(T1 *str1,const std::size_t size1,const T2 *str2,const std::size_t size2)
        requires (!std::is_const_v<T1>)
    {
        return const_cast<T1*>(hgl::strrstr<const T1,T2>(static_cast<const T1*>(str1), size1, str2, size2));
    }

    /**
    * 在字符串str1内查找另一个字符串str2,忽略大小写（ASCII）
    */
    template<typename T1,typename T2>
    inline const T1 *stristr(const T1 *str1,const std::size_t size1,const T2 *str2,const std::size_t size2)
    {
        if(!str1||!str2) return nullptr;
        if(size1==0||size2==0) return nullptr;
        if(size1 < size2) return nullptr;

        for(std::size_t i=0;i+size2<=size1;++i)
        {
            const T1 *s1 = str1 + i;
            const T2 *s2 = str2;
            std::size_t s = size2;
            bool ok = true;

            while(s>0)
            {
                int gap = hgl::compare_char_icase(*s1, *s2);
                if(gap!=0)
                {
                    ok = false;
                    break;
                }
                ++s1; ++s2; --s;
            }

            if(ok) return str1 + i;
        }

        return nullptr;
    }

    // non-const overload
    template<typename T1,typename T2>
    inline T1 *stristr(T1 *str1,const std::size_t size1,const T2 *str2,const std::size_t size2)
        requires (!std::is_const_v<T1>)
    {
        return const_cast<T1*>(hgl::stristr<const T1,T2>(static_cast<const T1*>(str1), size1, str2, size2));
    }

    /**
    * 在字符串中查找某个字符
    */
    template<typename TS,typename TC>
    inline const TS *strchr(const TS *str,TC ch)
    {
        if(!str) return nullptr;

        while(*str)
        {
            if(*str==ch) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC ch)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strchr<const TS,TC>(static_cast<const TS*>(str), ch));
    }

    /**
    * 在字符串中查找某个字符，限定最大长度
    */
    template<typename TS,typename TC>
    inline const TS *strchr(const TS *str,TC ch,std::size_t n)
    {
        if(!str||n==0) return nullptr;

        while(n-- && *str)
        {
            if(*str==ch) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC ch,std::size_t n)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strchr<const TS,TC>(static_cast<const TS*>(str), ch, n));
    }

    /**
    * 在字符串中查找多个字符中的任意一个
    */
    template<typename TS,typename TC>
    inline const TS *strchr(const TS *str,const TC *ch,const std::size_t ch_count)
    {
        if(!str||!ch||ch_count==0) return nullptr;

        while(*str)
        {
            if(hgl::strchr(ch, *str, ch_count))
                return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC *ch,const std::size_t ch_count)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strchr<const TS,TC>(static_cast<const TS*>(str), ch, ch_count));
    }

    /**
    * 在字符串中查找排除指定字符外的第一个字符
    */
    template<typename TS,typename TC>
    inline const TS *strechr(const TS *str,TC ch)
    {
        if(!str) return nullptr;

        while(*str)
        {
            if(*str!=ch) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC ch)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strechr<const TS,TC>(static_cast<const TS*>(str), ch));
    }

    template<typename TS,typename TC>
    inline const TS *strechr(const TS *str,TC ch,std::size_t n)
    {
        if(!str||n==0) return nullptr;

        while(n-- && *str)
        {
            if(*str!=ch) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC ch,std::size_t n)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strechr<const TS,TC>(static_cast<const TS*>(str), ch, n));
    }

    template<typename TS,typename TC>
    inline const TS *strechr(const TS *str,const TC *ch,const std::size_t ch_count)
    {
        if(!str||!ch||ch_count==0) return nullptr;

        while(*str)
        {
            if(!hgl::strchr(ch, *str, ch_count)) return str;
            ++str;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC *ch,const std::size_t ch_count)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strechr<const TS,TC>(static_cast<const TS*>(str), ch, ch_count));
    }

    /**
    * 在字符串中从结尾处开始查找某个字符
    */
    template<typename TS,typename TC>
    inline const TS *strrchr(const TS *str,const std::size_t len,const TC ch)
    {
        if(!str||len==0) return nullptr;

        for(std::size_t i = len; i>0; --i)
        {
            const TS *p = str + (i-1);
            if(*p==ch) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const std::size_t len,const TC ch)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strrchr<const TS,TC>(static_cast<const TS*>(str), len, ch));
    }

    template<typename TS,typename TC>
    inline const TS *strrchr(const TS *str,const std::size_t len,const TC *ch,const std::size_t ch_count)
    {
        if(!str||len==0||!ch||ch_count==0) return nullptr;

        for(std::size_t i = len; i>0; --i)
        {
            const TS *p = str + (i-1);
            if(hgl::strchr(ch, *p, ch_count)) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const std::size_t len,const TC *ch,const std::size_t ch_count)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strrchr<const TS,TC>(static_cast<const TS*>(str), len, ch, ch_count));
    }

    template<typename TS,typename TC>
    inline const TS *strrchr(const TS *str,const std::size_t len,const std::size_t off,const TC ch)
    {
        if(!str||len==0||off>=len) return nullptr;

        std::size_t start = len - off;
        for(std::size_t i = start; i>0; --i)
        {
            const TS *p = str + (i-1);
            if(*p==ch) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const std::size_t len,const std::size_t off,const TC ch)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strrchr<const TS,TC>(static_cast<const TS*>(str), len, off, ch));
    }

    template<typename TS,typename TC>
    inline const TS *strrchr(const TS *str,const std::size_t len,const std::size_t off,const TC *ch,const std::size_t ch_count)
    {
        if(!str||len==0||off>=len||!ch||ch_count==0) return nullptr;

        std::size_t start = len - off;
        for(std::size_t i = start; i>0; --i)
        {
            const TS *p = str + (i-1);
            if(hgl::strchr(ch, *p, ch_count)) return p;
        }

        return nullptr;
    }

    // non-const overload
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const std::size_t len,const std::size_t off,const TC *ch,const std::size_t ch_count)
        requires (!std::is_const_v<TS>)
    {
        return const_cast<TS*>(hgl::strrchr<const TS,TC>(static_cast<const TS*>(str), len, off, ch, ch_count));
    }

    /**
    * 给一个文件名更改扩展名
    */
    template<typename T>
    inline void replace_extname(T *new_filename,const T *old_filename,std::size_t max_len,const T *new_extname,const T &separative_char=T('.'))
    {
        if(!new_filename||!old_filename||max_len==0) return;

        const std::size_t old_len = hgl::strlen(old_filename);
        const T *p = hgl::strrchr(old_filename, old_len, separative_char);

        if(p)
        {
            std::size_t prefix_len = static_cast<std::size_t>(p - old_filename + 1);
            std::size_t written = static_cast<std::size_t>(hgl::strcpy(new_filename, max_len, old_filename, prefix_len));
            std::size_t remain = (written<=max_len)?(max_len - written):0;
            if(remain>0 && new_extname)
                hgl::strcpy(new_filename+written, remain, new_extname);
        }
        else
        {
            std::size_t written = static_cast<std::size_t>(hgl::strcpy(new_filename, max_len, old_filename, old_len));
            std::size_t remain = (written<=max_len)?(max_len - written):0;
            if(remain>1)
            {
                new_filename[written]=T('.');
                ++written; --remain;
                if(new_extname && remain>0)
                    hgl::strcpy(new_filename+written, remain, new_extname);
                else if(remain>0)
                    new_filename[written]=0;
            }
            else if(remain>0)
            {
                new_filename[written]=0;
            }
        }
    }

    template<typename T>
    inline void replace_extname(T *filename,const T *new_extname)
    {
        std::size_t len = hgl::strlen(filename);
        const T *p = hgl::strrchr(filename, len, T('.'));

        if(p)
        {
            // p points inside filename; need mutable to write after dot
            T *w = const_cast<T*>(p) + 1;
            hgl::strcpy(w, new_extname);
        }
        else
        {
            // concatenate
            hgl::strcat(filename, T('.'));
            hgl::strcat(filename, new_extname);
        }
    }

    template<typename T>
    inline int replace(T *str,const T tch,const T sch)
    {
        if(!str) return 0;

        int count=0;
        while(*str)
        {
            if(*str==sch)
            {
                *str=tch;
                ++count;
            }
            ++str;
        }
        return count;
    }
}//namespace hgl
