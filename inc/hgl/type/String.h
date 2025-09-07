#pragma once

#include <hgl/type/DataType.h>
#include <hgl/type/StrChar.h>
#include <hgl/Comparator.h>
#include <string>
#include <initializer_list>
#include <unordered_set>

/**
 * @file String.h
 * @brief 通用字符串模板类声明
 *
 * 本文件定义了模板类 hgl::String<T>，基于 std::basic_string<T> 封装
 * 出一套便捷的字符串操作接口，包含构造、赋值、比较、查找、裁剪、
 * 大小写转换、数字转换等功能。所有注释使用 Doxygen 风格，以便
 * 生成文档。
 */

namespace hgl
{
    /**
     * @brief 通用字符串模板类
     *
     * @tparam T 字符类型，比如 char、wchar_t、u16char 等。
     *
     * 该类以 std::basic_string<T> 为底层存储，提供额外的便捷方法，
     * 同时保持与现有 hgl 字符串工具函数（如 hgl::strlen、hgl::strcmp 等）的兼容。
     */
    template<typename T> class String:public Comparator<String<T>>
    {
    protected:

        using SelfClass = String<T>;

        std::basic_string<T> buffer;                  // 唯一权威存储

    public:

        using CharType = T;

        // 构造/析构 ---------------------------------------------------------
        /** @brief 默认构造函数，创建空字符串 */
        String()                              = default;
        /** @brief 拷贝构造函数，深拷贝底层 buffer */
        String(const SelfClass &rhs)          { buffer = rhs.buffer; }
        /** @brief 从 C 字符串构造（以 null 结尾） */
        String(const T *str)                  { fromString(str); }
        /** @brief 从指定长度的字符指针构造（len 可为 -1 表示自动测长） */
        String(const T *str, int len)         { fromString(str, len); }
        /** @brief 按重复字符构造 */
        String(size_t count, T ch)            { if (count > 0) buffer.assign(count, ch); }
        /** @brief 从初始化列表构造 */
        String(std::initializer_list<T> il)   { if (il.size()) buffer.assign(il.begin(), il.end()); }
        String(T)                             = delete;
        String(int)                           = delete; 
        String(unsigned int)                  = delete; 
        String(int64)                         = delete; 
        String(uint64)                        = delete; 
        String(float)                         = delete; 
        String(double)                        = delete;

        /** @brief 移动构造（显式标记为 explicit） */
        explicit String(SelfClass &&rhs) noexcept { buffer = std::move(rhs.buffer); }
        /** @brief 从 std::basic_string 构造 */
        explicit String(const std::basic_string<T> &s) { buffer = s; }

        virtual ~String()                     = default;

        // 赋值 --------------------------------------------------------------
        /**
         * @brief 移动赋值
         * @param rhs 右值引用来源字符串
         * @return 自身引用
         */
        SelfClass &operator=(       SelfClass &&                rhs ) noexcept  { if (this != &rhs) { buffer = std::move(rhs.buffer); } return *this; }

        /**
         * @brief 拷贝赋值
         * @param rhs 参考来源字符串
         * @return 自身引用
         */
        SelfClass &operator=(const  SelfClass &                 rhs )           { if (this != &rhs) buffer = rhs.buffer; return *this; }

        /**
         * @brief 从 std::basic_string 赋值
         * @param s 来源 std::basic_string
         * @return 自身引用
         */
        SelfClass &operator=(const  std::basic_string<T> &      s   )           { buffer = s; return *this; }

        /**
         * @brief 从初始化列表赋值
         * @param il 初始化列表
         * @return 自身引用
         */
        SelfClass &operator=(       std::initializer_list<T>    il  )           { if (!il.size()) { Clear(); return *this; } buffer.assign(il.begin(), il.end()); return *this; }

        /**
         * @brief 从 C 字符串赋值（保护自赋值）
         * @param str C 字符串指针
         * @return 自身引用
         */
        SelfClass &operator=(const  T *                         str )           { if (str != c_str()) fromString(str); return *this; }

        // 静态工厂/转换函数 (集中放置) ---------------------------------------
        // 工厂 ---------------------------------------------------------------
        /**
         * @brief 创建单字符字符串
         * @param ch 字符
         * @return 包含单个字符的字符串对象
         */
        static SelfClass charOf(const T &ch) { T tmp[2]; tmp[0] = ch; tmp[1] = 0; return SelfClass(tmp); }

        // 显式数字转换 ------------------------------------------------------
        /** @brief 将 int 转为字符串 */
        static SelfClass numberOf(int value)         { T tmp[8 * sizeof(int)]; itos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief 将 unsigned int 转为字符串 */
        static SelfClass numberOf(uint value)        { T tmp[8 * sizeof(uint)]; utos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief 将 int64 转为字符串 */
        static SelfClass numberOf(int64 value)       { T tmp[8 * sizeof(int64)]; itos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief 将 uint64 转为字符串 */
        static SelfClass numberOf(uint64 value)      { T tmp[8 * sizeof(uint64)]; utos(tmp, sizeof(tmp) / sizeof(T), value); return SelfClass(tmp); }
        /** @brief 将 float 转为字符串（指定小数位） */
        static SelfClass floatOf(float value, uint frac)   { T tmp[8 * sizeof(float)];  ftos(tmp, sizeof(tmp) / sizeof(T), frac, value); return SelfClass(tmp); }
        /** @brief 将 double 转为字符串（指定小数位） */
        static SelfClass floatOf(double value, uint frac)  { T tmp[8 * sizeof(double)]; ftos(tmp, sizeof(tmp) / sizeof(T), frac, value); return SelfClass(tmp); }
        /** @brief 计算百分比并格式化为字符串 */
        template<typename N1, typename N2>
        static SelfClass percentOf(const N1 num, const N2 den, uint frac) { return floatOf(double(num) / double(den) * 100.0, frac); }

        // 拼接组合为静态函数，便于集中管理
        /**
         * @brief 将两个字符序列按长度连接成新的字符串对象
         * @param s1 第一个字符序列
         * @param l1 第一个字符序列长度
         * @param s2 第二个字符序列
         * @param l2 第二个字符序列长度
         * @return 新生成的字符串对象
         */
        static SelfClass ComboString(const T *s1, int l1, const T *s2, int l2)
        {
            if (!s1 || l1 <= 0) { if (!s2 || l2 <= 0) return SelfClass(); return SelfClass(s2, l2); }
            if (!s2 || l2 <= 0) return SelfClass(s1, l1);
            SelfClass r; r.buffer.reserve(l1 + l2); r.buffer.append(s1, l1); r.buffer.append(s2, l2); return r;
        }

        // 基础信息 -----------------------------------------------------------
        /** @brief 返回字符长度（不包含末尾的隐含 NUL） */
        const int  Length()       const { return int(buffer.size()); }
        /** @brief 是否为空字符串 */
        const bool IsEmpty()      const { return buffer.empty(); }
        /** @brief 获取首字符，空时返回 0 */
        const T    GetFirstChar() const { return buffer.empty() ? T(0) : buffer.front(); }
        /** @brief 获取尾字符，空时返回 0 */
        const T    GetLastChar()  const { return buffer.empty() ? T(0) : buffer.back(); }

        // 访问/指针 ----------------------------------------------------------
        /**
         * @brief 获取指向内部连续内存的指针（只读与可写版本）
         * @note 当 buffer 为空时返回 nullptr
         */
        T *       c_str() const { return buffer.empty() ? nullptr : const_cast<T*>(buffer.data()); }
        T *       c_str()       { return buffer.empty() ? nullptr : const_cast<T*>(buffer.data()); }
        /** @brief 返回数据指针的别名 */
        T *       data_ptr()    { return c_str(); }

        // 载入/设置 ----------------------------------------------------------
        /**
         * @brief 从 C 字符串或指定长度的字符缓冲区载入到本对象
         * @param str 字符指针，可以为 nullptr
         * @param len 指定长度，-1 表示自动测长
         * @note 会剔除尾部多余的 NUL 字符
         */
        void fromString(const T *str, int len = -1)
        {
            if (!str || len == 0) { Clear(); return; }
            if (len < 0) buffer.assign(str); else buffer.assign(str, str + hgl::strlen(str, len));
            while (!buffer.empty() && buffer.back() == T(0)) buffer.pop_back();
        }

        /** @brief 将对象从其它地方“解绑”，当前实现总是返回 true（占位） */
        bool Unlink() { return true; }

        // 预分配/分配 --------------------------------------------------------
        /**
         * @brief 预分配底层容量以避免频繁扩容
         * @param n 期望容量
         * @return 成功返回 true，失败返回 false
         */
        bool PreAlloc(int n) { if (n <= 0) return false; buffer.reserve(size_t(n)); return true; }

        /**
         * @brief 调整字符串大小并返回底层内存指针
         * @param n 新的字符数（<=0 表示清空并返回 nullptr）
         * @return resize 后的底层数据指针或 nullptr
         */
        T *  Resize(int n)   { if (n <= 0) { Clear(); return nullptr; } buffer.resize(size_t(n)); return buffer.data(); }

        /** @brief 清空字符串 */
        void Clear() { buffer.clear(); }

        /**
         * @brief 用指定字符填充区间
         * @param ch 要填充的字符
         * @param start 起始位置
         * @param len 长度，-1 表示到末尾
         * @return 成功返回 true，失败返回 false（参数错误）
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
         * @brief 在指定位置插入字符序列
         * @param pos 插入位置（0..Length()）
         * @param str 插入的字符缓冲
         * @param len 插入长度，-1 表示自动测长
         * @return 成功返回 true，失败返回 false（参数错误）
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
        /** @brief 在 pos 处插入另一个 String 对象 */
        bool Insert(int pos, const SelfClass &s) { return Insert(pos, s.c_str(), s.Length()); }

        /**
         * @brief 删除从 pos 开始的 num 个字符
         * @param pos 起始位置
         * @param num 要删除的字符数量
         * @return 成功返回 true，失败返回 false（参数错误）
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
         * @param n 位置索引
         * @param ch 输出字符
         * @return 成功返回 true，否则 false
         */
        bool GetChar(int n, T &ch) const { if (n < 0 || n >= Length()) return false; ch = c_str()[n]; return true; }

        /**
         * @brief 设置指定位置的字符
         * @param n 位置索引
         * @param ch 新字符值
         * @return 成功返回 true，否则 false
         */
        bool SetChar(int n, const T ch)  { if (n < 0 || n >= Length()) return false; buffer[size_t(n)] = ch; return true; }

        // 比较/大小写/数字转换 等 ---------------------------------------------
        /**
         * @brief 全字符串比较（区分大小写）
         * @param rhs 右值字符串
         * @return <0 表示 *this < rhs, 0 表示相等, >0 表示 *this > rhs
         */
        int  Comp(const SelfClass &rhs)                                 const
        {
            if (Length() == 0) return rhs.Length();
            if (rhs.Length() == 0) return Length();
            return hgl::strcmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        /**
         * @brief 与 C 字符串比较（区分大小写）
         * @param str C 字符串
         * @return 比较结果，语义同上
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
         * @param pos 起始偏移
         * @param rhs 目标字符串
         * @return 比较结果
         */
        int  Comp(const uint pos, const SelfClass &rhs)                 const
        {
            if (pos > (uint)Length()) return 0;
            return hgl::strcmp(c_str() + pos, Length() - pos, rhs.c_str(), rhs.Length());
        }

        /**
         * @brief 从指定偏移处比较 rhs 的前 num 字符（区分大小写）
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
         */
        int  CaseComp(const SelfClass &rhs)                         const
        {
            if (Length() == 0) return rhs.Length();
            return hgl::stricmp(c_str(), Length(), rhs.c_str(), rhs.Length());
        }

        /**
         * @brief 与 C 字符串不区分大小写比较
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

                                bool ToBool (bool &result)  const { return stob(c_str(), result); }
        /** @brief 将字符串解析为整型（模板以支持不同整型类型） */
        template<typename I>    bool ToInt  (I &result)     const { return etof(c_str(), result); }
        /** @brief 将字符串解析为无符号整型（模板以支持不同无符号类型） */
        template<typename U>    bool ToUint (U &result)     const { return etof(c_str(), result); }
        /** @brief 将字符串解析为浮点数（模板以支持不同浮点类型） */
        template<typename F>    bool ToFloat(F &result)     const { return etof(c_str(), result); }

        /** @brief 将字符串转换为小写（就地修改） */
        SelfClass &LowerCase    ()      { if (Length()  > 0) { tolower(buffer.data()); } return *this; }
        /** @brief 返回小写拷贝 */
        SelfClass  ToLowerCase  () const{ if (Length() == 0) return SelfClass(); SelfClass tmp(*this); return tmp.LowerCase(); }
        /** @brief 将字符串转换为大写（就地修改） */
        SelfClass &UpperCase    ()      { if (Length()  > 0) { toupper(buffer.data()); } return *this; }
        /** @brief 返回大写拷贝 */
        SelfClass  ToUpperCase  () const{ if (Length() == 0) return SelfClass(); SelfClass tmp(*this); return tmp.UpperCase(); }

    protected:

        typedef const T *(*ConvFunc)(const T *, int &, const bool (*trimfunc)(const T &));

        /**
         * @brief 基于回调函数的通用字符串转换器（用于 Trim 等）
         * @param conv 转换函数，签名为 const T *func(const T *in,int &out_len, bool (*trimfunc)(const T&))
         * @return 转换后的字符串对象
         */
        SelfClass StrConv(ConvFunc conv) const
        {
            if (Length() == 0) return SelfClass();
            int new_len = Length();
            const T *new_str = conv(c_str(), new_len, isspace<T>);
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
         * @param start 起始索引
         * @param n 长度（-1 表示取到末尾）
         * @return 截取得到的新字符串
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
         * @param sc 输出字符串
         * @param start 起始索引
         * @param n 长度
         * @return 成功返回 true，失败返回 false
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
         */
        int FindRightChar(const T ch)                           const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), ch);
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 查找最后一次出现的字符集合（从右向左）
         */
        int FindRightChars(const SelfClass &ch)                 const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), ch.c_str(), ch.Length());
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 从右侧按偏移 off 查找字符（支持指定起始偏移）
         */
        int FindRightChar(const int off, const T ch)            const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), off, ch);
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 从右侧按偏移 off 查找字符集合（支持指定起始偏移）
         */
        int FindRightChar(const int off, const SelfClass &ch)   const
        {
            if (Length() == 0) return -1;
            const T *r = hgl::strrchr(c_str(), Length(), off, ch.c_str(), ch.Length());
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 查找不在指定字符集合内的字符（从 pos 开始），用于跳过集合中的字符
         */
        int FindExcludeChar(int pos, const T &ch)               const
        {
            if (pos < 0 || pos >= Length()) return -1;
            const T *r = hgl::strechr(c_str() + pos, ch);
            return r ? int(r - (c_str() + pos)) : -1;
        }

        /**
         * @brief 查找不在指定字符串集合内的字符（从 pos 开始）
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
         */
        int FindString(const SelfClass &str, int start = 0)     const
        {
            if (str.Length() <= 0 || start < 0 || start > Length() - str.Length()) return -1;
            const T *r = strstr(c_str() + start, Length() - start, str.c_str(), str.Length());
            return r ? int(r - c_str()) : -1;
        }

        /**
         * @brief 从字符串中移除所有匹配的子串
         * @param sub 要移除的子串
         * @return 实际移除次数
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
         * @param pos 写入起始位置
         * @param str 要写入的字符串
         * @return 成功返回 true，失败返回 false（pos 越界）
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
         * @return 替换的字符数量
         */
        int     Replace     (const T tch, const T sch)
        {
            if (IsEmpty()) return 0;
            int cnt = 0;
            for (auto &c : buffer)
                if (c == tch) { c = sch; ++cnt; }
            return cnt;
        }

        // 运算符/工具 --------------------------------------------------------
        /** @brief 常量索引访问（越界返回静态 0） */
        const T &   operator[](int index) const
        {
            static const T zero_char = 0;
            if (index >= 0 && index < Length()) return c_str()[index];
            return zero_char;
        }

        /** @brief 可写索引访问（越界返回动态分配的 0 引用） */
              T &   operator[](int index)
        {
            static T zero_char = * (new T(0));
            if (index < 0 || index >= Length()) return zero_char;
            return buffer[index];
        }

        /** @brief 隐式转换为 T*（指向内部数据） */
                    operator T *()
        {
            return c_str();
        }

        /** @brief 隐式转换为 const T*（指向内部数据） */
                    operator const T *() const
        {
            return c_str();
        }

        SelfClass &operator+=(const SelfClass &str)         { Insert(Length(), str.c_str(), str.Length()); return *this; }
        SelfClass &operator<<(const SelfClass &str)         { return operator+=(str); }
        SelfClass  operator+(const SelfClass &str) const    { if (str.IsEmpty()) return *this; if (IsEmpty()) return str; return ComboString(c_str(), Length(), str.c_str(), str.Length()); }
        SelfClass  operator+(const T ch)           const    { if (IsEmpty()) return SelfClass::charOf(ch); return ComboString(c_str(), Length(), &ch, 1); }
        SelfClass  operator+(const T *str)         const    { if (IsEmpty()) return SelfClass(str); return ComboString(c_str(), Length(), str, hgl::strlen(str)); }
        SelfClass  operator+(const int &)          const = delete;
        SelfClass  operator+(const uint &)         const = delete;
        SelfClass  operator+(const int64 &)        const = delete;
        SelfClass  operator+(const uint64 &)       const = delete;
        SelfClass  operator+(const float &)        const = delete;
        SelfClass  operator+(const double &)       const = delete;

        const int compare(const SelfClass &str) const override { return Comp(str); }

        /**
         * @brief 返回字符串中不同字符的数量（按字符值不同计数）
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
         * @param str 要追加的字符缓冲
         * @param len 长度，-1 表示自动测长
         * @return 成功返回 true，否则 false
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

        /** @brief 返回底层 std::basic_string 的常量引用 */
        const std::basic_string<T> &ToStdString() const { return buffer; }
    };//class String

