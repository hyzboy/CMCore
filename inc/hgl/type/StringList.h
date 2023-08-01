#ifndef HGL_STRINGLIST_INCLUDE
#define HGL_STRINGLIST_INCLUDE

#include<hgl/type/ObjectList.h>
#include<hgl/type/String.h>
#include<hgl/io/DataInputStream.h>
#include<hgl/io/DataOutputStream.h>
#include<initializer_list>

namespace hgl
{
    /**
    * 字符串列表处理类，用于保存字符串列表。
    */
    template<typename T> class StringList                                                           ///字符串列表处理类
    {
        using StringClass=String<T>;
        static StringClass NullString;

    protected:

        ObjectList<StringClass> Items;

    public: //属性

        StringClass **  GetDataList ()const{return Items.GetData();}                                ///<取得字符串列表指针数据
        const int       GetCount    ()const{return Items.GetCount();}                               ///<字符串列表行数虚拟变量

        StringClass **  begin       ()const{return Items.GetData();}
        StringClass **  end         ()const{return Items.GetData()+Items.GetCount();}

        const bool      IsEmpty     ()const{return Items.IsEmpty();}                                ///<字符串列表是否为空

    public: //操作符重载

        StringClass &operator[](int n)const
        {
            if(n<0||n>=Items.GetCount())
                return NullString;

            StringClass *result=Items[n];

            if(result)
                return(*result);
            else
                return NullString;
        }

        StringList<T> &operator = (const StringList<T> &sl)
        {
            Clear();

            const int n=sl.GetCount();
            StringClass *str;

            for(int i=0;i<n;i++)
            {
                str=sl.Items[i];
                this->Add(*str);
            }

            return(*this);
        }

    public: //方法

        StringList()=default;                                                                       ///<本类构造函数
        StringList(const StringList &)=delete;                                                      ///<禁用，防止出现StringList = const StringList &的情况
        StringList(const std::initializer_list<T *> &lt)
        {
            for(T *str:lt)
                Add(str);
        }

        //注：这里不要实现StringList(StringClass &)或StringList(StringClass *)之类
        virtual ~StringList(){Clear();}                                                             ///<本类析构函数

        /**
        * 增加一行字符串到当前列表中
        * @param str 要增加的字符串
        * @return 增加字符串成功后的索引
        */
        int Add(const StringClass &str){return Items.Add(new StringClass(str));}                    ///<添加字符串

        /**
         * 增加一行字符串到当前列表中,同时做重复判断，重复则不添加
         */
        int AddUnique(const StringClass &str)
        {
            int pos=Find(str);

            if(pos!=-1)
                return pos;

            return Items.Add(new StringClass(str));
        }

        /**
        * 增加一个字符串列表到当前字符串列表中
        * @param sl 要增加的字符串列表
        */
        int Add(const StringList<T> &sl)                                                            ///<添加字符串
        {
            const int count=sl.GetCount();

            StringClass **str=sl.Items.GetData();

            for(int i=0;i<count;i++)
            {
                Add(**str);
                ++str;
            }

            return(count);
        }

        /**
        * 增加一个字符串列表到当前字符串列表中，同时做去重复处理
        * @param sl 要增加的字符串列表
        */
        int AddUnique(const StringList<T> &sl)                                                            ///<添加字符串
        {
            const int count=sl.GetCount();

            StringClass **str=sl.Items.GetData();

            for(int i=0;i<count;i++)
            {
                if(Find(**str)==-1)
                    Add(**str);

                ++str;
            }

            return(count);
        }

        /**
        * 清除所有字符串
        */
        void Clear(){Items.ClearData();}                                                                ///<删除列表中的所有字符串

        /**
        * 删除指定的字符串
        * @param index 要删除的字符串索引
        */
        void Delete(int index){Items.Delete(index);}                                                ///<删除列表中指定的字符串

        /**
         * 删除指定字符串
         * @param str 要删除的字符串
         * @return 成功删除的行数
         */
        int Delete(const StringClass &str)
        {
            int count=Items.GetCount();
            int result=0;
            StringClass** sl = Items.GetData();
            sl += count - 1;

            while(count--)
            {
                if((*sl)->Comp(str)==0)
                {
                    Items.Delete(count);
                    result++;
                }

                --sl;
            }

            return(result);
        }

        /**
        * 查找字符串
        * @param str 要查找的字符串
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int  Find(const StringClass &str) const                                                     ///<查找字符串,未找到返回-1
        {
            const int count=Items.GetCount();
            StringClass** sl = Items.GetData();

            for(int i=0;i<count;i++)
                if((*sl)->Comp(str)==0)
                    return(i);
                else
                    ++sl;

            return(-1);
        }

        /**
        * 查找字符串,英文无视大小写
        * @param str 要指找的字符串
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int  CaseFind(const StringClass &str) const                                                 ///<查找字符串,英文无视大小写,未找到返回-1
        {
            const int count=Items.GetCount();
            StringClass** sl = Items.GetData();

            for (int i = 0; i < count; i++)
                if ((*sl)->CaseComp(str) == 0)
                    return(i);
                else
                    ++sl;

            return(-1);
        }

        /**
         * 确认字符串确在,英文区分大小写
         */
        bool Exist(const StringClass &str) const                                                    ///<确认字符串确在,英文区分大小写
        {
            return Find(str)!=-1;
        }

        /**
         * 确认字符串确在,英文无视大小写
         */
        bool CaseExist(const StringClass &str) const                                                ///<确认字符串确在,英文无视大小写
        {
            return CaseFind(str)!=-1;
        }

        /**
        * 查找字符串，并指定最大比较长度
        * @param str 要查找的字符串
        * @param cn 限定的要查找字符串的最大长度
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int  Find(const StringClass &str,const int cn) const                                        ///<查找字符串,未找到返回-1
        {
            const int count=Items.GetCount();

            StringClass** sl = Items.GetData();

            for(int i=0;i<count;i++)
                if((*sl)->Comp(str,cn)==0)
                    return(i);
                else
                    ++sl;

            return(-1);
        }

        /**
        * 查找字符串,英文无视大小写，并指定最大比较长度
        * @param str 要指找的字符串
        * @param cn 限定的要查找字符串的最大长度
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int  CaseFind(const StringClass &str,const int cn) const                                    ///<查找字符串,英文无视大小写,未找到返回-1
        {
            const int count=Items.GetCount();

            StringClass** sl = Items.GetData();

            for (int i = 0; i < count; i++)
                if ((*sl)->CaseComp(str, cn) == 0)
                    return(i);
                else
                    ++sl;

            return(-1);
        }

        /**
        * 在指定位置插入一个字符串
        * @param index 要插入字符串的位置
        * @param str 要插入的字符串
        */
        void Insert(int index,const StringClass &str)                                               ///<在指定位置插入一个字符串
        {
            if(index<Items.GetCount())
                Items.List<StringClass *>::Insert(index,new StringClass(str));
        }

        /**
        * 交换两个字符串的位置
        * @param index1 第一个字符串的位置
        * @param index2 第二个字符串的位置
        */
        void Exchange(int index1,int index2)                                                        ///<交换两行字符串
        {
            Items.Exchange(index1,index2);
        }

        const StringClass &GetString(int n)const{return *(Items[n]);}                               ///<取得指定行字符串
    };//template<typename T> class StringList

    template<typename T> String<T> StringList<T>::NullString;                                       ///<空字符串实例

#define DEFINE_STRING_LIST(name)    using name##StringList=StringList<name##String::CharType>;

    DEFINE_STRING_LIST(UTF8)
    DEFINE_STRING_LIST(UTF16)
    DEFINE_STRING_LIST(UTF32)
    DEFINE_STRING_LIST(Ansi)
    DEFINE_STRING_LIST(Wide)
    DEFINE_STRING_LIST(OS)

#undef DEFINE_STRING_LIST
}//namespace hgl
#endif//HGL_STRINGLIST_INCLUDE
