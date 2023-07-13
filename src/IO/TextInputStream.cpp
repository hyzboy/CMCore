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
            callback=nullptr;
        }

        template<typename T> int TextInputStream::Parse(const T *p)
        {
            const T *sp=(const T *)p;
            const T *end=(const T *)(buffer+cur_buf_size);

            int line_count=0;

            while(p<end)
            {
                if(*p=='\n')
                {
                    callback->OnLine(sp,p-sp,true);
                    ++line_count;
                    ++p;
                    sp=p;
                }
                else
                if(*p=='\r')
                {
                    if(p[1]=='\n')
                    {
                        callback->OnLine(sp,p-sp,true);
                        p+=2;
                    }
                    else
                    {
                        callback->OnLine(sp,p-sp,true);
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
                callback->OnLine(sp,end-sp,false);
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
                return Parse<u16char>((u16char *)p);
            else
            if(bom==ByteOrderMask::UTF32LE||bom==ByteOrderMask::UTF32BE)
                return Parse<u32char>((u32char *)p);
            else
                return Parse<char>((char *)p);
        }

        int TextInputStream::Run(ParseCallback *pc)
        {
            if(!pc)return(-2);
            if(!input_stream)return(-1);

            callback=pc;

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
                    callback->OnReadError();
                    return(-1);
                }

                result=TextBlockParse();

                if(result<0)
                {
                    callback->OnReadError();
                    return(result);
                }

                line_count+=result;

                stream_pos+=cur_buf_size;
            }

            callback->OnEnd();

            return line_count;
        }
    }//namespace io
}//namespace hgl
