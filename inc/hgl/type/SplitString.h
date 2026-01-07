#pragma once

#include<hgl/type/StringList.h>
namespace hgl
{
    /**
     * 拆分字符串模板类
     */
    template<typename T> class SplitString
    {
    public:

        virtual ~SplitString()=default;

        virtual void OnNewString(const T *,const int)=0;                                            ///<发现一个新的字符串

        virtual bool isSeparatorChar(const T &)=0;                                                  ///<是否是分隔符
        virtual uint isSeparator(const T *str)
        {
            uint size=0;

            while(isSeparatorChar(*str))
            {
                ++size;
                ++str;
            }

            return size;
        }

    public:

        virtual int Split(const T *str,int size)
        {
            if(!str||size<=0)return(-1);

            int count=0;
            int sc=0;
            const T *p,*sp;

            sp=p=str;

            while(size>0)
            {
                if(!(*p))
                {
                    if(p>sp)
                    {
                        OnNewString(sp,p-sp);
                        ++count;
                    }

                    --size;
                    return count;
                }

                sc=isSeparator(p);
                if(sc>0)
                {
                    if(p>sp)
                    {
                        OnNewString(sp,p-sp);
                        ++count;
                    }

                    p+=sc;      ///<跳过分割符
                    size-=sc;

                    sp=p;
                }
                else
                {
                    ++p;
                    --size;
                }
            }

            if(p>sp)
            {
                OnNewString(sp,p-sp);
                ++count;
            }

            return count;
        }

        virtual int Split(const String<T> &str)
        {
            return Split(str.c_str(),str.Length());
        }
    };//template<typename T> class SplitString

    /**
     * 拆分字符串到字符串列表模板类
     */
    template<typename T> class SplitStringToStringList:public SplitString<T>
    {
    protected:

        StringList<T> *string_list;

    public:

        SplitStringToStringList(StringList<T> *sl)
        {
            string_list=sl;
        }

        virtual ~SplitStringToStringList()=default;

        virtual void OnNewString(const T *str,const int size) override
        {
            if(!str||size<=0)return;

            string_list->Add(String<T>(str,size));
        }

        virtual int Split(const T *str,int size) override
        {
            if(!string_list)return(-1);

            return SplitString<T>::Split(str,size);
        }

        virtual int Split(const String<T> &str) override
        {
            if(!string_list)return(-1);

            return SplitString<T>::Split(str.c_str(),str.Length());
        }
    };//template<typename T> class SplitStringToStringList:public SplitString

    /**
     * 基于函数和指定条件拆分字符串到字符串列表模板类
     */
    template<typename T,typename C> class SplitStringToStringListByCondition:public SplitStringToStringList<T>
    {
        using FuncType = C (*)(T);
        FuncType isFunc;
        C is_condition;

    public:

        SplitStringToStringListByCondition(StringList<T> *sl,FuncType func,const C is_con):SplitStringToStringList<T>(sl)
        {
            isFunc=func;
            is_condition=is_con;
        }
        virtual ~SplitStringToStringListByCondition()=default;

        bool isSeparatorChar(const T &ch) override
        {
            return isFunc(ch)==is_condition;
        }

        virtual int Split(const T *str,int size) override
        {
            if(!isFunc)return(-1);

            return SplitStringToStringList<T>::Split(str,size);
        }

        virtual int Split(const String<T> &str) override
        {
            if(!isFunc)return(-1);

            return SplitStringToStringList<T>::Split(str.c_str(),str.Length());
        }
    };//template<typename T,typename C> class SplitStringToStringListByCondition:public SplitStringToStringList<T>

