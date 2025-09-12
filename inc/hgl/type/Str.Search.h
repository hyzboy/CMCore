#pragma once

namespace hgl
{
    /**
    * 在字符串str1内查找另一个字符串str2
    * @param str1 完整的字符串
    * @param size1 str1最大查找字符
    * @param str2 要查找的字符串
    * @param size2 str2长度
    * @return str2在str1中所在位置的指针
    */
    template<typename T1,typename T2>
    inline T1 *strstr(T1 *str1,const int size1,const T2 *str2,const int size2)
    {
        if(!str1||!str2)return(nullptr);
        if(!*str1||!*str2)return(nullptr);
        if(size1<=0)return(nullptr);
        if(size2<=0)return(nullptr);

        T1 *cp = str1;
        T1 *end= str1+size1-size2;
        T1 *s1, *s2;
        int s;

        while (*cp&&cp<=end)
        {
            s1 = cp;
            s2 = (T1 *)str2;

            s=size2;
            while ( s && !(*s1-*s2) )
                ++s1, ++s2,--s;

            if(!s)
                return(cp);

            ++cp;
        }

        return(nullptr);
    }

    /**
    * 在字符串str1内查找另一个字符串str2(从后向前)
    * @param str1 完整的字符串
    * @param size1 str1最大查找字符
    * @param str2 要查找的字符串
    * @param size2 str2长度
    * @return str2在str1中所在位置的指针
    */
    template<typename T1,typename T2>
    inline T1 *strrstr(T1 *str1,const int size1,const T2 *str2,const int size2)
    {
        if(!str1||!str2)return(nullptr);
        if(!*str1||!*str2)return(nullptr);
        if(size1<=0)return(nullptr);
        if(size2<=0)return(nullptr);

        T1 *cp = str1+size1-size2;
        T1 *s1, *s2;
        int s;

        while (*cp&&cp>str1)
        {
            s1 = cp;
            s2 = (T1 *)str2;

            s=size2;
            while ( s && !(*s1-*s2) )
                ++s1, ++s2,--s;

            if (!s)
                return(cp);

            --cp;
        }

        return(nullptr);
    }


    /**
    * 在字符串str1内查找另一个字符串str2,忽略大小写
    * @param str1 完整的字符串
    * @param size1 str1的长度
    * @param str2 要查找的字符串
    * @param size2 str2的长度
    * @return str2在str1中所在位置的指针
    * @return nullptr 没有找到
    */
    template<typename T1,typename T2>
    inline T1 *stristr(T1 *str1,const int size1,T2 *str2,const int size2)
    {
        if(!str1||!str2)return(nullptr);
        if(!*str1||!*str2)return(nullptr);
        if(size1<=0)return(nullptr);
        if(size2<=0)return(nullptr);

        T1 *cp = (T1 *) str1;
        T1 *s1;
        T2 *s2;
        int remain = size1;

        if ( !*str2)
            return (T1 *)str1;

        while (*cp && remain > 0)
        {
            s1 = cp;
            s2 = (T2 *) str2;
            int s = size2;

            while ( *s1 && *s2 && s > 0 )
            {
                if(*s1!=*s2)
                {
                    if(*s1>='a'&&*s1<='z')
                    {
                        if(*s1!=*s2-('A'-'a'))
                            break;
                    }
                    else
                        if(*s1>='A'&&*s1<='Z')
                        {
                            if(*s1!=*s2+('A'-'a'))
                                break;
                        }
                        else
                            break;
                }

                s1++, s2++; --s;
            }

            if (!*s2 || s == 0)
                return(cp);

            ++cp; --remain;
        }

        return(nullptr);
    }

