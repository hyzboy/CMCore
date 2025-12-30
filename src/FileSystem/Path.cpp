#include <hgl/filesystem/Path.h>

namespace hgl::filesystem
{
    Path &Path::Append(const OSString &part)
    {
        if (part.IsEmpty())
            return *this;

        full_path = JoinPathWithFilename(full_path, part);
        return *this;
    }
}
