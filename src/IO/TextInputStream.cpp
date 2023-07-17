#include<hgl/io/TextInputStream.h>

namespace hgl
{
    namespace io
    {
        TextInputStream::TextInputStream(InputStream *i,const int buf_size)
        {
            input_stream=i;
            buffer_size=buf_size;
            buffer=new uint8[buffer_size];
            cur_buf_size=0;

            stream_pos=0;
            stream_size=input_stream->Available();

            bom=ByteOrderMask::NONE;
            default_bom=ByteOrderMask::UTF8;

            callback_u8=nullptr;
            callback_u16=nullptr;
            callback_u32=nullptr;
            event_callback=nullptr;
        }

        template<> void TextInputStream::SetParseCallback(ParseCallback<char> *pc){callback_u8=pc;}
        template<> void TextInputStream::SetParseCallback(ParseCallback<u16char> *pc){callback_u16=pc;}
        template<> void TextInputStream::SetParseCallback(ParseCallback<u32char> *pc){callback_u32=pc;}

        template<typename T> int TextInputStream::Parse(const T *p,ParseCallback<T> *pc)
        {
            const T *sp=(const T *)p;
            const T *end=(const T *)(buffer+cur_buf_size);

            int line_count=0;

            while(p<end)
            {
                if(*p=='\n')
                {
                    pc->OnLine(sp,p-sp,true);
                    ++line_count;
                    ++p;
                    sp=p;
                }
                else
                if(*p=='\r')
                {
                    if(p[1]=='\n')
                    {
                        pc->OnLine(sp,p-sp,true);
                        p+=2;
                    }
                    else
                    {
                        pc->OnLine(sp,p-sp,true);
                        ++p;
                    }

                    ++line_count;
                    sp=p;
                }
                else
                    ++p;
            }

            if(sp<end)
            {
                pc->OnLine(sp,end-sp,false);
                ++line_count;
            }

            return line_count;
        }

        int TextInputStream::TextBlockParse()
        {
            uint8 *p=buffer;

            if(stream_pos==0)       //最开始，那检测一下BOM头
            {
                if(cur_buf_size>=2)
                {
                    bom=CheckBOM(p);

                    const BOMFileHeader *bfh=GetBOM(bom);

                    if(bfh)
                    {
                        if(bfh->size==cur_buf_size)
                            return(0);

                        p+=bfh->size;
                    }
                    else
                    {
                        bom=default_bom;
                    }   
                }
            }

            if(bom==ByteOrderMask::UTF16LE||bom==ByteOrderMask::UTF16BE)
            {
                if(!callback_u16)return(-1);
                return Parse<u16char>((u16char *)p,callback_u16);
            }
            else
            if(bom==ByteOrderMask::UTF32LE||bom==ByteOrderMask::UTF32BE)
            {
                if(!callback_u32)return(-1);
                return Parse<u32char>((u32char *)p,callback_u32);
            }
            else
            {
                if(!callback_u8)return(-1);
                return Parse<char>((char *)p,callback_u8);
            }
        }

        int TextInputStream::Run()
        {
            if(!input_stream)return(-1);
            
            if(!callback_u8
             &&!callback_u16
             &&!callback_u32)return(-2);            

            int64 read_size;

            int result;
            int line_count=0;

            while(stream_pos<stream_size)
            {
                read_size=stream_size-stream_pos;

                if(read_size>buffer_size)
                    read_size=buffer_size;

                cur_buf_size=input_stream->Read(buffer,read_size);

                if(cur_buf_size!=read_size)
                {
                    if(event_callback)
                        event_callback->OnReadError();

                    return(-1);
                }

                result=TextBlockParse();

                if(result<0)
                {
                    if(event_callback)
                        event_callback->OnReadError();

                    return(result);
                }

                line_count+=result;

                stream_pos+=cur_buf_size;
            }

            if(event_callback)
                event_callback->OnEnd();

            return line_count;
        }
    }//namespace io
}//namespace hgl
