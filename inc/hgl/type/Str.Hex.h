#pragma once
#include <hgl/type/Str.Number.h>
#include <hgl/type/Str.Length.h>
#include <cstddef>
namespace hgl
{

    /**
     * @brief CN: 将十六进制字符串解析为原始字节数据（每两个字符表示一字节）。
     * @brief EN: Parse a hex string into raw bytes (two characters per byte).
     *
     * @param[out] dst CN: 输出字节缓冲区. EN: output byte buffer.
     * @param[in] src CN: 十六进制字符串. EN: hex source string.
     * @param[in] byte_count CN: 要解析的字节数（即 src 中字符数应为 byte_count*2）. EN: number of bytes to parse.
     */
    template<typename CharT>
    inline void ParseHexStr(uint8 *dst, const CharT *src, std::size_t byte_count)
    {
        for(std::size_t i = 0; i < byte_count; ++i)
        {
            *dst = static_cast<uint8>(hgl::parse_number_char<16,CharT>(*src) << 4);
            ++src;
            *dst |= static_cast<uint8>(hgl::parse_number_char<16,CharT>(*src));
            ++src;
            ++dst;
        }
    }

    /**
     * @brief CN: 将十六进制字符串解析为固定类型的数据。
     * @brief EN: Parse a hex string into a fixed-size data type.
     *
     * @tparam CharT 字符类型. EN: character type.
     * @tparam HC 目标类型. EN: target data type.
     * @param[out] out_value CN: 输出目标变量. EN: output target value.
     * @param[in] str CN: 十六进制字符串. EN: hex source string.
     */
    template<typename CharT,typename HC>
    inline void ParseHexStr(HC &out_value,const CharT *str)
    {
        hgl::ParseHexStr<CharT>(reinterpret_cast<uint8 *>(&out_value), str, sizeof(HC));
    }

    template<typename CharT,typename U>
    inline void Hex2String(CharT *str,U value,bool upper=true)
    {
        const CharT A = static_cast<CharT>(upper ? 'A' : 'a');
        const std::size_t size = sizeof(U) * 2;
        const uint8 *sp = reinterpret_cast<const uint8 *>(&value);
        CharT *tp = str;

        for(std::size_t i = 0; i < sizeof(U); ++i)
        {
            uint8 l = static_cast<uint8>(((sp[i]) & 0xF0) >> 4);
            uint8 r = static_cast<uint8>((sp[i]) & 0x0F);

            if(l < 10) *tp++ = static_cast<CharT>(l + '0');
            else *tp++ = static_cast<CharT>(l - 10 + A);

            if(r < 10) *tp++ = static_cast<CharT>(r + '0');
            else *tp++ = static_cast<CharT>(r - 10 + A);
        }

        *tp = 0;
    }

    /**
     * @brief CN: 将原始数据转换为十六进制字符串。
     * @brief EN: Convert raw bytes to a hex string.
     *
     * @param[out] str CN: 输出十六进制字符串缓冲区. EN: output hex string buffer.
     * @param[in] src CN: 原始字节数据. EN: source byte data.
     * @param[in] byte_count CN: 原始数据字节长度. EN: number of bytes in src.
     * @param[in] hex_chars CN: 用于映射的十六进制字符表（长度至少为16）. EN: array of hex character mappings (length >= 16).
     * @param[in] gap_char CN: 可选间隔字符（插入在每个字节之间），默认无. EN: optional gap char inserted between bytes.
     */
    template<typename CharT>
    inline void DataToHexStr(CharT *str, const uint8 *src, std::size_t byte_count, const char *hex_chars, const CharT gap_char=0)
    {
        for(std::size_t i = 0; i < byte_count; ++i)
        {
            if(i && gap_char)
            {
                *str++ = gap_char;
            }

            *str++ = static_cast<CharT>(hex_chars[(src[i] & 0xF0) >> 4]);
            *str++ = static_cast<CharT>(hex_chars[(src[i] & 0x0F)]);
        }

        *str = 0;
    }

    template<typename CharT> inline void DataToLowerHexStr(CharT *str,const uint8 *src,std::size_t byte_count,const CharT gap_char=0){hgl::DataToHexStr<CharT>(str,src,byte_count,LowerHexChar,gap_char);} 
    template<typename CharT> inline void DataToUpperHexStr(CharT *str,const uint8 *src,std::size_t byte_count,const CharT gap_char=0){hgl::DataToHexStr<CharT>(str,src,byte_count,UpperHexChar,gap_char);} 

    /**
     * @brief CN: 将固定类型的原始数据转换为十六进制字符串。
     * @brief EN: Convert a fixed-size data type to a hex string.
     *
     * @tparam CharT 字符类型. EN: character type.
     * @tparam HC 目标数据类型. EN: target data type.
     * @param[out] str CN: 输出十六进制字符串缓冲区. EN: output hex string buffer.
     * @param[in] hc CN: 源数据. EN: source data.
     * @param[in] hex_chars CN: 用于映射的十六进制字符表. EN: hex character table.
     * @param[in] gap_char CN: 可选间隔字符. EN: optional gap char.
     */
    template<typename CharT,typename HC>
    inline void DataToHexStr(CharT *str,const HC &hc,const CharT *hex_chars,const CharT gap_char=0)
    {
        hgl::DataToHexStr(str,reinterpret_cast<const uint8 *>(&hc),sizeof(hc),hex_chars,gap_char);
    }

    template<typename CharT,typename HC> inline void ToUpperHexStr(CharT *str,const HC &hc,const CharT gap_char=0){hgl::DataToHexStr<CharT,HC>(str,hc,UpperHexChar,gap_char);} 
    template<typename CharT,typename HC> inline void ToLowerHexStr(CharT *str,const HC &hc,const CharT gap_char=0){hgl::DataToHexStr<CharT,HC>(str,hc,LowerHexChar,gap_char);} 

    template<typename CharT> inline void ToUpperHexStr(CharT *str,const void *data,std::size_t byte_count,const CharT gap_char=0){hgl::DataToHexStr<CharT>(str,(const uint8 *)data,byte_count,UpperHexChar,gap_char);} 
    template<typename CharT> inline void ToLowerHexStr(CharT *str,const void *data,std::size_t byte_count,const CharT gap_char=0){hgl::DataToHexStr<CharT>(str,(const uint8 *)data,byte_count,LowerHexChar,gap_char);} 

}//namespace hgl
