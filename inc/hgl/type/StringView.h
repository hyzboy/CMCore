#pragma once

#include <hgl/type/StrChar.h>
#include <hgl/Comparator.h>
#include <string>
#include <string_view>

namespace hgl
{
    template<typename T> class String;

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

        explicit StringView(const String<T> &s)
        {
            if (s.IsEmpty()) { ptr = nullptr; length = 0; }
            else { ptr = s.c_str(); length = s.Length(); }
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

        // find/scan helpers (non-owning)
        int FindChar(int pos, const T ch) const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strchr(c_str() + pos, ch);
            return r ? int(r - (c_str() + pos)) : -1;
        }

        int FindChar(const T ch) const { return FindChar(0, ch); }

        int FindString(const SelfClass &str, int start = 0) const
        {
            if (str.Length() <= 0 || start < 0 || start > Length() - str.Length()) return -1;
            const T *r = strstr(c_str() + start, Length() - start, str.c_str(), str.Length());
            return r ? int(r - c_str()) : -1;
        }

        int FindRightChar(const T ch) const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), ch);
            return r ? int(r - c_str()) : -1;
        }

        // comparison
        int Comp(const SelfClass &rhs) const
        {
            if (Length() == 0) return rhs.Length();
            if (rhs.Length() == 0) return Length();
            return hgl::strcmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        int Comp(const T *str) const
        {
            if (!c_str())
            {
                if (!str) return 0;
                else return *str;
            }
            return hgl::strcmp(c_str(), Length(), str, hgl::strlen(str));
        }

        int CaseComp(const SelfClass &rhs) const
        {
            if (Length() == 0) return rhs.Length();
            return hgl::stricmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        int CaseComp(const T *str) const
        {
            if (!c_str())
            {
                if (!str) return 0;
                return *str;
            }
            return hgl::stricmp(c_str(), Length(), str, hgl::strlen(str));
        }

        // compare override for Comparator compatibility
        const int compare(const SelfClass &str) const override { return Comp(str); }

        // utility
        int StatChar(const T ch) const { return ::StatChar(c_str(), ch); }
        int StatLine() const { return ::StatLine(c_str()); }

    };// class StringView

    using AnsiStringView = StringView<char>;
    using U8StringView   = StringView<u8char>;
    using U16StringView  = StringView<u16char>;
    using U32StringView  = StringView<char32_t>;
    using OSStringView   = StringView<os_char>;
    using WideStringView = StringView<wchar_t>;

} // namespace hgl
