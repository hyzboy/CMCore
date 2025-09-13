#ifndef HGL_STRINGLIST_INCLUDE
#define HGL_STRINGLIST_INCLUDE

#include<hgl/type/String.h>
#include<hgl/io/DataInputStream.h>
#include<hgl/io/DataOutputStream.h>
#include<initializer_list>
#include<algorithm>
#include<vector>
#include<memory>

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

        std::vector<std::unique_ptr<StringClass>> Items;

    public: //属性

        /**
         * @brief CN: 获取原始指针列表（按值返回临时容器）。
         *        EN: Get list of raw pointers (returns a temporary vector).
         *
         * @return CN: 返回可修改的指针向量。
         *         EN: Returns a vector of modifiable raw pointers.
         */
        std::vector<StringClass *> GetDataList()
        {
            std::vector<StringClass *> v;

            v.reserve(Items.size());

            for (auto &u : Items)
                v.push_back(u.get());

            return v;
        }

        /**
         * @brief CN: 获取原始指针列表（按值返回临时容器，指针为 const）。
         *        EN: Get list of raw const pointers (returns a temporary vector).
         *
         * @return CN: 返回只读指针向量。
         *         EN: Returns a vector of const raw pointers.
         */
        std::vector<const StringClass *> GetDataList() const
        {
            std::vector<const StringClass *> v;

            v.reserve(Items.size());

            for (auto &u : Items)
                v.push_back(u.get());

            return v;
        }

        // 使用 size_t 作为计数类型以符合 STL 风格
        size_t GetCount() const
        {
            return Items.size();
        }

        // 原始指针迭代器，兼容旧的 range-for 语法: for(StringClass* p : string_list)
        class RawPtrIterator
        {
            using Iter = typename std::vector<std::unique_ptr<StringClass>>::iterator;

            Iter it;
            mutable StringClass *cache{nullptr};

        public:
            RawPtrIterator() = default;
            explicit RawPtrIterator(const Iter &i)
                : it(i)
            {
            }

            // return reference to cached pointer so 'auto &p' can bind
            StringClass * & operator*()
            {
                cache = it->get();
                return cache;
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
            using Iter = typename std::vector<std::unique_ptr<StringClass>>::const_iterator;

            Iter it;
            mutable StringClass *cache{nullptr};

        public:
            ConstRawPtrIterator() = default;
            explicit ConstRawPtrIterator(const Iter &i)
                : it(i)
            {
            }

            // return reference to cached pointer so 'auto &p' can bind
            StringClass * & operator*() const
            {
                cache = it->get();
                return cache;
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
            return ConstRawPtrIterator(Items.cbegin());
        }

        ConstRawPtrIterator end() const
        {
            return ConstRawPtrIterator(Items.cend());
        }

        // 也同时保留直接访问 unique_ptr 迭代器
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
            return Items.cbegin();
        }

        auto up_end() const
        {
            return Items.cend();
        }

        bool IsEmpty() const
        {
            return Items.empty();
        }

    public: //操作符重载

        StringClass &operator[](int n) const
        {
            if (n < 0 || n >= static_cast<int>(Items.size()))
                return NullString;

            StringClass *result = Items[static_cast<size_t>(n)].get();

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

            const size_t n = sl.GetCount();

            for (size_t i = 0; i < n; ++i)
            {
                StringClass *str = sl.Items[i].get();

                // 这里使用 tmp.Add 可能抛出；若抛出则 *this 保持不变
                tmp.Add(*str);
            }

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
            Items.push_back(std::make_unique<StringClass>(str));

            return static_cast<int>(Items.size() - 1);
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
            const size_t count = sl.GetCount();

            for (size_t i = 0; i < count; ++i)
            {
                StringClass *p = sl.Items[i].get();

                if (p)
                    Add(*p);
            }

            return (static_cast<int>(count));
        }

        /**
        * 增加一个字符串列表到当前字符串列表中，同时做去重复处理
        * @param sl 要增加的字符串列表
        */
        int AddUnique(const StringList<T> &sl)                                                            ///<添加字符串
        {
            const size_t count = sl.GetCount();

            for (size_t i = 0; i < count; ++i)
            {
                StringClass *p = sl.Items[i].get();

                if (p && Find(*p) == -1)
                    Add(*p);
            }

            return (static_cast<int>(count));
        }

        /**
        * 清除所有字符串
        */
        void Clear()
        {
            Items.clear();
        }

        /**
        * 删除指定的字符串
        * @param index要删除的字符串索引
        */
        void Delete(int index)
        {
            if (index < 0 || index >= static_cast<int>(Items.size()))
                return;

            Items.erase(Items.begin() + index);
        }

        /**
         * 删除指定字符串
         * @param str 要删除的字符串
         * @return 成功删除的行数
         */
        int Delete(const StringClass &str)
        {
            int result = 0;

            for (int i = static_cast<int>(Items.size()) - 1; i >= 0; --i)
            {
                if (Items[i] && Items[i]->Comp(str) == 0)
                {
                    Items.erase(Items.begin() + i);
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
            const size_t count = Items.size();

            for (size_t i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->Comp(str) == 0)
                    return (static_cast<int>(i));
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
            const size_t count = Items.size();

            for (size_t i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->CaseComp(str) == 0)
                    return (static_cast<int>(i));
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
            const size_t count = Items.size();

            for (size_t i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->Comp(str, cn) == 0)
                    return (static_cast<int>(i));
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
            const size_t count = Items.size();

            for (size_t i = 0; i < count; ++i)
            {
                if (Items[i] && Items[i]->CaseComp(str, cn) == 0)
                    return (static_cast<int>(i));
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
            if (index < static_cast<int>(Items.size()))
                Items.insert(Items.begin() + index, std::make_unique<StringClass>(str));
        }

        /**
        * 交换两个字符串的位置
        * @param index1 第一个字符串的位置
        * @param index2 第二个字符串的位置
        */
        void Exchange(int index1, int index2)                                                        ///<交换两行字符串
        {
            if (index1 < 0 || index2 < 0 || index1 >= static_cast<int>(Items.size()) || index2 >= static_cast<int>(Items.size()))
                return;

            std::swap(Items[static_cast<size_t>(index1)], Items[static_cast<size_t>(index2)]);
        }

        const StringClass &GetString(int n) const
        {
            return *(Items[static_cast<size_t>(n)]);
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
