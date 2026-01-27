#pragma once

#include <hgl/filesystem/FileSystem.h>

namespace hgl
{
    namespace filesystem
    {
        /**
         * @brief 路径类，专门用于处理文件名、路径名之类的功能。使用 OSString 保存名称。
         */
        class Path
        {
            OSString full_path;

        public:

            Path() = default;
            Path(const OSString &p) : full_path(p) {}
            Path(const os_char *p) : full_path(p) {}
            Path(const Path &p) : full_path(p.full_path) {}
            Path(Path &&p) : full_path(std::move(p.full_path)) {}
            virtual ~Path() = default;

            Path &operator=(const Path &p) { full_path = p.full_path; return *this; }
            Path &operator=(Path &&p) { full_path = std::move(p.full_path); return *this; }
            Path &operator=(const OSString &p) { full_path = p; return *this; }
            Path &operator=(const os_char *p) { full_path = p; return *this; }

            bool operator==(const Path &p) const { return full_path == p.full_path; }
            bool operator!=(const Path &p) const { return full_path != p.full_path; }

            operator const OSString &() const { return full_path; }
            const OSString &ToOSString() const { return full_path; }
            const os_char *c_str() const { return full_path.c_str(); }

            void Clear() { full_path.Clear(); }
            bool IsEmpty() const { return full_path.IsEmpty(); }

            /**
             * 获取文件名部分（含扩展名）
             */
            OSString GetFilename() const { return hgl::filesystem::GetFilename(full_path); }

            /**
             * 获取扩展名（默认含点）
             */
            OSString GetExtension(bool include_dot = true) const { return hgl::filesystem::GetExtension(full_path, include_dot); }

            /**
             * 获取不带扩展名的文件名部分 (Stem)
             */
            OSString GetStem() const { return hgl::filesystem::GetStem(full_path); }

            /**
             * 获取父级路径
             */
            Path GetParent() const { return Path(hgl::filesystem::GetParentPath(full_path, false)); }

            /**
             * 获取路径的最后一个组件
             */
            OSString GetLastComponent() const { return hgl::filesystem::GetLastPathComponent(full_path); }

            /**
             * 规范化路径
             */
            Path &Normalize() { full_path = NormalizeFilename(full_path); return *this; }

            /**
             * 追加路径片段
             */
            Path &Append(const OSString &part);

            Path &operator/=(const OSString &part) { return Append(part); }
            Path &operator/=(const Path &part) { return Append(part.full_path); }

            friend Path operator/(const Path &lhs, const OSString &rhs) { Path p(lhs); p.Append(rhs); return p; }
            friend Path operator/(const Path &lhs, const Path &rhs) { Path p(lhs); p.Append(rhs.full_path); return p; }

            /**
             * 替换扩展名
             */
            Path &ReplaceExtension(const OSString &new_ext)
            {
                full_path = hgl::filesystem::ReplaceExtension(full_path, new_ext);
                return *this;
            }

            // --- 文件系统功能 ---
            bool Exists() const { return FileExist(full_path); }
            bool IsDirectory() const { return hgl::filesystem::IsDirectory(full_path); }
            bool CreateDirectory() const { return MakePath(full_path); }
        };
    }
}
