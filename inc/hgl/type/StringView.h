#pragma once

#include <hgl/type/StrChar.h>
#include <string>
#include <string_view>
#include <unordered_set>

namespace hgl
{
    /**
     * @class StringView
     * @brief CN: 非拥有型字符串视图，基于 std::basic_string_view 实现，提供高效的只读字符串操作接口。
     *        EN: Non-owning string view based on std::basic_string_view that provides
     *            efficient read-only string utilities。
     *
     * @note CN: 注意 StringView 不拥有底层数据，使用时必须保证被引用的数据在视图生命周期内有效。
     *       EN: Note that StringView does not own underlying data; ensure the referenced
     *           data outlives the view.
     *
     * @tparam T CN: 字符类型（例如 char、wchar_t 等）。
     *           EN: Character type (e.g., char, wchar_t).
     */
    template<typename T> class StringView
    {
    protected:
        using SelfClass = StringView<T>;

        std::basic_string_view<T> view; /**< underlying std::string_view compatible storage */

    public:
        using CharType = T;

        /**
         * @brief CN: 默认构造，创建空视图。
         *        EN: Default constructor. Creates an empty view.
         *
         * @return 无
         */
        StringView() = default;

        /**
         * @brief CN: 拷贝构造（浅拷贝视图）。
         *        EN: Copy constructor (shallow copy of the view).
         *
         * @param rhs CN: 源视图引用。
         *            EN: Source view reference.
         * @return 无
         */
        StringView(const SelfClass &rhs) = default;

        /**
         * @brief CN: 从 C 字符串或指定长度的缓冲区构造视图。
         *        EN: Construct from C string or buffer with specified length.
         *
         * @param s CN: 指向字符缓冲区的指针；可为 nullptr 表示空视图。
         *          EN: Pointer to character buffer; may be nullptr to create empty view.
         * @param len CN: 指定长度；-1 表示自动测长。
         *            EN: Length of buffer; -1 means auto-detect length.
         * @return 无
         */
        explicit StringView(const T *s, int len = -1)
        {
            if (!s || len == 0)
            {
                view = std::basic_string_view<T>();

                return;
            }

            if (len < 0)
            {
                len = hgl::strlen(s);
            }
            else
            {
                len = hgl::strlen(s, len);
            }

            if (len <= 0)
            {
                view = std::basic_string_view<T>();

                return;
            }

            view = std::basic_string_view<T>(s, size_t(len));
        }

        /**
         * @brief CN: 从 std::basic_string 构造视图（不拷贝数据）。
         *        EN: Construct from std::basic_string (no data copy).
         *
         * @param s CN: 源字符串引用。
         *          EN: Source std::basic_string reference.
         * @return 无
         */
        explicit StringView(const std::basic_string<T> &s)
        {
            if (s.empty())
            {
                view = std::basic_string_view<T>();
            }
            else
            {
                view = std::basic_string_view<T>(s.data(), s.size());
            }
        }

        /**
         * @brief CN: 从 std::basic_string_view 构造，直接复用其内部 view。
         *        EN: Construct from std::basic_string_view, directly reuse its view.
         *
         * @param sv CN: 源 std::basic_string_view。
         *           EN: Source std::basic_string_view.
         * @return 无
         */
        explicit StringView(const std::basic_string_view<T> &sv)
            : view(sv)
        {
        }

        /**
         * @brief CN: 获取视图长度（字符数）。
         *        EN: Get view length (number of characters).
         *
         * @return CN: 视图长度，空视图返回 0。
         *         EN: Length of the view; returns 0 if empty.
         */
        const int Length() const
        {
            return int(view.size());
        }

        /**
         * @brief CN: 判断视图是否为空。
         *        EN: Check if the view is empty.
         *
         * @return CN: 为空返回 true，否则 false。
         *         EN: Returns true if empty, otherwise false.
         */
        const bool IsEmpty() const
        {
            return view.empty();
        }

        /**
         * @brief CN: 获取首字符，空视图返回 0（不会抛出）。
         *        EN: Get first character; returns 0 for empty view.
         *
         * @return CN: 首字符或 0（若为空）。
         *         EN: First character or 0 if empty.
         */
        const T GetFirstChar() const
        {
            return IsEmpty() ? T(0) : view.front();
        }

        /**
         * @brief CN: 获取末字符，空视图返回 0（不会抛出）。
         *        EN: Get last character; returns 0 for empty view.
         *
         * @return CN: 末字符或 0（若为空）。
         *         EN: Last character or 0 if empty.
         */
        const T GetLastChar()  const
        {
            return IsEmpty() ? T(0) : view.back();
        }

        /**
         * @brief CN: 获取内部连续数据指针（只读）。
         *        EN: Get pointer to contiguous data (read-only).
         *
         * @return CN: 指针或 nullptr（若为空）。
         *         EN: Pointer to data or nullptr if empty.
         */
        const T *c_str() const
        {
            return IsEmpty() ? nullptr : view.data();
        }

        /**
         * @brief CN: 获取数据指针别名 (等同 c_str)。
         *        EN: Alias for c_str().
         *
         * @return CN: 指针或 nullptr（若为空）。
         *         EN: Pointer to data or nullptr if empty.
         */
        const T *data_ptr() const
        {
            return c_str();
        }

        /**
         * @brief CN: 隐式转换为 const T*，等同于 c_str()。
         *        EN: Implicit conversion to const T*, same as c_str().
         *
         * @return CN: 指向数据的指针或 nullptr（若为空）。
         *         EN: Pointer to data or nullptr if empty.
         */
        operator const T *() const
        {
            return c_str();
        }

        /**
         * @brief CN: 隐式转换为 std::basic_string_view<T>，便于与标准库互操作。
         *        EN: Implicit conversion to std::basic_string_view<T> for std interoperability.
         *
         * @return CN: 返回内部 std::basic_string_view<T> 副本。
         *         EN: Returns the internal std::basic_string_view<T>.
         */
        operator std::basic_string_view<T>() const
        {
            return view;
        }

        /**
         * @brief CN: 将视图内容复制为拥有型 std::basic_string<T>。
         *        EN: Copy view content into an owning std::basic_string<T>.
         *
         * @return CN: 拷贝后的 std::basic_string<T>。
         *         EN: Copied std::basic_string<T>.
         */
        std::basic_string<T> ToStdString() const
        {
            return view.empty()
                ? std::basic_string<T>()
                : std::basic_string<T>(view.data(), view.size());
        }

        /**
         * @brief CN: 获取子视图（不修改原视图）。
         *        EN: Obtain a substring view (does not modify the original).
         *
         * @param start CN: 起始位置（0-based）。
         *              EN: Start position (0-based).
         * @param n CN: 长度，-1 表示至末尾。
         *          EN: Length to take; -1 means until the end.
         * @return CN: 返回子视图，参数无效或越界时返回空视图。
         *         EN: Returns the substring view; returns empty view on invalid args or OOB.
         */
        SelfClass SubString(int start, int n = -1) const
        {
            if (start < 0 || start >= Length())
            {
                return SelfClass();
            }

            if (n < 0)
            {
                n = Length() - start;
            }

            if (n <= 0 || start + n > Length())
            {
                return SelfClass();
            }

            return SelfClass(view.substr(size_t(start), size_t(n)));
        }

        /**
         * @brief CN: 将子视图写入输出参数以避免返回构造开销。
         *        EN: Write substring into output parameter to avoid return construction
         *            overhead.
         *
         * @param sc CN: 输出视图引用，用于接收子视图。
         *           EN: Output view reference to receive substring.
         * @param start CN: 起始位置。
         *              EN: Start position.
         * @param n CN: 子串长度。
         *          EN: Length.
         * @return CN: 成功返回 true，参数无效或越界返回 false。
         *         EN: Returns true on success, false if args invalid or OOB.
         */
        bool SubString(SelfClass &sc, int start, int n) const
        {
            if (start < 0 || n <= 0 || start + n > Length())
            {
                return false;
            }

            sc = SelfClass(view.substr(size_t(start), size_t(n)));

            return true;
        }

        /**
         * @brief CN: 获取下标 n 的字符（只读）。
         *        EN: Get character at index n (read-only).
         *
         * @param n CN: 索引（0-based）。
         *          EN: Index (0-based).
         * @param ch CN: 输出参数，接收字符值。
         *           EN: Output parameter to receive the character.
         * @return CN: 成功返回 true，越界返回 false。
         *         EN: Returns true on success, false if out of range.
         */
        bool GetChar(int n, T &ch) const
        {
            if (n < 0 || n >= Length())
            {
                return false;
            }

            ch = view[size_t(n)];

            return true;
        }

        /**
         * @brief CN: 在 pos 位置开始查找字符 ch，返回相对于 pos 的偏移，未找到返回 -1。
         *        EN: Find character ch starting at pos and return offset relative to pos; -1
         *            if not found.
         *
         * @param pos CN: 起始偏移（0-based）。
         *            EN: Start offset (0-based).
         * @param ch CN: 目标字符。
         *           EN: Target character.
         * @return CN: 若找到返回 >=0 的偏移，未找到返回 -1。
         *         EN: Return >=0 offset if found, -1 if not.
         */
        int FindChar(int pos, const T ch) const
        {
            if (pos < 0 || pos >= Length())
            {
                return -1;
            }

            const T *r = hgl::strchr(view.data() + pos, ch);

            return r ? int(r - (view.data() + pos)) : -1;
        }

        /**
         * @brief CN: 从开头查找字符 ch。
         *        EN: Find character ch from beginning.
         *
         * @param ch CN: 目标字符。
         *           EN: Target character.
         * @return CN: 找到返回相对于起始位置的偏移，未找到返回 -1。
         *         EN: Return offset if found, -1 if not.
         */
        int FindChar(const T ch) const
        {
            return FindChar(0, ch);
        }

        /**
         * @brief CN: 查找子视图 ch 在本视图中的首次出现（从 pos 开始）。
         *        EN: Find first occurrence of substring view ch starting at pos.
         *
         * @param pos CN: 起始偏移。
         *            EN: Start offset.
         * @param ch CN: 要查找的子视图。
         *           EN: Substring view to find.
         * @return CN: 找到返回相对偏移，未找到返回 -1。
         *         EN: Return relative offset if found, -1 if not.
         */
        int FindChars(int pos, const SelfClass &ch) const
        {
            if (pos < 0 || pos >= Length())
            {
                return -1;
            }

            const T *r = hgl::strchr(view.data() + pos, ch.c_str(), ch.Length());

            return r ? int(r - (view.data() + pos)) : -1;
        }

        /**
         * @brief CN: 从开头查找子视图 ch。
         *        EN: Find substring from beginning.
         *
         * @param ch CN: 要查找的子视图。
         *           EN: Substring view to find.
         * @return CN: 找到返回相对偏移，未找到返回 -1。
         *         EN: Return relative offset if found, -1 if not.
         */
        int FindChars(const SelfClass &ch) const
        {
            return FindChars(0, ch);
        }

        /**
         * @brief CN: 查找字符在视图中从右向左的最后一次出现。
         *        EN: Find last occurrence of ch in this view.
         *
         * @param ch CN: 目标字符。
         *           EN: Target character.
         * @return CN: 找到返回索引位置，未找到返回 -1。
         *         EN: Return index if found, -1 if not.
         */
        int FindRightChar(const T ch) const
        {
            if (Length() == 0)
            {
                return -1;
            }

            const T *r = hgl::strrchr(view.data(), Length(), ch);

            return r ? int(r - view.data()) : -1;
        }

        /**
         * @brief CN: 查找子视图在视图中从右向左的最后一次出现。
         *        EN: Find last occurrence of substring ch in this view.
         *
         * @param ch CN: 子视图。
         *           EN: Substring to find.
         * @return CN: 找到返回索引位置，未找到返回 -1。
         *         EN: Return index if found, -1 if not.
         */
        int FindRightChars(const SelfClass &ch) const
        {
            if (Length() == 0)
            {
                return -1;
            }

            const T *r = hgl::strrchr(view.data(), Length(), ch.c_str(), ch.Length());

            return r ? int(r - view.data()) : -1;
        }

        /**
         * @brief CN: 查找在偏移 off 之前从右向左的字符 ch。
         *        EN: Find right-most occurrence of ch before offset off.
         *
         * @param off CN: 偏移上限（包含）。
         *            EN: Offset limit (inclusive).
         * @param ch CN: 目标字符。
         *           EN: Target character.
         * @return CN: 找到返回索引位置，未找到返回 -1。
         *         EN: Return index if found, -1 if not.
         */
        int FindRightChar(const int off, const T ch) const
        {
            if (Length() == 0)
            {
                return -1;
            }

            const T *r = hgl::strrchr(view.data(), Length(), off, ch);

            return r ? int(r - view.data()) : -1;
        }

        /**
         * @brief CN: 在偏移 off 之前从右向左查找子视图 ch 的最后一次出现。
         *        EN: Find right-most occurrence of substring ch before offset off.
         *
         * @param off CN: 偏移上限（包含）。
         *            EN: Offset limit (inclusive).
         * @param ch CN: 子视图。
         *           EN: Substring to find.
         * @return CN: 找到返回索引位置，未找到返回 -1。
         *         EN: Return index if found, -1 if not.
         */
        int FindRightChar(const int off, const SelfClass &ch) const
        {
            if (Length() == 0)
            {
                return -1;
            }

            const T *r = hgl::strrchr(view.data(), Length(), off,
                                      ch.c_str(), ch.Length());

            return r ? int(r - view.data()) : -1;
        }

        /**
         * @brief CN: 在 pos 位置开始查找第一个不等于 ch 的字符（排除 ch）。
         *        EN: Find first character that is not equal to ch starting at pos.
         *
         * @param pos CN: 起始位置。
         *            EN: Start position.
         * @param ch CN: 需要排除的字符。
         *           EN: Character to exclude.
         * @return CN: 找到返回相对偏移，未找到返回 -1。
         *         EN: Return relative offset if found, -1 if not.
         */
        int FindExcludeChar(int pos, const T &ch) const
        {
            if (pos < 0 || pos >= Length())
            {
                return -1;
            }

            const T *r = hgl::strechr(view.data() + pos, ch);

            return r ? int(r - (view.data() + pos)) : -1;
        }

        /**
         * @brief CN: 在 pos 位置开始查找第一个不等于 ch 的子视图（排除 ch）。
         *        EN: Find first substring that is not equal to ch starting at pos.
         *
         * @param pos CN: 起始位置。
         *            EN: Start position.
         * @param ch CN: 需要排除的子视图。
         *           EN: Substring to exclude.
         * @return CN: 找到返回相对偏移，未找到返回 -1。
         *         EN: Return relative offset if found, -1 if not.
         */
        int FindExcludeChar(int pos, const SelfClass &ch) const
        {
            if (pos < 0 || pos >= Length())
            {
                return -1;
            }

            const T *r = hgl::strechr(view.data() + pos, ch.c_str(), ch.Length());

            return r ? int(r - (view.data() + pos)) : -1;
        }

        /**
         * @brief CN: 从开头查找第一个不等于 ch 的子视图（排除 ch）。
         *        EN: Find first substring that is not equal to ch from beginning.
         *
         * @param ch CN: 需要排除的子视图。
         *           EN: Substring to exclude.
         * @return CN: 找到返回相对偏移，未找到返回 -1。
         *         EN: Return relative offset if found, -1 if not.
         */
        int FindExcludeChar(const SelfClass &ch) const
        {
            return FindExcludeChar(0, ch);
        }

        /**
         * @brief CN: 在视图中查找另一个视图 str 的首次出现位置，返回相对于本视图开头的偏移，未找到返回 -1。
         *        EN: Find first occurrence of view str in this view; return absolute offset or
         *            -1 if not found.
         *
         * @param str CN: 要查找的子视图。
         *            EN: Substring view to find.
         * @param start CN: 起始搜索位置（默认为 0）。
         *              EN: Start search position (default 0).
         * @return CN: 找到返回全局偏移，未找到返回 -1。
         *         EN: Returns absolute offset if found, -1 if not.
         */
        int FindString(const SelfClass &str, int start = 0) const
        {
            if (str.Length() <= 0 || start < 0 || start > Length() - str.Length())
            {
                return -1;
            }

            const T *r = strstr(view.data() + start, Length() - start,
                                str.c_str(), str.Length());

            return r ? int(r - view.data()) : -1;
        }

        /**
         * @brief CN: 与另一个视图逐字符比较（区分大小写）。
         *        EN: Compare with another view character-wise (case-sensitive).
         *
         * @param rhs CN: 右操作数视图。
         *            EN: Right-hand side view.
         * @return CN: 返回 <0/0/>0 分别表示小于/等于/大于。
         *         EN: Returns <0/0/>0 for less/equal/greater.
         */
        int  Comp(const SelfClass &rhs)                                 const
        {
            if (Length() == 0)
            {
                return rhs.Length();
            }

            if (rhs.Length() == 0)
            {
                return Length();
            }

            return hgl::strcmp(view.data(), Length(), rhs.c_str(), rhs.Length());
        }

        /**
         * @brief CN: 与 C 字符串比较（区分大小写）。
         *        EN: Compare with C string (case-sensitive).
         *
         * @param str CN: C 字符串指针。
         *            EN: C string pointer.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int  Comp(const T *str)                                         const
        {
            if (!c_str())
            {
                if (!str)
                {
                    return 0;
                }

                else
                {
                    return *str;
                }
            }

            return hgl::strcmp(view.data(), Length(), str, hgl::strlen(str));
        }

        /**
         * @brief CN: 与另一个视图比较前 num 个字符。
         *        EN: Compare first num characters with another view.
         *
         * @param rhs CN: 右操作数视图。
         *            EN: Right-hand side view.
         * @param num CN: 比较的字符数。
         *            EN: Number of characters to compare.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int  Comp(const SelfClass &rhs, const int num)                  const
        {
            if (num <= 0)
            {
                return 0;
            }

            return hgl::strcmp(view.data(), rhs.c_str(), num);
        }

        /**
         * @brief CN: 与 C 字符串比较前 num 个字符。
         *        EN: Compare first num characters with C string.
         *
         * @param str CN: C 字符串指针。
         *            EN: C string pointer.
         * @param num CN: 比较的字符数。
         *            EN: Number of characters to compare.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int  Comp(const T *str, const int num)                          const
        {
            if (num <= 0)
            {
                return 0;
            }

            return hgl::strcmp(view.data(), str, num);
        }

        /**
         * @brief CN: 从指定位置 pos 开始，与另一个视图逐字符比较（区分大小写）。
         *        EN: Compare from position pos with another view (case-sensitive).
         *
         * @param pos CN: 起始位置。
         *            EN: Start position.
         * @param rhs CN: 右操作数视图。
         *            EN: Right-hand side view.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int  Comp(const unsigned int pos, const SelfClass &rhs)         const
        {
            if (pos > (unsigned int)Length())
            {
                return 0;
            }

            return hgl::strcmp(view.data() + pos, Length() - pos,
                               rhs.c_str(), rhs.Length());
        }

        /**
         * @brief CN: 从指定位置 pos 开始，与另一个视图比较前 num 个字符。
         *        EN: Compare from position pos with another view for num characters.
         *
         * @param pos CN: 起始位置。
         *            EN: Start position.
         * @param rhs CN: 右操作数视图。
         *            EN: Right-hand side view.
         * @param num CN: 比较的字符数。
         *            EN: Number of characters to compare.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int  Comp(const unsigned int pos, const SelfClass &rhs, const int num)  const
        {
            if (pos > (unsigned int)Length() || num <= 0)
            {
                return 0;
            }

            return hgl::strcmp(view.data() + pos, rhs.c_str(), num);
        }

        /**
         * @brief CN: 从指定位置 pos 开始，与 C 字符串比较（区分大小写）。
         *        EN: Compare from position pos with C string (case-sensitive).
         *
         * @param pos CN: 起始位置。
         *            EN: Start position.
         * @param str CN: C 字符串指针。
         *            EN: C string pointer.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int  Comp(const unsigned int pos, const T *str)                         const
        {
            if (pos > (unsigned int)Length())
            {
                return 0;
            }

            return hgl::strcmp(view.data() + pos, Length() - pos, str,
                               hgl::strlen(str));
        }

        /**
         * @brief CN: 从指定位置 pos 开始，与 C 字符串比较前 num 个字符（区分大小写）。
         *        EN: Compare from position pos with C string for num characters.
         *
         * @param pos CN: 起始位置。
         *            EN: Start position.
         * @param str CN: C 字符串指针。
         *            EN: C string pointer.
         * @param num CN: 比较的字符数。
         *            EN: Number of characters to compare.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int  Comp(const unsigned int pos, const T *str, const int num)          const
        {
            if (pos > (unsigned int)Length() || num <= 0)
            {
                return 0;
            }

            return hgl::strcmp(view.data() + pos, str, num);
        }

        /**
         * @brief CN: 不区分大小写的逐字符比较。
         *        EN: Case-insensitive character-wise comparison.
         *
         * @param rhs CN: 右操作数视图。
         *            EN: Right-hand side view.
         * @return CN: 返回 <0/0/>0 分别表示小于/等于/大于。
         *         EN: Returns <0/0/>0 for less/equal/greater.
         */
        int CaseComp(const SelfClass &rhs)                         const
        {
            if (Length() == 0)
            {
                return rhs.Length();
            }

            return hgl::stricmp(view.data(), Length(), rhs.c_str(), rhs.Length());
        }

        /**
         * @brief CN: 不区分大小写地与 C 字符串比较。
         *        EN: Case-insensitive comparison with C string.
         *
         * @param str CN: C 字符串指针。
         *            EN: C string pointer.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int CaseComp(const T *str)                                 const
        {
            if (!c_str())
            {
                if (!str)
                {
                    return 0;
                }

                return *str;
            }

            return hgl::stricmp(view.data(), Length(), str, hgl::strlen(str));
        }

        /**
         * @brief CN: 不区分大小写地比较前 num 个字符。
         *        EN: Case-insensitive compare first num characters with another view.
         *
         * @param rhs CN: 右操作数视图。
         *            EN: Right-hand side view.
         * @param num CN: 比较的字符数。
         *            EN: Number of characters to compare.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int CaseComp(const SelfClass &rhs, const int num)          const
        {
            if (num <= 0)
            {
                return 0;
            }

            return hgl::stricmp(view.data(), Length(), rhs.c_str(), num);
        }

        /**
         * @brief CN: 不区分大小写地与 C 字符串比较前 num 个字符。
         *        EN: Case-insensitive compare first num characters with C string.
         *
         * @param str CN: C 字符串指针。
         *            EN: C string pointer.
         * @param num CN: 比较的字符数。
         *            EN: Number of characters to compare.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int CaseComp(const T *str, const int num)                  const
        {
            if (num <= 0)
            {
                return 0;
            }

            return hgl::stricmp(view.data(), Length(), str, num);
        }

        /**
         * @brief CN: 从 pos 开始不区分大小写地与 C 字符串比较前 num 个字符。
         *        EN: Case-insensitive compare from pos with C string for num characters.
         *
         * @param pos CN: 起始位置。
         *            EN: Start position.
         * @param str CN: C 字符串指针。
         *            EN: C string pointer.
         * @param num CN: 比较的字符数。
         *            EN: Number of characters to compare.
         * @return CN: 返回比较结果 <0/0/>0。
         *         EN: Comparison result <0/0/>0.
         */
        int CaseComp(const unsigned int pos, const T *str, const int num)  const
        {
            if (num <= 0 || pos > (unsigned int)Length())
            {
                return 0;
            }

            return hgl::stricmp(view.data() + pos, Length() - pos, str, num);
        }

        /**
         * @brief CN: 将视图内容解析为布尔值（使用 hgl::stob）。
         *        EN: Parse view contents to bool using hgl::stob.
         *
         * @param result CN: 输出参数，解析后的布尔值。
         *               EN: Output parameter receiving parsed bool.
         * @return CN: 解析成功返回 true，失败返回 false。
         *         EN: Returns true on success, false on failure.
         */
        bool ToBool (bool &result)  const
        {
            return stob(c_str(), result);
        }

        /**
         * @brief CN: 将视图内容解析为整型（模板）。
         *        EN: Parse view contents to integer (template).
         *
         * @tparam I CN/EN: 整型类型。
         * @param result CN: 输出参数，解析后的整数。
         *               EN: Output parameter receiving parsed integer.
         * @return CN: 解析成功返回 true，失败返回 false。
         *         EN: Returns true on success, false on failure.
         */
        template<typename I>
        bool ToInt  (I &result)     const
        {
            return etof(c_str(), result);
        }

        /**
         * @brief CN: 解析为无符号整型（模板）。
         *        EN: Parse view contents to unsigned integer (template).
         *
         * @tparam U CN/EN: 无符号整型类型。
         * @param result CN: 输出参数，解析后的无符号整数。
         *               EN: Output parameter receiving parsed unsigned integer.
         * @return CN: 解析成功返回 true，失败返回 false。
         *         EN: Returns true on success, false on failure.
         */
        template<typename U>
        bool ToUint (U &result)     const
        {
            return etof(c_str(), result);
        }

        /**
         * @brief CN: 将视图内容解析为浮点数（模板）。
         *        EN: Parse view contents to float (template).
         *
         * @tparam F CN/EN: 浮点类型。
         * @param result CN: 输出参数，解析后的浮点数。
         *               EN: Output parameter receiving parsed float.
         * @return CN: 解析成功返回 true，失败返回 false。
         *         EN: Returns true on success, false on failure.
         */
        template<typename F>
        bool ToFloat(F &result)     const
        {
            return etof(c_str(), result);
        }

        /**
         * @brief CN: 返回小写后的拥有型字符串副本（不修改视图）。
         *        EN: Return an owning lowercase std::basic_string copy (does not modify view).
         *
         * @return CN: 拥有型小写字符串副本。
         *         EN: Owning lowercase std::basic_string copy.
         */
        std::basic_string<T> ToLowerCase() const
        {
            if (IsEmpty())
            {
                return {};
            }

            std::basic_string<T> s(view.data(), view.size());

            to_lower_char(s.data());

            return s;
        }

        /**
         * @brief CN: 返回大写后的拥有型字符串副本（不修改视图）。
         *        EN: Return an owning uppercase std::basic_string copy (does not modify view).
         *
         * @return CN: 拥有型大写字符串副本。
         *         EN: Owning uppercase std::basic_string copy.
         */
        std::basic_string<T> ToUpperCase() const
        {
            if (IsEmpty())
            {
                return {};
            }

            std::basic_string<T> s(view.data(), view.size());

            to_upper_char(s.data());

            return s;
        }

        /**
         * @brief CN: 返回左侧修剪后的新视图（不修改原视图）。
         *        EN: Return a new view with left-side trimmed (does not modify original view).
         *
         * @return CN: 修剪后的新视图。
         *         EN: New trimmed view.
         */
        SelfClass TrimLeft() const
        {
            if (IsEmpty())
            {
                return SelfClass();
            }

            int i = 0;

            while (i < Length() && is_space<T>(view[i]))
            {
                ++i;
            }

            if (i == 0)
            {
                return *this;
            }

            return SelfClass(view.substr(size_t(i), size_t(Length() - i)));
        }

        /**
         * @brief CN: 返回右侧修剪后的新视图（不修改原视图）。
         *        EN: Return a new view with right-side trimmed (does not modify original view).
         *
         * @return CN: 修剪后的新视图。
         *         EN: New trimmed view.
         */
        SelfClass TrimRight() const
        {
            if (IsEmpty())
            {
                return SelfClass();
            }

            int r = Length() - 1;

            while (r >= 0 && is_space<T>(view[r]))
            {
                --r;
            }

            if (r == Length() - 1)
            {
                return *this;
            }

            return SelfClass(view.substr(0, size_t(r + 1)));
        }

        /**
         * @brief CN: 返回两端修剪后的新视图（不修改原视图）。
         *        EN: Return a new view trimmed on both sides (does not modify original view).
         *
         * @return CN: 修剪后的新视图。
         *         EN: New trimmed view.
         */
        SelfClass Trim() const
        {
            if (IsEmpty())
            {
                return SelfClass();
            }

            int l = 0;

            while (l < Length() && is_space<T>(view[l]))
            {
                ++l;
            }

            int r = Length() - 1;

            while (r >= l && is_space<T>(view[r]))
            {
                --r;
            }

            if (l == 0 && r == Length() - 1)
            {
                return *this;
            }

            if (r < l)
            {
                return SelfClass();
            }

            return SelfClass(view.substr(size_t(l), size_t(r - l + 1)));
        }

        /**
         * @brief CN: 返回剔除前 n 个字符后的新视图（不修改原视图）。
         *        EN: Return a new view with the first n characters removed (does not modify
         *            original view).
         *
         * @param n CN: 要剔除的前缀长度。
         *          EN: Number of characters to remove from prefix.
         * @return CN: 剔除后的新视图；若 n<=0 返回原视图，n>=Length() 返回空视图。
         *         EN: New view after removal; if n<=0 returns original view, if n>=Length()
         *             returns empty view.
         */
        SelfClass RemovePrefix(int n) const
        {
            if (n <= 0)
            {
                return *this;
            }

            if (n >= Length())
            {
                return SelfClass();
            }

            return SelfClass(view.substr(size_t(n)));
        }

        /**
         * @brief CN: 返回剔除末尾 n 个字符后的新视图（不修改原视图）。
         *        EN: Return a new view with the last n characters removed (does not modify
         *            original view).
         *
         * @param n CN: 要剔除的后缀长度。
         *          EN: Number of characters to remove from suffix.
         * @return CN: 剔除后的新视图；若 n<=0 返回原视图，n>=Length() 返回空视图。
         *         EN: New view after removal; if n<=0 returns original view, if n>=Length()
         *             returns empty view.
         */
        SelfClass RemoveSuffix(int n) const
        {
            if (n <= 0)
            {
                return *this;
            }

            if (n >= Length())
            {
                return SelfClass();
            }

            return SelfClass(view.substr(0, view.size() - size_t(n)));
        }

        /**
         * @brief CN: 原地从视图前移除 n 个字符（修改本视图）。
         *        EN: Remove first n characters in-place (modifies this view).
         *
         * @param n CN: 要移除的字符数。
         *          EN: Number of characters to remove.
         * @return 无
         */
        void RemovePrefix(int n)
        {
            if (n <= 0 || n >= Length())
            {
                return;
            }

            view = view.substr(size_t(n));
        }

        /**
         * @brief CN: 原地从视图末尾移除 n 个字符（修改本视图）。
         *        EN: Remove last n characters in-place (modifies this view).
         *
         * @param n CN: 要移除的字符数。
         *          EN: Number of characters to remove.
         * @return 无
         */
        void RemoveSuffix(int n)
        {
            if (n <= 0 || n >= Length())
            {
                return;
            }

            view = view.substr(0, view.size() - size_t(n));
        }

        /**
         * @brief CN: 判断视图是否以指定字符/子视图/字符串开头。
         *        EN: Check whether the view starts with given char/substring/string_view.
         *
         * @param ch CN: 目标字符（重载1）。
         *           EN: Target character (overload 1).
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool StartsWith(const T ch) const
        {
            if (IsEmpty())
            {
                return false;
            }

            return GetFirstChar() == ch;
        }

        /**
         * @brief CN: 判断视图是否以指定 C 字符串开头。
         *        EN: Check whether the view starts with given C string.
         *
         * @param s CN: C 字符串指针。
         *          EN: C string pointer.
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool StartsWith(const T *s) const
        {
            if (!s || *s == 0)
            {
                return false;
            }

            int l = hgl::strlen(s);

            if (l > Length())
            {
                return false;
            }

            return hgl::strcmp(view.data(), l, s, l) == 0;
        }

        /**
         * @brief CN: 判断视图是否以另一个 StringView 开头。
         *        EN: Check whether the view starts with another StringView.
         *
         * @param s CN: 目标子视图。
         *          EN: Target substring view.
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool StartsWith(const SelfClass &s) const
        {
            if (s.IsEmpty())
            {
                return false;
            }

            if (s.Length() > Length())
            {
                return false;
            }

            return hgl::strcmp(view.data(), s.Length(), s.c_str(), s.Length()) == 0;
        }

        /**
         * @brief CN: 判断视图是否以 std::basic_string_view 开头。
         *        EN: Check whether the view starts with std::basic_string_view.
         *
         * @param sv CN: 目标 std::basic_string_view。
         *           EN: Target std::basic_string_view.
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool StartsWith(const std::basic_string_view<T> &sv) const
        {
            if (sv.empty())
            {
                return false;
            }

            if ((int)sv.size() > Length())
            {
                return false;
            }

            return hgl::strcmp(view.data(), int(sv.size()), sv.data(), int(sv.size())) == 0;
        }

        /**
         * @brief CN: 判断视图是否以指定字符结尾。
         *        EN: Check whether the view ends with given character.
         *
         * @param ch CN: 目标字符。
         *           EN: Target character.
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool EndsWith(const T ch) const
        {
            if (IsEmpty())
            {
                return false;
            }

            return GetLastChar() == ch;
        }

        /**
         * @brief CN: 判断视图是否以指定 C 字符串结尾。
         *        EN: Check whether the view ends with given C string.
         *
         * @param s CN: C 字符串指针。
         *          EN: C string pointer.
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool EndsWith(const T *s) const
        {
            if (!s || *s == 0)
            {
                return false;
            }

            int l = hgl::strlen(s);

            if (l > Length())
            {
                return false;
            }

            return hgl::strcmp(view.data() + (Length() - l), l, s, l) == 0;
        }

        /**
         * @brief CN: 判断视图是否以另一个 StringView 结尾。
         *        EN: Check whether the view ends with another StringView.
         *
         * @param s CN: 目标子视图。
         *          EN: Target substring view.
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool EndsWith(const SelfClass &s) const
        {
            if (s.IsEmpty())
            {
                return false;
            }

            if (s.Length() > Length())
            {
                return false;
            }

            return hgl::strcmp(view.data() + (Length() - s.Length()),
                               s.Length(), s.c_str(), s.Length()) == 0;
        }

        /**
         * @brief CN: 判断视图是否以 std::basic_string_view 结尾。
         *        EN: Check whether the view ends with std::basic_string_view.
         *
         * @param sv CN: 目标 std::basic_string_view。
         *           EN: Target std::basic_string_view.
         * @return CN: 若匹配返回 true，否则 false。
         *         EN: Returns true if matches, otherwise false.
         */
        bool EndsWith(const std::basic_string_view<T> &sv) const
        {
            if (sv.empty())
            {
                return false;
            }

            if ((int)sv.size() > Length())
            {
                return false;
            }

            return hgl::strcmp(view.data() + (Length() - int(sv.size())),
                               int(sv.size()), sv.data(), int(sv.size())) == 0;
        }

        /**
         * @brief CN: 判断视图中是否包含指定字符。
         *        EN: Check whether the view contains the given character.
         *
         * @param ch CN: 目标字符。
         *           EN: Target character.
         * @return CN: 若包含返回 true，否则 false。
         *         EN: Returns true if contains, otherwise false.
         */
        bool Contains(const T ch) const
        {
            return FindChar(0, ch) >= 0;
        }

        /**
         * @brief CN: 判断视图中是否包含指定 C 字符串。
         *        EN: Check whether the view contains the given C string.
         *
         * @param s CN: C 字符串指针。
         *          EN: C string pointer.
         * @return CN: 若包含返回 true，否则 false。
         *         EN: Returns true if contains, otherwise false.
         */
        bool Contains(const T *s) const
        {
            if (!s || *s == 0)
            {
                return false;
            }

            return FindString(SelfClass(s), 0) >= 0;
        }

        /**
         * @brief CN: 判断视图中是否包含另一个 StringView。
         *        EN: Check whether the view contains the given StringView.
         *
         * @param s CN: 目标子视图。
         *          EN: Target substring view.
         * @return CN: 若包含返回 true，否则 false。
         *         EN: Returns true if contains, otherwise false.
         */
        bool Contains(const SelfClass &s) const
        {
            return FindString(s, 0) >= 0;
        }

        /**
         * @brief CN: 判断视图中是否包含 std::basic_string_view。
         *        EN: Check whether the view contains the given std::basic_string_view.
         *
         * @param sv CN: 目标 std::basic_string_view。
         *           EN: Target std::basic_string_view.
         * @return CN: 若包含返回 true，否则 false。
         *         EN: Returns true if contains, otherwise false.
         */
        bool Contains(const std::basic_string_view<T> &sv) const
        {
            if (sv.empty())
            {
                return false;
            }

            return FindString(SelfClass(sv), 0) >= 0;
        }

        /**
         * @brief CN: 统计指定字符在视图中出现的次数。
         *        EN: Count occurrences of a character in the view.
         *
         * @param ch CN: 目标字符。
         *           EN: Target character.
         * @return CN: 出现次数。
         *         EN: Number of occurrences.
         */
        int StatChar(const T ch) const
        {
            return ::StatChar(c_str(), ch);
        }

        /**
         * @brief CN: 统计视图中的行数（基于换行符）。
         *        EN: Get number of lines in the view (based on newline characters).
         *
         * @return CN: 行数。
         *         EN: Number of lines.
         */
        int StatLine() const
        {
            return ::StatLine(c_str());
        }

        /**
         * @brief CN: 统计视图中不同字符的个数。
         *        EN: Count unique characters in the view.
         *
         * @return CN: 不同字符数量。
         *         EN: Number of unique characters.
         */
        int UniqueCharCount() const
        {
            if (IsEmpty())
            {
                return 0;
            }

            std::unordered_set<T> uniq;

            uniq.reserve(size_t(Length()));

            for (int i = 0; i < Length(); ++i)
            {
                uniq.insert(view[size_t(i)]);
            }

            return int(uniq.size());
        }

        /**
         * @brief CN: 只读索引访问，越界返回静态 0 值引用。
         *        EN: Read-only index access; out-of-range returns a static zero reference.
         *
         * @param index CN: 索引位置（0-based）。
         *              EN: Index position (0-based).
         * @return CN: 对应字符的引用，越界返回静态 0 引用。
         *         EN: Reference to the character or static zero reference when OOB.
         */
        const T & operator[](int index) const
        {
            static const T zero_char = 0;

            if (index >= 0 && index < Length())
            {
                return view[size_t(index)];
            }

            return zero_char;
        }

        /**
         * @brief CN: C++20 三路比较操作符
         *        EN: C++20 three-way comparison operator
         */
        std::strong_ordering operator<=>(const SelfClass &other) const
        {
            int cmp = Comp(other);
            if(cmp < 0) return std::strong_ordering::less;
            if(cmp > 0) return std::strong_ordering::greater;
            return std::strong_ordering::equal;
        }

        /**
         * @brief CN: 相等性比较
         *        EN: Equality comparison
         */
        bool operator==(const SelfClass &other) const
        {
            return Comp(other) == 0;
        }

        /**
         * @brief CN: 与另一个 StringView 的比较运算符 (==)。
         *        EN: Equality operator for StringView.
         */
        friend bool operator==(const SelfClass &a, const SelfClass &b)
        {
            return a.Comp(b) == 0;
        }

        friend bool operator!=(const SelfClass &a, const SelfClass &b)
        {
            return a.Comp(b) != 0;
        }

        friend bool operator<(const SelfClass &a, const SelfClass &b)
        {
            return a.Comp(b) < 0;
        }

        friend bool operator<=(const SelfClass &a, const SelfClass &b)
        {
            return a.Comp(b) <= 0;
        }

        friend bool operator>(const SelfClass &a, const SelfClass &b)
        {
            return a.Comp(b) > 0;
        }

        friend bool operator>=(const SelfClass &a, const SelfClass &b)
        {
            return a.Comp(b) >= 0;
        }

        friend bool operator==(const SelfClass &a, const T *b)
        {
            return a.Comp(b) == 0;
        }

        friend bool operator!=(const SelfClass &a, const T *b)
        {
            return a.Comp(b) != 0;
        }

        friend bool operator==(const T *a, const SelfClass &b)
        {
            return b.Comp(a) == 0;
        }

        friend bool operator!=(const T *a, const SelfClass &b)
        {
            return b.Comp(a) != 0;
        }

        /**
         * @brief CN: 布尔转换：非空视图为 true。
         *        EN: Boolean conversion: non-empty view is true.
         *
         * @return CN: 非空返回 true，空返回 false。
         *         EN: True if view is non-empty, false otherwise.
         */
        explicit operator bool() const
        {
            return !IsEmpty();
        }

    };// class StringView

    using AnsiStringView = StringView<char>;
    using U8StringView   = StringView<u8char>;
    using U16StringView  = StringView<u16char>;
    using U32StringView  = StringView<char32_t>;
    using OSStringView   = StringView<os_char>;
    using WideStringView = StringView<wchar_t>;

} // namespace hgl
