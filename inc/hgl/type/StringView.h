#pragma once

#include <hgl/type/StrChar.h>
#include <hgl/Comparator.h>
#include <string>
#include <string_view>
#include <unordered_set>

namespace hgl
{
    template<typename T>
    class StringView : public Comparator<StringView<T>>
    {
    protected:
        using SelfClass = StringView<T>;

        const T *ptr = nullptr;
        int length = 0;

    public:
        using CharType = T;

        // constructors
        StringView() = default;
        StringView(const SelfClass &rhs) = default;
        explicit StringView(const T *s, int len = -1)
        {
            if (!s || len == 0) { ptr = nullptr; length = 0; return; }

            if (len < 0)
            {
                length = hgl::strlen(s);
            }
            else
            {
                length = hgl::strlen(s, len);
            }

            if (length <= 0) { ptr = nullptr; length = 0; }
            else ptr = s;
        }

        explicit StringView(const std::basic_string<T> &s)
        {
            if (s.empty()) { ptr = nullptr; length = 0; }
            else { ptr = s.data(); length = int(s.size()); }
        }

        // observers
        const int Length() const { return length; }
        const bool IsEmpty() const { return length == 0; }
        const T GetFirstChar() const { return IsEmpty() ? T(0) : ptr[0]; }
        const T GetLastChar()  const { return IsEmpty() ? T(0) : ptr[length - 1]; }

        const T *c_str() const { return IsEmpty() ? nullptr : ptr; }
        const T *data_ptr() const { return c_str(); }

        // conversions
        operator const T *() const { return c_str(); }
        operator std::basic_string_view<T>() const { if (IsEmpty()) return std::basic_string_view<T>(); return std::basic_string_view<T>(ptr, size_t(length)); }

        // conversion to owning std::basic_string
        std::basic_string<T> ToStdString() const { return IsEmpty() ? std::basic_string<T>() : std::basic_string<T>(ptr, ptr + length); }

        // substring/view creation
        SelfClass SubString(int start, int n = -1) const
        {
            if (start < 0 || start >= Length()) return SelfClass();
            if (n < 0) n = Length() - start;
            if (n <= 0 || start + n > Length()) return SelfClass();
            return SelfClass(ptr + start, n);
        }

        bool SubString(SelfClass &sc, int start, int n) const
        {
            if (start < 0 || n <= 0 || start + n > Length()) return false;
            sc = SelfClass(ptr + start, n);
            return true;
        }

        // get char (read-only)
        bool GetChar(int n, T &ch) const { if (n < 0 || n >= Length()) return false; ch = c_str()[n]; return true; }

        // find/scan helpers (non-owning)
        int FindChar(int pos, const T ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strchr(c_str() + pos, ch);
            return r ? int(r - (c_str() + pos)) : -1;
        }

        int FindChar(const T ch) const { return FindChar(0, ch); }

        int FindChars(int pos, const SelfClass &ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strchr(c_str() + pos, ch.c_str(), ch.Length());
            return r ? int(r - (c_str() + pos)) : -1;
        }

        int FindChars(const SelfClass &ch) const { return FindChars(0, ch); }

