#include<hgl/io/DataInputStream.h>
#include<hgl/Charset.h>

namespace hgl
{
    namespace io
    {
        template<char T>
        bool ReadUTF16Chars(u16char *str,DataInputStream *dis,uint count)
        {
            if(dis->ReadArrays<u16char>(str,count)!=count)
                return(false);

            EndianSwap<u16char>(str,count);
            str[count]=0;

            return(true);
        }

        template<>
        bool ReadUTF16Chars<HGL_ENDIAN>(u16char *str,DataInputStream *dis,uint count)
        {
            return(dis->ReadArrays<u16char>(str,count)==count);
        }

        bool DataInputStream::ReadUTF16LEChars(u16char *u16str,uint count)
        {
            if(!in||!u16str||!count)return(false);

            return ReadUTF16Chars<HGL_LITTLE_ENDIAN>(u16str,this,count);
        }

        bool DataInputStream::ReadUTF16BEChars(u16char *u16str,uint count)
        {
            if(!in||!u16str||!count)return(false);

            return ReadUTF16Chars<HGL_BIG_ENDIAN>(u16str,this,count);
        }

        template<typename T>
        bool ReadUTF8String(char *buf,uint max_len,DataInputStream *dis)
        {
            if(!buf||!max_len||!dis)
                return(false);

            T str_len;

            if(!dis->ReadNumber<T>(str_len))
                return(false);

            if(str_len<=0)
            {
                *buf=0;
                return(true);
            }

            if(max_len>=str_len)
            {
                if(dis->ReadArrays<char>(buf,str_len)!=str_len)
                    return(false);

                if(max_len>str_len)
                    buf[str_len]=0;

                return(true);
            }

            if(dis->ReadArrays<char>(buf,max_len)!=max_len)
                return(false);

            dis->Skip(str_len-max_len);

            return(true);
        }

        bool DataInputStream::ReadUTF8String        (char *u8str,uint max_len){return hgl::io::ReadUTF8String<uint32>(u8str,max_len,this);}
        bool DataInputStream::ReadUTF8ShortString   (char *u8str,uint max_len){return hgl::io::ReadUTF8String<uint16>(u8str,max_len,this);}
        bool DataInputStream::ReadUTF8TinyString    (char *u8str,uint max_len){return hgl::io::ReadUTF8String<uint8 >(u8str,max_len,this);}

        template<typename T>
        bool ReadUTF8String(U8String &u8str,uint max_len,DataInputStream *dis)
        {
            if(!max_len||!dis)
                return(false);

            T str_len;

            if(!dis->ReadNumber<T>(str_len))
                return(false);

            if(str_len==0)
            {
                u8str.Clear();
                return(true);
            }

            const uint count=(max_len>str_len?str_len:max_len);

            u8char *utf8_str=dis->ReadArrays<u8char>(count);

            if(!utf8_str)
                return(false);

            if(count<str_len)
                dis->Skip(str_len-count);

            // copy then free the buffer read from stream
            u8str.fromString(utf8_str, count);
            delete[] utf8_str;
            return(true);
        }

        bool DataInputStream::ReadUTF8String        (U8String &u8str,uint max_len){return hgl::io::ReadUTF8String<uint32>(u8str,max_len,this);}
        bool DataInputStream::ReadUTF8ShortString   (U8String &u8str,uint max_len){return hgl::io::ReadUTF8String<uint16>(u8str,max_len,this);}
        bool DataInputStream::ReadUTF8TinyString    (U8String &u8str,uint max_len){return hgl::io::ReadUTF8String<uint8 >(u8str,max_len,this);}

        template<typename T>
        bool ReadUTF8String(U16String &u16str,uint max_len,DataInputStream *dis)
        {
            if(!max_len||!dis)
                return(false);

            T str_len;

            if(!dis->ReadNumber<T>(str_len))
                return(false);

            if(str_len==0)
            {
                u16str.Clear();
                return(true);
            }

            const uint count=(max_len>str_len?str_len:max_len);

            u8char *utf8_str=dis->ReadArrays<u8char>(count);

            if(!utf8_str)
                return(false);

            if(count<str_len)
                dis->Skip(str_len-count);

            int wide_count;

            u16char *wide_str=u8_to_u16(utf8_str,count,wide_count);

            // copy then free the intermediate buffers
            u16str.fromString(wide_str, wide_count);

            delete[] wide_str;
            delete[] utf8_str;
            return(true);
        }