    /**
     * 以不可打印字符为分隔拆解一个字符串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListBySpace(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition<T,bool> split(&sl,is_space,true);

        return split.Split(str,size);
    }//int SplitToStringListBySpace

#define SPLIT_STRING_TO_STRING_LIST(name)   template<typename T> int SplitToStringListBy##name(StringList<T> &sl,const String<T> &str)  \
                                            {   \
                                                return SplitToStringListBy##name<T>(sl,str.c_str(),str.Length());  \
                                            }

    SPLIT_STRING_TO_STRING_LIST(Space)

    /**
     * 拆分代码用字符串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListByCodes(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition<T,bool> split(&sl,is_identifier_char,false);

        return split.Split(str,size);
    }//int SplitToStringListByCodes

    SPLIT_STRING_TO_STRING_LIST(Codes)

    /**
     * 拆分数字串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListByDigit(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition<T,bool> split(&sl,is_digit,false);

        return split.Split(str,size);
    }//int SplitToStringListByDigit

    SPLIT_STRING_TO_STRING_LIST(Digit)

    /**
     * 拆分16进制数字串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListByXDigit(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition<T,bool> split(&sl,is_hex_digit,false);

        return split.Split(str,size);
    }//int SplitToStringListByXDigit

    SPLIT_STRING_TO_STRING_LIST(XDigit)

    /**
     * 拆分浮点数字串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListByFloat(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition<T,bool> split(&sl,is_float_char,false);

        return split.Split(str,size);
    }//int SplitToStringListByFloat

    SPLIT_STRING_TO_STRING_LIST(Float)

    /**
     * 基于指定字符拆分字符串到字符串列表模板类
     */
    template<typename T> class SplitStringToStringListByChar:public SplitStringToStringList<T>
    {
        T separator_char;

    public:

        SplitStringToStringListByChar(StringList<T> *sl,const T &sc):SplitStringToStringList<T>(sl)
        {
            separator_char=sc;
        }
        virtual ~SplitStringToStringListByChar()=default;

        bool isSeparatorChar(const T &ch) override
        {
            return ch==separator_char;
        }
    };//template<typename T,typename C> class SplitStringToStringListByChar:public SplitStringToStringList<T>

    /**
     * 以指定字符为分隔拆解一个字符串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @param split_char 分隔字符
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListByChar(StringList<T> &sl,const T *str,int size,const T &split_char)
    {
        SplitStringToStringListByChar<T> split(&sl,split_char);

        return split.Split(str,size);
    }//int SplitToStringList

    template<typename T> int SplitToStringListByChar(StringList<T> &sl,const String<T> &str,const T &split_char)
    {
        SplitStringToStringListByChar<T> split(&sl,split_char);

        return split.Split(str.c_str(),str.Length());
    }

    /**
     * 基于指定的多个字符拆分字符串到字符串列表模板类
     */
    template<typename T> class SplitStringToStringListByChars:public SplitStringToStringList<T>
    {
        String<T> separator_char;

    public:

        SplitStringToStringListByChars(StringList<T> *sl,const String<T> &sc):SplitStringToStringList<T>(sl)
        {
            separator_char=sc;
        }
        virtual ~SplitStringToStringListByChars()=default;

        bool isSeparatorChar(const T &ch) override
        {
            return separator_char.FindChar(ch)!=-1;
        }
    };//template<typename T,typename C> class SplitStringToStringListByChars:public SplitStringToStringList<T>

    /**
     * 以指定字符为分隔拆解一个字符串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @param split_char 分隔字符
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListByChars(StringList<T> &sl,const T *str,int size,const String<T> &split_chars)
    {
        SplitStringToStringListByChars<T> split(&sl,split_chars);

        return split.Split(str,size);
    }//int SplitToStringList

    template<typename T> int SplitToStringListByChars(StringList<T> &sl,const String<T> &str,const String<T> &split_chars)
    {
        SplitStringToStringListByChars<T> split(&sl,split_chars);

        return split.Split(str.c_str(),str.Length());
    }

    /**
     * 拆解一个多行字符串到一个字符串列表
     * @param sl 字符串列表处理类
     * @param str 字符串
     * @param size 字符串长度
     * @return 字符串行数
     */
    template<typename T> int SplitToStringListByEnter(StringList<T> &sl,const T *str,int size)
    {
        const T sc[]={T('\r'),T('\n'),0};

        SplitStringToStringListByChars<T> split(&sl,sc);

        return split.Split(str,size);
    }//int SplitToStringList

    SPLIT_STRING_TO_STRING_LIST(Enter)