    using AnsiString = String<char>;
    using U8String   = String<u8char>;
    using U16String  = String<u16char>;
    using U32String  = String<char32_t>;
    using OSString   = String<os_char>;
    using WideString = String<wchar_t>;
    using CodeString = AnsiString;

    // 全局 const T* + String<T>
    template<typename T>
    inline String<T> operator+(const T *lhs, const String<T> &rhs)
    {
        if (!lhs || *lhs == 0) return rhs;
        if (rhs.IsEmpty()) return String<T>(lhs);
        return String<T>::ComboString(lhs, hgl::strlen(lhs), rhs.c_str(), rhs.Length());
    }

    template<typename C> bool ToNumber(const String<C> &str, int &value)    { return str.ToInt(value); }
    template<typename C> bool ToNumber(const String<C> &str, uint &value)   { return str.ToUint(value); }
    template<typename C> bool ToNumber(const String<C> &str, float &value)  { return str.ToFloat(value); }
    template<typename C> bool ToNumber(const String<C> &str, double &value) { return str.ToFloat(value); }

    template<typename T2, int HASH_MAX> uint StringFastHash(const String<T2> &str) { const T2 *p = str.c_str(); int c = str.Length(); uint result = 0; while (c-- > 0) result += *p++; return (result % HASH_MAX); }

