/**
 * @file String.h
 * @brief CN: 通用字符串模板类声明文件，基于 std::basic_string 封装，提供丰富的字符串操作接口。
 *        EN: General string template class declaration file, based on std::basic_string, providing rich string manipulation interfaces.
 */
#pragma once

#include <hgl/type/StrChar.h>
#include <hgl/Comparator.h>
#include <string>
#include <initializer_list>
#include <unordered_set>
#include <format>

namespace hgl
{
    template<typename T> class StringView; // forward declaration for interop

    /**
     * @class String
     * @brief CN: 通用字符串模板类
     *        EN: General string template class
     * @tparam T CN: 字符类型，比如 char、wchar_t、u16char 等。
     *           EN: Character type, such as char, wchar_t, u16char, etc.
     *
     * CN: 该类以 std::basic_string<T> 为底层存储，提供额外的便捷方法，同时保持与现有 hgl 字符串工具函数（如 hgl::strlen、hgl::strcmp 等）的兼容。
     * EN: This class uses std::basic_string<T> as underlying storage and provides additional convenient methods, while maintaining compatibility with existing hgl string utility functions (e.g., hgl::strlen, hgl::strcmp).
     */
    template<typename T> class String:public Comparator<String<T>>
    {
    protected:
        /// @brief CN: 用于简化模板类自身类型的引用 EN: Used to simplify references to the template class itself
        using SelfClass = String<T>;

        /// @brief CN: buffer 是底层唯一权威存储，所有字符串数据都存储于此 EN: buffer is the authoritative underlying storage, all string data is stored here
        std::basic_string<T> buffer;

    public:
        /// @brief CN: 当前字符串的字符类型 EN: Character type of the current string
        using CharType = T;

        // 构造/析构 ---------------------------------------------------------
        /** @brief CN: 默认构造函数，创建空字符串 EN: Default constructor, creates an empty string */
        String() = default;
        /** @brief CN: 拷贝构造函数，深拷贝底层 buffer EN: Copy constructor, deep copy of buffer */
        String(const SelfClass &rhs) { buffer = rhs.buffer; }
        /** @brief CN: 从 C 字符串构造（以 null 结尾） EN: Construct from C string (null-terminated) */
        String(const T *str) { fromString(str); }
        /** @brief CN: 从指定长度的字符指针构造（len 可为 -1 表示自动测长） EN: Construct from pointer with specified length (len=-1 means auto length) */
        String(const T *str, int len) { fromString(str, len); }
        /** @brief CN: 按重复字符构造 EN: Construct by repeating character */
        String(size_t count, T ch) { if (count > 0) buffer.assign(count, ch); }
        /** @brief CN: 从初始化列表构造 EN: Construct from initializer list */
        String(std::initializer_list<T> il) { if (il.size()) buffer.assign(il.begin(), il.end()); }
        /// @brief CN: 禁止直接用单个数值类型构造字符串，防止误用 EN: Disable construction from single numeric types to prevent misuse
        String(T) = delete;
        String(int) = delete; 
        String(unsigned int) = delete; 
        String(int64) = delete; 
        String(uint64) = delete; 
        String(float) = delete; 
        String(double) = delete;
        /** @brief CN: 移动构造（显式标记为 explicit） EN: Move constructor (explicit) */
        explicit String(SelfClass &&rhs) noexcept { buffer = std::move(rhs.buffer); }
        /** @brief CN: 从 std::basic_string 构造 EN: Construct from std::basic_string */
        explicit String(const std::basic_string<T> &s) { buffer = s; }
        /** @brief CN: 析构函数 EN: Destructor */
        virtual ~String() = default;

        // 赋值 --------------------------------------------------------------
        /**
         * @brief CN: 移动赋值运算符，支持右值引用 EN: Move assignment operator, supports rvalue references
         * @param rhs CN: 右值引用来源字符串 EN: Source string (rvalue reference)
         * @return CN: 自身引用 EN: Reference to self
         */
        SelfClass &operator=(SelfClass &&rhs) noexcept { if (this != &rhs) { buffer = std::move(rhs.buffer); } return *this; }
        /**
         * @brief CN: 拷贝赋值运算符 EN: Copy assignment operator
         * @param rhs CN: 来源字符串 EN: Source string
         * @return CN: 自身引用 EN: Reference to self
         */
        SelfClass &operator=(const SelfClass &rhs) { if (this != &rhs) buffer = rhs.buffer; return *this; }
        /**
         * @brief CN: 从 std::basic_string 赋值 EN: Assignment from std::basic_string
         * @param s CN: 来源 std::basic_string EN: Source std::basic_string
         * @return CN: 自身引用 EN: Reference to self
         */
        SelfClass &operator=(const std::basic_string<T> &s) { buffer = s; return *this; }
        /**
         * @brief CN: 从初始化列表赋值 EN: Assignment from initializer list
         * @param il CN: 初始化列表 EN: Initializer list
         * @return CN: 自身引用 EN: Reference to self
         */
        SelfClass &operator=(std::initializer_list<T> il) { if (!il.size()) { Clear(); return *this; } buffer.assign(il.begin(), il.end()); return *this; }
        /**
         * @brief CN: 从 C 字符串赋值，保护自赋值 EN: Assignment from C string, protects self-assignment
         * @param str CN: C 字符串指针 EN: C string pointer
         * @return CN: 自身引用 EN: Reference to self
         */
        SelfClass &operator=(const T *str) { if (str != c_str()) fromString(str); return *this; }

        // 静态工厂/转换函数 (集中放置) ---------------------------------------
        /**
         * @brief CN: 创建只包含一个字符的字符串对象 EN: Create a string object containing a single character
         * @param ch CN: 字符 EN: Character
         * @return CN: 包含单个字符的字符串对象 EN: String object containing one character
         */
        static SelfClass charOf(const T &ch) { T tmp[2]; tmp[0] = ch; tmp[1] = 0; return SelfClass(tmp); }

        /** @brief CN: 将 int 转为字符串 EN: Convert int to string */
        static SelfClass numberOf(int value) { T tmp[8 * sizeof(int)]; itos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief CN: 将 unsigned int 转为字符串 EN: Convert unsigned int to string */
        static SelfClass numberOf(uint value) { T tmp[8 * sizeof(uint)]; utos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief CN: 将 int64 转为字符串 EN: Convert int64 to string */
        static SelfClass numberOf(int64 value) { T tmp[8 * sizeof(int64)]; itos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief CN: 将 uint64 转为字符串 EN: Convert uint64 to string */
        static SelfClass numberOf(uint64 value) { T tmp[8 * sizeof(uint64)]; utos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief CN: 将 float 转为字符串（指定小数位） EN: Convert float to string (with fraction digits) */
        static SelfClass floatOf(float value, uint frac) { T tmp[8 * sizeof(float)]; ftos(tmp, sizeof(tmp) / sizeof(T), frac, value); return SelfClass(tmp); }
        /** @brief CN: 将 double 转为字符串（指定小数位） EN: Convert double to string (with fraction digits) */
        static SelfClass floatOf(double value, uint frac) { T tmp[8 * sizeof(double)]; ftos(tmp, sizeof(tmp) / sizeof(T), frac, value); return SelfClass(tmp); }
        /**
         * @brief CN: 计算百分比并格式化为字符串 EN: Calculate percent and format as string
         * @param num CN: 分子 EN: Numerator
         * @param den CN: 分母 EN: Denominator
         * @param frac CN: 小数位 EN: Fraction digits
         * @return CN: 百分比字符串 EN: Percent string
         */
        template<typename N1, typename N2>
        static SelfClass percentOf(const N1 num, const N2 den, uint frac) { return floatOf(double(num) / double(den) * 100.0, frac); }

        /**
        * @brief CN: 将无符号整数转为十六进制字符串
        *        EN: Convert unsigned integer to hexadecimal string
        * @param num CN: 最小字符数 EN: Minimum number of characters
        * @param upper CN: 是否大写 EN: Whether to use uppercase
        */
        static SelfClass hexOf(uint value,int num=0,bool upper=true)
        {
            T tmp[2*(1+sizeof(uint))];

            hgl::htos(tmp,sizeof(tmp) / sizeof(T),value,num,upper);

            return SelfClass(tmp);
        }

        static SelfClass hexOf(uint64 value,int num = 0,bool upper = true)
        {
            T tmp[2 * (1 + sizeof(uint64))];

            hgl::htos(tmp,sizeof(tmp) / sizeof(T),value,num,upper);

            return SelfClass(tmp);
        }

        /**
         * @brief CN: 拼接两个字符序列为一个新字符串 EN: Concatenate two character sequences into a new string
         * @param s1 CN: 第一个字符序列 EN: First character sequence
         * @param l1 CN: 第一个字符序列长度 EN: Length of first sequence
         * @param s2 CN: 第二个字符序列 EN: Second character sequence
         * @param l2 CN: 第二个字符序列长度 EN: Length of second sequence
         * @return CN: 新生成的字符串对象 EN: New string object
         */
        static SelfClass ComboString(const T *s1, int l1, const T *s2, int l2)
        {
            if (!s1 || l1 <= 0) { if (!s2 || l2 <= 0) return SelfClass(); return SelfClass(s2, l2); }
            if (!s2 || l2 <= 0) return SelfClass(s1, l1);
            SelfClass r; r.buffer.reserve(l1 + l2); r.buffer.append(s1, l1); r.buffer.append(s2, l2); return r;
        }

        /** @brief CN: 返回字符长度（不包含末尾的隐含 NUL） EN: Get string length (excluding trailing NUL) */
        const int Length() const { return int(buffer.size()); }
        /** @brief CN: 是否为空字符串 EN: Is string empty */
        const bool IsEmpty() const { return buffer.empty(); }
        /** @brief CN: 获取首字符，空时返回 0 EN: Get first character, returns 0 if empty */
        const T GetFirstChar() const { return buffer.empty() ? T(0) : buffer.front(); }
        /** @brief CN: 获取尾字符，空时返回 0 EN: Get last character, returns 0 if empty */
        const T GetLastChar() const { return buffer.empty() ? T(0) : buffer.back(); }

        /**
         * @brief CN: 获取指向内部连续内存的指针（只读与可写版本） EN: Get pointer to internal contiguous memory (const and non-const)
         * @note CN: 当 buffer 为空时返回 nullptr EN: Returns nullptr if buffer is empty
         * @return CN: 数据指针 EN: Data pointer
         */
        T *c_str() const { return buffer.empty() ? nullptr : const_cast<T*>(buffer.data()); }
        T *c_str() { return buffer.empty() ? nullptr : const_cast<T*>(buffer.data()); }
        /** @brief CN: 返回数据指针的别名 EN: Alias for data pointer */
        T *data_ptr() { return c_str(); }

        /**
         * @brief CN: 从 C 字符串或指定长度的字符缓冲区载入到本对象 EN: Load from C string or buffer with specified length
         * @param str CN: 字符指针 EN: Character pointer
         * @param len CN: 指定长度，-1 表示自动测长 EN: Specified length, -1 means auto length
         * @note CN: 会剔除尾部多余的 NUL 字符 EN: Removes trailing NUL characters
         */
        void fromString(const T *str, int len = -1)
        {
            if (!str || len == 0) { Clear(); return; }
            if (len < 0) buffer.assign(str); else buffer.assign(str, str + hgl::strlen(str, len));
            while (!buffer.empty() && buffer.back() == T(0)) buffer.pop_back();
        }

        /** @brief CN: 将对象从其它地方“解绑”，当前实现总是返回 true（占位） EN: Unlink object, always returns true (placeholder) */
        bool Unlink() { return true; }

        /**
         * @brief CN: 预分配底层容量以避免频繁扩容 EN: Pre-allocate underlying capacity to avoid frequent expansion
         * @param n CN: 期望容量 EN: Expected capacity
         * @return CN: 成功返回 true，失败返回 false EN: Returns true if successful, false otherwise
         */
        bool Reserve(int n) { if (n <= 0) return false; buffer.reserve(size_t(n)); return true; }

        /**
         * @brief CN: 调整字符串大小并返回底层内存指针 EN: Resize string and return underlying pointer
         * @param n CN: 新的字符数（<=0 表示清空并返回 nullptr） EN: New character count (<=0 means clear and return nullptr)
         * @return CN: resize 后的底层数据指针或 nullptr EN: Pointer after resize or nullptr
         */
        T *Resize(int n) { if (n <= 0) { Clear(); return nullptr; } buffer.resize(size_t(n)); return buffer.data(); }

        /** @brief CN: 清空字符串 EN: Clear string */
        void Clear() { buffer.clear(); }

        /**
         * @brief CN: 用指定字符填充区间 EN: Fill region with specified character
         * @param ch CN: 要填充的字符 EN: Character to fill
         * @param start CN: 起始位置 EN: Start position
         * @param len CN: 长度，-1 表示到末尾 EN: Length, -1 means to end
         * @return CN: 成功返回 true，失败返回 false EN: Returns true if successful, false otherwise
         */
        bool FillChar(const T ch, int start = 0, int len = -1)
        {
            if (start < 0 || start > Length()) return false;
            if (len < 0) len = Length() - start;
            if (len <= 0) return false;
            for (int i = 0; i < len; i++) buffer[size_t(start + i)] = ch;
            return true;
        }

        // 修改/插入/删除 ----------------------------------------------------
        /**
         * @brief CN: 在指定位置插入字符序列 EN: Insert character sequence at specified position
         * @param pos CN: 插入位置 EN: Position to insert
         * @param str CN: 字符序列指针 EN: Character sequence pointer
         * @param len CN: 字符序列长度，-1 表示自动测长 EN: Length of character sequence, -1 means auto length
         * @return CN: 成功返回 true，失败返回 false EN: Returns true if successful, false otherwise
         */
        bool Insert(int pos, const T *str, int len = -1)
        {
            if (!str || pos < 0 || pos > Length()) return false;
            if (len == 0) return false;
            if (len < 0) len = hgl::strlen(str);
            if (len <= 0) return false;
            buffer.insert(buffer.begin() + pos, str, str + len);
            return true;
        }
        /** @brief CN: 在 pos 处插入另一个 String 对象 EN: Insert another String object at pos */
        bool Insert(int pos, const SelfClass &s) { return Insert(pos, s.c_str(), s.Length()); }

        /**
         * @brief CN: 删除从 pos 开始的 num 个字符 EN: Delete num characters starting from pos
         * @param pos CN: 开始位置 EN: Start position
         * @param num CN: 删除字符数 EN: Number of characters to delete
         * @return CN: 成功返回 true，失败返回 false EN: Returns true if successful, false otherwise
         */
        bool Delete(int pos, int num)
        {
            if (num <= 0 || pos < 0 || pos >= Length()) return false;
            if (pos + num > Length())
                num = Length() - pos;

            buffer.erase(buffer.begin() + pos, buffer.begin() + pos + num);
            return true;
        }

        // 单字符操作 ---------------------------------------------------------
        /**
         * @brief 获取指定位置的字符
         * CN: 支持越界保护
         * EN: Supports out-of-bounds protection
         */
        bool GetChar(int n, T &ch) const { if (n < 0 || n >= Length()) return false; ch = c_str()[n]; return true; }

        /**
         * @brief 设置指定位置的字符
         * CN: 支持越界保护
         * EN: Supports out-of-bounds protection
         */
        bool SetChar(int n, const T ch)  { if (n < 0 || n >= Length()) return false; buffer[size_t(n)] = ch; return true; }

        // 比较/大小写/数字转换 等 ---------------------------------------------
        /**
         * @brief 全字符串比较（区分大小写）
         * CN: 使用 hgl::strcmp 实现，兼容 C 风格字符串比较
         * EN: Uses hgl::strcmp, compatible with C-style string comparison
         */
        int  Comp(const SelfClass &rhs)                                 const
        {
            if (Length() == 0) return rhs.Length();
            if (rhs.Length() == 0) return Length();
            return hgl::strcmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        /**
         * @brief 与 C 字符串比较（区分大小写）
         * CN: 支持空指针和空字符串处理
         * EN: Supports null pointer and empty string handling
         */
        int  Comp(const T *str)                                         const
        {
            if (!c_str())
            {
                if (!str) return 0;
                else       return *str;
            }

            return hgl::strcmp(c_str(), Length(), str, hgl::strlen(str));
        }

        /** @brief 前 n 个字符比较（区分大小写） */
        int  Comp(const SelfClass &rhs, const int num)                  const { if (num <= 0) return 0; return hgl::strcmp(c_str(), rhs.c_str(), num); }
        /** @brief 与 C 字符串的前 n 个字符比较（区分大小写） */
        int  Comp(const T *str, const int num)                          const { if (num <= 0) return 0; return hgl::strcmp(c_str(), str, num); }

        /**
         * @brief 从指定偏移处比较整个 rhs（区分大小写）
         * CN: 支持偏移比较，适合子串场景
         * EN: Supports offset comparison, suitable for substring scenarios
         */
        int  Comp(const uint pos, const SelfClass &rhs)                 const
        {
            if (pos > (uint)Length()) return 0;
            return hgl::strcmp(c_str() + pos, Length() - pos, rhs.c_str(), rhs.Length());
        }

        /**
         * @brief 从指定偏移处比较 rhs 的前 num 字符（区分大小写）
         * CN: 支持偏移和长度限制
         * EN: Supports offset and length limit
         */
        int  Comp(const uint pos, const SelfClass &rhs, const int num)  const
        {
            if (pos > (uint)Length() || num <= 0) return 0;
            return hgl::strcmp(c_str() + pos, rhs.c_str(), num);
        }

        /** @brief 从指定偏移处比较 C 字符串 */
        int  Comp(const uint pos, const T *str)                         const { if (pos > (uint)Length()) return 0; return hgl::strcmp(c_str() + pos, Length() - pos, str, hgl::strlen(str)); }
        int  Comp(const uint pos, const T *str, const int num)          const { if (pos > (uint)Length() || num <= 0) return 0; return hgl::strcmp(c_str() + pos, str, num); }

        /**
         * @brief 全字符串不区分大小写比较
         * CN: 使用 hgl::stricmp 实现，兼容 C 风格字符串比较
         * EN: Uses hgl::stricmp, compatible with C-style string comparison
         */
        int  CaseComp(const SelfClass &rhs)                         const
        {
            if (Length() == 0) return rhs.Length();
            return hgl::stricmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        /**
         * @brief 与 C 字符串不区分大小写比较
         * CN: 支持空指针和空字符串处理
         * EN: Supports null pointer and empty string handling
         */
        int  CaseComp(const T *str)                                 const
        {
            if (!c_str())
            {
                if (!str) return 0;
                return *str;
            }

            return hgl::stricmp(c_str(), Length(), str, hgl::strlen(str));
        }

        int  CaseComp(const SelfClass &rhs, const int num)          const { if (num <= 0) return 0; return hgl::stricmp(c_str(), Length(), rhs.c_str(), num); }
        int  CaseComp(const T *str, const int num)                  const { if (num <= 0) return 0; return hgl::stricmp(c_str(), Length(), str, num); }
        int  CaseComp(const uint pos, const T *str, const int num)  const { if (num <= 0 || pos > (uint)Length()) return 0; return hgl::stricmp(c_str() + pos, Length() - pos, str, num); }

        // CN: 字符串到布尔/整型/浮点型的转换，底层调用工具函数
        // EN: String to bool/int/float conversion, calls utility functions underneath
                                bool ToBool (bool &result)  const { return stob(c_str(), result); }
        /** @brief 将字符串解析为整型（模板以支持不同整型类型） */
        template<typename I>    bool ToInt  (I &result)     const { return etof(c_str(), result); }
        /** @brief 将字符串解析为无符号整型（模板以支持不同无符号类型） */
        template<typename U>    bool ToUint (U &result)     const { return etof(c_str(), result); }
        /** @brief 将字符串解析为浮点数（模板以支持不同浮点类型） */
        template<typename F>    bool ToFloat(F &result)     const { return etof(c_str(), result); }

        /** @brief 将字符串转换为小写（就地修改） */
        SelfClass &LowerCase    ()      { if (Length()  > 0) { to_lower_char(buffer.data()); } return *this; }
        /** @brief 返回小写拷贝 */
        SelfClass  ToLowerCase  () const{ if (Length() == 0) return SelfClass(); SelfClass tmp(*this); return tmp.LowerCase(); }
        /** @brief 将字符串转换为大写（就地修改） */
        SelfClass &UpperCase    ()      { if (Length()  > 0) { to_upper_char(buffer.data()); } return *this; }
        /** @brief 返回大写拷贝 */
        SelfClass  ToUpperCase  () const{ if (Length() == 0) return SelfClass(); SelfClass tmp(*this); return tmp.UpperCase(); }

    protected:
        // CN: ConvFunc 是用于字符串转换的回调函数类型
        // EN: ConvFunc is a callback function type for string conversion
        typedef const T *(*ConvFunc)(const T *, int &, const bool (*trimfunc)(const T &));

        /**
         * @brief 基于回调函数的通用字符串转换器（用于 Trim 等）
         * CN: 用于实现 TrimLeft/TrimRight/Trim 等功能
         * EN: Used to implement TrimLeft/TrimRight/Trim functions
         */
        SelfClass StrConv(ConvFunc conv) const
        {
            if (Length() == 0) return SelfClass();
            int new_len = Length();
            const T *new_str = conv(c_str(), new_len, is_space<T>);
            if (new_len <= 0) return SelfClass();
            return SelfClass(new_str, new_len);
        }

    public:
        // 修剪/截取 ----------------------------------------------------------
        /** @brief 左侧修剪，返回修剪后的新字符串 */
        SelfClass TrimLeft  () const            { return StrConv(trimleft); }
        /** @brief 右侧修剪，返回修剪后的新字符串 */
        SelfClass TrimRight () const            { return StrConv(trimright); }
        /** @brief 两端修剪，返回修剪后的新字符串 */
        SelfClass Trim      () const            { return StrConv(trim); }

        /** @brief 删除左侧 n 个字符（就地） */
        bool      TrimLeft  (int n)             { return Delete(0, n); }
        /** @brief 删除右侧 n 个字符（就地） */
        bool      TrimRight (int n)             { if (n <= 0 || n > Length()) return false; buffer.erase(buffer.end() - n, buffer.end()); return true; }

        /** @brief 保留左侧 n 个字符（就地），等价于截断至长度 n */
        bool      ClipLeft  (int n)             { if (n < 0) return false; if (n >= Length()) { Clear(); return true; } buffer.resize(size_t(n)); return true; }
        /** @brief 保留右侧 n 个字符（就地） */
        bool      ClipRight (int n)             { if (n < 0 || n > Length()) return false; return Delete(0, Length() - n); }
        /** @brief 在 pos 处保留 num 个字符（就地） */
        bool      Clip      (int pos, int num)  { return Delete(pos, num); }

        /**
         * @brief 获取子串的拷贝
         * CN: 支持自动测长和越界保护
         * EN: Supports auto length detection and out-of-bounds protection
         */
        SelfClass SubString(int start, int n = -1)const
        {
            if (start < 0 || start >= Length())
                return SelfClass();

            if (n < 0)
                n = Length() - start;

            if (n <= 0 || start + n > Length())
                return SelfClass();

            return SelfClass(c_str() + start, n);
        }

        /**
         * @brief 将子串复制到外部 String 对象（避免返回构造）
         * CN: 支持高效子串复制
         * EN: Supports efficient substring copy
         */
        bool      SubString(SelfClass &sc, int start, int n)const
        {
            if (start < 0 || n <= 0 || start + n > Length())
                return false;

            sc.fromString(c_str() + start, n);
            return true;
        }

        /** @brief 统计字符出现次数 */
        int StatChar(const T ch)const { return ::StatChar(c_str(), ch); }
        /** @brief 统计行数 */
        int StatLine()          const { return ::StatLine(c_str()); }

        // 查找/替换 ----------------------------------------------------------
        /**
         * @brief 从指定位置查找单个字符，返回相对于 pos 的偏移，未找到返回 -1
         * CN: 支持从任意位置查找字符
         * EN: Supports searching for a character from any position
         */
        int FindChar(int pos, const T ch)                       const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strchr(c_str() + pos, ch);
            return r ? int(r - (c_str() + pos)) : -1;
        }

        /** @brief 从头查找单字符（FindChar 的快捷） */
        int FindChar(const T ch)                                const { return FindChar(0, ch); }

        /**
         * @brief 从指定位置查找字符集合（按字面序列匹配）
         * CN: 支持查找多个字符中的任意一个
         * EN: Supports searching for any character in a set
         */
        int FindChars(int pos, const SelfClass &ch)             const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strchr(c_str() + pos, ch.c_str(), ch.Length());
            return r ? int(r - (c_str() + pos)) : -1;
        }

        /** @brief 从头查找字符集合（FindChars 的快捷） */
        int FindChars(const SelfClass &ch)                      const { return FindChars(0, ch); }

        /**
         * @brief 查找最后一次出现的单个字符（从右向左）
         * CN: 支持反向查找
         * EN: Supports reverse search
         */
        int FindRightChar(const T ch)                           const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), ch);
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 查找最后一次出现的字符集合（从右向左）
         * CN: 支持反向查找多个字符
         * EN: Supports reverse search for multiple characters
         */
        int FindRightChars(const SelfClass &ch)                 const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), ch.c_str(), ch.Length());
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 从右侧按偏移 off 查找字符（支持指定起始偏移）
         * CN: 支持带偏移的反向查找
         * EN: Supports reverse search with offset
         */
        int FindRightChar(const int off, const T ch)            const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), off, ch);
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 从右侧按偏移 off 查找字符集合（支持指定起始偏移）
         * CN: 支持带偏移的反向查找多个字符
         * EN: Supports reverse search for multiple characters with offset
         */
        int FindRightChar(const int off, const SelfClass &ch)   const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), off, ch.c_str(), ch.Length());
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 查找不在指定字符集合内的字符（从 pos 开始），用于跳过集合中的字符
         * CN: 用于跳过特定字符集合
         * EN: Used to skip specific character sets
         */
        int FindExcludeChar(int pos, const T &ch)               const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strechr(c_str() + pos, ch);
            return r ? int(r - (c_str() + pos)) : -1;
        }

        /**
         * @brief 查找不在指定字符串集合内的字符（从 pos 开始）
         * CN: 用于跳过特定字符串集合
         * EN: Used to skip specific string sets
         */
        int FindExcludeChar(int pos, const SelfClass &ch)       const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strechr(c_str() + pos, ch.c_str(), ch.Length());
            return r ? int(r - (c_str() + pos)) : -1;
        }

        /** @brief 从头查找不在集合内的字符的快捷方法 */
        int FindExcludeChar(const SelfClass &ch)                const { return FindExcludeChar(0, ch); }

        /**
         * @brief 在指定位置开始查找子字符串，返回全局偏移（以 c_str() 为基准），未找到返回 -1
         * CN: 支持高效子串查找
         * EN: Supports efficient substring search
         */
        int FindString(const SelfClass &str, int start = 0)     const
        {
            if (str.Length() <= 0 || start < 0 || start > Length() - str.Length()) return -1;
            const T *r = strstr(c_str() + start, Length() - start, str.c_str(), str.Length());
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 从字符串中移除所有匹配的子串
         * CN: 支持批量移除指定子串
         * EN: Supports batch removal of specified substrings
         */
        int     ClearString (const SelfClass &sub)
        {
            if (sub.IsEmpty() || IsEmpty()) return 0;
            int count = 0;
            size_t pos = 0;
            while ((pos = buffer.find(sub.c_str(), pos, sub.Length())) != std::basic_string<T>::npos)
            {
                buffer.erase(pos, sub.Length());
                ++count;
            }
            return count;
        }

        /**
         * @brief 将给定字符串写入到指定位置（覆盖或扩展）
         * CN: 支持自动扩展和覆盖
         * EN: Supports auto extension and overwrite
         */
        bool    WriteString (int pos, const SelfClass &str)
        {
            if (pos < 0 || pos > Length()) return false;
            if (pos + str.Length() > Length()) buffer.resize(pos + str.Length());
            memcpy(buffer.data() + pos, str.c_str(), str.Length() * sizeof(T));
            return true;
        }

        /**
         * @brief 替换字符（就地），将所有 tch 替换为 sch
         * CN: 支持批量字符替换
         * EN: Supports batch character replacement
         */
        int     Replace     (const T tch, const T sch)
        {
            if (IsEmpty()) return 0;
            int cnt = 0;
            for (auto &c : buffer)
                if (c == tch) { c = sch; ++cnt; }
            return cnt;
        }

        /**
         * @brief 常量索引访问（越界返回静态 0）
         * CN: 支持安全的只读索引访问
         * EN: Supports safe read-only index access
         */
        const T &   operator[](int index) const
        {
            static const T zero_char = 0;
            if (index >= 0 && index < Length()) return c_str()[index];
            return zero_char;
        }

        /**
         * @brief 可写索引访问（越界返回动态分配的 0 引用）
         * CN: 支持安全的可写索引访问
         * EN: Supports safe writable index access
         */
              T &   operator[](int index)
        {
            static T zero_char = * (new T(0));
            if (index < 0 || index >= Length()) return zero_char;
            return buffer[index];
        }

        /**
         * @brief 隐式转换为 T*（指向内部数据）
         * CN: 便于与 C 接口兼容
         * EN: Convenient for compatibility with C interfaces
         */
                    operator T *()
        {
            return c_str();
        }

        /**
         * @brief 隐式转换为 const T*（指向内部数据）
         * CN: 便于与 C 接口兼容
         * EN: Convenient for compatibility with C interfaces
         */
                    operator const T *() const
        {
            return c_str();
        }

        // CN: 支持字符串拼接和追加运算符
        // EN: Supports string concatenation and append operators
        SelfClass &operator+=(const SelfClass &str)         { Insert(Length(), str.c_str(), str.Length()); return *this; }
        SelfClass &operator<<(const SelfClass &str)         { return operator+=(str); }
        SelfClass  operator+(const SelfClass &str) const    { if (str.IsEmpty()) return *this; if (IsEmpty()) return str; return ComboString(c_str(), Length(), str.c_str(), str.Length()); }
        SelfClass  operator+(const T ch)           const    { if (IsEmpty()) return SelfClass::charOf(ch); return ComboString(c_str(), Length(), &ch, 1); }
        SelfClass  operator+(const T *str)         const    { if (IsEmpty()) return SelfClass(str); return ComboString(c_str(), Length(), str, hgl::strlen(str)); }
        // CN: 禁止与数值类型直接拼接，防止误用
        // EN: Disable direct concatenation with numeric types to prevent misuse
        SelfClass  operator+(const int &)          const = delete;
        SelfClass  operator+(const uint &)         const = delete;
        SelfClass  operator+(const int64 &)        const = delete;
        SelfClass  operator+(const uint64 &)       const = delete;
        SelfClass  operator+(const float &)        const = delete;
        SelfClass  operator+(const double &)       const = delete;

        // CN: 重载 compare 方法，便于与 Comparator 基类兼容
        // EN: Override compare method for compatibility with Comparator base class
        const int compare(const SelfClass &str) const override { return Comp(str); }

        /**
         * @brief 返回字符串中不同字符的数量（按字符值不同计数）
         * CN: 统计唯一字符数量，适合分析用途
         * EN: Count unique character values, useful for analysis
         */
        int UniqueCharCount() const
        {
            if (IsEmpty()) return 0;
            std::unordered_set<T> uniq; uniq.reserve(buffer.size());
            for (const auto &ch : buffer) uniq.insert(ch);
            return int(uniq.size());
        }

        /**
         * @brief 追加 C 字符串或指定长度到当前字符串末尾
         * CN: 支持高效追加
         * EN: Supports efficient append
         */
        bool Strcat(const T *str, int len = -1)
        {
            if (!str) return false;
            if (len == 0) return false;
            if (len < 0) len = hgl::strlen(str);
            if (len <= 0) return false;
            return Insert(Length(), str, len);
        }

        /** @brief 追加另一个 String 对象 */
        bool Strcat(const SelfClass &bs)          { return Insert(Length(), bs.c_str(), bs.Length()); }

              std::basic_string<T> &ToStdString()       { return buffer; }
        const std::basic_string<T> &ToStdString() const { return buffer; }

        // new conversions and checks to interoperate with std::string_view
        operator std::basic_string_view<T>() const { if (buffer.empty()) return std::basic_string_view<T>(); return std::basic_string_view<T>(buffer.data(), buffer.size()); }

        // Implicit conversion to StringView for interoperability
        operator StringView<T>() const { return StringView<T>(c_str(), Length()); }

        // PascalCase methods (match StringView naming)
        bool StartsWith(const T ch) const;
        bool StartsWith(const T *s) const;
        bool StartsWith(const SelfClass &s) const;
        bool StartsWith(const StringView<T> &sv) const;

        bool EndsWith(const T ch) const;
        bool EndsWith(const T *s) const;
        bool EndsWith(const SelfClass &s) const;
        bool EndsWith(const StringView<T> &sv) const;

        bool Contains(const T ch) const;
        bool Contains(const T *s) const;
        bool Contains(const SelfClass &s) const;
        bool Contains(const StringView<T> &sv) const;

    };//class String

    /**
     * @typedef AnsiString
     * @brief CN: ANSI 编码字符串类型 EN: ANSI encoded string type
     */
    using AnsiString = String<char>;

    /**
    * @typedef IDString
    * @brief CN: 标识符字符串类型 EN: Identifier string type
    */
    using IDString = String<char>;

    /**
     * @typedef U8String
     * @brief CN: UTF-8 编码字符串类型 EN: UTF-8 encoded string type
     */
    using U8String   = String<u8char>;
    /**
     * @typedef U16String
     * @brief CN: UTF-16 编码字符串类型 EN: UTF-16 encoded string type
     */
    using U16String  = String<u16char>;
    /**
     * @typedef U32String
     * @brief CN: UTF-32 编码字符串类型 EN: UTF-32 encoded string type
     */
    using U32String  = String<char32_t>;
    /**
     * @typedef OSString
     * @brief CN: 操作系统本地编码字符串类型 EN: OS native encoded string type
     */
    using OSString   = String<os_char>;
    /**
     * @typedef WideString
     * @brief CN: 宽字符编码字符串类型 EN: Wide character encoded string type
     */
    using WideString = String<wchar_t>;
    /**
     * @typedef CodeString
     * @brief CN: 默认编码字符串类型 EN: Default encoded string type
     */
    using CodeString = AnsiString;

    // CN: 支持全局 const T* + String<T> 拼接
    // EN: Supports global const T* + String<T> concatenation
    template<typename T>
    inline String<T> operator+(const T *lhs, const String<T> &rhs)
    {
        if (!lhs || *lhs == 0) return rhs;
        if (rhs.IsEmpty()) return String<T>(lhs);
        return String<T>::ComboString(lhs, hgl::strlen(lhs), rhs.c_str(), rhs.Length());
    }

    // CN: 支持 ToNumber 工具函数，便于类型转换
    // EN: Supports ToNumber utility functions for type conversion
    template<typename C> bool ToNumber(const String<C> &str, int &value)    { return str.ToInt(value); }
    template<typename C> bool ToNumber(const String<C> &str, uint &value)   { return str.ToUint(value); }
    template<typename C> bool ToNumber(const String<C> &str, float &value)  { return str.ToFloat(value); }
    template<typename C> bool ToNumber(const String<C> &str, double &value) { return str.ToFloat(value); }

    // CN: 快速哈希函数，适合哈希表等场景
    // EN: Fast hash function, suitable for hash table scenarios
    template<typename T2, int HASH_MAX> uint StringFastHash(const String<T2> &str) { const T2 *p = str.c_str(); int c = str.Length(); uint result = 0; while (c-- > 0) result += *p++; return (result % HASH_MAX); }

    // CN: 支持数值转十六进制字符串
    // EN: Supports conversion of numbers to hexadecimal strings
    template<typename T2, typename I> String<T2> ToHexString(const I &value) { T2 str[(sizeof(I) << 1) + 1]; ToUpperHexStr(str, value); return String<T2>(str); }
    template<typename T2, typename I> String<T2> HexToString(const I &value) { T2 str[(sizeof(I) << 1) + 1]; htos(str, (sizeof(I) << 1) + 1, value); return String<T2>(str); }
    template<typename T2> String<T2> PointerToHexString(const void *ptr)     { return HexToString<T2, HGL_POINTER_UINT>(reinterpret_cast<const HGL_POINTER_UINT>(ptr)); }
    inline String<os_char> PointerToHexOSString(const void *value)           { return PointerToHexString<os_char>(value); }
    inline String<char>    PointerToHexU8String(const void *value)          { return PointerToHexString<char>(value); }
    inline String<u16char> PointerToHexU16String(const void *value)         { return PointerToHexString<u16char>(value); }

    // CN: 支持字符串拷贝和追加工具函数
    // EN: Supports string copy and append utility functions
    template<typename T2> inline void strcpy(T2 *dst, int max_count, const String<T2> &src) { hgl::strcpy(dst, max_count, src.c_str(), src.Length()); }
    template<typename T2> inline void strcat(T2 *dst, int max_count, const String<T2> &src) { hgl::strcat(dst, max_count, src.c_str(), src.Length()); }

    // CN: 支持格式化字符串输出
    // EN: Supports formatted string output
    template<typename T> inline
    int Sprintf(String<T> &str, const T *format, ...)
    {
        if (!format || *format == 0) { str.Clear(); return 0; }
        va_list args;
        va_start(args, format);
        int len = hgl::vsprintf<T>(nullptr, 0, format, args);
        if (len <= 0) { str.Clear(); va_end(args); return 0; }
        str.Resize(len);
        len = hgl::vsprintf<T>(str.c_str(), len + 1, format, args);
        va_end(args);
        return len;
    }

    //C++ 20 format
    // CN: 支持 C++20 标准格式化字符串
    // EN: Supports C++20 standard string formatting
    template<typename T>
    int StrFormat(String<char> &str,const T *format,...)
    {
        va_list args;
        va_start(args, format);
        str=std::vformat(format,std::make_format_args(args));
        va_end(args);

        return str.Length();
    }

} // namespace hgl