    #undef SPLIT_STRING_TO_STRING_LIST

    ///**
    // * 以指定字符为分隔拆解一个字符串到一个字符串列表
    // * @param sl 字符串列表处理类
    // * @param str 字符串
    // * @param size 字符串长度
    // * @param split_char 分隔字符
    // * @param maxSize 最多执行次数
    // * @return 字符串行数
    // */
    //template<typename T> int SplitToStringList(StringList<T> &sl,const T *str,int size,const T &split_char,int maxSize)
    //{
    //    if(!str||size<=0)return(-1);

    //    int count=0;
    //    const T *p,*sp;

    //    sp=p=str;

    //    while(size>0)
    //    {
    //        if(!(*p))
    //        {
    //            if(p>sp)
    //            {
    //                sl.Add(String<T>(sp,p-sp));
    //                ++count;
    //            }

    //            --size;
    //            return count;
    //        }

    //        if(*p==split_char)
    //        {
    //            if(p>sp)
    //            {
    //                sl.Add(String<T>(sp,p-sp));
    //                ++count;
    //            }

    //            sp=p+1; ///<跳过分割符
    //            if(maxSize >0 && count >=maxSize-1)
    //            {
    //                ++p;
    //                --size;
    //                if(size > 0)
    //                {
    //                    sl.Add(String<T>(sp,size));
    //                    ++count;
    //                }

    //                return count;
    //            }
    //        }

    //        ++p;
    //        --size;
    //    }

    //    if(p>sp)
    //    {
    //        sl.Add(String<T>(sp,p-sp));
    //        ++count;
    //    }

    //    return count;
    //}//int SplitToStringList

    //template<typename T> int SplitToStringList(StringList<T> &sl,const String<T> &str,const T &split_char,int maxSize)
    //{
    //    return SplitToStringList<T>(sl,str.c_str(),str.Length(),split_char,maxSize);
    //}

    /**
     * 以指定字符串为分隔拆解出一个字符串的左右两部分
     * @param str 要拆解的字符串
     * @param center 分隔字符串
     * @param left 左边字符串
     * @param right 右边字符串
     */
    template<typename C>
    void SpliteByString(const String<C> &str,const String<C> &center,String<C> *left,String<C> *right)
    {
        if(str.IsEmpty())return;
        if(center.IsEmpty())return;

        int pos = str.FindString(center);

        if(pos < 0)
        {
            if(left)
                *left = str;

            if(right)
                right->Clear();
        }
        else
        {
            if(left)
            {
                if(pos==0)
                    left->Clear();
                else
                    *left = str.SubString(0, pos);
            }

            if(right)
            {
                if(pos+center.Length()>=str.Length())
                    right->Clear();
                else
                    *right = str.SubString(pos + center.Length());
            }
        }
    }

    /**
    * 按指定分隔符拆分字符串为多个字符串
    * @param str 要拆分的字符串
    * @param str_len 字符串长度
    * @param sc 分隔字符
    * @param result_list 拆分后的字符串保存的列表
    * @return 拆分出来的字符串数量
    * @return -1 出错
    */
    template<typename T,typename S,typename SL>
    inline int splite_string_to_stringlist(const T *str,const int str_len,const T &sc,SL &result_list)
    {
        if(!str||!(*str))return(-1);
        if(str_len<=0)return(-1);
        if(sc==0)return(-1);

        const T *p,*sp;
        int len=str_len;
        int count=0;

        sp=str;
        p=sp;

        while(*p&&len)
        {
            --len;

            if(*p!=sc)
            {
                ++p;
                continue;
            }

            result_list.Add(S(sp,p-sp));

            ++p;
            sp=p;
            ++count;
        }

        if(p>sp)
        {
            result_list.Add(S(sp,p-sp));
            ++count;
        }

        return(count);
    }

    template<typename T>
    inline int splite_string_to_stringlist(const T *str,const int str_len,const T &sc,StringList<T> &result_list)
    {
        return splite_string_to_stringlist<T,String<T>,StringList<T>>(str,str_len,sc,result_list);
    }
}//namespace hgl