    template<typename T2, typename I> String<T2> ToHexString(const I &value) { T2 str[(sizeof(I) << 1) + 1]; ToUpperHexStr(str, value); return String<T2>(str); }
    template<typename T2, typename I> String<T2> HexToString(const I &value) { T2 str[(sizeof(I) << 1) + 1]; htos(str, (sizeof(I) << 1) + 1, value); return String<T2>(str); }
    template<typename T2> String<T2> PointerToHexString(const void *ptr)     { return HexToString<T2, HGL_POINTER_UINT>(reinterpret_cast<const HGL_POINTER_UINT>(ptr)); }
    inline String<os_char> PointerToHexOSString(const void *value)           { return PointerToHexString<os_char>(value); }
    inline String<char>    PointerToHexU8String(const void *value)          { return PointerToHexString<char>(value); }
    inline String<u16char> PointerToHexU16String(const void *value)         { return PointerToHexString<u16char>(value); }

    template<typename T2> inline void strcpy(T2 *dst, int max_count, const String<T2> &src) { hgl::strcpy(dst, max_count, src.c_str(), src.Length()); }
    template<typename T2> inline void strcat(T2 *dst, int max_count, const String<T2> &src) { hgl::strcat(dst, max_count, src.c_str(), src.Length()); }
}//namespace hgl
