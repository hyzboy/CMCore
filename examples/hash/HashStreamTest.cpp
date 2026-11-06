// HashStreamTest —— 演示 HashInputStream：边读文件边算哈希，不整块载入内存。
//
// 用法：
//   HashStreamTest <算法名> <文件>
//
// 这里用 CMHash 运行期多态的 Hasher 驱动 CMCore 的 HashInputStream，
// 因此算法可以在运行期决定；旧版模板写法要求为每种算法各实例化一个
// 模板类，且「复制 hasher 取摘要」与 Base 删除拷贝构造相冲突。

#include<hgl/log/Log.h>
#include<hgl/log/Logger.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/io/HashInputStream.h>
#include<hgl/util/hash/Hash.h>
#include<hgl/utf.h>

using namespace hgl;
using namespace hgl::util::hash;

int main(int argc, char **argv)
{
    logger::InitLogger(OS_TEXT("HashStreamTest"));

    if(argc < 3)
    {
        GLogInfo(u8"用法：HashStreamTest <算法名> <文件>");
        return 0;
    }

    Algorithm algo;

    if(!TryParseAlgorithm(argv[1], algo))
    {
        GLogError(u8"不认识的算法：%s", argv[1]);
        return 2;
    }

    if(!IsStreamable(algo))
    {
        GLogError(u8"%s 不支持分块计算，请改用 HashFile（它会整块读入）", GetName(algo));
        return 2;
    }

    io::FileInputStream source;
    const OSString filename = ToOSString(argv[2]);

    if(!source.Open(filename))
    {
        GLogError(u8"无法打开文件：%s", argv[2]);
        return 1;
    }

    io::HashInputStream stream(&source, algo);

    if(!stream.IsValid())
    {
        GLogError(u8"无法为 %s 创建分块散列器", GetName(algo));
        return 2;
    }

    uint8 buffer[65536];
    int64 total = 0;
    int64 read_size;

    while((read_size = stream.Read(buffer, sizeof(buffer))) > 0)
        total += read_size;

    if(read_size < 0)
    {
        GLogError(u8"读取失败：%s", argv[2]);
        return 1;
    }

    uint8 digest[64];
    char  hex[129];

    const size_t digest_size = stream.GetDigestSize();

    if(digest_size > sizeof(digest))
    {
        GLogError(u8"摘要过长");
        return 2;
    }

    stream.GetDigest(digest);
    DigestToLowerHex(digest, digest_size, hex);

    GLogInfo(u8"文件：%s", argv[2]);
    GLogInfo(u8"算法：%s", GetName(algo));
    GLogInfo(u8"大小：%d 字节", (int)total);
    GLogInfo(u8"摘要：%s", hex);

    return 0;
}
