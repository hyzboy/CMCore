// HashFileTest —— 演示「算法列表来自 CMHash、文件哈希走 CMCore」的最小程序。
//
// 用法：
//   HashFileTest --list
//   HashFileTest <算法名> <文件>
//
// 注意这里没有任何算法名硬编码：算法清单与名称都来自 CMHash 的
// Clang AST 派生注册表，清单工具（AssetManifestTool）走的是同一套。

#include<hgl/log/Log.h>
#include<hgl/log/Logger.h>

#include<cstring>
#include<hgl/util/hash/Hash.h>
#include<hgl/util/hash/HashFile.h>
#include<hgl/utf.h>

using namespace hgl;
using namespace hgl::util::hash;

namespace
{
    void PrintAlgorithmList()
    {
        GLogInfo(u8"支持的哈希算法（共 %zu 种）：", GetAlgorithmCount());

        for(size_t i = 0; i < GetAlgorithmCount(); ++i)
        {
            Algorithm algo;

            if(!GetAlgorithmAt(i, algo))
                continue;

            GLogInfo(u8"  %-12s %2zu 字节%s",
                     GetName(algo),
                     GetDigestSize(algo),
                     IsStreamable(algo) ? u8"" : u8"（不支持分块，会整块读入）");
        }
    }
}//namespace

int main(int argc, char **argv)
{
    logger::InitLogger(OS_TEXT("HashFileTest"));

    if(argc < 2)
    {
        GLogInfo(u8"用法：HashFileTest --list");
        GLogInfo(u8"      HashFileTest <算法名> <文件>");
        PrintAlgorithmList();
        return 0;
    }

    if(strcmp(argv[1], "--list") == 0)
    {
        PrintAlgorithmList();
        return 0;
    }

    if(argc < 3)
    {
        GLogError(u8"缺少文件名");
        return 2;
    }

    Algorithm algo;

    if(!TryParseAlgorithm(argv[1], algo))
    {
        GLogError(u8"不认识的算法：%s", argv[1]);
        PrintAlgorithmList();
        return 2;
    }

    const size_t digest_size = GetDigestSize(algo);

    char hex[129];

    if(digest_size * 2 + 1 > sizeof(hex))
    {
        GLogError(u8"摘要过长");
        return 2;
    }

    const OSString filename = ToOSString(argv[2]);

    if(!HashFileHex(algo, filename, hex, sizeof(hex)))
    {
        GLogError(u8"无法计算文件哈希：%s", argv[2]);
        return 1;
    }

    GLogInfo(u8"文件：%s", argv[2]);
    GLogInfo(u8"算法：%s", GetName(algo));
    GLogInfo(u8"摘要：%s", hex);

    return 0;
}