// Include StringView implementation and define interop methods that require both types
#include <hgl/type/StringView.h>

namespace hgl
{
    template<typename T>
    bool String<T>::StartsWith(const T ch) const
    {
        if (IsEmpty()) return false;
        return GetFirstChar() == ch;
    }

    template<typename T>
    bool String<T>::StartsWith(const T *s) const
    {
        if (!s || *s == 0) return false;
        int l = hgl::strlen(s);
        if (l > Length()) return false;
        return hgl::strcmp(c_str(), l, s, l) == 0;
    }

    template<typename T>
    bool String<T>::StartsWith(const SelfClass &s) const
    {
        if (s.IsEmpty()) return false;
        if (s.Length() > Length()) return false;
        return hgl::strcmp(c_str(), s.Length(), s.c_str(), s.Length()) == 0;
    }

    template<typename T>
    bool String<T>::StartsWith(const StringView<T> &sv) const
    {
        if (sv.IsEmpty()) return false;
        if (sv.Length() > Length()) return false;
        return hgl::strcmp(c_str(), sv.Length(), sv.c_str(), sv.Length()) == 0;
    }

    template<typename T>
    bool String<T>::EndsWith(const T ch) const
    {
        if (IsEmpty()) return false;
        return GetLastChar() == ch;
    }

