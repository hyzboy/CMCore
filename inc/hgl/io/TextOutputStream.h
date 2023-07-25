﻿#ifndef HGL_IO_TEXT_OUTPUT_STREAM_INCLUDE
#define HGL_IO_TEXT_OUTPUT_STREAM_INCLUDE

#include<hgl/Endian.h>
#include<hgl/type/StringList.h>
namespace hgl
{
    namespace io
    {
        /**
        * 文本输出流<br>
        * 用于将数据输出成文件，用于日志、配置等文本信息处理。
        */
        class TextOutputStream
        {
        protected:

            ByteOrderMask bom;                                                                      ///<文本编码
            DataOutputStream *out;                                                                  ///<数据输出流

        public:

            TextOutputStream(ByteOrderMask _bom,DataOutputStream *_out)
            {
                bom=_bom;
                out=_out;
            }

            virtual ~TextOutputStream()
            {
                delete out;
            }

            const ByteOrderMask GetBOM()const
            {
                return bom;
            }

            DataOutputStream *GetDataOutputStream()
            {
                return out;
            }

            operator DataOutputStream *()
            {
                return out;
            }

            bool WriteBOM()                                                                         ///<写入文本文件bom头
            {
                if(!out)return(false);

                const BOMFileHeader *bom_item=BOMData+int(bom)-int(ByteOrderMask::BEGIN_RANGE);

                return(out->Write(bom_item->data,bom_item->size)==bom_item->size);
            }

            virtual bool WriteChars(const u8char *,int64)=0;                                        ///<写入一个字符串
            virtual bool WriteChars(const u16char *,int64)=0;                                       ///<写入一个字符串

            template<typename N>
            bool WriteString(const String<N> &str)                                                  ///<写入一个字符串
            {
                return WriteChars(str.c_str(),str.Length());
            }

            bool WriteLineEnd()                                                                     ///<写入一个换行符
            {
                return WriteChars(HGL_LINE_END,HGL_LINE_END_SIZE);
            }

            template<typename N>
            bool WriteLine(const N *str,int64 size)
            {
                if(!WriteChars(str,size))
                    return(false);

                return WriteLineEnd();
            }

            template<typename N>
            bool WriteLine(const N *str)
            {
                if(!str||!*str)return(false);

                return WriteLine(str,hgl::strlen(str));
            }

            template<typename N>
            bool WriteLine(const String<N> &str)
            {
                return WriteLine(str.c_str(),str.Length());
            }

            /**
             * 向流中写入一个文本(注：整体按标准文本文件模式)
             */
            template<typename N>
            bool WriteText(const StringList<N> &sl)
            {
                const int count=sl.GetCount();

                for(int i=0;i<count;i++)
                    if(!WriteLine(sl[i]))
                        return(false);

                return(true);
            }

            /**
            * 向流中写入一个字符串列表(注：整体为2进制流，不可用于文本文件)
            * @return 是否成功
            */
            template<typename T>
            bool Write(const StringList<T> &sl)
            {
                const int count=sl.GetCount();

                if(!out->WriteInt32(count))                 //写入行数
                    return(false);

                if(count<=0)
                    return(true);

                SharedArray<int32> line_length=new int32[count];
                SharedArray<T *> str_list=new T *[count];
                int32 *line_p=line_length;
                T **str_p=str_list;

                int32 total=0;

                for(int i=0;i<count;i++)
                {
                    const T &str=sl.GetString(i);

                    const int len=str.Length();

                    total+=len;

                    *line_p++=len;

                    *str_list++=str.c_str();
                }

                if(out->WriteInt32(line_length,count)!=count)   //写入每行长度
                    return(false);

                line_p=line_length;
                str_p=str_list;

                SharedArray<T> text=new T[total];
                T *text_p=text;

                for(int i=0;i<count;i++)
                {
                    const int len=*line_p++;

                    memcpy(text_p,*str_p++,len*sizeof(T));

                    text_p+=len;
                }

                return WriteChars(text,total);
            }
        };//class TextOutputStream

        template<ByteOrderMask BOM> class EndianTextOutputStream:public TextOutputStream    ///文本输出流
        {
        public:

            EndianTextOutputStream(OutputStream *);

            bool WriteChars(const u8char *,int64);                                          ///<写入一个字符串
            bool WriteChars(const u16char *,int64);                                         ///<写入一个字符串
        };//template<ByteOrderMask BOM> class EndianTextOutputStream

        typedef EndianTextOutputStream<ByteOrderMask::UTF8   > UTF8TextOutputStream;
        typedef EndianTextOutputStream<ByteOrderMask::UTF16LE> UTF16LETextOutputStream;
        typedef EndianTextOutputStream<ByteOrderMask::UTF16BE> UTF16BETextOutputStream;

        template<typename T> TextOutputStream *CreateTextOutputStream(OutputStream *os);
    }//namespace io
}//namespace hgl
#endif//HGL_IO_TEXT_OUTPUT_STREAM_INCLUDE
