#ifndef HGL_SPLIT_STRING_INCLUDE
#define HGL_SPLIT_STRING_INCLUDE

#include<hgl/type/StringList.h>
namespace hgl
{
    /**
     * ����ַ���ģ����
     */
    template<typename T> class SplitString
    {
    public:

        virtual ~SplitString()=default;
        
        virtual void OnNewString(const T *,const int)=0;                                            ///<����һ���µ��ַ���

        virtual bool isSeparatorChar(const T &)=0;                                                  ///<�Ƿ��Ƿָ���
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

                    p+=sc;      ///<�����ָ��
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
     * ����ַ������ַ����б�ģ����
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

            return SplitString::Split(str,size);
        }

        virtual int Split(const String<T> &str) override
        {
            if(!string_list)return(-1);

            return SplitString::Split(str.c_str(),str.Length());
        }
    };//template<typename T> class SplitStringToStringList:public SplitString

    /**
     * ���ں�����ָ����������ַ������ַ����б�ģ����
     */
    template<typename T,typename C> class SplitStringToStringListByCondition:public SplitStringToStringList<T>
    {
        const C (*isFunc)(const T &);
        C is_condition;

    public:

        SplitStringToStringListByCondition(StringList<T> *sl,const C (*func)(const T &),const C is_con):SplitStringToStringList(sl)
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

            return SplitStringToStringList::Split(str,size);
        }

        virtual int Split(const String<T> &str) override
        {
            if(!isFunc)return(-1);

            return SplitStringToStringList::Split(str.c_str(),str.Length());
        }
    };//template<typename T,typename C> class SplitStringToStringListByCondition:public SplitStringToStringList<T>

    /**
     * �Բ��ɴ�ӡ�ַ�Ϊ�ָ����һ���ַ�����һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListBySpace(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition split(&sl,isspace,true);

        return split.Split(str,size);
    }//int SplitToStringListBySpace

#define SPLIT_STRING_TO_STRING_LIST(name)   template<typename T> int SplitToStringListBy##name(StringList<T> &sl,const String<T> &str)  \
                                            {   \
                                                return SplitToStringListBy##name(sl,str.c_str(),str.Length());  \
                                            }

    SPLIT_STRING_TO_STRING_LIST(Space)

    /**
     * ��ִ������ַ�����һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListByCodes(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition split(&sl,iscodechar,false);

        return split.Split(str,size);
    }//int SplitToStringListByCodes
    
    SPLIT_STRING_TO_STRING_LIST(Codes)

    /**
     * ������ִ���һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListByDigit(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition split(&sl,isdigit,false);

        return split.Split(str,size);
    }//int SplitToStringListByDigit
    
    SPLIT_STRING_TO_STRING_LIST(Digit)
    
    /**
     * ���16�������ִ���һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListByXDigit(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition split(&sl,isxdigit,false);

        return split.Split(str,size);
    }//int SplitToStringListByXDigit
    
    SPLIT_STRING_TO_STRING_LIST(XDigit)

    /**
     * ��ָ������ִ���һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListByFloat(StringList<T> &sl,const T *str,int size)
    {
        if(!str||size<=0)return(-1);

        SplitStringToStringListByCondition split(&sl,isfloat,false);

        return split.Split(str,size);
    }//int SplitToStringListByFloat
    
    SPLIT_STRING_TO_STRING_LIST(Float)
    
    /**
     * ����ָ���ַ�����ַ������ַ����б�ģ����
     */
    template<typename T> class SplitStringToStringListByChar:public SplitStringToStringList<T>
    {
        T separator_char;

    public:

        SplitStringToStringListByChar(StringList<T> *sl,const T &sc):SplitStringToStringList(sl)
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
     * ��ָ���ַ�Ϊ�ָ����һ���ַ�����һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @param split_char �ָ��ַ�
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListByChar(StringList<T> &sl,const T *str,int size,const T &split_char)
    {
        SplitStringToStringListByChar split(&sl,split_char);

        return split.Split(str,size);
    }//int SplitToStringList
    
    template<typename T> int SplitToStringListByChar(StringList<T> &sl,const String<T> &str,const T &split_char)
    {
        return SplitToStringList<T>(sl,str.c_str(),str.Length(),split_char);
    }
    
    /**
     * ����ָ���Ķ���ַ�����ַ������ַ����б�ģ����
     */
    template<typename T> class SplitStringToStringListByChars:public SplitStringToStringList<T>
    {
        String<T> separator_char;

    public:

        SplitStringToStringListByChar(StringList<T> *sl,const String<T> &sc):SplitStringToStringList(sl)
        {
            separator_char=sc;
        }
        virtual ~SplitStringToStringListByChar()=default;

        bool isSeparatorChar(const T &ch) override
        {
            return separator_char.FindChar(ch)!=-1;
        }
    };//template<typename T,typename C> class SplitStringToStringListByChars:public SplitStringToStringList<T>
    
    /**
     * ��ָ���ַ�Ϊ�ָ����һ���ַ�����һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @param split_char �ָ��ַ�
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListByChars(StringList<T> &sl,const T *str,int size,const String<T> &split_chars)
    {
        SplitStringToStringListByChars split(&sl,split_chars);

        return split.Split(str,size);
    }//int SplitToStringList
    
    template<typename T> int SplitToStringListByChars(StringList<T> &sl,const String<T> &str,const String<T> &split_chars)
    {
        return SplitToStringList<T>(sl,str.c_str(),str.Length(),split_chars);
    }

    /**
     * ���һ�������ַ�����һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @return �ַ�������
     */
    template<typename T> int SplitToStringListByEnter(StringList<T> &sl,const T *str,int size)
    {
        const T sc[2]={T('\r'),T('\n')};

        SplitStringToStringListByChars split(&sl,sc);

        return split.Split(str,size);
    }//int SplitToStringList

    SPLIT_STRING_TO_STRING_LIST(Enter)

    #undef SPLIT_STRING_TO_STRING_LIST

    /**
     * ��ָ���ַ�Ϊ�ָ����һ���ַ�����һ���ַ����б�
     * @param sl �ַ����б�����
     * @param str �ַ���
     * @param size �ַ�������
     * @param split_char �ָ��ַ�
     * @param maxSize ���ִ�д���
     * @return �ַ�������
     */
    template<typename T> int SplitToStringList(StringList<T> &sl,const T *str,int size,const T &split_char,int maxSize)
    {
        if(!str||size<=0)return(-1);

        int count=0;
        const T *p,*sp;

        sp=p=str;

        while(size>0)
        {
            if(!(*p))
            {
                if(p>sp)
                {
                    sl.Add(String<T>(sp,p-sp));
                    ++count;
                }

                --size;
                return count;
            }

            if(*p==split_char)
            {
                if(p>sp)
                {
                    sl.Add(String<T>(sp,p-sp));
                    ++count;
                }

                sp=p+1; ///<�����ָ��
                if(maxSize >0 && count >=maxSize-1)
                {
                    ++p;
                    --size;
                    if(size > 0)
                    {
                        sl.Add(String<T>(sp,size));
                        ++count;
                    }

                    return count;
                }
            }

            ++p;
            --size;
        }

        if(p>sp)
        {
            sl.Add(String<T>(sp,p-sp));
            ++count;
        }

        return count;
    }//int SplitToStringList

    template<typename T> int SplitToStringList(StringList<T> &sl,const String<T> &str,const T &split_char,int maxSize)
    {
        return SplitToStringList<T>(sl,str.c_str(),str.Length(),split_char,maxSize);
    }
}//namespace hgl
#endif//HGL_SPLIT_STRING_INCLUDE