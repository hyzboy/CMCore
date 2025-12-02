#pragma once

#include<hgl/type/StringList.h>
#include<initializer_list>

/**
* Maximum Path Length Limitation
* https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
*/

namespace hgl::filesystem
{
    // Modernized and clearer API names; old names are preserved as inline wrappers for compatibility.

    /**
     * @brief CN: 将多个路径段连接为单个路径（接受可写的 char* 数组及其长度数组）。
     * @brief EN: Join multiple path segments into a single path (accepts writable char* array and lengths array).
     *
     * @param parts
     *     CN: 每个路径段的指针数组（可能包含前导或尾部分隔符）。
     *     EN: Array of pointers to each path segment (may include leading/trailing separators).
     * @param part_lengths
     *     CN: 每个段的长度数组（以字符为单位）。
     *     EN: Array of lengths for each segment (in characters).
     * @param count
     *     CN: parts 和 part_lengths 中元素的数量。
     *     EN: Number of entries in parts and part_lengths.
     * @param separator_char
     *     CN: 连接段时使用的分隔符字符（默认为平台默认分隔符）。
     *     EN: Separator character to insert between segments when joining (defaults to platform separator).
     *
     * @return
     *     CN: 返回拼接后的路径字符串（String<T>）。
     *     EN: Returns the joined path as String<T>.
     *
     * 逻辑段注释（CN/EN 均写在行内）:
     * - 检查输入计数并初始化返回字符串与临时数组 (check count and prepare buffers)
     * - 遍历片段: 测量和修剪前后分隔符，记录实际可用子串指针与长度 (measure and trim each segment)
     * - 统计要分配的总长度并分配缓冲区 (compute total length and allocate)
     * - 写入（可选保留首段前导分隔符、插入连接分隔符并拷贝片段）(write leading separators, separators, copy segments)
     */
    template<typename T>
    inline const String<T> JoinPath(T **parts,int *part_lengths,const int count,const T separator_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
    {
        // CN: fullname 为返回值缓冲区
        // EN: fullname is the output buffer
        String<T> fullname;

        // CN: 如果没有片段则返回空字符串
        // EN: validate args - return empty if no parts
        if(count<=0)
            return fullname;

        // CN: ptrs 和 lens 保存修剪后的每个片段的指针与长度
        // EN: store trimmed pointers and lengths
        AutoDeleteArray<const T *> ptrs(count);
        AutoDeleteArray<int> lens(count);

        // CN: 非空片段计数与总长度（字符数）
        // EN: non-empty segments count and total characters
        int non_empty_segments = 0;
        int total_len = 0;

        // CN: leading_count 记录第一个非空片段的前导分隔符数量（用于保留绝对路径语义）
        // EN: leading_count records number of leading separators of first non-empty segment
        int leading_count = 0;
        const T *leading_src = nullptr;
        bool leading_recorded = false;

        // 遍历并修剪每个片段 / iterate and trim each segment
        for(int i=0;i<count;i++)
        {
            int orig_len = part_lengths[i];
            // CN: 跳过空或非法长度的片段
            // EN: skip empty or invalid length segments
            if(orig_len<=0)
            {
                ptrs[i]=nullptr;
                lens[i]=0;
                continue;
            }

            // CN: 使用原始缓冲区指针并以原始末端为基准修剪尾部
            // EN: use original buffer end for trimming trailing separators
            const T *orig = parts[i];
            const T *start = trim<T>(orig, orig_len, hgl::is_slash<T>); // CN: 左侧修剪
            // EN: trim left side
            const T *endp = orig + orig_len; // CN: 末端基于原始缓冲区
            // EN: original buffer end

            // CN: 修剪尾部分隔符
            // EN: trim trailing separators
            while(endp>start && hgl::is_slash<T>(*(endp-1))) --endp;

            // CN: 计算修剪后的长度
            // EN: compute trimmed length
            int new_len = (int)(endp - start);

            if(new_len>0)
            {
                // CN: 保存指针与长度用于后续拷贝
                // EN: store pointer and len for copying later
                ptrs[i]=start;
                lens[i]=new_len;
                total_len += new_len;
                ++non_empty_segments;

                // CN: 记录第一个非空片段的前导分隔符数量（保留绝对/UNC 等语义）
                // EN: record leading separators count of first non-empty segment
                if(!leading_recorded)
                {
                    int lead = 0;
                    while(lead < orig_len && hgl::is_slash<T>(orig[lead])) ++lead;
                    if(lead>0)
                    {
                        leading_count = lead;
                        leading_src = orig;
                    }
                    leading_recorded = true;
                }
            }
            else
            {
                ptrs[i]=nullptr;
                lens[i]=0;
            }
        }

        // CN: 计算需要的分隔符数量并预留缓冲
        // EN: compute separator count and reserve buffer
        int separator_count = non_empty_segments > 0 ? (non_empty_segments - 1) : 0;
        int total_reserved = total_len + separator_count + leading_count;
        T *write_ptr = fullname.Resize(total_reserved);

        bool first = true;

        // CN: 写入保留的首段前导分隔符，统一为 separator_char（规范化）
        // EN: write preserved leading separators normalized to separator_char
        if(leading_count>0)
        {
            for(int k=0;k<leading_count;k++)
            {
                *write_ptr = separator_char;
                ++write_ptr;
            }
            first = false; // CN: 已写入前导分隔符
            // EN: leading separators already written
        }

        // CN: 逐段写入：插入连接分隔符并拷贝段内容
        // EN: write each segment, insert separator and copy content
        for(int i=0;i<count;i++)
        {
            int len = lens[i];
            if(len<=0) continue;

            if(!first)
            {
                *write_ptr = separator_char;
                ++write_ptr;
            }
            else
            {
                first = false;
            }

            hgl_cpy<T>(write_ptr, ptrs[i], len); // CN: 拷贝段内容
            // EN: copy segment content
            write_ptr += len;
        }

        // CN: 调整实际写入的长度并返回结果
        // EN: resize to actual written length and return
        int written = (int)(write_ptr - fullname);
        if(written > 0)
            fullname.Resize(written);
        else
            fullname.Resize(0);

        return fullname;
    }

    /**
     * @brief CN: JoinPath 重载，接受 const 指针数组和长度数组。
     * @brief EN: JoinPath overload that accepts const char* array and length array.
     *
     * 参数与返回值说明同上（refer to JoinPath above）。
     */
    template<typename T>
    inline const String<T> JoinPath(const T * const *parts,const int *part_lengths,const int count,const T separator_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
    {
        // Implementation mirrors non-const overload; comments omitted here to avoid duplication
        String<T> fullname;

        if(count<=0)
            return fullname;

        AutoDeleteArray<const T *> ptrs(count);
        AutoDeleteArray<int> lens(count);

        int non_empty_segments = 0;
        int total_len = 0;
        int leading_count = 0;
        const T *leading_src = nullptr;
        bool leading_recorded = false;

        for(int i=0;i<count;i++)
        {
            int orig_len = part_lengths[i];
            if(orig_len<=0)
            {
                ptrs[i]=nullptr;
                lens[i]=0;
                continue;
            }

            const T *orig = parts[i];
            const T *start = trim<T>(orig, orig_len, hgl::is_slash<T>);
            const T *endp = orig + orig_len;

            while(endp>start && hgl::is_slash<T>(*(endp-1))) --endp;

            int new_len = (int)(endp - start);

            if(new_len>0)
            {
                ptrs[i]=start;
                lens[i]=new_len;
                total_len += new_len;
                ++non_empty_segments;

                if(!leading_recorded)
                {
                    int lead = 0;
                    while(lead < orig_len && hgl::is_slash<T>(orig[lead])) ++lead;
                    if(lead>0)
                    {
                        leading_count = lead;
                        leading_src = orig;
                    }
                    leading_recorded = true;
                }
            }
            else
            {
                ptrs[i]=nullptr;
                lens[i]=0;
            }
        }

        int separator_count = non_empty_segments > 0 ? (non_empty_segments - 1) : 0;
        int total_reserved = total_len + separator_count + leading_count;
        T *write_ptr = fullname.Resize(total_reserved);

        bool first = true;

        if(leading_count>0)
        {
            for(int k=0;k<leading_count;k++)
            {
                *write_ptr = separator_char;
                ++write_ptr;
            }
            first = false;
        }

        for(int i=0;i<count;i++)
        {
            int len = lens[i];
            if(len<=0) continue;

            if(!first)
            {
                *write_ptr = separator_char;
                ++write_ptr;
            }
            else
            {
                first = false;
            }

            hgl_cpy<T>(write_ptr, ptrs[i], len);
            write_ptr += len;
        }

        int written = (int)(write_ptr - fullname);
        if(written > 0)
            fullname.Resize(written);
        else
            fullname.Resize(0);

        return fullname;
    }

    /**
     * @brief CN: JoinPath 接受 initializer_list 的重载（C 字符串指针）。
     * @brief EN: JoinPath overload that accepts initializer_list of C-string pointers.
     *
     * 参数与返回值说明同 JoinPath
     */
    template<typename T>
    inline const String<T> JoinPath(const T separator_char,const std::initializer_list<const T *> &args)
    {
        // CN: 处理 initializer_list 输入
        // EN: handle initializer_list input
        if(args.size()<=0)
            return String<T>();

        AutoDeleteArray<const T *> ptrs(static_cast<int>(args.size()));
        AutoDeleteArray<int> lens(static_cast<int>(args.size()));

        int index = 0;
        int leading_count = 0;
        const T *leading_src = nullptr;
        bool leading_recorded = false;

        // CN: 遍历 args 并修剪每个 C 字符串
        // EN: iterate and trim each C-string
        for(const T *s:args)
        {
            int l = hgl::strlen(s);
            if(l<=0) continue;

            const T *orig = s;
            const T *start = trim<T>(orig, l, hgl::is_slash<T>);
            const T *endp = orig + l;
            while(endp>start && hgl::is_slash<T>(*(endp-1))) --endp;
            int new_len = (int)(endp - start);
            if(new_len<=0) continue;

            ptrs[index]=start;
            lens[index]=new_len;

            if(!leading_recorded)
            {
                int lead = 0;
                while(lead < l && hgl::is_slash<T>(orig[lead])) ++lead;
                if(lead>0)
                {
                    leading_count = lead;
                    leading_src = orig;
                }
                leading_recorded = true;
            }

            ++index;
        }

        if(index==0) return String<T>();

        int total_len = 0;
        for(int i=0;i<index;i++) total_len += lens[i];
        int separator_count = index > 0 ? (index - 1) : 0;

        String<T> fullname;
        T *write_ptr = fullname.Resize(total_len + separator_count + leading_count);

        // CN: 写入前导分隔符（规范化）
        // EN: write normalized leading separators
        bool first = true;
        if(leading_count>0)
        {
            for(int k=0;k<leading_count;k++) { *write_ptr = separator_char; ++write_ptr; }
            first = false;
        }

        // CN: 拷贝各片段并在片段间插入 separator_char
        // EN: copy segments and insert separator_char between
        for(int i=0;i<index;i++)
        {
            hgl_cpy<T>(write_ptr, ptrs[i], lens[i]);
            write_ptr += lens[i];

            if(i < index-1)
            {
                *write_ptr = separator_char;
                ++write_ptr;
            }
        }

        int written = (int)(write_ptr - fullname);
        if(written > 0)
            fullname.Resize(written);
        else
            fullname.Resize(0);

        return fullname;
    }

    // CN: 简单的 char* initializer_list 便捷重载
    // EN: convenience overload for char*
    inline const String<char> JoinPath(const std::initializer_list<const char *> &args)
    {
        return JoinPath<char>(HGL_DIRECTORY_SEPARATOR_RAWCHAR,args);
    }

#ifdef HGL_SUPPORT_CHAR8_T
    inline const String<char8_t> JoinPath(const std::initializer_list<const char8_t *> &args)
    {
        return JoinPath<char8_t>(HGL_DIRECTORY_SEPARATOR_U8CHAR,args);
    }
#endif//HGL_SUPPORT_CHAR8_T

#if HGL_OS == HGL_OS_Windows
    inline const String<wchar_t> JoinPath(const std::initializer_list<const wchar_t *> &args)
    {
        return JoinPath<wchar_t>(HGL_DIRECTORY_SEPARATOR_WCHAR,args);
    }
#endif//HGL_OS == HGL_OS_Windows

    /**
     * @brief CN: JoinPath 接受 initializer_list 的 String<T> 重载。
     * @brief EN: JoinPath overload that accepts initializer_list of String<T>.
     */
    template<typename T>
    inline const String<T> JoinPath(const T separator_char,const std::initializer_list<const String<T>> &args)
    {
        if(args.size()<=0)
            return String<T>();

        AutoDeleteArray<const T *> ptrs(static_cast<int>(args.size()));
        AutoDeleteArray<int> lens(static_cast<int>(args.size()));

        int index = 0;
        int leading_count = 0;
        const T *leading_src = nullptr;
        bool leading_recorded = false;

        for(const String<T> &s:args)
        {
            if(s.IsEmpty()) continue;
            const T *orig = s.c_str();
            int l = s.Length();

            const T *start = trim<T>(orig, l, hgl::is_slash<T>);
            const T *endp = orig + l;
            while(endp>start && hgl::is_slash<T>(*(endp-1))) --endp;
            int new_len = (int)(endp - start);
            if(new_len<=0) continue;

            ptrs[index]=start;
            lens[index]=new_len;

            if(!leading_recorded)
            {
                int lead = 0;
                while(lead < l && hgl::is_slash<T>(orig[lead])) ++lead;
                if(lead>0)
                {
                    leading_count = lead;
                    leading_src = orig;
                }
                leading_recorded = true;
            }

            ++index;
        }

        if(index==0) return String<T>();

        int total_len = 0;
        for(int i=0;i<index;i++) total_len += lens[i];
        int separator_count = index > 0 ? (index - 1) : 0;

        String<T> fullname;
        T *write_ptr = fullname.Resize(total_len + separator_count + leading_count);

        bool first = true;
        if(leading_count>0)
        {
            for(int k=0;k<leading_count;k++) { *write_ptr = separator_char; ++write_ptr; }
            first = false;
        }

        for(int i=0;i<index;i++)
        {
            hgl_cpy<T>(write_ptr, ptrs[i], lens[i]);
            write_ptr += lens[i];

            if(i < index-1)
            {
                *write_ptr = separator_char;
                ++write_ptr;
            }
        }

        int written = (int)(write_ptr - fullname);
        if(written > 0)
            fullname.Resize(written);
        else
            fullname.Resize(0);

        return fullname;
    }

    inline const String<char> JoinPath(const std::initializer_list<const String<char>> &args)
    {
        return JoinPath<char>(HGL_DIRECTORY_SEPARATOR_RAWCHAR,args);
    }

#ifdef HGL_SUPPORT_CHAR8_T
    inline const String<char8_t> JoinPath(const std::initializer_list<const String<char8_t>> &args)
    {
        return JoinPath<char8_t>(HGL_DIRECTORY_SEPARATOR_U8CHAR,args);
    }
#endif//HGL_SUPPORT_CHAR8_T

#if HGL_OS == HGL_OS_Windows
    inline const String<wchar_t> JoinPath(const std::initializer_list<const String<wchar_t>> &args)
    {
        return JoinPath<wchar_t>(HGL_DIRECTORY_SEPARATOR_WCHAR,args);
    }
#endif//HGL_OS == HGL_OS_Windows

    /**
     * @brief CN: JoinPath 便捷重载：传入 C 字符串数组，函数会测长并调用主实现。
     * @brief EN: Convenience overload accepting C-string array; it measures lengths and delegates to main implementation.
     *
     * @param parts
     *     CN: 指向 C 字符串数组的指针。
     *     EN: Pointer to array of C-strings.
     * @param count
     *     CN: 数组项数量。
     *     EN: Number of items.
     * @param separator_char
     *     CN/EN: same as main JoinPath
     */
    template<typename T>
    inline const String<T> JoinPath(T **parts,const int count,const T separator_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
    {
        if(count<=0)
            return String<T>();

        AutoDeleteArray<int> part_lengths(count);
        for(int i=0;i<count;i++)
            part_lengths[i]=hgl::strlen(parts[i]);

        return JoinPath(parts, part_lengths.data(), count, separator_char);
    }

    /**
     * @brief CN: 从 StringList 构建路径并调用 JoinPath 主实现。
     * @brief EN: Build path from StringList and call main JoinPath implementation.
     */
    template<typename T>
    inline const String<T> JoinPath(const StringList<T> &sl,const T separator_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
    {
        const int count = static_cast<int>(sl.GetCount());
        if(count<=0)
            return String<T>();

        AutoDeleteArray<const T *> parts(count);
        AutoDeleteArray<int> part_lengths(count);

        int index=0;
        for(const String<T> *s:sl)
        {
            if(s->IsEmpty()) continue;
            parts[index]=s->c_str();
            part_lengths[index]=s->Length();
            ++index;
        }

        return JoinPath(parts.data(), part_lengths.data(), index, separator_char);
    }

    /**
     * @brief CN: 将路径名与文件名合并为完整路径，智能处理分隔符的添加或去重。
     * @brief EN: Merge path and filename into a full path, handling separator addition/removal appropriately.
     *
     * @param pathname
     *     CN: 路径部分
     *     EN: path component
     * @param filename
     *     CN: 文件名部分
     *     EN: filename component
     * @param directory_separator_char
     *     CN: 分隔符字符
     *     EN: separator char
     * @param directory_separator_str
     *     CN: 分隔符字符串（用于多字符分隔符）
     *     EN: separator string (for multi-char separators)
     * @return
     *     CN: 返回合并后的完整路径
     *     EN: merged full path
     */
    template<typename T>
    inline String<T> JoinPathWithFilename(const String<T> &pathname,const String<T> &filename,const T directory_separator_char,const T *directory_separator_str)
    {
        String<T> fullname;

        // CN: 如果 pathname 末尾已有分隔符
        // EN: if pathname ends with separator
        if(pathname.GetLastChar()==directory_separator_char)
        {
            // CN: 如果 filename 开头也有分隔符，则去掉 pathname 的最后一个分隔符以避免重复
            // EN: if filename also starts with separator then drop one from pathname
            if(filename.GetFirstChar()==directory_separator_char)
            {
                fullname.fromString(pathname.c_str(),pathname.Length()-1);
            }
            else
            {
                fullname=pathname;
            }
        }
        else
        {
            // CN: pathname 末尾没有分隔符，若 filename 开头也没有则添加分隔符
            // EN: pathname lacks trailing separator; add one if filename doesn't start with separator
            fullname=pathname;
            if(filename.GetFirstChar()!=directory_separator_char)
            {
                fullname.Strcat(directory_separator_str);
            }
        }

        // CN: 追加 filename 并返回完整路径
        // EN: append filename and return
        fullname.Strcat(filename);
        return fullname;
    }

    /**
     * @brief CN: 从完整路径中提取文件名部分（basename）。
     * @brief EN: Extract the filename component (basename) from a full path.
     *
     * @param fullname
     *     CN: 输入的完整路径
     *     EN: input full path
     * @return
     *     CN: 文件名部分
     *     EN: filename component
     */
    template<typename T>
    inline String<T> GetFilename(const String<T> &fullname)
    {
        if(fullname.IsEmpty())
            return(String<T>());

        // CN: 识别 '/' 和 '\\' 作为分隔符
        // EN: recognize '/' and '\\' as separators
        const T sep_chars[] = { '/','\\',0 };
        const int pos = fullname.FindRightChars(sep_chars);
        if(pos==-1)
            return String<T>(fullname);
        return fullname.SubString(pos+1);
    }

    /**
     * @brief CN: 从文件名中提取主名称（不含扩展名）。
     * @brief EN: Extract the stem (filename without extension) from a file name.
     *
     * @param filename
     *     CN: 输入的文件名或路径
     *     EN: input filename or path
     * @param split_char
     *     CN: 扩展名分隔符（默认 '.'）
     *     EN: extension delimiter (default '.')
     * @return
     *     CN: 主名称（不含扩展名）
     *     EN: stem (filename without extension)
     */
    template<typename T>
    inline String<T> GetStem(const String<T> &filename,const T split_char='.')
    {
        if(filename.IsEmpty())
            return(String<T>());

        const T sep_chars[] = { '/','\\',0 };
        const int dot = filename.FindRightChar(split_char);
        const int pos = filename.FindRightChars(sep_chars);

        if(dot==-1)
        {
            if(pos==-1)
                return String<T>(filename);
            else
                return filename.SubString(pos+1);
        }
        else
        {
            if(pos==-1)
                return filename.SubString(0,dot);
            else
                return filename.SubString(pos+1,dot-pos-1);
        }
    }

    /**
     * @brief CN: 提取完整文件名的扩展名，可选择是否包含点('.')。
     * @brief EN: Extract the extension from a full filename; option to include dot.
     *
     * @param fullname
     *     CN: 输入的完整文件名（可能包含 '?' 查询部分）
     *     EN: input full filename (may include query part '?')
     * @param include_dot
     *     CN: 是否在返回的扩展名前包含 '.'
     *     EN: whether to include leading '.' in returned extension
     * @return
     *     CN: 扩展名字符串（可能为空）
     *     EN: extension string (may be empty)
     */
    template<typename T>
    inline String<T> GetExtension(const String<T> &fullname,bool include_dot=true)
    {
        int end = fullname.FindChar(T('?'));
        if(end==-1) end = fullname.Length();
        if(end<=0) return String<T>();

        // CN: 在 end-1 之前查找最后一个 '.'
        // EN: find right-most '.' before end
        int pos = fullname.FindRightChar(end-1, T('.'));
        if(pos==-1) return String<T>();

        return include_dot? fullname.SubString(pos, end - pos) : fullname.SubString(pos+1, end-(pos+1));
    }

    /**
     * @brief CN: 移除扩展名并返回不带扩展名的文件名（可选择是否删除点）。
     * @brief EN: Remove extension and return filename without extension (optionally remove dot).
     */
    template<typename T>
    inline String<T> RemoveExtension(const String<T> &fullname,bool remove_dot=false)
    {
        int end = fullname.FindChar(T('?'));
        if(end==-1) end = fullname.Length();
        if(end<=0) return String<T>(fullname);

        int pos = fullname.FindRightChar(end-1, T('.'));
        if(pos==-1) return String<T>(fullname);

        return remove_dot? fullname.SubString(0,pos) : fullname.SubString(0,pos+1);
    }

    /**
     * @brief CN: 提取路径（不含文件名），可选是否包含末尾分隔符。
     * @brief EN: Extract directory path (without filename); option to include trailing separator.
     *
     * @param filename
     *     CN: 输入的完整路径
     *     EN: input full path
     * @param include_separator
     *     CN: 是否包含末尾分隔符
     *     EN: whether to include trailing separator
     * @return
     *     CN: 目录路径
     *     EN: directory path
     */
    template<typename T>
    inline String<T> GetParentPath(const String<T> &filename,bool include_separator=true)
    {
        if(filename.IsEmpty()) return(String<T>());

        const T sep_chars[] = { '/','\\',':',0 };
        const int pos = filename.FindRightChars(sep_chars);
        if(pos==-1) return filename;
        if(include_separator) return filename.SubString(0,pos+1);
        return filename.SubString(0,pos);
    }

    /**
     * @brief CN: 提取路径的最后一个组件（即最后一级目录名或文件名），会跳过尾部分隔符。
     * @brief EN: Extract the last path component (last directory name or filename), skipping trailing separators.
     */
    template<typename T>
    inline String<T> GetLastPathComponent(const String<T> &fullname)
    {
        if(fullname.IsEmpty()) return(String<T>());

        const T sep_chars[] = {'\\','/',0};
        const T *start = fullname.c_str();
        const T *endp = fullname.c_str() + fullname.Length() - 1;
        const T *p = nullptr;

        // CN: 从字符串尾部向左扫描以找到最后一个非分隔符的范围
        // EN: scan from end to find last non-separator range
        while(endp>start)
        {
            if(!p)
            {
                if(*endp==sep_chars[0] || *endp==sep_chars[1]) { --endp; continue; }
                p = endp; --endp;
            }
            else
            {
                if(*endp==sep_chars[0] || *endp==sep_chars[1])
                {
                    return String<T>(endp+1, (int)(p - endp));
                }
                --endp;
            }
        }

        return(String<T>());
    }

    /**
     * @brief CN: 将 fullname 拆分为路径和文件名两部分，通过 out_path/out_filename 返回。
     * @brief EN: Split fullname into path and filename components via out_path/out_filename.
     *
     * @return
     *     CN: 成功返回 true，失败返回 false（例如没有分隔符）。
     *     EN: Returns true on success, false if no separator found.
     */
    template<typename T>
    inline bool SplitPath(String<T> &out_path,String<T> &out_filename,const String<T> &fullname)
    {
        if(fullname.IsEmpty()) return false;

        const T sep_chars[] = { '/','\\' };
        const int pos = fullname.FindRightChars(sep_chars);
        if(pos==-1) return false;

        out_path    =fullname.SubString(0,pos);
        out_filename=fullname.SubString(pos+1);
        return true;
    }

    /**
     * @brief CN: 替换文件扩展名；若无扩展名则追加 '.' + new_extname。
     * @brief EN: Replace file extension; if none found append '.' + new_extname.
     */
    template<typename T>
    inline String<T> ReplaceExtension(const String<T> &old_name,const String<T> &new_extname,const T split_char='.',const bool from_right=true)
    {
        if(old_name.IsEmpty()) return(String<T>::charOf(split_char)+new_extname);

        int pos = from_right ? old_name.FindRightChar(split_char) : old_name.FindChar(split_char);
        if(pos!=-1) return old_name.SubString(0,pos+1)+new_extname;
        return old_name+String<T>::charOf(split_char)+new_extname;
    }

#ifdef HGL_SUPPORT_CHAR8_T
    inline AnsiString JoinPathWithFilename(const AnsiString &pathname,const AnsiString &filename)
    { return JoinPathWithFilename<char>(pathname,filename,HGL_DIRECTORY_SEPARATOR,HGL_DIRECTORY_SEPARATOR_RAWSTR); }
#endif//HGL_SUPPORT_CHAR8_T

    inline U8String JoinPathWithFilename(const U8String &pathname,const U8String &filename)
    { return JoinPathWithFilename<u8char>(pathname,filename,HGL_DIRECTORY_SEPARATOR,HGL_DIRECTORY_SEPARATOR_U8STR); }

#if HGL_OS == HGL_OS_Windows
    inline WideString JoinPathWithFilename(const WideString &pathname,const WideString &filename)
    { return JoinPathWithFilename<wchar_t>(pathname,filename,HGL_DIRECTORY_SEPARATOR,HGL_DIRECTORY_SEPARATOR_STR); }
#endif//HGL_OS == HGL_OS_Windows

    // Normalize filename (formerly FixFilename)
    /**
     * @brief CN: 规范化操作系统字符串形式的文件名，处理中/平台特定问题。
     * @brief EN: Normalize OSString filename, handling platform-specific issues.
     */
    OSString NormalizeFilename(const OSString &filename);
}//namespace hgl::filesystem
