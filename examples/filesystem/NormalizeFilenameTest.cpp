#include <hgl/filesystem/Filename.h>
#include <hgl/log/Log.h>
#include <hgl/log/Logger.h>

using namespace hgl;
using namespace hgl::filesystem;

int main()
{
    if (!logger::InitLogger(OS_TEXT("NormalizeFilenameTest")))
        return 2;

    const struct
    {
        const char *name;
        const os_char *input;
        const os_char *expected;
    } cases[] = {
        {"empty", OS_TEXT(""), OS_TEXT("")},
        {"filename", OS_TEXT("file.txt"), OS_TEXT("file.txt")},
        {"relative", OS_TEXT("folder/file.txt"), OS_TEXT("folder/file.txt")},
        {"backslashes", OS_TEXT("folder\\sub\\file.txt"), OS_TEXT("folder/sub/file.txt")},
        {"repeated", OS_TEXT("folder///sub\\\\file.txt"), OS_TEXT("folder/sub/file.txt")},
        {"mixed", OS_TEXT("folder/\\sub\\/file.txt"), OS_TEXT("folder/sub/file.txt")},
        {"mixed run", OS_TEXT("folder/\\//\\\\sub"), OS_TEXT("folder/sub")},
        {"trailing", OS_TEXT("folder/\\//"), OS_TEXT("folder/")},
        {"root", OS_TEXT("/"), OS_TEXT("/")},
        {"backslash root", OS_TEXT("\\"), OS_TEXT("/")},
        {"drive root", OS_TEXT("C:/"), OS_TEXT("C:/")},
        {"drive path", OS_TEXT("C:/folder\\//file.txt"), OS_TEXT("C:/folder/file.txt")},
        {"drive relative", OS_TEXT("C:folder/\\file.txt"), OS_TEXT("C:folder/file.txt")},
        {"dot segments", OS_TEXT("./folder/../file.txt"), OS_TEXT("./folder/../file.txt")},
        {"case and spaces", OS_TEXT("Folder Name/File.TXT"), OS_TEXT("Folder Name/File.TXT")},
#if HGL_OS == HGL_OS_Windows
        {"UNC", OS_TEXT("\\\\server\\share\\file.txt"), OS_TEXT("//server/share/file.txt")},
        {"UNC slashes", OS_TEXT("//server/share/file.txt"), OS_TEXT("//server/share/file.txt")},
        {"UNC mixed prefix", OS_TEXT("\\/server\\share/\\file.txt"), OS_TEXT("//server/share/file.txt")},
        {"UNC reverse mixed prefix", OS_TEXT("/\\server/share\\/file.txt"), OS_TEXT("//server/share/file.txt")},
        {"UNC repeated prefix", OS_TEXT("\\\\\\\\server\\\\share"), OS_TEXT("//server/share")},
        {"UNC trailing", OS_TEXT("\\\\server\\share\\\\"), OS_TEXT("//server/share/")},
        {"double root", OS_TEXT("\\\\"), OS_TEXT("//")},
        {"repeated root", OS_TEXT("////"), OS_TEXT("//")},
        {"extended drive", OS_TEXT("\\\\?\\C:\\folder\\file.txt"), OS_TEXT("//?/C:/folder/file.txt")},
        {"extended UNC", OS_TEXT("\\\\?\\UNC\\server\\share\\file.txt"), OS_TEXT("//?/UNC/server/share/file.txt")},
        {"device namespace", OS_TEXT("\\\\.\\pipe\\name"), OS_TEXT("//./pipe/name")},
#else
        {"double root", OS_TEXT("//"), OS_TEXT("/")},
        {"repeated root", OS_TEXT("////"), OS_TEXT("/")},
        {"repeated leading", OS_TEXT("///folder//file.txt"), OS_TEXT("/folder/file.txt")},
        {"mixed leading", OS_TEXT("/\\/folder"), OS_TEXT("/folder")},
#endif
    };

    int failures = 0;
    for (const auto &test : cases)
    {
        const OSString input(test.input);
        OSString expected(test.expected);
        expected.Replace('/', HGL_DIRECTORY_SEPARATOR);
        const OSString actual = NormalizeFilename(input);
        if (actual != expected || input != OSString(test.input))
        {
            GLogError(u8"NormalizeFilename failed: %s", test.name);
            ++failures;
        }
        if (NormalizeFilename(actual) != actual)
        {
            GLogError(u8"NormalizeFilename is not idempotent: %s", test.name);
            ++failures;
        }
    }
    if (failures == 0)
        GLogInfo(u8"NormalizeFilename regression cases passed");
    return failures == 0 ? 0 : 1;
}
