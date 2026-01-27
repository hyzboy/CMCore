#include <hgl/filesystem/CollectFiles.h>
#include <hgl/filesystem/EnumFile.h>
#include <hgl/filesystem/Filename.h>
#include <hgl/log/Log.h>
#include <regex>

namespace hgl
{
    namespace filesystem
    {
        // 通配符转正则表达式转换器: * -> .* , ? -> .
        static os_string WildcardToRegex(const OSString &pat)
        {
            os_string os_pat = pat.ToStdString();
            os_string out;
            out.reserve(os_pat.size() * 2);

            for (os_char ch : os_pat)
            {
                switch (ch)
                {
                case OS_TEXT('*'): out += OS_TEXT(".*"); break;
                case OS_TEXT('?'): out += OS_TEXT('.'); break;
                case OS_TEXT('.'): out += OS_TEXT("."); break;
                case OS_TEXT('('): case OS_TEXT(')'):
                case OS_TEXT('['): case OS_TEXT(']'):
                case OS_TEXT('{'): case OS_TEXT('}'):
                case OS_TEXT('^'): case OS_TEXT('$'):
                case OS_TEXT('|'): case OS_TEXT('+'):
                case OS_TEXT('\\'):
                    out += OS_TEXT('\\');
                    out += ch;
                    break;
                default:
                    out += ch;
                    break;
                }
            }

            // 添加首尾锚点
            out.insert(out.begin(), OS_TEXT('^'));
            out.push_back(OS_TEXT('$'));
            return out;
        }

        class CollectEnum : public EnumFile
        {
            ValueArray<FileInfo> &out;
            os_regex re;
            bool match_all = false;

        public:
            CollectEnum(ValueArray<FileInfo> &o, const OSString &pattern, bool use_regex)
                : out(o)
            {
                if (pattern.IsEmpty())
                {
                    match_all = true;
                }
                else
                {
                    if (use_regex)
                        re = os_regex(pattern.ToStdString(), std::regex::ECMAScript | std::regex::optimize);
                    else
                        re = os_regex(WildcardToRegex(pattern), std::regex::ECMAScript | std::regex::optimize);
                }
            }

            void ProcFile(EnumFileConfig *, FileInfo &fi) override
            {
                if (match_all)
                {
                    out.Add(fi);
                    return;
                }

                if (std::regex_match(fi.name, re))
                    out.Add(fi);
            }
        };

        int CollectFiles(ValueArray<FileInfo> &out,
                         const OSString &folder,
                         const OSString &pattern,
                         const bool use_regex,
                         const bool sub_folder)
        {
            EnumFileConfig cfg(folder);
            cfg.proc_folder = false;
            cfg.proc_file = true;
            cfg.sub_folder = sub_folder;
        #if HGL_OS == HGL_OS_Windows
            // let OS wildcard filter assist if not regex
            if (!use_regex && !pattern.IsEmpty())
                cfg.find_name = pattern;
        #endif

            CollectEnum collector(out, pattern, use_regex);
            return collector.Enum(&cfg);
        }
    }
}
