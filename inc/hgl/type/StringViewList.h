#pragma once

#include<hgl/type/StringView.h>
#include<hgl/type/ObjectList.h>
#include<hgl/CharSet.h>

namespace hgl
{
    /**
    * StringViewList 字符串视图列表处理类
    * StringViewList与StringList没有任何关系，它用于高效分割一个大字符串为多行字符串视图进行只读访问。
    */
    template<typename CharT> class StringViewList
    {
        String<CharT> text_string;                                              ///<完整原始文本保存区

        ObjectList<StringView<CharT>> line_string;                              ///<行字符串共享指针列表类型

        StringView<CharT> null_view;

    public:

        using underlying_iterator = typename ObjectList<StringView<CharT>>::Iterator;
        using underlying_const_iterator = typename ObjectList<StringView<CharT>>::ConstIterator;

        // Adapter iterator that yields StringView<CharT>& instead of pointer
        class iterator
        {
            friend class const_iterator;
        public:
            using value_type = StringView<CharT>;
            using pointer = StringView<CharT>*;
            using reference = StringView<CharT>&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::random_access_iterator_tag; // underlying supports random access-like ops

            iterator() = default;
            explicit iterator(const underlying_iterator &u): it(u) {}

            reference operator*() const
            {
                // underlying iterator returns pointer (T*), so dereference that
                return *(*it);
            }

            pointer operator->() const
            {
                return *it; // returns T*
            }

            iterator &operator++() { ++it; return *this; }
            iterator operator++(int) { iterator tmp = *this; ++it; return tmp; }
            iterator &operator--() { --it; return *this; }
            iterator operator--(int) { iterator tmp = *this; --it; return tmp; }

            iterator &operator+=(int v) { it += v; return *this; }
            iterator &operator-=(int v) { it -= v; return *this; }

            iterator operator+(int v) const { iterator t = *this; t += v; return t; }
            iterator operator-(int v) const { iterator t = *this; t -= v; return t; }

            bool operator==(const iterator &o) const { return it == o.it; }
            bool operator!=(const iterator &o) const { return it != o.it; }

        private:
            underlying_iterator it;
        };

        // Const iterator adapter
        class const_iterator
        {
        public:
            using value_type = const StringView<CharT>;
            using pointer = const StringView<CharT>*;
            using reference = const StringView<CharT>&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::random_access_iterator_tag;

            const_iterator() = default;
            explicit const_iterator(const underlying_const_iterator &u): it(u) {}
            // Allow constructing const_iterator from non-const underlying iterator
            const_iterator(const underlying_iterator &u): it(u) {}
            // Implicit conversion from iterator -> const_iterator
            const_iterator(const iterator &o): it(o.it) {}

            reference operator*() const
            {
                return *(*it);
            }

            pointer operator->() const
            {
                return *it;
            }

            const_iterator &operator++() { ++it; return *this; }
            const_iterator operator++(int) { const_iterator tmp = *this; ++it; return tmp; }
            const_iterator &operator--() { --it; return *this; }
            const_iterator operator--(int) { const_iterator tmp = *this; --it; return tmp; }

            const_iterator &operator+=(int v) { it += v; return *this; }
            const_iterator &operator-=(int v) { it -= v; return *this; }

            const_iterator operator+(int v) const { const_iterator t = *this; t += v; return t; }
            const_iterator operator-(int v) const { const_iterator t = *this; t -= v; return t; }

            bool operator==(const const_iterator &o) const { return it == o.it; }
            bool operator!=(const const_iterator &o) const { return it != o.it; }

        private:
            underlying_const_iterator it;
        };

        StringViewList() = default;
        StringViewList(String<CharT>& text)
        {
            FromString(text);
        }

        void FromString(const String<CharT>& text)
        {
            if (text.IsEmpty())
                return;

            text_string = text;

            size_t line_start = 0;
            size_t pos = 0;

            const size_t total_length = text_string.Length();
            CharT *data = text_string.c_str();

            while (pos < total_length)
            {
                if (data[pos] == CharT('\n'))
                {
                    //data[pos] = CharT(0);
                    size_t line_length = pos - line_start;

                    if (line_length > 0 && data[pos - 1] == CharT('\r'))
                    {
                        //data[pos - 1] = CharT(0);
                        --line_length;
                    }

                    line_string.Add(new StringView<CharT>(data + line_start, int(line_length)));

                    line_start = pos + 1;
                }
                ++pos;
            }

            if (line_start < total_length)
            {
                line_string.Add(new StringView<CharT>(data + line_start, int(total_length - line_start)));
            }
        }

        virtual ~StringViewList()=default;

        void Clear()
        {
            text_string.Clear();
            line_string.Clear();
        }

        bool   IsEmpty()const{return line_string.IsEmpty();}                        ///<字符串列表是否为空
        size_t GetCount()const{return line_string.GetCount();}                         ///<取得字符串数量

        // Range-for support: begin / end
        iterator begin() { return iterator(line_string.begin()); }
        iterator end()   { return iterator(line_string.end()); }
        const_iterator begin() const { return const_iterator(line_string.begin()); }
        const_iterator end()   const { return const_iterator(line_string.end()); }

        // cbegin/cend helpers
        const_iterator cbegin() const { return const_iterator(line_string.begin()); }
        const_iterator cend() const { return const_iterator(line_string.end()); }

        const StringView<CharT> GetString(int n)const                          ///<取得指定行字符串
        {
            if(n<0||n>=line_string.GetCount())
                return null_view;

            return(*line_string[n]);
        }

        const StringView<CharT> operator [](int n)const                        ///<取得指定行字符串
        {
            return GetString(n);
        }
    };//template<typename CharT> class StringViewList

    using AnsiStringViewList = StringViewList<char>;
    using WideStringViewList = StringViewList<wchar_t>;
    using U8StringViewList   = StringViewList<u8char>;
    using U16StringViewList  = StringViewList<u16char>;
    //  using U32StringViewList  = StringViewList<u32char>;
    using OSStringViewList   = StringViewList<os_char>;

    int LoadStringViewListFromText(U8StringViewList  &sl, const void* data, const int size, const CharSet& cs = UTF8CharSet);               ///<加载一个原始文本块到AnsiStringViewList
    int LoadStringViewListFromText(U16StringViewList &sl, const void* data, const int size, const CharSet& cs = UTF8CharSet);               ///<加载一个原始文本块到WideStringViewList

    int LoadStringViewListFromTextFile(U8StringViewList  &sl, const OSString &filename, const CharSet& cs = UTF8CharSet);                   ///<加载一个原始文本块到AnsiStringViewList
    int LoadStringViewListFromTextFile(U16StringViewList &sl, const OSString &filename, const CharSet& cs = UTF8CharSet);                   ///<加载一个原始文本块到WideStringViewList

}//namespace hgl
