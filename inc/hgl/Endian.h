#ifndef HGL_ENDIAN_INCLUDE
#define HGL_ENDIAN_INCLUDE

#include<hgl/platform/Platform.h>               // 平台定义
#include<hgl/TypeFunc.h>
namespace hgl
{
    namespace endian
    {
        /**
        * Windows代码页枚举
        * 全部Windows所支持代码页请参见 http://msdn.microsoft.com/en-us/library/dd317756
        *                            https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
        */
        enum class CharCodePage                         ///代码页枚举
        {
            NONE=0,                                     ///<起始定义，无意义

            IBM437                  =437,               ///<OEM United States (dos)

            //中文
            GBK                     =936,               ///<中国GBK标准中文
            Big5                    =950,               ///<中国台湾Big5标准繁体中文
            GB2312                  =20936,             ///<中国GB2312标准简体中文
            GB18030                 =54936,             ///<中国GB18030-2000标准中文

            //日文
            ShiftJIS                =932,               ///<日文ShiftJIS
            EUC_JP                  =20932,             ///<日文JIS 0208-1990 and 0212-1990
            ISO2022JP               =50220,             ///<ISO 2022 Japanese with no halfwidth Katakana; Japanese (JIS)
            csISO2022JP             =50221,             ///<ISO 2022 Japanese with halfwidth Katakana; Japanese (JIS-Allow 1 byte Kana)
            JISX                    =50222,             ///<日文JIS X/ISO 2022

            //韩文
            Korean                  =949,               ///<韩文

            //苹果编码
            MacJanpan               =10001,             ///<日文
            MacTraditionalChinese   =10002,             ///<繁体中文
            MacSimplifiedChinese    =10008,             ///<简体中文

            //ISO
            ISO_8859_1              =28591,             ///<ISO 8859-1 Latin 1; Western European                    西欧语系(阿尔巴尼亚语,西班牙加泰罗尼亚语,丹麦语,荷兰语,英语,Faeroese语,芬兰语,法语,德语,加里西亚语,爱尔兰语,冰岛语,意大利语,挪威语,葡萄牙语,瑞士语.)这同时适用于美国英语. 
            ISO_8859_2              =28592,             ///<ISO 8859-2 Central European; Central European (ISO)     斯拉夫/中欧语系(捷克语,德语,匈牙利语,波兰语,罗马尼亚语,克罗地亚语,斯洛伐克语,斯洛文尼亚语) 
            ISO_8859_3              =28593,             ///<ISO 8859-3 Latin 3                                      世界语,加里西亚语,马耳他语,土耳其语
            ISO_8859_4              =28594,             ///<ISO 8859-4 Baltic                                       爱莎尼亚语,拉脱维亚语,立陶宛语
            ISO_8859_5              =28595,             ///<ISO 8859-5 Cyrillic                                     斯拉夫语系(保加利亚语,Byelorussian语,马其顿语,俄语,塞尔维亚语,乌克兰语)
            ISO_8859_6              =28596,             ///<ISO 8859-6 Arabic                                       阿拉伯语
            ISO_8859_7              =28597,             ///<ISO 8859-7 Greek                                        现代希腊语
            ISO_8859_8              =28598,             ///<ISO 8859-8 Hebrew; Hebrew (ISO-Visual)                  希伯来语
            ISO_8859_9              =28599,             ///<ISO 8859-9 Turkish                                      Latin 5 字符集, (去掉了 Latin 1中不经常使用的一些冰岛语字符而代以土耳其语字符) 
            ISO_8859_13             =28603,             ///<ISO 8859-13 Estonian
            ISO_8859_15             =28605,             ///<ISO 8859-15 Latin 9                                     Latin 9 字符集, 是Latin 1字符集的更新版本,去掉一些不常用的字符,增加了对爱莎尼亚语的支持,修正了法语和芬兰语部份,增加了欧元字符) 

            //unicode
            UTF7                    =65000,             ///<utf-7
            UTF8                    =65001,             ///<utf-8

            UTF16LE                 =1200,
            UTF16BE                 =1201,
            UTF32LE                 =12000,
            UTF32BE                 =12001,

            END                                         ///<结束定义，无意义
        };//enum CharCodePage

        /**
         * 字节序类型枚举
         */
        enum class ByteOrderMask
        {
            NONE=0,
            UTF8,
            UTF16LE,
            UTF16BE,
            UTF32LE,
            UTF32BE,

            ENUM_CLASS_RANGE(UTF8,UTF32BE)
        };

        constexpr uint CHAR_SET_NAME_MAX_LENGTH=32;                                                 ///<字符集名称长度
        using CharSetName=char[CHAR_SET_NAME_MAX_LENGTH];                                           ///<字符集名称类型定义

