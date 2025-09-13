#pragma once

#include<hgl/type/StringList.h>
#include<hgl/math/PrimaryMathematics.h>
#include<initializer_list>

/**
* Maximum Path Length Limitation
* https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
*/

namespace hgl::filesystem
{
    // Modernized and clearer API names; old names are preserved as inline wrappers for compatibility.

    template<typename T>
    inline const String<T> JoinPath(T **parts,int *part_lengths,const int count,const T separator_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
    {
        String<T> fullname;

        if(count<=0)
            return fullname;

        AutoDeleteArray<const T *> ptrs(count);
        AutoDeleteArray<int> lens(count);

        int non_empty_segments = 0;
        int total_len = 0;
        int preserve_leading = 0;
        bool preserve_leading_recorded = false;

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
            const T *endp = orig + orig_len; // use original buffer end

            while(endp>start && hgl::is_slash<T>(*(endp-1))) --endp;

            int new_len = (int)(endp - start);

            if(new_len>0)
            {
                ptrs[i]=start;
                lens[i]=new_len;
                total_len += new_len;
                ++non_empty_segments;

                if(!preserve_leading_recorded)
                {
                    // count leading separators in the original segment to preserve absolute/UNC semantics
                    int lead = 0;
                    while(lead < orig_len && hgl::is_slash<T>(orig[lead])) ++lead;
                    preserve_leading = lead;
                    preserve_leading_recorded = true;
                }
            }
            else
            {
                ptrs[i]=nullptr;
                lens[i]=0;
            }
        }

        int separator_count = non_empty_segments > 0 ? (non_empty_segments - 1) : 0;
        int total_reserved = total_len + separator_count + preserve_leading;
        T *write_ptr = fullname.Resize(total_reserved);

        bool first = true;

        // write preserved leading separators if any
        if(preserve_leading>0)
        {
            for(int k=0;k<preserve_leading;k++)
            {
                *write_ptr = separator_char;
                ++write_ptr;
            }
            first = false; // leading separators already written
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

    // Overload accepting arrays of const pointers
    template<typename T>
    inline const String<T> JoinPath(const T * const *parts,const int *part_lengths,const int count,const T separator_char=(T)HGL_DIRECTORY_SEPARATOR_RAWCHAR)
    {
        String<T> fullname;

        if(count<=0)
            return fullname;

        AutoDeleteArray<const T *> ptrs(count);
        AutoDeleteArray<int> lens(count);

        int non_empty_segments = 0;
        int total_len = 0;
        int preserve_leading = 0;
        bool preserve_leading_recorded = false;

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
            const T *endp = orig + orig_len; // use original buffer end

            while(endp>start && hgl::is_slash<T>(*(endp-1))) --endp;

            int new_len = (int)(endp - start);

            if(new_len>0)
            {
                ptrs[i]=start;
                lens[i]=new_len;
                total_len += new_len;
                ++non_empty_segments;

                if(!preserve_leading_recorded)
                {
                    int lead = 0;
                    while(lead < orig_len && hgl::is_slash<T>(orig[lead])) ++lead;
                    preserve_leading = lead;
                    preserve_leading_recorded = true;
                }
            }
            else
            {
                ptrs[i]=nullptr;
                lens[i]=0;
            }
        }

        int separator_count = non_empty_segments > 0 ? (non_empty_segments - 1) : 0;
        int total_reserved = total_len + separator_count + preserve_leading;
        T *write_ptr = fullname.Resize(total_reserved);

        bool first = true;

        if(preserve_leading>0)
        {
            for(int k=0;k<preserve_leading;k++)
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

    template<typename T>
    inline const String<T> JoinPath(const T separator_char,const std::initializer_list<const T *> &args)
    {
        if(args.size()<=0)
            return String<T>();

        AutoDeleteArray<const T *> ptrs(static_cast<int>(args.size()));
        AutoDeleteArray<int> lens(static_cast<int>(args.size()));

        int index = 0;
        int preserve_leading = 0;
        bool preserve_leading_recorded = false;

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

            if(!preserve_leading_recorded)
            {
                int lead = 0;
                while(lead < l && hgl::is_slash<T>(orig[lead])) ++lead;
                preserve_leading = lead;
                preserve_leading_recorded = true;
            }

            ++index;
        }

        if(index==0) return String<T>();

        int total_len = 0;
        for(int i=0;i<index;i++) total_len += lens[i];
        int separator_count = index > 0 ? (index - 1) : 0;

        String<T> fullname;
        T *write_ptr = fullname.Resize(total_len + separator_count + preserve_leading);

        // write leading preserved separators
        bool first = true;
        if(preserve_leading>0)
        {
            for(int k=0;k<preserve_leading;k++) { *write_ptr = separator_char; ++write_ptr; }
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

    template<typename T>
    inline const String<T> JoinPath(const T separator_char,const std::initializer_list<const String<T>> &args)
    {
        if(args.size()<=0)
            return String<T>();

        AutoDeleteArray<const T *> ptrs(static_cast<int>(args.size()));
        AutoDeleteArray<int> lens(static_cast<int>(args.size()));

        int index = 0;
        int preserve_leading = 0;
        bool preserve_leading_recorded = false;

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

            if(!preserve_leading_recorded)
            {
                int lead = 0;
                while(lead < l && hgl::is_slash<T>(orig[lead])) ++lead;
                preserve_leading = lead;
                preserve_leading_recorded = true;
            }

            ++index;
        }

        if(index==0) return String<T>();

        int total_len = 0;
        for(int i=0;i<index;i++) total_len += lens[i];
        int separator_count = index > 0 ? (index - 1) : 0;

        String<T> fullname;
        T *write_ptr = fullname.Resize(total_len + separator_count + preserve_leading);

        bool first = true;
        if(preserve_leading>0)
        {
            for(int k=0;k<preserve_leading;k++) { *write_ptr = separator_char; ++write_ptr; }
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

    // Convenience overload that measures C-string parts
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

    // Join from StringList
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

    // Join a path and a filename, preserving or adding separator as needed
    template<typename T>
    inline String<T> JoinPathWithFilename(const String<T> &pathname,const String<T> &filename,const T directory_separator_char,const T *directory_separator_str)
    {
        String<T> fullname;

        if(pathname.GetLastChar()==directory_separator_char)
        {
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
            fullname=pathname;
            if(filename.GetFirstChar()!=directory_separator_char)
            {
                fullname.Strcat(directory_separator_str);
            }
        }

        fullname.Strcat(filename);
        return fullname;
    }

    // Filename/path utility getters
    template<typename T>
    inline String<T> GetFilename(const String<T> &fullname)
    {
        if(fullname.IsEmpty())
            return(String<T>());

        const T sep_chars[] = { '/','\\',0 };
        const int pos = fullname.FindRightChars(sep_chars);
        if(pos==-1)
            return String<T>(fullname);
        return fullname.SubString(pos+1);
    }

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

    template<typename T>
    inline String<T> GetExtension(const String<T> &fullname,bool include_dot=true)
    {
        int end = fullname.FindChar(T('?'));
        if(end==-1) end = fullname.Length();
        if(end<=0) return String<T>();

        int pos = fullname.FindRightChar(end-1, T('.'));
        if(pos==-1) return String<T>();

        return include_dot? fullname.SubString(pos, end - pos) : fullname.SubString(pos+1, end-(pos+1));
    }

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

    template<typename T>
    inline String<T> GetLastPathComponent(const String<T> &fullname)
    {
        if(fullname.IsEmpty()) return(String<T>());

        const T sep_chars[] = {'\\','/',0};
        const T *start = fullname.c_str();
        const T *endp = fullname.c_str() + fullname.Length() - 1;
        const T *p = nullptr;

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

    template<typename T>
    inline bool SplitPath(String<T> &out_path,String<T> &out_filename,const String<T> &fullname)
    {
        if(fullname.IsEmpty()) return false;

        const T sep_chars[] = { '/','\\' };
        const int pos = fullname.FindRightChars(sep_chars);
        if(pos==-1) return false;

        out_path.Strcpy(fullname,pos);
        out_filename.Strcpy(fullname.c_str()+pos+1);
        return true;
    }

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
    OSString NormalizeFilename(const OSString &filename);
}//namespace hgl::filesystem
