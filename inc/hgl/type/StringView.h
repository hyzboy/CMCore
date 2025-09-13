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

        std::basic_string_view<T> view; // underlying std::string_view compatible storage

    public:
        using CharType = T;

        // constructors
        StringView() = default;
        StringView(const SelfClass &rhs) = default;
        explicit StringView(const T *s, int len = -1)
        {
            if (!s || len == 0) { view = std::basic_string_view<T>(); return; }

            if (len < 0)
                len = hgl::strlen(s);
            else
                len = hgl::strlen(s, len);

            if (len <= 0) { view = std::basic_string_view<T>(); return; }
            view = std::basic_string_view<T>(s, size_t(len));
        }

        explicit StringView(const std::basic_string<T> &s)
        {
            if (s.empty()) view = std::basic_string_view<T>();
            else view = std::basic_string_view<T>(s.data(), s.size());
        }

        explicit StringView(const std::basic_string_view<T> &sv) : view(sv) {}

        // observers
        const int Length() const { return int(view.size()); }
        const bool IsEmpty() const { return view.empty(); }
        const T GetFirstChar() const { return IsEmpty() ? T(0) : view.front(); }
        const T GetLastChar()  const { return IsEmpty() ? T(0) : view.back(); }

        const T *c_str() const { return IsEmpty() ? nullptr : view.data(); }
        const T *data_ptr() const { return c_str(); }

        // conversions
        operator const T *() const { return c_str(); }
        operator std::basic_string_view<T>() const { return view; }

        // conversion to owning std::basic_string
        std::basic_string<T> ToStdString() const { return view.empty() ? std::basic_string<T>() : std::basic_string<T>(view.data(), view.size()); }

        // substring/view creation
        SelfClass SubString(int start, int n = -1) const
        {
            if (start < 0 || start >= Length()) return SelfClass();
            if (n < 0) n = Length() - start;
            if (n <= 0 || start + n > Length()) return SelfClass();
            return SelfClass(view.substr(size_t(start), size_t(n)));
        }

        bool SubString(SelfClass &sc, int start, int n) const
        {
            if (start < 0 || n <= 0 || start + n > Length()) return false;
            sc = SelfClass(view.substr(size_t(start), size_t(n)));
            return true;
        }

        // get char (read-only)
        bool GetChar(int n, T &ch) const { if (n < 0 || n >= Length()) return false; ch = view[size_t(n)]; return true; }

        // find/scan helpers (non-owning)
        int FindChar(int pos, const T ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strchr(view.data() + pos, ch);
            return r ? int(r - (view.data() + pos)) : -1;
        }

        int FindChar(const T ch) const { return FindChar(0, ch); }

        int FindChars(int pos, const SelfClass &ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strchr(view.data() + pos, ch.c_str(), ch.Length());
            return r ? int(r - (view.data() + pos)) : -1;
        }

        int FindChars(const SelfClass &ch) const { return FindChars(0, ch); }

        int FindRightChar(const T ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(view.data(), Length(), ch);
            return r ? int(r - view.data()) : -1;
        }

        int FindRightChars(const SelfClass &ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(view.data(), Length(), ch.c_str(), ch.Length());
            return r ? int(r - view.data()) : -1;
        }

        int FindRightChar(const int off, const T ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(view.data(), Length(), off, ch);
            return r ? int(r - view.data()) : -1;
        }

        int FindRightChar(const int off, const SelfClass &ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(view.data(), Length(), off, ch.c_str(), ch.Length());
            return r ? int(r - view.data()) : -1;
        }

        int FindExcludeChar(int pos, const T &ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strechr(view.data() + pos, ch);
            return r ? int(r - (view.data() + pos)) : -1;
        }

        int FindExcludeChar(int pos, const SelfClass &ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strechr(view.data() + pos, ch.c_str(), ch.Length());
            return r ? int(r - (view.data() + pos)) : -1;
        }

        int FindExcludeChar(const SelfClass &ch) const { return FindExcludeChar(0, ch); }

        int FindString(const SelfClass &str, int start = 0) const
        {
            if (str.Length() <= 0 || start < 0 || start > Length() - str.Length()) return -1;
            const T *r = strstr(view.data() + start, Length() - start, str.c_str(), str.Length());
            return r ? int(r - view.data()) : -1;
        }

        // comparison
        int  Comp(const SelfClass &rhs)                                 const
        {
            if (Length() == 0) return rhs.Length();
            if (rhs.Length() == 0) return Length();
            return hgl::strcmp(view.data(), Length(), rhs.c_str(), rhs.Length());
        }

        int  Comp(const T *str)                                         const
        {
            if (!c_str())
            {
                if (!str) return 0;
                else       return *str;
            }

            return hgl::strcmp(view.data(), Length(), str, hgl::strlen(str));
        }

        int  Comp(const SelfClass &rhs, const int num)                  const { if (num <= 0) return 0; return hgl::strcmp(view.data(), rhs.c_str(), num); }
        int  Comp(const T *str, const int num)                          const { if (num <= 0) return 0; return hgl::strcmp(view.data(), str, num); }

        int  Comp(const unsigned int pos, const SelfClass &rhs)         const
        {
            if (pos > (unsigned int)Length()) return 0;
            return hgl::strcmp(view.data() + pos, Length() - pos, rhs.c_str(), rhs.Length());
        }

        int  Comp(const unsigned int pos, const SelfClass &rhs, const int num)  const
        {
            if (pos > (unsigned int)Length() || num <= 0) return 0;
            return hgl::strcmp(view.data() + pos, rhs.c_str(), num);
        }

        int  Comp(const unsigned int pos, const T *str)                         const { if (pos > (unsigned int)Length()) return 0; return hgl::strcmp(view.data() + pos, Length() - pos, str, hgl::strlen(str)); }
        int  Comp(const unsigned int pos, const T *str, const int num)          const { if (pos > (unsigned int)Length() || num <= 0) return 0; return hgl::strcmp(view.data() + pos, str, num); }

        int CaseComp(const SelfClass &rhs)                         const
        {
            if (Length() == 0) return rhs.Length();
            return hgl::stricmp(view.data(), Length(), rhs.c_str(), rhs.Length());
        }

        int CaseComp(const T *str)                                 const
        {
            if (!c_str())
            {
                if (!str) return 0;
                return *str;
            }

            return hgl::stricmp(view.data(), Length(), str, hgl::strlen(str));
        }

        int CaseComp(const SelfClass &rhs, const int num)          const { if (num <= 0) return 0; return hgl::stricmp(view.data(), Length(), rhs.c_str(), num); }
        int CaseComp(const T *str, const int num)                  const { if (num <= 0) return 0; return hgl::stricmp(view.data(), Length(), str, num); }
        int CaseComp(const unsigned int pos, const T *str, const int num)  const { if (num <= 0 || pos > (unsigned int)Length()) return 0; return hgl::stricmp(view.data() + pos, Length() - pos, str, num); }

        // CN: String to bool/int/float conversion
        bool ToBool (bool &result)  const { return stob(c_str(), result); }
        template<typename I>    bool ToInt  (I &result)     const { return etof(c_str(), result); }
        template<typename U>    bool ToUint (U &result)     const { return etof(c_str(), result); }
        template<typename F>    bool ToFloat(F &result)     const { return etof(c_str(), result); }

        // case conversions returning owning std::basic_string (non-modifying for view)
        std::basic_string<T> ToLowerCase() const { if (IsEmpty()) return {}; std::basic_string<T> s(view.data(), view.size()); to_lower_char(s.data()); return s; }
        std::basic_string<T> ToUpperCase() const { if (IsEmpty()) return {}; std::basic_string<T> s(view.data(), view.size()); to_upper_char(s.data()); return s; }

        // trim returning view
        SelfClass TrimLeft() const
        {
            if (IsEmpty()) return SelfClass();
            int i = 0; while (i < Length() && is_space<T>(view[i])) ++i;
            if (i == 0) return *this;
            return SelfClass(view.substr(size_t(i), size_t(Length()-i)));
        }

        SelfClass TrimRight() const
        {
            if (IsEmpty()) return SelfClass();
            int r = Length() - 1; while (r >= 0 && is_space<T>(view[r])) --r;
            if (r == Length() - 1) return *this;
            return SelfClass(view.substr(0, size_t(r+1)));
        }

        SelfClass Trim() const
        {
            if (IsEmpty()) return SelfClass();
            int l = 0; while (l < Length() && is_space<T>(view[l])) ++l;
            int r = Length() - 1; while (r >= l && is_space<T>(view[r])) --r;
            if (l == 0 && r == Length() - 1) return *this;
            if (r < l) return SelfClass();
            return SelfClass(view.substr(size_t(l), size_t(r - l + 1)));
        }

        // std::string_view-style modifiers returning a new view (PascalCase)
        SelfClass RemovePrefix(int n) const
        {
            if (n <= 0) return *this;
            if (n >= Length()) return SelfClass();
            return SelfClass(view.substr(size_t(n)));
        }

        SelfClass RemoveSuffix(int n) const
        {
            if (n <= 0) return *this;
            if (n >= Length()) return SelfClass();
            return SelfClass(view.substr(0, view.size() - size_t(n)));
        }

        void RemovePrefix(int n)
        {
            if (n <= 0 || n >= Length()) return;
            view = view.substr(size_t(n));
        }

        void RemoveSuffix(int n)
        {
            if (n <= 0 || n >= Length()) return;
            view = view.substr(0, view.size() - size_t(n));
        }

        // StartsWith/EndsWith/Contains (PascalCase names)
        bool StartsWith(const T ch) const { if (IsEmpty()) return false; return GetFirstChar() == ch; }
        bool StartsWith(const T *s) const { if (!s || *s == 0) return false; int l = hgl::strlen(s); if (l > Length()) return false; return hgl::strcmp(view.data(), l, s, l) == 0; }
        bool StartsWith(const SelfClass &s) const { if (s.IsEmpty()) return false; if (s.Length() > Length()) return false; return hgl::strcmp(view.data(), s.Length(), s.c_str(), s.Length()) == 0; }
        bool StartsWith(const std::basic_string_view<T> &sv) const { if (sv.empty()) return false; if ((int)sv.size() > Length()) return false; return hgl::strcmp(view.data(), int(sv.size()), sv.data(), int(sv.size())) == 0; }

        bool EndsWith(const T ch) const { if (IsEmpty()) return false; return GetLastChar() == ch; }
        bool EndsWith(const T *s) const { if (!s || *s == 0) return false; int l = hgl::strlen(s); if (l > Length()) return false; return hgl::strcmp(view.data() + (Length() - l), l, s, l) == 0; }
        bool EndsWith(const SelfClass &s) const { if (s.IsEmpty()) return false; if (s.Length() > Length()) return false; return hgl::strcmp(view.data() + (Length() - s.Length()), s.Length(), s.c_str(), s.Length()) == 0; }
        bool EndsWith(const std::basic_string_view<T> &sv) const { if (sv.empty()) return false; if ((int)sv.size() > Length()) return false; return hgl::strcmp(view.data() + (Length() - int(sv.size())), int(sv.size()), sv.data(), int(sv.size())) == 0; }

        bool Contains(const T ch) const { return FindChar(0, ch) >= 0; }
        bool Contains(const T *s) const { if (!s || *s == 0) return false; return FindString(SelfClass(s), 0) >= 0; }
        bool Contains(const SelfClass &s) const { return FindString(s, 0) >= 0; }
        bool Contains(const std::basic_string_view<T> &sv) const { if (sv.empty()) return false; return FindString(SelfClass(sv), 0) >= 0; }

        // utility
        int StatChar(const T ch) const { return ::StatChar(c_str(), ch); }
        int StatLine() const { return ::StatLine(c_str()); }

        int UniqueCharCount() const
        {
            if (IsEmpty()) return 0;
            std::unordered_set<T> uniq; uniq.reserve(size_t(Length()));
            for (int i = 0; i < Length(); ++i) uniq.insert(view[size_t(i)]);
            return int(uniq.size());
        }

        // read-only operator[]
        const T & operator[](int index) const
        {
            static const T zero_char = 0;
            if (index >= 0 && index < Length()) return view[size_t(index)];
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