    /**
    * 在字符串中查找某个字符
    * @param str 字符串
    * @param ch 字符
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC ch)
    {
        if(!str||!(*str)||ch==0)return(nullptr);

        while(*str)
        {
            if(*str==ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
    * 在字符串中查找多个字符中的任意一个,指定字符串最大长度
    * @param str 字符串
    * @param ch 字符
    * @param n 字符串长度
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC ch,int n)
    {
        if(!str||!(*str)||ch==0||n<=0)return(nullptr);

        while(*str&&n--)
        {
            if(*str==ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
    * 在字符串中查找多个字符中的任意一个
    * @param str 字符串
    * @param ch 字符列表
    * @param ch_count 字符个数
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strchr(TS *str,TC *ch,const int ch_count)
    {
        if(!str||!(*str)||!ch||!(*ch)||ch_count<=0)return(nullptr);

        while(*str)
        {
            if(hgl::strchr(ch,*str,ch_count))
                return(str);

            ++str;
        }

        return(nullptr);
    }

    /**
    * 在字符串中查找排除指定字符外的第一个字符
    * @param str 字符串
    * @param ch 要排除的字符
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC ch)
    {
        if(!str||!(*str)||ch==0)return(nullptr);

        while(*str)
        {
            if(*str!=ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
    * 在字符串中查找排除指定字符外的第一个字符,指定字符串最大长度
    * @param str 字符串
    * @param ch 要排除的字符
    * @param n 字符串长度
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC ch,int n)
    {
        if(!str||!(*str)||ch==0||n<=0)return(nullptr);

        while(*str&&n--)
        {
            if(*str!=ch)
                return(str);
            else
                ++str;
        }

        return(nullptr);
    }

    /**
    * 在字符串中查找在字符串中查找排除指定字符外的第一个字符
    * @param str 字符串
    * @param ch 要排除的字符列表
    * @param ch_count 字符个数
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strechr(TS *str,TC *ch,const int ch_count)
    {
        if(!str||!(*str)||!ch||!(*ch)||ch_count<=0)return(nullptr);

        while(*str)
        {
            if(!hgl::strchr(ch,*str,ch_count))
                return(str);

            ++str;
        }

        return(nullptr);
    }

    /**
    * 在字符串中从结尾处开始查找某个字符
    * @param str 字符串
    * @param len 字符串长度
    * @param ch 字符
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const TC ch)
    {
        if(!str||!(*str)||len<=0||ch==0)return(nullptr);

        TS *ep=str+len-1;

        while(ep>=str)
        {
            if(*ep==ch)
                return ep;

            --ep;
        }

        return(nullptr);
    }

    /**
    * 在字符串中从结尾处开始查找某个字符
    * @param str 字符串
    * @param len 字符串长度
    * @param ch 字符
    * @param ch_count 字符个数
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const TC *ch,const int ch_count)
    {
        if(!str||!(*str)||len<=0||!ch||!(*ch)||ch_count<=0)return(nullptr);

        TS *ep=str+len-1;

        while(ep>=str)
        {
            if(hgl::strchr(ch,*ep,ch_count))
                return ep;

            --ep;
        }

        return(nullptr);
    }

    /**
    * 在字符串中从结尾处开始查找某个字符
    * @param str 字符串
    * @param len 字符串长度
    * @param off 起始查找位置(倒数)
    * @param ch 字符
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const int off,const TC ch)
    {
        if(!str||!(*str)||len<=0||off>=len||ch==0)return(nullptr);

        TS *ep=str+len-1-off;

        while(ep>=str)
        {
            if(*ep==ch)
                return ep;

            --ep;
        }

        return(nullptr);
    }

    /**
    * 在字符串中从结尾处开始查找某个字符
    * @param str 字符串
    * @param len 字符串长度
    * @param off 起始查找位置(倒数)
    * @param ch 字符
    * @param ch_count 字符个数
    * @return 查找到的位置指针
    */
    template<typename TS,typename TC>
    inline TS *strrchr(TS *str,const int len,const int off,const TC *ch,const int ch_count)
    {
        if(!str||!(*str)||len<=0||off>=len||!ch||!(*ch)||ch_count<=0)return(nullptr);

        TS *ep=str+len-1-off;

        while(ep>=str)
        {
            if(hgl::strchr(ch,*ep,ch_count))
                return ep;

            --ep;
        }

        return(nullptr);
    }


    /**
    * 给一个文件名更改扩展名
    * @param old_filename 原始文件名称
    * @param new_filename 新文件名称
    * @param max_len 文件名最大长度以
    * @param new_extname 新扩展名(不带.)
    */
    template<typename T>
    inline void replace_extname(T *new_filename,const T *old_filename,int max_len,const T *new_extname,const T &separative_char=T('.'))
    {
        if(!new_filename||!old_filename||max_len<=0) return;

        const int old_len=hgl::strlen(old_filename);
        const T *p=hgl::strrchr((T*)old_filename,old_len,separative_char);

        if(p)
        {
            int prefix_len = int(p - old_filename + 1);
            int written = hgl::strcpy(new_filename, max_len, old_filename, prefix_len);
            int remain = max_len - written;
            if(remain>0 && new_extname)
            {
                hgl::strcpy(new_filename+written, remain, new_extname);
            }
        }
        else
        {
            int written = hgl::strcpy(new_filename, max_len, old_filename, old_len);
            int remain = max_len - written;
            if(remain>1)
            {
                new_filename[written]=T('.');
                ++written;
                --remain;
                if(new_extname && remain>0)
                {
                    hgl::strcpy(new_filename+written, remain, new_extname);
                }
                else
                {
                    if(remain>0) new_filename[written]=0;
                }
            }
            else if(remain>0)
            {
                new_filename[written]=0;
            }
        }
    }

    /**
    * 给一个文件名更改扩展名
    * @param filename 文件名称
    * @param new_extname 新扩展名(不带.)
    */
    template<typename T>
    inline void replace_extname(T *filename,const T *new_extname)
    {
        T *p=hgl::strrchr(filename,T('.'));

        if(p)
        {
            hgl::strcpy(p+1,new_extname);
        }
        else
        {
            hgl::strcat(filename,T('.'));
            hgl::strcat(filename,new_extname);
        }
    }

    /**
    * 将一个字符串中的某个字符全部替换为另一个字符
    * @param string 要处理的字符串
    * @param sch 要被替换掉的字符
    * @param tch 替换后的字符
    * @return 替换的次数
    */
    template<typename T>
    inline const int replace(T *str,const T tch,const T sch)
    {
        if(!str)
            return(0);

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

        return(count);
    }
}//namespace hgl
