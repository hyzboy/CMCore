#pragma once

#include <hgl/type/ValueArray.h>
#include <hgl/filesystem/FileSystem.h>

namespace hgl::filesystem
{
    /**
    * CN: 在指定文件夹中收集符合模式（通配符或正则表达式）的文件。
    * @pragma pattern      模式，可以是通配符（例如 "*.txt"）或完整的正则表达式，具体取决于标志设置
    * @pragma use_regex    如果为 true，则将 pattern 视为 std::regex；否则视为通配符
    * @pragma sub_folder   是否遍历子文件夹
    * @return              返回收集到的文件数量。
    *
    * EN: Collect files in a folder matching pattern (wildcard or regex).
    * @pragma pattern      wildcard like "*.txt" or full regex depending on flag
    * @pragma use_regex    when true, treat pattern as std::regex; otherwise wildcard
    * @pragma sub_folder   whether to traverse subfolders
    * Returns              number of files collected.
    */
    int CollectFiles(ValueArray<FileInfo> &out,
                        const OSString &folder,
                        const OSString &pattern,
                        const bool use_regex = false,
                        const bool sub_folder = false);
}
