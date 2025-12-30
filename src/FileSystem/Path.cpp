#include <hgl/filesystem/Path.h>
#include <hgl/filesystem/Filename.h>

namespace hgl
{
    namespace filesystem
    {
        Path &Path::Append(const OSString &part)
        {
            if (part.IsEmpty())
                return *this;

            full_path = JoinPathWithFilename(full_path, part);
            return *this;
        }
    }
}