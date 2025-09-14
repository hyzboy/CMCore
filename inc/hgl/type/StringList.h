﻿#ifndef HGL_STRINGLIST_INCLUDE
#define HGL_STRINGLIST_INCLUDE

#include<hgl/type/String.h>
#include<hgl/io/DataInputStream.h>
#include<hgl/io/DataOutputStream.h>
#include<initializer_list>
#include<algorithm>
#include<hgl/type/ObjectList.h>

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

        // 使用 size_t 作为计数类型以符合 STL 风格
        size_t GetCount() const
        {
            return static_cast<size_t>(Items.GetCount());
        }

        // 原始指针迭代器，兼容旧的 range-for 语法: for(StringClass* p : string_list)
        class RawPtrIterator
        {
            using Iter = typename ObjectList<StringClass>::Iterator;

            Iter it;

        public:
            RawPtrIterator() = default;
            explicit RawPtrIterator(const Iter &i)
                : it(i)
            {
            }

            // return pointer by value so 'for (auto *p : list)' works naturally
            StringClass * operator*()
            {
                return *it; // ObjectList iterator returns T*
            }

            RawPtrIterator & operator++()
            {
                ++it;
                return *this;
            }

            bool operator!=(const RawPtrIterator &o) const
            {
                return it != o.it;
            }

            bool operator==(const RawPtrIterator &o) const
            {
                return it == o.it;
            }
        };

        class ConstRawPtrIterator
        {
            using Iter = typename ObjectList<StringClass>::ConstIterator;

            Iter it;

        public:
            ConstRawPtrIterator() = default;
            explicit ConstRawPtrIterator(const Iter &i)
                : it(i)
            {
            }

            // return const pointer by value for safe const-iteration
            const StringClass * operator*() const
            {
                return *it; // returns T*
            }

            ConstRawPtrIterator & operator++()
            {
                ++it;
                return *this;
            }

            bool operator!=(const ConstRawPtrIterator &o) const
            {
                return it != o.it;
            }

            bool operator==(const ConstRawPtrIterator &o) const
            {
                return it == o.it;
            }
        };

        // begin/end 返回原始指针迭代器，向后兼容旧代码
        RawPtrIterator begin()
        {
            return RawPtrIterator(Items.begin());
        }

        RawPtrIterator end()
        {
            return RawPtrIterator(Items.end());
        }

        ConstRawPtrIterator begin() const
        {
            return ConstRawPtrIterator(Items.begin());
        }

        ConstRawPtrIterator end() const
        {
            return ConstRawPtrIterator(Items.end());
        }

        // 也同时保留直接访问 ObjectList 迭代器
        auto up_begin()
        {
            return Items.begin();
        }

        auto up_end()
        {
            return Items.end();
        }

        auto up_begin() const
        {
            return Items.begin();
        }

        auto up_end() const
        {
            return Items.end();
        }

        bool IsEmpty() const
        {
            return Items.IsEmpty();
        }

    public: //操作符重载

        StringClass &operator[](int n) const
        {
            if (n < 0 || n >= Items.GetCount())
                return NullString;

            StringClass *result = Items[n];

            if (result)
                return (*result);

            return NullString;
        }

        // 强异常安全的赋值：先构造临时副本，然后 swap
        StringList<T> &operator=(const StringList<T> &sl)
        {
            if (this == &sl)
                return *this;

            StringList<T> tmp;

            for(auto str:sl)
                tmp.Add(*str);

            std::swap(Items, tmp.Items);

            return *this;
        }

    public: //方法

        StringList() = default;                                                                       ///<本类构造函数
        StringList(const StringList &) = delete;                                                      ///<禁用，防止出现StringList = const StringList &的情况
        StringList(const std::initializer_list<T *> &lt)
        {
            for (T *str : lt)
                Add(str);
        }

        // 注：这里不要实现StringList(StringClass &)或StringList(StringClass *)之类
        virtual ~StringList()
        {
            Clear();
        }                                                                                            ///<本类析构函数

        /**
        * 增加一行字符串到当前列表中
        * @param str 要增加的字符串
        * @return 增加字符串成功后的索引
        */
        int Add(const StringClass &str)
        {   
            return Items.Add(new StringClass(str));
        }

        /**
         * 增加一行字符串到当前列表中,同时做重复判断，重复则不添加
         */
        int AddUnique(const StringClass &str)
        {
            int pos = Find(str);

            if (pos != -1)
                return pos;

            return Add(str);
        }

        /**
        * 增加一个字符串列表到当前字符串列表中
        * @param sl 要增加的字符串列表
        */
        int Add(const StringList<T> &sl)                                                            ///<添加字符串
        {
            for(auto str:sl)
                Add(*str);

            return sl.GetCount();
        }

        /**
        * 增加一个字符串列表到当前字符串列表中，同时做去重复处理
        * @param sl 要增加的字符串列表
        */
        int AddUnique(const StringList<T> &sl)                                                            ///<添加字符串
        {
            int count = 0;

            for(auto str:sl)
            {
                if (Find(*str) != -1)
                    continue;

                Add(*str);
                ++count;
            }

            return (count);
        }

        /**
        * 清除所有字符串
        */
        void Clear()
        {
            Items.Free();
        }

        /**
        * 删除指定的字符串
        * @param index要删除的字符串索引
        */
        void Delete(int index)
        {
            if (index < 0 || index >= Items.GetCount())
                return;

            Items.DeleteAtOwn(index);
        }

        /**
         * 删除指定字符串
         * @param str 要删除的字符串
         * @return 成功删除的行数
         */
        int Delete(const StringClass &str)
        {
            int result = 0;

            for (int i = Items.GetCount() - 1; i >= 0; --i)
            {
                if (Items[i] && Items[i]->Comp(str) == 0)
                {
                    Items.DeleteAtOwn(i);
                    ++result;
                }
            }

            return (result);
        }

        /**
        * 查找字符串
        * @param str 要查找的字符串
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int Find(const StringClass &str) const                                                     ///<查找字符串,未找到返回-1
        {
            const int count = Items.GetCount();

            for (int i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->Comp(str) == 0)
                    return (i);
            }

            return (-1);
        }

        /**
        * 查找字符串,英文不区分大小写
        * @param str 要查找的字符串
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int CaseFind(const StringClass &str) const                                                 ///<查找字符串,英文不区分大小写,未找到返回-1
        {
            const int count = Items.GetCount();

            for (int i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->CaseComp(str) == 0)
                    return (i);
            }

            return (-1);
        }

        /**
         * 确认字符串存在,英文区分大小写
         */
        bool Exist(const StringClass &str) const                                                    ///<确认字符串存在,英文区分大小写
        {
            return Find(str) != -1;
        }

        /**
         * 确认字符串存在,英文不区分大小写
         */
        bool CaseExist(const StringClass &str) const                                                ///<确认字符串存在,英文不区分大小写
        {
            return CaseFind(str) != -1;
        }

        /**
        * 查找字符串，并指定最大比较长度
        * @param str 要查找的字符串
        * @param cn 限定的要查找字符串的最大长度
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int Find(const StringClass &str, const int cn) const                                        ///<查找字符串,未找到返回-1
        {
            const int count = Items.GetCount();

            for (int i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->Comp(str, cn) == 0)
                    return (i);
            }

            return (-1);
        }

        /**
        * 查找字符串,英文不区分大小写，并指定最大比较长度
        * @param str 要查找的字符串
        * @param cn 限定的要查找字符串的最大长度
        * @return 查找到的字符串的索引,未找到返回-1
        */
        int CaseFind(const StringClass &str, const int cn) const                                    ///<查找字符串,英文不区分大小写,未找到返回-1
        {
            const int count = Items.GetCount();

            for (int i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->CaseComp(str, cn) == 0)
                    return (i);
            }

            return (-1);
        }

        /**
        * 在指定位置插入一个字符串
        * @param index 要插入字符串的位置
        * @param str 要插入的字符串
        */
        void Insert(int index, const StringClass &str)                                               ///<在指定位置插入一个字符串
        {
            if (index < Items.GetCount())
                Items.Insert(index, new StringClass(str));
        }

        /**
        * 交换两个字符串的位置
        * @param index1 第一个字符串的位置
        * @param index2 第二个字符串的位置
        */
        void Exchange(int index1, int index2)                                                        ///<交换两行字符串
        {
            if (index1 < 0 || index2 < 0 || index1 >= Items.GetCount() || index2 >= Items.GetCount())
                return;

            Items.Exchange(index1, index2);
        }

        const StringClass &GetString(int n) const
        {
            return *(Items[n]);
        }                                                                                               ///<取得指定行字符串
    };//template<typename T> class StringList

    template<typename T> String<T> StringList<T>::NullString;                                       ///<空字符串实例

#define DEFINE_STRING_LIST(name)    using name##StringList=StringList<name##String::CharType>;

    DEFINE_STRING_LIST(U8)
    DEFINE_STRING_LIST(U16)
    DEFINE_STRING_LIST(U32)
    DEFINE_STRING_LIST(Ansi)
    DEFINE_STRING_LIST(Wide)
    DEFINE_STRING_LIST(OS)

#undef DEFINE_STRING_LIST
}//namespace hgl
#endif//HGL_STRINGLIST_INCLUDE