        bool DataInputStream::ReadUTF8String        (U16String &u16str,uint max_len){return hgl::io::ReadUTF8String<uint32>(u16str,max_len,this);}
        bool DataInputStream::ReadUTF8ShortString   (U16String &u16str,uint max_len){return hgl::io::ReadUTF8String<uint16>(u16str,max_len,this);}
        bool DataInputStream::ReadUTF8TinyString    (U16String &u16str,uint max_len){return hgl::io::ReadUTF8String<uint8 >(u16str,max_len,this);}

        template<char E,typename T>
        bool ReadU16String(u16char *str,uint max_len,DataInputStream *dis)
        {
            if(!dis)return(false);
            if(!str||!max_len)return(false);

            T count;

            if(!dis->ReadNumber<T>(count))
                return(false);

            if(count<=0)
            {
                *str=0;
                return(true);
            }

            if(max_len>=count)
            {
                if(dis->ReadArrays<u16char>(str,count)!=count)
                    return(false);

                if(max_len>count)
                    str[count]=0;
            }
            else
            {
                if(dis->ReadArrays<u16char>(str,max_len)!=max_len)
                    return(false);

                dis->Skip((count-max_len)*sizeof(u16char));

                count=max_len;
            }

            UTF16CharConvert<E>::convert(str,count);
            return(true);
        }

        bool DataInputStream::ReadUTF16LEString     (u16char *u16str,uint max_len){return ReadU16String<HGL_LITTLE_ENDIAN,uint32>(u16str,max_len,this);}
        bool DataInputStream::ReadUTF16LEShortString(u16char *u16str,uint max_len){return ReadU16String<HGL_LITTLE_ENDIAN,uint16>(u16str,max_len,this);}
        bool DataInputStream::ReadUTF16LETinyString (u16char *u16str,uint max_len){return ReadU16String<HGL_LITTLE_ENDIAN,uint8 >(u16str,max_len,this);}

        bool DataInputStream::ReadUTF16BEString     (u16char *u16str,uint max_len){return ReadU16String<HGL_BIG_ENDIAN,uint32>(u16str,max_len,this);}
        bool DataInputStream::ReadUTF16BEShortString(u16char *u16str,uint max_len){return ReadU16String<HGL_BIG_ENDIAN,uint16>(u16str,max_len,this);}
        bool DataInputStream::ReadUTF16BETinyString (u16char *u16str,uint max_len){return ReadU16String<HGL_BIG_ENDIAN,uint8 >(u16str,max_len,this);}

        template<char E,typename T>
        bool ReadU16String(U16String &str,DataInputStream *dis,uint max_len)
        {
            if(!dis||!max_len)return(false);

            T count;

            if(!dis->ReadNumber<T>(count))
                return(false);

            if(count<0)
                return(false);

            if(max_len>0)
            {
                if(count>max_len)
                    return(false);
            }

            if(count==0)
            {
                str.Clear();
                return(true);
            }

            u16char *utf16_str=dis->ReadArrays<u16char>(count);

            if(!utf16_str)
                return(false);

            UTF16CharConvert<E>::convert(utf16_str,count);

            // copy then free the buffer read from stream
            str.fromString(utf16_str, count);
            delete[] utf16_str;

            return(true);
        }

        bool DataInputStream::ReadUTF16LEString     (U16String &u16str,uint max_len){return ReadU16String<HGL_LITTLE_ENDIAN,uint32>(u16str,this,max_len);}
        bool DataInputStream::ReadUTF16LEShortString(U16String &u16str,uint max_len){return ReadU16String<HGL_LITTLE_ENDIAN,uint16>(u16str,this,max_len);}
        bool DataInputStream::ReadUTF16LETinyString (U16String &u16str,uint max_len){return ReadU16String<HGL_LITTLE_ENDIAN,uint8 >(u16str,this,max_len);}

        bool DataInputStream::ReadUTF16BEString     (U16String &u16str,uint max_len){return ReadU16String<HGL_BIG_ENDIAN,uint32>(u16str,this,max_len);}
        bool DataInputStream::ReadUTF16BEShortString(U16String &u16str,uint max_len){return ReadU16String<HGL_BIG_ENDIAN,uint16>(u16str,this,max_len);}
        bool DataInputStream::ReadUTF16BETinyString (U16String &u16str,uint max_len){return ReadU16String<HGL_BIG_ENDIAN,uint8 >(u16str,this,max_len);}
    }//namespace io
}//namespace hgl
