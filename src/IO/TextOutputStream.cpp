#include<hgl/io/TextOutputStream.h>
#include<hgl/io/DataOutputStream.h>
namespace hgl
{
    namespace io
    {
        template<> EndianTextOutputStream<ByteOrderMask::UTF8>::EndianTextOutputStream(OutputStream *os):TextOutputStream(ByteOrderMask::UTF8,new DirectDataOutputStream(os)){}
        template<> EndianTextOutputStream<ByteOrderMask::UTF16LE>::EndianTextOutputStream(OutputStream *os):TextOutputStream(ByteOrderMask::UTF16LE,new LEDataOutputStream(os)){}
        template<> EndianTextOutputStream<ByteOrderMask::UTF16BE>::EndianTextOutputStream(OutputStream *os):TextOutputStream(ByteOrderMask::UTF16BE,new BEDataOutputStream(os)){}

        template<> TextOutputStream *CreateTextOutputStream<u8char>(OutputStream *os){return(new UTF8TextOutputStream(os));}
        template<> TextOutputStream *CreateTextOutputStream<wchar_t>(OutputStream *os){return(new UTF16LETextOutputStream(os));}

#ifdef HGL_SUPPORT_CHAR8_T
        // When char8_t is supported, u8char is char8_t, so we need a separate specialization for char
        template<> TextOutputStream *CreateTextOutputStream<char>(OutputStream *os){return(new UTF8TextOutputStream(os));}
#endif
    }//namespace io

    namespace io        //WriteChars函数
    {
        template<> bool EndianTextOutputStream<ByteOrderMask::UTF8>::WriteChars(const u8char *str,int64 size)
        {
            return out?out->WriteUTF8Chars(str,size):false;
        }

        template<> bool EndianTextOutputStream<ByteOrderMask::UTF8>::WriteChars(const u16char *str,int64 size)
        {
            return out?out->WriteUTF8Chars(str,size):false;
        }

        template<> bool EndianTextOutputStream<ByteOrderMask::UTF16LE>::WriteChars(const u8char *str,int64 size)
        {
            return out?out->WriteUTF16LEChars(str,size):false;
        }

        template<> bool EndianTextOutputStream<ByteOrderMask::UTF16LE>::WriteChars(const u16char *str,int64 size)
        {
            return out?out->WriteUTF16LEChars(str,size):false;
        }

        template<> bool EndianTextOutputStream<ByteOrderMask::UTF16BE>::WriteChars(const u8char *str,int64 size)
        {
            return out?out->WriteUTF16BEChars(str,size):false;
        }

        template<> bool EndianTextOutputStream<ByteOrderMask::UTF16BE>::WriteChars(const u16char *str,int64 size)
        {
            return out?out->WriteUTF16BEChars(str,size):false;
        }
    }//namespace io
}//namespace hgl
