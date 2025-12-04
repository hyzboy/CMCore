#pragma once

#include<hgl/platform/Platform.h>               // 平台定义
#include<hgl/type/EnumUtil.h>
namespace hgl
{
    namespace endian
    {
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

        constexpr uint CHAR_SET_NAME_MAX_LENGTH=64;                                                 ///<字符集名称长度
        using CharSetName=char[CHAR_SET_NAME_MAX_LENGTH];                                           ///<字符集名称类型定义

        template<int,char> const CharSetName &GetCurCharSet();                                      ///<取得当前程序编码字符集

        constexpr CharSetName utf8_charset   ="utf8";
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
        };

        /**
         * 字节序文件头定义
         */
        constexpr BOMFileHeader BOMData[size_t(ByteOrderMask::RANGE_SIZE)]=
        {
            {3,{0xEF,0xBB,0xBF}     ,ByteOrderMask::UTF8,   &utf8_charset      },
            {2,{0xFF,0xFE}          ,ByteOrderMask::UTF16LE,&utf16le_charset   },
            {2,{0xFE,0xFF}          ,ByteOrderMask::UTF16BE,&utf16be_charset   },
            {4,{0xFF,0xFE,0x00,0x00},ByteOrderMask::UTF32LE,&utf32le_charset   },
            {4,{0x00,0x00,0xFE,0xFF},ByteOrderMask::UTF32BE,&utf32be_charset   }
        };

        inline const BOMFileHeader *GetBOM(const ByteOrderMask &bom)
        {
            return RangeCheck(bom)?BOMData+uint(bom)-uint(ByteOrderMask::BEGIN_RANGE):nullptr;
        }

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