        template<int,char> const CharSetName &GetCurCharSet();                                      ///<取得当前程序编码字符集

        constexpr CharSetName utf8_charset="utf8";
        constexpr CharSetName utf16le_charset="utf-16le";
        constexpr CharSetName utf16be_charset="utf-16be";
        constexpr CharSetName utf32le_charset="utf-32le";
        constexpr CharSetName utf32be_charset="utf-32be";

        template<> inline const CharSetName &GetCurCharSet<2,HGL_LITTLE_ENDIAN    >(){return utf16le_charset;}
        template<> inline const CharSetName &GetCurCharSet<2,HGL_BIG_ENDIAN       >(){return utf16be_charset;}
        template<> inline const CharSetName &GetCurCharSet<4,HGL_LITTLE_ENDIAN    >(){return utf32le_charset;}
        template<> inline const CharSetName &GetCurCharSet<4,HGL_BIG_ENDIAN       >(){return utf32be_charset;}

        template<typename T> inline const CharSetName &GetCharSet()
        {
            return GetCurCharSet<sizeof(T),HGL_ENDIAN>();
        }

        /**
         * 字节序文件头数据结构
         */
        struct BOMFileHeader
        {
            int size;                   ///<字节序文件头长度
            uint8 data[4];              ///<字节序数据

            ByteOrderMask bom;          ///<字节序枚举
            const CharSetName *char_set;///<字符集名称
            uint16 code_page;           ///<代码页
        };

        /**
         * 字节序文件头定义
         */
        constexpr BOMFileHeader BOMData[size_t(ByteOrderMask::RANGE_SIZE)]=
        {
            {3,{0xEF,0xBB,0xBF}     ,ByteOrderMask::UTF8,   &utf8_charset      ,(uint16)CharCodePage::UTF8     },
            {2,{0xFF,0xFE}          ,ByteOrderMask::UTF16LE,&utf16le_charset   ,(uint16)CharCodePage::UTF16LE  },
            {2,{0xFE,0xFF}          ,ByteOrderMask::UTF16BE,&utf16be_charset   ,(uint16)CharCodePage::UTF16BE  },
            {4,{0xFF,0xFE,0x00,0x00},ByteOrderMask::UTF32LE,&utf32le_charset   ,(uint16)CharCodePage::UTF32LE  },
            {4,{0x00,0x00,0xFE,0xFF},ByteOrderMask::UTF32BE,&utf32be_charset   ,(uint16)CharCodePage::UTF32BE  }
        };

        inline ByteOrderMask CheckBOM(const void *data)
        {
            const BOMFileHeader *bom=BOMData;

            for(int i=int(ByteOrderMask::BEGIN_RANGE);i<int(ByteOrderMask::END_RANGE);i++)
            {
                if(memcmp(data,bom->data,bom->size)==0)
                    return (ByteOrderMask)i;
            }

            return ByteOrderMask::NONE;
        }

        template<typename T>
        inline T EndianSwap(const T value)
        {
            union
            {
                T v;
                uint8 bytes[sizeof(T)];
            }a,b;

            a.v=value;

            for(uint i=0;i<sizeof(T);i++)
                b.bytes[i]=a.bytes[sizeof(T)-1-i];

            return b.v;
        }

        template<> inline  int8 EndianSwap< int8>(const  int8 value){return value;}
        template<> inline uint8 EndianSwap<uint8>(const uint8 value){return value;}

        template<> inline uint16 EndianSwap(const uint16 value)
        {
            return ((value&0xFF)<<8)
                |((value&0xFF00)>>8);
        }

        template<> inline uint32 EndianSwap(const uint32 value)
        {
            return ((value&0xFF)<<24)
                |((value&0xFF00)<<8)
                |((value&0xFF0000)>>8)
                |((value&0xFF000000)>>24);
        }

        template<> inline uint64 EndianSwap(const uint64 value)
        {
            return ((value&0xFF)<<56)
                |((value&0xFF00)<<40)
                |((value&0xFF0000)<<24)
                |((value&0xFF000000)<<8)
                |((value&0xFF00000000)>>8)
                |((value&0xFF0000000000)>>24)
                |((value&0xFF000000000000)>>40)
                |((value&0xFF00000000000000)>>56);
        }

        template<> inline u16char EndianSwap(const u16char value){return EndianSwap(uint16(value));}

        template<typename T>
        inline void EndianSwap(T *value,const int64 count)
        {
            for(int64 i=0;i<count;i++)
            {
                *value=EndianSwap(*value);
                ++value;
            }
        }

