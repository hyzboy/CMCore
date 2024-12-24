#include<hgl/io/DataOutputStream.h>

namespace hgl
{
    namespace io    // write utf8 chars
    {
        template<> bool DataOutputStream::WriteUTF8Chars<u8char>        (const u8char *str,uint size)
        {
            if(!out)return(false);
            if(size==0)return(true);

            if(!str||!*str)return(false);

            return(WriteArrays(str,size)==size);
        }

        template<> bool DataOutputStream::WriteUTF8Chars<u16char>    (const u16char *str,uint size)
        {
            if(!out)return(false);
            if(size==0)return(true);

            if(!str||!*str)return(false);

            return WriteUTF8Chars(to_u8(str,size));
        }
    }

    namespace io    //write utf8 string
    {
        template<typename T> bool DataOutputStream::WriteU8StringWithLength(const u8char *str,const uint length)
        {
            if(!out)return(false);

            if(length&&!str)return(false);

            if(!WriteNumber<T>(length))
                return(false);

            if(!length)
                return(true);

            return WriteUTF8Chars(str,length);
        }

        template<typename T> bool DataOutputStream::WriteU8StringWithLength(const U16String &str)
        {
            if(!out)return(false);

            int utf8_count;
            u8char *utf8_str=u16_to_u8(str.c_str(),str.Length(),utf8_count);

            const bool result=WriteU8StringWithLength<T>(utf8_str,utf8_count);

            delete[] utf8_str;

            return result;
        }
    }

    namespace io    //write utf16 chars
    {
        template<char T>        //非特例化版本，用于需要swap的
        bool WriteUTF16Chars(DataOutputStream *dos,const u16char *str,uint count)
        {
            SharedArray<u16char> swap_str=new u16char[count];

            EndianSwap<u16char>(swap_str,str,count);

            return(dos->WriteArrays<u16char>(swap_str,count)==count);
        }

        template<>                //特例化版本，用于直接写入的
        bool WriteUTF16Chars<HGL_ENDIAN>(DataOutputStream *dos,const u16char *str,uint count)
        {
            return(dos->WriteArrays(str,count)==count);
        }
    }

    namespace io    //write utf16-le chars
    {
        template<> bool DataOutputStream::WriteUTF16LEChars<u8char>(const u8char *str,uint length)
        {
            if(length==0)return(true);
            if(!out||!str||!*str)return(false);

            U16String u16_str=to_u16(str);

            return WriteUTF16Chars<HGL_LITTLE_ENDIAN>(this,u16_str.c_str(),u16_str.Length());
        }

        template<> bool DataOutputStream::WriteUTF16LEChars<u16char>(const u16char *str,uint length)
        {
            if(length==0)return(true);
            if(!out||!str||!*str)return(false);

            return WriteUTF16Chars<HGL_LITTLE_ENDIAN>(this,str,length);
        }
    }

    namespace io    //write utf16-be chars
    {
        template<> bool DataOutputStream::WriteUTF16BEChars<u8char>(const u8char *str,uint length)
        {
            if(length==0)return(true);
            if(!out||!str||!*str)return(false);

            U16String u16_str=to_u16(str,length);

            return WriteUTF16Chars<HGL_BIG_ENDIAN>(this,u16_str.c_str(),u16_str.Length());
        }

        template<> bool DataOutputStream::WriteUTF16BEChars<u16char>(const u16char *str,uint length)
        {
            if(length==0)return(true);
            if(!out||!str||!*str)return(false);

            return WriteUTF16Chars<HGL_BIG_ENDIAN>(this,str,length);
        }
    }//namespace io

    namespace io    //write utf16 string
    {
        template<uchar C,typename T> bool DataOutputStream::WriteU16StringWithLength(const u16char *str,const uint len)
        {
            if(!out)return(false);
            if(len&&!str)return(false);

            if(!WriteNumber<T>(len))
                return(false);

            if(!len)return(true);

            return WriteUTF16Chars<C>(this,str,len);
        }
    }//namespace io

