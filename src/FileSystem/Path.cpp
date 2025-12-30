#include <hgl/filesystem/Path.h>

namespace hgl::filesystem
{
    Path &Path::Append(const OSString &part)
    {
        if (part.IsEmpty())
            return *this;

#if HGL_OS == HGL_OS_Windows
        full_path = JoinPathWithFilename<os_char>(full_path, part, HGL_DIRECTORY_SEPARATOR, HGL_DIRECTORY_SEPARATOR_STR);
#else
        full_path = JoinPathWithFilename<os_char>(full_path, part, HGL_DIRECTORY_SEPARATOR, HGL_DIRECTORY_SEPARATOR_U8STR);
#endif
        return *this;
    }
}