    template<typename T>
    bool String<T>::EndsWith(const T *s) const
    {
        if (!s || *s == 0) return false;
        int l = hgl::strlen(s);
        if (l > Length()) return false;
        return hgl::strcmp(c_str() + (Length() - l), l, s, l) == 0;
    }

    template<typename T>
    bool String<T>::EndsWith(const SelfClass &s) const
    {
        if (s.IsEmpty()) return false;
        if (s.Length() > Length()) return false;
        return hgl::strcmp(c_str() + (Length() - s.Length()), s.Length(), s.c_str(), s.Length()) == 0;
    }

    template<typename T>
    bool String<T>::EndsWith(const StringView<T> &sv) const
    {
        if (sv.IsEmpty()) return false;
        if (sv.Length() > Length()) return false;
        return hgl::strcmp(c_str() + (Length() - sv.Length()), sv.Length(), sv.c_str(), sv.Length()) == 0;
    }

    template<typename T>
    bool String<T>::Contains(const T ch) const
    {
        return FindChar(0, ch) >= 0;
    }

    template<typename T>
    bool String<T>::Contains(const T *s) const
    {
        return FindString(SelfClass(s), 0) >= 0;
    }

    template<typename T>
    bool String<T>::Contains(const SelfClass &s) const
    {
        return FindString(s, 0) >= 0;
    }

    template<typename T>
    bool String<T>::Contains(const StringView<T> &sv) const
    {
        if (sv.IsEmpty()) return false;
        return FindString(SelfClass(sv.c_str(), sv.Length()), 0) >= 0;
    }

    // Operators to interoperate with StringView
    template<typename T>
    inline String<T> operator+(const String<T> &a, const StringView<T> &b)
    {
        if (a.IsEmpty()) return String<T>(b.c_str(), b.Length());
        if (b.IsEmpty()) return a;
        return String<T>::ComboString(a.c_str(), a.Length(), b.c_str(), b.Length());
    }

    template<typename T>
    inline String<T> operator+(const StringView<T> &a, const String<T> &b)
    {
        if (a.IsEmpty()) return b;
        if (b.IsEmpty()) return String<T>(a.c_str(), a.Length());
        return String<T>::ComboString(a.c_str(), a.Length(), b.c_str(), b.Length());
    }

    template<typename T>
    inline String<T> &operator+=(String<T> &a, const StringView<T> &b)
    {
        if (b.IsEmpty()) return a;
        a.Insert(a.Length(), b.c_str(), b.Length());
        return a;
    }

} // namespace hgl
