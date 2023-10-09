#pragma once
#include<hgl/type/DataType.h>
#include<hgl/type/String.h>
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
        public:

            template<typename T>
            struct ParseCallback
            {
            protected:

                String<T> tmp;

            public:

                virtual bool OnBOM(const ByteOrderMask &){return true;}                                  ///<读取到BOM头的回调函数

                virtual bool OnLine(const T *text,const int len){return true;}

                /**
                * 读取到一行文本的回调函数
                * @param text 读取到的文本内容
                * @param len 读取到的文本字长度
                * @param line_end 当前行是否结束
                */
                virtual bool OnLine(const T *text,const int len,const bool line_end)
                {
                    if(!line_end)
                    {
                        tmp.Strcat(text,len);
                        return true;
                    }
                    else
                    {
                        if(tmp.IsEmpty())
                            return OnLine(text,len);

                        tmp.Strcat(text,len);

                        const bool result=OnLine(tmp.c_str(),tmp.Length());

                        tmp.Clear();
                        return(result);
                    }
                }
            };

            struct EventCallback
            {
                virtual bool OnEnd(){return true;}                                                      ///<读取结束的回调函数
                virtual bool OnReadError(){return true;}                                                ///<读取错误的回调函数
                virtual bool OnParseError(){return true;}                                               ///<解析错误的回调函数
            };

        private:

            InputStream *input_stream;                                                              ///<输入流

            char *buffer;                                                                           ///<缓冲区
            int32 buffer_size;                                                                      ///<缓冲区大小
            int32 cur_buf_size;                                                                     ///<当前缓冲区大小

            int64 stream_pos,stream_size;                                                           ///<流当前位置/大小

            ByteOrderMask bom;                                                                      ///<BOM头

            ByteOrderMask default_bom;                                                              ///<缺省BOM，在没有BOM头时使用

            ParseCallback<char> *callback_u8;                                                       ///<回调函数
            ParseCallback<u16char> *callback_u16;                                                   ///<回调函数
            ParseCallback<u32char> *callback_u32;                                                   ///<回调函数

            EventCallback *event_callback;                                                          ///<事件回调函数

        private:

            template<typename T> int Parse(const T *,ParseCallback<T> *);

            int TextBlockParse();                                                                   ///<文本块解析

        public:

            TextInputStream(InputStream *i,const int buf_size=HGL_SIZE_1MB);
            TextInputStream(char *buf,const int buf_size);
            virtual ~TextInputStream()
            {
                if(input_stream)    //有input_stream证明是从流加载的，需要删除临时缓冲区
                {
                    SAFE_CLEAR_ARRAY(buffer);
                }                
            }

            void SetDefaultBOM(const ByteOrderMask &bo){default_bom=bo;}                            ///<设置缺省BOM头}

            template<typename T> void SetParseCallback(ParseCallback<T> *pc);                       ///<设置回调函数

            void SetEventCallback(EventCallback *ec){event_callback=ec;}

            /**
            * 运行并解晰文本
            * @param pc 解晰结果回调函数
            * @return 解析出的文本行数
            */
            virtual int Run();
        };//class TextInputStream
    }//namespace io
}//namespace hgl