        int FindRightChar(const T ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), ch);
            return r ? int(r - c_str()) : -1;
        }

        int FindRightChars(const SelfClass &ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), ch.c_str(), ch.Length());
            return r ? int(r - c_str()) : -1;
        }

        int FindRightChar(const int off, const T ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), off, ch);
            return r ? int(r - c_str()) : -1;
        }

        int FindRightChar(const int off, const SelfClass &ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), off, ch.c_str(), ch.Length());
            return r ? int(r - c_str()) : -1;
        }

        int FindExcludeChar(int pos, const T &ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strechr(c_str() + pos, ch);
            return r ? int(r - (c_str() + pos)) : -1;
        }

        int FindExcludeChar(int pos, const SelfClass &ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strechr(c_str() + pos, ch.c_str(), ch.Length());
            return r ? int(r - (c_str() + pos)) : -1;
        }

        int FindExcludeChar(const SelfClass &ch) const { return FindExcludeChar(0, ch); }

        int FindString(const SelfClass &str, int start = 0) const
        {
            if (str.Length() <= 0 || start < 0 || start > Length() - str.Length()) return -1;
            const T *r = strstr(c_str() + start, Length() - start, str.c_str(), str.Length());
            return r ? int(r - c_str()) : -1;
        }

        // comparison
        int  Comp(const SelfClass &rhs)                                 const
        {
            if (Length() == 0) return rhs.Length();
            if (rhs.Length() == 0) return Length();
            return hgl::strcmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        int  Comp(const T *str)                                         const
        {
            if (!c_str())
            {
                if (!str) return 0;
                else       return *str;
            }

            return hgl::strcmp(c_str(), Length(), str, hgl::strlen(str));
        }

        int  Comp(const SelfClass &rhs, const int num)                  const { if (num <= 0) return 0; return hgl::strcmp(c_str(), rhs.c_str(), num); }
        int  Comp(const T *str, const int num)                          const { if (num <= 0) return 0; return hgl::strcmp(c_str(), str, num); }

        int  Comp(const unsigned int pos, const SelfClass &rhs)         const
        {
            if (pos > (unsigned int)Length()) return 0;
            return hgl::strcmp(c_str() + pos, Length() - pos, rhs.c_str(), rhs.Length());
        }

        int  Comp(const unsigned int pos, const SelfClass &rhs, const int num)  const
        {
            if (pos > (unsigned int)Length() || num <= 0) return 0;
            return hgl::strcmp(c_str() + pos, rhs.c_str(), num);
        }

        int  Comp(const unsigned int pos, const T *str)                         const { if (pos > (unsigned int)Length()) return 0; return hgl::strcmp(c_str() + pos, Length() - pos, str, hgl::strlen(str)); }
        int  Comp(const unsigned int pos, const T *str, const int num)          const { if (pos > (unsigned int)Length() || num <= 0) return 0; return hgl::strcmp(c_str() + pos, str, num); }

        int CaseComp(const SelfClass &rhs)                         const
        {
            if (Length() == 0) return rhs.Length();
            return hgl::stricmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        int CaseComp(const T *str)                                 const
        {
            if (!c_str())
            {
                if (!str) return 0;
                return *str;
            }

            return hgl::stricmp(c_str(), Length(), str, hgl::strlen(str));
        }

        int CaseComp(const SelfClass &rhs, const int num)          const { if (num <= 0) return 0; return hgl::stricmp(c_str(), Length(), rhs.c_str(), num); }
        int CaseComp(const T *str, const int num)                  const { if (num <= 0) return 0; return hgl::stricmp(c_str(), Length(), str, num); }
        int CaseComp(const unsigned int pos, const T *str, const int num)  const { if (num <= 0 || pos > (unsigned int)Length()) return 0; return hgl::stricmp(c_str() + pos, Length() - pos, str, num); }

        // CN: String to bool/int/float conversion
        bool ToBool (bool &result)  const { return stob(c_str(), result); }
        template<typename I>    bool ToInt  (I &result)     const { return etof(c_str(), result); }
        template<typename U>    bool ToUint (U &result)     const { return etof(c_str(), result); }
        template<typename F>    bool ToFloat(F &result)     const { return etof(c_str(), result); }

        // case conversions returning owning std::basic_string (non-modifying for view)
        std::basic_string<T> ToLowerCase() const { if (IsEmpty()) return {}; std::basic_string<T> s(ptr, ptr + length); to_lower_char(s.data()); return s; }
        std::basic_string<T> ToUpperCase() const { if (IsEmpty()) return {}; std::basic_string<T> s(ptr, ptr + length); to_upper_char(s.data()); return s; }

        // trim returning view
        SelfClass TrimLeft() const
        {
            if (IsEmpty()) return SelfClass();
            int i = 0; while (i < length && is_space<T>(ptr[i])) ++i;
            if (i == 0) return *this;
            return SelfClass(ptr + i, length - i);
        }

        SelfClass TrimRight() const
        {
            if (IsEmpty()) return SelfClass();
            int r = length - 1; while (r >= 0 && is_space<T>(ptr[r])) --r;
            if (r == length - 1) return *this;
            return SelfClass(ptr, r + 1);
        }

        SelfClass Trim() const
        {
            if (IsEmpty()) return SelfClass();
            int l = 0; while (l < length && is_space<T>(ptr[l])) ++l;
            int r = length - 1; while (r >= l && is_space<T>(ptr[r])) --r;
            if (l == 0 && r == length - 1) return *this;
            if (r < l) return SelfClass();
            return SelfClass(ptr + l, r - l + 1);
        }

        // utility
        int StatChar(const T ch) const { return ::StatChar(c_str(), ch); }
        int StatLine() const { return ::StatLine(c_str()); }

        int UniqueCharCount() const
        {
            if (IsEmpty()) return 0;
            std::unordered_set<T> uniq; uniq.reserve(size_t(length));
            for (int i = 0; i < length; ++i) uniq.insert(ptr[i]);
            return int(uniq.size());
        }

        // read-only operator[]
        const T & operator[](int index) const
        {
            static const T zero_char = 0;
            if (index >= 0 && index < Length()) return c_str()[index];
            return zero_char;
        }

        // compare override for Comparator compatibility
        const int compare(const SelfClass &str) const override { return Comp(str); }

        // comparison operators with StringView and C string
        friend bool operator==(const SelfClass &a, const SelfClass &b) { return a.Comp(b) == 0; }
        friend bool operator!=(const SelfClass &a, const SelfClass &b) { return a.Comp(b) != 0; }
        friend bool operator<(const SelfClass &a, const SelfClass &b)  { return a.Comp(b) < 0; }
        friend bool operator<=(const SelfClass &a, const SelfClass &b) { return a.Comp(b) <= 0; }
        friend bool operator>(const SelfClass &a, const SelfClass &b)  { return a.Comp(b) > 0; }
        friend bool operator>=(const SelfClass &a, const SelfClass &b) { return a.Comp(b) >= 0; }

        friend bool operator==(const SelfClass &a, const T *b) { return a.Comp(b) == 0; }
        friend bool operator!=(const SelfClass &a, const T *b) { return a.Comp(b) != 0; }
        friend bool operator==(const T *a, const SelfClass &b) { return b.Comp(a) == 0; }
        friend bool operator!=(const T *a, const SelfClass &b) { return b.Comp(a) != 0; }

        // boolean conversion
        explicit operator bool() const { return !IsEmpty(); }

    };// class StringView

    using AnsiStringView = StringView<char>;
    using U8StringView   = StringView<u8char>;
    using U16StringView  = StringView<u16char>;
    using U32StringView  = StringView<char32_t>;
    using OSStringView   = StringView<os_char>;
    using WideStringView = StringView<wchar_t>;

} // namespace hgl