        template<typename T>
        inline void EndianSwap(T *dst,const T *src,const int64 count)
        {
            for(uint i=0;i<count;i++)
            {
                *dst=EndianSwap(*src);
                ++dst;
                ++src;
            }
        }

        template<typename D,typename S>
        inline void EndianSwap(D *dst,const S *src,const int64 count)
        {
            for(uint i=0;i<count;i++)
            {
                *dst=EndianSwap(D(*src));       //必须在ENDIAN SWAP前转换类型，否则在32转16位时，会将0000转出来。
                ++dst;
                ++src;
            }
        }

        #if HGL_ENDIAN == HGL_BIG_ENDIAN

            constexpr uint HGL_BOM_UTF16LE  =0xfffe;
            constexpr uint HGL_BOM_UTF16BE  =0xfeff;
            constexpr uint HGL_BOM_UTF32LE  =0xfffe0000;
            constexpr uint HGL_BOM_UTF32BE  =0x0000feff;

            #define LittleToCurrentEndian   EndianSwap
            #define BigToCurrentEndian      ToBigEndian

            template<typename T> T ToBigEndian(T value){return value;}

            template<typename T> inline void ToBigEndian(T *value,const int64 count){}
            template<typename D,typename S> inline void ToBigEndian(D *dst,const S *src,const int64 count){hgl_cpy(dst,src,count);}

            template<typename T> T ToLittleEndian(T value){return EndianSwap(value);}

            template<typename T> inline void ToLittleEndian(T *value,const int64 count){EndianSwap<T>(value,count);}
            template<typename D,typename S> inline void ToLittleEndian(D *dst,const S *src,const int64 count){EndianSwap<D,S>(dst,src,count);}

        #else

            constexpr uint HGL_BOM_UTF16LE  =0xfeff;
            constexpr uint HGL_BOM_UTF16BE  =0xfffe;
            constexpr uint HGL_BOM_UTF32LE  =0x0000feff;
            constexpr uint HGL_BOM_UTF32BE  =0xfffe0000;

            #define LittleToCurrentEndian   ToLittleEndian
            #define BigToCurrentEndian      EndianSwap

            template<typename T> T ToBigEndian(T value){return EndianSwap(value);}

            template<typename T> inline void ToBigEndian(T *value,const int64 count){EndianSwap<T>(value,count);}
            template<typename D,typename S> inline void ToBigEndian(D *dst,const S *src,const int64 count){EndianSwap<D,S>(dst,src,count);}

            template<typename T> T ToLittleEndian(T value){return value;}

            template<typename T> inline void ToLittleEndian(T *,const int64){}
            template<typename D,typename S> inline void ToLittleEndian(D *dst,const S *src,const int64 count){typeconv(dst,src,count);}

        #endif//HGL_BIG_ENDIAN

        template<char> struct UTF16CharConvert;

        template<> struct UTF16CharConvert<HGL_LITTLE_ENDIAN>
        {
        #if HGL_ENDIAN == HGL_BIG_ENDIAN
            static void convert(u16char *str,const int length)
            {
                    EndianSwap<u16char>(str,length);
            }
        #else
            static void convert(const u16char *,const int){}
        #endif//HGL_ENDIAN == HGL_LITTLE_ENDIAN

            static void convert(u16char *out_str,const u16char *in_str,const int length)
            {
                #if HGL_ENDIAN == HGL_LITTLE_ENDIAN
                    memcpy(out_str,in_str,length*sizeof(u16char));
                #else
                    EndianSwap<u16char>(in_str,length);
                #endif//HGL_ENDIAN == HGL_LITTLE_ENDIAN
            }
        };//template<> struct UTF16CharConvert<HGL_LITTLE_ENDIAN>

        template<> struct UTF16CharConvert<HGL_BIG_ENDIAN>
        {
            static void convert(u16char *str,const int length)
            {
                #if HGL_ENDIAN == HGL_LITTLE_ENDIAN
                    EndianSwap<u16char>(str,length);
                #endif//HGL_ENDIAN == HGL_LITTLE_ENDIAN
            }

            static void convert(u16char *out_str,const u16char *in_str,const int length)
            {
                #if HGL_ENDIAN == HGL_LITTLE_ENDIAN
                    memcpy(out_str,in_str,length*sizeof(u16char));
                #else
                    EndianSwap<u16char>(in_str,length);
                #endif//HGL_ENDIAN == HGL_LITTLE_ENDIAN
            }
        };//template<> struct UTF16ToWideChar<HGL_BIG_ENDIAN>
    }//namespace endian

    using namespace endian;
}//namespace hgl
#endif//HGL_ENDIAN_INCLUDE