    namespace io
    {
        bool DataOutputStream::WriteU8String        (const u8char *str,uint length){return WriteU8StringWithLength<uint32>(str,         length);}                               ///<按utf16-le格式写入字符串(前置4字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteU8String        (const u8char *str            ){return WriteU8StringWithLength<uint32>(str,         hgl::strlen(str));}                     ///<按utf8格式写入字符串(前置4字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteU8String        (const U8String &str      ){return WriteU8StringWithLength<uint32>(str.c_str(), str.Length());}                         ///<按utf8格式写入字符串(前置4字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteU8String        (const U16String &str     ){return WriteU8StringWithLength<uint32>(str);}                                               ///<按utf8格式写入字符串(前置4字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LEString     (const u16char *str,uint len){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint32>(str,        len);}              ///<按utf16-le格式写入字符串(前置4字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BEString     (const u16char *str,uint len){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint32>(str,        len);}              ///<按utf16-be格式写入字符串(前置4字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LEString     (const U16String &str     ){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint32>(str.c_str(),str.Length());}     ///<按utf16-le格式写入字符串(前置4字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BEString     (const U16String &str     ){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint32>(str.c_str(),str.Length());}     ///<按utf16-be格式写入字符串(前置4字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LEString     (const u16char *str         ){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint32>(str,        hgl::strlen(str));} ///<按utf16-le格式写入字符串(前置4字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BEString     (const u16char *str         ){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint32>(str,        hgl::strlen(str));} ///<按utf16-be格式写入字符串(前置4字节字符串长度,再写入字符阵列)



        bool DataOutputStream::WriteUTF8ShortString   (const u8char *str,uint length){return WriteU8StringWithLength<uint16>(str,         length);}                               ///<按utf16-le格式写入字符串(前置2字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF8ShortString   (const u8char *str            ){return WriteU8StringWithLength<uint16>(str,         hgl::strlen(str));}                     ///<按utf8格式写入字符串(前置2字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF8ShortString   (const U8String &str      ){return WriteU8StringWithLength<uint16>(str.c_str(), str.Length());}                         ///<按utf8格式写入字符串(前置2字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF8ShortString   (const U16String &str     ){return WriteU8StringWithLength<uint16>(str);}                                               ///<按utf8格式写入字符串(前置2字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LEShortString(const u16char *str,uint len){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint16>(str,        len);}              ///<按utf16-le格式写入字符串(前置2字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BEShortString(const u16char *str,uint len){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint16>(str,        len);}              ///<按utf16-be格式写入字符串(前置2字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LEShortString(const U16String &str     ){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint16>(str.c_str(),str.Length());}     ///<按utf16-le格式写入字符串(前置2字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BEShortString(const U16String &str     ){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint16>(str.c_str(),str.Length());}     ///<按utf16-be格式写入字符串(前置2字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LEShortString(const u16char *str         ){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint16>(str,        hgl::strlen(str));} ///<按utf16-le格式写入字符串(前置2字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BEShortString(const u16char *str         ){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint16>(str,        hgl::strlen(str));} ///<按utf16-be格式写入字符串(前置2字节字符串长度,再写入字符阵列)



        bool DataOutputStream::WriteUTF8TinyString    (const u8char *str,uint length){return WriteU8StringWithLength<uint8>(str,          length);}                             ///<按utf8格式写入字符串(前置1字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF8TinyString    (const u8char *str            ){return WriteU8StringWithLength<uint8>(str,          hgl::strlen(str));}                   ///<按utf8格式写入字符串(前置1字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF8TinyString    (const U8String &str        ){return WriteU8StringWithLength<uint8>(str.c_str(),  str.Length());}                       ///<按utf8格式写入字符串(前置1字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF8TinyString    (const U16String &str       ){return WriteU8StringWithLength<uint8>(str);}                                              ///<按utf8格式写入字符串(前置1字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LETinyString (const u16char *str,uint len){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint8>(str,         len);}              ///<按utf16-le格式写入字符串(前置1字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BETinyString (const u16char *str,uint len){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint8>(str,         len);}              ///<按utf16-be格式写入字符串(前置1字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LETinyString (const U16String &str     ){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint8>(str.c_str(), str.Length());}     ///<按utf16-le格式写入字符串(前置1字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BETinyString (const U16String &str     ){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint8>(str.c_str(), str.Length());}     ///<按utf16-be格式写入字符串(前置1字节字符串长度,再写入字符阵列)

        bool DataOutputStream::WriteUTF16LETinyString (const u16char *str         ){return WriteU16StringWithLength<HGL_LITTLE_ENDIAN,  uint8>(str,         hgl::strlen(str));} ///<按utf16-le格式写入字符串(前置1字节字符串长度,再写入字符阵列)
        bool DataOutputStream::WriteUTF16BETinyString (const u16char *str         ){return WriteU16StringWithLength<HGL_BIG_ENDIAN,     uint8>(str,         hgl::strlen(str));} ///<按utf16-be格式写入字符串(前置1字节字符串长度,再写入字符阵列)
    }//namespace io
}//namespace hgl
