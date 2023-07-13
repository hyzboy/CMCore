#pragma once
#include<hgl/type/DataType.h>
#include<hgl/io/InputStream.h>

namespace hgl
{
    namespace io
    {
        /**
        * 文本输入流<br>
        * 它与TextOutputStream并无对应关注，主要作用是方便超大文本的读取与解晰。
        */
        class TextInputStream
        {
        private:

            InputStream *input_stream;                                                              ///<输入流

            uint8 *buffer;                                                                          ///<缓冲区
            int32 buffer_size;                                                                      ///<缓冲区大小
            int32 cur_buf_size;                                                                     ///<当前缓冲区大小

            int64 stream_pos,stream_size;                                                           ///<流当前位置/大小

            ByteOrderMask bom;                                                                      ///<BOM头

        private:

            template<typename T> int Parse(const T *);

            int TextBlockParse();                                                                   ///<文本块解析

        public:

            TextInputStream(InputStream *i,const int buf_size=HGL_SIZE_1MB);
            virtual ~TextInputStream()
            {
                SAFE_CLEAR_ARRAY(buffer);
            }

            virtual bool OnBOM(const ByteOrderMask &){return true;}                                  ///<读取到BOM头的回调函数

            /**
            * 读取到一行文本的回调函数(ansi/utf8)
            * @param text 读取到的文本内容
            * @param len 读取到的文本字长度
            * @param line_end 当前行是否结束
            */
            virtual bool OnLine(const char *text,const int len,const bool line_end){return true;}

            /**
            * 读取到一行文本的回调函数(utf16le/utf16be)
            * @param text 读取到的文本内容
            * @param len 读取到的文本字长度
            * @param line_end 当前行是否结束
            */
            virtual bool OnLine(const u16char *text,const int len,const bool line_end){return true;}

            /**
            * 读取到一行文本的回调函数(utf32le/utf32be)
            * @param text 读取到的文本内容
            * @param len 读取到的文本字长度
            * @param line_end 当前行是否结束
            */
            virtual bool OnLine(const u32char *text,const int len,const bool line_end){return true;}

            virtual bool OnEnd(){return true;}                                                      ///<读取结束的回调函数
            virtual bool OnReadError(){return true;}                                                ///<读取错误的回调函数
            virtual bool OnParseError(){return true;}                                               ///<解析错误的回调函数

            /**
            * 运行并解晰文本
            * @return 解析出的文本行数
            */
            virtual int Run();
        };//class TextInputStream
    }//namespace io
}//namespace hgl
